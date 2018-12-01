import math
from pymongo import MongoClient
import pprint
import json
import matplotlib.pyplot as plt
import matplotlib.path as mplPath
import numpy as np
import urllib.request, json 

class electricFence():
    def __init__(self):
        self.latitude = []
        self.longitude = []
        self.location = []
        self.frequency = {}
        self.chosenPoint = []
        self.user = ''
        
    def pullData(self, user): #pull latitude and longitude
        
        self.user = user
        
        client = MongoClient('120.126.136.17',27017)
        db = client['Tracker']
        collection = db[user]
        cursor = collection.find({})

        jsonData = [d for d in cursor]

        for x in jsonData:
            self.latitude.append(float(x['latitude']))
            self.longitude.append(float(x['longitude']))
            
    def onlySanxia(self):
        
        with urllib.request.urlopen("https://sheethub.com/ronnywang/%E9%84%89%E9%8E%AE%E5%B8%82%E5%8D%80%E8%A1%8C%E6%94%BF%E5%8D%80%E5%9F%9F%E7%95%8C%E7%B7%9A/uri/19260593?format=geojson") as url:
            data = json.loads(url.read().decode())
        
        # from scipy.spatial import ConvexHull
        boarderData = data['features'][0]['geometry']['coordinates'][0]
        
        #make out a path
        borderPath = mplPath.Path(boarderData) 
        
        for x in range(len(self.latitude)):
            if borderPath.contains_points(np.array([[self.longitude[x], self.latitude[x]]])):       
                self.location.append([round(self.latitude[x],4), round(self.longitude[x],4)])
                
    def removeOutlier(self):
        #remove that the variance is too large
        trainingset = np.array(self.location)
        mean = np.mean(trainingset, axis=0)
        sd = np.std(trainingset, axis=0)
        final_list = [x for x in self.location if ((x[0] > mean[0] - 6 * sd[0])and(x[1] > mean[1] - 6 * sd[1]))]
        final_list = [x for x in final_list if ((x[0] < mean[0] + 6 * sd[0])and(x[1] < mean[1] + 6 * sd[1]))]       
        self.location = final_list
        
    def abDistance(a, b):# calculate a and b distance  a[a緯, a經], b[b緯, b經]
        """
        兩點間距離D(x1,y1,x2,y2)=r×acos(sin(x1) ×sin(x2)+cos(x1) ×cos(x2) ×cos(y1-y2))
        x,y是緯度，經度的弧度單位，r是地球半徑
        """
        op = math.pi/180
        r = 6378.39
        return r*math.acos(math.sin(a[0]*op) * math.sin(b[0]*op) + 
                           math.cos(a[0]*op) * math.cos(b[0]*op) * math.cos(a[1]*op-b[1]*op))
    
    #establish a dict to count how many duplicate location
#     def addFrequency(self):
#         for x in self.location:
#             if tuple(x) in self.frequency:
#                 self.frequency[tuple(x)] += 1
#             else:
#                 self.frequency[tuple(x)] = 1
                
    def chosePoint(self):#point without duplicate
        #map out a tuplelist that it`s element is tuple type
        tuplelist = list(map(tuple, self.location))
        self.chosenPoint = list(set(tuplelist))
        return self.chosenPoint
        # return self.location
    
    #make the bounds
    def squareBounds(self,boundScale = 6):
        downlat = 24.938590 
        leftlong = 121.360761
        toplat = 24.94884
        rightlong = 121.373937

        temptlist = [[round(x[0],4), round(x[1],4)] for x in self.location]

        squarefreq = {}
        for i in range(int(round(toplat - downlat,4)*10000)):
            for j in range(int(round(rightlong - leftlong,4)*10000)):
        #         print(i,j,tuple([round(downlat + i/10000 ,5), round(leftlong + j/10000,5)]))
                squarefreq[tuple([round(downlat + i/10000 ,4), round(leftlong + j/10000,4)])]=0

        
        for x in temptlist:
            if tuple(x) in squarefreq:
                if round((x[0] - downlat)*10000,0) % boundScale != 0:
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
        #         print(tuple(x))
        # print(max(squarefreq.values()))

        boundlist = []
        squarefreqMax = math.log10(max(squarefreq.values()))
        # print(squarefreqMax)
        for k,values in squarefreq.items():
            key = list(k)
            if round((key[0] - downlat)*10000,0) % boundScale == 0:
                if round((key[1] - leftlong)*10000,0) % boundScale == 0:
                    lt = [round(key[0] + 0.00005*boundScale,5), round(key[1] - 0.00005*boundScale,5)]
                    rt = [round(key[0] + 0.00005*boundScale,5), round(key[1] + 0.00005*boundScale,5)]
                    rd = [round(key[0] - 0.00005*boundScale,5), round(key[1] + 0.00005*boundScale,5)]
                    ld = [round(key[0] - 0.00005*boundScale,5), round(key[1] - 0.00005*boundScale,5)]
                #     print(lt,rt,rd,ld)
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
                    else:
                        precentage = 4
                    boundlist.append([lt,rt,rd,ld,precentage])
        return boundlist