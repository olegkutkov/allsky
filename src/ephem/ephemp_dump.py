#!/usr/bin/python
import ephem
import datetime
import json
from datetime import timedelta
import config

###

ephem_data = {}


civil_tw_angle = '-6'
astro_tw_angle = '-18'

localtime = datetime.datetime.now()
utctime = datetime.datetime.utcnow()

localtime_str = localtime.strftime("%Y-%m-%d %H:%M:%S")
utctime_str = utctime.strftime("%Y-%m-%d %H:%M:%S")

print 'Local date and time: ', localtime_str
print 'UTC date and time: ', utctime_str

observer = ephem.Observer()

observer.lat = '44.727015'
observer.lon = '34.013317'
observer.elevation = 588

observer.date = utctime

sun_ephem = ephem.Sun()
moon_ephem = ephem.Moon()

is_night = None
time_of_day_str = ''

next_civil_tw_start_time = None
next_astro_tw_start_time = None
astro_tw_start_time = None
next_sunrise_time = None

if observer.previous_rising(sun_ephem) > observer.previous_setting(sun_ephem):
	is_night = False
	time_of_day_str = 'Day'

	observer.horizon = civil_tw_angle
	next_civil_tw_start_time = ephem.localtime(observer.next_setting(sun_ephem, use_center=True))

	observer.horizon = astro_tw_angle
	next_astro_tw_start_time = ephem.localtime(observer.next_setting(sun_ephem, use_center=True))
else:
	is_night = True
	time_of_day_str = 'Night'

	observer.horizon = astro_tw_angle
	next_sunrise_time = ephem.localtime(observer.next_rising(sun_ephem, use_center=True))
#	prev_astro_tw_start_time = ephem.localtime(observer.previous_setting(sun_ephem, use_center=True))

	night_start_next = ephem.localtime(observer.next_setting(sun_ephem, use_center=True))

	tdelta = (night_start_next - localtime).total_seconds()

	if tdelta > 0:
		if tdelta <= 5400:
			print 'Today night start at ', night_start_next
			astro_tw_start_time = night_start_next
	else:
		astro_tw_start_time = ephem.localtime(observer.previous_setting(sun_ephem, use_center=True))
			

if is_night:
	print 'Night start', astro_tw_start_time.strftime("%H:%M:%S")
	print 'Sunrise at', next_sunrise_time.strftime("%H:%M:%S")

	ephem_data['night'] = is_night
	ephem_data['night_start'] = str(astro_tw_start_time)
	ephem_data['sunrise'] = str(next_sunrise_time)
else:
	print 'twilight at:', next_civil_tw_start_time.strftime("%H:%M:%S")
	print 'Night at:', next_astro_tw_start_time.strftime("%H:%M:%S")

	ephem_data['night'] = is_night
	ephem_data['civil_twilight_start'] = str(next_civil_tw_start_time)
	ephem_data['astro_twilight_start'] = str(next_astro_tw_start_time)

print ephem_data

with open(config.JSON_PATH, 'w') as fp:
    json.dump(ephem_data, fp)

## moon

observer.horizon = '-0:34'

#next_full = ephem.localtime(ephem.next_full_moon(observer.date))
#previous_full = ephem.localtime(ephem.previous_full_moon(observer.date))

prev_moon_transit = ephem.localtime(observer.previous_transit(moon_ephem))
prev_moon_rising = ephem.localtime(observer.previous_rising(moon_ephem))
next_moon_transit = ephem.localtime(observer.next_transit(moon_ephem))
next_moon_setting = ephem.localtime(observer.next_setting(moon_ephem))

previous_full = ephem.localtime(ephem.previous_full_moon(observer.date))
next_full = ephem.localtime(ephem.next_full_moon(observer.date))

#print prev_moon_rising
#print prev_moon_transit
print next_moon_transit
#print next_moon_setting

print '#'

#print previous_full
#print next_full

names = ['Waxing Crescent', 'Waxing Gibbous',
         'Waning Gibbous', 'Waning Crescent']

if is_night:
	if localtime > prev_moon_rising and localtime < next_moon_setting:
		print 'Moon on the sky during observations'

		sunlon = ephem.Ecliptic(sun_ephem).lon
		moonlon = ephem.Ecliptic(moon_ephem).lon

		tau = 2.0 * ephem.pi

		angle = (moonlon - sunlon) % tau
		quarter = int(angle * 4.0 // tau)

		if quarter == 1 or quarter == 2:
			print 'Moon is to bright'

		sunlon = ephem.Ecliptic(sun_ephem).lon
		moonlon = ephem.Ecliptic(moon_ephem).lon

		tau = 2.0 * ephem.pi


#print next_full
#print previous_full


#sun_ephem.compute(observer)
#moon_ephem.compute(observer)

#print ephem.localtime(observer.next_rising(moon_ephem, use_center=True))


#sunlon = ephem.Ecliptic(sun_ephem).lon
#moonlon = ephem.Ecliptic(moon_ephem).lon

#tau = 2.0 * ephem.pi

#angle = (moonlon - sunlon) % tau
#quarter = int(angle * 4.0 // tau)

#names = ['Waxing Crescent', 'Waxing Gibbous',
#         'Waning Gibbous', 'Waning Crescent']

#print names[quarter]

#print 'moon', ephem.localtime(observer.previous_setting(ephem.Moon()))

