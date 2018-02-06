#!/bin/bash

datetime=`date`

EXEC_DIR='/opt/allsky/bin/webdatagen'
SRC_DIR='/storage/webdata'
DST_DIR='/storage/web'

echo 'Webdata generator started at '$datetime

case $1 in
	textdata)
		python ${EXEC_DIR}/web_textdata_gen.py
		cp -fv $SRC_DIR/*txt $DST_DIR/
	;;

	sensors-day)
		python ${EXEC_DIR}/sensors-graphgen.py $1
		cp -fv $SRC_DIR/*day* $DST_DIR/
	;;

	sensors-week)
		python ${EXEC_DIR}/sensors-graphgen.py $1
		cp -fv $SRC_DIR/*week* $DST_DIR/
	;;

	sensors-month)
		python ${EXEC_DIR}/sensors-graphgen.py $1
		cp -fv $SRC_DIR/*month* $DST_DIR/
	;;

	sensors-year)
		python ${EXEC_DIR}/sensors-graphgen.py $1
		cp -fv $SRC_DIR/*year* $DST_DIR/
	;;

	system-report-sensors)
		python ${EXEC_DIR}/system-sensors.py
		cp -fv $SRC_DIR/*cpu* $DST_DIR/
		cp -fv $SRC_DIR/*internal* $DST_DIR/
		cp -fv $SRC_DIR/*disk* $DST_DIR/
	;;

	system-report)
		python ${EXEC_DIR}/system-report.py
		cp -fv $SRC_DIR/*system* $DST_DIR/
	;;

	sky-ambient)
		python ${EXEC_DIR}/sky-ambient-datagen.py
		cp -fv $SRC_DIR/sky* $DST_DIR/
	;;
esac

exit 0

