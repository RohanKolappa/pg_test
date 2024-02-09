strmsrvctl stop
test.sh
enablevideo_sl.sh
loadvideodrivers.sh
maint setedid -a 0x20 -f /V2O/bin/3840x1080.edid
sleep 1
enableinputvideotopc.sh
DVIAdv7604.sh
sleep 2
#1080pAdv7612.sh
echo "enable 1920x1080 on PC"
sleep 15
echo "power cycle monitor to see video"
maint sreg -r 0x3c -W 0x0
maint sreg -r 0xc -W 0x2
maint sreg -r 0xc -W 0x0
echo "reading lock register 0x64 in S6"
maint sreg -r 0x64 -R
sleep 2
#rmmod Barco_NetvizXpi
#rmmod Barco_NetvizXpi
#rmmod Barco_NetvizXpi
#rmmod Barco_Sii7172
#rmmod Barco_Sii7172
#rmmod Barco_Sii7172
#insmod /V2O/modules/Barco_Sii7172.ko
#insmod /V2O/modules/Barco_NetvizXpi.ko
add_lut_list -alutreg 25 -orig_hres 1920 -orig_vres 1080
maint setaddsidelut -L 25
touch /tmp/xpilutid
echo "1" >/tmp/xpilutid
sleep 1
strmsrvctl start
standalone
sleep 3
enablevideo_sl.sh
