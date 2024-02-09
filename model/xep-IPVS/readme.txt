Generating the pdf and html files

1.      download docbook zip from Saurabh area http://10.51.32.8/~sjain/docbook.zip

2.      Unzip this docbook to c: /

3.      Now install the xstlproc in windows. Follow the steps in this link and setup the xstlproc. http://www.sagehill.net/docbookxsl/InstallingAProcessor.html
        (note for 62 bit download 32 bit xslproc libraries and copy to system 32 and syswow64 folder)

4.		Copy verbatim.xsl and param.xsl from /model/xep-IPVS and paste and replace into C:\docbook\xsl\fo

5.      Now open command prompt and go to the place where you stored the docbooktohtml and docbooktopdf. (In my work area this is the location F:\work\workspace\model\trunk\xep-IPVS. Go to your work area where you updated the xml)

6.      Now just run the docbooktohtml.bat and docbooktopdf.bat. It will give some warnings or errors but ignore them and check the documents it’s included your changes are not.

Related files that need to be changed

1. Bump up the service version
2. Migration if required if something in the database changes
3. /model/xsd/mediaroom.sessionservice.xsd, xsd/mediaroom.sessionservice.datamodel.xsd
4. /model/mediaroomXML/com.ipvs.sessionservice.xml/*
5. update test scripts which set values in the requestXML
6. UI/Middleware to use new xsd/xml
