#!/bin/sh
dir=$1
serviceversionfile=$2
serviceVersion=`xsltproc ../utils/xsl/get_version.xsl $serviceversionfile`

for i in `find $dir -name "*.xml" -print`
#for i in `ls $dir/*.xml`
do
	xsltproc -o $i.tmp --stringparam serviceVersion $serviceVersion ../utils/xsl/update_object_version.xsl $i
	mv $i.tmp $i
done
