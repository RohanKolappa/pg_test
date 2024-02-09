#!/bin/sh
# Source common library
. /etc/scripts/common.rc

#this fw_printenv is necessary for now since the first fw_printenv will fail
#and "macaddress.sh" will get the wrong IP if fw_printenv fails
/usr/sbin/fw_printenv
hostname NGS-D200-`/usr/sbin/fw_printenv AssemblySerialNumber| cut -f 2 -d '='`
FW_PATH="/mnt/cf"
EXP_FILE="expansion_fpga"
MAINT_CMD="/V2O/bin/maint"
logprint "checking u-boot"
#check u-boot
NOR_UBOOT_VER=`/usr/bin/md5sum /dev/mtd1 |cut -f 1 -d " "`
SD_UBOOT_VER=`/usr/bin/md5sum ${FW_PATH}/u-boot.bin |cut -f 1 -d " "`

if [ x${NOR_UBOOT_VER} != x${SD_UBOOT_VER} ] ; then
  echo "Upgrading u-boot"
  /usr/sbin/flashcp ${FW_PATH}/u-boot.bin /dev/mtd1
fi

#check for u-boot variables
old1=`fw_printenv loadfpga`
old2=`fw_printenv bootcmd`
new1='loadfpga=watchdog stop; mmcinfo ; if fatload mmc 0:1 ${default_load_addr} sys_type_xpi_decoder ; then setenv sys_type "xpi_decoder"; else  setenv sys_type "xpi_encoder"; fi ; if fatload mmc 0:1 ${default_load_addr} sys_type_xpi_bringup ; then setenv sys_type "xpi_bringup"; else ; fi ; fatload mmc 0:1 ${default_load_addr} ${sys_type}.bit;smc upload ${default_load_addr} ${fpga_sz}'
new2='bootcmd=gpio clear 95;smc regwrite 0xf044 0;run loadfpga;run mmc_irfs_boot'

if [ "x${old1}" != "x${new1}" ] || [ "x${old2}" != "x${new2}" ]; then
   echo "updating u-boot variables"
   /bin/sh /V2O/bin/update_var_boardtype.sh 
fi

#if u-boot got updated or variables needed update, reboot system
if [ x${NOR_UBOOT_VER} != x${SD_UBOOT_VER} ] || [ "x${old2}" != "x${new2}" ] || [ "x${old1}" != "x${new1}" ] ; then
   echo "uboot ver in nor         = ${NOR_UBOOT_VER}"
   echo "uboot ver is SD card     = ${SD_UBOOT_VER}"
   echo "loadfpga var in test.sh  = ${new1}"
   echo "bootcmd var in  u-boot   = ${old2}"
   echo "loadfpga var in test.sh  = ${new2}"
   echo "Setting system to reboot"
   /V2O/bin/reboot_system
   sleep 10
   exit
fi

logprint "u-boot ok, resetting fpga and phy"
#reset fpga
/V2O/bin/setgpio.sh 93 out 0
#reset mainboard phy
/V2O/bin/setgpio.sh 96 out 0
#check what board type is on SD card
logprint "checking S6"

if [ -f ${FW_PATH}/sys_type_xpi_decoder ] ; then 
    SYS_TYPE_REQ=xpi_decoder
    FCP=fanout_fpga_dec
else
    SYS_TYPE_REQ=xpi_encoder
    FCP=fanout_fpga_enc
fi

if [ -f ${FW_PATH}/sys_type_xpi_bringup ] ; then 
    SYS_TYPE_REQ=xpi_bringup
    FCP=fanout_fpga_bringup
fi

#load Spartan 6
S6_TYPE=`/usr/sbin/fw_printenv s6_type | cut -f 2 -d '='`
S6_VER=`/usr/sbin/fw_printenv s6_ver | cut -f 2 -d '='`
S6_VER_REQ=`cat ${FW_PATH}/${FCP}_time.v |grep "16'h"|sed "s/.*16'h//" | xargs | sed "s/ /_/"`

if [ x$S6_TYPE != x$SYS_TYPE_REQ ] || [ x$S6_VER != x$S6_VER_REQ ]; then 
    echo "Found S6 $S6_TYPE in hw while $SYS_TYPE_REQ on SD Card"
    echo "Found S6 $S6_VER in hw while $S6_VER_REQ on SD Card"
    echo "Upgrading S6 Flash"
    /V2O/bin/reprogS6.sh $FCP
    /usr/sbin/fw_setenv s6_type $SYS_TYPE_REQ
    /usr/sbin/fw_setenv s6_ver $S6_VER_REQ
fi
#prog pin of S6 is set to reset, released later
/V2O/bin/setgpio.sh 95 out 0
#removing V6 fpga reset, delaying to debug network not working on decoder bug 
/V2O/bin/setgpio.sh 93 out 1
#removing PHY RESET
/V2O/bin/setgpio.sh 96 out 1

logprint "Starting omnitek drivers"
#insert drivers
/sbin/insmod /V2O/modules/OmniTek_Driver.ko
/sbin/insmod /V2O/modules/OmniTek_EFDMA.ko

/V2O/bin/loadvideodrivers.sh
logprint "Finished loadvideodriver.sh"
#reset expansion
${MAINT_CMD} dreg -o -r 0x34 -W 0x0
${MAINT_CMD} dreg -o -r 0x34 -W 0x1
#
#   #checking Expansion FPGA
#   EXP_VER=`/usr/sbin/fw_printenv exp_ver | cut -f 2 -d '='`
#   EXP_VER_REQ=`cat ${FW_PATH}/${EXP_FILE}_time.v |grep "16'h"|sed "s/.*16'h//" | xargs | sed "s/ /_/"`
#   
#   if [ x${EXP_VER} != x${EXP_VER_REQ} ]; then 
#       logprint "Found EXP ${EXP_VER} in hw while ${EXP_VER_REQ} on SD Card"
#       logprint "Upgrading Expansion FPGA"
#       #revisit reprogramming sector 0,1
#       ${MAINT_CMD} s3_upgrade -i ${FW_PATH}/${EXP_FILE}.bit -a 1
#       logprint "finished upgrading s3"
#       /usr/sbin/fw_setenv exp_ver ${EXP_VER_REQ}
#   fi
#   
#       logprint "resetting expansion to boot from sector 2"
#       /V2O/bin/maint dreg -3 -r 0x28 -W 0x03020000
#       /V2O/bin/maint dreg -3 -r 0x24 -W 0x1
#
#${MAINT_CMD} dreg -o -r 0x34 -W 0x0
#${MAINT_CMD} dreg -o -r 0x34 -W 0x1

logprint "Starting mdio"
${MAINT_CMD} network_init
#enable mdio primary mac
#${MAINT_CMD} macreg -r 0x340 -W 0x4d
#${MAINT_CMD} mdioreg -p 1 -r 0 -R
##enable mdio expansion mac
#${MAINT_CMD} macreg -r 0x740 -W 0x4d
#${MAINT_CMD} mdioreg -p 1 -r 0 -R -o 0x400
##program phy for sgmii and autoneg - primary mac
#sh /V2O/bin/physub.sh
##program phy for sgmii and autoneg - expansion mac
#sh /V2O/bin/physub2.sh
logprint "Starting network drivers"
/sbin/insmod /V2O/modules/OmniTek_Ethernet_Driver.ko
/sbin/insmod /V2O/modules/Barco_Ethernet_Extension.ko
logprint "network drivers loaded"
/V2O/bin/macaddress.sh
#/sbin/udhcpc &
logprint "starting avahi"
avahi-daemon -k
if [ -f /sbin/avahi-daemon ]; then
    /sbin/avahi-daemon --no-chroot -D
else
    /usr/sbin/avahi-daemon  -D
fi

#release reset on S6
/V2O/bin/setgpio.sh 95 out 1
logprint " S6 ok"
