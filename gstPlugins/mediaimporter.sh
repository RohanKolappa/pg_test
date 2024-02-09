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

PLUGIN_DIR="/usr/lib/gstreamer-1.0"
DEST_DIR=""
CREATE_MIN=0
CREATE_TN="FALSE"
CREATE_HLS="FALSE"
FIXED_TS="FALSE"
OUTPUT_INFO=0
INPUT_FILE=""
SDP_FILE_PATH=""
IMPORTABLE="FALSE"
ERROR_MSG=""
CORRUPT_FILE=""
HLS_COMPATIBLE="FALSE"
VIDEO_ENCODING=""
AUDIO_ENCODING=""
VIDEO_SUPPORTED="FALSE"
AUDIO_SUPPORTED="FALSE"
AUDIO_PRESENT="FALSE"
VIDEO_PRESENT="FALSE"
AVMODE="AUDIOVIDEO"

# Parse Arguments
args=`getopt icf:d:s:hFHt $*`
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
    -F  ) FIXED_TS="TRUE"
          shift ;;
    -s  ) SDP_FILE_PATH="$2"
          shift ; shift ;;
    -t  ) CREATE_TN="TRUE"
          shift ;;
    -h  ) show_usage
          shift  ;;
    --  ) shift ; break ;;
  esac
done

if [ ! -f ${INPUT_FILE} ] ; then
    echo "Input file ${INPUT_FILE} doesn't exist !!"
    exit 1
fi

if [ -d ${PLUGIN_DIR} ] ; then
    if [ "${GST_PLUGIN_PATH}" = "" ] ; then
        export GST_PLUGIN_PATH="${PLUGIN_DIR}"
    fi
else
    if [ "${GST_PLUGIN_PATH}" = "" ] ; then
        echo "Did not find GST_PLUGIN_PATH !!"
    fi
fi

CheckInput() {
    filename=$(basename "${INPUT_FILE}")

    CMD="gst-discoverer-1.0 -v ${INPUT_FILE}"

    rm -rf output.txt

    $CMD >> output.txt

    OUTPUT='output.txt'
    if [ -f "$OUTPUT" ] 
    then
        result=`grep -i 'video codec: ' $OUTPUT`
        if [ "$result" = "" ] ; then
           VIDEO_ENCODING="video: NOT FOUND"
           VIDEO_PRESENT="FALSE"
        else
            VIDEO_PRESENT="TRUE"
            VIDEO_ENCODING="$result"
            if [ "$VIDEO_ENCODING" = "      video codec: H.264 / AVC" ]
            then
                VIDEO_SUPPORTED="TRUE"
            fi
        fi

        result=`grep -i 'audio codec: ' $OUTPUT`
        if [ "$result" = "" ] ; then
            AUDIO_ENCODING="audio: NOT FOUND"
            AUDIO_PRESENT="FALSE"
        else
            AUDIO_PRESENT="TRUE"
            AUDIO_ENCODING="$result"

            if [ "$AUDIO_ENCODING" = "      audio codec: MPEG-4 AAC audio" ]
            then
                channels=`grep -i '      Channels: ' $OUTPUT`
                if [ "$channels" = "      Channels: 1" ] || [ "$channels" = "      Channels: 2" ]
                then
                    AUDIO_SUPPORTED="TRUE"
                else
                    AUDIO_ENCODING=" Audio $channels Not supported."
                fi
            fi
        fi
    else
         ERROR_MSG="Not a valid input file!!"
    fi
    
    #Audio Only
    if [[ "$AUDIO_PRESENT" == "TRUE" ]] && [[ "$VIDEO_PRESENT" == "FALSE" ]]
    then
        if [[ "$AUDIO_SUPPORTED" == "TRUE" ]]
        then
            IMPORTABLE="TRUE"
            HLS_COMPATIBLE="TRUE"
            AVMODE="AUDIOONLY"
        fi
    #Video Only
    elif [[ "$VIDEO_PRESENT" == "TRUE" ]] && [[ "$AUDIO_PRESENT" == "FALSE" ]]
    then
        if [[ "$VIDEO_SUPPORTED" == "TRUE" ]]
        then
            IMPORTABLE="TRUE"
            HLS_COMPATIBLE="TRUE"
            AVMODE="VIDEOONLY"
        fi
    #Video and Audio
    elif [[ "$AUDIO_PRESENT" == "TRUE" ]] && [[ "$VIDEO_PRESENT" == "TRUE" ]]
    then
        if [[ "$AUDIO_SUPPORTED" == "TRUE" ]] && [[ "$VIDEO_SUPPORTED" == "TRUE" ]]
        then
            IMPORTABLE="TRUE"
            HLS_COMPATIBLE="TRUE"
            AVMODE="AUDIOVIDEO"
        fi
    fi
}

DisplayInfo() {
    CheckInput

    echo "<?xml version=\""1.0"\" encoding=\""UTF-8"\" standalone="\"no"\" ?>"
    echo "<MediaInfo>"
    echo "    <MediaImportable>${IMPORTABLE}</MediaImportable>"
    echo "    <ErrorString>${ERROR_MSG}</ErrorString>"
    echo "    <HLSCompatible>${HLS_COMPATIBLE}</HLSCompatible>"
    echo "    <Encoding>${VIDEO_ENCODING} / ${AUDIO_ENCODING}</Encoding>"
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

CreateMinuteFiles() {
    CMD=""
    CMD_MAIN="gst-launch-1.0 filesrc location=${INPUT_FILE} ! qtdemux name=demux"
    CMD_VIDEO=""
    CMD_AUDIO=""
    CMD_MUXER="ipvsrtpmuxer name=mux"
    CMD_WRITER="ipvsmediawriter location=${DEST_DIR} sdp-file-path=${SDP_FILE_PATH} "
    CMD_MUX_AUDIO=" mux."

    if [ "$FIXED_TS" == "TRUE" ]
    then
        CMD_MUXER=" $CMD_MUXER use-fixed-start-time=true ! "
        CMD_WRITER=" $CMD_WRITER use-fixed-start-time=true "
    else
        CMD_MUXER=" $CMD_MUXER ! "
    fi

    if [ "$CREATE_HLS" == "TRUE" ]
    then
        CMD_WRITER=" $CMD_WRITER create-hls=true "
    fi
 
    if [ "$CREATE_TN" == "TRUE" ] && [ "$AVMODE" != "AUDIOONLY" ]
    then
        CMD_WRITER=" $CMD_WRITER create-primary-thumbnail=true "
    fi

    if [ "$VIDEO_SUPPORTED" == "TRUE" ]
    then
        CMD_VIDEO="demux.video_0 ! queue ! rtph264pay pt=96 ! "
    fi

    if [ "$AUDIO_SUPPORTED" == "TRUE" ]
    then
        CMD_AUDIO="demux.audio_0 ! queue ! rtpmp4gpay pt=97 ! "
    fi

    if [ "$CMD_VIDEO" != "" ] && [ "$CMD_AUDIO" != "" ] 
    then
        CMD="${CMD_MAIN} ${CMD_VIDEO} ${CMD_MUXER} ${CMD_WRITER} ${CMD_AUDIO} ${CMD_MUX_AUDIO}"
    elif [ "$CMD_AUDIO" != "" ]
    then
        CMD="${CMD_MAIN} ${CMD_AUDIO} ${CMD_MUXER} ${CMD_WRITER}"
    else
        CMD="${CMD_MAIN} ${CMD_VIDEO} ${CMD_MUXER} ${CMD_WRITER}"
    fi
 
#    echo "$CMD"

    $CMD
}

CreateOutput() {
    CheckInput

    if [ $IMPORTABLE = "TRUE" ]
    then
        CheckDestination
        CreateMinuteFiles
    else
        DisplayInfo
    fi
}

if [ ${CREATE_MIN} = 1 ] ; then
    CreateOutput 
fi

if [ ${OUTPUT_INFO} = 1 ] ; then
    
    DisplayInfo
fi
