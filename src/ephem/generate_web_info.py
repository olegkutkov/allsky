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
				+ ' ' + time.tzname[0]

	if json_data['moon_correction']:
		moon_angle = json_data['moon_alt_deg']

		if moon_angle > 1:
			moon_alt_str = str(round(moon_angle, 2))

			web_text += '<br><strong>Bright Moon at ' + moon_alt_str + '&deg over the horizon on ' + json_data['moon_place'] + '.'

			if moon_angle > 5:
				web_text += ' Camera images may be overexposed.</strong></div>'
			else:
				web_text += '</strong></div>'
		else:
			web_text += '</div>'
	else:
		web_text += '</div>'
else:
	web_text = '<div class="twilight_grad_day"><div><strong>Recommended time for shooting flat fields: </strong>' \
					+ parser.parse(json_data['civil_twilight_start']).strftime("%H:%M") \
					+ ' ' + time.tzname[0] + '</div><div><strong>Reccomended time to start observations: </strong>' \
					+ parser.parse(json_data['astro_twilight_start']).strftime("%H:%M") + ' ' + time.tzname[0] + '</div></div>'

print web_text

with open(config.WEB_OUT_FILE, 'w') as web_out_file:
	web_out_file.write(web_text)

