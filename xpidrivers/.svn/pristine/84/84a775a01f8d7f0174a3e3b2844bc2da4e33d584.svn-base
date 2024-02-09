# This script is to check received audio path.
#toggle audio enable in rx direction
INITVAL=`/V2O/bin/maint dreg -r 0x80 -R | cut -d"=" -f 3`
/V2O/bin/maint dreg -r 0x80 -W 0x0
/V2O/bin/maint dreg -r 0x80 -W 0x4
#Disbale audio mixer - bit 7
/V2O/bin/maint dreg -r 0x27C -W 0x0

# Bit 2 should be 0 to receive data from network
# Bit 7 should be 0 to disable local audio loopback from v6
/V2O/bin/maint dreg -r 0x894 -W 0x0

# Enable audio pattern verification in expansion
echo "Audio pattern verification is enabled"
#disable and enable back to clean counters
/V2O/bin/maint dreg -3 -r 0x20 -W 0
/V2O/bin/maint dreg -3 -r 0x20 -W 1

#delay so that mismatch count settles down
sleep 1

# Read the RX bandwidth counter to check the data is received from the network
echo "RX bandwidth counter value should increase"
/V2O/bin/maint dreg -r 0x8a8 -R -c 10 -d 100

# Read the mismatch counter 10 times to see if the value changes
echo "Read audio data mismatch counter. This counter is valid only if the above counter increments." 
echo "This value should not increase."
/V2O/bin/maint dreg -r 0x8ac -R -c 10 -d 100

# Read the audio mismatch counter in expansion 10 times to see if the value changes
echo "Read expansion audio mismatch counter"
echo "This value should not increase"
/V2O/bin/maint dreg -3 -r 0x1c -R -c 10 -d 100

#enable audio mixer back - bit 7
/V2O/bin/maint dreg -r 0x27C -W 0x80
#replace config reg
/V2O/bin/maint dreg -r 0x80 -W ${INITVAL}
