#!/bin/sh

TMP_INSTALL_DIR=./tmp_install_dir

currentdir=`pwd`

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


cleanup () {
    cd $currentdir
    #rm -rf ${TMP_INSTALL_DIR}
}

get_file () {
    local URL=$1
    local TARGET=$2

    log_info "Downloading ${URL}..."
    if wget --connect-timeout 5 -t 3 $URL -O $TARGET; then
        log_info "Downloaded ${URL}"
    else
        log_error "Could not download ${URL}."
        log_info "Please check ${URL} and make sure"
        log_info "the file can be retrieved."
        cleanup
        exit 1
    fi
}

quit_with_error () {
    local message=$1

    log_error "$message. Aborting..."
    cleanup
    exit 1
}

show_usage() {
    log_info ""
    log_info "Usage: software_install.sh URL Device_Type"
    log_info ""
    log_info "List of supported Device_Types:"
    log_info "MS, MS-25, MS-150"
    log_info "DMS-25, DMS-100, DMS-120,DMS-150, DMS-200, DMS-1000, DMS-1200, DMS-1500,"
    log_info "DMS-1600, DMS-1700, DMS-1550"
    log_info "V2D-Tx, V2D-Rx, V2D-XP200, V2D-XP220,"
    log_info "V2D-XP200S, V2D-XP220S,  V2D-XP100, Generic"
    log_info "V2D-XPi"
    log_info ""
    log_info "Example:"
    log_info "software_install.sh http://192.168.1.228/DMS/IMAGES V2D-Tx"
}

# Get URL from command line
if [ x"$1" = "x" -o x"$2" = "x" ]; then
    log_error "BAD Usage: Need two arguments."
    show_usage
    exit
fi

DOWNLOAD_URL=$1
DEVICE=$2


# Determine manifest file
MANIFEST=""
if [ x"${DEVICE}" = "xORB-500" ] ; then
    MANIFEST=manifest_orb.txt
fi

if [ x"${DEVICE}" = "xV2D-Tx" ] ; then
    MANIFEST=manifest_v2d.txt
fi

if [ x"${DEVICE}" = "xV2D-XP200" ] ; then
    MANIFEST=manifest_v2dxp200.txt
fi

if [ x"${DEVICE}" = "xV2D-XP200S" ] ; then
    MANIFEST=manifest_v2dxp200.txt
fi

if [ x"${DEVICE}" = "xV2D-XP220" ] ; then
    MANIFEST=manifest_v2dxp200.txt
fi

if [ x"${DEVICE}" = "xV2D-XP220S" ] ; then
    MANIFEST=manifest_v2dxp200.txt
fi

if [ x"${DEVICE}" = "xV2D-XP100" ] ; then
    MANIFEST=manifest_v2dxp200.txt
fi

if [ x"${DEVICE}" = "xV2D-XPi" ] ; then
    MANIFEST=manifest_xpi.txt
fi

if [ x"${DEVICE}" = "xV2D-Rx" ] ; then
    MANIFEST=manifest_v2d.txt
fi

if [ x"${DEVICE}" = "xDMS-25" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-100" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-120" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-150" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-200" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-1000" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-1200" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-1500" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-1550" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-1600" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xDMS-1700" ] ; then
    MANIFEST=manifest_dms.txt
fi

if [ x"${DEVICE}" = "xMS-25" ] ; then
    MANIFEST=manifest_ms.txt
fi

if [ x"${DEVICE}" = "xMS-150" ] ; then
    MANIFEST=manifest_ms.txt
fi

if [ x"${DEVICE}" = "xMS" ] ; then
    MANIFEST=manifest_ms.txt
fi

if [ x"${DEVICE}" = "xGeneric" ] ; then
    MANIFEST=manifest_generic.txt
fi



if [ x"$MANIFEST" = "x" ] ; then
    log_error "Could not determine Device Type."
    log_info "Provide it on the command line as the second argument."
    show_usage
    exit 1
fi

echo ${DOWNLOAD_URL}
echo $MANIFEST

# Start with a clean work area
rm -rf ${TMP_INSTALL_DIR}
mkdir -p ${TMP_INSTALL_DIR}
cd  ${TMP_INSTALL_DIR}

# Download this script and make sure we are using latest version
get_file ${DOWNLOAD_URL}/software_install.sh software_install.sh
diff $0 software_install.sh > /dev/null 2>&1
if [ "x$?" != "x0" ]; then
    log_info "Not using an uptodate version of this script (software_install.sh)"
    log_info "Attempting to start again with new version..."
    cp -f software_install.sh $0
    cleanup
    chmod +x $0
    $0 ${DOWNLOAD_URL} ${DEVICE}    
    exit 0
fi

# Download Manifest file
get_file ${DOWNLOAD_URL}/${MANIFEST} manifest.txt

# Find the name of the install script from the manifest file
installer_script=""
md5sum=""
nvpairs=`grep type=installer ./manifest.txt`
for nvpair in ${nvpairs}; do
    name=`echo $nvpair | cut -d = -f 1`
    value=`echo $nvpair | cut -d = -f 2`
    if [ $name = 'file' ] ; then
       installer_script=$value
    fi
    if [ $name = 'md5' ] ; then
       md5sum=$value
    fi
done

if [ x$installer_script = "x" ] ; then
    quit_with_error "Could not determine installer script file name"
fi

if [ x$md5sum = "x" ] ; then
    quit_with_error "No checksum set for the script file"
fi


# Download the install script
get_file ${DOWNLOAD_URL}/${installer_script} install.sh

# Download the default config file
if [ x"${DEVICE}" = "xGeneric" ] ; then
    get_file ${DOWNLOAD_URL}/DMS-1000-Device.xml Device.xml
    sed -i -e s/DMS-1000-Device/Generic-Device/ Device.xml
else
    get_file ${DOWNLOAD_URL}/${DEVICE}-Device.xml Device.xml
fi

# Verify checksum on install script
log_info "Verifying Checksum for  installer script..."
md5new=`md5sum install.sh | cut -d " " -f 1`
if [ x$md5sum != x$md5new ] ; then
    quit_with_error "Bad Checksum on install script"
fi

log_info "Checksum OK"

# Run the install script
chmod +x ./install.sh
if [ -x install.sh ] ; then
    log_info "Initiating install..."
	if [ -f /etc/config/Device.xml ] ; then
		./install.sh ${DOWNLOAD_URL} changepersonality
	else
		./install.sh ${DOWNLOAD_URL} install
	fi
fi

cleanup
