# This script is to be run on the tester box for stereo testing
# Display mismatch counter value. 
# This should remain constant if a crossed stereo cable is connected.
# To enable counter data increment by 1 and compare receive data

# issue soft reset to stereo test module
/V2O/bin/maint dreg -6 -r 0x648 -W 0x10
/V2O/bin/maint dreg -6 -r 0x648 -W 0x0

#enable stereo test, verification
/V2O/bin/maint dreg -6 -r 0x648 -W 0x23
echo ""
echo "Read data valid counter. This should increase."
for i in `seq 1 10`
do 
	/V2O/bin/maint dreg -6 -r 0x650 -R
done

echo ""
echo "Read mismatch counter. This is valid only if the counter above increases." 
echo "This counter should not increase."
for i in `seq 1 10`
do 
	/V2O/bin/maint dreg -6 -r 0x64c -R
done
	
#disable test
/V2O/bin/maint dreg -6 -r 0x648 -W 0x0
