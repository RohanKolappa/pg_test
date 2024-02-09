  #!/bin/sh
  SCRIPTS_DIR=/etc/scripts
  # Source upgrade library
  . ${SCRIPTS_DIR}/database_upgrade_variables.rc

  # tar / untar is running
  _FILE_FLAG_RUNNING="/var/run/tar_utils.running"

  #lock the instance
  _FILE_LOCK_INSTANCE="/tmp/lock_tar_utils.instance" 

  #tar 
  _CONVERT_EXEC="/home/onair/TV/bin/mediaconverter"
  _IMPORTER_EXEC="/home/onair/TV/bin/mediaimporter"
  _SEND_NOTIFICATION_EXEC="send_notification"
  _SEND_NOTIFICATION_SRC="MediaFileConverter"
  _TYPE_PREPARE_COMPLETE="MediaFilePrepareComplete"
  _TYPE_PREPARE_ERROR="MediaFilePrepareError"
  _TYPE_CLEAR_CACHE_COMPLETE="ClearDownloadCacheComplete"
  _TYPE_CLEAR_CACHE_ERROR="ClearDownloadCacheError"
  _TYPE_CREATE_CUTLIST_COMPLETE="MediaFileCreateCutlistComplete"
  _TYPE_CREATE_CUTLIST_ERROR="MediaFileCreateCutlistError"
  _TYPE_MEDIA_INGESTION_COMPLETE="MediaIngestionComplete"
  _TYPE_TRIM_MEDIA_ERROR="TrimMediaError"
  _TYPE_TRIM_MEDIA_COMPLETE="TrimMediaComplete"
  _TYPE_MEDIA_INGESTION_ERROR="MediaIngestionError"
  _TYPE_PACK_COMPLETE="MediaFilePackComplete"
  _TYPE_PACK_ERROR="MediaFilePackError"
  _TYPE_UNPACK_COMPLETE="MediaFileUnPackComplete"
  _TYPE_UNPACK_ERROR="MediaFileUnPackError"
  _TYPE_UNPACK_2_4_COMPLETE="Media2dot4FileUnPackComplete"
  _TYPE_UNPACK_2_4_ERROR="Media2dot4FileUnPackError"
  _IONICECMD="/usr/bin/ionice -c 3"

  ############## end definations of variables ###########
  logprint() {
      logger "$1"
      echo -e "$1\n"
  }
  logshow() {
      echo -e "$1\n"
  }
  
  ########## save_pid ##########
  save_pid() {
      local save_file=$1
      local pid=$2
      if [ -z $save_file ] ; then
          echo "ERROR: pid file name Argument is empty "
          return 1
      fi

      # Check if we are already running
      if [ -f $save_file ]; then
        /bin/rm -f ${save_file}
      fi

      # Now write current process's PID to the save file.
      # Use noclobber to avoid a potential race condition
      if set -o noclobber; echo ${pid} > "${save_file}" ; then
          return 0
      fi
      return 1
  }

  
  ############## send_notification ##############
  mediaroom_send_notif() {
      #send notification to middelware  
      local event_type="${1}"
      local clientID="${2}"
      local error="${3}"
      local dirNID="${4}"      
      local fileUUID="${5}"

      ${_SEND_NOTIFICATION_EXEC} "-s" "${_SEND_NOTIFICATION_SRC}"  "-t" "${event_type}" "clientID" "${clientID}"  "error" "${error}" "dirNID" "${dirNID}" "fileUUID" "${fileUUID}"  
      if [ "x$?" != "x0" ]; then
          logprint "Failed to send_notification : ${event_type} error=${error} clientID=${clientID} fileUUID=${fileUUID} "   
      fi
      return 0  
  }
  
   mediaroom_send_prepare_notif() {
      #send notification to middelware  
      local event_type="${1}"
      local clientID="${2}"
      local dirNID="${3}"
      local fileUUID="${4}"
      local mediaFormat="${5}"
      local error="${6}"
      local typeOfRequest="${7}"
      local prepareFileName="${8}"
      local size="${9}"

      ${_SEND_NOTIFICATION_EXEC} "-s" "${_SEND_NOTIFICATION_SRC}"  "-t" "${event_type}" "clientID" "${clientID}"  "error" "${error}"  "dirNID" "${dirNID}" "fileUUID" "${fileUUID}" "mediaFormat" "${mediaFormat}"  "typeOfRequest" "${typeOfRequest}" "prepareFileName" "${prepareFileName}" "size" "${size}"  
      if [ "x$?" != "x0" ]; then
          logprint "Failed to send_notification : ${event_type} error=${error} clientID=${clientID} fileUUID=${fileUUID} prepareFileName=${prepareFileName} "   
      fi
      return 0  
  }
  
mediaroom_send_unpack_notif() {
     #send notification to middelware  
     local event_type="${1}"
     local clientID="${2}"
     local dirNID="${3}"
     local fileUUID="${4}"
	 local queueID="${5}"
     local groupFileUUID="${6}"  
     local isGroupFilePresent="${7}"   	
     local error="${8}"

	 ${_SEND_NOTIFICATION_EXEC} "-s" "${_SEND_NOTIFICATION_SRC}"  "-t" "${event_type}" "clientID" "${clientID}"  "error" "${error}"  "dirNID" "${dirNID}" "fileUUID" "${fileUUID}" "queueID" "${queueID}" "groupFileUUID" "${groupFileUUID}" "isGroupFilePresent" "${isGroupFilePresent}"  
      	if [ "x$?" != "x0" ]; then
          logprint "Failed to send_notification : ${event_type} error=${error} clientID=${clientID} fileUUID=${fileUUID} groupFileUUID=${groupFileUUID} "   
      	fi
      return 0  
  }


mediaGroup_prepare() {
      local prepare_file_path="${1}"
      local media_files_folder="${2}"
      local prepare_file_folder="${3}"
      local fileNID="${4}"
      local startOffset="${5}"
      local endOffset="${6}"
      local requestTimeStamp="${7}"
      local format="${8}"
      local stream_type="${9}"
      local sdp_path="${10}"
      local std_media_filename="${11}"
      local dirNID="${12}"
      local fileUUID="${13}"
      local typeOfRequest="${14}"
      local old_fileUUID_size="${15}"
      local media_group_playlength="${16}"
  
      local error_desc=""
      if [ -z ${prepare_file_path} ] ; then
          error_desc="ERROR: Destination File name to mediaGroup_prepare is not provided. Aborting prepare  mediafile..."
      elif [ -z ${media_files_folder} ] ; then
          error_desc="ERROR: Source media files folder to mediaGroup_prepare is not provided. Aborting prepare  mediafile..."
      elif [ -z ${prepare_file_folder} ] ; then
          error_desc="ERROR: Source db files folder to mediaGroup_prepare is not provided. Aborting prepare  mediafile..."
      elif [ -z $fileNID ] ; then
          error_desc="ERROR: fileNID argument to mediaGroup_prepare is not provided. Aborting prepare  mediafile..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}" 
          return 1
      fi
	  
      #save process pid
      save_pid "${prepare_file_folder}/process.pid" "$$"
      
      mkdir -p $prepare_file_folder/proprietary
      mkdir -p $prepare_file_folder/standard
    
      #copy the db.xml to metadata    
      local db_xml="${media_files_folder}/db.xml"
      local to_db_xml="${prepare_file_folder}/db.xml"
      rm -f $to_db_xml
      cp -f $db_xml $to_db_xml      
      local filePath="${prepare_file_folder}/db/IPVSc_default/${fileNID}"
      local outputfilePath="${prepare_file_path}"
	  
      logprint "Mediaconverter prepare- '${prepare_file_path}' media_folder='${media_files_folder}' db='${prepare_file_folder}'"
      
      rm -f ${prepare_file_folder}/temp_bookmarks.xml	
      echo "<Bookmarks>" > ${prepare_file_folder}/temp_bookmarks.xml
	  cat ${media_files_folder}/bookmarks/*.xml >> ${prepare_file_folder}/temp_bookmarks.xml
	  echo "</Bookmarks>" >> ${prepare_file_folder}/temp_bookmarks.xml
      sed -i 's/<?xml version="1.0" encoding="UTF-8"?>//g' ${prepare_file_folder}/temp_bookmarks.xml
	  local tempBookmarkPath="${prepare_file_folder}/temp_bookmarks.xml";
	  
 ####### apply xsl to media resource and db.xml ##########
 	local tmp_transform_dir=$prepare_file_folder
    cat > $tmp_transform_dir/transform.xsl <<EOF
    <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="node()|@*">
        <xsl:copy>
            <xsl:apply-templates select="@*|node()"/>
        </xsl:copy>
    </xsl:template>
    <xsl:param name="tempBookmarkPath"></xsl:param>
    <xsl:template match="MediaStoreFile/MediaGroup">
        <xsl:copy-of select="."/>
        <xsl:copy-of select="document('$tempBookmarkPath')"/>
    </xsl:template>   
</xsl:stylesheet>
EOF

 	  local cmd="xsltproc -o ${outputfilePath} --stringparam tempBookmarkPath ${tempBookmarkPath} ${tmp_transform_dir}/transform.xsl ${filePath}"
      logprint "${cmd}"                                                                                              
      #execute the cmd
      ${cmd}
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
      	local error_desc="xsltproc '${filePath}' to='${filePath}', return_code=${ret}."                              
     	logprint "${error_desc}"                                                                                       
     	mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                 
     	return 1                                                                                                       
      fi
      
      
####### apply xsl to db.xml for updating group file playlength##########
 	local tmp_transform_dir=$prepare_file_folder
    cat > $tmp_transform_dir/transform_pl.xsl <<EOF
    <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
    <xsl:template match="node()|@*">
        <xsl:copy>
            <xsl:apply-templates select="@*|node()"/>
        </xsl:copy>
    </xsl:template>
    <xsl:param name="media_group_playlength"></xsl:param>
    <xsl:template match="MediaGroup/PlayLength">
        <PlayLength><xsl:value-of select="string('$media_group_playlength')"/></PlayLength>
    </xsl:template>   
</xsl:stylesheet>
EOF

 	  local cmd="xsltproc -o ${outputfilePath} --stringparam playlength ${media_group_playlength} ${tmp_transform_dir}/transform_pl.xsl ${outputfilePath}"
      logprint "${cmd}"                                                                                              
      #execute the cmd
      ${cmd}
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
      	local error_desc="xsltproc '${to_db_xml}' to='${to_db_xml}', return_code=${ret}."                              
     	logprint "${error_desc}"                                                                                       
     	mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                 
     	return 1                                                                                                       
      fi      
  	    	    	   
  	  if [ "${format}" != "Standard" ]; then  	   
  	  	cp -r ${prepare_file_path} ${prepare_file_folder}/proprietary
  	  fi
  	   
	  if [ "${format}" != "Proprietary" ]; then	  
	    cp -r ${prepare_file_path} ${prepare_file_folder}/standard
	  fi
	   
      #copy .grp file to blob Directory
      if [ -f ${prepare_file_folder}/proprietary/*.grp ]; then 
		  rm -rf $media_files_folder/proprietary  
		  mkdir -p $media_files_folder/proprietary
	      cp -r $prepare_file_folder/proprietary/*.grp $media_files_folder/proprietary/$std_media_filename.grp      	      
      fi  
      
      #copy .grp file to blob Directory
      if [ -f ${prepare_file_folder}/standard/*.grp ]; then 
		  rm -rf $media_files_folder/standard  
		  mkdir -p $media_files_folder/standard
	      cp -r $prepare_file_folder/standard/*.grp $media_files_folder/standard/$std_media_filename.grp      	      
      fi  
              
      #removing the temprorary file
      rm -rf $prepare_file_folder    
            
      mediaroom_send_prepare_notif "${_TYPE_PREPARE_COMPLETE}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
      return 0
  }

  mediafile_prepare() {
  
      local prepare_file_path="${1}"
      local media_files_folder="${2}"
      local prepare_file_folder="${3}"
      local fileNID="${4}"
      local startOffset="${5}"
      local endOffset="${6}"
      local requestTimeStamp="${7}"
      local format="${8}"
      local stream_type="${9}"
      local sdp_path="${10}"
      local std_media_filename="${11}"
      local dirNID="${12}"
      local fileUUID="${13}"
      local typeOfRequest="${14}"
      local old_fileUUID_size="${15}"
      local new_UUID="${16}"
            
      local error_desc=""
      if [ -z ${prepare_file_path} ] ; then
          error_desc="ERROR: Destination File name to mediafile_prepare is not provided. Aborting prepare  mediafile..."
      elif [ -z ${media_files_folder} ] ; then
          error_desc="ERROR: Source media files folder to mediafile_prepare is not provided. Aborting prepare  mediafile..."
      elif [ -z ${prepare_file_folder} ] ; then
          error_desc="ERROR: Source db files folder to mediafile_prepare is not provided. Aborting prepare  mediafile..."
      elif [ -z $fileNID ] ; then
          error_desc="ERROR: fileNID argument to mediafile_prepare is not provided. Aborting prepare  mediafile..."
      elif [ -z $new_UUID ] ; then
          error_desc="ERROR: new_UUID argument to mediafile_prepare is not provided. Aborting prepare  mediafile..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
          return 1
      fi
      
      local cutListFolder="${prepare_file_folder}/../../cutlist"
      
      #save process pid
      save_pid "${prepare_file_folder}/process.pid" "$$"
      
      mkdir -p $cutListFolder
      mkdir -p $prepare_file_folder/proprietary
      mkdir -p $prepare_file_folder/standard
      
      #copy the db.xml to metadata    
      local db_xml="${media_files_folder}/db.xml"
      local to_db_xml="${prepare_file_folder}/db.xml"
      rm -f $to_db_xml
      cp -f $db_xml $to_db_xml
      local filePath="${prepare_file_folder}/db/IPVSc_default/${fileNID}"
      local indexFilePath="${prepare_file_folder}/db/_index"
      
      logprint "Mediaconverter prepare- '${prepare_file_path}' media_folder='${media_files_folder}' db='${prepare_file_folder}'"
      
      local cutList=""
      local convertCmd=""
      local playlength=""
      local size=""
      local dateCreated=0
      		
      if [ ${startOffset} != "" -a  ${endOffset} != "" ]; then
          if [ ${startOffset} -lt 0 ]; then
              startOffset=0
          fi
          if [ ${endOffset} -le 0 ]; then
              local error_desc="Incorrect endOffset for export"                              
              logprint "${error_desc}"                                                                                       
              mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                   
              return 1 
          fi
      	
          cutList="${startOffset}:${endOffset}"
      	
          local infoCmd=`${_CONVERT_EXEC} -i gaps -f ${media_files_folder} | awk -F"[<>]" '/TotalGaps/ {print $3;exit}'`
          if [ ${infoCmd} -gt 0 ]; then
              # Gaps exist, need to adjust cutlist.
              local currTime=`date +%s`
              local currTimeInMilliSec=`expr ${currTime} \* 1000` # Approximated granularity for milliseconds
              local diff=`expr ${currTimeInMilliSec} - ${requestTimeStamp}`
              if [ ${diff} -le 0 ]; then
                  local warn_desc="Current system time is before request time. Time not in sync. Not adjusting the cutlist"                              
                  logprint "${warn_desc}"
                  break                                                                                       
              else
                  startOffset=`expr ${startOffset} - ${diff}`
                  if [ ${startOffset} -lt 0 ]; then
                      startOffset=0
                  fi
                  endOffset=`expr ${endOffset} - ${diff}`
                  if [ ${endOffset} -le 0 ]; then
                      local error_desc="Requested cutlist is no longer available to export"                              
                      logprint "${error_desc}"                                                                                       
                      mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                    
                      return 1 
                  fi
      		
                  local cut="Adjusting the cutlist by ${diff}"
                  logprint "${cut}"
                  ${cutList}="${startOffset}:${endOffset}"
              fi
          fi
      	
          convertCmd="${_CONVERT_EXEC} -c -f ${prepare_file_path} -b ${media_files_folder} -d ${prepare_file_folder} -C ${cutList}"
      else
          convertCmd="${_CONVERT_EXEC} -c -f ${prepare_file_path} -b ${media_files_folder} -d ${prepare_file_folder}"
      fi

      munge_file_info_pack "${filePath}" "${media_files_folder}" "${to_db_xml}" "${cutList}" "${prepare_file_path}"    
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
      	mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                   
      	return 1                                                                                                       
      fi
      
      if [ "${format}" != "Standard" ] || [ "${stream_type}" = "V2D" ] || [ "${stream_type}" = "UDP" ] ; then
          
          ## Munge index file with bookmark info
          for f in `ls ${media_files_folder}/bookmarks/*.xml`;
          do
              local cmd="xsltproc -o ${indexFilePath} --stringparam filepath ${f}  ${XSL_DIR}/update_index_file.xsl ${indexFilePath}"
              logprint "${cmd}"                                                                                              
              ${cmd}                                     
              ret="$?"                                                                                                       
              if [ "x${ret}" != "x0" ]; then                                                                                 
                  local error_desc="xsltproc '${indexFilePath}' to='${indexFilePath}', return_code=${ret}."                              
                  logprint "${error_desc}"                                                                                       
                  mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                   
                  return 1                                                                                                       
              fi
          
              ## copy bookmark files
              local bookmarkFileName=`echo ${f} | awk -F"/" '{print $NF}'`
              /bin/cp ${f} ${prepare_file_folder}/db/IPVSc_default/bookmark_${bookmarkFileName}
          done
          
          ## execute the command
          logprint "${convertCmd}"

          ${_IONICECMD} ${convertCmd} > /dev/null 2>&1 &
          
          #save process pid
          save_pid "${prepare_file_folder}/childprocess.pid" "$!"
      	  
          #wait for process to finish
          wait "$!"
          
          ret="$?"
          if [ "x${ret}" != "x0" ]; then
              local error_desc="return_code=${ret}, Mediaconverter failed to prepare. '${prepare_file_path}' media_folder='${media_files_folder}' db='${prepare_file_folder}'"
              logprint "${error_desc}"
              mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
              
              #remove childprocess pid
              /bin/rm -rf ${prepare_file_folder}/childprocess.pid
              return 1
          fi
          
          #remove childprocess pid
          /bin/rm -rf ${prepare_file_folder}/childprocess.pid
          
          ## move to proprietary 
          /bin/mv ${prepare_file_path} ${prepare_file_folder}/proprietary
      fi
      
	  # check if pack requested for other formats
      if [ "${format}" != "Proprietary" ] && [ "${stream_type}" != "V2D" ] && [ "${stream_type}" != "UDP" ] ; then
          
          create_update_metadata_pack "${prepare_file_folder}" "${indexFilePath}" "${std_media_filename}" "${media_files_folder}"
          ret="$?"                                                                                                       
          if [ "x${ret}" != "x0" ]; then                                                                                 
              mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                   
              return 1                                                                                                       
          fi
          
          ## create the mediacovertor command to create standard format file
          
          local cmd="${_CONVERT_EXEC} -N -t ${stream_type} -s ${sdp_path} -f ${prepare_file_folder}/standard/${std_media_filename}/${std_media_filename} -b ${media_files_folder}" "${old_fileUUID_size}"
          logprint "${cmd}"  
          ${_IONICECMD} ${cmd}   > /dev/null 2>&1 &
          
          #save process pid
          save_pid "${prepare_file_folder}/childprocess.pid" "$!"
      	  
          #wait for process to finish
          wait "$!"
          
          ret="$?"
          if [ "x${ret}" != "x0" ]; then
              local error_desc="return_code=${ret}, Mediaconverter failed to prepare. '${prepare_file_folder}/standard/${std_media_filename}/${std_media_filename}' media_folder='${media_files_folder}'' 'sdpPath='${sdp_path}''"
              logprint "${error_desc}"
              mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
              /bin/rm -f ${sdp_path}
              #remove childprocess pid
              /bin/rm -rf ${prepare_file_folder}/childprocess.pid
              return 1
          fi
          #remove childprocess pid
          /bin/rm -rf ${prepare_file_folder}/childprocess.pid
          
          ## zip up the contents of standard
          
          cd ${prepare_file_folder}/standard
          local cmd="zip -r ${prepare_file_folder}/standard/standard.zip ${std_media_filename}"
          logprint "${cmd}"
          ${cmd} &
          
          #save process pid
          save_pid "${prepare_file_folder}/childprocess.pid" "$!"
      	  
          #wait for process to finish
          wait "$!"
          
          ret="$?"
          if [ "x${ret}" != "x0" ]; then
              local error_desc="return_code=${ret}, Mediaconverter failed to prepare standard media ZIP. 'standard.zip' media_folder='${prepare_file_folder}/standard/${std_media_filename}'"
              logprint "${error_desc}"
              mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
              
              #remove childprocess pid
              /bin/rm -rf ${prepare_file_folder}/childprocess.pid
              
              return 1
          fi
          
           #remove childprocess pid
          /bin/rm -rf ${prepare_file_folder}/childprocess.pid
      fi
      
      /bin/rm -f ${sdp_path}
      
      #copy .mkd file to blob Directory
      local cp_cmd=""
      if [ -f ${prepare_file_folder}/proprietary/*.mkd ]; then 
          if [ "${cutList}" != "" ] ; then
              ## Save cutlist in temporary folder 
              mkdir -p $cutListFolder/$new_UUID
              cp_cmd="cp -r $prepare_file_folder/proprietary/*.mkd $cutListFolder/$new_UUID/$std_media_filename.mkd"  
          else 
              rm -rf $media_files_folder/proprietary  
              mkdir -p $media_files_folder/proprietary
              cp_cmd="cp -r $prepare_file_folder/proprietary/*.mkd $media_files_folder/proprietary/$std_media_filename.mkd"
          fi      	      
      fi  
      
      logprint "${cp_cmd}"
      ${cp_cmd} &

      #save process pid
      save_pid "${prepare_file_folder}/childprocess.pid" "$!"
      	  
      #wait for process to finish
      wait "$!"
      
      #remove childprocess pid
      /bin/rm -rf ${prepare_file_folder}/childprocess.pid
      
      #copy .zip file to blob Directory
      if [ -f ${prepare_file_folder}/standard/*.zip ]; then 
          if [ "${cutList}" != "" ] ; then
               ## Save cutlist in temporary folder
               mkdir -p $cutListFolder/$new_UUID
               cp_cmd="cp -r $prepare_file_folder/standard/*.zip $cutListFolder/$new_UUID/$std_media_filename.zip"   
          else 
               rm -rf $media_files_folder/standard
               mkdir -p $media_files_folder/standard   
               cp_cmd="cp -r $prepare_file_folder/standard/*.zip $media_files_folder/standard/$std_media_filename.zip"
          fi	     
      fi 
      
      logprint "${cp_cmd}"
      ${cp_cmd} &

      #save process pid
      save_pid "${prepare_file_folder}/childprocess.pid" "$!"
      	  
      #wait for process to finish
      wait "$!"
      
      #remove childprocess pid
      /bin/rm -rf ${prepare_file_folder}/childprocess.pid
      
      #removing the temprorary file
      rm -rf $prepare_file_folder    
            
      mediaroom_send_prepare_notif "${_TYPE_PREPARE_COMPLETE}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
      return 0
  }
  
  create_update_metadata_pack() {
      local prepare_file_folder="${1}"
      local indexFilePath="${2}"
      local std_media_filename="${3}"
      local media_files_folder="${4}"
      
      ## Create the metadata xml
      local cmd="mkdir -p ${prepare_file_folder}/standard/${std_media_filename}"
      ${cmd}
          
      local cmd="xsltproc -o ${prepare_file_folder}/standard/${std_media_filename}/MediaInfo.xml --stringparam filepath ${prepare_file_folder}/db.xml --stringparam UUID ${std_media_filename} ${XSL_DIR}/create_mediainfo_xml.xsl ${indexFilePath}"          
      logprint "${cmd}"                                                                                              
      ${cmd}                                     
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
          local error_desc="xsltproc '${indexFilePath}' to='${prepare_file_folder}/standard/${std_media_filename}/MediaInfo.xml', return_code=${ret}."                              
          logprint "${error_desc}"                                                                                       
          #mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                 
          return 1                                                                                                       
      fi
          
      ## Munge the metadata xml with BookmarkInfo if any
       
      for f in `ls ${media_files_folder}/bookmarks/*.xml`;
      do
      local cmd="xsltproc -o ${prepare_file_folder}/standard/${std_media_filename}/MediaInfo.xml --stringparam filepath ${f}  ${XSL_DIR}/update_media_metadata_xml.xsl ${prepare_file_folder}/standard/${std_media_filename}/MediaInfo.xml"
      logprint "${cmd}"                                                                                              
      ${cmd}                                     
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
         local error_desc="xsltproc '${prepare_file_folder}/standard/${std_media_filename}/MediaInfo.xml' to='${prepare_file_folder}/standard/${std_media_filename}/MediaInfo.xml', return_code=${ret}."                              
         logprint "${error_desc}"                                                                                       
         #mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                     
         return 1                                                                                                       
      fi
      done
      return 0
  }
  
  munge_file_info_pack() {
  
    local filePath="${1}"
    local media_files_folder="${2}"
    local to_db_xml="${3}"
    local cutList="${4}"
    local prepare_file_path="${5}"
    
    if [ "${cutList}" != "" ] ; then
        local startOffset=$(echo $cutList | tr -d ' ' | cut -c1-1)
        playlength=`${_CONVERT_EXEC} -i gaps -f ${media_files_folder} -C ${cutList} | awk -F"[<>]" '/Duration/ {print $3;exit}'`
        size=`${_CONVERT_EXEC} -i size -f ${media_files_folder} -C ${cutList} | awk -F"[<>]" '/Size/ {getline;print $3;exit}'`
        dateCreated=`${_CONVERT_EXEC} -i gettimecodeatoffset -a ${startOffset} -f ${media_files_folder} | awk -F"[<>]" '/<TimeCode>/ {print $3;exit}'`   	           
    else 
        playlength=`${_CONVERT_EXEC} -i gaps -f ${media_files_folder} | awk -F"[<>]" '/Duration/ {print $3;exit}'`
        size=`${_CONVERT_EXEC} -i size -f ${media_files_folder} | awk -F"[<>]" '/Size/ {getline;print $3;exit}'`
        dateCreated=`${_CONVERT_EXEC} -i gettimecodeatoffset -a 0 -f ${media_files_folder} | awk -F"[<>]" '/<TimeCode>/ {print $3;exit}'`
    fi
          
      ####### apply xsl to media resource and db.xml ##########
      local tmp_transform_dir=$media_files_folder
      cat > $tmp_transform_dir/transform.xsl <<EOF
      <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
          <xsl:template match="node()|@*">
              <xsl:copy>
                  <xsl:apply-templates select="@*|node()"/>
              </xsl:copy>
      </xsl:template>
          <xsl:param name="playlength"></xsl:param>
          <xsl:param name="size"></xsl:param>
          <xsl:param name="dateCreated"></xsl:param>
          <xsl:template match="MediaStreamFileResource/Info/MediaClip/PlayLength">
              <PlayLength><xsl:value-of select="string('$playlength')"/></PlayLength>
          </xsl:template>
          <xsl:template match="MediaStreamFileResource/Info/Properties/Size">
              <Size><xsl:value-of select="string('$size')"/></Size>
          </xsl:template>
          <xsl:template match="MediaStreamFileResource/Info/Properties/DateCreated">
              <DateCreated><xsl:value-of select="string('$dateCreated')"/></DateCreated>
      </xsl:template>
  	  </xsl:stylesheet>
EOF
  	  
      local cmd="xsltproc -o ${filePath} --stringparam playlength ${playlength} --stringparam size $size --stringparam dateCreated ${dateCreated} ${tmp_transform_dir}/transform.xsl ${filePath}"
      logprint "${cmd}"                                                                                              
      #execute the cmd
      ${cmd}
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
      	local error_desc="xsltproc '${filePath}' to='${filePath}', return_code=${ret}."                              
      	logprint "${error_desc}"                                                                                       
      	#mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                 
      	return 1                                                                                                       
      fi

	  # remove temp transform stuff
	  /bin/rm -rf $tmp_transform_dir/transform.xsl
	  
	  local cmd="xsltproc -o ${to_db_xml} --stringparam filePath $filePath --stringparam playlength ${playlength} --stringparam dateCreated ${dateCreated} ${XSL_DIR}/update_resource_db_xml.xsl ${to_db_xml}"                        
      logprint "${cmd}"                                                                                              
      #execute the cmd
      ${cmd}                                     
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
      	local error_desc="xsltproc '${to_db_xml}' to='${to_db_xml}', return_code=${ret}."                              
      	logprint "${error_desc}"                                                                                       
      	#mediaroom_send_prepare_notif "${_TYPE_PREPARE_ERROR}"  "${fileNID}" "${dirNID}" "${fileUUID}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                  
      	return 1                                                                                                       
      fi
      return 0
  }
  
  remove_2dot4_media() {
      local root_folder="${1}"
      local uuid="${2}"
      local error_desc=""
      if [ -z ${root_folder} ] ; then
          error_desc="ERROR: blob root folder to 2.4 remove_2dot4_media is not provided . Aborting ..."
      elif [ -z ${uuid} ] ; then
          error_desc="ERROR: uuid folder to 2.4 remove_2dot4_media is not provided . Aborting ..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          return 1
      fi
      logprint "remove_2dot4_media /bin/rm -rf ${root_folder}blob/${uuid} ${root_folder}db/${uuid}"
      /bin/rm -rf ${root_folder}blob/${uuid}
      /bin/rm -rf ${root_folder}db/${uuid}
      return 0
  }

  mediafile_unpack_2dot4() {
      local tar_file="${1}"
      local temp_folder="${2}"
      local root_folder="${3}"
      local db_tar_file_name="${4}"
      local uuid="${5}"
      local dateModified="${6}"
      local mediaStoreFile_Rev="${7}"
      local clientID="${8}"
      local groupID=`uuidgen`

      local error_desc=""
      if [ -z ${tar_file} ] ; then
          error_desc="ERROR: File name to 2.4 mediafile_unpack is not provided. Aborting unpack  mediafile..."
      elif [ -z ${temp_folder} ] ; then
          error_desc="ERROR: temp folder name to 2.4 mediafile_unpack is not provided. Aborting unpack  mediafile..."
      elif [ -z ${clientID} ] ; then
          error_desc="ERROR: clientID argument to 2.4 mediafile_unpack is not provided. Aborting unpack  mediafile..."
      elif [ -z ${db_tar_file_name} ] ; then
          error_desc="ERROR: ${db_tar_file_name} folder to 2.4 mediafile_unpack is not provided . Aborting unpack  mediafile..."
      elif [ -z ${root_folder} ] ; then
          error_desc="ERROR: blob root folder to 2.4 mediafile_unpack is not provided . Aborting unpack  mediafile..."
      elif [ -z ${uuid} ] ; then
          error_desc="ERROR: uuid folder to 2.4 mediafile_unpack is not provided . Aborting unpack  mediafile..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_UNPACK_2_4_ERROR}"  "${clientID}" "${error_desc}"
          return 1
      fi

      #make sure the temp extract folder exist and empty. Bug on windows
      local temp_blob_uuid_folder="${root_folder}import/${uuid}"
      /bin/rm -rf  $temp_blob_uuid_folder
      /bin/mkdir -p  $temp_blob_uuid_folder
      
      #extract the file
      local cmd="/bin/unzip -x ${tar_file} -d ${temp_blob_uuid_folder}" 
      logprint "${cmd}"
      ${_IONICECMD} /usr/bin/unzip -x  ${tar_file} -d ${temp_blob_uuid_folder}  > /dev/null 2>&1 
      ret="$?"
      #ls ${temp_blob_uuid_folder}/2dot4media
      if [ "x${ret}" != "x0" ]; then
          local error_desc="2.4 Media failed to unpack. '${tar_file}' media_folder='${temp_blob_uuid_folder}' , return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_UNPACK_2_4_ERROR}"  "${clientID}" "${error_desc}"
          return 1
      fi

      #check if db xml exist ... get its name
      local old_uuid_blob_folder="`ls $temp_blob_uuid_folder/2dot4media/db/*`"
      if [ -z $old_uuid_blob_folder ]; then
          local error_desc="2.4 Media failed to unpack. '${tar_file}' media_folder='${temp_blob_uuid_folder}'. Invalid tar file, db file does not exist."
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_UNPACK_2_4_ERROR}"  "${clientID}" "${error_desc}"
          return 1
      fi
      
      local old_uuid="`(basename "${old_uuid_blob_folder}")`"
      logprint  " old_uuid ${old_uuid} new uuid ${uuid}"
      
      #patch uuid
      sed -i -e "s/${old_uuid}/${uuid}/g" $old_uuid_blob_folder

      #remove if prev blob db exist 
      /bin/rm -rf ${root_folder}blob/${uuid}
      /bin/rm -rf ${root_folder}db/${uuid}
      /bin/mkdir -p ${root_folder}blob
      /bin/mkdir -p ${root_folder}db

      logprint  " /bin/mv ${temp_blob_uuid_folder}/2dot4media//blob/${old_uuid} ${root_folder}blob/${uuid} "
      logprint  " /bin/mv -f ${temp_blob_uuid_folder}/2dot4media/db/${old_uuid} ${root_folder}db/${uuid} "

      #copy into new blob and db
      /bin/mv -f ${temp_blob_uuid_folder}/2dot4media//blob/${old_uuid} ${root_folder}blob/${uuid}
      /bin/mv -f ${temp_blob_uuid_folder}/2dot4media/db/${old_uuid} ${root_folder}db/${uuid}

      #remove temp extracted folder
      /bin/rm -rf ${temp_blob_uuid_folder}

      mediaroom_send_notif "${_TYPE_UNPACK_2_4_COMPLETE}"  "${clientID}" ""
      return 0  
  }

  # try creating job and verify the tar is not courrupted.....  be called one more time if fails
  # bug in tar creation... some times evn though it fails... it does not return any error
  _create_db_tar() {
      local db_folder="$1"
      local db_tar_file_name="$2"
      local retry_attempt="$3"
      /bin/rm -f "${db_folder}/${db_tar_file_name}"
      tar "-cvf" "${db_folder}/${db_tar_file_name}" "-C" "${db_folder}/db" "."
      local ret="$?"
      if [ "x${ret}" = "x0" ]; then
          tar "-tvf" "${db_folder}/${db_tar_file_name}" "-C" "${db_folder}/db" "."
          ret="$?"
          if [ "x${ret}" = "x0" ]; then
              return 0
          fi
          logprint "Tar creation failed... tar is corrupted  ( retry_attempt ${retry_attempt} file ${db_folder}/${db_tar_file_name} ) "
      else
          logprint "Failed to package jar file. ( retry_attempt ${retry_attempt} file ${db_folder}/${db_tar_file_name} ) "
      fi
      return 1
  }


  


mediafile_unpack() {
      # /home/onair/TV/bin/mediaconverter -x -f x.mkd -b outputblob -d outputdb
      local tar_file="${1}"
      local temp_folder="${2}"
      local root_folder="${3}"
      local db_tar_file_name="${4}"
      local uuid="${5}"
      local dateModified="${6}"
      local mediaStoreFile_Rev="${7}"
      local clientID="${8}"
	  local dirID="${9}"
      local dirNID="${10}"
      local serviceVer="${11}"
      local queueID="${12}"
      local groupID="${13}"
      local groupNID="${14}"
      local groupFileUUID="${15}"
      local isGroupPresent="${16}"
      
      if [ -z ${groupID} ] ; then
    	  groupID=`uuidgen`
	  fi	
      local error_desc=""
      if [ -z ${tar_file} ] ; then
          error_desc="ERROR: File name to mediafile_unpack is not provided. Aborting unpack  mediafile..."
      elif [ -z ${temp_folder} ] ; then
          error_desc="ERROR: temp folder name to mediafile_unpack is not provided. Aborting unpack  mediafile..."
      elif [ -z ${clientID} ] ; then
          error_desc="ERROR: clientID argument to mediafile_unpack is not provided. Aborting unpack  mediafile..."
      elif [ -z ${db_tar_file_name} ] ; then
          error_desc="ERROR: ${db_tar_file_name} folder  to mediafile_unpack is not provided . Aborting unpack  mediafile..."
      elif [ -z ${root_folder} ] ; then
          error_desc="ERROR: blob root folder to mediafile_unpack is not provided . Aborting unpack  mediafile..."
      elif [ -z ${uuid} ] ; then
          error_desc="ERROR: uuid folder  to mediafile_unpack is not provided . Aborting unpack  mediafile..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_unpack_notif "${_TYPE_UNPACK_ERROR}"  "${clientID}" "${dirNID}" "${uuid}" "${queueID}" "${groupFileUUID}" "${isGroupPresent}" "${error_desc}"
          return 1
      fi     

      local db_folder="${temp_folder}/${uuid}"
      local blob_uuid_folder="${root_folder}blob/${uuid}"
      local temp_blob_uuid_folder="${temp_folder}/${uuid}"
      local from_db_xml="${db_folder}/db.xml"
      local db_xml="${temp_blob_uuid_folder}/db.xml"
      local bookmarks_folder="${temp_blob_uuid_folder}/bookmarks"
      local ipvs_folder="${db_folder}/db/IPVSc_default"
      
      #temp_blob_uuid_folder
      /bin/rm -rf  ${temp_blob_uuid_folder}/blob
      #/bin/rm -rf  ${blob_uuid_folder}
      /bin/rm -rf ${db_folder}/db
      /bin/rm -rf ${db_xml}
      /bin/mkdir -p  ${temp_blob_uuid_folder}
      /bin/mkdir -p ${db_folder}
      /bin/mkdir -p ${bookmarks_folder}

      #save process pid
      save_pid "${temp_blob_uuid_folder}/process.pid" "$$"
      
      local sdp_path="${temp_blob_uuid_folder}/${uuid}.sdp"
      local hls_enable=`${_CONVERT_EXEC} -i all -f ${tar_file} | awk -F"[<>]" '/HLSConfig/ {getline; print $3;exit}'`
      local stream_type=`${_CONVERT_EXEC} -i all -f ${tar_file} | awk -F"[<>]" '/StreamType/ {getline $1; print $3;exit}'`
      local cmd="${_CONVERT_EXEC} -x -f ${tar_file} -b ${temp_blob_uuid_folder} -d ${db_folder} -t '${stream_type}'"

      # provide sdp path
      if [ "${stream_type}" = "RTP" ] ; then
          cmd="${cmd} -s ${sdp_path}"
      fi
      
      # generate HLS content
      if [ "${hls_enable}" = "true" ] ; then
          cmd="${cmd} -H"
      fi 
    
      logprint "${cmd}"
      ##${_IONICECMD} ${_CONVERT_EXEC} -x -f "${tar_file}" -b "${temp_blob_uuid_folder}" -d "${db_folder}"
      ${_IONICECMD} ${cmd}  > /dev/null 2>&1 &
      
      #save process pid
      save_pid "${temp_blob_uuid_folder}/childprocess.pid" "$!"
      	  
      #wait for process to finish
      wait "$!"
          
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="Mediaconverter failed to unpack. '${tar_file}' media_folder='${temp_blob_uuid_folder}' db='${db_folder}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_unpack_notif "${_TYPE_UNPACK_ERROR}"  "${clientID}" "${dirNID}" "${uuid}" "${queueID}" "${groupFileUUID}" "${isGroupPresent}" "${error_desc}"
          /bin/rm -rf ${temp_blob_uuid_folder}/childprocess.pid
          return 1
      fi
      /bin/rm -f ${sdp_path}
      
      /bin/rm -rf ${temp_blob_uuid_folder}/childprocess.pid
      
      ####### apply xsl and copy db.xml ##########	
      local cmd="xsltproc -o ${db_xml} --stringparam groupID ${groupID}  -stringparam dateModified $dateModified --stringparam mediaStoreFile_Rev $mediaStoreFile_Rev ${XSL_DIR}/update_groupid_db_xml.xsl ${from_db_xml}"
      logprint "${cmd}"
      xsltproc -o ${db_xml} --stringparam groupID $groupID --stringparam dateModified $dateModified --stringparam mediaStoreFile_Rev $mediaStoreFile_Rev ${XSL_DIR}/update_groupid_db_xml.xsl $from_db_xml
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="xsltproc '${from_db_xml}' to='${db_xml}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_unpack_notif "${_TYPE_UNPACK_ERROR}"  "${clientID}" "${dirNID}" "${uuid}" "${queueID}" "${groupFileUUID}" "${isGroupPresent}" "${error_desc}"
          return 1
      fi
      
	  ####### create bookmarks ##########
	  local tmp_bookmark_dir=$temp_blob_uuid_folder

    cat > $tmp_bookmark_dir/bookmark.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<Bookmark serviceVer="" Ver="1.0" Rev="1">
</Bookmark>
EOF
    for f in `ls ${ipvs_folder}/bookmark*`;
	  do
		local filename=`uuidgen`
		if test -n "${groupNID}" ; then
			for i in `xsltproc ${XSL_DIR}/get_media_bookmark_owner.xsl ${f}`;
				do
					if [ "x${i}" == "x${groupNID}" ]; then
						if [ "x${isGroupPresent}" == "xfalse" ]; then
							local cmd="xsltproc -o ${root_folder}blob/${groupFileUUID}/bookmarks/${filename}.xml --stringparam filepath ${f} --stringparam serviceVer ${serviceVer} --stringparam mediaStoreFile_Rev ${mediaStoreFile_Rev} ${XSL_DIR}/update_media_bookmark_xml.xsl ${tmp_bookmark_dir}/bookmark.xml"	
						fi
					else
						local cmd="xsltproc -o ${bookmarks_folder}/${filename}.xml --stringparam filepath ${f} --stringparam serviceVer ${serviceVer} --stringparam mediaStoreFile_Rev ${mediaStoreFile_Rev} ${XSL_DIR}/update_media_bookmark_xml.xsl ${tmp_bookmark_dir}/bookmark.xml"	
					fi
				done		
		else
			 local cmd="xsltproc -o ${bookmarks_folder}/${filename}.xml --stringparam filepath ${f} --stringparam serviceVer ${serviceVer} --stringparam mediaStoreFile_Rev ${mediaStoreFile_Rev} ${XSL_DIR}/update_media_bookmark_xml.xsl ${tmp_bookmark_dir}/bookmark.xml"
		fi
		logprint "${cmd}"                                                                                              
			${cmd}                                     
		   ret="$?"                                                                                                       
		   if [ "x${ret}" != "x0" ]; then                                                                                 
		      local error_desc="Error in creating bookmark, return_code=${ret}."                              
		      logprint "${error_desc}"                                                                                       
		      mediaroom_send_unpack_notif "${_TYPE_UNPACK_ERROR}"  "${clientID}" "${dirNID}" "${uuid}" "${queueID}" "${groupFileUUID}" "${isGroupPresent}" "${error_desc}"                                      
	    	return 1
		  fi
	done
	
	  #remove temp bookmark stuff
      /bin/rm -rf $tmp_bookmark_dir/bookmark.xml
    
      #clean temp folder before moving to blob
      /bin/rm ${temp_blob_uuid_folder}/process.pid
      /bin/rm -f ${tar_file}
    
      ######### Move unpacked media to blob ############# 
      local cmd="/bin/cp -r ${temp_blob_uuid_folder} ${root_folder}blob"
      logprint "${cmd}"
      #/bin/mv -f "${temp_blob_uuid_folder}" "${root_folder}blob"
      ${cmd}
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="move media files from=${temp_blob_uuid_folder} to=${root_folder}blob return_code=${ret}"
          logprint "${error_desc}"
          mediaroom_send_unpack_notif "${_TYPE_UNPACK_ERROR}"  "${clientID}" "${dirNID}" "${uuid}" "${queueID}" "${groupFileUUID}" "${isGroupPresent}" "${error_desc}"
          /bin/rm -rf ${temp_blob_uuid_folder}
          return 1
      fi
      
      /bin/rm -rf ${temp_blob_uuid_folder}
      
      mediaroom_send_unpack_notif "${_TYPE_UNPACK_COMPLETE}"  "${clientID}" "${dirNID}" "${uuid}" "${queueID}" "${groupFileUUID}" "${isGroupPresent}" ""
      return 0
  }

################ MediFileCutList #######
mediafile_cutlist(){
      local prepare_file_path="${1}"
      local media_files_folder="${2}"
      local prepare_file_folder="${3}"
      local fileNID="${4}"
      local cutListOffSet="${5}"
      local requestTimeStamp="${6}"
      local format="${7}"
      local stream_type="${8}"
      local sdp_path="${9}"
      local std_media_filename="${10}"
      local dirNID="${11}"
      local fileUUID="${12}"
      local typeOfRequest="${13}"
      local temp_folder="${14}"
      local root_folder="${15}"
      local db_tar_file_name="${16}"
      local uuid="${17}"
      local mediaStoreFile_Rev="${18}"
      local serviceVer="${19}"
      local old_fileUUID_size="${20}"
      local dateModified="${21}"
      local dirID="${22}"
      
      local typeOfRequest="CreateMediaFromCutlist"
      
      local error_desc=""
      if [ -z ${prepare_file_path} ] ; then
          error_desc="ERROR: Destination File name to mediafile_prepare is not provided. Aborting create  mediafile cutlist..."
      elif [ -z ${media_files_folder} ] ; then
          error_desc="ERROR: Source media files folder to mediafile_prepare is not provided. Aborting create  mediafile cutlist..."
      elif [ -z ${prepare_file_folder} ] ; then
          error_desc="ERROR: Source db files folder to mediafile_prepare is not provided. Aborting create  mediafile cutlist..."
      elif [ -z $fileNID ] ; then
          error_desc="ERROR: fileNID argument to mediafile_prepare is not provided. Aborting create  mediafile cutlist..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
          return 1
      fi
          
      #copy the db.xml to metadata    
      local db_xml="${media_files_folder}/db.xml"
      local to_db_xml="${prepare_file_folder}/db.xml"
      rm -f $to_db_xml
      cp -f $db_xml $to_db_xml
      local filePath="${prepare_file_folder}/db/IPVSc_default/${fileNID}"
      local indexFilePath="${prepare_file_folder}/db/_index"
      
      logprint "Mediaconverter create cutlist- '${prepare_file_path}' media_folder='${media_files_folder}' db='${prepare_file_folder}'"
      
      # Create the command for converting media to MKD
      local convertCmd="${_CONVERT_EXEC} -c -f ${prepare_file_path} -b ${media_files_folder} -d ${prepare_file_folder}"
      
      local cutList="${cutListOffSet}"
      
      if [ "${cutList}" != "" ] ; then
          convertCmd="${convertCmd} -C ${cutList}"
      fi
      
      munge_file_info_pack "${filePath}" "${media_files_folder}" "${to_db_xml}" "${cutList}" "${prepare_file_folder}"    
      local ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then
      	mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                                                                
      	return 1                                                                                                       
      fi
      
      ####### Munge new metadata and title in db.xml ##########
      local currTime=`date +%s`
      local currTimeInMilliSec=`expr ${currTime} \* 1000` # Approximated granularity for milliseconds
      local cmd="xsltproc -o ${to_db_xml} --stringparam path ${prepare_file_folder}/cutlist_metadata.xml --stringparam dateCreated ${currTimeInMilliSec} ${XSL_DIR}/update_cutlist_media_metadata_xml.xsl ${to_db_xml}"
      
      logprint "${cmd}"                                                                                              
      #execute the cmd
      ${cmd}
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
        local error_desc="xsltproc '${to_db_xml}' to='${to_db_xml}', return_code=${ret}."                              
        logprint "${error_desc}"                                                                                       
        mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"                                  
        return 1                                                                                                       
      fi
      
      local cutListFolder="${prepare_file_folder}/../../cutlist"
      mkdir -p $cutListFolder
      
      if [ "${format}" != "Standard" ] || [ "${stream_type}" = "V2D" ] || [ "${stream_type}" = "UDP" ] ; then
          
          #munge indexfile with bookmark info 
          munge_indexfile_info_with_bookmark "${media_files_folder}" "${cutList}" "${uuid}" "${prepare_file_folder}"
          ret="$?"                                                                                                       
          if [ "x${ret}" != "x0" ]; then
            mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
            return 1                                                                                                       
          fi
          /bin/rm -f ${prepare_file_folder}/${uuid}.xml

          ## execute the command
          logprint "${convertCmd}"
          ${_IONICECMD} ${convertCmd}   > /dev/null 2>&1
          ret="$?"
          if [ "x${ret}" != "x0" ]; then
              local error_desc="return_code=${ret}, Mediaconverter failed to create cutlist. '${prepare_file_path}' media_folder='${media_files_folder}' db='${prepare_file_folder}'"
              logprint "${error_desc}"
              mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
              return 1
          fi
          
          ## move to cutlist folder 
          mkdir -p $cutListFolder/$std_media_filename
          cp -r ${prepare_file_path} $cutListFolder/$std_media_filename/$std_media_filename.mkd
      fi

      local cutList_std_media_folder=$cutListFolder/$std_media_filename
      
      #### Start unpacking this to the blob dir for saving the new media
 	  
      local error_desc=""
      if [ -z $cutList_std_media_folder/$std_media_filename.mkd ] ; then
          error_desc="ERROR: File not present for unpacking to new media Aborting create mediafile cutlist..."
      elif [ -z ${root_folder} ] ; then
          error_desc="ERROR: blob root folder to mediafile_unpack is not provided . Aborting unpack  mediafile..."
      elif [ -z ${uuid} ] ; then
          error_desc="ERROR: uuid folder  to mediafile_unpack is not provided . Aborting unpack  mediafile..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
          return 1
      fi     
      
      local db_folder="$cutList_std_media_folder/${uuid}"
      local blob_uuid_folder="${root_folder}blob/${uuid}"
      local blob_db_xml="${db_folder}/db.xml"
      local bookmarks_folder="${db_folder}/bookmarks"
      local ipvs_folder="${db_folder}/db/IPVSc_default"
    
      #temp_blob_uuid_folder
      /bin/rm -rf ${db_folder}
      /bin/rm -rf ${blob_uuid_folder}
      /bin/rm -rf ${db_folder}/db
      /bin/rm -rf ${blob_db_xml}
 
      /bin/mkdir -p ${db_folder}
      /bin/mkdir -p ${bookmarks_folder}

      local hls_enable=`${_CONVERT_EXEC} -i all -f $cutList_std_media_folder/$std_media_filename.mkd | awk -F"[<>]" '/HLSConfig/ {getline; print $3;exit}'`
      local cmd="${_CONVERT_EXEC} -x -f $cutList_std_media_folder/$std_media_filename.mkd -b ${db_folder} -d ${db_folder} -t ${stream_type} -T"
      
      if [ "${stream_type}" = "RTP" ] ; then
          cmd="${cmd} -s ${sdp_path}"
      fi
      
      # generate HLS content
      if [ "${hls_enable}" = "true" ] ; then
          cmd="${cmd} -H"
      fi      
      
      logprint "${cmd}"
      ${_IONICECMD} ${cmd}  > /dev/null 2>&1
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="Mediaconverter failed to create mediafile cutlist. '$cutList_std_media_folder/$std_media_filename.mkd' media_folder='${db_folder}' db='${db_folder}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
          return 1
      fi
      
      ####### apply xsl and copy db.xml ##########  
      local cmd="xsltproc -o ${blob_db_xml} --stringparam dateModified $dateModified --stringparam mediaStoreFile_Rev $mediaStoreFile_Rev ${XSL_DIR}/update_groupid_db_xml.xsl ${blob_db_xml}"
      logprint "${cmd}"
      ${cmd}
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="xsltproc '${blob_db_xml}' to='${blob_db_xml}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
          return 1
      fi
      ###### create bookmarks ##########
	local tmp_bookmark_dir=$prepare_file_folder
	
    cat > $tmp_bookmark_dir/bookmark.xml <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<Bookmark serviceVer="" Ver="1.0" Rev="1">
</Bookmark>
EOF
    for f in `ls ${ipvs_folder}/bookmark*`;
	  do
		local filename=`uuidgen`
        local cmd="xsltproc -o ${bookmarks_folder}/${filename}.xml --stringparam filepath ${f} --stringparam serviceVer ${serviceVer} --stringparam mediaStoreFile_Rev ${mediaStoreFile_Rev} ${XSL_DIR}/update_media_bookmark_xml.xsl ${tmp_bookmark_dir}/bookmark.xml"
		logprint "${cmd}"                                                                                              
		${cmd}                                     
		ret="$?"                                                                                                       
		if [ "x${ret}" != "x0" ]; then                                                                                 
		    local error_desc="Error in creating bookmark, return_code=${ret}."                              
		    logprint "${error_desc}"                                                                                       
		    mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}"  "${old_fileUUID_size}"                                    
	        return 1
		fi
	done
    
    ######### Move unpacked media to blob ############# 
    local cmd="/bin/mv -f ${db_folder} ${root_folder}blob "
    logprint "${cmd}"
    #${cmd}
    /bin/mv -f "${db_folder}" "${root_folder}blob"
    ret="$?"
    if [ "x${ret}" != "x0" ]; then
        local error_desc="move media files from=${db_folder} to=${root_folder}blob return_code=${ret}"
        logprint "${error_desc}"
        mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
        return 1
    fi
     
    mediaroom_send_prepare_notif "${_TYPE_CREATE_CUTLIST_COMPLETE}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${old_fileUUID_size}"
    
    #removing cutlist temp directory
    /bin/rm -rf ${cutList_std_media_folder}
    #removing the temprorary prepare file
    rm -rf $prepare_file_folder
    
    return 0
 }

munge_indexfile_info_with_bookmark() {
  local media_files_folder="${1}"
  local cutList="${2}"
  local uuid="${3}"
  local prepare_file_folder="${4}"

  local indexFilePath="${prepare_file_folder}/db/_index"
  local playlength=`${_CONVERT_EXEC} -i gaps -f ${media_files_folder} -C ${cutList} | awk -F"[<>]" '/Duration/ {print $3;exit}'`
  local deleteBookmarks=""

  ## Munge index file with bookmark info
  for f in `ls ${media_files_folder}/bookmarks/*.xml`;
    do
      
      local deleteflag=0
      logprint "Bookmark File : ${f}"
      local bookmark_offset=`cat ${f} | sed  's#><#>\n<#g' | grep "<Offset>" | awk -F"[<>]" '/Offset/ {getline; print $3; exit}'`
      local offsets=$(echo $cutList | tr "," "\n")
      local medialength=0
      logprint "offsets : $offsets"
      for reqoffset in $offsets
        do
          local soffset=$(echo $reqoffset | cut -d':' -f1)
          local eoffset=$(echo $reqoffset | cut -d':' -f2)
          logprint "offsetrange : $reqoffset ,  bookmark_offset : $bookmark_offset, soffset : $soffset , eoffset: $eoffset, medialength : $medialength"
          if [ $bookmark_offset -ge $soffset ] && [ $bookmark_offset -le $eoffset ]; then
             local offset=`expr $medialength + $bookmark_offset - $soffset`
             logprint "New bookmark offset within range : $offset , media playlength = ${playlength}"

      ####### apply xsl to bookmarks xml ##########
      local tmp_transform_dir=$prepare_file_folder
      cat > $tmp_transform_dir/transform.xsl <<EOF
      <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
          <xsl:template match="node()|@*">
              <xsl:copy>
                  <xsl:apply-templates select="@*|node()"/>
              </xsl:copy>
      </xsl:template>
          <xsl:param name="offset"></xsl:param>
          <xsl:template match="Bookmark/Offset">
              <Offset><xsl:value-of select="string('$offset')"/></Offset>
          </xsl:template>
       </xsl:stylesheet>
EOF

             ## copy bookmark files to temp dir
             /bin/cp ${f} ${prepare_file_folder}/db/IPVSc_default/
             logprint "/bin/cp ${f} ${prepare_file_folder}/db/IPVSc_default/"
             local bookmarkFileName=`ls ${f} | awk -F"/" '{print $NF}'`
           
            #update offset to bookmark.xml
             local cmd="xsltproc -o ${prepare_file_folder}/db/IPVSc_default/$bookmarkFileName --stringparam offset ${offset} ${tmp_transform_dir}/transform.xsl ${f}"
              logprint "${cmd}"                                                                                              
              ${cmd}                                     
              ret="$?"                                                                                                       
              if [ "x${ret}" != "x0" ]; then                                                                                 
                 local error_desc="xsltproc --stringparam offset ${offset} to='${f}', return_code=${ret}."                              
                 logprint "${error_desc}"
                 return 1                                                                                                       
              fi
                   
              #create Index file to temp area from bookmarks.xml
              local cmd="xsltproc -o ${indexFilePath} --stringparam filepath ${f}  ${XSL_DIR}/update_index_file.xsl ${indexFilePath}"
              logprint "${cmd}"                                                                                              
              ${cmd}                                     
              ret="$?"                                                                                                       
              if [ "x${ret}" != "x0" ]; then                                                                                 
                local error_desc="xsltproc '${indexFilePath}' to='${indexFilePath}', return_code=${ret}."                              
                logprint "${error_desc}"
                return 1                                                                                                       
              fi
              deleteflag=1
            fi
          medialength=`expr $medialength + $eoffset - $soffset`
        done
      
      if [ "${cutList}" != "" ] && [ $deleteflag == 0 ]; then
        local bookmarkuuid=`ls ${f} | awk -F"/" '{print $NF}' | awk -F"." '{print $1}'`
        logprint "bookmarkuuid= ${bookmarkuuid}"
        deleteBookmarks="${deleteBookmarks}<Bookmark id=\"${bookmarkuuid}\"/>" 
      fi

      # copy bookmark files to temp dir
      if [ "${cutList}" == "" ]; then
        /bin/cp ${f} ${prepare_file_folder}/db/IPVSc_default/
        logprint "/bin/cp ${f} ${prepare_file_folder}/db/IPVSc_default/"
      fi
    done

# remove temp transform stuff
rm -rf $tmp_transform_dir/transform.xsl

if [ "${cutList}" != "" ]; then
  logprint "deleteBookmarkList= <DeleteBookmarkList>${deleteBookmarks}</DeleteBookmarkList>" 
  /bin/rm -f ${prepare_file_folder}/${uuid}.xml
  ####### create delbookmarks xml ##########
  cat > ${prepare_file_folder}/${uuid}.xml <<EOF
      <DeleteBookmarkList>$deleteBookmarks</DeleteBookmarkList>
EOF
fi
return 0
}

   ############## stop_prepare_service ###################
   
   stop_prepare_service(){
  		logprint "Killing prepare process.."
  		TEMP_PREPARE_AREA="/data/dms/mediafile/prepare"

		# Make sure mediaconverter is not running
		killall -9 mediaconverter
		if [ "x$?" != "x0" ]; then
		    logprint "Cannot kill prepare process... Mediaconverter is running."
		    return 1
		fi
		
		# And remove the temporary file
		if [ -d  ${TEMP_PREPARE_AREA} ]; then 
			rm -rf ${TEMP_PREPARE_AREA}/*
		fi
		echo "KILLED"
		return 0
  	}
	

   ############## stop_process ###################
   
   stop_process(){
   		local pid="${1}"
  		logprint "Killing process pid: ${pid}"
  		
  		if [ -z $pid ] ; then
          #nothing running
          logprint "mediafile_conversion_utils : nothing running $pid"
          return 0 
        fi
        
        kill -0 $pid > /dev/null 2>&1
        if [ "x$?" = "x0" ]; then
          logprint "mediafile_conversion_utils : attempting kill"
          kill $pid > /dev/null 2>&1
        fi
  		
  		logprint "KILLED ${pid}"
		return 0
  	}
  	
  ############## status tar ##############

  status() {
      local pid=`cat ${_FILE_LOCK_INSTANCE}`
      if [ "$$" = "${pid}" ] ; then
          #should never come here. Same script is tring to acquire lock again.
          #logshow "'$0' Instance lock has been already aquired."
          echo "BUSY"    
          return 1
      else
          kill -0 $pid > /dev/null 2>&1
          if [ "x$?" = "x0" ]; then
              #ERROR: Busy: Another instance of '$0' is running (time=${diff} secs, prev_pid=${pid})
              echo "BUSY"    
              return 1
          else
              #logshow "WARNING: Previous instance of '$0' attempt exited prematurely?? (time=${diff} secs, prev_pid=${pid})"
              echo "FREE"    
              return 0
          fi
      fi
      echo "FREE"   
      return 0
  }

  ############## stop tar ##############

  stop() {
      logprint "mediafile_conversion_utils : stop request"
      local pid=`cat ${_FILE_LOCK_INSTANCE}`
      if [ -z $pid ] ; then
          #nothing running
          logprint "mediafile_conversion_utils : nothing running $pid"
          return 0 
      fi                 
      
      if [ "$$" = "${pid}" ] ; then
          #should never come here. Same script is tring to acquire lock again.
          logprint "mediafile_conversion_utils : should never come here. Same script is tring to acquire lock again."
          return 1
      fi                 
      #get time difference in seconds
      local filedate=`date -r "${_FILE_LOCK_INSTANCE}"  +%s`
      local curdate=`date +%s`
      local diff=`expr $curdate - $filedate`

      kill -0 $pid > /dev/null 2>&1
      if [ "x$?" = "x0" ]; then
          #Busy: Another instance of '$0' is running (time=${diff} secs, prev_pid=${pid})
          logprint "mediafile_conversion_utils : kill. Another instance of '$0' is running (time=${diff} secs, prev_pid=${pid})"
          kill -9 $pid > /dev/null 2>&1
          local err=0
          local count=0
          local timout=60
          while(true); do
              count=`expr ${count} + 1`
              kill -0 $pid > /dev/null 2>&1
              if [ "x$?" != "x0" ]; then
                  break
              fi
              if [ $count -gt $timout ]; then
                  err=1
                  break
              fi
              logprint "Waiting to stop file conversion. [${count}/${timout}]"
              sleep 1
          done
          if [ $err = "1" ] ; then
              logprint "ERROR: Could not file conversion."
              return 1
          fi
      fi
      
      logprint "mediafile_conversion_utils : stopped $pid"
      return 0

  }

cleardownloadcache() {     
      local media_files_folder="${1}"     
      local fileNID="${2}"    
      local dirNID="${3}"
      local fileUUID="${4}"     
  
      local error_desc=""      
      if [ -z ${media_files_folder} ] ; then
          error_desc="ERROR: Source media files folder to cleardownloadcache is not provided. Aborting clear download cache..."     
      elif [ -z $fileNID ] ; then
          error_desc="ERROR: fileNID argument to cleardownloadcache is not provided. Aborting clear download cache..."
      elif [ -z $dirNID ] ; then
          error_desc="ERROR: dirNID argument to cleardownloadcache is not provided. Aborting clear download cache..."
      fi

      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_CLEAR_CACHE_ERROR}"  "${fileNID}" "${error_desc}" "${dirNID}" "${fileUUID}" 
          return 1
      fi	
  
      #removing proprietary dir from blob Directory
      if [ -d ${media_files_folder}/proprietary ]; then 
		  rm -rf $media_files_folder/proprietary            	      
      fi  
      
      #removing standard dir from blob Directory
      if [ -d ${media_files_folder}/standard ]; then 
		  rm -rf $media_files_folder/standard  	   
      fi  
     
      #sleep for 5 seconds. for NAS directory immedialty we can not get disk size update.
      sleep 5
      mediaroom_send_notif "${_TYPE_CLEAR_CACHE_COMPLETE}"  "${fileNID}" "${error_desc}" "${dirNID}" "${fileUUID}" 
      return 0
  }
 
media_ingestion() {     
      local temp_files="${1}" 
      local media_files_folder="${2}" 
      local temp_folder="${3}"
      local sdp_path="${4}"  
      local dirID="${5}"
      local fileUUID="${6}"
      local userHLSflag="${7}"
      local standardMediaFileName="${8}"
  
      local error_desc=""
      if [ -z ${temp_files} ] ; then
          error_desc="ERROR: Source File name to media ingestion is not provided. Aborting media ingestion..."
      elif [ -z ${media_files_folder} ] ; then
          error_desc="ERROR: Destination media files folder to media ingestion is not provided. Aborting media ingestion..."
      elif [ -z ${temp_folder} ] ; then
          error_desc="ERROR: Temp files folder to media ingestion is not provided. Aborting media ingestion..."
      elif [ -z ${sdp_path} ] ; then
          error_desc="ERROR: SDP files path to media ingestion is not provided. Aborting media ingestion..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_MEDIA_INGESTION_ERROR}" "${sdp_path}" "${error_desc}" "${dirID}" "${fileUUID}"
          return 1
      fi

      local temp_blob_uuid_folder=$temp_folder/$fileUUID
      /bin/mkdir -p  "${temp_blob_uuid_folder}/bookmarks"
      
      local eror_string=`${_IMPORTER_EXEC} -i -f $temp_files | awk -F"[<>]" '/ErrorString/ {print $3;exit}'`
      local hls_enable=`${_IMPORTER_EXEC} -i -f $temp_files | awk -F"[<>]" '/HLSCompatible/ {print $3;exit}'`

      local cmd="${_IMPORTER_EXEC} -c -f $temp_files -d ${temp_blob_uuid_folder} -s ${sdp_path} -t"
      local hls=false
       
       # generate HLS content
 
	   if [ "${userHLSflag}" != "" ] && [ "${userHLSflag}" != "false" ] && [ "${hls_enable}" = "TRUE" ] ; then	     
		  hls=true
		  cmd="${cmd} -H"	           
	   fi  
	     
      logprint "${cmd}"
${_IONICECMD} ${cmd}   > /dev/null 2>&1
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="Mediaconverter failed to media ingestion. '$temp_files' media_folder='${media_files_folder}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_MEDIA_INGESTION_ERROR}"  "${sdp_path}" "${error_desc}" "${dirID}" "${fileUUID}"
          return 1
      fi 
      
      playlength=`${_CONVERT_EXEC} -i gaps -f "${temp_blob_uuid_folder}" | awk -F"[<>]" '/Duration/ {print $3;exit}'`
      local tn_enable=false
      #check Thumbnail config
      if [ -f ${temp_blob_uuid_folder}/tn/tn.jpg ]; then 
	   tn_enable=true  	   
      fi 

   ####### apply xsl to media resource and db.xml ##########
      local tmp_transform_dir=$temp_blob_uuid_folder
      cat > $tmp_transform_dir/transform.xsl <<EOF
      <xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
          <xsl:template match="node()|@*">
              <xsl:copy>
                  <xsl:apply-templates select="@*|node()"/>
              </xsl:copy>
      </xsl:template>
          <xsl:param name="playlength"></xsl:param>
	  <xsl:param name="hls"></xsl:param>
	  <xsl:param name="tn_enable"></xsl:param>
          <xsl:template match="MediaStoreFile/MediaClip/PlayLength">
              <PlayLength><xsl:value-of select="string('$playlength')"/></PlayLength>
          </xsl:template>
          <xsl:template match="MediaStoreFile/MediaClip/MediaStreamProfileInfo/RTPMediaStreamProfileInfo/RTPRecoderConfig/HLSConfig/Enable">
              <Enable><xsl:value-of select="string('$hls')"/></Enable>
          </xsl:template>
          <xsl:template match="MediaStoreFile/MediaClip/MediaStreamProfileInfo/RTPMediaStreamProfileInfo/RTPRecoderConfig/ThumbnailConfig/Enable">
              <Enable><xsl:value-of select="string('$tn_enable')"/></Enable>
          </xsl:template>
       </xsl:stylesheet>
EOF

      local cmd="xsltproc -o ${temp_blob_uuid_folder}/db.xml --stringparam playlength ${playlength} --stringparam hls ${hls} --stringparam tn_enable ${tn_enable} ${tmp_transform_dir}/transform.xsl ${temp_blob_uuid_folder}/db.xml"
      logprint "${cmd}"                                                                                              
      #execute the cmd
      ${cmd}
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
      	local error_desc="xsltproc '${temp_blob_uuid_folder}/db.xml' to='${temp_blob_uuid_folder}/db.xml', return_code=${ret}."                              
      	logprint "${error_desc}"
      	mediaroom_send_notif "${_TYPE_MEDIA_INGESTION_ERROR}" "${sdp_path}" "${error_desc}" "${dirID}" "${fileUUID}"
      	return 1                                                                                                       
      fi
      
      #remove temp transform stuff
      rm -rf $tmp_transform_dir/transform.xsl

      #Create/Update native Preapre location
      local indexFilePath="${temp_blob_uuid_folder}/db/_index"
            
      #Create MediaInfo file to native lacation
      create_update_metadata_pack "${temp_blob_uuid_folder}" "${indexFilePath}" "${standardMediaFileName}" "${temp_blob_uuid_folder}"
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then                                                                                 
         local error_desc="Mediaconverter failed to media ingestion. '$temp_files' Prepare_folder='${preparepath}/${standardMediaFileName}.zip', return_code=${ret}."
         logprint "${error_desc}"
         mediaroom_send_notif "${_TYPE_MEDIA_INGESTION_ERROR}" "${sdp_path}" "${error_desc}" "${dirID}" "${fileUUID}"                                  
         return 1                                                                                                       
      fi

      local tempfileExt=$(echo $temp_files | cut -d'.' -f2)
      logprint "tempfileExt: $tempfileExt"
      cmd="/bin/mv -f ${temp_files} ${temp_blob_uuid_folder}/standard/${standardMediaFileName}/${standardMediaFileName}.${tempfileExt}"
      logprint "${cmd}"
      ${cmd}
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="Mediaconverter failed to media ingestion. '$temp_files' to='${temp_blob_uuid_folder}/standard/${standardMediaFileName}/${standardMediaFileName}.${tempfileExt}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_MEDIA_INGESTION_ERROR}" "${sdp_path}" "${error_desc}" "${dirID}" "${fileUUID}"
          return 1
      fi 

      cd "${temp_blob_uuid_folder}/standard"
      cmd="zip -r ${standardMediaFileName}.zip ${standardMediaFileName}"
      logprint "${cmd}"
      ${cmd}
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="Mediaconverter failed to media ingestion. '$temp_files' Prepare_folder='${temp_blob_uuid_folder}/standard/${standardMediaFileName}.zip', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_MEDIA_INGESTION_ERROR}" "${sdp_path}" "${error_desc}" "${dirID}" "${fileUUID}"
          return 1
      fi 

      #remove if already exist in blob
      /bin/rm -rf ${media_files_folder}
      /bin/rm -rf "${temp_blob_uuid_folder}/standard/${standardMediaFileName}"

      cmd="/bin/mv -f ${temp_blob_uuid_folder} ${media_files_folder}"
      logprint "${cmd}"
      ${cmd}
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="Mediaconverter failed to media ingestion. '$temp_files' media_folder='${media_files_folder}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_notif "${_TYPE_MEDIA_INGESTION_ERROR}" "${sdp_path}" "${error_desc}" "${dirID}" "${fileUUID}"
          return 1
      fi 
          
      mediaroom_send_notif "${_TYPE_MEDIA_INGESTION_COMPLETE}" "${sdp_path}" "${error_desc}" "${dirID}" "${fileUUID}"
      return 0
}

validate_media_ingestion() {     
      local temp_files="${1}" 
  
      local error_desc=""
      if [ -z ${temp_files} ] ; then
          error_desc="ERROR: Source File name to media ingestion is not provided. Aborting media ingestion..."
      fi

      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          return 1
      fi
      
      local import_enable=`${_IMPORTER_EXEC} -i -f $temp_files | awk -F"[<>]" '/MediaImportable/ {print $3;exit}'`
      local eror_string=`${_IMPORTER_EXEC} -i -f $temp_files | awk -F"[<>]" '/ErrorString/ {print $3;exit}'`
      
      if [ "${import_enable}" = "FALSE" ] ; then
          error_desc="ERROR: $eror_string. Aborting media ingestion..."
          logprint "${error_desc}"
          return 1
      fi   
      return 0
} 

################ Trim MediaFile #######
mediafile_trim(){
      local prepare_file_path="${1}"
      local media_files_folder="${2}"
      local prepare_file_folder="${3}"
      local fileNID="${4}"
      local cutListOffSet="${5}"
      local stream_type="${6}"
      local sdp_path="${7}"
      local std_media_filename="${8}"
      local dirNID="${9}"
      local root_folder="${10}"
      local uuid="${11}"
      local mediaStoreFile_Rev="${12}"
      local serviceVer="${13}"
      local fileUUID_size="${14}"
      local dateModified="${15}"
           
      local format="Proprietary"
      local typeOfRequest="TrimFile"
      
      local error_desc=""
      if [ -z ${prepare_file_path} ] ; then
          error_desc="ERROR: Destination File name to media_trim is not provided. Aborting Trim File..."
      elif [ -z ${media_files_folder} ] ; then
          error_desc="ERROR: Source media files folder to media_trim is not provided. Aborting Trim File..."
      elif [ -z ${prepare_file_folder} ] ; then
          error_desc="ERROR: Source db files folder to media_trim is not provided. Aborting Trim File..."
      elif [ -z $fileNID ] ; then
          error_desc="ERROR: fileNID argument to media_trim is not provided. Aborting Trim File..."
      elif [ -z $cutListOffSet ] ; then
          error_desc="ERROR: cutListOffSet argument to media_trim is not provided. Aborting Trim File..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_TRIM_MEDIA_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${fileUUID_size}"
          return 1
      fi
      
      local cutListFolder="${prepare_file_folder}/../../cutlist"
      
      mkdir -p $cutListFolder
      
      #copy the db.xml to metadata    
      local db_xml="${media_files_folder}/db.xml"
      local to_db_xml="${prepare_file_folder}/db.xml"
      rm -f $to_db_xml
      cp -f $db_xml $to_db_xml
      local filePath="${prepare_file_folder}/db/IPVSc_default/${fileNID}"
      local indexFilePath="${prepare_file_folder}/db/_index"
      
      logprint "Mediaconverter Trim File- '${prepare_file_path}' media_folder='${media_files_folder}' db='${prepare_file_folder}'"
      
      # Create the command for converting media to MKD
      local convertCmd="${_CONVERT_EXEC} -c -f ${prepare_file_path} -b ${media_files_folder} -d ${prepare_file_folder}"
      
      local cutList="${cutListOffSet}"
      
      if [ "${cutList}" != "" ] ; then
          convertCmd="${convertCmd} -C ${cutList}"
      fi
      
      munge_file_info_pack "${filePath}" "${media_files_folder}" "${to_db_xml}" "${cutList}" "${prepare_file_folder}"      
      ret="$?"                                                                                                       
      if [ "x${ret}" != "x0" ]; then
      	mediaroom_send_prepare_notif "${_TYPE_TRIM_MEDIA_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${fileUUID_size}"                                                                                
      	return 1                                                                                                       
      fi
                
      if [ "${format}" != "Standard" ] || [ "${stream_type}" = "V2D" ] || [ "${stream_type}" = "UDP" ] ; then
      
        #munge indexfile with bookmark info 
        munge_indexfile_info_with_bookmark "${media_files_folder}" "${cutList}" "${uuid}" "${prepare_file_folder}"

      	## execute the command
      	logprint "${convertCmd}"
      	${_IONICECMD} ${convertCmd}  > /dev/null 2>&1
      	ret="$?"
      	if [ "x${ret}" != "x0" ]; then
          local error_desc="return_code=${ret}, Mediaconverter failed to trim file. '${prepare_file_path}' media_folder='${media_files_folder}' db='${prepare_file_folder}'"
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_TRIM_MEDIA_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${fileUUID_size}"
          return 1
      	fi
          
        ## move to cutlist folder 
        mkdir -p $cutListFolder/$std_media_filename
        cp -r ${prepare_file_path} $cutListFolder/$std_media_filename/$std_media_filename.mkd
     fi

     #### Start unpacking this to the blob dir for saving the new media
 	  
      local error_desc=""
      if [ -z $cutListFolder/$std_media_filename/$std_media_filename.mkd ] ; then
          error_desc="ERROR: File not present for trim to new media Aborting Trim File..."
      elif [ -z ${root_folder} ] ; then
          error_desc="ERROR: blob root folder to mediafile_trim is not provided . Aborting Trim File..."
      elif [ -z ${uuid} ] ; then
          error_desc="ERROR: uuid folder  to mediafile_trim is not provided . Aborting Trim File..."
      fi
      if [ ! -z $error_desc ] ; then
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_TRIM_MEDIA_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${fileUUID_size}"
          return 1
      fi     
      
      local db_folder="$cutListFolder/$std_media_filename/${uuid}"
      local blob_uuid_folder="${root_folder}blob/${uuid}"
      local from_db_xml="${db_folder}/db.xml"
      local bookmarks_folder="${db_folder}/bookmarks"
      local ipvs_folder="${db_folder}/db/IPVSc_default"
    
      logprint "Deleting temp folders ..."
      logprint "blob_uuid_folder : ${blob_uuid_folder}, db_folder/db : ${db_folder}/db, from_db_xml: ${from_db_xml}"
      /bin/rm -rf  ${blob_uuid_folder}
      /bin/rm -rf ${db_folder}/db
      /bin/rm -rf ${from_db_xml}
      
      /bin/mkdir -p ${db_folder}
      /bin/mkdir -p ${bookmarks_folder}

      local hls_enable=`${_CONVERT_EXEC} -i all -f $cutListFolder/$std_media_filename/$std_media_filename.mkd | awk -F"[<>]" '/HLSConfig/ {getline; print $3;exit}'`
      local cmd="${_CONVERT_EXEC} -x -f $cutListFolder/$std_media_filename/$std_media_filename.mkd -b ${db_folder} -d ${db_folder} -t ${stream_type} -T"
      
      if [ "${stream_type}" = "RTP" ] ; then
          cmd="${cmd} -s ${sdp_path}"
      fi
      
      # generate HLS content
      if [ "${hls_enable}" = "true" ] ; then
          cmd="${cmd} -H"
      fi      
      
      logprint "${cmd}"
      ${_IONICECMD} ${cmd}  > /dev/null 2>&1
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="Mediaconverter failed to Trim Media. '$cutListFolder/$std_media_filename/$std_media_filename.mkd' media_folder='${db_folder}' db='${db_folder}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_TRIM_MEDIA_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${fileUUID_size}"
          return 1
      fi
      
      ####### apply xsl and copy db.xml ##########  
      local cmd="xsltproc -o ${from_db_xml} --stringparam dateModified $dateModified --stringparam mediaStoreFile_Rev $mediaStoreFile_Rev ${XSL_DIR}/update_groupid_db_xml.xsl ${from_db_xml}"
      logprint "${cmd}"
      ${cmd}
      ret="$?"
      if [ "x${ret}" != "x0" ]; then
          local error_desc="xsltproc '${from_db_xml}' to='${db_xml}', return_code=${ret}."
          logprint "${error_desc}"
          mediaroom_send_prepare_notif "${_TYPE_TRIM_MEDIA_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${fileUUID_size}"
          return 1
      fi

    ####### Cpoy Bookmarks to temp area ######
     for f in `ls ${prepare_file_folder}/db/IPVSc_default/*.xml`;
        do
            /bin/cp ${f} ${db_folder}/bookmarks
            logprint "/bin/cp ${f} ${db_folder}/bookmarks"
        done
    
    ######### Move unpacked media to blob ############# 
    local cmd="/bin/mv -f ${db_folder} ${root_folder}blob "
    logprint "${cmd}"
    /bin/mv -f "${db_folder}" "${root_folder}blob"
    ret="$?"
    if [ "x${ret}" != "x0" ]; then
        local error_desc="move media files from=${db_folder} to=${root_folder}blob return_code=${ret}"
        logprint "${error_desc}"
        mediaroom_send_prepare_notif "${_TYPE_TRIM_MEDIA_ERROR}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${fileUUID_size}"
        return 1
    fi
    
    mediaroom_send_prepare_notif "${_TYPE_TRIM_MEDIA_COMPLETE}"  "${fileNID}" "${dirNID}" "${uuid}" "${format}" "${error_desc}" "${typeOfRequest}" "${std_media_filename}" "${fileUUID_size}"
    
    #removing the temprorary file
    #rm -rf $prepare_file_folder
    rm -rf $cutListFolder/$std_media_filename
    rm -rf ${sdp_path}
    
    return 0
 }
 
 
 stop_mediaconverter_service(){
      logprint "Killing mediaconverter process.."
      RECYCLE_BIN_AREA="/data/mediadir/trash"
      TEMP_PREPARE_AREA="/data/dms/mediafile/prepare"
      TEMP_CUTLIST_AREA="/data/dms/mediafile/cutlist"
      TEMP_IMPORT_AREA="/data/mediadir/inject"
      TEMP_EXPORT_AREA="/data/mediadir/export"

      # Make sure mediaconverter is not running
      killall -9 mediaconverter
      if [ "x$?" != "x0" ]; then
          logprint "Cannot kill prepare process... Mediaconverter is running."
      fi
      
      /bin/mkdir -p ${RECYCLE_BIN_AREA}
              
      #echo "KILLED"
      # And remove the temporary file
      if [ -d  ${TEMP_PREPARE_AREA} ]; then
            if [ "$(ls -A ${TEMP_PREPARE_AREA})" ]; then
                /bin/mv -f ${TEMP_PREPARE_AREA}/*  ${RECYCLE_BIN_AREA}/              
            fi     
      fi
      if [ -d  ${TEMP_IMPORT_AREA} ]; then 
          if [ "$(ls -A ${TEMP_IMPORT_AREA})" ]; then
              /bin/mv -f ${TEMP_IMPORT_AREA}/* ${RECYCLE_BIN_AREA}/
          fi
      fi
      if [ -d  ${TEMP_EXPORT_AREA} ]; then
            if [ "$(ls -A ${TEMP_EXPORT_AREA})" ]; then 
                /bin/mv -f ${TEMP_EXPORT_AREA}/* ${RECYCLE_BIN_AREA}/
            fi
      fi
      
      if [ -d  ${TEMP_CUTLIST_AREA} ]; then
            if [ "$(ls -A ${TEMP_CUTLIST_AREA})" ]; then 
                /bin/mv -f ${TEMP_CUTLIST_AREA}/* ${RECYCLE_BIN_AREA}/
            fi
      fi
              
      return 0
}

is_mediagroup_present(){
	local blob_path="${1}"
	local mediaGroupID="${2}"
	local uuid="${3}"
	file=/data/mediadir/inject/$uuid/group
	grep -lr $mediaGroupID ${blob_path}/*/db.xml > $file	
	while read p; do
		type=`xsltproc ${XSL_DIR}/get_media_type.xsl ${p}`;	
	if [ "${type}" = "MediaGroup" ]; then		
		/bin/rm -f $file
		return 0
	fi
	done <$file	
	/bin/rm -f $file
	return 1
						
}	

  action_name="$1"

  case "$1" in

    mediafile_prepare)
      	local prepare_file_path="$2"
      	local media_files_folder="$3"
      	local prepare_file_folder="$4"
      	local fileNID="$5"
      	local startOffset="$6"
      	local endOffset="$7"
      	local requestTimeStamp="$8"
      	local format="$9"
      	local stream_type="$10"
      	local sdp_path="$11"
      	local std_media_filname="$12"
      	local dirNID="$13"
      	local fileUUID="$14"
      	local typeOfRequest="$15"
      	local old_fileUUID_size="$16"
      	local new_UUID="$17"
      	mediafile_prepare  "$prepare_file_path" "$media_files_folder" "$prepare_file_folder" "$fileNID" "$startOffset" "$endOffset" "$requestTimeStamp" "$format" "$stream_type" "$sdp_path" "$std_media_filname" "$dirNID" "$fileUUID" "$typeOfRequest" "$old_fileUUID_size" "$new_UUID"
      	RETVAL="$?"
      	;;
         
    mediafile_unpack)
    	mediafile_unpack  "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "$10" "$11" "$12" "$13" "$14" "$15" "$16" "$17" "$18"
    	RETVAL="$?"
    	;;

    mediafile_cutlist)
    	mediafile_cutlist "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "$10" "$11" "$12" "$13" "$14" "$15" "$16" "$17" "$18" "${19}" "${20}" "${21}" "${22}"
     	RETVAL="$?"
    	;;

    cleardownloadcache)
    	local media_files_folder="$2"
    	local fileNID="$3"
    	local dirNID="$4"
    	local fileUUID="$5"
    	cleardownloadcache "$media_files_folder" "$fileNID" "$dirNID" "$fileUUID"
    	RETVAL="$?"
    	;;

    media_ingestion)
    	local temp_files="${2}" 
    	local media_files_folder="${3}" 
    	local temp_folder="${4}"
    	local sdp_path="${5}"  
    	local dirID="${6}"
    	local fileUUID="${7}"
    	local userHLSflag="${8}"
    	local standardMediaFileName="${9}"
    	media_ingestion "$temp_files" "$media_files_folder" "$temp_folder" "$sdp_path" "$dirID" "$fileUUID" "$userHLSflag" "$standardMediaFileName"
    	RETVAL="$?"
    	;;

    mediafile_trim)
      	local prepare_file_path="${2}"
      	local media_files_folder="${3}"
      	local prepare_file_folder="${4}"
     	 local fileNID="${5}"
      	local cutListOffSet="${6}"
      	local stream_type="${7}"
      	local sdp_path="${8}"
      	local std_media_filename="${9}"
      	local dirNID="${10}"
      	local root_folder="${11}"
      	local uuid="${12}"
      	local mediaStoreFile_Rev="${13}"
      	local serviceVer="${14}"
      	local fileUUID_size="${15}"
      	local dateModified="${16}"
      	mediafile_trim "$prepare_file_path" "$media_files_folder" "$prepare_file_folder" "$fileNID" "$cutListOffSet" "$stream_type" "$sdp_path" "$std_media_filename" "$dirNID" "$root_folder" "$uuid" "$mediaStoreFile_Rev" "$serviceVer" "$fileUUID_size" "$dateModified"
      	RETVAL="$?"
    	;;

    validate_media_ingestion)
    	local temp_files="${2}" 
    	validate_media_ingestion "$temp_files"
    	RETVAL="$?"
    	;;

    mediaGroup_prepare)
      	local prepare_file_path="$2"
      	local media_files_folder="$3"
      	local prepare_file_folder="$4"
      	local fileNID="$5"
      	local startOffset="$6"
      	local endOffset="$7"
      	local requestTimeStamp="$8"
      	local format="$9"
      	local stream_type="$10"
      	local sdp_path="$11"
      	local std_media_filname="$12"
      	local dirNID="$13"
      	local fileUUID="$14"
      	local typeOfRequest="$15"
      	local old_fileUUID_size="$16"
      	local media_group_playlength="$17"
      	mediaGroup_prepare  "$prepare_file_path" "$media_files_folder" "$prepare_file_folder" "$fileNID" "$startOffset" "$endOffset" "$requestTimeStamp" "$format" "$stream_type" "$sdp_path" "$std_media_filname" "$dirNID" "$fileUUID" "$typeOfRequest" "$old_fileUUID_size" "$media_group_playlength"
      	RETVAL="$?"
      	;;
    
    stop_prepare_service)
      	stop_prepare_service
      	RETVAL="$?"
      	;;
      
    stop_process)
      	stop_process  "$2" 
      	RETVAL="$?"
      	;;
      	
    status)
      	status
      	RETVAL="$?"
      	;;

    stop)
      	stop
      	RETVAL="$?"
      	;;

    mediafile_unpack_2dot4)
      	mediafile_unpack_2dot4  "$2" "$3" "$4" "$5" "$6" "$7" "$8" "$9" "$10" 
      	RETVAL="$?"
      	;;

    remove_2dot4_media)
      	remove_2dot4_media  "$2" "$3" "$4" "$5" 
      	RETVAL="$?"
      	;;
      
      stop_mediaconverter_service)
      stop_mediaconverter_service
      RETVAL="$?"
      ;;
	is_mediagroup_present)
      	is_mediagroup_present  "$2" "$3" "$4" 
      	RETVAL="$?"
      	;;

      *)

      	echo "Usage: $0 [ mediafile_pack | mediafile_prepare | mediaGroup_prepare | mediafile_unpack | mediafile_cutlist | cleardownloadcache | media_ingestion | mediafile_trim | validate_media_ingestion | stop_prepare_service | stop_process | status | stop ]"
      	RETVAL=1
  esac
  exit $RETVAL
  