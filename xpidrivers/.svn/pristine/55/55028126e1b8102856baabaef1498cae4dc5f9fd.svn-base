#loadvideodrivers.sh 
/V2O/bin/maint dreg -3 -r 0x10 -R
/V2O/bin/maint dreg -3 -r 0x10 -W 0x3c2
/V2O/bin/maint dreg -3 -r 0x10 -R
/V2O/bin/maint dreg -3 -r 0x18 -R
/V2O/bin/maint dreg -3 -r 0x18 -W 0xcd
/V2O/bin/maint dreg -3 -r 0x18 -R
/V2O/bin/maint seteeprom -p 2 -a 0x50 -f /V2O/bin/USBEEPROM_HOST.dat
/V2O/bin/maint printeeprom -p 2 -a 0x50
/V2O/bin/maint seteeprom -p 7 -a 0x50 -f /V2O/bin/USBEEPROM_DEV.dat
/V2O/bin/maint printeeprom -p 7 -a 0x50
/V2O/bin/maint dreg -3 -r 0x18 -W 0x14d
/V2O/bin/maint dreg -3 -r 0x18 -R
