#!/bin/bash

datetime=`date`

SRC_DIR='/storage/webdata'
DST_DIR='/storage/web'

echo 'Webdata generator started at '$datetime

echo 'Cleanup first...'

rm -f $SRC_DIR/*

python ./sensors-graphgen.py
python ./system-report.py
python ./system-sensors.py

echo 'Copying all generated content'

cp -fv $SRC_DIR/* $DST_DIR/

