RED='\e[0;31m'
GREEN='\e[0;32m'
NO_COLOR='\e[0m'

rm -rf tmp_test_area
mkdir -p tmp_test_area
mkdir -p tmp_test_area/blob

bPassed="TRUE"

./mediaimporter.sh -i -f /home/durga/videos/boeing.mp4 >> tmp_test_area/output.txt

echo -e "${GREEN}\n BOEING ${NO_COLOR}\n"

importable_pattern="<MediaImportable>TRUE"
if ! grep -q "$importable_pattern" tmp_test_area/output.txt ; 
 then
    echo "${RED}Media Import is not possible.${NO_COLOR}"
    bPassed="FALSE"
fi

hls_pattern="<HLSCompatible>TRUE"
if ! grep -q "$hls_pattern" tmp_test_area/output.txt ;
 then
    echo "${RED}HLS Is not possible${NO_COLOR}"
    bPassed="FALSE"
fi

./mediaimporter.sh -c -f /home/durga/videos/boeing.mp4 -d tmp_test_area/blob/ -s tmp_test_area/blob/boeing.sdp -H -t -F 

if ! md5sum tmp_test_area/blob/boeing.sdp | grep "4763b19750" >>/dev/null ; 
 then
    echo -e "${RED} SDP is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

if ! md5sum tmp_test_area/blob/tn/tn.jpg | grep "c67103c17c" >>/dev/null ;
 then
    echo -e "${RED} tn/tn.jpg is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

if ! md5sum tmp_test_area/blob/INDEX | grep "7c35539a74" >>/dev/null ;
 then
    echo -e "${RED} INDEX is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

if ! md5sum tmp_test_area/blob/hls/output.m3u8 | grep "ef45123d52" >> /dev/null ;
 then
    echo -e "${RED} hls/output.m3u8 is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

#if ! md5sum tmp_test_area/blob/0 | grep "ae92350655" >> /dev/null ;
# then
#    echo -e "${RED} 0 Minute file  is mis-matched.${NO_COLOR}"
#    echo -e "${RED}[  FAILED  ]${NO_COLOR} "
#    exit 7
#fi

if [ "$bPassed" = "TRUE" ] ;
then
    echo -e "${GREEN}[ BOEING PASSED  ]${NO_COLOR}"
else
    echo -e "${RED}[ BOEING FAILED  ]${NO_COLOR} "
fi

rm -rf tmp_test_area

