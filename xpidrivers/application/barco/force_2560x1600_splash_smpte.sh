/V2O/bin/set_tx_splash_parm -enable_splash Yes -horiz_resolution 2560 -vert_resolution 1600 -refresh_rate 59.972 -pixel_clock_rate 268.5 -horiz_front_porch 48 -vert_front_porch 3 -horiz_back_porch 80 -vert_back_porch 37 -horiz_synch 32 -vert_synch 6
/V2O/bin/set_splash_mode -force_splash Yes
/V2O/bin/maint dreg -6 -r 0x214 -W 0x01000000
sh /V2O/bin/splash_smpte.sh

#soft reset to pattern generator -bit 1 active high
/V2O/bin/maint dreg -6 -r 0xc -W 0x2
/V2O/bin/maint dreg -6 -r 0xc -W 0x0

echo "CRC Even From Fanout FPGA - output side"
/V2O/bin/maint dreg -6 -r 0xc60 -c 5 -d 100 -R
echo "CRC Odd From Fanout FPGA - output side"
/V2O/bin/maint dreg -6 -r 0xc64 -c 5 -d 100 -R
