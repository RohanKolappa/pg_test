#!/bin/sh

# This file is installed as the file linux kernel will
# call when it wants to poweroff the device upon critical
# temperature trip point. 

# Currently we just log the message. A user daemon will 
# independently monitor system temperature and take
# appropriate actions.

logger "Critical temperature reached."

log_file=/tmp/critical_temperature_trips.log

/bin/touch ${log_file} 

tail -100 ${log_file} > ${log_file}.tmp
echo  `date` >> ${log_file}.tmp
/bin/mv ${log_file}.tmp ${log_file}

