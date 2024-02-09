#!/bin/sh
datadevs="/dev/md0 /dev/hdc1 /dev/hda1 /dev/hdd1 /dev/sda1 /dev/sdb1 /dev/sdc1 /dev/sdd1 /dev/hda2"
DATAPARTITION=$1
ALTPARTITION=$2
LOGFILE=$3
mountalt=0
umountalt=0
mountdata=0

makeDir()
{
	rootDir=$1;
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/default/db
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/default/blob
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/default/templates
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site/videolibrary/db
	/bin/mkdir -p ${rootDir}/DSS/data/portal/site-cache
	/bin/mkdir -p ${rootDir}/upload_tmp_dir
	/bin/mkdir -p ${rootDir}/upgrade_dir
	chmod -R 777 ${rootDir}/upload_tmp_dir
	chmod -R 777 ${rootDir}/upgrade_dir
	chmod -R 777 ${rootDir}/DSS/data/portal
	return
}

/sbin/e2fsck -y ${ALTPARTITION} > ${LOGFILE}
if [ $? != 0 ];
then
	mountalt=0
else
	mountalt=1
fi

if [ x"$mountalt" != "x0" ];
then
	/bin/mkdir -p /data1
	/bin/mount -t ext3 ${ALTPARTITION} /data1
	if [ $? != 0 ];
	then
		umountalt=0
	else
		/bin/mkdir -p /data1/DSS/data/file
		makeDir /data1
		umountalt=1
	fi
fi

/bin/mkdir -p /data2
if [ x"${DATAPARTITION}" = "xNone" ];
then
	for dev in ${datadevs}
	do
		/bin/mount -t ext3 ${dev} /data2
		if [ $? -eq 0 ];
		then
			DATAPARTITION=${dev}
			/bin/umount /data2
			break
		fi
	done
fi


/sbin/e2fsck -y ${DATAPARTITION} >> ${LOGFILE}
/bin/mount -t ext3 ${DATAPARTITION} /data
if [ $? != 0 ];
then
	/bin/mkdir /home/onair/data
	/bin/rmdir /data
	/bin/ln -s /home/onair/data /
	makeDir /data
	echo "Error mounting data"
else
	/bin/mkdir -p /data/DSS/data/file
	makeDir /data
fi
date >> /data/uptime.log
uptime >> /data/uptime.log

if [ x"$umountalt" != "x0" ];
then
	while(true)
	do
		/bin/umount /data1
		if [ $? != 0 ];
		then
			mountalt=0
		else
			mountalt=1
			break;
		fi
		sleep 2
	done
fi
if [ x"$mountalt" != "x0" ];
then
	/bin/mkdir /data2
	/bin/mount -t ext3 ${ALTPARTITION} /data2
	if [ $? != 0 ];
	then
		umountalt=0
	else
		/bin/cp /data2/DSS/data/file/* /data/DSS/data/file
		/bin/rm -f /data2/DSS/data/file/* 
		umountalt=1
	fi
fi
if [ x"$umountalt" != "x0" ];
then
	while(true)
	do
		/bin/umount /data2
		if [ $? != 0 ];
		then
			echo "Umount in progress"
		else
			break;
		fi
		sleep 2
	done
fi

if [ -f /data/custom.sh ] ;
then
	/bin/sh /data/custom.sh
fi

