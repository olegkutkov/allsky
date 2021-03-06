#!/bin/bash

put_to_db()
{
	temp=`echo $1 | awk '{print $1}' | awk -F '=' '{print $2}'`
	humidity=`echo $1 | awk '{print $2}' | awk -F '=' '{print $2}'`

	if [ -z "$temp" ]; then
		echo "Failed to get temperature! Sensor #"$2
		exit 1
	fi

	if [ -z "$humidity" ]; then
		echo "Failed to get humidity! Sensor #"$2
		exit 1
	fi

	echo "INSERT INTO ${3} (temperature, humidity) VALUES ($temp, $humidity);" | tee | mysql -uallsky -pallsky allsky
}

external_sensor=`/opt/allsky/bin/read_dht 17`
internal_sensor=`/opt/allsky/bin/read_dht 4`

put_to_db "${external_sensor}" 17 "external_dh22"
put_to_db "${internal_sensor}" 4 "internal_dh22"

