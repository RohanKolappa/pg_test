#!/bin/sh

TEMPLATEDIR=./templates
DEFAULTDIR=./src/com/ipvs/xsd/defaultObjectXml
DESTDIR=./src/com/ipvs/xsd/objectXml

DEVICENAMES="DMS-1000 DMS-120 V2D-Tx V2D-Rx IPVSClient DMS-100 DMS-1500 MS"

for devicename in ${DEVICENAMES}
do
    echo "Creating ${devicename}-Device.xml"
    java -cp ./dist/mediaroom.jar:"./3rdpartylib/*" com.ipvs.device/DMSApp UpgradeConfig ${DEFAULTDIR}/${devicename}-Default.xml ${DESTDIR}/${devicename}-Device.xml ${TEMPLATEDIR}/${devicename}-Device-Template.xml
done