echo "Input is Line In. Output is available on Line Out and Headphone."
# This code assumes an MCLK of 33MHz provided by the FPGA
# This code generates a BCLK of 1.02MHz and a WCLK of 16kHz

# Initialize the device through software reset
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x01 -W 0x01
# Initialize to Page 0
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x00
# Power up the NADC divider with value 8
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x12 -W 0x88
# Power up the MADC divider with value 2
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x13 -W 0x82
# Program the OSR of ADC to 128
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x14 -W 0x80
#-------
# NDAC divider of 16
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0b -W 0x90
# Power up the MDAC divider with value 1
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0c -W 0x81
# Program the OSR of DAC to 128
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0d -W 0x00
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0e -W 0x80
#-------
# Set the BDIV_CLKIN Mux input to ADC_CLK and disable loopback 
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x1d -W 0x06
# Power up the N-Divider with value 4
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x1e -W 0x84
# Configure WCLK to come from generated ADC_FS
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x21 -W 0x10
# Configure WCLK and BCLK as outputs and data word length as 16
#and left justified
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x1b -W 0xcc
#offset 1 bitclk
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x1c -W 0x01
# Disable Internal Crude AVdd in presence of external AVdd supply or before
# powering up internal AVdd LDO
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x01
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x01 -W 0x08
# Enable Master Analog Power Control
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x02 -W 0x00
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x00
#-------
# 
# Audio routing 
# Power on the left and right ADC channels
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x51 -W 0xc0
# Unmute left and right ADC Digital volume control
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x52 -W 0x00 
# Change the page to 1
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x01
# Route the IN1L to left MICPGA Amp positive terminal with 20k resistance
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x34 -W 0x80
# Route the Common Mode to left MICPGA Amp negative terminal with 20k resistance
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x36 -W 0x80
# Route the IN1R to right MICPGA Amp positive terminal with 20k resistance
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x37 -W 0x80
# Route the Common Mode to right MICPGA Amp negative terminal with 20k resistance
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x39 -W 0x80

# Unmute left MICPGA with gain selection of 4.5dB
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x3b -W 0x09
# Unmute right MICPGA with gain selection of 4.5dB
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x3c -W 0x09
# Audio output routing
# Power DAC channels and route inputs to audio interface data
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x00
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x3f -W 0xd6
# Unmute DAC channels
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x40 -W 0x00

# Set page to 1
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x01
# Route LDAC to LOL
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0e -W 0x08
# Route RDAC to LOR
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0f -W 0x08 
# Route output of LDAC to HPL
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0c -W 0x08
# Route output of RDAC to HPR
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0d -W 0x08

# Unmute LOL driver with gain of 0dB
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x12 -W 0x00
# Unmute LOR driver with gain of 0dB
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x13 -W 0x00 

# Unmute HPL driver and set gain to 0dB
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x10 -W 0x00
# Unmute HPR driver and set gain to 0dB
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x11 -W 0x00
# Power on Mixer Amplifier, Line outputs and Headphone drivers.
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x09 -W 0x3f
