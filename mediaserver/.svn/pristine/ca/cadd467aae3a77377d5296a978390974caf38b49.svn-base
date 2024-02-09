#!/bin/sh

# Source common library
. /etc/scripts/common.rc

logprint "Starting Base Services..."

# Start syslogd
/usr/bin/mkfifo /tmp/watchdogpipe
/usr/bin/mkfifo /tmp/alertpipe
/usr/bin/mkfifo /tmp/statpipe
/usr/bin/mkfifo /tmp/debugpipe
/bin/touch /var/log/persistmessages
/bin/chmod 644 /var/log/persistmessages
/bin/touch /var/log/messages
/bin/chmod 644 /var/log/messages

if [  -f /etc/config/arm_system ] ; then
    /usr/bin/mkfifo /tmp/keyboardfifofd
    /usr/bin/mkfifo /tmp/mouseabsfd
fi

# Start syslog and kologd sending all kernel messages to syslog 
# and hiding "info" and "debug" messages sent to console
if [ ! -f /etc/config/arm_system ] ; then
    /sbin/rsyslogd -r -f /etc/syslog.conf
    /sbin/rklogd -c 6 -x
else
    /usr/sbin/rsyslogd -r514 -f/etc/rsyslog.conf
    #/sbin/klogd -c 6
fi

#Change permission on /dev/urandom so the httpd server running as other than
# root can retrieve random numbers from /dev/urandom. This is used by the
# bweb local UI to create a seed for SHA1 hashkey generation related to 
# local UI session management and authentication.
chmod o+r /dev/urandom

# Start crond(skip this on the XPi for now)
/bin/mkdir -p /var/spool/cron/crontabs
/usr/sbin/crond

# Start sshd 
if [ ! -f /etc/config/arm_system ] ; then
    /sbin/sshd -f /etc/ssh/sshd_config
fi

# Start Busybox hardware watchdog 
if [ ! -f /etc/config/arm_system ] ; then
    /sbin/watchdog -t 10 /dev/watchdog
fi

# Start the dbus daemon
mkdir /var/run/dbus
if [ -f /bin/dbus-daemon ]; then
    /bin/dbus-daemon --system
else
    /usr/bin/dbus-daemon --system
fi

# Start the avahi daemon
if [ ! -f /etc/config/arm_system ] ; then
    # We suffux avahi host name with a random string to reduce collisions
    AVAHI_HOST_NAME=device-`tr -dc a-z < /dev/urandom | head -c5`
    /bin/sed -i -e "s/#host-name=foo/host-name=${AVAHI_HOST_NAME}/" /etc/avahi/avahi-daemon.conf 
    /sbin/avahi-daemon --no-chroot -D
else
    # On XPi setup avahi-autoipd
    /bin/mkdir -p /tmp/avahi-autoipd
    /bin/ln -s /usr/sbin/avahi-autoipd /sbin/avahi-autoipd

    # start avahi-daemon
    /usr/sbin/avahi-daemon  -D

fi

# Retrieve Serial Number of the device
${SCRIPTS}/retrieve_serial_number.sh


# OS OK
/usr/bin/logger -p local6.notice "${OSOK} : OS OK"
echo ${OSOK} > ${LEDFILE}

