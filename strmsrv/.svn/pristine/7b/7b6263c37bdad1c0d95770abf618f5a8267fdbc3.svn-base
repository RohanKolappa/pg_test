#!/bin/sh

#
#Test MediaImporter to inject media.
#
#Author : Durga Prasad <durga.davuluri@barco.com
#
#

show_usage() {
    echo
    echo "$0 [OPTIONS]"
    echo "   -f ARG Input file"
    echo "   -i ARG Output the information"
    echo "   -c ARG Create minute files"
    echo "   -d ARG Destination directory"
    echo "   -H ARG Create HLS output"
    echo "   -t ARG Crate Thumbnail"
    echo "   -s ARG SDP File path(output)" 
    echo "   -h ARG Help"
    exit 1
}

SRC_DIR="/data/fakemedia"
DEST_DIR=""
CREATE_MIN=0
CREATE_TN="FALSE"
CREATE_HLS="FALSE"
OUTPUT_INFO=0
INPUT_FILE=""
SDPFILE_PATH=""
IMPORTABLE="TRUE"
ERROR_MSG=""
CORRUPT_FILE=""
HLS_COMPATIBLE="FALSE"

# Parse Arguments
args=`getopt icf:d:s:hHt $*`
if [ $? != 0 ] ; then
    show_usage
fi
set -- $args

for i; do
  case "$i" in
    -c  ) CREATE_MIN=1
          shift ;;
    -i  ) OUTPUT_INFO=1
          shift ;;
    -f  ) INPUT_FILE="$2"
          shift ; shift ;;
    -d  ) DEST_DIR="$2"
          shift ; shift ;;
    -H  ) CREATE_HLS="TRUE"
          shift  ;;
    -s  ) SDPFILE_PATH="$2"
          shift ; shift ;;
    -t  ) CREATE_TN="TRUE"
          shift ;;
    -h  ) show_usage
          shift  ;;
    --  ) shift ; break ;;
  esac
done

#echo "Input = ${INPUT_FILE}"
#echo "Dest = ${DEST_DIR}"
#echo "Create HLS = ${CREATE_HLS}"
#echo "Sdp = ${SDPFILE_PATH}"
#echo "Create TN = ${CREATE_TN}"

if [ ! -f ${INPUT_FILE} ] ; then
    echo "Input file ${INPUT_FILE} doesn't exist !!"
    exit 1
fi

if [ ! -d ${SRC_DIR} ] ; then
    IMPORTABLE="FALSE"
    ERROR_MSG="Source $SRC_DIR not found."
fi

CheckInput() {
    filename=$(basename "${INPUT_FILE}")
    ext="${INPUT_FILE##*.}"
    
    if [ ${ext} != "mp4" ] ; then
        ERROR_MSG="Not a valid input file."
        IMPORTABLE="FALSE"
    fi

    if [ -d ${SRC_DIR}/hls ] ; then
        HLS_COMPATIBLE="TRUE"
    fi

    if [ -f ${SRC_DIR}/corrupt ] ; then
        IMPORTABLE="FALSE"
        ERROR_MSG="Corrupted media."
    fi
}

DisplayInfo() {
    CheckInput
    echo "<?xml version=\""1.0"\" encoding=\""UTF-8"\" standalone="\"no"\" ?>"
    echo "<MediaInfo>"
    echo "    <MediaImportable>${IMPORTABLE}</MediaImportable>"
    echo "    <ErrorString>${ERROR_MSG}</ErrorString>"
    echo "    <HLSCompatible>${HLS_COMPATIBLE}</HLSCompatible>"
    echo "</MediaInfo>"
}

CheckDestination() {
    if [ x${DEST_DIR} = x ] ; then
        echo "Destination directory not set !"
        show_usage
    fi

    if [ ! -d ${DEST_DIR} ] ; then
        echo "Destination directory : $DEST_DIR doesn't exist !!"
        exit 2 
    fi
}

CopyMinuteFiles() {
    if [ ! -d ${SRC_DIR} ] ; then
        echo "Source $SRC_DIR not found."
        exit 3
    fi

    cp $SRC_DIR/* $DEST_DIR/ 2>/dev/null
}

CreateHLSFolder() {
    if [ ! -d ${DEST_DIR}/hls ] ; then
        mkdir ${DEST_DIR}/hls
    fi
}

CopyHLSFiles() {
    if [ ${CREATE_HLS} = "TRUE" ] ; then
        CreateHLSFolder
        cp $SRC_DIR/"hls"/* $DEST_DIR/hls
    fi
}

CopySDPFile() {
    if [ x${SDPFILE_PATH} = x ] ; then
        echo "SDP file path is empty."
        exit 4 
    fi

    cp ${SRC_DIR}/sdp.sdp ${SDPFILE_PATH} 
}

CopyThumbnail() {
    if [ ${CREATE_TN} = "TRUE" ] ; then
        cp -r ${SRC_DIR}/tn ${DEST_DIR}
    fi
}

CreateOutput() {
    CheckDestination
    CopyMinuteFiles
    CopyHLSFiles
    CopySDPFile
    CopyThumbnail

    sleep 5
}

if [ ${CREATE_MIN} = 1 ] ; then
    CreateOutput 
fi

if [ ${OUTPUT_INFO} = 1 ] ; then
    DisplayInfo
fi
