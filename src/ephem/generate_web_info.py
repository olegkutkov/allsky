#!/usr/bin/python
import datetime
from dateutil import parser
import time
import json

import config

json_data = None

with open(config.JSON_PATH, 'r') as fp:
    json_data = json.load(fp)

web_text = ''

#dt = parser.parse(json_data['civil_twilight_start'])

#print dt.strftime("%H:%M")

#print dt + datetime.timedelta(minutes = 10)

if json_data['night']:
	web_text = '<div class="twilight_grad_night"><strong>Sunrise at:</strong> ' + parser.parse(json_data['sunrise']).strftime("%H:%M") \
				+ ' ' + time.tzname[0] + '</div>'
else:
	web_text = '<div class="twilight_grad_day"><div><strong>Recommended time for shooting flat fields: </strong>' \
					+ parser.parse(json_data['civil_twilight_start']).strftime("%H:%M") \
					+ ' ' + time.tzname[0] + '</div><div><strong>Reccomended time to start observations: </strong>' \
					+ parser.parse(json_data['astro_twilight_start']).strftime("%H:%M") + ' ' + time.tzname[0] + '</div></div>'

print web_text

with open(config.WEB_OUT_FILE, 'w') as web_out_file:
	web_out_file.write(web_text)

