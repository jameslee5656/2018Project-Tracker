import serial
import time
import codecs
import time

ser=serial.Serial("/dev/ttyXRUSB1", 9600, timeout= 0.5 ) 
ser.write(bytes("AT\r\n", 'UTF-8'))

for count in range(50):   
	response = ser.readline()
	print(response)
