import socket

address = ('10.82.128.122', 2001)
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

while True:
	msg = bytes('010203', 'UTF-8')
	s.sendto(msg,address)

s.close()
