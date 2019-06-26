#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.dates as mdates
import csv

import MySQLdb
import sys

import config


def plot_sqm(sensor_data, output_file, output_dat_file, one_day=False):
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

	plt.ylabel('Sky Brightness (mag/arcsec2)')
	plt.title('Sky quality meter, CrAO, sensor #4220')
	plt.grid(True)

	plt.tight_layout()

	plt.savefig(output_file, dpi=120)
	plt.gcf().clear()

	print 'Skytemp graph saved as ' + output_file

	curr_sqm_val = float(sensor_data[0][1])
	prev_sqm_val = float(sensor_data[1][1])

	curr_sqm_str = '<div style="display: inline" class="text-danger">' + str(curr_sqm_val) + '</div> '

	if (curr_sqm_val > prev_sqm_val):
		curr_sqm_str += '&#8593'
	else:
		curr_sqm_str += '&#8595'

	with open(config.SQM_CURRENT_VAL, 'w') as curr_sqm:
		curr_sqm.write(curr_sqm_str)

#	with open(output_dat_file, 'wb') as csvfile:
#		wr = csv.writer(csvfile, quoting=csv.QUOTE_ALL)
#		wr.writerow(['date', 'sky temperature'])

#		for item in sensor_data:
#			wr.writerow([str(item[0]), item[1]])

#	print 'Skytemp csv saved as ' + output_dat_file



def generate_graph_for_day(cur, sensor_table, out_img_file, out_dat_file):
	print 'Fetching sqm sensor data for 1 day'

	cur.execute("SELECT time, sky_brightness from " + sensor_table + " WHERE time >= NOW() - INTERVAL 2 DAY")

	plot_sqm(cur.fetchall(), one_day=True, output_file=out_img_file, \
								output_dat_file=out_dat_file)

def generate_graph_for_week(cur, sensor_table, out_img_file, out_dat_file):
	print 'Fetching sqm sensor data for 1 week'

	cur.execute("SELECT time, sky_brightness from " + sensor_table + " WHERE time >= NOW() - INTERVAL 1 WEEK")

	plot_sqm(cur.fetchall(), one_day=False, output_file=out_img_file, \
								output_dat_file=out_dat_file)

def generate_graph_for_month(cur, sensor_table, out_img_file, out_dat_file):
	print 'Fetching sqm sensor data for 1 month'

	cur.execute("SELECT time, sky_brightness from " + sensor_table + " WHERE time >= NOW() - INTERVAL 1 MONTH")

	plot_sqm(cur.fetchall(), one_day=False, output_file=out_img_file, \
								output_dat_file=out_dat_file)

def generate_graph_for_year(cur, sensor_table, out_img_file, out_dat_file):
	print 'Fetching sqm sensor data for 1 year'

	cur.execute("SELECT time, sky_brightness from " + sensor_table + " WHERE time >= NOW() - INTERVAL 1 YEAR")

	plot_sqm(cur.fetchall(), one_day=False, output_file=out_img_file, \
								output_dat_file=out_dat_file)

def main(args):
	db = MySQLdb.connect(host=config.MYSQL_HOST, user=config.MYSQL_USER, \
							passwd=config.MYSQL_PASSWORD, db=config.MYSQL_DB_SQM, connect_timeout=90)

	cur = db.cursor()

	table_name = 'sqm_data'

	if len(args) == 1:
		generate_graph_for_day(cur, table_name, config.SQM_SENSOR_DAY, config.DAT_SQM_SENSOR_DAY)

		generate_graph_for_week(cur, table_name, config.SQM_SENSOR_WEEK, config.DAT_SQM_SENSOR_WEEK)

		generate_graph_for_month(cur, table_name, config.SQM_SENSOR_MONTH, config.DAT_SQM_SENSOR_MONTH)

		generate_graph_for_year(cur, table_name, config.SQM_SENSOR_YEAR, config.DAT_SQM_SENSOR_YEAR)
	else:
		if args[1] == 'sqm-day':
			generate_graph_for_day(cur, table_name, config.SQM_SENSOR_DAY, config.DAT_SQM_SENSOR_DAY)

		elif args[1] == 'sqm-week':
			generate_graph_for_week(cur, table_name, config.SQM_SENSOR_WEEK, config.DAT_SQM_SENSOR_WEEK)

		elif args[1] == 'sqm-month':
			generate_graph_for_month(cur, table_name, config.SQM_SENSOR_MONTH, config.DAT_SQM_SENSOR_MONTH)

		elif args[1] == 'sqm-year':
			generate_graph_for_year(cur, table_name, config.SQM_SENSOR_YEAR, config.DAT_SQM_SENSOR_YEAR)

	db.close()

	print 'Done\n'

if __name__ == "__main__":
    main(sys.argv)

