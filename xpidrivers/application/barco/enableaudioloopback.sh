echo "This will enable audio loopback via Main FPGA"

#/V2O/bin/maint dreg -3 -r 0x20 -W 0x8  #expansion

#Bit 7 - enable local audio loopback from main fpga
/V2O/bin/maint dreg -r 0x894 -W 0x80 
