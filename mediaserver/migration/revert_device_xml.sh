#!/bin/sh

SCRIPTS_DIR=/etc/scripts

#SCRIPTS_DIR=.

# Source upgrade library
. ${SCRIPTS_DIR}/database_upgrade_variables.rc

# Source common library
. ${SCRIPTS_DIR}/common.rc

logprint "$0 Searching for an older version in compact flash..."
test -x /bin/cfmount && /bin/cfmount

# Does old config directory exist?
if [ ! -d ${CF_DIR}/old_configs ] ; then
	exit 1
fi
# Determine target version
if [ ! -f ${XML_DIR}/service.version ] ; then
    logprint "$0 Cannot determine target version. Aborting..."
    exit 1
fi

target_version=`xsltproc ${XSL_DIR}/get_version.xsl ${XML_DIR}/service.version`;
target_major=`echo ${target_version} | cut -f1 -d'.'`
target_minor=`echo ${target_version} | cut -f2 -d'.'`
target_revision=`echo ${target_version} | cut -f3 -d'.'`
if [ x"${target_revision}" = "x" ]; then
	target_revision=0
fi
cfgmajor="1"
cfgminor="0"
cfgrevision="0"
cfgname="version_1.0.0.tgz"
for fname in `ls ${CF_DIR}/old_configs | grep version`
do
	version=`echo $fname |sed -e 's/version_\(.*\)\.tgz/\1/g'`
	major=`echo $version| cut -f1 -d'.'`
	minor=`echo $version| cut -f2 -d'.'`
	revision=`echo $version| cut -f3 -d'.'`


	if [ x"${revision}" = "x" ]; then
		revision=0
	fi
   
	if [ $major -gt $target_major ]; then
		continue 
	elif [ $major -eq $target_major ]; then
		if [ $minor -gt $target_minor ]; then
			continue
		elif [ $minor -eq $target_minor ]; then
			if [ $revision -gt $target_revision ]; then
				continue
			fi
		fi
	fi
	if [ $cfgmajor -gt $major ]; then
		continue 
	elif [ $cfgmajor -eq $major ]; then
		if [ $cfgminor -gt $minor ]; then
			continue
		elif [ $cfgminor -eq $minor ]; then
			if [ $cfgrevision -gt $revision ]; then
				continue
			fi
		fi
	fi
	cfgmajor=$major
	cfgminor=$minor
	cfgrevision=$revision
	cfgname=$fname
done

if [ ! -f ${CF_DIR}/old_configs/${fname} ]; then
	echo "Could not find any configuration $cfgmajor $cfgminor $cfgrevision"
	test -x /bin/cfunmount && /bin/cfunmount
	exit 1
fi

logprint "$0 Retrieving older version ${cfgmajor}.${cfgminor}.${cfgrevision} ${cfgname} from compact flash."
echo "Retrieving older version ${cfgmajor}.${cfgminor}.${cfgrevision} ${cfgname} from compact flash."
rm -rf ${TMP_AREA}/files                                                
if [ x"${cfgrevision}" = "x0" ]; then
	tar -C ${TMP_AREA} -xzf ${CF_DIR}/old_configs/version_${cfgmajor}.${cfgminor}.tgz
else
	tar -C ${TMP_AREA} -xzf ${CF_DIR}/old_configs/version_${cfgmajor}.${cfgminor}.${cfgrevision}.tgz
fi
if [ x"$?" != "x0" ]; then                     
	exit 1                                 
fi   
if [ -d ${TMP_AREA}/files ] ; then                                      
	cp -r ${TMP_AREA}/files/* ${CONFIG_DIR}                             
fi                                                                      





test -x /bin/cfunmount && /bin/cfunmount
exit 0
