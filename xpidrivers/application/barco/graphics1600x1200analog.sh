##Graphics - UXGA, 1600x1200##
:Graphics-8.2 1600x1200 _@ 60 162.000MHz Out through DVI:
50 20 10 ; FPGA HPD on TX
40 0C 42 ; Power up part and Power down VDP
40 00 16 ; VID_STD=00111b for autographics mode
40 01 82 ; Enable Simultaneous Mode ,Prim_Mode =0010b for GR
40 02 F2 ; Auto CSC , RGB Out
40 03 40 ; 24 Bit SDR
40 05 28 ; TURN OFF EAV & SAV CODES & Enable Freerun
40 06 A6 ; invert Hs and VS for compliance to spec and In LLC
40 0B 44 ; Power down ESDP block
40 14 7F ; Drive strength adjustment
40 15 90 ; Disable Tristate of Pins except for Audio pins
40 33 60 ; LLC DLL Mux Enable
44 3E 04 ; Enable gain control for CP
44 40 5C ; Adjust CP gain for PGA gain of 1.4
44 69 30 ; Enable CP CSC
40 33 40 ; llc MUX……..Maybe removed. Write was added probabaly to get old TX board working. Pls check before removing
44 85 0B ; Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use seperate HS & VS.
44 C3 39 ; ADI recommended write
44 CF 01 ; Power off macrovision
4C 13 93 ; Set LLC DLL Phase
4C 02 02 ; Ain_Sel to 010. (Ain 7,8,9)
4C 05 07 ; AA filter Enable
4C 06 20 ; Set AA_filt_high_BW to 11
4C 07 A0 ; Set AA_filt_high_BW to 11 & aa_filt_prog_bw to 01
4C 12 63 ; Turn Noise shaping OFF
4C 0C 1F ; Set PGA gain to 1.4
4C 13 86 ; DLL on LLC
4C C6 5F ; Disable Audio Core
50 19 01 ; FPGA ADV7604
50 10 05 ; FPGA MUX to enable 36 bit
50 20 0F ; FPGA HPD All Ports
72 01 00 ; Set 'N' value at 6144
72 02 18 ; Set 'N' value at 6144
72 03 00 ; Set 'N' value at 6144
72 15 00 ; 24-bit, 444 RGB input
72 16 00 ; RGB 444
72 18 46 ; Disable CSC
72 40 80 ; General control packet enable
72 41 10 ; Power down control
72 49 A8 ; Set dither mode - 12-to-10 bit
72 96 20 ; HPD interrupt clear
72 56 08 ; Set active format aspect
72 98 03 ; ADI recommended write
72 99 02 ; ADI recommended write - lock count limit
72 9C 30 ; PLL filter R1 value
72 9D 61 ; Set clock divide
72 A2 A4 ; ADI recommended write
72 A3 A4 ; ADI recommended write
72 AF 14 ; Select DVI mode
72 BA A0 ; Set TX Clock Delay
72 DE 9C ; ADI recommended write
72 E4 60 ; VCO_Swing_Reference_Voltage
72 FA 7D ; Nbr of times to look for good phase
50 20 00 ; HPD
End
