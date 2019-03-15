import serial
import time
import codecs
import os

#mi band import libary
import sys
import os
import signal
import subprocess
import threading
import time
import argparse
from datetime import datetime
from base import MiBand2
from constants import ALERT_TYPES
from bluepy.btle import Scanner, DefaultDelegate


#mi band

class ScanDelefate(DefaultDelegate):
    def __init__(self):
        DefaultDelegate.__init__(self)
    def handleDiscovery(self, dev, isNewDev, isNewData):
        if isNewDev:
            print('Discovered device', dev.addr)
        elif isNewData:
            print('Received new data from', dev.addr)

# GPS code 
def fixlat(data):
    lat_int=float(data[0:2])
    lat_float=float(data[2:])/60
    return lat_int+lat_float

def fixlng(data):
    lng_int=float(data[0:3])
    lng_float=float(data[3:])/60
    return lng_int+lng_float

def initialize():
# initialise serial port settings
    Port = serial.Serial()
    Port.baudrate = 9600#115200
    Port.port = '/dev/ttyS0'

    Port.xonxoff = 1
# return the port as an object we can use
    return Port

def getdata(port):
# open the serial port
    port.open()
# check that the port is open
    if port.isOpen():
# read 16 line
        flag=20
        f = open('/home/bigdata/Desktop/nbiotWithDongle/logs', 'a+')
        while True:
            line=str(port.readline())
            print(line)
            if line.find('RMC')>0:
                if line.find(',A,')>0:
                    print(line)
                    lat=line[line.index('A')+1:line.index('N',10)].strip(',')
                    lng=line[line.index('N',10)+1:line.index('E')].strip(',')
                    flng = fixlng(lng)
                    flat = fixlat(lat)
                    f.close()
                    break
                else:
                    flag -= 1
                    if flag ==0:
                        f.write('DATA ERROR!!/n') 
                        f.close()
        
# close the serial port
    port.close()

# discard the first line (sometimes it contains rubbish, so just always discard it)
    #del line[0]
# return the list of lines
    return flng, flat

#Ping the server and read the reponse to file f
def ping(ser):
    while True:
        ser.write(bytes("AT+NPING=120.126.136.17\r\n", 'UTF-8'))
        f = open('/home/bigdata/Desktop/nbiotWithDongle/logs', 'a+')#open logs
        response = []
        for _ in range(5):
            response.append(ser.readline().decode("utf-8"))
            print(response)
        f.write("    PING:" + str(response))
        f.close()
        if 'OK\r\n' in response:
            return

# for miband init
def bandinit(MAC='c7:28:2d:cc:4f:c9'):
    band = MiBand2(MAC, debug=True)
    band.setSecurityLevel(level="medium")
    if not band.authenticate():
        band.initialize()
        print("Init OK")
    return band

# this function take the current heartrate out of miband
def getdataMI(band, data = []):
    def l(x):
        print('Realtime heart:', x)
        data.append(x)
    def b(x):
        print('Raw heart:', x)
    def f(x):
        print('Raw accel heart:', x)                

    # band.set_heart_monitor_sleep_support(enabled=False)
    # print('Print previews recorded data')
    # band._auth_previews_data_notif(True)
    # preview_time = datetime.strptime("12.03.2018 01:01", "%d.%m.%Y %H:%M")
    # band.start_get_previews_data(preview_time)
    # while band.active:
    #   band.waitForNotifications(0.1)

    # print ('Message notif')
    # band.send_alert(ALERT_TYPES.MESSAGE)
    # print('Phone notif')
    # band.send_alert(ALERT_TYPES.PHONE)
    # print('OFF')
    # band.send_alert(ALERT_TYPES.NONE)
    # print('Soft revision:',band.get_revision())
    # print('Hardware revision:',band.get_hrdw_revision())
    # print('Serial:',band.get_serial())
    # print('Battery:', band.get_battery_info())

    # print('Time:', band.get_current_time())
    data.append(band.get_steps()['steps'])
    print('Steps:', data[0])

    band.start_raw_data_realtime(
    heart_measure_callback=l,
    heart_raw_callback=b,
    accel_raw_callback=f)
    print(data)
    time.sleep(0.5)
    return data
        
if __name__ == '__main__':
    ser=serial.Serial('/dev/ttyXRUSB1', 9600, timeout = 0.5)

    failFlag = 50
    response = []
    start_time = 0
    target_name = 'MI Band 2'
    target_address = 'c7:28:2d:cc:4f:c9'
    #The first stage Open a socket and read a socketID
    while True:
        f = open('/home/bigdata/Desktop/nbiotWithDongle/logs', 'a+')
        try:
            ser.write(bytes("AT+NSOCR=DGRAM,17,5245,1\r\n", 'UTF-8'))
            for _ in range(5):
                response.append(ser.readline().decode("utf-8"))
                print(response)
            f.write("   NSOCR:" + str(response))
            
            if failFlag <= 0:
                #It try more than 50 times reboot the ubuntu
                f.write('Fail Connection/r/n')
                f.close()
                #os.system('reboot')
                #Prevent reboot fail
                print('failFlag')
                sys.exit(0)  
            elif 'OK\r\n' in response:
                #response == OK
                #socketID = 1~6       
                socketID = int(response[response.index('OK\r\n') - 2])
                f.write('socketID=' + str(socketID) + '\r\n')
                f.close()
                break
            else:
                failFlag -= 1
                f.write('Still trying\r\n')
                f.close()
                continue

        except Exception as e:
            f.write('the last part in NSCOR: ' + str(e))

    #Basic sentence for sending message
    SendtoServer = 'AT+NSOST=' + str(socketID) + ',120.126.136.17,5687'
    nameNum = 5
    count = 0
    while True:
        #must be 0 beacause the first package must ping
        start_time = time.time()
        ping(ser)
        time.sleep(5)
        f = open('/home/bigdata/Desktop/nbiotWithDongle/logs', 'a+')#open logs
        print('Package' , str(count))
        f.write('Package'+ str(count) + ':')
        count += 1
        sendsuccessflag = 0
        try:
            messageHexAscii = ''
            sPort = initialize()#GPS initialize
            lng, lat = getdata(sPort)#GPS data
            message = str(time.time())#now time

            #find mi band
            deviceslist = []
            scanner = Scanner().withDelegate(ScanDelefate())
            devices = scanner.scan(5.0)

            for dev in devices:
                deviceDic = {}
                #print("Device %s (%s), RSSI=%d dB" % (dev.addr, dev.addrType, dev.rssi))
                deviceDic['device'] = str(dev.addr)
                for (adtype, desc, value) in dev.getScanData():
                    #print("%s = %s" % (desc, value))
                    deviceDic[desc] = value
                deviceslist.append(deviceDic)

            if deviceslist == None:
                waittime = 60 - (time.time() - start_time)
                print("--- %s seconds ---" % (waittime))
                # time.sleep(waittime)
                continue
            data = []
            for dev in deviceslist:
                try:
                    print(dev['device'])
                    if dev['device'] == target_address:
                        MAC = dev['device']
                        band = bandinit(MAC)
                        data = getdataMI(band)

                        if data == None:
                            print('No data')
                            break
                        #form Message
                        message = str(round(lng,5))+ ':' +str(round(lat,5)) + ':' + str(nameNum) + ':' + message + ':' + str(count) + ':' + str(data)
                        #count how many byte
                        messageByteNumber = str(len(message.encode('utf-8')))
                        #Turn to ascii code
                        for x in message:
                            messageHexAscii += format(ord(x), "x")
                        #Write to Dongle
                        if ser.is_open:
                            ser.write(bytes(SendtoServer + ',' + messageByteNumber + ','
                                    + messageHexAscii + '\r\n', 'UTF-8'))

                        # #read data 5 line
                        # response = []
                        # for _ in range(5):
                        #     read = ser.readline().decode("utf-8")
                        #     if read:
                        #         response.append(read)
                        #write message     
                        sendsuccessflag = 1  
                        print('successfull')
                        #f.write(response.append('successfull\r\n'))
                except Exception as e:
                    print('there is a problem here' ,str(e))
                    pass
            
        except Exception as e: 
            print('Last part Sending: ' + str(e) + '\r\n')
            #f.write('Last part Sending: ' + str(e) + '\r\n')
            pass
        if sendsuccessflag == 1:
             while 60 - (time.time() - start_time) > 0:
                read = ser.readline().decode("utf-8")
                print(read)
                if read.find('+NSONMI') != -1:
                    print('into NSONMI')
                    start = read.find('+NSONMI:')
                    print("AT+NSORF="+read[start:])
                    ser.write(bytes("AT+NSORF="+read[start+8:], 'UTF-8'))
                    for _ in range(5):
                        read = ser.readline().decode("utf-8")
                        if read:
                            read_start = read.find('120.126.136.17,5687,')
                            if read_start != -1:
                                print(read)
                                returnMessage = read[read_start + 20:read.find(',0')]
                                print(returnMessage)
                                if returnMessage == '1,33':
                                    band.send_alert(ALERT_TYPES.MESSAGE)
                print("--- %s seconds ---" % (60 - (time.time() - start_time)))
        else:
            while 60 - 60 - (time.time() - start_time):
                time.sleep(3)
                print("--- %s seconds ---" % (60 - (time.time() - start_time)))
                read = ser.readline().decode("utf-8")
        if isinstance(band, int):
            band.disconnect()
        f.close()
    ser.close()

