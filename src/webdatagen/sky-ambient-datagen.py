#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import csv

import MySQLdb
import sys

import config

def plot_sky_ambient_temp(data, output_file):
	xdata = []
	ydata1 = []
	ydata2 = []

	print 'Plotting sky and ambient graph using ' + str(len(data)) + ' db records'

	for row in data:
		ydata1.append(row[1])
		ydata2.append(row[2])
		xdata.append(row[0])

	min_skytemp = min(ydata1)
	max_skytemp = max(ydata1)

	curr_delta = ydata2[-1] - ydata1[-1]

	max_skytemp_time = xdata[ydata1.index(max_skytemp)]
	min_skytemp_time = xdata[ydata1.index(min_skytemp)]

	s1 = np.array(ydata1)
	s2 = np.array(ydata2)

	fig, ax = plt.subplots()

	plt.plot(xdata, s1, label='Sky temperature')
	plt.plot(xdata, s2, label='Air temperature')

	plt.legend()

	plt.xlabel('Date: ' + str(xdata[0].date()))

	ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

	plt.ylabel('Temperature C')

	plt.title('Sky and air temperatures')

	plt.grid(True)

	plt.tight_layout()

	plt.savefig(output_file, dpi=120)
	plt.gcf().clear()

	print 'Skytemp graph saved as ' + output_file

	with open(config.WEB_OUT_TDELTA_FILE, 'w') as web_outtdelta_file:
		web_outtdelta_file.write(str(curr_delta))

	print 'Temperatures delta saved to', config.WEB_OUT_TDELTA_FILE

	with open(config.WEB_OUT_TMM_FILE, 'w') as web_outtmm_file:
		web_outtmm_file.write('<p>Max sky temperature: ' + str(max_skytemp) \
			+ ' at ' + max_skytemp_time.strftime("%H:%M") + '</p>' + '<p>Min sky temperature: ' + str(min_skytemp) \
			+ ' at ' + min_skytemp_time.strftime("%H:%M") + '</p>')

	print 'Sky temperature max min saved to', config.WEB_OUT_TMM_FILE

db = MySQLdb.connect(host=config.MYSQL_HOST, user=config.MYSQL_USER, \
						passwd=config.MYSQL_PASSWORD, db=config.MYSQL_DB, connect_timeout=90)

cur = db.cursor()

cur.execute('select cloud_sensor.time, cloud_sensor.ir_value, external_dh22.temperature from cloud_sensor \
	inner join external_dh22 on date_format(cloud_sensor.time, "%Y-%m-%d %H:%i") = date_format(external_dh22.time, "%Y-%m-%d %H:%i") \
	where cloud_sensor.time >= DATE_SUB(NOW(),INTERVAL 2 HOUR)')

plot_sky_ambient_temp(cur.fetchall(), config.PLOT_SKY_AIR_TEMP)

