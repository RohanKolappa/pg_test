#!/bin/bash

#################################################################
##
## script to find .jbs.noenter and .jbs.nobuild files
##
## Usage: jbs_exclude_finder.sh [0|1] [0|1]
##	
##	- first argument:	exclude files in dirs not under version control
##	- second argument:	exclude generated files by the building system
##
##	both arguments default to 1
##
#################################################################

#defaults
exclude_non_versioned_dirs=1
exclude_generated_files=1

if [ $# -gt 0 ]
then
	exclude_non_versioned_dirs=$1
fi

if [ $# -gt 1 ]
then
	exclude_generated_files=$2
fi

list=`find . \( -name "\\.jbs\\.noenter" \) -o \( -name "\\.jbs\\.nobuild" \)`

for f in $list
do

	fdir="`dirname $f`"
	fnam="`basename $f`"

	dirnam="`basename $fdir`"

	# exclude files in dirs not under version control
	if [ $exclude_non_versioned_dirs -eq 1 ]
	then
		if ! (svn info $fdir &> /dev/null)
		then
			continue
		fi
	fi

	# skip well known generated files
	if [ $exclude_generated_files -eq 1 ]
	then
		if [ "$dirnam" = ".jbs" ] || [ "$dirnam" = "Doc" ]
		then
			continue
		fi
	fi

	if ! (svn info $f &> /dev/null)
	then
		echo "$f"
	fi

done
