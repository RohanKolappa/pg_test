
@ ECHO off

SET STDOPTS=-batch  -l root -pw 0at3n


SET IP=10.1.5.106
SET SCRIPT=script1.sh

pscp %STDOPTS% -q -scp %SCRIPT% %IP%:/data
plink %STDOPTS% %IP% "/bin/sh /data/%SCRIPT% > /dev/null 2>&1 &"

SET IP=10.1.5.133
SET SCRIPT=script2.sh

pscp %STDOPTS% -q -scp %SCRIPT% %IP%:/data
plink %STDOPTS% %IP% "/bin/sh /data/%SCRIPT% > /dev/null 2>&1 &"




