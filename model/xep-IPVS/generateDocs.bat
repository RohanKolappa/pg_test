call docbooktohtml.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call docbooktopdf.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call restjsondocbooktohtml.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call restjsondocbooktopdf.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call restxmldocbooktohtml.bat
if %errorlevel% neq 0 exit /b %errorlevel%
call restxmldocbooktopdf.bat
if %errorlevel% neq 0 exit /b %errorlevel%
