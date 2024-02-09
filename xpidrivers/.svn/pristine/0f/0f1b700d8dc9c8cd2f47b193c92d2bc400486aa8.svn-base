#set phy to sgmii to copper mode
/V2O/bin/maint mdioreg -p 1 -r 0x16 -W 0x12
/V2O/bin/maint mdioreg -p 1 -r 0x14 -W 0x8001
#set phy to autoneg mode
/V2O/bin/maint mdioreg -p 1 -r 0x16 -W 0x1
/V2O/bin/maint mdioreg -p 1 -r 0x1a -W 0x46
#set phy for serdes loopback
/V2O/bin/maint mdioreg -p 1 -r 0x10 -W 0xc00e
/V2O/bin/maint mdioreg -p 1 -r 0x00 -W 0x1340
