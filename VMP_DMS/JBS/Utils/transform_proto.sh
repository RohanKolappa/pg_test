#!/bin/sh

###############################################################################
###                                                                         ###
### Copyright (c) 2003-2010 Barco N.V.                                      ###
###                                                                         ###
###############################################################################
###                                                                         ###
### This file is part of "JBS".                                             ###
###                                                                         ###
### "JBS" is free software; you can redistribute it and/or modify it        ###
### under the terms of the GNU Lesser General Public License as published   ###
### by the Free Software Foundation; either version 2.1 of the License, or  ###
### (at your option) any later version.                                     ###
###                                                                         ###
### "JBS" is distributed in the hope that it will be useful, but            ###
### WITHOUT ANY WARRANTY; without even the implied warranty of              ###
### MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       ###
### Lesser General Public License for more details.                         ###
###                                                                         ###
### You should have received a copy of the GNU Lesser General Public        ###
### License along with "JBS"; if not, write to the Free Software            ###
### Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  2111-1307 USA ###
###                                                                         ###
###############################################################################

if [ $# -ne 3 ]
then
	echo "Usage: $0 <old_token> <nbr_args> <new_token>"
	exit 1
fi

if ( ! which ProtoModifier > /dev/null 2>&1 )
then
	echo "Can't find ProtoModifier!"
	exit 1
fi

old_token=$1
nbr_args=$2
new_token=$3

list=`find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.C" -o -name "*.c"`

for fname in $list
do

	grep "$old_token" "$fname" > /dev/null 2> /dev/null

	if [ $? -eq 0 ]
	then
		echo "Converting in $fname..."
		nname=$fname.back
		mv $fname $nname
		ProtoModifier "$old_token" "$nbr_args" "$new_token" < $nname > $fname
#		rm $nname
	fi

done



