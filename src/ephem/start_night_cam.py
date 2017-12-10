#!/usr/bin/python

from dateutil import parser
import time
import json
import config
import sys

json_data = None

#with open(config.JSON_PATH, 'r') as fp:
#    json_data = json.load(fp)

json_data = json.loads('{"night_start": "2017-11-30 18:49:32.000003", "sunrise": "2017-12-01 06:16:24.000004", "night": true}')

#{'night_start': '2017-11-30 18:49:32.000003', 'sunrise': '2017-12-01 06:16:24.000004', 'night': True}

if not json_data['night']:
	activation_time = parser.parse(json_data['astro_twilight_start']).strftime("%H:%M") \
		+ ' ' + time.tzname[0]
	print 'Camera will be activated after', activation_time
	sys.exit(1)

sys.exit(0)

