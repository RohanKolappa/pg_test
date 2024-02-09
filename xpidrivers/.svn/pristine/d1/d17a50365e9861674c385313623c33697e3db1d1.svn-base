# This script read crc at various places to do video mfg test

#crc - output path
echo "OUTPUT PATH"
echo "CRC Even From Fanout FPGA - output side"
for i in `seq 1 5`
do
	/V2O/bin/maint dreg -6 -r 0xc60 -R
done	
echo "CRC Odd From Fanout FPGA - output side"
for i in `seq 1 5`
do
	/V2O/bin/maint dreg -6 -r 0xc64 -R
done


#crc at input of fanout fpga
echo ""
echo ""
echo "INPUT PATH"
echo "CRC Even From Fanout FPGA - input side"
for i in `seq 1 5`
do
	/V2O/bin/maint dreg -6 -r 0xc58 -R
done	
echo "CRC Odd From Fanout FPGA - input side"
for i in `seq 1 5`
do
	/V2O/bin/maint dreg -6 -r 0xc5c -R
done
exit
