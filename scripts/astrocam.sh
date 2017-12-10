#!/bin/bash

CAM1_CHECK_RUN_FILE='/storage/ephemdata/cam1_blackbox'

process=`ps ax | grep qhy_camera | grep -v grep | wc -l`

if [ $process -eq 0 ]; then
	echo "Run astrocamera script"
else
	echo "Already running, exiting..."
	exit 1
fi

start_checker_out=`/opt/allsky/bin/ephem/start_night_cam.py`
start_checker_exit_code=$?

if [ $start_checker_exit_code -eq 0 ]; then
	if [ -f ${CAM1_CHECK_RUN_FILE} ]; then

		/opt/allsky/bin/iris_control c

		/opt/allsky/bin/qhy_camera -m "qhy5ii" -e 30000 -g 30 -o /storage/web/dark.jpg

		rm -f $CAM1_CHECK_RUN_FILE
	fi

	/opt/allsky/bin/iris_control o

else
	if [ -f ${CAM1_CHECK_RUN_FILE} ]; then
		exit 1
	fi

	/opt/allsky/bin/iris_control c

	convert /opt/allsky/etc/black_box.jpg -background '#383838' -gravity Center -fill white \
			-pointsize 35 -annotate 0 "${start_checker_out}" /storage/web/cam1_tmp.jpg

	mv /storage/web/cam1_tmp.jpg /storage/web/cam1.jpg

	touch $CAM1_CHECK_RUN_FILE

	exit 1
fi


last_ambient_lux=`echo "select * from ambient_sensor order by time desc limit 1;" | mysql -uallsky -pallsky allsky | tail -n1 | awk '{print $3}'`
last_ambient_lux=`bc <<< "scale=2; ${last_ambient_lux}/1"`
last_ambient_lux_for_iso=${last_ambient_lux%.*}

last_temp_humidity=`echo "select * from external_dh22 order by time desc limit 1;" | mysql -uallsky -pallsky allsky | tail -n1`

last_temp=`echo ${last_temp_humidity} | awk '{print $3}'`
last_temp=`bc <<< "scale=2; ${last_temp}/1"`

last_humidity=`echo ${last_temp_humidity} | awk '{print $4}'`
last_humidity=`bc <<< "scale=2; ${last_humidity}/1"`

current_date_time=`date +"%d.%m.%Y %H:%M" `

last_skytemp=`echo "select * from cloud_sensor order by time desc limit 1;" | mysql -uallsky -pallsky allsky | tail -n1 | awk '{print $3}'`
last_skytemp=`bc <<< "scale=2; ${last_skytemp}/1"`

rm -f /storage/web/cam1_tmp.jpg

#-e 30000 -g 25

optimal_shooting_params=`/opt/allsky/bin/ephem/get_optimal_night_cam_params.py`

/opt/allsky/bin/qhy_camera -m "qhy5ii" ${optimal_shooting_params} -o /storage/web/cam1_tmp.jpg -k /storage/web/dark.jpg
#/opt/allsky/bin/qhy_camera -m "qhy5ii" -e 30000 -g 30 -o /storage/web/cam1_tmp.jpg -k /storage/web/dark.jpg  #-k /storage/web/bias.jpg #-k /storage/web/dark.jpg -b 5
#/opt/allsky/bin/qhy_camera -m "qhy5ii" -e 300 -g 5 -o /storage/web/cam1_tmp.jpg -k /storage/web/dark.jpg
#/opt/allsky/bin/qhy_camera -m "qhy5ii" -e 1000 -g 10 -o /storage/web/cam1_tmp.jpg -k /storage/web/dark.jpg

convert -background '#00000080' -fill white -size 1280x50 label:"Nauchniy - CAM1\nDate: ${current_date_time}  Temperature: ${last_temp} C  Humidity: ${last_humidity} %  Skytemp: ${last_skytemp} C"\
	-gravity southwest /storage/web/cam1_tmp.jpg +swap -gravity south -set colorspace Gray -composite /storage/web/cam1_woverlay.jpg

mv /storage/web/cam1_woverlay.jpg /storage/web/cam1.jpg

/opt/allsky/bin/cam_to_arch.sh cam1

