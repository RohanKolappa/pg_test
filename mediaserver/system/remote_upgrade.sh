#!/bin/sh

# Remotely upgrade a device using PuTTYs's plink

usage() {
    echo "Usage"
    echo "$0 IPADDRESS PASSPORD UPGRADEURL"
}

if [ x$1 = "x" ] ; then
    usage
    exit 1
fi

if [ x$2 = "x" ] ; then
    usage
    exit 1
fi

if [ x$3 = "x" ] ; then
    usage
    exit 1
fi

IPADDRESS=$1
PW=$2
UPGRADEURL=$3

echo n | /bin/plink -pw ${PW} admin@${IPADDRESS} /V2O/bin/upgrade_device -url ${UPGRADEURL} | grep "Initiating"
- /etc/scripts/remote_upgrade.sh 29/29 100%
