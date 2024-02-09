#!/bin/sh

# Define init disk procedures
TWHOME=/bin

fdin() {
    /bin/rm /tmp/fd.in
    echo "n" >>/tmp/fd.in
    echo "p" >>/tmp/fd.in
    echo "1" >>/tmp/fd.in
    echo " " >>/tmp/fd.in
    echo "+$1" >>/tmp/fd.in
    echo "w" >>/tmp/fd.in
    echo " " >>/tmp/fd.in
    echo " " >>/tmp/fd.in
}


# This function assumes del_array has been called.
# fdisk should only see one device, which should be 
# the compact flash
DISKDEV=sda
find_disk_device() {
    fdisk -l | grep "Disk /dev/sda"
    if [ x$? = "x0" ] ; then
        DISKDEV=sdb
    fi
}

# Find thedisk devices for DMS 1500
# If compact flash is mounted on /dev/sda
# then hard drives are assumed to be on sdb and sdc
DISKDEV_1=sda
DISKDEV_2=sdb
find_dms1500_disk_device() {
    ls -al /dev/CF | grep sda
    if [ x$? = "x0" ] ; then
        DISKDEV_1=sdb
        DISKDEV_2=sdc
    fi
}

exit_on_error() {
    if [ x$? != "x0" ]; then
        echo "Error dectected. Aborting."
        exit 1
    fi
}


wait_for_raid_initialization() {
    echo "Waiting for RAID Initialization..."
    local lastPercentDone="%"
    local start_time=$(date +%s)
    local backspaces='\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b'
    while (true); do
        local istatus=`tw_cli info c0 u0 initializestatus`
        echo ${istatus} | grep "is initialized" > /dev/null 2>&1
        if [ x$? = "x0" ]; then
            echo
            echo "RAID Initialized."
            break
        fi
        local percentDone=`echo ${istatus} | sed -e 's/.*Completion = //'` 
        if [ ${percentDone} != ${lastPercentDone} ]; then
            local time_now=$(date +%s)
            local time_spent=$(( ($time_now - $start_time)/60 ))
            local progress="RAID Initialization: ${percentDone}  [${time_spent} minutes]"
            echo -en ${backspaces}${backspaces}${progress}
            lastPercentDone=${percentDone}
        fi
        sleep 10
    done
}

ms25() {
    echo -n $"Starting $1 installation: "
    
    umount /data1

    dd if=/dev/zero of=/dev/sdb bs=1k count=1
    fdisk /dev/sdb < /tmp/fd.in
    mkfs /dev/sdb1
    mkdir /data1
    mount /dev/sdb1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/sdb1
    umount /data1
    /bin/rm /tmp/fd.in
}

dms25() {
    echo -n $"Starting $1 installation: "
    
    umount /data1

    dd if=/dev/zero of=/dev/sdb bs=1k count=1
    fdisk /dev/sdb < /tmp/fd.in
    mkfs /dev/sdb1
    mkdir /data1
    mount /dev/sdb1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/sdb1
    umount /data1
    /bin/rm /tmp/fd.in
}

ms100() {
    echo -n $"Starting $1 installation: "
    
    find_disk_device
    
    umount /data1

    $TWHOME/tw_cli /c0 add type=raid1 disk=0-1
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    fdisk /dev/${DISKDEV} < /tmp/fd.in
    mkfs /dev/${DISKDEV}1
    mkdir /data1
    mount /dev/${DISKDEV}1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/${DISKDEV}1
    umount /data1
    /bin/rm /tmp/fd.in
}

dms100() {
    echo -n $"Starting $1 installation: "

    find_disk_device
    
    umount /data1

    $TWHOME/tw_cli /c0 add type=raid1 disk=0-1
    exit_on_error
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    fdisk /dev/${DISKDEV} < /tmp/fd.in
    exit_on_error
    mkfs /dev/${DISKDEV}1
    mkdir /data1
    mount /dev/${DISKDEV}1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/${DISKDEV}1
    umount /data1
    /bin/rm /tmp/fd.in
}


dms100R2() {
    echo -n $"Starting $1 installation: "
    
    find_disk_device

    umount /data1

    $TWHOME/tw_cli /c0 add type=raid1 disk=0-1
    exit_on_error
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    $TWHOME/tw_cli /c0/u0 set cache=on
    fdisk /dev/${DISKDEV} < /tmp/fd.in
    exit_on_error
    mkfs /dev/${DISKDEV}1
    mkdir /data1
    mount /dev/${DISKDEV}1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/${DISKDEV}1
    umount /data1
    /bin/rm /tmp/fd.in
}

dms200() {
    echo -n $"Starting $1 installation: "
    
    find_disk_device

    umount /data1

    $TWHOME/tw_cli /c0 add type=raid10 disk=0-3
    exit_on_error
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    fdisk /dev/${DISKDEV} < /tmp/fd.in
    exit_on_error
    mkfs /dev/${DISKDEV}1
    mkdir /data1
    mount /dev/${DISKDEV}1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/${DISKDEV}1
    umount /data1
    /bin/rm /tmp/fd.in
}

dms200R2() {
    echo -n $"Starting $1 installation: "

    find_disk_device

    umount /data1

    $TWHOME/tw_cli /c0 add type=raid10 disk=0-3
    exit_on_error
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    $TWHOME/tw_cli /c0/u0 set cache=on
    fdisk /dev/${DISKDEV} < /tmp/fd.in
    exit_on_error
    mkfs /dev/${DISKDEV}1
    mkdir /data1
    mount /dev/${DISKDEV}1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/${DISKDEV}1
    umount /data1
    /bin/rm /tmp/fd.in
}

dms1200() {
    echo -n $"Starting dms1200 installation: "
    
    umount /data1

    $TWHOME/tw_cli /c0 add type=raid10 disk=0-7
    exit_on_error
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    $TWHOME/tw_cli /c0/u0 set cache=on
    fdisk /dev/sda < /tmp/fd.in
    exit_on_error
    mkfs /dev/sda1
    mkdir /data1
    mount /dev/sda1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/sda1
    umount /data1
    /bin/rm /tmp/fd.in
}

dms1500() {
    
    echo -n $"Starting dms1500 installation: "

    find_dms1500_disk_device
    
    umount /data1
    umount /data2

    $TWHOME/tw_cli /c0 add type=raid10 disk=0-3
    exit_on_error
    $TWHOME/tw_cli /c0 add type=raid10 disk=4-7
    exit_on_error
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    $TWHOME/tw_cli /c0/u0 set cache=on
    $TWHOME/tw_cli /c0/u1 set autoverify=off
    $TWHOME/tw_cli /c0/u1 set cache=on
    
    #  Prepare 1st Disk
    fdisk /dev/${DISKDEV_1} < /tmp/fd.in
    exit_on_error
    mkfs /dev/${DISKDEV_1}1
    mkdir /data1
    mount /dev/${DISKDEV_1}1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/${DISKDEV_1}1
    umount /data1
    
    # Prepare 2nd Disk
    fdisk /dev/${DISKDEV_2} < /tmp/fd.in
    exit_on_error
    mkfs /dev/${DISKDEV_2}1
    mount /dev/${DISKDEV_2}1 /data1
    /sbin/tune2fs -c 0 -i 0 -j /dev/${DISKDEV_2}1
    umount /data1
    /bin/rm /tmp/fd.in
}

dms1600() {
    
    find_dms1500_disk_device

    umount /data1 > /dev/null 2>&1

    echo "Creating RAID array: "
    $TWHOME/tw_cli /c0 add type=raid6 disk=0-7
    exit_on_error
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    $TWHOME/tw_cli /c0/u0 set cache=on

    wait_for_raid_initialization

    echo "Creating disk partitions..."
    dd if=/dev/zero of=/dev/${DISKDEV_1} bs=1k count=1
    /sbin/parted -s /dev/${DISKDEV_1} mklabel gpt
    /sbin/parted -s /dev/${DISKDEV_1} mkpart primary 0 5950G
    exit_on_error

    echo "Creating file system..."
    /sbin/mke2fs -T ext3 /dev/${DISKDEV_1}1
    /sbin/tune2fs -c 0 -i 0  /dev/${DISKDEV_1}1    
}

dms1700() {

    find_dms1500_disk_device
    
    umount /data1 > /dev/null 2>&1

    echo "Creating RAID array: "
    $TWHOME/tw_cli /c0 add type=raid6 disk=0-7
    exit_on_error
    $TWHOME/tw_cli /c0 set autorebuild=off
    $TWHOME/tw_cli /c0/u0 set autoverify=off
    $TWHOME/tw_cli /c0/u0 set cache=on

    wait_for_raid_initialization

    echo "Creating disk partitions..."
    dd if=/dev/zero of=/dev/${DISKDEV_1} bs=1k count=1
    /sbin/parted -s /dev/${DISKDEV_1} mklabel gpt
    /sbin/parted -s /dev/${DISKDEV_1} mkpart primary 0 11900G
    exit_on_error

    echo "Creating file system..."
    /sbin/mke2fs -T ext3 /dev/${DISKDEV_1}1
    /sbin/tune2fs -c 0 -i 0  /dev/${DISKDEV_1}1    
}

del_array() {
    echo "Destroying existing RAID arrays..."
    umount /dev/sda1 > /dev/null 2>&1
    umount /dev/sdb1 > /dev/null 2>&1
    $TWHOME/tw_cli /c0/u0 del quiet > /dev/null 2>&1
    $TWHOME/tw_cli /c0/u1 del quiet > /dev/null 2>&1
}


init_array() {
	case "$1" in
		MS-25)
			fdin 500101M
			ms25
			result
			;;
		DMS-25)
			fdin 500101M
			dms25
			result
			;;
		MS)
			del_array
			fdin 249978M
			ms100
			result
			;;
		DMS-100)
			del_array
			fdin 249978M
			dms100
			result
			;;
		MS-100-R2)
			del_array
			fdin 499977M
			dms100R2
			result
			;;
		DMS-100-R2)
			del_array
			fdin 499977M
			dms100R2
			result
			;;
		DMS-200)
			del_array
			fdin 1499966M
			dms200
			result
			;;
		DMS-200-R2)
			del_array
			fdin 1999972M
			dms200R2
			result
			;;
		DMS-1200)
			del_array
			fdin 1999948M
			dms1200
			result
			;;
		DMS-1500)
			del_array
			fdin 1999972M
			dms1500
			result
			;;
		DMS-1500-R2)
			del_array
			fdin 1999972M
			dms1500
			result
			;;
		DMS-1600)
			del_array
			dms1600
			result
			;;
		DMS-1600-R2)
			del_array
			dms1600
			result
			;;
		DMS-1700)
			del_array
			dms1700
			result
			;;
		DMS-1700-R2)
			del_array
			dms1700
			result
			;;
		*)
				echo $"Bug: Should not get here."
	esac
}

result() {
if [ $? -eq 0 ]
then
    echo "Result: Pass" >result.txt
else
    echo "Result: Fail" >result.txt
fi
}    

# Take start time
START=$(date +%s)
echo "**** Start Time $START ****"
    
# See how we were called.
case "$1" in
  del_array)
	 del_array
	 ;; 
  init)
	 init_array $2
	 ;; 
  *)
		echo $"Bug: Should not get here."
esac

# Take end time
END=$(date +%s)

# Difference
DIFF=$(( $END - $START ))
echo "**** End Time $END, Time taken = $DIFF seconds ****"

exit
