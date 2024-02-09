#!/bin/bash

read -e -p "Erase all local volumes ? " -t 2 answ || answ=yes

echo ""
echo ""
echo "Erasing all local volumes..."
echo ""
echo "Please wait until the operation completes."
echo ""

c=0

while [ true ]
do

	echo -n "-" ; echo -e -n "\b" ; sleep 0.05
	echo -n "\\" ; echo -e -n "\b" ; sleep 0.05
	echo -n "|" ; echo -e -n "\b" ; sleep 0.05
	echo -n "/" ; echo -e -n "\b" ; sleep 0.05

	echo -n "."

	let c=$c+1
	if [ $c -ge 40 ] ; then break ; fi

done

echo ""
echo "Done!"
echo ""
echo "Thank you for using JBS!"


