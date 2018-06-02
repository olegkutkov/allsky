#!/bin/bash

CAM1_CHECK_RUN_FILE="/storage/ephemdata/cam1_blackbox"
CAM1_LAST_IMAGE="/storage/fits/last.fits"
CAM1_PREV_IMAGE="/storage/fits/prev.fits"
CAM1_DARK_IMAGE="/storage/fits/dark.fits"
CAM1_BIAS_IMAGE="/storage/fits/bias.fits"

CAM1_TMP_ROTATED="/storage/web/cam1_tmp_rotated.tiff"

FITS_HEADER_DATA_FILE='/storage/fits/fits_header.dat'

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
		rm -f $CAM1_CHECK_RUN_FILE
	fi

	optimal_shooting_params=`/opt/allsky/bin/ephem/get_optimal_night_cam_params.py`

	if [ ! -f ${CAM1_DARK_IMAGE} ]; then
		echo "Shooting dark frame "${CAM1_DARK_IMAGE}

		/opt/allsky/bin/iris_control c

		/opt/allsky/bin/qhy_camera_new -m "qhy5ii" ${optimal_shooting_params} -o ${CAM1_DARK_IMAGE}
	fi

	if [ ! -f ${CAM1_BIAS_IMAGE} ]; then
		echo "Shooting bias frame "${CAM1_BIAS_IMAGE}

		bias_gain=`echo ${optimal_shooting_params} | awk '{print $3}'`

		/opt/allsky/bin/iris_control c

		/opt/allsky/bin/qhy_camera_new -m "qhy5ii" -e 1 -g ${bias_gain} -o ${CAM1_BIAS_IMAGE}
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

	rm ${CAM1_DARK_IMAGE}

	rm ${CAM1_BIAS_IMAGE}

	exit 1
fi

optimal_shooting_params=`/opt/allsky/bin/ephem/get_optimal_night_cam_params.py`

if test "`find ${CAM1_DARK_IMAGE} -mmin +60`"; then
	echo "Calibration frames is to old, renew!"

	rm -f ${CAM1_DARK_IMAGE}
	rm -f ${CAM1_BIAS_IMAGE}

	/opt/allsky/bin/iris_control c

	echo "Shooting new dark frame"

	/opt/allsky/bin/qhy_camera_new -m "qhy5ii" ${optimal_shooting_params} -o ${CAM1_DARK_IMAGE}

	bias_gain=`echo ${optimal_shooting_params} | awk '{print $3}'`

	echo -e "\nShooting new bias frame"

	/opt/allsky/bin/qhy_camera_new -m "qhy5ii" -e 1 -g ${bias_gain} -o ${CAM1_BIAS_IMAGE}

	/opt/allsky/bin/iris_control o
fi


last_ambient_lux=`echo "select * from ambient_sensor order by time desc limit 1;" | mysql --connect-timeout=10 -uallsky -pallsky allsky -h 192.168.8.1 | tail -n1 | awk '{print $3}'`
last_ambient_lux=`printf '%.2f\n' ${last_ambient_lux}`
last_ambient_lux_for_iso=${last_ambient_lux%.*}

last_temp_humidity=`echo "select * from external_dh22 order by time desc limit 1;" | mysql --connect-timeout=10 -uallsky -pallsky allsky -h 192.168.8.1 | tail -n1`

last_temp=`echo ${last_temp_humidity} | awk '{print $3}'`
last_temp=`printf '%.2f\n' ${last_temp}`

last_humidity=`echo ${last_temp_humidity} | awk '{print $4}'`
last_humidity=`printf '%.2f\n' ${last_humidity}`

current_date_time=`date +"%d.%m.%Y %H:%M" `

last_skytemp=`echo "select * from cloud_sensor order by time desc limit 1;" | mysql --connect-timeout=10 -uallsky -pallsky allsky -h 192.168.8.1 | tail -n1 | awk '{print $3}'`
last_skytemp=`bc <<< "scale=2; ${last_skytemp}/1"`
last_skytemp=`printf "%.2f\n" ${last_skytemp}`

echo -e "\nCurrent environment conditions:"
echo "    Temperature: "${last_temp}
echo "    Humidity: "${last_humidity}
echo "    Sky temperature: "${last_skytemp}

rm -f /storage/web/cam1_tmp.jpg

echo -e "\nSelected camera exposure and gain: "${optimal_shooting_params}""

rm -f ${FITS_HEADER_DATA_FILE}

echo "Building FITS header data file "${FITS_HEADER_DATA_FILE}

echo -e "CREATOR Allsky camera" > ${FITS_HEADER_DATA_FILE}
echo -e "INSTRUME QHY5-IIM" >> ${FITS_HEADER_DATA_FILE}

exposure_r=`echo ${optimal_shooting_params} | awk '{print $1}'`
exposure_hr=`printf "%.1f\n" "$(bc <<< "scale=1; ${exposure_r}/1000")"`

echo -e "EXPTIME "${exposure_hr} >> ${FITS_HEADER_DATA_FILE}
echo -e "TEMPERAT "${last_temp} >> ${FITS_HEADER_DATA_FILE}
echo -e "HUMIDITY "${last_humidity} >> ${FITS_HEADER_DATA_FILE}
echo -e "SKYTEMP "${last_skytemp} >> ${FITS_HEADER_DATA_FILE}
echo -e "OBSERVAT Crimean astrophysical observatory" >> ${FITS_HEADER_DATA_FILE}
echo -e "SITENAME Nauchniy, Crimea" >> ${FITS_HEADER_DATA_FILE}
echo -e "SITELAT 44.727007" >> ${FITS_HEADER_DATA_FILE}
echo -e "SITELONG 34.013173" >> ${FITS_HEADER_DATA_FILE}
echo -e "SITEELEV 600" >> ${FITS_HEADER_DATA_FILE}

echo "Ok, file content:"
cat ${FITS_HEADER_DATA_FILE}

echo -e "\nCleanup..."
mv -f ${CAM1_LAST_IMAGE}  ${CAM1_PREV_IMAGE}

echo -e "\nStarting camera utility..."

/opt/allsky/bin/qhy_camera_new -m "qhy5ii" ${optimal_shooting_params} -o ${CAM1_LAST_IMAGE} -b ${CAM1_BIAS_IMAGE} -k ${CAM1_DARK_IMAGE}  -x ${FITS_HEADER_DATA_FILE}

echo -e "\nRotating original image on 180 degree and applying SNWE overlay..."

convert ${CAM1_LAST_IMAGE} \( +flop \) \( -fill white -gravity West -pointsize 21 -annotate +5-220 'N' -gravity North -pointsize 21 \
	-annotate +195+5 'W' -gravity East -pointsize 21 -annotate +5+190 'S' \) ${CAM1_TMP_ROTATED}

echo "Rotated image was saved as "${CAM1_TMP_ROTATED}

echo "Applying weather overlay text and converting to jpg..."

convert -background '#00000080' -fill white -size 1280x50 label:"Nauchniy - CAM1\nDate: ${current_date_time}  Temperature: ${last_temp} C  Humidity: ${last_humidity} %  Skytemp: ${last_skytemp} C"\
	-gravity southwest ${CAM1_TMP_ROTATED} +swap -gravity south -set colorspace Gray -composite -quality 85% /storage/web/cam1_woverlay.jpg

echo "Moving the image to web interface"

mv /storage/web/cam1_woverlay.jpg /storage/web/cam1.jpg

echo "Cleanup..."

rm ${CAM1_TMP_ROTATED}

echo ${CAM1_TMP_ROTATED}" was removed"

echo "Archiving camera image"

/opt/allsky/bin/cam_to_arch.sh cam1

