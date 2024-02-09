#!/bin/sh

# Source common library
. /etc/scripts/common.rc 

DBNAME=$1
if [ -z ${DBNAME} ] ; then
    showerror "Provide a name of the recording directory."
    exit 1
fi
WORKAREA=/data/DBDelete
#create a copy of xquery to use 

cp ${WORKAREA}/db_files_delete.xquery.bak ${WORKAREA}/db_files_delete.xquery
#set recdir=$1

#replace  <RecordingDir> in db_files_delete.xquery  with recording directory argument
sed -i "s/<RecordingDir>/$1/"  ${WORKAREA}/db_files_delete.xquery

#delete the prev file
#rm -f /data/db_files_delete.xml

#create db_files_delete.sh for given archiever
/home/onair/sedna/bin/se_term -file ${WORKAREA}/db_files_delete.xquery -output ${WORKAREA}/db_files_delete.xml msdb




#sed -i 's/<exec>//' ${WORKAREA}/db_files_delete.xml
#sed -i 's/<\/exec>//' ${WORKAREA}/db_files_delete.xml
#chmod +x ${WORKAREA}/db_files_delete.sh
#delete files from db
#${WORKAREA}/db_files_delete.sh



