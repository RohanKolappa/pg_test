#!/bin/sh

# Source common library
. /etc/scripts/common.rc

MEDIA_BASE_DIR=/data/DSS/data/portal/site/default

print_usage() {
    echo "Usage: $0  media_name [partial|full] [base_directory]"
}

# Get the media name from command line
MEDIA_NAME=$1

if [ -z ${MEDIA_NAME} ] ; then
    print_usage
    exit 1
fi


# If second argument is provided and says "partial" we delete
# everything except the "db.xml" file in the directory
PARTIAL=0

if [ x"$2" = "xpartial" ] ; then
    PARTIAL=1
elif [ x"$2" = "xfull" ] ; then
    PARTIAL=0
elif [ x"$2" != "x" ] ; then
    print_usage
    exit 1
fi

ALT_DIR=$3

if [ ! -z ${ALT_DIR} ] ; then
    MEDIA_BASE_DIR=${ALT_DIR}
fi

TRASH_DIR_NAME=$4
   echo ${TRASH_DIR_NAME}
if [ x"$4" = "x" ] ;  then
    TRASH_DIR_NAME="trash"
fi
 echo ${TRASH_DIR_NAME}

# Does media directory exist?
if [ ! -d ${MEDIA_BASE_DIR}/blob/${MEDIA_NAME} ] ; then
    showerror "Could not find directory ${MEDIA_BASE_DIR}/blob/${MEDIA_NAME}"
    exit 1
fi

/bin/mkdir -p ${MEDIA_BASE_DIR}/${TRASH_DIR_NAME}

# Simple case: delete everything
if [ $PARTIAL = 0 ] ; then
    /bin/mv ${MEDIA_BASE_DIR}/blob/${MEDIA_NAME} ${MEDIA_BASE_DIR}/${TRASH_DIR_NAME}/${MEDIA_NAME}
fi

# Partial Delete: Keep db.xml file. First create desired dir structure in
# a tmpdir and then move the directory to trash in one shot
if [ $PARTIAL = 1 ] ; then
    mkdir -p ${MEDIA_BASE_DIR}/tmpdir/${MEDIA_NAME}
    for file in `ls ${MEDIA_BASE_DIR}/blob/${MEDIA_NAME}/*` ; do
        base_name=`basename $file`
        if [ x$base_name = "xdb.xml" ] ; then
            continue
        fi
        /bin/mv $file ${MEDIA_BASE_DIR}/tmpdir/${MEDIA_NAME}
    done

    /bin/mv ${MEDIA_BASE_DIR}/tmpdir/${MEDIA_NAME} ${MEDIA_BASE_DIR}/${TRASH_DIR_NAME}/${MEDIA_NAME}
    /bin/rmdir ${MEDIA_BASE_DIR}/tmpdir
fi


