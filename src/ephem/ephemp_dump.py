#!/usr/bin/python
import ephem
import datetime
import json
from datetime import timedelta
import config

###

ephem_data = {}

sunrise_tw_angle = '-4'
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

#	observer.horizon = astro_tw_angle
	observer.horizon = sunrise_tw_angle
	next_sunrise_time = ephem.localtime(observer.next_rising(sun_ephem, use_center=True))
#	prev_astro_tw_start_time = ephem.localtime(observer.previous_setting(sun_ephem, use_center=True))

	observer.horizon = civil_tw_angle

	night_start_next = ephem.localtime(observer.next_setting(sun_ephem, use_center=True))

	tdelta = (night_start_next - localtime).total_seconds()

	if tdelta > 0:
		if tdelta <= 5400:
			print 'Today night start at ', night_start_next
			astro_tw_start_time = night_start_next
		else:
			astro_tw_start_time = ephem.localtime(observer.previous_setting(sun_ephem, use_center=True))
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

print 'prev_moon_rising', prev_moon_rising
#print prev_moon_transit
#print next_moon_transit
print 'next_moon_setting', next_moon_setting

#print '#'

#print previous_full
#print next_full

names = ['Waxing Crescent', 'Waxing Gibbous',
         'Waning Gibbous', 'Waning Crescent']

ephem_data['moon_correction'] = False

moon_ephem.compute(observer)

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
			ephem_data['moon_correction'] = True
			ephem_data['moon_alt_deg'] = float(moon_ephem.alt) * 57.2957795

			ephem_data['moon_phase_name'] = names[quarter]

			moon_az = float(moon_ephem.az) * 57.2957795
	
			moon_place = ''

			if moon_az > 0 and moon_az <= 45:
				moon_place = 'North-East'
			elif moon_az > 45 and moon_az <= 90:
				moon_place = 'East'
			elif moon_az > 90 and moon_az <= 135:
				moon_place = 'South-East'
			elif moon_az > 135 and moon_az <= 180:
				moon_place = 'South'
			elif moon_az > 180 and moon_az <= 225:
				moon_place = 'South-West'
			elif moon_az > 225 and moon_az <= 270:
				moon_place = 'West'
			elif moon_az > 270 and moon_az <= 315:
				moon_place = 'North-West'
			else:
				moon_place = 'North ?'

			ephem_data['moon_place'] = moon_place

print ephem_data

with open(config.JSON_PATH, 'w') as fp:
    json.dump(ephem_data, fp)


#print next_full
#print previous_full


#sun_ephem.compute(observer)

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

