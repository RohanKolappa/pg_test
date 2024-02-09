# This script is intended to be run on the tester
# It tests the sync connection electronics of the DUT

#issue soft reset
/V2O/bin/maint dreg -r 0x840 -W 0x10
usleep 1
/V2O/bin/maint dreg -r 0x840 -W 0x00

#Enable counter data by 1 and compare receive data
/V2O/bin/maint dreg -r 0x840 -W 0x3

# Read the input data valid counter 10 times to see if the value changes
echo "This value should increase if the sync cable is connected"
for i in `seq 1 10`
do 
       /V2O/bin/maint dreg -r 0x8c8 -R
done
echo ""
# Read the mismatch counter 10 times to see if the value changes
echo "Read sync input data mismatch counter. This counter is valid only if valid counter increments." 
echo "This should not increase."
for i in `seq 1 10`
do 
       /V2O/bin/maint dreg -r 0x844 -R
done

# measurement of received clock from dut 
#echo "Measurement of clock. This counter is valid only if valid counter increments." 
#/V2O/bin/maint dreg -o -r 0x21 -W 1
#/V2O/bin/maint dreg -o -r 0x21 -W 0
#sleep 2
#echo "Ref1 clock count 62.5MHz"
#/V2O/bin/maint dreg -o -r 0x22 -R
#echo "Sync clock count - 200MHz"
#/V2O/bin/maint dreg -o -r 0x24 -R

#disable test
/V2O/bin/maint dreg -r 0x840 -W 0x0
