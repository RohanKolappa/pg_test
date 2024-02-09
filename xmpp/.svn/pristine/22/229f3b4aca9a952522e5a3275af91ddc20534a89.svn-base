#!/bin/sh
#-DipvsAppName=sednanativedbtest  existdbtest sednaxmldbtest
#ipvsstarter_sednanativedbtest
#-DipvscomponentHome=C:/pravin3/extcomp/xmpp/IPVSServComponent/home  
#-Dipvscomponent.lib.dir=C:/pravin3/extcomp/xmpp/IPVSServComponent/out
#-testdbxml=testdb.xml or -testdbxml=testdb_readdb.xml  or -testdbxml=testdb_deletealldb.xml or -testdbxml=testdb_createdb.xml
#-numberOfThreads=1
#-numberOfLoops=100

path=`pwd`
cd "${path}/../../../"
#echo "cd ${path}/../../../"
path=`pwd`

IPVSCOMPONENT_APP_NAME=existdbtest
IPVSCOMPONENT_HOME="${path}"
IPVSCOMPONENT_LOGDIR=${IPVSCOMPONENT_HOME}/logs
IPVSCOMPONENT_LIB="${IPVSCOMPONENT_HOME}/lib"
JAR_NAME=ipvsstarter_${IPVSCOMPONENT_APP_NAME}.jar

#help
# 1-number of threads, 2-number Of Loops per threads, 3- test xml 

NUMBEROFTHREADS=$1
if [ -z ${NUMBEROFTHREADS} ] ; then
	NUMBEROFTHREADS="1"
fi

numberOfLoops=$2
if [ -z ${numberOfLoops} ] ; then
	numberOfLoops="100"
fi

TESTDBXML=$3
if [ -z ${TESTDBXML} ] ; then
	TESTDBXML="testdb.xml"
fi

ARGUMENTS="-testdbxml=${TESTDBXML} -numberOfThreads=${NUMBEROFTHREADS} -numberOfLoops=${numberOfLoops}"


IPVSCOMPONENT_OPTS="-Xmx512m"
IPVSCOMPONENT_OPTS="${IPVSCOMPONENT_OPTS} -DipvsAppName=${IPVSCOMPONENT_APP_NAME} -DipvscomponentHome=${IPVSCOMPONENT_HOME} -Dipvscomponent.lib.dir=${IPVSCOMPONENT_LIB}"

if [ -z "$LOCALCLASSPATH" ]; then
	LOCALCLASSPATH="${IPVSCOMPONENT_LIB}/${JAR_NAME}"
else
	LOCALCLASSPATH="${IPVSCOMPONENT_LIB}/${JAR_NAME} :${LOCALCLASSPATH}"
fi
 
 
mkdir -p $IPVSCOMPONENT_LOGDIR
cd $IPVSCOMPONENT_LOGDIR

# Start daemons.
printf "Starting ${IPVSCOMPONENT_APP_NAME} ..."

#CMD_EXEC="nohup /usr/bin/java -server ${IPVSCOMPONENT_OPTS} -classpath ${LOCALCLASSPATH} -jar ${IPVSCOMPONENT_LIB}/${JAR_NAME}  ${ARGUMENTS}  &"
CMD_EXEC="java -server ${IPVSCOMPONENT_OPTS} -classpath ${LOCALCLASSPATH} -jar ${IPVSCOMPONENT_LIB}/${JAR_NAME}  ${ARGUMENTS}  "
printf "${CMD_EXEC} \n   "	
eval $CMD_EXEC 
