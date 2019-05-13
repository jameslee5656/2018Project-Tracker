from pymongo import MongoClient
import time
import paho.mqtt.client as mqtt

def getHourData():

    t = time.localtime(time.time())
    year = t.tm_year
    month = t.tm_mon
    day = t.tm_mday
    hour = t.tm_hour

    client=mqtt.Client()
    client.connect("120.126.136.17",1883,60)
    topic = ""
    

    mongo = MongoClient('120.126.136.17', 27017)
    db = mongo['Tracker']
    collection = db['prediction']
    
    data = collection.find({'year':year,'month':month,'day':day,'hour':hour,'type':'hourly'})
    
    for x in data:
        topic = x['user']
        print(topic)
        top = '步數:%5s步' % int(x['totalsteps'])
        bot = '目標:%5s步' % x['prediction']
        mes = top+bot
        print(mes)
        client.publish(topic,mes)
        time.sleep(0.01)
    
    mongo.close()
    client.disconnect()
#呼叫   
getHourData()
