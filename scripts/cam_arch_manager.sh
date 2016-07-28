#!/bin/bash

#
# Allsky camera simple archive manager
#  - create new directory for next day
#  - cleanup old files and directories
#
#

####

next_date=`date +"%d.%m.%Y" --date="next day"`

archdir=/storage/web/archive
archpath=${archdir}/${next_date}


# Cleanup old directories

arch_dirs=`ls /storage/web/archive/`

for dir in ${arch_dirs}
do
	echo "Checking archive directory "${dir}

	dir_name_to_date=`echo ${dir} | awk -F "." '{print $3"-"$2"-"$1}'`

	days_between_days=`date -d @$(( $(date +%s) - $(date -d "$dir_name_to_date" +%s) )) -u +'%d'`

	if [ $days_between_days -le 0 ]; then
		continue
	fi

	if [ $days_between_days -ge 4 ]
	then
		echo ${dir}" is "$days_between_days" days old and will be removed"

		rm -fr ${archdir}/${dir}
	fi

	echo -e "\n"

done

#  Create directory for next day

if [ ! -d ${archpath} ]
then
	echo "Creating archive directory " ${archpath}
	mkdir -p ${archpath}
fi

