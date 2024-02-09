Organization of code
====================
ProxyClientApp
--------------------------
	- The Main App
	- Reads the Preferences file, Logs in, SetServiceDomain, Starts ProxyClient
	- Implements ServiceMessageHandlerI , XAuthServerAgentObserverI, ProxyClientListenerI interfaces.
	- Handles the proxy requests. 
	
ProxyClient         
--------------------------
    - Connects to the ocp server. 
    - Starts Heartbeat.
    - Continuously listens for request from the ocp server.
    - Whenever a new request comes it hands it over to the ProxyClientApp. 
      
ProxyClientHeartBeat
-------------------------------
    - Sends heartbeat to the OCP server every x(default is 1 second. This is value can be configured in Preferences file.) second.

ProxyServer
-------------------------------
    - Class which acts as an OCP Server.
    
ProxyServerHeartBeat
---------------------------
	- Class which sends heatbeat to Proxy Client.     

    
 
       
         
     
                
       
          
                
                    
                
         
        
 
