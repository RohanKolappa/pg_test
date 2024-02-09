package com.ipvs.test;
 
import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.app.ServiceUtils;
import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRException;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.DummyDeviceAdminServerAgent;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.ServiceAgentObserverI;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.DummySourceStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.DummyMediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.xmpp.client.IPVSSessionTest;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public class TestApp  implements ServiceMessageHandlerI, ServiceAgentObserverI {

  String lastResponse;
  static String prompt = "ready>";
  static PrintStream stdout;
  static boolean quiet = false;
  static boolean setServiceDomainDone = false;
  
  String[] ignoreMessageList = {
		  // "<ReplaceResponse>"
  };

  //boolean isApplet = true;

  public void println(String str) {
	  if(quiet && !setServiceDomainDone) {
		  if(str.indexOf("SetServiceDomain") != -1)
			  TestApp.setServiceDomainDone = true;
		  return;
	  }
	  stdout.println(str);
  }

  public synchronized void handleCmdlineMessage(String userJID, String message)
      throws Exception {

	  // Added to weed out unwanted messages that clutter the test
	  for(String s:ignoreMessageList) {
		  if(message.indexOf(s) != -1) {
			  System.out.println("Dropping Message=" + s);
			  return;
		  }
	  }
	  
    try {
      Document doc = MRXMLUtils.getDocument(message);
      String rootName = doc.getRootElement().getName();
      
      if ("Response".equals(rootName)) {
        this.setLastResponse(message); // Only if this is a response message
        // return;
      }
      
      if("SS".equals(rootName)) { // Drop SS notifications
    	return;
      }
      
      if("Notification".equals(rootName)) {
        String notificationtype = MRXMLUtils.getValueXpath(doc, "//notificationtype", "");
        if(ServiceClient.EVENT.equals(notificationtype)) {
        	return;
        }
        else if(ServiceClient.XMPPEVENT.equals(notificationtype)) {
        	//System.out.println("message :"+message);
        	Element ipvsiq = MRXMLUtils.getElementXpath(doc, "//iq");
        	Element ipvsmessage = MRXMLUtils.getElementXpath(doc, "//message");
        	if(ipvsiq != null) {
	        	Element query = MRXMLUtils.getDescendentWithoutNamespace(ipvsiq, "query");
	        	if(query == null)
	        		return;
	        	
	            String xmlns = MRXMLUtils.getElementNameSpaceURI(query, "");            
	        	if(!IPVSMessageExtension.IPVS_SESSION_XMLNS.equals(xmlns))
	        		return;
	    		String type = MRXMLUtils.getAttributeValue(ipvsiq,"type");
	    		if("set".equals(type))
	    			return;
	        	String responseXML  = SessionServiceUtils.IQToServiceResponseXml(ipvsiq);
	        	// Once we cut over to only using notifications we will need this 
	        	// String responseMessage = "<message to='" + to + "'>" + responseXML + "</message>";
	        	// String notificationXML = MRClientAgentUtils.createNotificationXML(responseMessage, ServiceClient.AGENTMESSAGE);
	        	// doc = MRXMLUtils.stringToDocument(notificationXML);
	        	doc = MRXMLUtils.stringToDocument(responseXML);
        	} else if(ipvsmessage != null) {
	        	Element extension = MRXMLUtils.getDescendentWithoutNamespace(ipvsmessage, "x");
	            String xmlns = MRXMLUtils.getElementNameSpaceURI(extension, "");     
	        	// System.out.println("TestApp:handleCmdLineMessage ipvsmessage=" + MRXMLUtils.elementToString(ipvsmessage));
	        	if(!IPVSMessageExtension.IPVS_SESSION_XMLNS.equals(xmlns))
	        		return;
	        	// System.out.println("TestApp:handleCmdLineMessage ipvsmessage=" + MRXMLUtils.elementToString(extension));
	        	
	        	//remove the name space to look for elements in the notification
	        	MRXMLUtils.removeNameSpace(extension);
        	} else{
        		//System.out.println("message :="+message);
        		return;
        	}
        }
      }
      
	  this.println(MRXMLUtils.documentToString(doc));
    }
    catch(Exception e) {
      e.printStackTrace();
      this.println("Invalid message:" + message);
    }
    this.println(prompt);
  }

  public void handleServiceMessage(String userJID, String message) {
    try {
      // validateResponse(message);
      handleCmdlineMessage(userJID, message);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }

  public String readXML(BufferedReader stdin) throws Exception {
    StringBuffer request = new StringBuffer();
    String line;
    while (true) {
      line = stdin.readLine();
      if (TestApp.prompt!=null && TestApp.prompt.equals(line.trim()))
        break;
      request.append(line + "\n");
    }
    return request.toString();
  }
  
  public void addDefaultAgents(String loginResponse) {
    ServiceUtils.getServiceClient(this).addServiceAgent(
        ServiceUtils.getUserJID(loginResponse), new DestStreamAgent());
    ServiceUtils.getServiceClient(this).addServiceAgent(
        ServiceUtils.getUserJID(loginResponse), MediaRoomServiceConst.SOURCESTREAMAGENT, new DummySourceStreamAgent(this));
       
    MediaStoreAdminServerAgent mediaStoreAdminAgent = new DummyMediaStoreAdminServerAgent();
    ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse),MediaStoreAdminServerAgent.class.getName(), mediaStoreAdminAgent);

    ConfigStreamAgent configStreamAgent = new ConfigStreamAgent();
    ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse), configStreamAgent);
    
    DummyDeviceAdminServerAgent deviceAdminServerAgent = new DummyDeviceAdminServerAgent();
    ServiceUtils.getServiceClient(this).addServiceAgent(ServiceUtils.getUserJID(loginResponse),DeviceAdminServerAgent.class.getName(), deviceAdminServerAgent);
   
  }
  
  /*private void validateResponse(String xml)throws Exception{
    Document doc = MRXMLUtils.stringToDocument(xml);
    if(!"Response".equals(doc.getRootElement().getName()))
      return;
      
    String serviceName = (MRXMLUtils.getElementXpath(doc, "//servicename")).getValue();
    if(serviceName.equals("xmpp"))
      return;
    // if(serviceName.equals("assetadminservice"))
    //  return;
    Integer state  = Integer.parseInt((MRXMLUtils.getElementXpath(doc, "//header/state")).getValue());
    if(state.intValue() == 0)
      return;
    String requestName = (MRXMLUtils.getElementXpath(doc, "//requestname")).getValue();
    String responseDataName = requestName.replace("Request", "Response") + "Data";
    String responseDataXML = MRXMLUtils.elementToString(
        MRXMLUtils.getElementXpath(doc, "//" + responseDataName ));
    
    String xsdPath = "/com/ipvs/xsd/mediaroom." + serviceName + ".xsd";
    URL url = TestApp.class.getResource(xsdPath);
    try {
      MRXMLUtils.getDocument(responseDataXML, url.toString());
      System.out.println (responseDataName + ".... IS VALID");
    }
    catch(Exception e) {
      System.out.println (responseDataName + ".... IS NOT VALID\n" + e.toString());
      System.out.println(xml);
      // e.printStackTrace();
    }
  }*/
  
  public void validateMediaRoom(Document rdoc) throws Exception {
    Element mediaRoom = MRXMLUtils.getElementXpath(rdoc, "//MediaRoom");
    if(mediaRoom != null) {
      System.out.println("Checking MediaRoom XML");
  
      String xsdURL = "com/ipvs/xsd/mediaroom.mediaroomservice.datamodel.xsd";
      String xml = MRXMLUtils.elementToString(mediaRoom);
      StringTokenizer st = new StringTokenizer(xml, "\n");
      int ii = 1;
      while(st.hasMoreTokens()) {
        System.out.println(String.valueOf(ii) + ":" + st.nextToken());
        ii++;
      }
      MRXMLUtils.getDocument(xml, xsdURL);
    }    
  }

  public void handleRequestFiles(String[] args) throws Exception {
    if(stdout == null)
      stdout = System.out;
    // ServiceUtils.setDummyXMPPImpl(true);
    String loginRequest = FileUtils.readFile(new File(args[0]));

    String loginResponse = ServiceUtils.getServiceClient(this).login(loginRequest);
    // Set the StreamAgent
    addDefaultAgents(loginResponse);
    System.out.println(loginResponse);
    setLastResponse(loginResponse);
    
    String setServiceDomainRequest = FileUtils.readFile(new File(args[1]));

    ServiceUtils.getServiceClient(this).setServiceDomain("*", 
          setServiceDomainRequest);
   /* System.out.println(setServiceDomainResponse);
    setLastResponse(setServiceDomainResponse);
   */ 
    for (int i = 2; i < args.length; i++) {
      if (args[i].startsWith("#")) {
        if (args[i].startsWith("#sleep=")) {
          System.out.println("Started sleeping");
          int st = Integer.parseInt(args[i].substring("#sleep=".length()));
          Thread.sleep(st * 1000);
          System.out.println("Done sleeping");
        }
        continue;
      }
      // Ensure that the prev request has finished
      String response = null;
      while (true) {
        response = this.getLastResponse();
        if (response != null)
          break;
        Thread.sleep(1000);
      }
      this.setLastResponse(null);

      // System.out.println("Last Response:" + response);
      String request = FileUtils.readFile(new File(args[i]));
      // Move in values from lastResponse into new Request
      // find the element in the request which have "$"
      Document doc = MRXMLUtils.stringToDocument(request);
      Document rdoc = MRXMLUtils.stringToDocument(response);
      
      String requestname = MRXMLUtils.getValueXpath(doc, "//requestname","");
      Element rqdata = MRXMLUtils.getElementXpath(doc, "//" + requestname + "Data");
      
      
      // validateMediaRoom(rdoc);
      if(rqdata != null) {
        Iterator<?> children = rqdata.elements().iterator();
        while (children.hasNext()) {
          Element child = (Element) children.next();
          // find the corresponding elements in the last response and set the
          // values
          if ("$".equals(MRXMLUtils.getValue(child))) {
            String name = child.getName();
            // System.out.println("Found $:" + name);
            try {
              Element el = MRXMLUtils.getElementXpath(rdoc, "//" + name);
              MRXMLUtils.setValue(child, MRXMLUtils.getValue(el));
            } catch (Exception ee) {
               ee.printStackTrace();
            }
          }
        }
      }
      String requestXML = MRXMLUtils.documentToString(doc);
      // System.out.println("RequestXML :" + requestXML);
      String requestNIDXML = ServiceUtils.getServiceClient(this).sendRequest("*", requestXML);
      System.out.println("RequestNID:" + requestNIDXML);
    }
    // Get the last response
    BufferedReader stdin = new BufferedReader(new InputStreamReader(System.in));
    System.out.println("hit return to logout");
    stdin.readLine();
    String userJID = ServiceUtils.getUserJID(loginResponse);
    String result = ServiceUtils.getServiceClient(this).logout(userJID);
    System.out.println(result);
    System.out.println("hit return to exit");
    stdin.readLine();
    System.out.println("Done");
  }
  
  /*
  <iq from='roomuser-default@localhost/userpc' 
	     id='zzz-123' 
	     to='managementserver@localhost/managementserver' 
	     type='set/get'> 
	     <query xmlns='com.ipvs.session'> 
	     	<!-- Request data goes here --> 
	     </query> 
	   </iq>

  */
  
  public static void main(String[] args) {
    TestApp app = new TestApp();
    boolean doSocket= false;
    int port = 5678;

	System.out.println("<?xml version='1.0' encoding='UTF-8'?><appletlog>\n");

    // grab command line arguments
    if (args.length > 0 && args[0].equals("socket")) {
      doSocket = true;
      if (args.length > 1 ) {
        try{
          port = Integer.parseInt(args[1]);
        } catch(Exception ex){
          port = 5678;
        }
      }     
    }

    if (!doSocket && args.length > 1) {
      try {
        app.handleRequestFiles(args);
      } catch (Exception e) {
        e.printStackTrace();
        System.out.println("<Error>" + e.toString() + "</Error>");
      }
      System.exit(1);
    }
    
    for(int j = 0; j < args.length; j++) {
    	if(args[j].equals("quiet")) {
    		System.out.println("Setting quiet mode");
    		TestApp.quiet = true;
    	}
    }

    for(int j = 0; j < args.length; j++) {
    	if(args[j].equals("dbdebug")) {
    		XMPPTransaction.setDEBUG(true);
    		System.out.println("<?xml version='1.0' encoding='UTF-8'?><appletlog>\n");
    	}
    }

    for(int j = 0; j < args.length; j++) {
    	if(args[j].equals("appserver")) {
    		AppServerAgent.setTestAppServerAgentJID(args[j+1]);
    		System.out.println("Setting appserverJID=" + args[j+1] + "\n");
    	}
    }

    BufferedReader stdin = null;
    ServerSocket serverSocket = null;
    Socket clientSocket = null;
    InputStream  is = null;
    OutputStream  os = null;
    if (doSocket) {
      try {
        // Create server socket
        serverSocket = new ServerSocket(port, 5);
        System.out.println("Running TCP server interface on port " + port + "...");
        MRClientAgent.setLegacyResponse(true);
        // Wait for first connection
        clientSocket = serverSocket.accept();

        // Setup the input output streams
        is = clientSocket.getInputStream();
        os = clientSocket.getOutputStream();
        stdin = new BufferedReader(new InputStreamReader(is));
        stdout = new PrintStream(os);
      }
      catch (IOException ioe) {
        System.out.println("Error creating socket: " + ioe);
        System.exit(1);
      } 
    }
    else {
      stdin = new BufferedReader(new InputStreamReader(System.in));
      stdout = System.out;
    }
    stdout.println(prompt);

    while (true) {
      try {
        //added by pravin to see if any activity going on or not
        String line = stdin.readLine();
        //System.out.println();
        if (line == null) {
          // eof on input (e.g. socket closed)
          clientSocket.close();
          break;
        }
        synchronized(app) {  
          if ("quit".equals(line.trim()))
            break;
          else if (line.startsWith("prompt")) {
            StringTokenizer st = new StringTokenizer(line);
            st.nextToken();
            prompt = st.nextToken();
          } else if (line.startsWith("database")) {
            StringTokenizer st = new StringTokenizer(line);
            st.nextToken();
            String database = st.nextToken();
            if (database.trim().equals("dummy")) {
              ServiceUtils.setDummyXMPPImpl(true);
            }
            stdout.println(" ");
          } else if (line.startsWith("setServiceDomain")) {
            StringTokenizer st = new StringTokenizer(line);
            if (st.countTokens() < 2) {
              stdout.println("Usage: setServiceDomain <userJID>\n<SetServiceDomainRequest>");
              continue;
            }
            st.nextToken();
            String userJID = st.nextToken();
            String request = app.readXML(stdin);
            // System.out.println("sendRequest " + request);
            String requestNID = ServiceUtils.getServiceClient(app).setServiceDomain(userJID, request);
            stdout.println(requestNID);
          } else if (line.startsWith("StartDiscoveryService")) {
              StringTokenizer st = new StringTokenizer(line);
              if (st.countTokens() < 2) {
                stdout.println("Usage: StartDiscoveryService <userJID>\n<StartDiscoveryServiceRequest>");
                continue;
              }
              st.nextToken();
              String userJID = st.nextToken();
              String request = app.readXML(stdin);
              // System.out.println("sendRequest " + request);
              String requestNID = ServiceUtils.getServiceClient(app).handleStartDiscoveryServiceRequest(userJID, request);
              stdout.println(requestNID);
            } else if (line.startsWith("StopDiscoveryService")) {
                StringTokenizer st = new StringTokenizer(line);
                if (st.countTokens() < 2) {
                  stdout.println("Usage: StopDiscoveryService <userJID>\n<StopDiscoveryServiceRequest>");
                  continue;
                }
                st.nextToken();
                String userJID = st.nextToken();
                String request = app.readXML(stdin);
                // System.out.println("sendRequest " + request);
                String requestNID = ServiceUtils.getServiceClient(app).handleStopDiscoveryServiceRequest(userJID, request);
                stdout.println(requestNID);
              }else if (line.startsWith("loginFile")) {
            StringTokenizer st = new StringTokenizer(line);
            String request = null;
            if (st.countTokens() < 2) {
              stdout.println("Usage: loginFile <loginRequestXMLFile>");
              continue;
            }
            st.nextToken();
            request = FileUtils.readFile(new File(st.nextToken()));
            if (request.trim().equals("")) {
              stdout.println("Usage: login\n<loginRequest>");
              continue;
            }
            String status = ServiceUtils.getServiceClient(app).login(request);

            // Set the StreamAgent
            app.addDefaultAgents(status);          
            stdout.println(status);
          } else if (line.startsWith("login")) {
            String request = app.readXML(stdin);

            if (request.trim().equals("")) {
              stdout.println("Usage: login\n<loginRequest>");
              continue;
            }
            String status = ServiceUtils.getServiceClient(app).login(request);
            // Set the StreamAgent
            app.addDefaultAgents(status);          
            stdout.println(status);
          } else if (line.startsWith("logout")) {
            StringTokenizer st = new StringTokenizer(line);
            if (st.countTokens() < 2) {
              stdout.println("Usage: logout <userJID>");
              continue;
            }
            st.nextToken();
            String userJID = st.nextToken();
            String response = ServiceUtils.getServiceClient(app).logout(userJID);
            stdout.println(response);
          } else if (line.startsWith("testsmack")) {
            StringTokenizer st = new StringTokenizer(line);
            if (st.countTokens() < 2) {
              stdout.println("Usage: testsmack <testfunction> <args> ...");
              continue;
            }
            st.nextToken();
            String server="192.168.1.209";
            port = 5222;
            String userJID="si_1";
            String password= "siyer";
            String resource="v2dtx";
            IPVSSessionTest xmpp = new IPVSSessionTest();
            try {
              xmpp.login (server, port, userJID, password, resource);
            } catch (Exception e) {
              e.printStackTrace();
            }
            String response = xmpp.testfunction(st);

            stdout.println(response);
            xmpp.logout();
          } else if (line.startsWith("sendRequestFile")) {
            StringTokenizer st = new StringTokenizer(line);
            if (st.countTokens() < 3) {
              stdout
              .println("Usage: sendRequestFile <userJID>\n<requestFile>");
              continue;
            }
            st.nextToken();
            String userJID = st.nextToken();
            String request = FileUtils.readFile(new File(st.nextToken()));
            // System.out.println("sendRequest " + request);
            String requestNID = ServiceUtils.getServiceClient(app).sendRequest(userJID, request);
            stdout.println(requestNID);
          } else if (line.startsWith("sendRequest")) {
            StringTokenizer st = new StringTokenizer(line);
            if (st.countTokens() < 2) {
              stdout.println("Usage: sendRequest <userJID>\n<request>");
              continue;
            }
            st.nextToken();
            String userJID = st.nextToken();
            String request = app.readXML(stdin);
            //System.out.println("sendRequest Length=" + request.length());
            
            // Check if this is a sessionservice request, in which case convert request to IQ
            Document doc = MRXMLUtils.stringToDocument(request);
            String servicename = MRXMLUtils.getAttributeValueXpath(doc,"//Header", "serviceName", "");
            if(doc.getRootElement().getName().equals("Request") && ServiceClient.isSessionServiceRequest(servicename)) { // Convert the sessionservice to an iq packet
            	try {
	            	String iqXml = SessionServiceUtils.sessionServiceRequestToIQXml(doc, userJID,  AppServerAgentUtils.getAppServerAgentJID(userJID, ServiceUtils.getServiceClient(app).getMRClientAgent(userJID).getServiceAgentFactory()));
	            	// System.out.println(iqXml);
	            	request = iqXml;
            	}
            	catch(Exception e) {
            		e.printStackTrace();
            	}
            }
            String requestNID = ServiceUtils.getServiceClient(app).sendRequest(userJID, request);
            //System.out.println("sendRequest " + requestNID);
            stdout.println(requestNID);
          } else if (line.startsWith("getServices")) {
            stdout
            .println("mediaroomservice\nassetadminservice\npolicyadminservice");
          } else if (line.startsWith("getRequests")) {
            StringTokenizer st = new StringTokenizer(line);
            if (st.countTokens() < 2) {
              stdout.println("Usage: getRequests <serviceName>");
              continue;
            }
            st.nextToken();
            String serviceName = st.nextToken();
            String[] requests = ServiceUtils.getRequestList(serviceName);
            for (int i = 0; i < requests.length; i++)
              stdout.println(requests[i]);
          } else if (line.startsWith("getRequestXML")) {
            StringTokenizer st = new StringTokenizer(line);
            if (st.countTokens() < 3) {
              stdout
              .println("Usage: getRequestXML <serviceName> <requestName>");
              continue;
            }
            st.nextToken();
            String serviceName = st.nextToken();
            String requestName = st.nextToken();
            stdout.println(ServiceUtils.getRequestXML(serviceName, requestName));
          } else {
            stdout.println("CommandList:\n" + "login\n" + "LoginFile\n"
                + "logout\n" + "sendRequest\n" + "getServices\n"
                + "getRequests\n" + "getRequestXML\n" + "quit");
          }
          // print the next prompt before releasing lock
          stdout.println(prompt);
        }
      }
      catch(java.net.SocketException e) {
        e.printStackTrace();
        System.out.println("Exiting");
        break;
      } catch (Exception e) {
        stdout.println("<Error>" + e.toString() + "</Error>");
        break;
      }
    }
    if (doSocket) {
      try {
        clientSocket.close();
        serverSocket.close();
      }      
      catch(Exception e) {
        
      }
    }
    System.out.println("Done");
    System.exit(0);
  }

  public synchronized String getLastResponse() {
    return lastResponse;
  }

  public synchronized void setLastResponse(String lastResponse) {
    this.lastResponse = lastResponse;
  }

  public void handleMessage(String message)  throws MRException {
    System.out.println("TestApp::handleMessage message=" + message);
    // throw new MRException(MRExceptionCodes.XAUTH_FAILED, "NO");
  }
}
