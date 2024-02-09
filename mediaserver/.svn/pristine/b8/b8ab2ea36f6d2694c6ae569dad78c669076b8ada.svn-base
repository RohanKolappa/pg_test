#reset fpga
setgpio.sh 93 out 0
setgpio.sh 93 out 1
#reset phy
setgpio.sh 96 out 0
setgpio.sh 96 out 1
#insert drivers
insmod /V2O/modules/OmniTek_Driver.ko
sleep 1
insmod /V2O/modules/OmniTek_EFDMA.ko
sleep 1
#enable mdio
maint macreg -r 0x340 -W 0x4d
maint mdioreg -p 1 -r 0 -R
sleep 1
#program phy for sgmii and autoneg
sh /V2O/bin/physub.sh
sleep 1
insmod /V2O/modules/OmniTek_Ethernet_Driver.ko
insmod /V2O/modules/Barco_Ethernet_Extension.ko
/V2O/bin/macaddress.sh
/sbin/udhcpc &
#sleep 1
#ping 10.51.49.64

