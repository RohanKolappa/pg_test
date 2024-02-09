#!/bin/sh

TMP_UPGRADE_DIR=/data/tmp_upgrade_files

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
    rm -rf ${TMP_UPGRADE_DIR}
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

# Get URL from command line
if [ x"$1" != "x" ]
then
    DOWNLOAD_URL=$1
else
    log_error "Error: Missing URL"
    log_info "Usage: software_upgrade.sh URL"
    log_info "Example:"
    log_info "upgradems.sh http://192.168.1.228/DMS/IMAGES"
    exit
fi

# Determine manifest file
if [ x"$2" != "x" ] ; then
    # Software profile forced on command line
    MANIFEST=manifest_${2}.txt
else
    # Determine the manifest from sw profile on the box
    MANIFEST=`ls /etc/config/sw_profile_*.txt | sed -e 's/.*sw_profile_\(.*\)\.txt/manifest_\1.txt/'`
fi

if [ x"$MANIFEST" = "x" ] ; then
    log_error "Could not determine software profile on current device."
    log_info "Provide it on the command line as the second argument [v2d or dms]"
    exit 1
fi


# Start with a clean work area
rm -rf ${TMP_UPGRADE_DIR}
mkdir -p ${TMP_UPGRADE_DIR}
cd  ${TMP_UPGRADE_DIR}

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
    log_info "Initiating upgrade... System will reboot shortly..."
    ./install.sh ${DOWNLOAD_URL} upgrade
fi

cleanup
