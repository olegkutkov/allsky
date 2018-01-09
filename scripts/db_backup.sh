#!/bin/bash

DB_NAME="allsky"
DB_USER="allsky"
DB_PASSWORD="allsky"
DB_DIR=/storage/web/backups

datetime=$(date +'%Y-%m-%d' -d "yesterday")

rm -f ${DB_DIR}/*

TARGET_FILE=${DB_DIR}/${DB_NAME}_$datetime.sql.gz

echo "Dumping db to " ${TARGET_FILE}

mysqldump -v -u ${DB_USER} --password=${DB_PASSWORD} ${DB_NAME} | gzip -9 > ${TARGET_FILE}

