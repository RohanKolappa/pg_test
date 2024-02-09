#!/bin/sh

ip=$1

if [ x$ip = "x" ] ; then
    echo
    echo "Provide segmentation fault instruction pointer on the command line."
    echo
    echo "For example for a segfault message such as:"
    echo "segfault at dd ip 08074ce6 sp b4d470d0 error 4 in StreamingServer[8048000+91000]"
    echo 
    echo "The instruction pointer is the string after \"ip\" that is \"08074ce6\"."
    echo
    exit 1
fi

# Strip leading zeros from instruction pointer
ip=`echo ${ip} | /bin/sed s/^0*//`

# Create full objectdump of StreamingServer executable
skip=$2
if [ x$skip != "xskip" ] ; then
   echo "Extracting debug info from StreamingServer executable."
   echo "This will take a while..."
   objdump -DClS StreamingServer >| STRMSRV_DEBUG
fi

# grep for instruction pointer in the object dump
# showing 40 lines of surrounding context


grep -n -A 20 -B 20 ${ip} STRMSRV_DEBUG

if [ x$? =  "x1" ] ; then
    echo "Sorry... could not find ${ip} in the debug dump file STRMSRV_DEBUG"
    exit 1
fi
