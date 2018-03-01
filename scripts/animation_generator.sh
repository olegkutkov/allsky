#!/bin/bash -e

start_checker_out=`/opt/allsky/bin/ephem/start_animation_generator.py`

start_checker_exit_code=$?

echo $start_checker_exit_code

if [ ! $start_checker_exit_code -eq 0 ]; then
	echo 'Observation not started yet'
	exit 1
fi


CURRENT_DAY=`date +"%d.%m.%Y"`

WORK_IMG_DIR="/storage/web/animation_gen"
SRC_IMG_DIR="/storage/web/archive/${CURRENT_DAY}/cam1"
VIDEO_SAVE_DIR="/storage/web"

rm -fr ${WORK_IMG_DIR}/*

files=(`ls ${SRC_IMG_DIR} | sort | tail -n7`)

FILE_IDX=0

for file in "${files[@]}"
do
	cp -v ${SRC_IMG_DIR}/${file} ${WORK_IMG_DIR}/${FILE_IDX}.jpg

	FILE_IDX=$[$FILE_IDX + 1]

	cp -v ${SRC_IMG_DIR}/${file} ${WORK_IMG_DIR}/${FILE_IDX}.jpg

	FILE_IDX=$[$FILE_IDX + 1]

	cp -v ${SRC_IMG_DIR}/${file} ${WORK_IMG_DIR}/${FILE_IDX}.jpg


	FILE_IDX=$[$FILE_IDX + 1]
done

rm -f ${VIDEO_SAVE_DIR}/sky_animation_tmp.*

echo "Generating MP4 video..."

avconv -i ${WORK_IMG_DIR}/'%01d.jpg' -c:v libx264 -crf 15 -strict experimental ${VIDEO_SAVE_DIR}/sky_animation_tmp.mp4

mv -f ${VIDEO_SAVE_DIR}/sky_animation_tmp.mp4 ${VIDEO_SAVE_DIR}/sky_animation.mp4

date +"%H:%M %d.%m.%Y" > ${VIDEO_SAVE_DIR}/animation_gen_time.txt


echo "Generating WEBM video..."

avconv -i ${WORK_IMG_DIR}/'%01d.jpg' -c:v libvpx -crf 15 -b:v 1M -qscale:a 5 ${VIDEO_SAVE_DIR}/sky_animation_tmp.webm

date +"%H:%M %d.%m.%Y" > ${VIDEO_SAVE_DIR}/animation_gen_time.txt

mv -f ${VIDEO_SAVE_DIR}/sky_animation_tmp.webm ${VIDEO_SAVE_DIR}/sky_animation.webm

