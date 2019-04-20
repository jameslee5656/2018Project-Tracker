import subprocess
import pandas as pd
import matplotlib.pyplot as plt
import datetime, time


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

# I produce a dfespercent to calculate the percentage of exercise time
dfespercent = pd.DataFrame({'month': [], 'day': []})
day = datetime.datetime.fromtimestamp(dfspark['timestamp'].min()).strftime('%d')
month = datetime.datetime.fromtimestamp(dfspark['timestamp'].min()).strftime('%m')
basetime = dfspark['timestamp'].min()
while day != datetime.datetime.fromtimestamp(dfspark['timestamp'].max()).strftime('%d') \
    or month != datetime.datetime.fromtimestamp(dfspark['timestamp'].max()).strftime('%m'):
    df2 = pd.DataFrame({'month': [month], 'day': [day], 'exercisetime':0})
    dfespercent = dfespercent.append(df2, ignore_index=True)
    basetime = (datetime.datetime.fromtimestamp(basetime) + datetime.timedelta(days=1)).timestamp()
    day = datetime.datetime.fromtimestamp(basetime).strftime('%d')
    month = datetime.datetime.fromtimestamp(basetime).strftime('%m')

day = datetime.datetime.fromtimestamp(dfspark['timestamp'].min()).strftime('%d')
dfspark['timedelta'] = dfspark['timestamp'].diff()
period = 0
for i, row in enumerate(dfspark.values):
	_id , prediction, hr_value, step_value, timestamp, timedelta = row
	if int(prediction) == 1:
		period += timedelta
	if day != datetime.datetime.fromtimestamp(timestamp).strftime('%d'):
		month = datetime.datetime.fromtimestamp(timestamp).strftime('%m')
		dfespercent.loc[(dfespercent['day'] == day) & (dfespercent['month'] == month), ['exercisetime']] = period
		period = 0
		day = datetime.datetime.fromtimestamp(timestamp).strftime('%d')
month = datetime.datetime.fromtimestamp(dfspark['timestamp'].max()).strftime('%m')
dfespercent.loc[(dfespercent['day'] == day) & (dfespercent['month'] == month), ['exercisetime']] = period
dfespercent['exercisetime'] = dfespercent['exercisetime'] / 86400
print(dfespercent)

dfespercent.loc[(dfespercent['day'] == 3) & (dfespercent['month'] == 29), ['exercisetime']]
# plot = plt.figure(figsize=(12,10)).gca()
# plot.scatter(dfspark.hr_value, dfspark.step_value, c=dfspark.prediction)
# plot.set_xlabel('x')
# plot.set_ylabel('y')
# plt.show()



