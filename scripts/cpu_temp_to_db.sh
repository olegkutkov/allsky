#!/bin/bash

put_to_db()
{
	temp=$1

	echo "INSERT INTO cpu_sensor (temperature) VALUES ($temp);" | tee | mysql --connect-timeout=10 -uallsky -pallsky allsky -h 192.168.8.1
}

cpu_temp=`cat /sys/class/thermal/thermal_zone0/temp`
cpu_temp=`bc <<< "scale=2; $cpu_temp/1000"`

put_to_db $cpu_temp

