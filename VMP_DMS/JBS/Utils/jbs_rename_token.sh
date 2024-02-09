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

if [ $# -lt 2 ]
then
	echo "Usage: $0 <old_token> <new_token>"
	exit 1
fi

old_token="$1"
new_token="$2"

if [ $# -gt 2 ]
then
	shift
	shift
	list="$@"
else
	list=`find . -type f -not -path './*/.svn' \
		'(' -name "*.cpp" -o -name "*.cc" -o -name "*.cxx" \
		-o -name "*.h" -o -name "*.hpp" -o -name "*.hh" -o -name "*.hxx" \
		-o -name "*.C" -o -name "*.c" \
		-o -name "*.java" \
		-o -name "*.as" \
		-o -name "*.ui" \
		-o -name "Make.target" -o -name "Makefile" ')' `
fi

# echo "Renaming in $list"
# exit 0

# echo "Pattern length: ${#old_token}"

for f in $list
do

	fdir="`dirname $f`"
	fnam="`basename $f`"

	# filename ?

	echo "$fnam" | egrep "$old_token" > /dev/null 2> /dev/null

	if [ $? -eq 0 ]
	then
		# rename !
		fname="$fdir"/`echo "$fnam" | sed -e "s/$old_token/$new_token/"`
		echo "Renaming $f to $fname..."
		( svn info ${f} | grep Revision ) > /dev/null 2>&1
		if [ $? -eq 0 ]
		then
			( cd ${fdir} && svn commit -m "File renaming." ${fnam} )
			svn move "$f" "$fname"
		else
			mv "$f" "$fname"
		fi
	else
		fname="$f"
	fi

	# file content ?

#	echo "Testing ${fname}..."

	cat "$fname" | egrep "$old_token" > /dev/null 2> /dev/null

	if [ $? -eq 0 ]
	then
		echo "Converting in $fname..."
		nname=$fname.new
#		mv -f $fname $nname
		( cat $fname | sed -r -e "s/$old_token/$new_token/g" > $nname ) && mv -f $nname $fname
#		rm $nname
	fi

done



