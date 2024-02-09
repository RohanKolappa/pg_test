echo "This will disable audio loopback via Main FPGA"

#/V2O/bin/maint dreg -3 -r 0x20 -W 0x0 #expansion

#Bit 7 - enable local audio loopback from main fpga
/V2O/bin/maint dreg -r 0x894 -W 0x0
