#!/bin/bash

# simple copy current image to archive
cp /storage/web/cam1.jpg /storage/web/archive/`date +"%d.%m.%Y"`/cam1/`date +"%H.%M__%d.%m.%Y"`.jpg
cp /storage/web/cam2.jpg /storage/web/archive/`date +"%d.%m.%Y"`/cam2/`date +"%H.%M__%d.%m.%Y"`.jpg
