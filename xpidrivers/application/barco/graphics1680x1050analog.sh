#!/bin/sh
#ADV7604IO_ADV7604CP_ADV7604AFE_ADV7604HDMI_ADV7341B-VER.1.4c_RevA.txt
##Graphics - WSXGA+, 1680x1050##
#:Graphics-7.1 1680x1050 _@ 60 _MHz Out through DAC:
/V2O/bin/maint i2creg -a 0x20 -r 0x0C -W 0x42 # Power up part and Power down VDP
/V2O/bin/maint i2creg -a 0x20 -r 0x00 -W 0x18 # VID_STD=11000b for 1680x1050 _@ 60
/V2O/bin/maint i2creg -a 0x20 -r 0x01 -W 0x82 # Enable Simultaneous Mode,Prim_Mode=0010b for RGB-GR
/V2O/bin/maint i2creg -a 0x20 -r 0x02 -W 0xF2 # Auto CSC, RGB Out
/V2O/bin/maint i2creg -a 0x20 -r 0x03 -W 0x42 # 36 Bit SDR 444 Mode 0
/V2O/bin/maint i2creg -a 0x20 -r 0x05 -W 0x28 # TURN OFF EAV & SAV CODES & Enable Freerun
#maint i2creg -a 0x20 -r 0x06 -W 0xA1 # Inv LLC
/V2O/bin/maint i2creg -a 0x20 -r 0x06 -W 0xA6 # Inv HS and VS 
/V2O/bin/maint i2creg -a 0x20 -r 0x0B -W 0x44 # Power down ESDP block
/V2O/bin/maint i2creg -a 0x20 -r 0x14 -W 0x7F # Drive strength adjustment
/V2O/bin/maint i2creg -a 0x20 -r 0x15 -W 0x90 # Disable Tristate of Pins except for Audio pins
/V2O/bin/maint i2creg -a 0x20 -r 0x33 -W 0x60 # LLC DLL Mux Enable
i2creg -a 0x20 -r 0xb3E -W 0x04 # Enable gain control for CP
i2creg -a 0x20 -r 0xb40 -W 0x5C # Adjust CP gain for PGA gain of 1.4
i2creg -a 0x20 -r 0xb69 -W 0x30 # Enable CP CSC 
i2creg -a 0x20 -r 0xb85 -W 0x0B # Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use seperate HS & VS.
i2creg -a 0x20 -r 0xbC3 -W 0x39 # ADI recommended write
i2creg -a 0x20 -r 0xbCF -W 0x01 # Power off macrovision
i2creg -a 0x20 -r 0x613 -W 0x93 # Set LLC DLL Phase
i2creg -a 0x20 -r 0x602 -W 0x02 # Ain_Sel to 010. (Ain 7,8,9)
i2creg -a 0x20 -r 0x605 -W 0x07 # AA filter Enable
i2creg -a 0x20 -r 0x606 -W 0x20 # Set AA_filt_high_BW to 11
i2creg -a 0x20 -r 0x607 -W 0xA0 # Set AA_filt_high_BW to 11 & aa_filt_prog_bw to 01
i2creg -a 0x20 -r 0x612 -W 0x63 # Turn Noise shaping OFF (reg 12 not in AFE map)
i2creg -a 0x20 -r 0x60C -W 0x1F # Set PGA gain to 1.4 (reg 0c not in AFE map)
i2creg -a 0x20 -r 0x6C6 -W 0x5F # Disable Audio Core (reg c6 not in AFE map)
