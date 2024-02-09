Organization of code
====================
ProxySession
--------------------------
	- Class which keeps track of a recording session.
	- Information such as the record id, state, fileNID, streamNID etc of the recording, current recording request in progress is stored.
	- Handles the transition of recording request from one state to another.
		
ProxyClientAppUtils         
--------------------------
    - Has a reference to the ProxySession.
    - Has methods that call appropriate request handlers based of the input request and send response.  
    - Converts ipvs xml responses from app server to strings with seperators.
    - send proxy response calls ProxyClientUtils to convert the strings to ocp xml responses.
      
ProxyClientUtils
---------------------------------
	- Has methods that generate response xmls from the "|" seperated sting input.
      
ProxyConstants
-------------------------------
    - Class which has constants that Proxy module uses.

ProxyException
-------------------------------
     - Exception that is thrown by the proxy Application. 
        
ProxyExceptionCodes
----------------------------
     - Class that has the constants for possible exceptions that the proxy app can throw.
     - Has a method that returns the error description given the error code.
	 	
message.properties
-----------------------------
     - Properties file that has mapping between the proxy error code and error description.

MetaDataInstance.xml 
----------------------------
     - Format of the metadata that is added in proxy recording file      

