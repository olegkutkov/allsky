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

if json_data['night']:
	now = datetime.datetime.now()
	activation_time = parser.parse(json_data['night_start']) + datetime.timedelta(minutes = 15)

	if now >= activation_time:
		print 'Run animation generator'
		sys.exit(0)

print 'Suspend animation generator'
sys.exit(1)

