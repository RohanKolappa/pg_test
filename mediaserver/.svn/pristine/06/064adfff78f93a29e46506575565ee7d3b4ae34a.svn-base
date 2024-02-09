#!/bin/sh

. /etc/scripts/database_management_variables.rc

_HOME_ONAIR_FOLDER=/home/onair
_SCRIPTS_FOLDER=/etc/scripts
_IPVS_XMLDB_UTILS=${_SCRIPTS_FOLDER}/ipvsxmldbutils.sh
_EXIST_COL_LIST_XSL=${_SCRIPTS_FOLDER}/xsl/exist_col_list.xsl
_EXIST_COL_LIST_INDEX_XSL=${_SCRIPTS_FOLDER}/xsl/exist_col_list_index.xsl
_EXIST_SCOL_TO_MCOL=${_SCRIPTS_FOLDER}/xsl/exist_scol_to_mcol.xsl
_MRSchema_xml=${_HOME_ONAIR_FOLDER}/objectXml/MRSchema.xml
_IPVS_COL_PREFIX=ipvs
_DOMAIN_SERVICE=default
_IPVS_MCOL_PREFIX="${_IPVS_COL_PREFIX}.${_DOMAIN_SERVICE}."
_IPVS_SCOL_DATA=IPVSc_${_DOMAIN_SERVICE}
_IPVS_SCOL_SUB=IPVSds_${_DOMAIN_SERVICE}
 #IPVSi_nids_default
_IPVS_SCOL_INDEX_NID=IPVSi_nids_${_DOMAIN_SERVICE}
_IPVS_SCOL_INDEX_PNID=IPVSis_colnids_${_DOMAIN_SERVICE}

RETVAL=0
MV_LIB=/bin/mv

logprint() {
    logger "$1"
    echo "$1"
}

logshow() {
    echo "$1"
}

findPropertyInFile_withDefault() {
    local info_value=`findPropertyInFile "$1" "$2"`
    local len=`echo -n $info_value | wc -c`
    if [ ! -z "$info_value" ]; then
        echo $info_value
        return 0
    fi
    echo $3
    return 1
}

findPropertyInFile() {
    local info_file=$1
    local info_key=$2
    cat $info_file | while read read_line
      do
      len=`echo -n $read_line | wc -c`
      if [ $len -gt 1 ]; then
          prop_key=`echo $read_line | cut -f1  -d':'`
          prop_value=`echo $read_line | cut -f2-5  -d':'` 
          prop_value=`echo $prop_value  | sed 's/^ *\(.*\) *$/\1/'`
          if [ "$info_key" = "$prop_key" ]; then
              echo $prop_value
              return 0
          fi
      fi
    done
    return 1
}


####################### for cur db : _exist_xmldb #########################################
createExistDBIndexesFolderXQ() {
    local work_folder=$1
    if [ -z $work_folder ]; then
        logprint "ERROR: 'work_folder' argument is empty. Aborting createExistDBIndexesFolderXQ...." 
        return 1
    fi	 
    local restore_db_xq=$2
    if [ -z $restore_db_xq ]; then
        restore_db_xq="${work_folder}/exist_indexes.xq"
    fi	 

    local collections_log=${work_folder}/collection.txt
    local exist_coll_folder=${work_folder}/db/system/config/db
    #logshow "${exist_coll_folder} restore_db_xq ${restore_db_xq} "
    /bin/rm -rf $exist_coll_folder
    /bin/mkdir -p $exist_coll_folder
    xsltproc  -o $collections_log $_EXIST_COL_LIST_XSL $_MRSchema_xml > /dev/null 2>&1
    cat $collections_log | while read read_line
      do
      len=`echo -n $read_line | wc -c`
      if [ $len -gt 1 ]; then
          col_name=`echo $read_line  | sed 's/^ *\(.*\) *$/\1/'`
          col_name_lowercase=`echo $col_name  | tr '[A-Z]' '[a-z]'`
          col_path=${exist_coll_folder}/${_IPVS_MCOL_PREFIX}${col_name_lowercase}
          mkdir -p ${col_path}
          xsltproc --stringparam list_name ${col_name} -o ${col_path}/collection.xconf $_EXIST_COL_LIST_INDEX_XSL $_MRSchema_xml  > /dev/null 2>&1
      fi
    done

    rm -f $restore_db_xq
    echo "xquery version \"1.0\";" > $restore_db_xq
    echo "declare namespace  xdb=\"http://exist-db.org/xquery/xmldb\";" >> $restore_db_xq
    echo "declare namespace util=\"http://exist-db.org/xquery/util\";" >> $restore_db_xq
    echo "xdb:create-collection('','system')," >> $restore_db_xq
    echo "xdb:create-collection('system','config')," >> $restore_db_xq
    echo "xdb:create-collection('system/config','db')," >> $restore_db_xq
    echo "xdb:store-files-from-pattern('system/config/db','${exist_coll_folder}',  '**/*.xconf','text/xml',true() )" >> $restore_db_xq
    return 0
}


_createExistDBMColFolder() {
    local exist_coll_folder=$1
    if [ -z $exist_coll_folder ]; then
        logprint "ERROR: 'exist_coll_folder' argument is empty. Aborting _createExistDBMColFolder...." 
        return 1
    fi	 
    local collections_log=${exist_coll_folder}/collection.txt
    xsltproc  -o $collections_log $_EXIST_COL_LIST_XSL $_MRSchema_xml  > /dev/null 2>&1
    cat $collections_log | while read read_line
      do
      len=`echo -n $read_line | wc -c`
      if [ $len -gt 1 ]; then
          col_name=`echo $read_line  | sed 's/^ *\(.*\) *$/\1/'`
          col_name_lowercase=`echo $col_name  | tr '[A-Z]' '[a-z]'`
          col_path=${exist_coll_folder}/${_IPVS_MCOL_PREFIX}${col_name_lowercase}
          mkdir -p ${col_path}
      fi
    done
    /bin/rm -rf $collections_log
    return 0

}

moveSCollToExistDBMColl() {

    local scol_xmldb_folder=$1
    if [ -z $scol_xmldb_folder ]; then
        logprint "ERROR: scol_xmldb_folder argument missing"
        return 1
    fi

    local  mcol_xmldb_folder=$2
    if [ -z $mcol_xmldb_folder ]; then
        logprint "ERROR: mcol_xmldb_folder argument missing"
        return 1
    fi

    local restore_db_xq=$3
    if [ -z $restore_db_xq ]; then
        restore_db_xq="${scol_xmldb_folder}/exist_restore_db.xq"
    fi
    
    local exit_folder=$mcol_xmldb_folder/db

		#remove unwanted files 
    /bin/rm -rf $scol_xmldb_folder/db/system
    del_files=`find "$scol_xmldb_folder" -print| egrep "[.]txt|[.]xquery|__contents__.xml|system"`
    for ARG in $del_files
      do
      directory_name=`(dirname "${ARG}")`
      /bin/rm -rf $ARG
    done
    
		#make sure the folder is empty
    /bin/rm -rf $exit_folder
    mkdir -p $exit_folder
		#create all list collection folders
    _createExistDBMColFolder $exit_folder
    temp_file="${scol_xmldb_folder}/../temp_file"

    local total=`find "$scol_xmldb_folder" -print | wc -l`
    local counter=0
    local counterLog=0
    local log_interval=100
    local FILELIST=`find "$scol_xmldb_folder" -print`
    for ARG in $FILELIST
      do
      counter=`expr ${counter} + 1`
      counterLog=`expr ${counterLog} + 1`
      file_name=`(basename "${ARG}")`
      #collaction_name=`(dirname "${ARG}")`
      isDoc=`grep NID $ARG | grep parentNID | wc -c`
      if [ -f $ARG ]; then
          if [ $isDoc -gt 1 ]; then
              xsltproc -o $temp_file --stringparam ipvscolprefix $_IPVS_COL_PREFIX $_EXIST_SCOL_TO_MCOL $ARG  > /dev/null 2>&1
              dest_file=`cat $temp_file`
              if [ "x$?" = "x0"  ]; then
                  dest="${exit_folder}/${dest_file}"
                  $MV_LIB $ARG $dest > /dev/null 2>&1
              #else
                  #echo "error SKIP $MV_LIB $ARG $dest"
              fi
          else
              dest="${exit_folder}/$file_name"
              $MV_LIB $ARG $dest > /dev/null 2>&1
          fi
          if [ $counterLog -gt $log_interval ]; then
              log_date=`date`
              logprint "$log_date moved files from sednaDB to existDB format ( $xml_file -> $doc_uri ) ${counter}/${total}."
              counterLog=1
          fi
      fi
    done

    rm -f $restore_db_xq
    echo "xquery version \"1.0\";" > $restore_db_xq
    echo "declare namespace  xdb=\"http://exist-db.org/xquery/xmldb\";" >> $restore_db_xq
    echo "declare namespace util=\"http://exist-db.org/xquery/util\";" >> $restore_db_xq
    local col_files=`ls  "${exit_folder}" `
    for ARG in $col_files
      do
      directory_name=`(basename "${ARG}")`
      if [ -d "${exit_folder}/${ARG}" ]; then
          echo "xdb:create-collection('','${directory_name}'),"  >> $restore_db_xq
      fi
    done
    echo "xdb:store-files-from-pattern('/db','${exit_folder}',  '**/**','text/xml',true() )" >> $restore_db_xq
    return 0
}

sednaToExist_exist_xmldb_R2() {
    local sednadb_xml_folder=$1
    if [ -z $sednadb_xml_folder ]; then
        logprint "ERROR: sednadb_xml_folder argument missing"
        return 1
    fi

    local exit_folder_root=$2
    if [ -z $exit_folder_root ]; then
        logprint "ERROR: exit_folder_root argument missing"
        return 1
    fi
    
    local exit_folder=$exit_folder_root/db
    mkdir -p $exit_folder

    local restore_db_xq=$3
    if [ -z $restore_db_xq ]; then
        restore_db_xq="${sednadb_xml_folder}/exist_restore_db.xq"
    fi
    
    local root_collection="/db"
    local load_docs="${sednadb_xml_folder}/load_docs.xquery"
    local create_collections="${sednadb_xml_folder}/create_collections.xquery"
    local create_collections_1="${sednadb_xml_folder}/create_collections.xquery.1"
    local load_docs="${sednadb_xml_folder}/load_docs.xquery"
    local temp_file="${sednadb_xml_folder}/temp_file"

		#create all list collection folders
    _createExistDBMColFolder $exit_folder

    echo "" >> $load_docs

    local total=`ls -alt ${sednadb_xml_folder} | wc -l`
    local counter=0
    local counterLog=0
    local log_interval=100
    
		#move all documents into its own collection and with its db name
    cat $load_docs | while read read_line
      do
      len=`echo -n $read_line | wc -c`
      if [ $len -gt 1 ]; then
          counter=`expr ${counter} + 1`
          counterLog=`expr ${counterLog} + 1`
          rm -rf temp_file
          echo $read_line > $temp_file
          cur_line=`cut -f5,6,7  -d' ' $temp_file | sed -e 's/["&]//g' `
          xml_file=`cut -f5  -d' ' $temp_file | sed -e 's/["&]//g' `
          doc_uri=`cut -f6  -d' ' $temp_file | sed -e 's/["&]//g' `
          collection=`cut -f7  -d' ' $temp_file | sed -e 's/["&]//g' `
          collection_len=`echo -n $collection | wc -c`
          if [ $collection_len -gt 1 ]; then
              xsltproc -o $temp_file --stringparam ipvscolprefix $_IPVS_COL_PREFIX $_EXIST_SCOL_TO_MCOL $sednadb_xml_folder/$xml_file > /dev/null 2>&1
              dest_file=`cat $temp_file`
              dest="${exit_folder}/$dest_file"
          else
              mkdir -p ${exit_folder}
              dest="${exit_folder}/$doc_uri" 
          fi
          $MV_LIB $sednadb_xml_folder/$xml_file $dest > /dev/null 2>&1
          if [ $counterLog -gt $log_interval ]; then
              log_date=`date`
              logprint "$log_date moved files from sednaDB to existDB format ( $xml_file -> $doc_uri ) ${counter}/${total}."
              counterLog=1
          fi
      fi
    done
    
    rm -f $restore_db_xq
    echo "xquery version \"1.0\";" > $restore_db_xq
    echo "declare namespace  xdb=\"http://exist-db.org/xquery/xmldb\";" >> $restore_db_xq
    echo "declare namespace util=\"http://exist-db.org/xquery/util\";" >> $restore_db_xq
    local col_files=`ls  "${exit_folder}" `
    for ARG in $col_files
      do
      directory_name=`(basename "${ARG}")`
      if [ -d "${exit_folder}/${ARG}" ]; then
				echo "xdb:create-collection('','${directory_name}')," >> $restore_db_xq
			fi
    done
    echo "xdb:store-files-from-pattern('/db','${exit_folder}',  '**/**','text/xml',true() )" >> $restore_db_xq
    return 0
}

sednaToExist_exist_xmldb() {
    local sednadb_xml_folder=$1
    local info_txt="${sednadb_xml_folder}/../db/info.txt"
    if [ -z $sednadb_xml_folder ]; then
        logprint "ERROR: sednadb_xml_folder argument missing"
        return 1
    fi
    local backupAppVersion=`findPropertyInFile_withDefault $info_txt backupAppVersion 1`
    local serviceVersion=`findPropertyInFile_withDefault $info_txt serviceVersion 1.4`
    local backupAppVersion=`expr $backupAppVersion + 0`
    if [ $backupAppVersion -eq 1 -o $backupAppVersion -eq 2  -o $backupAppVersion -eq 3 ]; then
        sednaToExist_exist_xmldb_R2 $1 $2 $3 $4 
    else
        logprint "ERROR: restore is not implemented for backupAppVersion='${backupAppVersion}'."
        return 1
    fi
    
    if [ x$? = "x0" ]; then
        return 0
    fi
    return 1
} 

####################### for cur db : _exist_xmldb #########################################

####################### scol to/from mcol #########################################
convertSColDirToMColDir() {
		#used for import export file/dir
    local scol_folder=$1
    local mcol_folder=$2
    local restore_db_xq=$3
    /bin/rm -rf $mcol_folder
    /bin/mkdir -p $mcol_folder
    temp_file="${mcol_folder}/../temp_file"
    _createExistDBMColFolder $mcol_folder
    local FILELIST=`find "$scol_folder" -print`
    for ARG in $FILELIST
      do
      if [ -f $ARG ]; then
          xsltproc -o $temp_file --stringparam ipvscolprefix $_IPVS_COL_PREFIX $_EXIST_SCOL_TO_MCOL $ARG > /dev/null 2>&1
          dest_file=`cat $temp_file`
          dest="${mcol_folder}/$dest_file"
          echo "$MV_LIB -f $ARG $dest"
          $MV_LIB -f $ARG $dest
					#else
						#echo " isDIR $ARG"
      fi
    done
    /bin/rm -rf $temp_file
    if [ ! -z  "${restore_db_xq}" ]; then
        rm -f $restore_db_xq
        echo "xquery version \"1.0\";" > $restore_db_xq
        echo "declare namespace  xdb=\"http://exist-db.org/xquery/xmldb\";" >> $restore_db_xq
        echo "declare namespace util=\"http://exist-db.org/xquery/util\";" >> $restore_db_xq
        echo "xdb:store-files-from-pattern('/db','${mcol_folder}',  '**/**','text/xml',true() )" >> $restore_db_xq
    fi
    /bin/rm -rf $scol_folder
    return 0
}

convertMColDirToSColDir() {
		#used for import export file/dir
    local scol_folder=$1
    local mcol_folder=$2
    local restore_db_xq=$3
    /bin/rm -rf $scol_folder
    /bin/mkdir -p $scol_folder
    temp_file="${mcol_folder}/../temp_file"
    local FILELIST=`find "$mcol_folder" -print`
    for ARG in $FILELIST
      do
      if [ -f $ARG ]; then
          dest_file=`(basename "${ARG}")`
          dest="${scol_folder}/$dest_file"
          echo "$MV_LIB -f $ARG $dest"
          $MV_LIB -f $ARG $dest
      else
          echo " isDIR $ARG"
      fi
    done
    /bin/rm -rf $temp_file
    if [ ! -z  "${restore_db_xq}" ]; then
        rm -f $restore_db_xq
        echo "xquery version \"1.0\";" > $restore_db_xq
        echo "declare namespace  xdb=\"http://exist-db.org/xquery/xmldb\";" >> $restore_db_xq
        echo "declare namespace util=\"http://exist-db.org/xquery/util\";" >> $restore_db_xq
        echo "xdb:store-files-from-pattern('/db','${mcol_folder}',  '**/**','text/xml',true() )" >> $restore_db_xq
    fi
    /bin/rm -rf $mcol_folder
    return 0
}

####################### scol to/from mcol #########################################


####################### for cur db : _sedna_native #########################################
existToSedna_sedna_native_R2() {
    exitdb_folder=$1
    if [ -z $exitdb_folder ]; then
        logprint "ERROR: exitdb_folder argument missing"
        exit 1
    fi

    exist_to_sedna_import_xq_xsl="/etc/scripts/xsl/exist_to_sedna_import_xq.xsl"
    content_xml="__contents__.xml"

    db_security_data_xml="\$db_security_data.xml"
    create_indexes_xquery="$exitdb_folder/create_indexes.xquery"
    export_log_txt="$exitdb_folder/export_log.txt"
    load_docs_xquery="$exitdb_folder/load_docs.xquery"
    create_collections_xquery="$exitdb_folder/create_collections.xquery"

    touch $create_collections_xquery
    touch $export_log_txt
    touch $load_docs_xquery
		#echo "rm -rf $exitdb_folder/system"

    rm -rf $create_collections_xquery
    rm -rf $load_docs_xquery
    rm -rf $create_indexes_xquery
    rm -rf $export_log_txt
    touch $create_collections_xquery
    touch $export_log_txt
    touch $create_indexes_xquery

		#create_indexes.xquery : todo: its tied to default domain
    echo "CREATE INDEX \"${_IPVS_SCOL_INDEX_NID}\" ON collection(\"${_IPVS_SCOL_DATA}\")//@NID BY . AS xs:string&" >> $create_indexes_xquery
    echo "CREATE INDEX \"${_IPVS_SCOL_INDEX_PNID}\" ON collection(\"${_IPVS_SCOL_DATA}\")//@parentNID BY . AS xs:string" >> $create_indexes_xquery

		#create create_indexes.xquery, load_docs.xquery: 
    local FILELIST=`find "$exitdb_folder" -print | grep $content_xml | grep  -v system `
		echo "FILELIST $FILELIST "

    for ARG in $FILELIST
      do
      folderPath=`(dirname "${ARG}")`
      collection_name=`(basename "$folderPath")`
      if [ "$collection_name" = "db" ]; then
					#echo "............ xsltproc   $exist_to_sedna_import_xq_xsl $ARG  "
          xsltproc  --stringparam folderPath  $folderPath $exist_to_sedna_import_xq_xsl $ARG  >> $load_docs_xquery
      else
          echo "CREATE COLLECTION \"${collection_name}\"&" >> $create_collections_xquery
					#echo "............ xsltproc  --stringparam collection_name  $collection_name $exist_to_sedna_import_xq_xsl $ARG "
          xsltproc  --stringparam folderPath  $folderPath --stringparam collection_name  $collection_name $exist_to_sedna_import_xq_xsl $ARG  >> $load_docs_xquery
      fi
    done
    grep -v "db_security_data" $load_docs_xquery > ${load_docs_xquery}_temp
    mv  ${load_docs_xquery}_temp $load_docs_xquery
    echo "<a/>" >> $create_collections_xquery
    echo "<a/>" >> $load_docs_xquery
    exit 0

}

existToSedna_sedna_native_R3() {
    exitdb_folder=$1
    if [ -z $exitdb_folder ]; then
        logprint "ERROR: exitdb_folder argument missing"
        exit 1
    fi
    exist_to_sedna_import_xq_xsl="/etc/scripts/xsl/exist_to_sedna_import_xq.xsl"
    content_xml="__contents__.xml"

    db_security_data_xml="\$db_security_data.xml"
    create_indexes_xquery="$exitdb_folder/create_indexes.xquery"
    export_log_txt="$exitdb_folder/export_log.txt"
    load_docs_xquery="$exitdb_folder/load_docs.xquery"
    create_collections_xquery="$exitdb_folder/create_collections.xquery"

    touch $create_collections_xquery
    touch $export_log_txt
    touch $load_docs_xquery
		#echo "rm -rf $exitdb_folder/system"

    rm -rf $create_collections_xquery
    rm -rf $load_docs_xquery
    rm -rf $create_indexes_xquery
    rm -rf $export_log_txt
    touch $create_collections_xquery
    touch $export_log_txt
    touch $create_indexes_xquery

		#create_indexes.xquery : todo: its tied to default domain
    echo "CREATE INDEX \"${_IPVS_SCOL_INDEX_NID}\" ON collection(\"${_IPVS_SCOL_DATA}\")//@NID BY . AS xs:string&" >> $create_indexes_xquery
    echo "CREATE INDEX \"${_IPVS_SCOL_INDEX_PNID}\" ON collection(\"${_IPVS_SCOL_DATA}\")//@parentNID BY . AS xs:string" >> $create_indexes_xquery

    echo "CREATE COLLECTION \"${_IPVS_SCOL_DATA}\"&" >> $create_collections_xquery
    echo "CREATE COLLECTION \"${_IPVS_SCOL_SUB}\"&" >> $create_collections_xquery
		#create create_indexes.xquery, load_docs.xquery: 
    local FILELIST=`find "$exitdb_folder" -print | grep $content_xml | grep  -v system `
    for ARG in $FILELIST
      do
      folderPath=`(dirname "${ARG}")`
      collection_name=`(basename "$folderPath")`
      if [ "$collection_name" = "db" ]; then
          xsltproc  --stringparam folderPath  $folderPath $exist_to_sedna_import_xq_xsl $ARG  >> $load_docs_xquery
      else
          xsltproc  --stringparam folderPath  $folderPath --stringparam collection_name  $_IPVS_SCOL_DATA $exist_to_sedna_import_xq_xsl $ARG  >> $load_docs_xquery
      fi
    done
    
    grep -v "db_security_data" $load_docs_xquery > ${load_docs_xquery}_temp
    mv  ${load_docs_xquery}_temp $load_docs_xquery
    echo "<a/>" >> $create_collections_xquery
    echo "<a/>" >> $load_docs_xquery
    exit 0
}

existToSedna_sedna_native() {
    existdb_xml_folder=$1
    info_txt="${existdb_xml_folder}/../db/info.txt"
    if [ -z $existdb_xml_folder ]; then
        logprint "ERROR: existdb_xml_folder argument missing"
        return 1
    fi
    backupAppVersion=`findPropertyInFile_withDefault $info_txt backupAppVersion 1`
    serviceVersion=`findPropertyInFile_withDefault $info_txt serviceVersion 1.4`
    backupAppVersion=`expr $backupAppVersion + 0`
    if [ $backupAppVersion -eq 1 -o $backupAppVersion -eq 2 ]; then
        existToSedna_sedna_native_R2 $1 $2 $3 $4 
    elif [ $backupAppVersion -eq 3 ]; then
        existToSedna_sedna_native_R3 $1 $2 $3 $4 
    else
        logprint "ERROR: restore is not implemented for backupAppVersion='${backupAppVersion}'."
        return 1
    fi
    if [ "x$?" = "x0" ]; then
        return 0
    fi
    return 1
}

####################### for cur db : _sedna_native #########################################



action_name="$1"

case "$1" in

    createExistDBIndexesFolderXQ)
    createExistDBIndexesFolderXQ $2  $3 $4 $5
    RETVAL="$?"
    ;;

    moveSCollToExistDBMColl)
    moveSCollToExistDBMColl $2  $3 $4 $5
    RETVAL="$?"
    ;;
    
    sednaToExist_exist_xmldb)
    sednaToExist_exist_xmldb $2 $3 $4 $5
    RETVAL="$?"
    ;;

    existToSedna_sedna_native)
    existToSedna_sedna_native $2 $3 $4 $5
    RETVAL="$?"
    ;;

    convertSColDirToMColDir)
    convertSColDirToMColDir $2 $3 $4 $5
    RETVAL="$?"
    ;;

    convertMColDirToSColDir)
    convertMColDirToSColDir $2 $3 $4 $5
    RETVAL="$?"
    ;;

    *)

    echo "Usage: $0 [ createExistDBIndexesFolderXQ|moveSCollToExistDBMColl|sednaToExist_exist_xmldb|convertSColDirToMColDir|convertMColDirToSColDir ]"
    RETVAL=1
esac
exit $RETVAL



