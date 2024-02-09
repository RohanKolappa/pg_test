#!/bin/sh

if [ ! -f /etc/config/arm_system ] ; then

    # Device serial number currently derived from the mac address.

    # We take the first 7 bytes of the 16 byte hash of mac address and
    # convert each of them to an integer representation modulo 100. These
    # integers are zero padded (so that each integer is 2 digits) and
    # appended to each other to get a 14 digit serial number

    mac_address=`ifconfig -a eth0 | grep HWaddr | sed -e 's/.*HWaddr \(.*\)/\1/'`

    hash_of_mac=`echo $mac_address | md5sum`

    field_0=`echo ${hash_of_mac} | cut -c 1,2`
    field_0=`printf "%d" 0x${field_0}`
    field_0=`expr $field_0 \% 100`

    field_1=`echo ${hash_of_mac} | cut -c 3,4`
    field_1=`printf "%d" 0x${field_1}`
    field_1=`expr $field_1 \% 100`

    field_2=`echo ${hash_of_mac} | cut -c 5,6`
    field_2=`printf "%d" 0x${field_2}`
    field_2=`expr $field_2 \% 100`

    field_3=`echo ${hash_of_mac} | cut -c 7,8`
    field_3=`printf "%d" 0x${field_3}`
    field_3=`expr $field_3 \% 100`

    field_4=`echo ${hash_of_mac} | cut -c 9,10`
    field_4=`printf "%d" 0x${field_4}`
    field_4=`expr $field_4 \% 100`

    field_5=`echo ${hash_of_mac} | cut -c 11,12`
    field_5=`printf "%d" 0x${field_5}`
    field_5=`expr $field_5 \% 100`

    field_6=`echo ${hash_of_mac} | cut -c 13,14`
    field_6=`printf "%d" 0x${field_6}`
    field_6=`expr $field_6 \% 100`

    format_list="${field_0} ${field_1} ${field_2} ${field_3} ${field_4} ${field_5} ${field_6}"
    serial_number=`printf "%02d%02d%02d%02d%02d%02d%02d" $format_list`

    echo $serial_number > /etc/config/device_serial_number
    echo $serial_number

else

    device_serial_number=`fw_printenv AssemblySerialNumber | cut -d '=' -f2`
    echo $device_serial_number > /etc/config/device_serial_number

fi
