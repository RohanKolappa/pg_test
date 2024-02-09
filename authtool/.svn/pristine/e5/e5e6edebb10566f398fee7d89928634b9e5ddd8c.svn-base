echo "Setting up Auth Station......"

mkdir "C:\IP Video Systems"

mkdir "C:\IP Video Systems\IPVSAuthTool"

xcopy IPVSAuthTool "C:\IP Video Systems\IPVSAuthTool" /s /e /k /i /y

if NOT Exist "C:\IP Video Systems\IPVSAuthTool\config\settings.xml" xcopy "C:\IP Video Systems\IPVSAuthTool\default\settings.xml" "C:\IP Video Systems\IPVSAuthTool\config\"




linkcreator.exe

regsvr32 /s "C:\IP Video Systems\IPVSAuthTool\IPVSAxAuthor.dll"