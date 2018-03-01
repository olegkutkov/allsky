#!/bin/bash

SRC_DIR="/home/oleg/project/allsky/web"

cp -v ${SRC_DIR}/*html /storage/web
cp -v ${SRC_DIR}/*tpl /storage/web

cp -vfr ${SRC_DIR}/css /storage/web
cp -vfr ${SRC_DIR}/js /storage/web
cp -vfr ${SRC_DIR}/fonts /storage/web

cp -v ${SRC_DIR}/allsky_cameras2.jpg /storage/web
cp -v ${SRC_DIR}/allsky_cameras.jpg /storage/web
cp -v ${SRC_DIR}/allsky_overview_alpha_8.jpg /storage/web
cp -v ${SRC_DIR}/android-chrome-192x192.png /storage/web
cp -v ${SRC_DIR}/android-chrome-256x256.png /storage/web
cp -v ${SRC_DIR}/apple-touch-icon.png /storage/web
cp -v ${SRC_DIR}/favicon-16x16.png /storage/web
cp -v ${SRC_DIR}/favicon-32x32.png /storage/web
cp -v ${SRC_DIR}/favicon.ico /storage/web
cp -v ${SRC_DIR}/mstile-150x150.png /storage/web
cp -v ${SRC_DIR}/safari-pinned-tab.svg /storage/web
cp -v ${SRC_DIR}/yt_logo_mono_light.png /storage/web
cp -v ${SRC_DIR}/th_1.png /storage/web
cp -v ${SRC_DIR}/th_2.png /storage/web
cp -v ${SRC_DIR}/mlx_1.png /storage/web
cp -v ${SRC_DIR}/light_sensor.png /storage/web

cp -v ${SRC_DIR}/manifest.json /storage/web
cp -v ${SRC_DIR}/browserconfig.xml /storage/web

/opt/allsky/bin/ephem/ephemp_dump.py && /opt/allsky/bin/ephem/generate_web_info.py
/opt/allsky/bin/webdatagen/generate_web_data.sh textdata > /storage/webdata/webdatagen_text_last.log 2>&1
/opt/allsky/bin/ephem/ephemp_dump.py && /opt/allsky/bin/ephem/generate_web_info.py

/opt/allsky/bin/webdatagen/generate_web_data.sh sensors-day > /storage/webdata/webdatagen_day_last.log 2>&1
/opt/allsky/bin/webdatagen/generate_web_data.sh sensors-week > /storage/webdata/webdatagen_week_last.log 2>&1
/opt/allsky/bin/webdatagen/generate_web_data.sh sensors-month > /storage/webdata/webdatagen_month_last.log 2>&1
/opt/allsky/bin/webdatagen/generate_web_data.sh sensors-year > /storage/webdata/webdatagen_year_last.log 2>&1
/opt/allsky/bin/webdatagen/generate_web_data.sh sky-ambient > /storage/webdata/webdatagen_sky-ambient_last.log 2>&1

/opt/allsky/bin/webdatagen/generate_web_data.sh system-report-sensors > /storage/webdata/webdatagen_system_sensors_last.log 2>&1
/opt/allsky/bin/webdatagen/generate_web_data.sh system-report > /storage/webdata/webdatagen_system_all_last.log 2>&1

