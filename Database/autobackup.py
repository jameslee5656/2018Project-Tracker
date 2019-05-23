import paramiko
import datetime
import os
import time

REMOTEHOST = '120.126.136.17'
USERNAME = 'bigdata'
KEYPATH = '/home/james/.ssh/id_rsa'

#ssh settings
ssh = paramiko.SSHClient()
key = paramiko.RSAKey.from_private_key_file(KEYPATH)
ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
now = datetime.datetime.now()
firstflag = True
while True: 
	if firstflag or int((datetime.datetime.now() - now).total_seconds()) > 60*60*12:
		ssh.connect(hostname=REMOTEHOST, port=22,username=USERNAME, pkey=key)
		now = datetime.datetime.now()
		filename = datetime.datetime.now().strftime("%Y-%m-%d-%H-%M-%S") 
		#code, printout
		ssh_stdin, ssh_stdout, ssh_stderr = ssh.exec_command \
						('mongodump -h 127.0.0.1 -d Tracker -o /home/bigdata/dump/'+ filename)
		print('\n'.join( ssh_stdout.readlines()))

		os.system("scp -r bigdata@120.126.136.17:/home/bigdata/dump/" + filename + " backup/")
		os.system("mongorestore -h 127.0.0.1 -d Tracker " + 'backup/' + filename + "/Tracker --drop")
		firstflag = False
		ssh.close()
	else:
		time.sleep(30*61)
