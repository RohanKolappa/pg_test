#resets crc error counters in fanout fpga
/V2O/bin/maint dreg -6 -r 0xc -W 4
/V2O/bin/maint dreg -6 -r 0xc -W 0
#resets crc error counters in main fpga
/V2O/bin/maint dreg -r 0xc50 -W 1
/V2O/bin/maint dreg -r 0xc50 -W 0
