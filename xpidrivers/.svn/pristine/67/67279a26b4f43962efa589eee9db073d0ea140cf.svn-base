# This code assumes testaudio.sh has been already run.
# This code changes the input to line in.
# Set the page to 1 
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x01                                    
# Route the IN1L to left MICPGA Amp positive terminal with 20k resistance^M     
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x34 -W 0x80                                    
# Route the IN1R to right MICPGA Amp positive terminal with 20k resistance^M     
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x37 -W 0x80                                    
