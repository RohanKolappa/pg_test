# This test is to check transmit audio path, starting from Expansion FPGA

#toggle audio enable in rx direction
INITVAL=`/V2O/bin/maint dreg -r 0x40 -R | cut -d"=" -f 3`
/V2O/bin/maint dreg -r 0x40 -W 0x0
/V2O/bin/maint dreg -r 0x40 -W 0x4

# Disable counter data in main FPGA so that data from expansion FPGA gets sent over the network
# bit4 should 0
/V2O/bin/maint dreg -r 0x894 -W 0x00
# Enable pattern in expansion
#disable and enable back, this will reset the counters
/V2O/bin/maint dreg -3 -r 0x20 -W 0x0
/V2O/bin/maint dreg -3 -r 0x20 -W 0x6

# Read the TX bandwidth counter. 
echo "Reading TX Bandwidth counter. This value should increase"
/V2O/bin/maint dreg -r 0x89c -R -c 10 -d 100

#replace back config reg
/V2O/bin/maint dreg -r 0x40 -W ${INITVAL}
