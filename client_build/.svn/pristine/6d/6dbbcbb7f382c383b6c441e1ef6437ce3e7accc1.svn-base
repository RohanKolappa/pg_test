set Product=IPVS Collaboration Client.
set File=ipvs_collabclient_setup.exe
set TimeURL=http://timestamp.verisign.com/scripts/timstamp.dll
set InfoURL=http://www.ipvideosys.com

signcode -spc IPVideoPlayerCtrlPublicKey.spc -v IPVideoPlayerCtrlPrivateKey.pvk -n "%Product%" -i "%InfoURL%" -t "%TimeURL%" "%File%"

chktrust %File%

