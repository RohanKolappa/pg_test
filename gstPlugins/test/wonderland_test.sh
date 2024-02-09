RED='\e[0;31m'
GREEN='\e[0;32m'
NO_COLOR='\e[0m'

rm -rf tmp_test_area
mkdir -p tmp_test_area
mkdir -p tmp_test_area/blob

bPassed="TRUE"

./mediaimporter.sh -i -f /home/durga/videos/wonderland.mp4 >> tmp_test_area/output.txt

echo -e "${GREEN}\n WONDERLAND ${NO_COLOR}\n"

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

./mediaimporter.sh -c -f /home/durga/videos/wonderland.mp4 -d tmp_test_area/blob/ -s tmp_test_area/blob/wonderland.sdp -H -t -F 

if ! md5sum tmp_test_area/blob/wonderland.sdp | grep "a520838517" >>/dev/null ; 
 then
    echo -e "${RED} SDP is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

if ! md5sum tmp_test_area/blob/tn/tn.jpg | grep "08bf816e37" >>/dev/null ;
 then
    echo -e "${RED} tn/tn.jpg is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

if ! md5sum tmp_test_area/blob/INDEX | grep "0928d04df5" >>/dev/null ;
 then
    echo -e "${RED} INDEX is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

if ! md5sum tmp_test_area/blob/hls/output.m3u8 | grep "c0c48e1777" >> /dev/null ;
 then
    echo -e "${RED} hls/output.m3u8 is mis-matched.${NO_COLOR}"
    bPassed="FALSE"
fi

if [ "$bPassed" = "TRUE" ] ;
then
    echo -e "${GREEN}[ WONDERLAND PASSED  ]${NO_COLOR}"
else
    echo -e "${RED}[ WONDERLAND FAILED  ]${NO_COLOR} "
fi

rm -rf tmp_test_area

