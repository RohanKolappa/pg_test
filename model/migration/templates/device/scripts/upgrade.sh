#!/bin/sh
SCRIPTS_DIR=/etc/migration
ver=OLDSERVICEVERSION
devicexml=/etc/config/Device.xml
xsltproc -o $devicexml.tmp ${SCRIPTS_DIR}/$ver/xsl/Device.xsl $devicexml
mv $devicexml.tmp $devicexml
