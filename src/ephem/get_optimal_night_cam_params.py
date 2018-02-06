#!/usr/bin/python

from dateutil import parser
import datetime
import time
import json
import config
import sys

json_data = None

with open(config.JSON_PATH, 'r') as fp:
    json_data = json.load(fp)

localtime = datetime.datetime.now()
night_start = parser.parse(json_data['night_start'])

if localtime < night_start:
	tdelta = (night_start - localtime).total_seconds()
	print tdelta
	if tdelta <= 900:
		print '-e 23000 -g 30'
	elif tdelta <= 1500:
		print '-e 18000 -g 30'
	elif tdelta <= 1800:
		print '-e 9000 -g 15'
	elif tdelta <= 2400:
		print '-e 3700 -g 15'
	elif tdelta <= 2600:
		print '-e 1200 -g 10'
	elif tdelta <= 2700:
		print '-e 1000 -g 10'
	elif tdelta >= 2700:
		print '-e 500 -g 10'
else:
	sunrise_time = parser.parse(json_data['sunrise'])
	sunrise_delta = (sunrise_time - localtime).total_seconds()

	if sunrise_delta <= 900:
		print '-e 500 -g 5'
	elif sunrise_delta <= 1500:
		print '-e 1000 -g 10'
	elif sunrise_delta <= 1800:
		print '-e 3700 -g 15'
	elif sunrise_delta <= 2400:
		print '-e 9000 -g 15'
	elif sunrise_delta <= 2700:
		print '-e 18000 -g 15'
	elif sunrise_delta <= 3000:
		print '-e 18000 -g 27'
	elif sunrise_delta >= 2000:
		if json_data['moon_correction']:
			moon_alt = json_data['moon_alt_deg']

			if moon_alt >= 36:
				print '-e 16000 -g 7'
			elif moon_alt <= 5:
				print '-e 30000 -g 30'
			elif moon_alt <= 10:
				print '-e 30000 -g 24'
			elif moon_alt <= 19:
				print '-e 24000 -g 24'
			elif moon_alt <= 25:
				print '-e 20000 -g 20'
			elif moon_alt <= 30:
				print '-e 20000 -g 15'
			elif moon_alt < 36:
				print '-e 20000 -g 10'	
		else:
			print '-e 30000 -g 30'
		

