#!/bin/sh
if [ -f /sys/class/gpio/gpio$1 ]; then
    echo $1 > /sys/class/gpio/unexport
fi
echo $1 > /sys/class/gpio/export
cat /sys/class/gpio/gpio$1/direction
cat /sys/class/gpio/gpio$1/value
echo $1 > /sys/class/gpio/unexport
