#!/bin/bash

TBFC_BASE=`dirname "$0"`

cd "${TBFC_BASE}/Src"

echo "Starting in : `pwd`"
echo "Comparing trees..."

list=`find . \( -name "*.cpp" -o -name "*.h" \) -exec basename \{\} \;`

for f in $list
do
	echo "===> Comparing versions of ${f}"

	SRC_FILE_JBS="./${f}"
	TMP_FILE_JBS="/tmp/${f}.jbs"
	cat ${SRC_FILE_JBS} | sed 's/TBFC/BFC/g' > "${TMP_FILE_JBS}"

	SRC_FILE_BFC="`find ../../../../BFC/ -name ${f/TBFC/BFC}`"
	TMP_FILE_BFC="/tmp/`basename ${SRC_FILE_BFC}`.bfc"
	cat ${SRC_FILE_BFC} \
		| sed 's/BFC_IO/BFC_Light/g' \
		| sed 's/BFC_VFS/BFC_Light/g' \
		| sed 's/BFC_Debug/BFC_Light/g' > "${TMP_FILE_BFC}"

	if ( ! diff "${TMP_FILE_JBS}" "${TMP_FILE_BFC}" )
	then
		read -e -p "Merge [y/N] ? " answer
		case "$answer" in
		"y" )
			mv ${SRC_FILE_JBS} ${SRC_FILE_JBS}.old
			cat ${SRC_FILE_BFC} \
				| sed 's/BFC_IO/BFC_Light/g'	\
				| sed 's/BFC_VFS/BFC_Light/g'	\
				| sed 's/BFC_Debug/BFC_Light/g'	\
				| sed 's/BFC/TBFC/g' > "${SRC_FILE_JBS}" ;;
		* )
			echo "Nothing changed." ;;
		esac
	fi

#	rm -f "${TMP_FILE_JBS}"
#	rm -f "${TMP_FILE_BFC}"

done

