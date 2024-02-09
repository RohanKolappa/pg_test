#!/bin/sh

NET2USB=/home/onair/TV/bin/net2usb 
NET2USBSTATUS=/tmp/net2usb_status.txt
KBMCRES=/tmp/current_resolution
KBMCRESCOPY=/tmp/current_resolution_copy
FORCERES=/tmp/forceres
FORCERESCOPY=/tmp/forceres_copy

logprint() {
    logger $1
    echo $1
}

start() {

    # Make sure net2usb is not running
    killall -0 net2usb > /dev/null 2>&1
    if [ "x$?" = "x0" ]; then
	    logprint "Cannot start net2usb service. Stop the service first."
	    return
    fi
    
    boardtype=`get_board_info | grep boardtype | grep value | sed 's/.*value="\(.*\)".*/\1/'`

    if [ $boardtype == "StreamDst" ]; then
        logprint "NET2USB will not serve on Rx."
        return 
    fi

    # Ready to start the service
    logprint "Starting net2usb service..."

    # Create the status file. If any attempt is made to start the service
    # this file should always exist
    touch $NET2USBSTATUS
    touch  $KBMCRESCOPY
     
    if [ -f $FORCERES ]; then
        hres=`cat $FORCERES | cut -d' ' -f1`
        vres=`cat $FORCERES | cut -d' ' -f2`
        mv $FORCERES $FORCERESCOPY
        /bin/cp -f  $KBMCRES $KBMCRESCOPY
    else
        hres=`cat $KBMCRES | cut -d' ' -f1`
        vres=`cat $KBMCRES | cut -d' ' -f2`
        mv $KBMCRES $KBMCRESCOPY
    fi

    $NET2USB $hres $vres  > /dev/null  2&>1  &

    logprint "NET2USB service started."
    
}

stop() {
    
   logprint "Stopping NET2USB service..."

    # Just kill the daemon
   killall net2usb > /dev/null 2>&1
    
    # And remove the status file
   /bin/rm -f $NET2USBSTATUS

    if [ -f $FORCERESCOPY ]; then
        mv $FORCERESCOPY $FORCERES
    fi

}

watch() {


    # No need to monitor the daemon if service was never started
    if [ ! -f $NET2USBSTATUS ]; then
        return
    fi

    if [ -f $FORCERESCOPY ]; then
        return
    fi

    if [ -f $FORCERES ]; then
        hres=`cat $FORCERES | cut -d' ' -f1`
        vres=`cat $FORCERES | cut -d' ' -f2`
        killall net2usb > /dev/null 2>&1
        $NET2USB $hres $vres  > /dev/null  2&>1  &
        mv $FORCERES $FORCERESCOPY
        mv $KBMCRESCOPY $KBMCRES
        return
    fi


    if [ ! -f $KBMCRES ]; then
        return
    fi	

    hres=`cat $KBMCRES | cut -d' ' -f1`
    vres=`cat $KBMCRES | cut -d' ' -f2`
	

    logprint  "Resolution changed $hres X $vres $lutid $lutidcopy "

    killall net2usb > /dev/null 2>&1
    
    # And remove the status file
    /bin/rm -f $NET2USBSTATUS

    logprint "Starting net2usb service..."

    $NET2USB $hres $vres  > /dev/null  2&>1  &
    mv $KBMCRES $KBMCRESCOPY
    touch $NET2USBSTATUS

   logprint "NET2USB service started."


    # The daemon better be running
    killall -0 net2usb > /dev/null 2>&1
    if [ "x$?" != "x0" ]; then
        # net2usb  exited for some reason
       echo "Failed: net2usb failed in running ." > $NET2USBSTATUS 
       return
    fi


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
