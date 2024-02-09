#!/bin/sh

# No longer need to use this script
exit

exit_onerror() { 
    if [ $? != 0 ]; then
        echo "$0 exiting ..." ;
        rm -rf ${ABSTMPZIPDIR}
        exit 1
    fi
}

BUILDPROXY=1
BUILDCLIENT=0
BUILDPLAYER=0

WORKDIR="/data/client_build"
ZIPFILE="${WORKDIR}/IPVSCollaboration.zip"
TMPZIPDIR="IPVSCollaboration"
ABSTMPZIPDIR="${WORKDIR}/${TMPZIPDIR}"

CLIENTSRC="/home/onair/modules/clients/public/GI"
CLIENTDST="${ABSTMPZIPDIR}/GI"
MEDIAROOMSRC="/home/onair/modules/mediaroom/public/*"
MEDIAROOMDST="${CLIENTDST}/JSXAPPS/IPVS-Common/mediaroom"
PLAYERSRC="/home/onair/modules/player/public/*"
PLAYERDST="${CLIENTDST}/JSXAPPS/IPVS-Common/player"

TEMPLATEDIR="/home/onair/templates"
LOGINREQUESTSRC="${TEMPLATEDIR}/LoginRequest.xml"
LOGINREQUESTDST="${CLIENTDST}/ProxyLoginRequestTemplate.xml"
SERVICEDOMAINREQUESTSRC="${TEMPLATEDIR}/SetServiceDomainRequest.xml"
SERVICEDOMAINREQUESTDST="${CLIENTDST}/ProxySetServiceDomainRequestTemplate.xml"
PROXYCLIENTCONFIGSRC="${TEMPLATEDIR}/ProxyClientConfig.xml"
PROXYCLIENTCONFIGDST="${CLIENTDST}/ProxyClientConfigTemplate.xml"
PROXYBATSRC="${CLIENTSRC}/IPVSProxy.bat"
PROXYBATDST="${CLIENTDST}/IPVSProxy.bat"

SETUPFILESDIR="/home/onair/client_setup/setup_files"

if [ $# -gt 0 ]
then 
    echo "$0 does not take any arguments."
    exit 1
fi

# Exit if zip already exists
if [ -e ${ZIPFILE} ]; then 
    echo "${ZIPFILE} already created."
    exit 0
fi

echo "Creating ${ZIPFILE}..."

mkdir -p ${WORKDIR}
mkdir -p ${ABSTMPZIPDIR}
mkdir -p ${CLIENTDST}
mkdir -p ${MEDIAROOMDST}
mkdir -p ${PLAYERDST}

if [ ${BUILDCLIENT} -gt 0 ] 
then
    cp -r ${CLIENTSRC}/* ${CLIENTDST}
    exit_onerror
fi

if [ ${BUILDCLIENT} -gt 0 -o ${BUILDPROXY} -gt 0 ]
then
    cp -r ${MEDIAROOMSRC} ${MEDIAROOMDST}
    exit_onerror
fi

if [ ${BUILDPLAYER} -gt 0 ] 
then
    cp -r ${PLAYERSRC} ${PLAYERDST}
    exit_onerror
fi

cp ${SETUPFILESDIR}/* ${ABSTMPZIPDIR}
exit_onerror

if [ ${BUILDPROXY} -gt 0 ] 
then
    cp ${LOGINREQUESTSRC} ${LOGINREQUESTDST}
    exit_onerror
    cp ${SERVICEDOMAINREQUESTSRC} ${SERVICEDOMAINREQUESTDST}
    exit_onerror
    cp ${PROXYCLIENTCONFIGSRC} ${PROXYCLIENTCONFIGDST}
    exit_onerror
    cp ${PROXYBATSRC} ${PROXYBATDST}
    exit_onerror
fi

cd ${WORKDIR}; zip -rq ${ZIPFILE} ${TMPZIPDIR}
exit_onerror 
rm -rf ${ABSTMPZIPDIR}

# Create link to make zip file downloadable via http
HTTPDOWNLOADDIR=/home/onair/htdocs/public/client_setup
if [ ! -L ${HTTPDOWNLOADDIR} ] ; then
   /bin/ln -s ${WORKDIR} ${HTTPDOWNLOADDIR}
fi


