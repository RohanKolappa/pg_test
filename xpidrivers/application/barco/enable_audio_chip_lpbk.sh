# Set page to 1
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x00 -W 0x1
echo "Enabling audio loopback through the codec.."
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0e -W 0x2
/V2O/bin/maint i2creg -x -p 0 -a 0x18 -r 0x0f -W 0x2
