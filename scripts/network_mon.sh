#!/bin/bash

LOGFILE=/var/log/network_mon.log
TCFILE=/tmp/tc

if ifconfig eth0 | grep -q "inet addr:" ;
then
		if [ -e ${TCFILE} ] ## This checks for the exisitence of a file call 'tc', if not there then leave the script
		then
			rm ${TCFILE} ## If the file is there, remove it to prevent more "up" messages
			echo "$(date "+%m %d %Y %T") : Ethernet OK" >> $LOGFILE
		fi
else
		touch ${TCFILE}

		echo "$(date "+%m %d %Y %T") : Ethernet connection down! Attempting reconnection." >> $LOGFILE

		ifup --force eth0

		OUT=$? #save exit status of last command to decide what to do next

		if [ $OUT -eq 0 ] ; then
				STATE=$(ifconfig eth0 | grep "inet addr:")
				echo "$(date "+%m %d %Y %T") : Network connection reset. Current state is" $STATE >> $LOGFILE
		else
				echo "$(date "+%m %d %Y %T") : Failed to reset ethernet connection" >> $LOGFILE
		fi
fi
