# This script is to check received audio network path.
#toggle audio enable in rx direction
INITVAL=`/V2O/bin/maint dreg -r 0x80 -R | cut -d"=" -f 3`
/V2O/bin/maint dreg -r 0x80 -W 0x0
/V2O/bin/maint dreg -r 0x80 -W 0x4

# Bit 2 should be 0 to receive data from network
# Bit 7 should be 0 to disable local audio loopback from v6
/V2O/bin/maint dreg -r 0x894 -W 0x0

#delay for counters to settle down
sleep 1

# Read the RX bandwidth counter to check the data is received from the network
echo "RX bandwidth counter value should increase"
/V2O/bin/maint dreg -r 0x8a8 -R -c 10 -d 100

# Read the mismatch counter 10 times to see if the value changes
echo "Read audio data mismatch counter. This counter is valid only if the above counter increments." 
echo "This value should not increase."
/V2O/bin/maint dreg -r 0x8ac -R -c 10 -d 100

#replace back config reg
/V2O/bin/maint dreg -r 0x80 -W ${INITVAL}
