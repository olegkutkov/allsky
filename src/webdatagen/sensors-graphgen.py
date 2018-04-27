#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import csv

import MySQLdb
import sys

import config

def plot_sky_temp(sensor_data, output_file, output_csv_file, one_day=False):
	xdata = []
	ydata = []

	print 'Plotting skytemp graph using ' + str(len(sensor_data)) + ' db records'

	for row in sensor_data:
		ydata.append(row[1])
		xdata.append(row[0])

	s = np.array(ydata)

	fig, ax = plt.subplots()

	plt.plot(xdata, s)

	plt.xlabel('Time period: ' + str(xdata[0].date()) \
				+ ' - ' + str((xdata[len(xdata)-1]).date()))

	if one_day:
		ax.xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
	else:
		ax.xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
		fig.autofmt_xdate()

	plt.ylabel('Temperature C')
	plt.title('Sky temperature')
	plt.grid(True)

	plt.tight_layout()

	plt.savefig(output_file, dpi=120)
	plt.gcf().clear()

	print 'Skytemp graph saved as ' + output_file

	with open(output_csv_file, 'wb') as csvfile:
		wr = csv.writer(csvfile, quoting=csv.QUOTE_ALL)
		wr.writerow(['date', 'sky temperature'])

		for item in sensor_data:
			wr.writerow([str(item[0]), item[1]])

	print 'Skytemp csv saved as ' + output_csv_file

def plot_ambient_temp(sensor_data, output_file, output_csv_file, one_day=False):
	xdata = []
	ydata_temper = []
	ydata_humidity = []

	print 'Plotting ambient temperature/humidity graph using ' + str(len(sensor_data)) + ' db records'

	for row in sensor_data:
		xdata.append(row[0])
		ydata_temper.append(row[1])
		ydata_humidity.append(row[2])

	temper = np.array(ydata_temper)
	humid = np.array(ydata_humidity)

	plt.subplot(211)
	plt.title('Air temperature and humidity')
	plt.plot(xdata, temper, label = "Temperature")

	if one_day:
		plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
	else:
		plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
		plt.gcf().autofmt_xdate()

	plt.legend()
	plt.ylabel('Temperature C')
	plt.grid(True)

	plt.tight_layout()


	plt.subplot(212)
	plt.plot(xdata, humid, label = "Humidity", color='green')

	plt.xlabel('Time period: ' + str(xdata[0].date()) \
				+ ' - ' + str((xdata[len(xdata)-1]).date()))

	if one_day:
		plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
	else:
		plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
		plt.gcf().autofmt_xdate()

	plt.grid(True)
	plt.legend()
	plt.ylabel('Humidity %')

	plt.tight_layout()

	plt.savefig(output_file, dpi=120)
	plt.gcf().clear()

	print 'Temperature/humidity graph saved as ' + output_file

	with open(output_csv_file, 'wb') as csvfile:
		wr = csv.writer(csvfile, quoting=csv.QUOTE_ALL)
		wr.writerow(['date', 'temperature', 'humidity'])

		for item in sensor_data:
			wr.writerow([str(item[0]), item[1], item[2]])

	print 'Temperature/humidity csv saved as ' + output_csv_file

def plot_ambient_light(sensor_data, output_file, output_csv_file, one_day=False):
	xdata = []
	ydata_temper = []
	ydata_humidity = []

	print 'Plotting ambient ambient light graph using ' + str(len(sensor_data)) + ' db records'

	for row in sensor_data:
		xdata.append(row[0])
		ydata_temper.append(row[1])
		ydata_humidity.append(row[2])

	temper = np.array(ydata_temper)
	humid = np.array(ydata_humidity)

	plt.subplot(211)
	plt.title('Luminosity and infrared radiation')
	plt.plot(xdata, temper, label = "Visible light", color="green")

	if one_day:
		plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
	else:
		plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
		plt.gcf().autofmt_xdate()

	plt.legend()
	plt.ylabel('Lux')
	plt.grid(True)

	plt.tight_layout()


	plt.subplot(212)
	plt.plot(xdata, humid, label = "Infrared", color='red')

	plt.xlabel('Time period: ' + str(xdata[0].date()) \
				+ ' - ' + str((xdata[len(xdata)-1]).date()))

	if one_day:
		plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%H:%M'))
	else:
		plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%Y-%m-%d'))
		plt.gcf().autofmt_xdate()

	plt.grid(True)
	plt.legend()
	plt.ylabel('Lux')

	plt.tight_layout()

	plt.savefig(output_file, dpi=120)
	plt.gcf().clear()

	print 'Ambient light graph saved as ' + output_file

	with open(output_csv_file, 'wb') as csvfile:
		wr = csv.writer(csvfile, quoting=csv.QUOTE_ALL)
		wr.writerow(['date', 'visible', 'infrared'])

		for item in sensor_data:
			wr.writerow([str(item[0]), item[1], item[2]])

	print 'Ambient light csv saved as ' + output_csv_file

def generate_graphs_for_day(cur):
	print 'Fetching cloud sensor data for 1 day'

	cur.execute("SELECT * from cloud_sensor WHERE time >= NOW() - INTERVAL 1 DAY")

	plot_sky_temp(cur.fetchall(), one_day=True, output_file=config.PLOT_CLOUD_SENSOR_DAY, \
								output_csv_file=config.CSV_CLOUD_SENSOR_DAY)

	###

	print '\nFetching external dh22 sensor data for 1 day'

	cur.execute("SELECT * from external_dh22 WHERE time >= NOW() - INTERVAL 1 DAY")

	plot_ambient_temp(cur.fetchall(), one_day=True, output_file=config.PLOT_EXTERNAL_DH22_DAY,\
										output_csv_file=config.CSV_EXTERNAL_DH22_DAY)

	###

	print '\nFetching ambient light sensor data for 1 day'

	cur.execute("SELECT * from ambient_sensor WHERE time >= NOW() - INTERVAL 1 DAY")

	plot_ambient_light(cur.fetchall(), one_day=True, output_file=config.PLOT_AMBIENT_LIGHT_DAY,\
										output_csv_file=config.CSV_AMBIENT_LIGHT_DAY)


def generate_graphs_for_week(cur):
	print '\nFetching cloud sensor data for 1 week'

	cur.execute("SELECT * from cloud_sensor WHERE time >= NOW() - INTERVAL 1 WEEK")

	plot_sky_temp(cur.fetchall(), one_day=False, output_file=config.PLOT_CLOUD_SENSOR_WEEK,\
								output_csv_file=config.CSV_CLOUD_SENSOR_WEEK)

	###

	print '\nFetching external dh22 sensor data for 1 week'

	cur.execute("SELECT * from external_dh22 WHERE time >= NOW() - INTERVAL 1 WEEK")

	plot_ambient_temp(cur.fetchall(), one_day=False, output_file=config.PLOT_EXTERNAL_DH22_WEEK,\
										output_csv_file=config.CSV_EXTERNAL_DH22_WEEK)

	###

	print '\nFetching ambient light sensor data for 1 week'

	cur.execute("SELECT * from ambient_sensor WHERE time >= NOW() - INTERVAL 1 WEEK")

	plot_ambient_light(cur.fetchall(), one_day=False, output_file=config.PLOT_AMBIENT_LIGHT_WEEK,\
										output_csv_file=config.CSV_AMBIENT_LIGHT_WEEK)

def generate_graphs_for_month(cur):
	print '\nFetching cloud sensor data for 1 month'

	cur.execute("SELECT * from cloud_sensor WHERE time >= NOW() - INTERVAL 1 MONTH")

	plot_sky_temp(cur.fetchall(), one_day=False, output_file=config.PLOT_CLOUD_SENSOR_MONTH,\
								output_csv_file=config.CSV_CLOUD_SENSOR_MONTH)

	###

	print '\nFetching external dh22 sensor data for 1 month'

	cur.execute("SELECT * from external_dh22 WHERE time >= NOW() - INTERVAL 1 MONTH")

	plot_ambient_temp(cur.fetchall(), one_day=False, output_file=config.PLOT_EXTERNAL_DH22_MONTH,\
										output_csv_file=config.CSV_EXTERNAL_DH22_MONTH)

	###

	print '\nFetching ambient light sensor data for 1 month'

	cur.execute("SELECT * from ambient_sensor WHERE time >= NOW() - INTERVAL 1 MONTH")

	plot_ambient_light(cur.fetchall(), one_day=False, output_file=config.PLOT_AMBIENT_LIGHT_MONTH,\
										output_csv_file=config.CSV_AMBIENT_LIGHT_MONTH)

def generate_graphs_for_year(cur):
	print '\nFetching cloud sensor data for 1 year'

	cur.execute("SELECT * from cloud_sensor WHERE time >= NOW() - INTERVAL 1 YEAR")

	plot_sky_temp(cur.fetchall(), one_day=False, output_file=config.PLOT_CLOUD_SENSOR_YEAR,\
								output_csv_file=config.CSV_CLOUD_SENSOR_YEAR)

	###

	print '\nFetching external dh22 sensor data for 1 year'

	cur.execute("SELECT * from external_dh22 WHERE time >= NOW() - INTERVAL 1 YEAR")

	plot_ambient_temp(cur.fetchall(), one_day=False, output_file=config.PLOT_EXTERNAL_DH22_YEAR,\
										output_csv_file=config.CSV_EXTERNAL_DH22_YEAR)

	###

	print '\nFetching ambient light sensor data for 1 year'

	cur.execute("SELECT * from ambient_sensor WHERE time >= NOW() - INTERVAL 1 YEAR")

	plot_ambient_light(cur.fetchall(), one_day=False, output_file=config.PLOT_AMBIENT_LIGHT_YEAR,\
									output_csv_file=config.CSV_AMBIENT_LIGHT_YEAR)

def main(args):
	db = MySQLdb.connect(host=config.MYSQL_HOST, user=config.MYSQL_USER, \
							passwd=config.MYSQL_PASSWORD, db=config.MYSQL_DB, connect_timeout=90)

	cur = db.cursor()

	if len(args) == 1:
		generate_graphs_for_day(cur)
		generate_graphs_for_week(cur)	
		generate_graphs_for_month(cur)
		generate_graphs_for_year(cur)
	else:
		if args[1] == 'sensors-day':
			generate_graphs_for_day(cur)
		elif args[1] == 'sensors-week':
			generate_graphs_for_week(cur)
		elif args[1] == 'sensors-month':
			generate_graphs_for_month(cur)
		elif args[1] == 'sensors-year':
			generate_graphs_for_year(cur)

	db.close()

	print 'Done\n'

if __name__ == "__main__":
    main(sys.argv)

