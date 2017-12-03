#!/bin/bash

CAM_NAME=$1

# simple copy current image to archive
cp -v /storage/web/${CAM_NAME}.jpg /storage/web/archive/`date +"%d.%m.%Y"`/${CAM_NAME}/`date +"%H.%M__%d.%m.%Y"`.jpg
cp -v /storage/web/${CAM_NAME}.jpg /storage/web/archive/`date +"%d.%m.%Y"`/${CAM_NAME}/`date +"%H.%M__%d.%m.%Y"`.jpg
