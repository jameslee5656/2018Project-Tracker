import serial
import time
import codecs
import time

ser=serial.Serial("/dev/ttyXRUSB1", 9600, timeout= 0.5 ) 
ser.write(bytes("AT+NSOCR=DGRAM,17,8888,1\r\n", 'UTF-8'))
ser.write(bytes("AT+NPING=120.126.136.17\r\n", 'UTF-8'))
SendtoServer = 'AT+NSOST=1,120.126.136.17,5687'
for count in range(1,1000):   
    message = str(time.time())
    messageHexAscii = ''
    message = str(count) + ' ' + message  
    messageByteNumber = str(len(message.encode('utf-8')))
    for x in message:
        messageHexAscii += format(ord(x), "x")
    if ser.is_open:
        ser.write(bytes(SendtoServer + ',' + messageByteNumber + ','
                + messageHexAscii + '\r\n', 'UTF-8'))
    time.sleep(5)