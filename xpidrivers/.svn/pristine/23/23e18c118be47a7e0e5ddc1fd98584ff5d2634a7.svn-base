while (true)
do
echo "crc main fpga"
/V2O/bin/maint dreg -r 0xc58 -R
/V2O/bin/maint dreg -r 0xc5c -R
echo "link crc fanout fpga"
/V2O/bin/maint dreg -6 -r 0xc14 -R
/V2O/bin/maint dreg -6 -r 0xc18 -R
echo "display crc fanout fpga"
/V2O/bin/maint dreg -6 -r 0xc60 -R
/V2O/bin/maint dreg -6 -r 0xc64 -R
sleep 4
echo ""
done
