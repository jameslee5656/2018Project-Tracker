import paho.mqtt.client as mqtt
from pymongo import MongoClient

userdic = {}

def getStep(step,s,n):
	stepSeriel = s
	nonzero = n

	if step == 0:
		stepSeriel = stepSeriel+1
		nonzero = 0
		if stepSeriel == 18:
			print("Call Function")
			stepSeriel = 0
	elif step > 20:
		nonzero = nonzero+1
		if nonzero == 3:
			stepSeriel = 0
	array = [stepSeriel,nonzero]
	print(array)
	return array

def on_connect(client,userdata,flags,rc):
	print("Connected with result code"+str(rc))
	client.subscribe("test")

def on_message(client,userdata,msg):
	#print(str(msg.payload))
	message = str(msg.payload)
	message = message[2:]
	message = message.split(":")
	try:
		step = int(message[5])
		user = message[0]
	except Exception as e:
		raise
	print(user)
	print(step)

	if user in userdic:
		stepSeriel = userdic[user][0]
		nonzero = userdic[user][1]
		userdic[user] = getStep(step,stepSeriel,nonzero)
	else:
		userdic[user] = [0,0]



client=mqtt.Client()
client.on_connect=on_connect
client.on_message=on_message
client.connect("120.126.136.17",1883,60)
client.loop_forever()