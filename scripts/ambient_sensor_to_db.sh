#!/bin/bash

SENSOR_UTIL=/opt/allsky/bin/read_tsl2561
DOME_DIMMING_FACTOR=2.28

put_to_db()
{
	sensor_data=$1

	ambient_infrared=`echo $sensor_data | awk -F ',' '{print $2}' | awk -F '=' '{print $2}'`
	ambient_lux=`echo $sensor_data | awk -F ',' '{print $3}' | awk -F '=' '{print $2}'`

	ambient_infrared=`bc <<< "$ambient_infrared*$DOME_DIMMING_FACTOR"`
	ambient_lux=`bc <<< "$ambient_lux*$DOME_DIMMING_FACTOR"`

	echo "INSERT INTO ambient_sensor (light_value, ir_value) VALUES ($ambient_lux, $ambient_infrared);" | tee | mysql --connect-timeout=10 -uallsky -pallsky allsky -h 192.168.8.1
}

sensor_data=`${SENSOR_UTIL}`

put_to_db "${sensor_data}"

