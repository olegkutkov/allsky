#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates

import MySQLdb
import sys

import config

def plot_cpu_temperature(sensor_data, output_file):
	xdata = []
	ydata = []

	print 'Plotting cpu temperature graph using ' + str(len(sensor_data)) + ' db records'

	for row in sensor_data:
		xdata.append(row[0])
		ydata.append(row[1])

	temper = np.array(ydata)

	plt.title('CPU temperature: ' + str(ydata[-1]) + ' C\n')
	plt.plot(xdata, temper, label = "Temperature", color="red")

	plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

	plt.legend()
	plt.ylabel('Temperature C')
	plt.grid(True)

	plt.tight_layout()

	plt.savefig(output_file, dpi=120)

	print 'Graph saved as ' + output_file

	plt.gcf().clear()

def plot_internal_climate(sensor_data, output_file):
	xdata = []
	ydata_temper = []
	ydata_humidity = []

	print 'Plotting internal temperature/humidity graph using ' + str(len(sensor_data)) + ' db records'

	for row in sensor_data:
		xdata.append(row[0])
		ydata_temper.append(row[1])
		ydata_humidity.append(row[2])

	temper = np.array(ydata_temper)
	humid = np.array(ydata_humidity)

	plt.subplot(211)
	plt.title('Box air temperature and humidity\nCurrent temperature: '
				+ str(ydata_temper[-1]) + ' C\nCurrent humidity: ' + str(ydata_humidity[-1]) + ' %\n')
	plt.plot(xdata, temper, label = "Temperature")

	plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

	plt.legend()
	plt.ylabel('Temperature C')
	plt.grid(True)

	plt.tight_layout()

	plt.subplot(212)
	plt.plot(xdata, humid, label = "Humidity", color='green')

	plt.xlabel('Time period: ' + str(xdata[0].date()) \
				+ ' - ' + str((xdata[len(xdata)-1]).date()))

	plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))

	plt.grid(True)
	plt.legend()
	plt.ylabel('Humidity %')

	plt.tight_layout()

	plt.savefig(output_file, dpi=120)

	print 'Graph saved as ' + output_file

	plt.gcf().clear()


db = MySQLdb.connect(host=config.MYSQL_HOST, user=config.MYSQL_USER, \
			passwd=config.MYSQL_PASSWORD, db=config.MYSQL_DB, connect_timeout=90)

cur = db.cursor()

print 'Selecting data from db'

cur.execute("SELECT * from cpu_sensor WHERE time >= NOW() - INTERVAL 1 DAY")

plot_cpu_temperature(cur.fetchall(), output_file=config.PLOT_CPU_TEMPERATURE_DAY)

cur.execute("SELECT * from internal_dh22 WHERE time >= NOW() - INTERVAL 1 DAY")

plot_internal_climate(cur.fetchall(), output_file=config.PLOT_INTERNAL_DH22_DAY)

db.close()

print 'Done\n'

