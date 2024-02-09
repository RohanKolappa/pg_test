# This code assumes testaudio.sh has been already run.
# This code changes the input to mic.
# Change the page to 1
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x01                                    
# Route the IN3L to left MICPGA Amp positive terminal with 20k resistance^M     
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x34 -W 0x08                                    
# Route the IN3R to right MICPGA Amp positive terminal with 20k resistance^M     
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x37 -W 0x08                                    
