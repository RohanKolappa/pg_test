echo off


echo signing jars in %1\*.jar
for /f %%a in ('dir /b/a-d %1\*.jar') do call  jarSign\signonejar   "%1\%%a"
 
 
:eof
echo "completed"