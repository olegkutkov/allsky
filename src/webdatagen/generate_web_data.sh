#!/bin/bash

datetime=`date`

SRC_DIR='/storage/webdata'
DST_DIR='/storage/web'

echo 'Webdata generator started at '$datetime

python /opt/allsky/bin/webdatagen/web_textdata_gen.py

cp -fv $SRC_DIR/*txt $DST_DIR/

python /opt/allsky/bin/webdatagen/sensors-graphgen.py
python /opt/allsky/bin/webdatagen/system-report.py
python /opt/allsky/bin/webdatagen/system-sensors.py

echo 'Copying all generated content'

cp -fv $SRC_DIR/* $DST_DIR/

