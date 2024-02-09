#!/bin/sh
JDKDIR=/home/miyer/worksvn/osgi/jdk1.7.0
BUCKMINSTERDIR=/home/miyer/worksvn/osgi/usr/local/headless-buckminster
PRODUCTFILE=/tmp/buckminster/build/com.barco.nimbus.equinox.product_1.0.0-eclipse.feature/com.barco.nimbus.equinox.product.zip
WORKSPACE=/home/miyer/workspace
OSGIDIST=osgi-dist

echo "Initializing build directories"
rm -rf ${WORKSPACE} ${PRODUCTFILE} ${OSGIDIST}
mkdir ${OSGIDIST}
cd ${OSGIDIST}

CWD=`pwd`
mkdir -p ${CWD}/usr/local
ln -s ${JDKDIR} ${CWD}
ln -s ${BUCKMINSTERDIR} ${CWD}/usr/local

echo "Checking out releng"
svn checkout http://silsvn01.barco.com/svn/svnroot/SC/trunk/com.barco.nimbus.releng

echo "Building the zip file"
JAVA_HOME=${CWD}/jdk1.7.0
export JAVA_HOME
${CWD}/usr/local/headless-buckminster/buckminster -vm ${CWD}/jdk1.7.0/bin/java -LDEBUG --scriptfile com.barco.nimbus.releng/buckminster/buckminster.nimbus.product.export.script.txt
cp ${PRODUCTFILE} ${CWD}/osgi.zip
unset JAVA_HOME

