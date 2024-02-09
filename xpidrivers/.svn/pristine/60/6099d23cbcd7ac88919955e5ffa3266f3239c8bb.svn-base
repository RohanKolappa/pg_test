echo "ENCODER Starts"
echo "ENCODER - Video in - To Test S6-V6 Link"
echo "CRC Even From Fanout FPGA"
/V2O/bin/maint dreg -6 -r 0xc58 -R
echo "CRC Even From Main FPGA"
/V2O/bin/maint dreg -r 0xc58 -R
echo""
echo "CRC Odd From Fanout FPGA"
/V2O/bin/maint dreg -6 -r 0xc5c -R
echo "CRC Odd From Main FPGA"
/V2O/bin/maint dreg -r 0xc5c -R
echo "ENCODER ENDS"

echo ""
echo "DECODER Starts"
echo "Remote Video CRC to test V6-S6 Link"
echo "CRC Even From Main FPGA"
/V2O/bin/maint dreg -r 0xc58 -R
echo "CRC Even From Fanout FPGA"
/V2O/bin/maint dreg -6 -r 0xc14 -R
echo ""
echo "CRC Odd From Main FPGA"
/V2O/bin/maint dreg -r 0xc5c -R
echo "CRC Odd From Fanout FPGA"
/V2O/bin/maint dreg -6 -r 0xc18 -R

#echo ""
#echo "CRC of Video just before display - Out CRC"
#echo "CRC Even of out video just before display - Fanout Fpga"
#/V2O/bin/maint dreg -6 -r 0xc60 -R
#echo "CRC Odd of out video just before display - Fanout Fpga"
#/V2O/bin/maint dreg -6 -r 0xc64 -R
#echo""

#echo "IN CRC in decoder mode"
#echo "CRC Even of input video(local) From Fanout FPGA in dec mode"
#/V2O/bin/maint dreg -6 -r 0xc58 -R
#echo "CRC Odd of input video(local) From Fanout FPGA in dec mode"
#/V2O/bin/maint dreg -6 -r 0xc5c -R
echo "DECODER ENDS"
