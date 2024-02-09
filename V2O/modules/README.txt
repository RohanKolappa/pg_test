V2D software release V2.1-b05 has the following changes
from V1.4.2-b08:

(1) added a "Charge Pump Curr" field to the LUT table
    so users can change it directly (Bug #916)

(2) changed the lower limit for refresh rate to 25 (Hz)
    (Bug #915)

(3) added a "Splash Parameters" to the "Configuration"
    menu for TX and RX to allow the use to select the
    type of splash screen to use (Bug #932) and to 
    change video parameters for the splash screen (Bug
    #537).

(4) Splash screen now uses 2 png files: one shows the ipvideo
    logo, the other shows "V2D TX" or "V2D RX". The png files
    is 300x128 pixels, sample depth 8 bits, color type 6 (color
    depth 32 or true color and alpha), interlace 0, compression 0,
    filter 0, rowbytes 1200.

(5) Removed "Keyboard Type" from the "Video and IO Parameters" 
    menu on the RX. Added "Keyboard Type" and "Mouse Type" to 
    the "Connection Parameters" menu on the RX.

(6) The wheel on Intellimouse now works (if the keyboard type is
    PC. There's a bug in the FPGA that always put the mouse into
    PS2 mode if the keyboard is SGI.) 

(7) Added "Video Lookup Table" to the Configuration Menu on the 
    RX also added a parameter "Local LUT" to the "Video and IO
    Parameters" menu on the RX. When "Local LUT" is turned on,
    the RX will override the video parameters sent from the TX 
    with those in the local LUT entry with matching LUT ID.
   
(8) Fixed a memory leak where memory allocated to LUT entries
    (by parse_lut()) is not freed.

(9) Added two commands: "get_edid" retrieves the EDID data
    the TX FPGA is using, "set_edid_with_lut" to replace 
    Descriptor Block #1 (timing recommendation #1) with the
    LUT entry specified in the command.

(10) Added the ability for TX to set the FPGA to no-slice-drop for
     4 frames upon receiving a no-slice-drop request from the RX.

(11) Added "http_upgrade_software.sh" which uses http to retrieve 
     remote software image and "secure_upgrade_software.sh" which 
     uses scp to retrieve remote software image.

(12) Fixed Bug# 900, 1022, 1036, 1078, 1081 and 49.

(13) Removed all 8KHz types for audio since both averaging and
     re-sampling the 8KHz samples add noise to the audio. 

(14) For DVI inputs if the horizontal polarity in the LUT is
     positive (which means the DVI output from the ADC is
     negative and therefore oppositie to the analog output from
     the ADC since analog outputs from the ADC always have
     positive polarity), add horizontal sync width to the 
     user-specified horizontal shift when setting the FPGA
     horizontal delay register (FPGA_AVHDREG) in fpga_control.c
     (routine configure_addside_fpga()). 

(15) Added control packet statistics to the "Diagnostics->
     Display Control and IO Information" menu.
 
(16) Added a "DUMMYV2DTYPE" parameter to netconf.txt. You
     can use dummy hardware to emulate the V2D TX or RX 
     by setting this parameter to TX or RX, respectively.

