#!/bin/sh

CURRENT_DIR=`pwd`
CF22_DIR=/CF
#CF22_DIR=/home/anoop/testarea/tmp1
SYSLINUX_V2D2_2_BKP=syslinux.v2d_2_2
SYSLINUX_V2D3_0_BKP=syslinux.v2d_3_0
SYSLINUX_FILE=syslinux.cfg
CF30_DIR=/mnt/cf     

print_usage(){

    echo "*******************************************************************************"
    echo "Error: Missing URL"
    echo "Usage: upgradv2d.sh <upgrade/rollback> <if upgrade enter url>"
    echo "Example:"
    echo "upgradev2d.sh upgrade http://192.168.1.228"
    echo "    or"
    echo "upgradev2d.sh rollback"
    echo "*******************************************************************************"
}

cleanup () {
    cd $CURRENT_DIR
}

backupsyslinux() {
if [ -d $CF22_DIR ]     
then
   cp $CF22_DIR/syslinux.cfg $CF22_DIR/syslinux.bak
   cp $CF22_DIR/syslinux.cfg $CF22_DIR/$SYSLINUX_V2D2_2_BKP
   sync
   echo "Created backup of syslinux.cfg"
else
   echo "$CF22_DIR not found"
fi

}

upgrade_v2d () {
cd $CURRENT_DIR
backupsyslinux
echo "Downloading files"
for i in OA onair.gz onairapp.tgz local.txt onair.txt onair.tgz syslinux.cfg
do
if wget ${DOWNLOAD_URL}/$i -O $CF22_DIR/$i; then
    echo "Downloaded ${DOWNLOAD_URL}/$i"
else
    echo "Error: Could not download ${DOWNLOAD_URL}/$i."
    cp $CF22_DIR/syslinux.bak $CF22_DIR/syslinux.cfg
    sync
    exit 1
fi
done
}

rollback_v2d () {
echo "Inside rollback_v2d"
echo "Mounting compact flash......"
mount -t vfat /dev/hda1 $CF30_DIR
if [ -d $CF30_DIR ]     
then
    echo "Flash Mounted"
    cd $CF30_DIR 
    echo "Copying $SYSLINUX_FILE to $SYSLINUX_V2D3_0_BKP"
    cp $SYSLINUX_FILE $SYSLINUX_V2D3_0_BKP
    echo "Copying $SYSLINUX_V2D2_2_BKP to $SYSLINUX_FILE"
    cp $SYSLINUX_V2D2_2_BKP $SYSLINUX_FILE
    echo "Unmounting Compact flash....."
    umount $CF30_DIR
else
    echo "Mounting Failed"
    exit 1
fi
}

if [ x"$1" != "x" ]
then
    ACTION=$1
    DOWNLOAD_URL=$2
else
    print_usage
    exit
fi

cleanup

if [ x"$ACTION" = x"upgrade" ];
then
    if [ x"$2" != "x" ]; then
        DOWNLOAD_URL=$2
        upgrade_v2d
    else
        print_usage
        exit
    fi
elif [ x"$ACTION" = x"rollback" ];
then
    rollback_v2d
else
    echo "Unknown command --> $ACTION"
    print_usage
    exit
fi

cd $CURRENT_DIR

exit
