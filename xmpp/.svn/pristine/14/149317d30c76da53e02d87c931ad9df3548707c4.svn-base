set deployroot="C:\etc\migration"

set mig_dev="C:\pravin\branch2\xmpp\IPVSServComponent\src\dbmigration\migration"
set curFolder="C:\pravin\branch2\xmpp\IPVSServComponent\src\dbmigration"




del /F /Q %deployroot%\1.0\database
del /F /Q %deployroot%\1.3\database
del /F /Q %deployroot%\1.4\database

mkdir %deployroot%\1.0\database
mkdir %deployroot%\1.0\database\xsl
mkdir %deployroot%\1.0\database\xquery
mkdir %deployroot%\1.0\database\xupdate

mkdir %deployroot%\1.3\database
mkdir %deployroot%\1.3\database\xsl
mkdir %deployroot%\1.3\database\xquery
mkdir %deployroot%\1.3\database\xupdate


mkdir %deployroot%\1.4\database
mkdir %deployroot%\1.4\database\xsl
mkdir %deployroot%\1.4\database\xquery
mkdir %deployroot%\1.4\database\xupdate

copy /Y %mig_dev%\1.0\database\*  %deployroot%\1.0\database
copy /Y %mig_dev%\1.0\database\xsl\*  %deployroot%\1.0\database\xsl
copy /Y %mig_dev%\1.0\database\xquery\*  %deployroot%\1.0\database\xquery
copy /Y %mig_dev%\1.0\database\xupdate\*  %deployroot%\1.0\database\xupdate

copy /Y %mig_dev%\1.3\database\*  %deployroot%\1.3\database
copy /Y %mig_dev%\1.3\database\xsl\*  %deployroot%\1.3\database\xsl
copy /Y %mig_dev%\1.3\database\xquery\*  %deployroot%\1.3\database\xquery
copy /Y %mig_dev%\1.3\database\xupdate\*  %deployroot%\1.3\database\xupdate
 
copy /Y %mig_dev%\1.4\database\*  %deployroot%\1.4\database
copy /Y %mig_dev%\1.4\database\xsl\*  %deployroot%\1.4\database\xsl
copy /Y %mig_dev%\1.4\database\xquery\*  %deployroot%\1.4\database\xquery
copy /Y %mig_dev%\1.4\database\xupdate\*  %deployroot%\1.4\database\xupdate
 

