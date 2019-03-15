import paho.mqtt.client as mqtt
from pymongo import MongoClient
import datetime

def on_connect(client,userdata,flags,rc):
  print("Connected with result code"+str(rc))
  client.subscribe("test")

def on_message(client,userdata,msg):
  print(str(msg.payload))
  message = str(msg.payload)
  message = message[2:]
  time = datetime.datetime.now()
  s = message.split(":")

  mongo = MongoClient('120.126.136.17', 27017)
  db = mongo['Tracker']
  collection = db['User_Info']

  num = s[2]
  num = num[:-1]
  FindUser = collection.find_one({"bracelet_number" : num})
  user = FindUser['username']

  '''insert進資料庫'''
  collection = db[user]

  doc = { "longitude": s[0],
          "latitude": s[1],
          "year": time.year,
          "month" : time.month,
          "day" : time.day,
          "hour" : time.hour,
          "minute" : time.minute,
          "second" : time.second
  }
  post_id = collection.insert_one(doc).inserted_id
  print(post_id)
  mongo.close()


'''mqtt訂閱'''
client=mqtt.Client()
client.on_connect=on_connect
client.on_message=on_message
client.connect("120.126.136.17",1883,60)


client.loop_forever()
