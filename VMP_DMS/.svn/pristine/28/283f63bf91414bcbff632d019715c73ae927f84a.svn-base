#!/bin/bash

# Usage: $0 <dir>

### Sanity checks...

# ... usage

if [ $# -ne 1 ]
then
	echo "Usage: $(basename $0) <dirname>"
	exit 1
fi

# ... arguments

if [ ! -d $1 ]
then
	echo "Argument not a directory!"
	exit 2
fi

cd $1

# ... must be a JBS folder

make dumpvars > /dev/null 2>&1

if [ $? -ne 0 ]
then
	echo "Argument not a JBS project!"
	exit 3
fi

TARGET="$(make -s dumptarget)"
TARGET="${TARGET/_/.}"

#echo "Non-legacy pattern: ${TARGET}"

CPPFILES="$(make -s dumpcppfiles)"

#echo "Files to analyze: ${CPPFILES}"

for f in ${CPPFILES}
do
	COUNT=$(g++ -E $(make dumpcflags) $f | \
		egrep "^# [0-9]+ \"[A-z].*" | \
		grep -v ${TARGET} | \
		cut -d' ' -f3 | \
		sort | \
		uniq | \
		wc -l)
	echo -e "${COUNT}\t${f}"
done



exit 0

