from pymongo import MongoClient
import datetime
import json

uri = "mongodb://120.126.136.17:27017" 
client = MongoClient(uri)
db = client['Tracker']

class Health():
	def __init__(self, user):
		self.user = user

	def avr(self,arr,mode):    #mode 0 for step data
		total = 0
		count = 1
		for a in arr:
			if a != '':
				total += int(a)
				count += 1
		if mode:
			avr = int(total/count)
			return avr
		else:
			return total

	def Day(self, datatype, day, month):
		x_time = []
		y_data = []
		count = {}
		datas = db[self.user].find({'day': day , 'month': month})
		for data in datas:
			if data[datatype] == '' or 0:
				continue
			elif data['hour'] not in count:
				temp = [data[datatype]]
				count[data['hour']] = temp
			else:
				temp = count[data['hour']]
				temp.append(data[datatype])
				count[data['hour']] = temp
        #計算平均
		for c in count:
			if datatype == 'step_value':
				count[c] = self.avr(count[c],0)
			else:
				count[c] = self.avr(count[c],1)
			x_time.append(int(c))
			y_data.append(count[c])
		return x_time, y_data

	def Week(self, datatype):
		x_time = []
		y_data = []
		today = datetime.date.today()
		#獲取七天的天數
		for i in range(7):
			yesterday = today - datetime.timedelta(days=i)
			yesterday = str(yesterday)[5:].replace("-","/")
			x_time.append(yesterday)
		for x in x_time:
			datas = db[self.user].find({'day': int(x[3:]) , 'month': int(x[:2])})
			tempdata = []
			for data in datas:
				if data[datatype] == '' or 0:
					continue
				else:
					tempdata.append(data[datatype])
			if datatype == 'step_value':
				y_data.append(self.avr(tempdata,0))
			else:
				y_data.append(self.avr(tempdata,1))
		return x_time, y_data

# he = Health('leo')
# he.Week('hr_value')
