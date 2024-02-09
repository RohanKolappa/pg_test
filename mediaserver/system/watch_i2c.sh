#!/bin/sh

# Source common library
. /etc/scripts/common.rc

logprint() {
    logger $1
    echo $1
}

start() {
    logprint "starting watch_i2c.sh"
    /bin/touch /tmp/.watch_i2c
}

stop() {
    logprint "stopping watch_i2c.sh"
    /bin/rm -f /tmp/.watch_i2c
}

watc() {

    for i in 1 2
    do
        let "j = $i - 1"
        if [ -f /tmp/reseti2c.$j ]; then
            get_board_info -channelNumber $i | grep Src > /dev/null 2>&1
            if [ $? -eq 0 ]; then
                logprint "Reloading channel $i FPGA (StreamSrc)"
                set_board_info -channelNumber $i -boardtype StreamSrc
            else 
               logprint "Reloading channel $i FPGA (StreamDst)"
               set_board_info -channelNumber $i -boardtype StreamDst
            fi
            /bin/rm -f /tmp/reseti2c.$j > /dev/null 2>&1
        fi
    done
}

case "$1" in
    start)
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

    *)
        echo "Usage: $0 [start|stop|watch|restart]"
        exit 1
esac
