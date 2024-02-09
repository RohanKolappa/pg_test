CURR_DIR=`pwd`
DEV_PATH=../dev/dev_i386
GST_PATH=${DEV_PATH}/libseamer

export PATH=$PATH:${GST_PATH}/bin

export LD_LIBRARY_PATH=../rfbproto:../v2dlib:../mediaindex:../V2O/modules:../V2O/modules/drivers:../strmsrv:${DEV_PATH}/matroska/lib:${DEV_PATH}/xerces/lib:${DEV_PATH}/ffmpeg/lib:${DEV_PATH}/portaudio/lib:${DEV_PATH}/log4c/lib:${DEV_PATH}/libxkbcommon/lib:${DEV_PATH}/libgstreamer/lib

export GST_PLUGIN_PATH=${CURR_DIR}:${GST_PATH}/lib/gstreamer-1.0

