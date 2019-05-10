import math
from pymongo import MongoClient
from xml.etree import ElementTree
import pprint
import json
import matplotlib.pyplot as plt
import matplotlib.path as mplPath
import numpy as np
import urllib.request, json
import datetime, time
import operator
import pandas as pd
import numpy as np

class electricFence():
    def __init__(self):
        self.df = pd.DataFrame({})
        self.latitude = []
        self.longitude = []
        self.location = []
        self.frequency = {}
        self.chosenPoint = []
        self.user = ''
        self.userlist = []
        self.jsonData = {}
        
    def pullData(self, user, userlist,days=14): #pull latitude and longitude
        self.user = user
        self.userlist = userlist
        for x in userlist:
            conn = MongoClient('120.126.136.17')
            db = conn.Tracker
            collection = [db.james]# db.db2, db.dn2, db.james, db.leo
            userInfo = []
            end = datetime.datetime.now().timestamp()
            start = (datetime.datetime.now() + datetime.timedelta(days = -days)).timestamp()
            for col in collection:
                cursor = col.find({'timestamp': {'$gte': start, '$lt': end}})
                df = pd.DataFrame(list(cursor))
            df.replace('', np.nan, inplace=True)
            df.fillna(method='ffill', inplace=True)
            df['latitude'] = df['latitude'].astype(float).round(4)
            df['longitude'] = df['longitude'].astype(float).round(4)
            self.df = df
        
    def abDistance(a, b):# calculate a and b distance  a[a緯, a經], b[b緯, b經]
        """
        兩點間距離D(x1,y1,x2,y2)=r×acos(sin(x1) ×sin(x2)+cos(x1) ×cos(x2) ×cos(y1-y2))
        x,y是緯度，經度的弧度單位，r是地球半徑
        """
        op = math.pi/180
        r = 6378.39
        return r*math.acos(math.sin(a[0]*op) * math.sin(b[0]*op) + 
                           math.cos(a[0]*op) * math.cos(b[0]*op) * math.cos(a[1]*op-b[1]*op))
    
    #make the bounds
    def squareBounds(self,boundScale = 1,baseLocation = [0,0]):#baseLocation = [24.938590,121.360761]
        # Let baseLocation be the lattest data
        if baseLocation == [0,0]:
            lattestData = self.df[-1:]
            baseLocation = [lattestData['latitude'], lattestData['longitude']]
                
        #checking boundScale
        if type(boundScale) != 'int':
            boundScale = int(boundScale)
        if boundScale < 1:
            boundScale = 1
        elif boundScale > 10:
            boundScale = 10
        
        distance = 0.01
        downlat = float(baseLocation[0]) - distance #24.938590 
        leftlong = float(baseLocation[1]) - distance #121.360761

        # squarefreq = df.groupby(df[['longitude','latitude']].columns.tolist(),as_index=False).size()
        squarefreq = {}
        #make out a dic that is the smallest boundScale = 1
        for i in range(int(round(distance * 2,4)*10000)):
            for j in range(int(round(distance * 2,4)*10000)):
                squarefreq[tuple([round(downlat + i/10000 ,4), round(leftlong + j/10000,4)])] = 0
        # print(squarefreq)
        # print(temptlist)
        temptlist = self.df[['latitude','longitude']].values
        for x in temptlist:
            # print(tuple(x), tuple(x) in squarefreq)
            if tuple(x) in squarefreq:
                #the way i think: use mod to divide into two group just like roundup
                if round((x[0] - downlat)*10000,0) % boundScale != 0:
                    #less or equal than boundScale/2 = the smaller on 
                    if round((x[0] - downlat)*10000,0)% boundScale <= boundScale/2:
                        x[0] -= (round((x[0] - downlat)*10000,0) % boundScale)/10000
                    else:
                        x[0] += (boundScale - (round((x[0] - downlat)*10000,0) % boundScale))
                    x[0] = round(x[0],4)
                if round((x[1] - leftlong)*10000,0) % boundScale != 0:
                    if round((x[0] - leftlong)*10000,0) % boundScale != 0:
                        x[1] -= (round((x[0] - leftlong)*10000,0) % boundScale)/10000
                    else:
                        x[1] += (boundScale - (round((x[0] - leftlong)*10000,0) % boundScale))
                    x[1] = round(x[1],4)
                if tuple(x) in squarefreq:
                    squarefreq[tuple(x)] += 1

#         #divide into different percentage
        boundlist = []
#         # print(squarefreq)
        squarefreqMax = max(squarefreq.values())
#         # print(squarefreqMax)
        if squarefreqMax != 0:
            squarefreqMax = math.log10(max(squarefreq.values()))
        for k,values in squarefreq.items():
            key = list(k)
            org_values = values
            # print(org_values)
            if round((key[0] - downlat)*10000,0) % boundScale == 0:
                if round((key[1] - leftlong)*10000,0) % boundScale == 0:
                    lt = [round(key[0] + 0.00005*boundScale,5), round(key[1] - 0.00005*boundScale,5)]
                    rt = [round(key[0] + 0.00005*boundScale,5), round(key[1] + 0.00005*boundScale,5)]
                    rd = [round(key[0] - 0.00005*boundScale,5), round(key[1] + 0.00005*boundScale,5)]
                    ld = [round(key[0] - 0.00005*boundScale,5), round(key[1] - 0.00005*boundScale,5)]
                    precentage = 0
                    if values!=0:
                        values = math.log10(values)
                    if(values>=0)and(values<squarefreqMax/5):
                        precentage = 0
                    elif (values>=squarefreqMax/5)and(values<2*squarefreqMax/5):
                        precentage = 1
                    elif (values>=2*squarefreqMax/5)and(values<3*squarefreqMax/5):
                        precentage = 2
                    elif (values>=3*squarefreqMax/5)and(values<4*squarefreqMax/5):
                        precentage = 3
                    elif (squarefreqMax == 0):
                        precentage = 0
                    else:
                        precentage = 4
                    boundlist.append([lt,rt,rd,ld,precentage,org_values])
                    
        squarelen = int(math.sqrt(len(boundlist)))
        # print(squarelen,'\n\n////')
        spacelist = []
        #imply from bottomleft to topright
        # temptlist.append(boundlist[0][3])
        # temptlist.append(boundlist[0][2])
        # temptlist.append(boundlist[0][0])
        # temptlist.append(boundlist[0][1])

        # temptlist.append(boundlist[0][3])
        # temptlist.append(boundlist[10][1])
        # temptlist.append(boundlist[1][3])
        # temptlist.append([boundlist[10][1][0],boundlist[1][3][1]])

        #imply from topright to bottomleft
        # temptlist.append(boundlist[squarelen*squarelen - 1][1])
        # temptlist.append(boundlist[squarelen*squarelen - 1][3])
        # temptlist.append(boundlist[squarelen*squarelen - 1][2])
        # temptlist.append(boundlist[squarelen*squarelen - 1][0])

        # temptlist.append(boundlist[squarelen*squarelen - 2][0])
        # temptlist.append(boundlist[squarelen*(squarelen-1) - 1][2])
        # temptlist.append([boundlist[squarelen*(squarelen-1) - 1][3][0],boundlist[squarelen*squarelen - 2][0][1]])
        for x in range(squarelen):
            temptlist = []
            temptlist.append(boundlist[0][3])
            temptlist.append(boundlist[x * squarelen][0])
            temptlist.append([boundlist[x * squarelen][0][0],boundlist[x][2][1]])
            temptlist.append(boundlist[x][2])
            spacelist.append(temptlist)

        for x in range(squarelen - 1):
            temptlist = []
            temptlist.append(boundlist[squarelen*squarelen - 1][1])
            temptlist.append(boundlist[squarelen*squarelen - 1 - x][0])
            temptlist.append([boundlist[squarelen*(squarelen-x) - 1][3][0],boundlist[squarelen*squarelen - 1 - x][0][1]])
            temptlist.append(boundlist[squarelen*(squarelen-x) - 1][2])
            spacelist.append(temptlist)
            
        valuelist=[]
        # print(boundlist[0])
        for x in boundlist:
            # print(x[4],x[5])
            if x[4] != 0:
                # print(x[4])
                valuelist.append(x)
        valuelist = sorted(valuelist,key=lambda x: x[5],reverse=True)
#         sortNum = 1
        newlist = []
        # print(valuelist[0])
        # print(valuelist)
        for x in valuelist:
            tempt = x[0:6]
#             tempt.append(sortNum)
#             sortNum += 1
            newlist.append(tempt)
        return spacelist,newlist,baseLocation


user = 'james' 
userlist = ['james'] 
elFence = electricFence() 
elFence.pullData(user,userlist,days=14)
spacelist,valuelist,base = elFence.squareBounds() 
print(spacelist) 
# print(valuelist)
# print(base)