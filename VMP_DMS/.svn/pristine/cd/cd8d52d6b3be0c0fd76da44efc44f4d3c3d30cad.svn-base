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

if [ $# -ne 2 ]
then
	echo "Usage: $0 <old_token> <new_token>"
	exit 1
fi

if ( ! which TokenModifier > /dev/null 2>&1 )
then
	echo "Can't find TokenModifier!"
	exit 1
fi

old_token=$1
new_token=$2

list=`find . -name "*.cpp" -o -name "*.h" -o -name "*.hpp" -o -name "*.hh" -o -name "*.C" -o -name "*.c" -o -name "*.java" -o -name "*.as"`

for fname in $list
do

	grep "$old_token" "$fname" > /dev/null 2> /dev/null

	if [ $? -eq 0 ]
	then
		echo "Converting in $fname..."
		nname=$fname.back
		mv $fname $nname
		TokenModifier "$old_token" "$new_token" < $nname > $fname
#		rm $nname
	fi

done



