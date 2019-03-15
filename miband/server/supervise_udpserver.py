import paho.mqtt.client as mqtt
from pymongo import MongoClient
import datetime
import socket
import time

def on_connect(client,userdata,flags,rc):
 print("Connect with result code"+str(rc))

def on_publish(client,userdata,mid):
 print("Publish to test")

'''MQTT設定'''
client=mqtt.Client()
client.on_connect=on_connect
client.on_publish=on_publish
client.connect("120.126.136.17",1883,60)

'''UDP設定與實作'''
address = ( '120.126.136.17' ,  5687 )
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(address)
print("Connect to UDP 120.126.136.17:5687")

while True :
	data, addr = s.recvfrom( 2048 )
	Nowtime = time.time()
	f = open('nbiotdata', 'a+')
	if not data:
		print ("client has exist")
		break
	pub = data.decode('utf-8')
	time.sleep(0.5)
	client.publish("test",pub)
	lis = pub.split(':')
	sendlng = lis[0]
	sendlat = lis[1]
	sendTime = time.asctime(time.localtime(float(lis[3])))
	recevieTime = time.asctime(time.localtime(Nowtime))
	lag = round(Nowtime - float(lis[3]), 2)
	print ("Package", lis[4], "Send Time", sendTime,
				  "Recevie Time", recevieTime,
				  "Lag", lag)
	f.write(pub + ':' + str(Nowtime) + '\r\n')
	f.close()
s.close()
