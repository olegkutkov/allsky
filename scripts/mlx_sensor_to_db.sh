#!/bin/bash

put_to_db()
{
	sensor_data=$1

	skytemp=`echo $sensor_data | awk '{print $3}'`

	echo "INSERT INTO cloud_sensor (ir_value) VALUES ($skytemp);" | tee | mysql -uallsky -pallsky allsky
}

sensor_data=`/opt/allsky/bin/read_mlx90614 --bus 1 --i2c_addr 0x2a -i`

put_to_db "${sensor_data}"

