import subprocess
import pandas as pd
import matplotlib.pyplot as plt


def callspark(user='james'):
	python3_command = "./pyspark_class.py -u " + user  # launch your python2 script using bash
	process = subprocess.Popen(python3_command.split(), stdout=subprocess.PIPE)
	output, error = process.communicate()  # receive output from the python2 script
	returned_val = output.decode('utf-8')
	returnlist = returned_val.split('\n')
	returnlist.remove('')
	print('message: ',returnlist)
	# this is a pandas dataframe
	# if you don't know how to use this tell me
	dfspark = pd.read_csv('out.csv')
	return dfspark


# this is the main fuction first argument 
# is the user you want to look at
dfspark = pd.DataFrame()
dfspark = callspark('leo')
# print(dfspark)


plot = plt.figure(figsize=(12,10)).gca()
plot.scatter(dfspark.hr_value, dfspark.step_value, c=dfspark.prediction)
plot.set_xlabel('x')
plot.set_ylabel('y')
plt.show()