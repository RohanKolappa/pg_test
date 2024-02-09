# This script read crc at various places to do video mfg test

#read crc at fanout fpga
echo "FANOUT_FPGA"
echo "CRC Even From Fanout FPGA"
for i in `seq 1 5`
do
	/V2O/bin/maint dreg -6 -r 0xc58 -R
done	
echo "CRC Odd From Fanout FPGA"
for i in `seq 1 5`
do
	/V2O/bin/maint dreg -6 -r 0xc5c -R
done

#read crc at main fpga
echo ""
echo ""
echo "MAIN FPGA"
echo "CRC Even From Main FPGA"
for i in `seq 1 5`
do
       /V2O/bin/maint dreg -r 0xc58 -R
done
echo "CRC Odd From Main FPGA"
for i in `seq 1 5`
do
       /V2O/bin/maint dreg -r 0xc5c -R
done
