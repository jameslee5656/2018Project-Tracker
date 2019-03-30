from pymongo import MongoClient
from bson.objectid import ObjectId
import time,datetime
conn = MongoClient()
db = conn.Tracker
collection = [db.james]# db.db2, db.dn2, db.james, db.leo
for col in collection:
	cursor = col.find({})
	data = [d for d in cursor]
	# print('In',str(col))
	count = 0
	for dic in data:
		if 'o2_value' and 'hr_value' in dic.keys():
			if (dic['longitude'] != 0) and (dic['longitude'] != '') and (dic['latitude'] != '') \
							and (dic['o2_value'] != '') and (dic['hr_value']!= ''):
				count += 1
	with open('data' , 'w') as f:
		f.write(str(data))
	print(count)# 5535
