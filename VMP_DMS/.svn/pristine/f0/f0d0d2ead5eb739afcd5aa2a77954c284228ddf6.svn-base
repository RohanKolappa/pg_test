#!/bin/bash

function proceed() {

	while [ true ]
	do
		read -p "$1 [y/N] ? " a
		if [ "$a" == "" -o "$a" == "n" ]
		then
			echo "Aborting."
			exit 0
		elif [ "$a" == "y" ]
		then
			echo ""
			return
		else
			echo "???"
		fi
	done

}

cd $( dirname $0 )

ROOT_EXT_FILE=".jbs.externals"

### Discover (unique) revision of our externals...

export COMMON_VERS

#while read dummy EXT_SRC_PATH EXT_TGT_PATH
while read EXT_SRC_PATH EXT_TGT_PATH
do
	EXT_TGT_VERS=$( svn info ${EXT_TGT_PATH} | egrep "^Revision: " | cut -d' ' -f2 )
	echo "Module ${EXT_TGT_PATH} is at revision ${EXT_TGT_VERS}."
	if [ "${COMMON_VERS}" == "" ]
	then
		COMMON_VERS=${EXT_TGT_VERS}
	elif [ "${COMMON_VERS}" != "${EXT_TGT_VERS}" ]
	then
		echo "Different revisions. This is not supported."
		exit 1
	fi
done < ${ROOT_EXT_FILE}

echo "Common version : ${COMMON_VERS}."

### Discover where to create the tag...

CURR_TIME=$( date -u +"%Y%m%d%H%M%S" )
TAG_NAME="${CURR_TIME}_r${COMMON_VERS}"
TRUNK_URL=$( svn info | egrep "^URL: " | cut -d' ' -f2 )
TAG_URL=${TRUNK_URL/\/trunk/\/tags\/${TAG_NAME}}

echo "Trunk URL      : ${TRUNK_URL}."
echo "Tag name       : ${TAG_NAME}."
echo "Tag URL        : ${TAG_URL}."
echo ""

### Create temp file with fixed externals...

TEMP_EXT_FILE="${ROOT_EXT_FILE}.${TAG_NAME}"

echo "Creating temp file \"${TEMP_EXT_FILE}\"."
sed -e "s/@@@COMMON_VERS@@@/${COMMON_VERS}/" < ${ROOT_EXT_FILE} > ${TEMP_EXT_FILE}

### Create copy on server...

proceed "Create tag on the server"

echo "Creating tag copy \"${TAG_URL}\"."
svn copy \
	--quiet \
	--ignore-externals \
	--message "[VMP_DMS] Created tag \"${TAG_NAME}\"." \
	${TRUNK_URL} \
	${TAG_URL}

echo "Editing svn:externals property."
svn propedit svn:externals \
	--editor-cmd "cat ${TEMP_EXT_FILE} >" \
	--message "[VMP_DMS] Edited tag \"${TAG_NAME}\"." \
	${TAG_URL}

### Cleanup and exit...

echo "Removing temp file \"${TEMP_EXT_FILE}\"."
rm -f ${TEMP_EXT_FILE}

echo "Tagged version \"${TAG_URL}\" ready for checkout."

exit 0
