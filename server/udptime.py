import  socket
import	time

address = ( '120.126.136.17' ,  5687 )
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(address)
print("Connect to 120.126.136.17:5687")

while True :
	data, addr = s.recvfrom( 2048 )
	f = open('nbiotdata', 'a+')
	Nowtime = time.time()
	if not data:
		print ("client has exist")
		break
	data = data.decode('utf-8')
	lis = data.split(':')
	sendlng = lis[0]
	sendlat = lis[1]
	sendTime = time.asctime(time.localtime(float(lis[3])))
	recevieTime = time.asctime(time.localtime(Nowtime))
	lag = round(Nowtime - float(lis[3]), 2)
	print ("Package", lis[2], "Send Time", sendTime,
				  "Recevie Time", recevieTime,
				  "Lag", lag)
	f.write(data + ':' + str(Nowtime) + '\r\n')
	f.close()
	
s.close()
