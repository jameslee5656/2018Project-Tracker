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

target_name = 'MI Band 2'
target_address = None


class ScanDelefate(DefaultDelegate):
	def __init__(self):
		DefaultDelegate.__init__(self)

	def handleDiscovery(self, dev, isNewDev, isNewData):
		if isNewDev:
			print('Discovered device', dev.addr)
		elif isNewData:
			print('Received new data from', dev.addr)

def l(x):
    print('Realtime heart:', x)

def b(x):
    print('Raw heart:', x)

def f(x):
    print('Raw accel heart:', x)

for i in range(10):
	start_time = time.time()
	deviceslist = []
	scanner = Scanner().withDelegate(ScanDelefate())
	devices = scanner.scan(3.0)


	for dev in devices:
		deviceDic = {}
		print("Device %s (%s), RSSI=%d dB" % (dev.addr, dev.addrType, dev.rssi))
		deviceDic['device'] = str(dev.addr)
		for (adtype, desc, value) in dev.getScanData():
			print("%s = %s" % (desc, value))
			deviceDic[desc] = value
		deviceslist.append(deviceDic)

	if deviceslist == None:
		waittime = 60 - (time.time() - start_time)
		print("--- %s seconds ---" % (waittime))
		# time.sleep(waittime)
		continue
	for dev in deviceslist:
		try:
			if dev['Complete Local Name'] == target_name:
				MAC = dev['device']
				band = MiBand2(MAC, debug=True)
				band.setSecurityLevel(level="medium")
				if not band.authenticate():
					band.initialize()
					print("Init OK")
					# band.set_heart_monitor_sleep_support(enabled=False)
				# print('Print previews recorded data')
				# band._auth_previews_data_notif(True)
				# preview_time = datetime.strptime("12.03.2018 01:01", "%d.%m.%Y %H:%M")
				# band.start_get_previews_data(preview_time)
				# while band.active:
				# 	band.waitForNotifications(0.1)

				# print ('Message notif')
				# band.send_alert(ALERT_TYPES.MESSAGE)
				# print('Phone notif')
				# band.send_alert(ALERT_TYPES.PHONE)
				# print('OFF')

				band.send_alert(ALERT_TYPES.NONE)
				print('Soft revision:',band.get_revision())
				print('Hardware revision:',band.get_hrdw_revision())
				print('Serial:',band.get_serial())
				print('Battery:', band.get_battery_info())
				print('Time:', band.get_current_time())
				print('Steps:', band.get_steps())

				band.start_raw_data_realtime(
		            heart_measure_callback=l,
		            heart_raw_callback=b,
		            accel_raw_callback=f)
				time.sleep(0.1)
				band.disconnect()
				break
		except Exception:
			pass
	waittime = 60 - (time.time() - start_time)
	print("--- %s seconds ---" % (waittime))
	# time.sleep(waittime)


