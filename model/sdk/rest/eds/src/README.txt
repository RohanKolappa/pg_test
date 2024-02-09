Following are the steps to setup up and run automated rest api diagnostic tool (Swagger).

1. Swagger is available on MS at location /home/onair/htdocs/public. It can be accessed using URL http://<MS_IP>/rest/src/

Steps 2-4 discribe how to deploy swagger on any other webserver

2. Deploy swagger on webserver (for example : in case of Tomcat, just copy the rest direcoty in webapps directory of Tomcat )

3. Start tomcat server. (double click on startup window batch file present in bin directory of tomcat).

4. Hit http://<webserver_IP>:8080/rest/src/ on any browser (webserver_IP is ip adderess of machine where webserver is deployed.). list of APIs will be seen in UI.


5. First login by providing resourcename (default value is web-client),serverIp, ursername  and password in corresponding boxes and click on "login" button on UI. serverIp is your MS IP.

6. Access any api by providing required field values, In POST request, to enter body just click on Model schema and change the field values.Finally click on "Try it out!" button to send request.

7. If you are using IE then you might get prompt saying "This page is accessing information that is not under its control. This poses a security risk." on every click . To get rid of this Go to Tools ->Internet Options->Security tab->click Custom Level , locate the Access data sources across domains under the Miscellaneous section and change the setting from Prompt to enable.
