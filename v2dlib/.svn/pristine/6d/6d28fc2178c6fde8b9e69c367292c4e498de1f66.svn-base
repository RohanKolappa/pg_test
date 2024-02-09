#!/bin/sh

ip=$1
base=$2
if [ x$ip = "x" -o x$base = "x" ] ; then
    echo
    echo "Provide segmentation fault instruction pointer and"
    echo "libv2d.so library base offset on the command line."
    echo
    echo "For example for a segfault message such as:"
    echo "segfault at 0 ip b7689cc5 sp b50fe090 error 4 in libv2d.so[b7675000+40000]"
    echo 
    echo "The instruction pointer is the string after \"ip\" that is \"b7689cc5\"."
    echo "The library base offset is the first value in braces that is \"b7675000\"."
    echo
    exit 1
fi

# Strip leading zeros from instruction pointer
ip=`echo ${ip} | /bin/sed s/^0*//`
base=`echo ${base} | /bin/sed s/^0*//`

# Covert to upper case (bc needs upper case)
ip=`echo "$ip" | tr '[a-f]' '[A-F]'`
base=`echo "$base" | tr '[a-f]' '[A-F]'`

diff=`echo "obase=16; ibase=16; $ip - $base" | bc` 

# Covert to result back to lower case
diff=`echo "$diff" | tr '[A-F]' '[a-f]'`

# Create full objectdump of the libv2d.so file
skip=$3
if [ x$skip != "xskip" ] ; then
   echo "Extracting debug info from  libv2d.so"
   echo "This will take a while..."
   objdump -DClS libv2d.so >| LIBV2D_DEBUG
fi

# grep for diff string in the object dump
# showing 40 lines of surrounding context

echo "Looking for  ${diff}: in the object dump..."
echo

grep -n -A 20 -B 20 "${diff}:" LIBV2D_DEBUG

if [ x$? =  "x1" ] ; then
    echo "Sorry... could not find ${diff}: in the debug dump file LIBV2D_DEBUG"
    exit 1
fi
