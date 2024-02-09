#!/bin/sh
SCRIPTS_DIR=/etc/migration
ver=1.0
devicexml=/etc/config/Device.xml
if [ -f /bin/tw_cli ]; then
	capacity=`tw_cli info c0 p0 capacity | sed -e 's/\(.*\)= \(.*\)\.\(.*\)/\2/'`
	if [ $capacity -gt 900 ]; then
		devname="DMS-1500-Device"
	elif [ $capacity -gt 600 ]; then
		devname="DMS-200-Device"
	elif [ $capacity -gt 300 ]; then
		devname="DMS-1200-Device"
	else
		devname="DMS-100-Device"
	fi
else
	devname=`xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml`
fi
xsltproc -o $devicexml.tmp --stringparam devname $devname ${SCRIPTS_DIR}/$ver/xsl/Device.xsl $devicexml
mv $devicexml.tmp $devicexml
