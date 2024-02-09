#!/bin/sh 
#
#

KEYSTORE="jarSign/seckeys"
STOREPASS="ipvsroot"
KEYPASS="ipvs123"
ALIAS="IPVS"
JARSIGNER="jarsigner"
KEYTOOL="keytool"
VALIDITY="3650"
RET=1
generatekeys() {
    ${KEYTOOL} -keystore $KEYSTORE -delete  -alias IPVS -storepass $STOREPASS 
    ${KEYTOOL} -storepass $STOREPASS -keypass $KEYPASS -keystore $KEYSTORE -genkeypair -alias IPVS -validity $VALIDITY -dname "cn=IP Video Systems,ou=Engineering,o=IP Video Systems Inc"
    ${KEYTOOL} -storepass $STOREPASS -keypass $KEYPASS -keystore $KEYSTORE -list
    if [ "x$?" != "x0" ]; then
        echo "----ERROR: generatekeys failed ---"
        return 1
    fi
    return 0
}

signfiles() {
    loc=`pwd`
    echo "Sign jars in folder '${1}'....=$loc"
    filewcount=`find "$1" -print | grep "[.]jar" | wc -l`
    if [ "x$filewcount" = "x0" ]; then
        echo "ERROR: No jar files found, aborting jars signing ... "
        return 1
    fi

    FILELIST=`find "$1" -print | grep "[.]jar"`
    echo "'${filewcount}' jars found. $FILELIST --"
    for ARG in $FILELIST
    do
        echo "Processing $ARG ....."
        ${JARSIGNER} -storepass $STOREPASS -keypass $KEYPASS -keystore $KEYSTORE $ARG $ALIAS
        if [ "x$?" != "x0" ]; then
            echo "ERROR: jar signing failed for ${ARG}"
            return 1
        fi
        echo " "
    done
    return 0
}


if [ x"$1" != "xgeneratekeys" ]; then
    signfiles "$1"
    RET="$?"
else
    generatekeys 
    RET="$?"
fi
set KEYSTORE=""
set STOREPASS=""
set KEYPASS=""
set ALIAS=""

exit $RET

