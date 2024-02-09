# This test is to check audio data rate controlled by register. 
echo "This tests only the main FPGA-main FPGA audio network connection" 

#toggle audio enable in rx direction
INITVAL=`/V2O/bin/maint dreg -r 0x40 -R | cut -d"=" -f 3`
/V2O/bin/maint dreg -r 0x40 -W 0x0
/V2O/bin/maint dreg -r 0x40 -W 0x4

# Select bandwidth
echo "100Mbps bandwidth selected"
/V2O/bin/maint dreg -r 0x8c4 -W 0x15

# Select counter data instead of data from Expansion to send out to network. Bit 4 should be 1
#disable and enable back, this will reset the counters
/V2O/bin/maint dreg -r 0x894 -W 0x0
/V2O/bin/maint dreg -r 0x894 -W 0x10

# Read the TX bandwidth counter. 
echo "Reading TX Bandwidth counter. This value should increase"
/V2O/bin/maint dreg -r 0x89c -R -c 10 -d 100

#replace back config reg
/V2O/bin/maint dreg -r 0x40 -W ${INITVAL}
