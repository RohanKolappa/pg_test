#!/bin/sh
echo 92 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio92/direction
echo 0 > /sys/class/gpio/gpio92/value
echo 1 > /sys/class/gpio/gpio92/value
echo 0 > /sys/class/gpio/gpio92/value

echo 33 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio33/direction
echo 0 > /sys/class/gpio/gpio33/value
echo 1 > /sys/class/gpio/gpio33/value

echo 32 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio32/direction
echo 0 > /sys/class/gpio/gpio32/value
echo 1 > /sys/class/gpio/gpio32/value

echo 36 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio36/direction
echo 0 > /sys/class/gpio/gpio36/value
echo 1 > /sys/class/gpio/gpio36/value

echo 92 > /sys/class/gpio/unexport
echo 33 > /sys/class/gpio/unexport
echo 32 > /sys/class/gpio/unexport
echo 36 > /sys/class/gpio/unexport
