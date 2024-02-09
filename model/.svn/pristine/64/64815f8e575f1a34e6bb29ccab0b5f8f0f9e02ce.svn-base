#!/bin/sh
SCRIPTS_DIR=/etc/migration
ver=2.30.1
devicexml=/etc/config/Device.xml
xsltproc -o $devicexml.tmp ${SCRIPTS_DIR}/$ver/xsl/Device.xsl $devicexml
mv $devicexml.tmp $devicexml
