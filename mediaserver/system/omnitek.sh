#reset fpga
#rmmod Barco_Ethernet_Extension
#rmmod OmniTek_Ethernet_Driver
#rmmod OmniTek_EFDMA
#rmmod OmniTek_Driver

setgpio.sh 93 out 0
setgpio.sh 93 out 1
#reset phy
setgpio.sh 96 out 0
setgpio.sh 96 out 1

#insert drivers
#insmod /lib/modules/3.0.0-rc7-tst5+/xpi/OmniTek_Driver.ko
#modprobe OmniTek_Driver.ko
/sbin/insmod /home/onair/V2O/modules/OmniTek_Driver.ko
sleep 1

#insmod /lib/modules/3.0.0-rc7-tst5+/xpi/OmniTek_EFDMA.ko
#modprobe OmniTek_EFDMA.ko
/sbin/insmod /home/onair/V2O/modules/OmniTek_EFDMA.ko
sleep 1

# enable mdio
macreg -r 340 -W 4d
mreg -p 1 -r 0 -R
sleep 1

#program phy for sgmii and autoneg
#sh /V2O/bin/physub.sh
sh /bin/physub.sh
sleep 1

#insmod /V2O/modules/OmniTek_Ethernet_Driver.ko
#insmod /lib/modules/3.0.0-rc7-tst5+/xpi/OmniTek_Ethernet_Driver.ko
#modprobe OmniTek_Ethernet_Driver.ko
/sbin/insmod /home/onair/V2O/modules/OmniTek_Ethernet_Driver.ko

#insmod /V2O/modules/Barco_Ethernet_Extension.ko
#insmod /lib/modules/3.0.0-rc7-tst5+/xpi/Barco_Ethernet_Extension.ko
#modprobe Barco_Ethernet_Extension.ko
/sbin/insmod /home/onair/V2O/modules/Barco_Ethernet_Extension.ko

# Assign mac address 

MACADDRESS=`fw_printenv ethaddr | cut -d '=' -f2`
echo Setting MAC for eth0 to $MACADDRESS
echo "Setting MAC Interface eth0 MAC address..."
ifconfig eth0 hw ether `echo $MACADDRESS`

#  /sbin/insmod /lib/modules/${KERNEL_VERSION}/netviz/OmniTek_Driver.ko
#   /sbin/insmod /lib/modules/${KERNEL_VERSION}/netviz/OmniTek_EFDMA.ko
#   /sbin/insmod /lib/modules/${KERNEL_VERSION}/netviz/gpion.ko
#   /sbin/insmod /lib/modules/${KERNEL_VERSION}/netviz/fb.ko
#   /sbin/insmod /lib/modules/${KERNEL_VERSION}/netviz/ptp.ko
#   /sbin/insmod /lib/modules/${KERNEL_VERSION}/netviz/OmniTek_Ethernet_Driver.ko
#   /sbin/insmod /lib/modules/${KERNEL_VERSION}/netviz/Barco_Ethernet_Extension.ko

