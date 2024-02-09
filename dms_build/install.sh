#!/bin/sh

CFDIR=/mnt/cf
ISV2D2_2=0

logit() {
    message="${1}: ${2}"
    /usr/bin/logger -t upgrade "${message}"
    echo "${message}"

}
log_info() {
    logit "INFO" "$1"
}

log_error() {
    logit "ERROR" "$1"
}

cleanup () {
   log_info "Cleaning up..."
   finish_cf
}

get_file () {
    local URL=$1
    local TARGET=$2

    log_info "Downloading ${URL}..."
    if wget --connect-timeout 5 -t 3 $URL -O $TARGET; then
        log_info "Downloaded ${URL}"
    else
        log_error "Could not download ${URL}."
        log_info "Please check ${URL} and make sure"
        log_info "the file can be retrieved."
        cleanup
        exit 1
    fi
}

quit_with_error () {
    local message=$1

    log_error "$message"
    cleanup
    exit 1
}

prepare_cf() {

    if [ -x /bin/cfmount ] ; then
        /bin/cfmount
        if [ "x$?" != "x0" ]; then
            quit_with_error "Could not mount compact flash."
        fi
    else
        if [ -L /dev/cf ] ; then
            # Assume this is a V2D 2.1 system
            mkdir -p /mnt/cf
            umount /CF
            if [ -f /etc/config/grub_system  ] ; then
               /bin/mount -t ext3 /dev/cf /mnt/cf
            else
               /bin/mount -t vfat /dev/cf /mnt/cf
            fi
            if [ ! -f /mnt/cf/bzImage ] ; then
                quit_with_error "Could not mount compact flash for V2D device"
            fi
            # Save V2D's syslinux.cfg for reverting if needed
            cp /mnt/cf/syslinux.cfg /mnt/cf/syslinux.v2d_2_2
            sync
            ISV2D2_2=1
        else 
            # Assume this is a Manufacturing station
            mkdir -p ${CFDIR}

            if [ ! -b /dev/CF ]; then
                quit_with_error "Device /dev/CF does not exist."
            fi
    
            # Mount the compact flash
            echo "Mounting compact flash..."
            umount ${CFDIR} > /dev/null 2>&1
            if [ -f  /etc/config/grub_system  ] ; then
              mount -t ext3 /dev/CF ${CFDIR}
            else
              mount -t vfat /dev/CF ${CFDIR}
            fi
            
            if [ "x$?" != "x0" ]; then
                quit_with_error "Cannot mount the compact flash."
            fi
            
        fi
    fi
}

finish_cf() {

    log_info "Synching compact flash..."
    sync
    log_info "Unmounting..."
    if [ -x /bin/cfunmount ] ; then
        /bin/cfunmount
    else
        umount ${CFDIR}
    fi
}

sync_cf() {

    log_info "Synching compact flash..."
    sync
}


process_manifest_line() {
    local line="$1"
    local type=""
    local file=""
    local target=""
    local format=""
    local md5sum=""


    # Parse manifest line
    for nvpair in ${line}; do
        name=`echo $nvpair | cut -d = -f 1`
        value=`echo $nvpair | cut -d = -f 2`
        if [ $name = 'type' ] ; then
            type=$value
        elif [ $name = 'format' ] ; then
            format=$value
        elif [ $name = 'file' ] ; then
            file=$value
        elif [ $name = 'target' ] ; then
            target=$value
        elif [ $name = 'md5' ] ; then
            md5sum=$value
        fi
    done

    # Handle raw format files
    if [ x$format = 'xraw'  ] ; then

        # If target not set, it is same as source
        if [ x$target = 'x' ] ; then
            target=$file
        fi

        # Compare with checksum of original file on compact flash
        local md5orig=`grep file=${target} ${CFDIR}/manifest.txt | sed -e 's/.*md5=\([[:alnum:]]*\).*/\1/'`
        if [ x$md5orig = "x" -o x$md5sum != x$md5orig ] ; then

            # Download file
            get_file ${DOWNLOAD_URL}/$file ${CFDIR}/tmp/$target
            
            # Verify checksum 
            log_info "Verifying Checksum for $target"
            local md5new=`md5sum  ${CFDIR}/tmp/$target | cut -d " " -f 1`
            if [ x$md5sum != x$md5new ] ; then
                quit_with_error "Bad Checksum for $target: $md5sum != $md5new"
            fi
            log_info "Checksum OK for $target"
            
            # Move file to final location
            log_info "Copying \"$target\" to compact flash"
            mv ${CFDIR}/tmp/$target ${CFDIR}/$target
            sync_cf
        else
            log_info "Skipping file $target. Already uptodate."
        fi
    fi

    # Handle tar format files
    if [ x$format = 'xtar'  ] ; then

        # Compare with checksum of original file on compact flash
        local md5orig=`grep file=${file} ${CFDIR}/manifest.txt | sed -e 's/.*md5=\([[:alnum:]]*\).*/\1/'`

        if [ x$md5orig = "x" -o x$md5sum != x$md5orig ] ; then

            # Download file
            get_file ${DOWNLOAD_URL}/$file ${CFDIR}/tmp/$file

            # Verify checksum 
            log_info "Verifying Checksum for $file"
            local md5new=`md5sum  ${CFDIR}/tmp/$file | cut -d " " -f 1`
            if [ x$md5sum != x$md5new ] ; then
                quit_with_error "Bad Checksum for $file: $md5sum != $md5new"
            fi
            log_info "Checksum OK for $file"
            
            # Untar file to temp directory
            rm -rf ${CFDIR}/tmp/tmpdir
            mkdir -p ${CFDIR}/tmp/tmpdir
            tar -C ${CFDIR}/tmp/tmpdir -oxf ${CFDIR}/tmp/$file
            
            # Move all untarred files to final location
            for src in `ls ${CFDIR}/tmp/tmpdir/*` ; do
                log_info "Copying \"$src\" to compact flash"
                mv $src ${CFDIR}
            done

            if [ -d /mnt/cf/boot ] && [ -f /etc/config/grub_system ] ; then
               cp  /mnt/cf/OA /mnt/cf/boot/
               cp  /mnt/cf/onair.gz /mnt/cf/boot/
               cp  /mnt/cf/menu.lst /mnt/cf/boot/grub/
               if [ -f /etc/scripts/xsl/get_name.xsl ]; then 
                  devname=`xsltproc /etc/scripts/xsl/get_name.xsl /etc/config/Device.xml`
                  devname=`echo $devname | sed 's/-Device*//'`
                  sed -i "s|DMS|$devname|g" /mnt/cf/boot/grub/menu.lst

               fi
            fi
                
            # Clean temp files
            rm -f ${CFDIR}/tmp/$file
            rm -rf ${CFDIR}/tmp/tmpdir
            sync_cf
        else
            log_info "Skipping file $file. Already uptodate."
        fi
    fi


}

apply_transform() {

    ipaddress=$1
    netmask=$2
    gateway=$3

    # Create an XSLT transform document to change the values we want in Device.xml
    rm -f transform.xsl
    cat > transform.xsl <<EOF
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="node()|@*">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()"/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="EthernetPortConfig[PortID=1]/IPConfig/IPAddress">
    <IPAddress>${ipaddress}</IPAddress>
  </xsl:template>
  <xsl:template match="EthernetPortConfig[PortID=1]/IPConfig/Netmask">
    <Netmask>${netmask}</Netmask>
  </xsl:template>
  <xsl:template match="RouteTableConfig/RouteEntry/Gateway">
    <Gateway>${gateway}</Gateway>
  </xsl:template>

</xsl:stylesheet>
EOF

    # Use xsltproc utility to apply the transform
    xsltproc transform.xsl Device.xml >| Device-Transformed.xml
    if [ "x$?" = "x0" ]; then
        /bin/mv Device-Transformed.xml Device.xml
        log_info "Applied Network configuration to Device.xml"
    fi

}

_upgrade_db() {
		#added 11/21/2010... may be very old versions are not compatible with this code
    #if ms is enabled than we need to create snapshot 
    if [ ! -f /var/run/ms.conf ]; then
        log_info "No need to create snapshot.... Management server is not enabled ..."
        return 0
    fi

    local dbctl=""
    if [ -f /etc/scripts/ipvsxmldbctl.sh  ]; then
        dbctl="/etc/scripts/ipvsxmldbctl.sh"
    elif [ -f /etc/scripts/sednactl.sh  ]; then
        dbctl="/etc/scripts/sednactl.sh"
    else
        return 0
    fi
    
    if [ -f /etc/scripts/watchdog/watch_linuxha.sh  ]; then
        local ha_active=`/etc/scripts/watchdog/watch_linuxha.sh status_resource`
        if [ "${ha_active}" = "Standby" ]; then
            log_info "No need to create snapshot... HA is in 'Standby' mode..."
            return 0
        fi
    fi

		# ---- check if db is running for creating snapshot
    local db_status=`$dbctl status`
    if [ "${db_status}" != "OK" ]; then
        log_info "Starting db to create snapshot..."
       #db is not running try starting it
        /etc/scripts/watchdog/watch_managementserver.sh manual
        $dbctl start
    fi
    # ---- create snapshot regard less
    log_info "Creating db snapshot..."
    $dbctl create_snapshot

    # ---- if sedna db--- schedule restore on next startup 
    #DB_SERVER default to sedna_native
    DB_SERVER="sedna_native"
    if [ -f /etc/scripts/_db_server.rc ]; then
       #if exist -- import the script /etc/scripts/_db_server.rc
       #will overwrite DB_SERVER -- it could be sedna_native or exist_xmldb
        . /etc/scripts/_db_server.rc
    fi
    if [ "${DB_SERVER}" != "exist_xmldb" ]; then
       #current running database is not exist... create snapshot and restore on next startup
        log_info "From sedna to exist..... schedule restore snapshot on next reboot."
        $dbctl schedule_restoreSnapshot
        if [ -f "/data/sednaserver/sedna-db/cfg/msdb_recreate" ]; then
            mkdir -p /data/sedna_dbsnapshot/status
            rm -rf /data/sedna_dbsnapshot/status/msdb_recreate
            cp /data/sednaserver/sedna-db/cfg/msdb_recreate /data/sedna_dbsnapshot/status
        fi
    fi    
		return 0
}

upgrade() {

    # Get the compact flash device ready for upgrade
    prepare_cf

#    logger -s "Upgrading software into ${CFDIR}"

    # Making sure manifest.txt already exists

    if [ ! -f ./manifest.txt ] ; then
       quit_with_error "Did not find file ./manifest.txt"
    fi

    rm  -rf ${CFDIR}/tmp
    mkdir -p ${CFDIR}/tmp


    # Run any custom upgrade scripts here...
    
    # If device is v2d, we stop some services to free up some memory
    if [ -f /etc/config/sw_profile_v2d.txt ] ; then
        /bin/devappctl stop
        /bin/strmsrvctl stop
    fi
    # If device is xpi, we stop some services to free up some memory
    if [ -f /etc/config/sw_profile_xpi.txt ] ; then
        /bin/strmsrvctl stop
		/usr/bin/killall net2usb
    fi
    # Manifest file check required for ms. The software profile for MS is no 
    # longer same as that of the dms
    if [ -f /etc/config/sw_profile_dms.txt ] && [ -f /etc/config/hw_profile_ms.txt ] ; then
            get_file ${DOWNLOAD_URL}/manifest_ms.txt manifest.txt
	fi

    if [ -f /etc/config/sw_profile_dms.txt -o -f /etc/config/sw_profile_ms.txt ] ; then
        log_info "create db snpashot and if needed will be used to restore ..."
        _upgrade_db
    fi

    # Install the files listed in manifest.txt
    # For each line in manifest file, invoke process_manifest_line routine
    while read manifestline
    do
        process_manifest_line "$manifestline"
    done < "manifest.txt"

    # Finally copy the manifest file to the flash
    log_info "Copying manifest.txt to compact flash..."
    cp manifest.txt ${CFDIR}/
    rm  -rf ${CFDIR}/tmp

    # Save cache files to compact flash
    mkdir -p ${CFDIR}/cache
    cp -f /V2O/config/*cache.txt ${CFDIR}/cache

    # Done with compact flash
    finish_cf

    # now reboot
    if [ x"$ISV2D2_2" = "x1" ] ; then
        log_info "System Upgraded. Please reboot manually."
    else
        log_info "Initiating sytem reboot after upgrade..."

        from_version=`grep type=version manifest.txt | sed 's/type=.*=//'`
        to_version=`cat /home/onair/server.version`
        echo -n "Device Upgrade: " >> /data/uptime.log
        /bin/date >> /data/uptime.log
        echo "Upgrade From ${from_version} To ${to_version}" >> /data/uptime.log
        touch /tmp/reset
    fi

    return 0
}

wipeout() {
	echo "Deleting all partitions from the disk"
	disk=`ls -l /dev/CF |cut -f2 -d'>' | cut -f1 -d 1 | cut -f 2 -d' '`

	dd if=/dev/zero of=${disk} bs=512 count=1
	parted ${disk} mklabel msdos
	parted ${disk} mkpartfs primary fat32 0 100%
	parted ${disk} set 1 boot on
	parted ${disk} set 1 lba on
	sleep 5
}

install() {


    # Make sure we have all the files we need
    filelist="manifest.txt Device.xml"

    for file in $filelist
    do
        if [ ! -f $file ]; then
            quit_with_error "Missing File $file."
        fi
    done
	if [ ! -f /etc/config/grub_system ] ; then
        which syslinux
       if [ x$? = "x1" ] ; then
          quit_with_error "Utility syslinux not found on this system"
       fi
    fi

    result="n"
    grep "Generic-Device" Device.xml > /dev/null 2>&1
    if [ x$? = "x1" ] ; then
        echo
        read -p "Change Network Config [y/n] ? " result; echo
    fi

    if [ "$result" == "y" ]; then

        which xsltproc
        if [ x$? = "x1" ] ; then
            log_error "Utility xsltproc not found on this system"
            log_error "Cannot apply network config."
            log_info "Retry and say no next time."
            cleanup
            exit 1
        fi

        read -p "IP Address: " result
        eth0_ipaddress=$result
        read -p "Netmask: " result
        eth0_netmask=$result
        read -p "Default Gateway: " result
        eth0_gateway=$result

        echo 
        echo "******************************"
        echo "IP Address: $eth0_ipaddress"
        echo "Netmask: $eth0_netmask"
        echo "Default Gateway: $eth0_gateway"
        echo "******************************"
        echo 

        read -p "Are you sure [y/n] ? " result; echo

        if [ "$result" != "y" ]; then
           echo "Aborting..."
           return 1
        else 
           echo "Transforming Device.xml..."
           apply_transform $eth0_ipaddress $eth0_netmask $eth0_gateway
        fi

    fi

    prepare_cf

    # First cleanup the compact flash
    echo "Cleaning up compact flash..."
    /bin/rm -rf ${CFDIR}/*
    sync
    sync
    finish_cf
    
    if [ ! -f /etc/config/sw_profile_xpi.txt ] ; then
    # Next syslinux on the CF on non XPi devices
    echo "Setting up syslinux bootloader on compact flash..."
    syslinux /dev/CF
    fi

    # Mount flash again
    prepare_cf

    # Copy all the files over
    rm  -rf ${CFDIR}/tmp
    mkdir -p ${CFDIR}/tmp

    # Install the files listed in manifest.txt
    # For each line in manifest file, invoke process_manifest_line routine
    while read manifestline
    do
        process_manifest_line "$manifestline"
    done < "manifest.txt"


    # Add marker in compact flash to detect factory default at startup
    /bin/touch ${CFDIR}/factory
    
    log_info "Copying Device.xml to compact flash..."
    cp Device.xml ${CFDIR}/

    # Finally copy the manifest file to the flash
    log_info "Copying manifest.txt to compact flash..."
    cp manifest.txt ${CFDIR}/
    rm  -rf ${CFDIR}/tmp

    # Finish
    finish_cf
    echo "Done!!"
}

usage() {
    echo "*******************************************************************************"
    echo "Error: $1"
    echo "Usage: ./install.sh URL {upgrade|install}"
    echo "Example:"
    echo "install.sh http://192.168.1.228/DMS install"
    echo "*******************************************************************************"
    exit 1
}

if [ x"$1" != "x" ]
then
    DOWNLOAD_URL=$1
else
    usage "Need to specify URL"
fi

case "$2" in
  upgrade)
        upgrade
        ;;

  install)
        wipeout
        install
        ;;

  changepersonality)
        install
        ;;

  *)
        usage "Need to specify command : install or upgrade"

esac

exit 0
