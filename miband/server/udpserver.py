import  socket

address = ( '120.126.136.17' ,  5687 )
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.bind(address)
print("Connect to 120.126.136.17:5687")

while True :
	data, addr = s.recvfrom( 2048 )
	if not data:
		print ("client has exist")
		break
	data = data.decode('utf-8')
	print(data)
	s.sendto(b'68656c6c6f',addr)
	print ("received")
s.close()
