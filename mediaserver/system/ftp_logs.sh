#!/bin/sh

# Source common library
. /etc/scripts/common.rc

_FILE_VAR_FTP_URL_EXPORT="/etc/config/export.database.url"
_SCRIPT_FTP_UPLOAD="/etc/scripts/ftp_upload.sh"

#if ftp disaled return
if [ ! -f $_FILE_VAR_FTP_URL_EXPORT ] ; then
    exit 0
fi

src_file="$1"
if [ -z "${src_file}" ] ; then
    logprint "ERROR: File path to export logs is not provided."
    exit 1
fi

ftp_url=`cat "$_FILE_VAR_FTP_URL_EXPORT" 2>&1`
time_stamp=`date +%Y-%b-%d-%H.%M.%S`
dest_file_name="mslog-${time_stamp}.txt"


${_SCRIPT_FTP_UPLOAD}  ${src_file} "${ftp_url}" "${dest_file_name}" 
