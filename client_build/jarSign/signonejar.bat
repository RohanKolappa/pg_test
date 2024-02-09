
IF "%1" == ""  goto jar_empty
IF NOT "%1" == "" goto sign

:sign
jarsigner -storepass "ipvsroot" -keypass "ipvs123" -keystore "jarSign/seckeys" %1 "IPVS"

 :end
 echo done %1
 
 :jar_empty
 