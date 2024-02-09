#!/bin/sh 

# Source common library
. /etc/scripts/common.rc

TW_CLI_ALERT_FILE=/tmp/3ware.alert
TW_CLI_CMD=/bin/tw_cli


start() {
	if [ ! -f ${TW_CLI_ALERT_FILE} ]; then
		${TW_CLI_CMD} /c0 show alarms > ${TW_CLI_ALERT_FILE}
		logprint "Starting HDD Monitor"
	fi
}

stop() {
	/bin/rm ${TW_CLI_ALERT_FILE}
	logprint "Stopping HDD Monitor"
}


watch() {
	${TW_CLI_CMD} /c0 show alarms > ${TW_CLI_ALERT_FILE}.new
	diff -q  ${TW_CLI_ALERT_FILE}.new ${TW_CLI_ALERT_FILE}
	if [ x"$?" != "x0" ]; then
		/bin/mv ${TW_CLI_ALERT_FILE}.new ${TW_CLI_ALERT_FILE}
		logprint "Alert HDD Monitor..."
		/V2O/bin/get_disk_status -publish
	fi
}

case "$1" in
  start)
        start
        ;;

  forcestart)
        start
        ;;

  watch)
        watch
        ;;

  stop)
        stop
        ;;

  restart)
        stop
        start
        ;;

  hup)
        hup
        ;;

  *)
        echo "Usage: $0 [start|stop|watch|restart|hup]"
        exit 1

esac
