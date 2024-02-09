#!/bin/sh

# Source common library
. /etc/scripts/common.rc

. /etc/scripts/database_management_variables.rc

cleanup() {
    /bin/rm -rf ${WORKAREA}
    #reset
    #echo "Aborted."
    #exit
}

# Are we really on a dms??
if [ ! -f /etc/config/sw_profile_dms.txt ] && [ ! -f /etc/config/sw_profile_ms.txt ] ; then
    logerror "This does not seem to be a Management Server device."
    exit 1
fi

# Create directory if it does not exist
mkdir -p ${DBREPOS}


# Get the upload URL from command line
URL=$1
if [ -z ${URL} ] ; then
    showerror "First argument should be a URL for downloading"
    exit 1
fi

ftpStr=`echo ${URL} | sed -e 's/^\(ftp:\/\/\)\([^\/]*\)\(.*\)/\1/'`

if [ x${ftpStr} != "xftp://" ] ; then
        showerror "The URL for uploading has to start with ftp"
        echo "Aborted."
        exit
fi

# Retrive the hostname/ip from the URL

HOSTNAME=`echo ${URL} | sed -e 's/^\(ftp:\/\/\)\([^\/]*\)\(.*\)/\2/'`

if [ -z ${HOSTNAME} ] ; then
    showerror "Hostname/ip is not specified correctly in the url"
    exit 1
fi

# Get the upload directory path from command line
DIRPATH=`echo ${URL} | sed -e 's/^\(ftp:\/\/\)\([^\/]*\)\(.*\)/\3/'`


if [ -z ${DIRPATH} ] ; then
    showerror "Correct directory to ftp the file is not specified in the url"
    exit 1
fi


# Get the remote file from command line
DBNAME=$2
if [ -z ${DBNAME} ] ; then
    showerror "Second argument should be name of a database"
    exit 1
fi

# Make sure that we are not overwriting an existing snapshot
if [ -f ${DBREPOS}/${DBNAME}.tgz ]; then
    echo
    echo "Database with name \"${DBNAME}\" already exists."

    echo
    read -p "Overwite [y/n] ? " result; echo

    if [ x"$result" != "xy" ]; then
        echo "Aborted."
        exit
    fi
fi


# Do we have the named config??
if [ -f ${URL}/${DBNAME}.tgz ]; then
    showerror "Failed to locate database \"${DBNAME}\" at \"${URL}\""
    exit 1
fi

#trap cleanup INT

WORKAREA=/data/database_work_area
/bin/rm -rf ${WORKAREA}
/bin/mkdir -p ${WORKAREA}

# Download the database from ftp site
/home/onair/bin/ncftpget ${HOSTNAME} ${WORKAREA} ${DIRPATH}/${DBNAME}.tgz > command_output.txt 2>&1

if [ x$? != "x0" ]; then
    sed -e "s/ncftp[^:]*: //" command_output.txt
    showerror "Failed to download \"${DBNAME}\" from \"${URL}\""
    cleanup
    exit 1
fi

    
# Untar the database to check if it is sane
cd ${WORKAREA}
tar -xzf ${WORKAREA}/${DBNAME}.tgz > /dev/null 2>&1
if [ x$? != "x0" ]; then
    showerror "Database \"${DBNAME}\" \" seems to be corrupt. Aborting..."
    cleanup
    exit 1
fi

# Make sure info.txt file is present to check if it is a valid .tgz file
if [ ! -f ${WORKAREA}/db/info.txt ] ; then
    showerror "Database \"${DBNAME}\" \" seems to be corrupt, not a proper tgz format. Aborting..."
    cleanup
    exit 1
fi


# Copy current db file to workarea
/bin/cp ${WORKAREA}/${DBNAME}.tgz ${DBREPOS}/${DBNAME}.tgz

logprint "Successfully imported \"${DBNAME}\" from \"${URL}\""

cleanup
