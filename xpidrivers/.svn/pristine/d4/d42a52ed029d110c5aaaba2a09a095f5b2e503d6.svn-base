#this script measures the pixel clock at fanout fpga
#issue soft reset
/V2O/bin/maint dreg -6 -r 0x298 -W 0x1
/V2O/bin/maint dreg -6 -r 0x298 -W 0x0
sleep 1
echo "reference clock count - 100MHz"
/V2O/bin/maint dreg -6 -r 0x29c -R
echo "pixel clock count"
/V2O/bin/maint dreg -6 -r 0x2a0 -R
echo "dvib clock count"
/V2O/bin/maint dreg -6 -r 0x2a8 -R
echo "measured clock value = respective clock count/ref clock count*ref_clk_freq"
