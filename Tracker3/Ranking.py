from pymongo import MongoClient
import datetime
import json
import sys

uri = "mongodb://120.126.136.17:27017" 
client = MongoClient(uri)
db = client['Tracker']

class Rank():
	
	def __init__(self, user):
		self.user = user
		self.friends = []
		self.record = []

	#取得所有的好友
	def Getfriends(self):
		collect = db['Friend']
		self.friends = collect.find({'username' : self.user})[0]['Friends']
		self.friends.append(self.user)

	def GetRanking(self):
		self.Getfriends()
		today = datetime.date.today()
		for friend in self.friends:
			step_value = 0
			collect = db['realtimetotalsteps']
			try:
				step_value = collect.find({'user' : friend})[0]['totalsteps']
			except:
				continue
			templist = (friend,step_value)
			self.record.append(templist)

		#排序
		self.record.sort(key=lambda value: value[1], reverse=True)
		rank = []
		for r in self.record:
			r = list(r)
			r.append('../static/images/user/' + r[0] + '.jpg')
			rank.append(list(r))
		return rank
				

'''a = Rank('leo')
b = []
b = a.GetRanking()
print(b)'''
		
