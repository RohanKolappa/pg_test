RED='\e[0;31m'
GREEN='\e[0;32m'
NO_COLOR='\e[0m'

rm -rf tmp_test_area
mkdir -p tmp_test_area
mkdir -p tmp_test_area/blob

bPassed="TRUE"

./mediaimporter.sh -i -f /home/durga/videos/aac_only.m4a >> tmp_test_area/output.txt

echo -e "${GREEN}\n AUDIO ONLY ${NO_COLOR}\n"

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

./mediaimporter.sh -c -f /home/durga/videos/aac_only.m4a -d tmp_test_area/blob/ -s tmp_test_area/blob/audio_only.sdp -H -t -F 

if ! md5sum tmp_test_area/blob/audio_only.sdp | grep "c08a91e196" >>/dev/null ; 
 then
    echo -e "${RED} SDP is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

if ! md5sum tmp_test_area/blob/INDEX | grep "8379f8c4de" >>/dev/null ;
 then
    echo -e "${RED} INDEX is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

#if ! md5sum tmp_test_area/blob/hls/output.m3u8 | grep "14c26250f6" >> /dev/null ;
# then
#    echo -e "${RED} hls/output.m3u8 is mis-matched.${NO_COLOR}"
#    bPassed="FALSE"
#fi

if [ "$bPassed" = "TRUE" ] ;
then
    echo -e "${GREEN}[ AUDIO ONLY PASSED  ]${NO_COLOR}"
else
    echo -e "${RED}[ AUDIO ONLY FAILED  ]${NO_COLOR} "
fi

rm -rf tmp_test_area

