from pymongo import MongoClient
from bson.objectid import ObjectId
import time,datetime
conn = MongoClient()
db = conn.Tracker
collection = [db.james, db.db2, db.dn2, db.james, db.leo]
for col in collection:
	cursor = col.find({})
	data = [d for d in cursor]
	print('In',str(col))
	for dic in data:
		year = dic['year']
		month = dic['month']
		hour = dic['hour']
		day = dic['day']
		minute = dic['minute']
		second = dic['second']
		# time.strftime
		timestring = (str(year) + ' ' + str(month) + ' ' + str(day) + ' ' 
			+ str(hour) + ' ' + str(minute) + ' ' + str(second))
		# print(timestring)
		timestamp = time.mktime(datetime.datetime.strptime((
			str(year) + ' ' + str(month) + ' ' + str(day) + ' ' 
			+ str(hour) + ' ' + str(minute) + ' ' + str(second))
			,'%Y %m %d %H %M %S').timetuple())
		# print(timestamp)
		dic['timestamp'] = timestamp
		dic['year'] = int(year)
		dic['month'] = int(month)
		dic['day'] = int(day)
		dic['hour'] = int(hour)
		dic['minute'] = int(minute)
		dic['second'] = int(second)
		col.update_one({'_id':dic['_id']},{'$set':dic},upsert=False)
# 	break

# cursor = collection.find({})
# data = [d for d in cursor]
# print(data[0])