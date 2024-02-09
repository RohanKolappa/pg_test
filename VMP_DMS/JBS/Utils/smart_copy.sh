#!/bin/bash

function usage {

	echo "Usage: ${progname} <file_pattern> <old_token> <new_token>"
	echo ""
	echo "Example:"
	echo "        ${progname} MySourceClass Source Final"
	echo "would copy and convert:"
	echo "        NS.MySourceClass.cpp ---> NS.MyFinalClass.cpp"
	echo "        NS.MySourceClass.h ---> NS.MyFinalClass.h"
	echo "        NS.MySourceClassFactory.cpp ---> NS.MyFinalClassFactory.cpp"
	echo "        NS.MySourceClassFactory.h ---> NS.MyFinalClassFactory.h"
	echo "        ..."
}

progname=`basename $0`

if [ $# -ne 3 ]
then
	usage
	exit 1
fi

FILE_PATTERN=$1
OLD_TOKEN=$2
NEW_TOKEN=$3

FILES_LIST=`ls *${FILE_PATTERN}*.c *${FILE_PATTERN}*.cpp *${FILE_PATTERN}*.h *${FILE_PATTERN}*.java 2> /dev/null`

for f in ${FILES_LIST}
do

	OLD_FILENAME=${f}
	NEW_FILENAME=${f/${OLD_TOKEN}/${NEW_TOKEN}}

	if [ -f ${NEW_FILENAME} ]
	then
		echo "File \"${NEW_FILENAME}\" already exist! Remove it first!"
		continue
	fi

	echo "Converting ${OLD_FILENAME} ---> ${NEW_FILENAME}..."

	cat ${OLD_FILENAME} | sed -e "s/${OLD_TOKEN}/${NEW_TOKEN}/g" > ${NEW_FILENAME}

done

