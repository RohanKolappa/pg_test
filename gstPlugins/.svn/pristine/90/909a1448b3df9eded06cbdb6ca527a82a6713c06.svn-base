RED='\e[0;31m'
GREEN='\e[0;32m'
NO_COLOR='\e[0m'

rm -rf tmp_test_area
mkdir -p tmp_test_area
mkdir -p tmp_test_area/blob

bPassed="TRUE"

./mediaimporter.sh -i -f /home/durga/videos/legend.mp4 >> tmp_test_area/output.txt

echo -e "${GREEN}\n LEGEND ${NO_COLOR}\n"

importable_pattern="<MediaImportable>TRUE"
if ! grep -q "$importable_pattern" tmp_test_area/output.txt ; 
 then
    bPassed="FALSE"
fi

hls_pattern="<HLSCompatible>TRUE"
if ! grep -q "$hls_pattern" tmp_test_area/output.txt ;
 then
    bPassed="FALSE"
fi

if [ "$bPassed" == "FALSE" ]
then
     echo -e "${GREEN}[ LEGEND PASSED  ]${NO_COLOR}"
else
    echo -e "${RED}[ LEGEND FAILED  ]${NO_COLOR} "
fi

if [ "$bPassed" == "TRUE" ]
then
    ./mediaimporter.sh -c -f /home/durga/videos/legend.mp4 -d tmp_test_area/blob/ -s tmp_test_area/blob/legend.sdp -H -t -F 

    if ! md5sum tmp_test_area/blob/legend.sdp | grep "b86fde6ec4" >>/dev/null ; 
    then
        echo -e "${RED} SDP is mis-matched.${NO_COLOR}"
        bPassed="FALSE"
    fi

    if ! md5sum tmp_test_area/blob/tn/tn.jpg | grep "3cefb0ec11" >>/dev/null ;
    then
        echo -e "${RED} tn/tn.jpg is mis-matched.${NO_COLOR}"
        bPassed="FALSE"
    fi

    if ! md5sum tmp_test_area/blob/INDEX | grep "307a5df446" >>/dev/null ;
    then
        echo -e "${RED} INDEX is mis-matched.${NO_COLOR}"
        bPassed="FALSE"
    fi

    if ! md5sum tmp_test_area/blob/hls/output.m3u8 | grep "7ae85fe05d" >> /dev/null ;
    then
        echo -e "${RED} hls/output.m3u8 is mis-matched.${NO_COLOR}"
        bPassed="FALSE"
    fi

    if [ "$bPassed" = "TRUE" ] ;
    then
        echo -e "${GREEN}[ LEGEND PASSED  ]${NO_COLOR}"
    else
        echo -e "${RED}[ LEGEND FAILED  ]${NO_COLOR} "
    fi
fi

rm -rf tmp_test_area

