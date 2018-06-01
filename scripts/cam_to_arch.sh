#!/bin/bash

CAM_NAME=$1

FILE_EXTENSION=""
ORIGINAL_FILE=""

if [ "$CAM_NAME" = "cam1" ]; then
	FILE_EXTENSION="fits"
	ORIGINAL_FILE="/storage/fits/last.fits"
else
	FILE_EXTENSION=".jpg"
	ORIGINAL_FILE="/storage/web/"${CAM_NAME}".jpg"
fi

NEW_FILE_NAME=`date +"%Y.%m.%dT%H.%M"`.${FILE_EXTENSION}
DATE=`date +"%d.%m.%Y"`

echo "Copying "${ORIGINAL_FILE} " to "/storage/web/${NEW_FILE_NAME}

cp -v ${ORIGINAL_FILE} /storage/web/${NEW_FILE_NAME}

echo "Upload FTP "

ftp-upload -v -h 192.168.8.1 -u observer --password observer -d allsky/cam_img/${DATE}/${CAM_NAME} /storage/web/${NEW_FILE_NAME}

rm /storage/web/${NEW_FILE_NAME}
