#!/bin/sh

# Mapping for return error codes from the script 
# (Any change here should also reflect in calling Java code)
# |------|----|-----------------------------|
# | Code | -- | Description                 |
# |------|----|-----------------------------|
# | 1    | -- | MISSING_IMAGE_FILE_DIR_NAME |
# | 2    | -- | MISSING_IMAGE_FILE          |
# | 3    | -- | INVALID_TAR_CONTENTS        |
# | 4    | -- | UNTAR_IMAGE_FAILED          |
# | 5    | -- | INVALID_NGSD200_TAR         |
# | 6    | -- | INVALID_URL_FOR_UPGRADE     |
# | 7    | -- | UPGRADE_FAILED_FOR_URL      |
# |------|----|-----------------------------|
 
 MISSING_IMAGE_FILE_DIR_NAME=1
 MISSING_IMAGE_FILE=2
 INVALID_TAR_CONTENTS=3
 UNTAR_IMAGE_FAILED=4
 INVALID_NGSD200_TAR=5
 INVALID_URL_FOR_UPGRADE=6
 UPGRADE_FAILED_FOR_URL=7
 
 IMAGES_FOLDER_NAME="IMAGES"
 MEDIAROOM_MODULE_UPGRADE_AREA="/home/onair/modules/mediaroom/public/upgrade_IMAGES"
 NGS_UPGRADE_AREA="/home/onair/modules/mediaroom/public/ngs_IMAGES"
 XSL_DIR="/etc/scripts/xsl"
 
 
 logit() {
    message="${1}: ${2}"
    /usr/bin/logger -t upgrade "${message}"
    echo "${message}"

 }
 log_info() {
    logit "INFO" "$1"
 }

 log_error() {
    logit "ERROR" "$1"
 }

 image_tar_file=${1}
 untar_dir=${2}   
 software_URL=${3}
 deviceType=${4}
 msSerVersion=${5}

 image_tar_file_len=`expr length "$image_tar_file"`
 untar_dir_len=`expr length "$untar_dir"`
    
 if [ "x$image_tar_file_len" = "x0"  -o  "x$untar_dir_len" = "x0" ]; then
    log_error "tar file name and untar dir required"
    exit ${MISSING_IMAGE_FILE_DIR_NAME}
 fi 

 if [ ! -f ${image_tar_file} ]; then
	log_error "Input Tar file not found ${image_tar_file}"
 	exit ${MISSING_IMAGE_FILE}
 fi 

 if [ -d  ${untar_dir}/${IMAGES_FOLDER_NAME} ]; then 
	rm -rf ${untar_dir}/${IMAGES_FOLDER_NAME}
 fi

 cd ${untar_dir}

 tar tvf ${image_tar_file} | grep IMAGES/server.version
 
 if [ "x$?" != "x0" ]; then
	log_error "Invalid tar file contents, cleaning temporary area"
	rm -rf ./*
 	exit ${INVALID_TAR_CONTENTS}
 fi
 
 tar xvf ${image_tar_file}

 if [ "x$?" != "x0" ]; then
	log_error "Untar of ${image_tar_file} file failed."   
	exit ${UNTAR_IMAGE_FAILED}
 fi 
 log_info "Validating tar"
 
 if [ "${deviceType}" = "V2D-XPi-Device" ] ; then 
  if [ -f ${IMAGES_FOLDER_NAME}/V2D-XPi-Device.xml ] ; then
   current_version=`xsltproc ${XSL_DIR}/get_service_version.xsl ${IMAGES_FOLDER_NAME}/V2D-XPi-Device.xml`;
   log_info "xpi version ${current_version} and MS version ${msSerVersion}"  
    if [ "${current_version}" != "${msSerVersion}" ] ; then
	 # this check is removed to allow NGSD200 devices in 1.1 version 
	 # to work with the management software in Nimbus 4.2 version.
	 log_info "Allowing NGSD200 device to login with ${current_version}"
	fi
	if [ ! -f ${IMAGES_FOLDER_NAME}/manifest_xpi.txt ] ; then
	 rm -rf ${IMAGES_FOLDER_NAME}
	 log_error "Invalid tar, upload a NGS-D200 Image"   
	 exit ${INVALID_NGSD200_TAR}
    fi
  fi
 fi
 log_info "Validation successful"

 

 log_info "Copied Software Image to ${untar_dir}"
 log_info "Copied Software Image to ${NGS_UPGRADE_AREA}"
 log_info "Software Image to ${MEDIAROOM_MODULE_UPGRADE_AREA}" 

 if [ "${deviceType}" = "V2D-XPi-Device" ] ; then
  /bin/rm -rf ${NGS_UPGRADE_AREA} 
  if [ ! -d ${NGS_UPGRADE_AREA} ] ; then     
     ln -s ${untar_dir}/${IMAGES_FOLDER_NAME} ${NGS_UPGRADE_AREA}
     log_info "Link upgrade dir ${untar_dir}/${IMAGES_FOLDER_NAME} to public area ${NGS_UPGRADE_AREA}" 
  fi  
 fi
 
 if [ "${deviceType}" != "V2D-XPi-Device" ] ; then
 /bin/rm -rf ${MEDIAROOM_MODULE_UPGRADE_AREA}
 if [ ! -d ${MEDIAROOM_MODULE_UPGRADE_AREA} ] ; then     
     ln -s ${untar_dir}/${IMAGES_FOLDER_NAME} ${MEDIAROOM_MODULE_UPGRADE_AREA}
     log_info "Link upgrade dir ${untar_dir}/${IMAGES_FOLDER_NAME} to public area ${MEDIAROOM_MODULE_UPGRADE_AREA}" 
 fi
 fi

 
 url_len=`expr length "$software_URL"`
     
 if [ "x$url_len" = "x0" ]; then
    log_error "URL Required."
    exit ${INVALID_URL_FOR_UPGRADE}
 fi 

 if [ "${deviceType}" != "V2D-XPi-Device" ] ; then
 /V2O/bin/upgrade_device -url $software_URL

 if [ "x$?" != "x0" ]; then
	log_error "software Upgrade Failed."   
	exit ${UPGRADE_FAILED_FOR_URL}
 fi
 fi
 
 log_info "Upraded Device"