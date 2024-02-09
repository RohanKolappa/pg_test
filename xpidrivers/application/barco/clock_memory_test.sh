echo "starting clock measurements"
/V2O/bin/maint dreg -o -r 0x21 -W 1
/V2O/bin/maint dreg -o -r 0x21 -W 0
sleep 2
echo "Ref1 clock 62.5MHz"
/V2O/bin/maint dreg -o -r 0x22 -R
echo "100MHz"
/V2O/bin/maint dreg -o -r 0x23 -R
#echo "Sync clock"
#maint dreg -o -r 0x24 -R
echo "expansion clock - 66MHz"
/V2O/bin/maint dreg -o -r 0x25 -R
echo "MAC1 clock 125Mhz"
/V2O/bin/maint dreg -o -r 0x26 -R
echo "MAC2 clock 125Mhz"
/V2O/bin/maint dreg -o -r 0x27 -R
echo "pipeline clock 165Mhz"
/V2O/bin/maint dreg -o -r 0x28 -R
echo "PTP clock 125Mhz"
/V2O/bin/maint dreg -o -r 0x29 -R
echo "pixel clock"
/V2O/bin/maint dreg -o -r 0x2a -R
echo
echo "Ref2 clock 62.5MHz"
/V2O/bin/maint dreg -o -r 0x2b -R
echo "c0 mem clock 400MHz"
/V2O/bin/maint dreg -o -r 0x2d -R
echo "c0 read clock 400MHz"
/V2O/bin/maint dreg -o -r 0x2e -R
echo "c0 user clock 200MHz"
/V2O/bin/maint dreg -o -r 0x2f -R
echo "c1 mem clock 400MHz"
/V2O/bin/maint dreg -o -r 0x30 -R
echo "c1 read clock 400MHz"
/V2O/bin/maint dreg -o -r 0x31 -R
echo "c1 user clock 200MHz"
/V2O/bin/maint dreg -o -r 0x32 -R

echo "Running memory test"
echo "Trigger C0"
/V2O/bin/maint dreg -o -r 0x35 -W 0
/V2O/bin/maint dreg -o -r 0x35 -W 1
echo "Trigger C1"
/V2O/bin/maint dreg -o -r 0x37 -W 0
/V2O/bin/maint dreg -o -r 0x37 -W 1
sleep 1
/V2O/bin/maint dreg -o -r 0x36 -R 
/V2O/bin/maint dreg -o -r 0x38 -R 
echo "sleeping for 20 seconds"
sleep 20
echo "c0 status"
/V2O/bin/maint dreg -o -r 0x36 -R 
echo "c1 status"
/V2O/bin/maint dreg -o -r 0x38 -R 
