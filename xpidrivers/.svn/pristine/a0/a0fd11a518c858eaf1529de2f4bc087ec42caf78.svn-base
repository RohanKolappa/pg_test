#set spi switch to connect to processor
FCP_PATH="/tmp"
BITFILE_PATH="/mnt/cf"
/V2O/bin/setgpio.sh 88 out 1
/sbin/insmod /V2O/modules/m25p80.ko
echo "programming flash with new bit file $1.bit" 
echo $1
/V2O/bin/maint bit2fcp -i ${BITFILE_PATH}/$1.bit -o ${FCP_PATH}/$1.fcp
/usr/sbin/flashcp ${FCP_PATH}/$1.fcp /dev/mtd13
/bin/rm ${FCP_PATH}/$1.fcp
#set spi switch to connect to FPGA
/V2O/bin/setgpio.sh 88 out 0
sleep 1
/sbin/rmmod m25p80
