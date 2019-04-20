#!/usr/bin/env python2
from pymongo import MongoClient
from bson.objectid import ObjectId

import time,datetime
import os

import pandas as pd
import pandas_datareader.data as web
import numpy as np

from sklearn import preprocessing

from copy import deepcopy
from IPython.display import clear_output

# from __future__ import print_function

from mpl_toolkits.mplot3d import Axes3D
from sklearn.datasets.samples_generator import make_blobs
from pyspark import SparkContext
from pyspark.ml.clustering import KMeans
from pyspark.ml.feature import VectorAssembler
from pyspark.sql import SQLContext
import pyspark

from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("-u", "--user-arg", help="user argument", dest="user", default="james")
args = parser.parse_args()
# print("user arg:", args.user)
# print(type( args.user))

class spark():
	def __init__(self):
		self.df = pd.DataFrame()
		self.dfspark = pd.DataFrame()
	def pullData(self, name='james'):
		# pull data from database
		conn = MongoClient()
		db = conn.Tracker
		# read every collection
		collection = [db[name]]# db.db2, db.dn2, db.james, db.leo
		clean_data = []
		for col in collection:
			cursor = col.find({})
			self.df = pd.DataFrame(list(cursor))
		self.df.replace('', np.nan, inplace=True)
		self.df.fillna(method='ffill', inplace=True)
		if self.df.isnull().sum().sum() != 0:
			print('ErrordataFrameisNotAllNull')

		# turn into float
		self.df['hr_value'] = self.df['hr_value'].astype(float)
		self.df['o2_value'] = self.df['o2_value'].astype(float)
		self.df['latitude'] = self.df['latitude'].astype(float)
		self.df['longitude'] = self.df['longitude'].astype(float)
		self.df['step_value'] = self.df['step_value'].astype(float)

		# eliminate the zeros
		self.df = self.df[(self.df['hr_value'] != 0)]

	def extractData(self):
		self.dfspark = self.df.loc[:,['hr_value', 'step_value']] #  'o2_value'
		self.dfspark.to_csv('input.csv', index=True)

	def sparkRunning(self):
		sc = SparkContext(appName="pysparkKmeas") # exist by default
		sqlContext = SQLContext(sc)

		FEATURES_COL = ['hr_value', 'step_value']
		path = 'input.csv'

		# read dfspark 
		self.dfspark = sqlContext.read.csv(path, header=True)

		lines = sc.textFile(path)
		data = lines.map(lambda line: line.split(","))

		# turn to DF
		self.dfspark = data.toDF(['id','hr_value', 'step_value'])
		# convert to float
		df_feat = self.dfspark.select(*(self.dfspark[c].cast("float").alias(c) for c in self.dfspark.columns[1:]))
		# get a cleaner result one by one
		for col in self.dfspark.columns:
			if col in FEATURES_COL:
				self.dfspark = self.dfspark.withColumn(col,self.dfspark[col].cast('float'))
		# drop the null results 
		self.dfspark = self.dfspark.na.drop()

	 	# create a cluster column for featuring
		vecAssembler = VectorAssembler(inputCols=FEATURES_COL, outputCol="features")
		df_kmeans = vecAssembler.transform(self.dfspark).select('id', 'features')

		# find k means center
		k = 2
		kmeans = KMeans().setK(k).setSeed(1).setFeaturesCol("features")
		model = kmeans.fit(df_kmeans)
		centers = model.clusterCenters()

		# assign the individual rows to the nearest cluster centroid
		transformed = model.transform(df_kmeans).select('id', 'prediction')
		rows = transformed.collect()
		# let this become the dataframe
		df_pred = sqlContext.createDataFrame(rows)
		# join the prediction with the original data
		df_pred = df_pred.join(self.dfspark, 'id')
		# convert to pandas dataframe
		pddf_pred = df_pred.toPandas().set_index('id')
		sc.stop()
		# converting dataframe
		pddf_pred.index = pddf_pred.index.astype(int)
		pddf_pred.sort_index(inplace=True)
		pddf_pred = pddf_pred.join([self.df['timestamp']])
		self.pyspark = pddf_pred
		self.pyspark.to_csv('out.csv')


sparkclass = spark()
sparkclass.pullData(args.user)
sparkclass.extractData()
sparkclass.sparkRunning()

# print(start.df[0:10])