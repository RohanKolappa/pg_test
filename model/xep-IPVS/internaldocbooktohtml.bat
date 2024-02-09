REM Internet based XSL
REM xsltproc http://docbook.sourceforge.net/release/xsl/current/html/docbook.xsl API-IPVS-Internal.xml > API-IPVS-Internal.html

REM Local File based XSL (faster)
xsltproc C:\docbook\xsl\html\docbook.xsl API-IPVS-Internal.xml > API-IPVS-Internal.html
