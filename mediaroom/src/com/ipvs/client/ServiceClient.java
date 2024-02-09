package com.ipvs.client;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;

import org.dom4j.Document;
import org.jivesoftware.smack.XMPPException;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPEvent;
import com.ipvs.common.XMPPEventHandlerI;
import com.ipvs.common.XMPPEventHandlerThread;
import com.ipvs.common.XMPPEventQueue;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.xmpp.client.smack.DiscoveryServiceI;
import com.ipvs.xmpp.client.smack.DiscoveryServiceImpl;


public class ServiceClient implements XMPPEventHandlerI, ServiceMessageHandlerI, XMPPF {
    public static final String SYSTEMSERVICE = "systemservice";
    public static final String MEDIASTORESERVICE = "mediastoreservice";
    public static final String MEDIAROOMSERVICE = "mediaroomservice";
    public static final String ASSETADMINSERVICE = "assetadminservice";
    public static final String ASSETADMINSERVICE_DEVICE = ASSETADMINSERVICE + "." + AMConst.Device.toLowerCase();
    public static final String ASSETADMINSERVICE_PORT = ASSETADMINSERVICE + "." + SessionServiceConst.PORT.toLowerCase();
    public static final String ASSETADMINSERVICE_FILE = ASSETADMINSERVICE + "." + SessionServiceConst.FILE.toLowerCase();
    public static final String ASSETADMINSERVICE_DIR = ASSETADMINSERVICE + "." + SessionServiceConst.DIR.toLowerCase();
    public static final String POLICYADMINSERVICE = "policyadminservice";    
    public static final String SESSIONSERVICE = "sessionservice"; 
    public static final String SESSIONSERVICE_RESOURCE = SESSIONSERVICE+".resource";
    public static final String SESSIONSERVICE_SESSION = SESSIONSERVICE+".session";
    public static final String SESSIONSERVICE_WINDOW = SESSIONSERVICE+".window";
    public static final String SESSIONSERVICE_SERVICE = SESSIONSERVICE+".service";
    public static final String RESPONSE = "Response";
    public static final String REQUEST = "Request";
    public static final String EVENT = "Event";
    public static final String NOTIFICATION = "Notification";
    public static final String IPVS_SESSION_ERROR = "ipvserror";
    public static final String IPVS_SESSION_AGENT = "com.ipvs.SessionAgent";
    public static final String IQ = "iq";
    public static final String AGENTMESSAGE = "AgentMessage";
    //public static final String AGENTEVENT = "AgentEvent";
    public static final String XMPPEVENT = "XMPPEvent";
    //public static final String AGENTMESSAGETYPE = "agentmessagetype";
    public static final String AGENT = "agent";
    //public static final String AGENTJID = "agentJID";
    //public static final String FROMAGENT = "fromagent";
    public static int DEFAULT_PORT = 5222;
    public static int INVALID_IPADDR = 502;
    String xmppImplClassName;
    ServiceMessageHandlerI messageHandler = null;
    //MRResourceFactoryI resourceFactory;
    Hashtable<String, MRClientAgent> mrClientAgentTable;

    //////////////new 
    //public static boolean bEnableLogging = false;
    private String logNIDKey = "LogRoot";
    private String logNID = "";
    private MRRequest log_request = null;
    
    public ServiceClient(String xmppImplClassName, ServiceMessageHandlerI messageHandler, MRResourceFactoryI resourceFactory) {
        this.xmppImplClassName = xmppImplClassName;
        this.messageHandler = messageHandler;
        //this.resourceFactory = resourceFactory;
        this.mrClientAgentTable = new Hashtable<String, MRClientAgent>();
    }

    public String getUserJID(String userJID) {
        if ("*".equals(userJID)) { // Use the default logged in user

            Enumeration<String> en = mrClientAgentTable.keys();

            if (en.hasMoreElements()) {
            	MRClientAgent mca = this.mrClientAgentTable.get(en.nextElement());
            	userJID =mca.getUserJID();
            }
        }

        return userJID;
    }

    // TBD: Check the code that calls this method, will be removed in next release
    public boolean hasInitilalizedClient() {
        if (mrClientAgentTable.isEmpty()) {
            return false;
        }

        String defaultUserJID = getUserJID("*");

        try {
            return this.getMRClientAgent(defaultUserJID).getInitialized();
        } catch (Exception e) {
            return false;
        }
    }

    public MRClientAgent getMRClientAgent(String userJID) throws MRException {
    	for(String clientKey : this.mrClientAgentTable.keySet()) {
    		MRClientAgent mca = mrClientAgentTable.get(clientKey);
    		if(mca.getUserJID().equals(userJID)) {
    			return mca;
    		}
    	}
    	
    	throw new MRException(MRExceptionCodes.LOGINNOTFOUND, MRXMLUtils.generateErrorXML("userJID", userJID));
    }
    
    
    private void removeAgent(String userJID) throws MRException {
    	for(String clientKey : this.mrClientAgentTable.keySet()) {
    		MRClientAgent mca = mrClientAgentTable.get(clientKey);
    		if(mca.getUserJID().equals(userJID)) {
    			mrClientAgentTable.remove(clientKey);
    			return;
    		}
    	}
    	
    	throw new MRException(MRExceptionCodes.LOGINNOTFOUND, MRXMLUtils.generateErrorXML("userJID", userJID));
    }

    public XMPPI getXMPP(String userJID) throws Exception {
        return this.getMRClientAgent(userJID).getXmpp();
    }

    XMPPI startXMPPInstance() throws Exception {
        Class<?> c = Class.forName(this.xmppImplClassName);
        XMPPI xmpp = (XMPPI) c.newInstance();
        xmpp.start();
        System.out.println("start XMPP instance");

        return xmpp;
    }

    public String handleLogin(Document xmppLoginRequestDoc,Document linkLocalLoginRequestDoc) throws Exception {

    	MRClientAgent mca = null;
    	MRException exception = null;
    	boolean isXMPPLoginSuccess = false;
    	boolean isLinkLocalLoginSuccess = false;

    	String resourceID = "";    	
    	String serverName = "";
    	String clientKey = "";
    	
    	XMPPI xmpp = null;
        

    	if(xmppLoginRequestDoc != null) {
    		try {
    			xmpp = startXMPPInstance();
    			clientKey = MRXMLUtils.getValueXpath(xmppLoginRequestDoc, "//clientKey", "");
    			resourceID = MRXMLUtils.getValueXpath(xmppLoginRequestDoc, "//resource", "");
    			serverName = MRXMLUtils.getValueXpath(xmppLoginRequestDoc, "//server", "");    			
    			handleXMPPLogin(xmppLoginRequestDoc,xmpp);
    			isXMPPLoginSuccess = true;
    		} catch(MRException exp) {
    			exception = exp;
    		} catch (Exception e) {
    			exception = new MRException(MRExceptionCodes.XMPPLOGINFAILED, MRException.getStackTrace(e));
    		}
    	}   else {
    		exception = new MRException(MRExceptionCodes.XMPPLOGINFAILED, "XMPP Login Failed");
    	}
    	
    	String userJID =  null;     
    	
    	if(isXMPPLoginSuccess) {
    		userJID = xmpp.getXMPPUserJid();
    	}
    	
    	//if xmpp or link local login success then create client and start the handler thread.
    	if(isLinkLocalLoginSuccess || isXMPPLoginSuccess) {
    		
    		if(clientKey ==null || clientKey.isEmpty()) {
    			clientKey = userJID;
    		}
    		// remove  object and stop listening for messages
   			handleRemoveMRCleintAgent(clientKey);
 
    		mca = new MRClientAgent(this, xmpp, userJID, resourceID, serverName, this);
    		mca.loglevel = MRLog.NOTIFY; // TBD : Overriding the login log level, was breaking scripts

    		mca.xmppEventHandlerThread = new XMPPEventHandlerThread(userJID, xmpp, this, mca.getLog());
    		mrClientAgentTable.put(clientKey, mca);
    		mca.xmppEventHandlerThread.startListening();
    	} else if(xmpp!=null){
    		xmpp.stop();
    	}

    	//if xmpp login fail throw exception
    	if(!isXMPPLoginSuccess && !isLinkLocalLoginSuccess) {
    		throw exception;
    	}

    	return userJID;
    }
    
    
    public void handleRemoveMRCleintAgent(String clientKey) {
    	MRClientAgent mca  =  this.mrClientAgentTable.get(clientKey);
    	if (mca==null){
    		return;
    	}
    	System.out.println("stopping xmppEventHandlerThread ...");
    	mca.xmppEventHandlerThread.stopListening();
    	System.out.println("stopping xmpp ...");    	
    	mca.getXmpp().stop();
    	mca.reset();    	
    	System.out.println("Removing MRClietAgent From Queue ...");
    	this.mrClientAgentTable.remove(clientKey);
    }
    
    private DiscoveryServiceI handleDiscoveryServiceStart(String xmppLoginJID,String clientKey,XMPPI xmpp) throws MRException,Exception {

    	DiscoveryServiceI discoveryService = null;
    	MRClientAgent client = null;
    	try {
    		client = this.mrClientAgentTable.get(clientKey);
    	} catch (Exception e) {
    	}

    	// if running, stop the discovery service.
    	if(client!=null) {
    		discoveryService = client.getDiscoveryService();
			if(discoveryService != null) {
				discoveryService.stopDiscoveryService();
			}
		}	
    	
    	XMPPEventQueue xmppEventQueue = xmpp.getXMPPEventQueue(xmpp.getUser());
    	boolean startDiscoveryService = true;
      	discoveryService = new DiscoveryServiceImpl(xmppEventQueue, startDiscoveryService);
      	discoveryService.setUserJID(xmppLoginJID);
      	discoveryService.startDiscoveryService();
    	MRLog.getInstance(MRLog.CONSOLE).addInfo("ServiceClient:handleDiscoveryServiceStart. Started Discovery Service. UserJID:="+xmppLoginJID,MRLog.INFO,MRLog.NOTIFY);
    	return discoveryService;
    	
    }

    private XMPPI handleXMPPLogin(Document xmppLoginRequestDoc,XMPPI  xmpp) throws MRException,Exception {
    	String user = MRXMLUtils.getValueXpath(xmppLoginRequestDoc, "//user", "");
    	String resourceID = MRXMLUtils.getValueXpath(xmppLoginRequestDoc, "//resource", "");
    	String password = MRXMLUtils.getValueXpath(xmppLoginRequestDoc, "//password", "");
    	String serverName = MRXMLUtils.getValueXpath(xmppLoginRequestDoc, "//server", "");
    	String portStr = MRXMLUtils.getValueXpath(xmppLoginRequestDoc, "//port", "5222");

    	isDuplicateLogin(user, serverName, resourceID);       

    	int port = Integer.parseInt(portStr);
    	
    	try {
    		xmpp.login(serverName, port, user, password, resourceID);        	           
    	} catch (XMPPException e) {
    		e.printStackTrace();
    		xmpp.stop();
    		if(e.getXMPPError() != null && e.getXMPPError().getCode() == INVALID_IPADDR) {
                throw new MRException(MRExceptionCodes.INVALID_SERVER_IPADDRESS, "Invalid Server IP Address");
            } else {
                throw new MRException(MRExceptionCodes.XMPPLOGINFAILED, "<error>" + user + ":" + serverName + ":" + String.valueOf(port) + "</error>");
            }
    	} catch (Exception e) {
    		e.printStackTrace();
    		xmpp.stop();
    		throw new MRException(MRExceptionCodes.XMPPLOGINFAILED, "<error>" + user + ":" + serverName + ":" + String.valueOf(port) + "</error>");
    	}
    	return xmpp;		
    }

    public String handleStartDiscoveryServiceRequest(String userJID,String startJmDNSRequest) { 
		Document result =null;
		Document startDiscoveryRequestDoc = null;
		int state = MRRequest.STATE_DONE;
		String logdata="";		
		try {
			MRClientAgent mca = this.getMRClientAgent(userJID);
			System.out.println("ServiceClient:handleStartDiscoveryServiceRequest");
			result =  MRXMLUtils.stringToDocument("<StartDiscoveryServiceResponseData><UserJID>" + "" + "</UserJID></StartDiscoveryServiceResponseData>");
			startDiscoveryRequestDoc = MRXMLUtils.stringToDocument(startJmDNSRequest);
			
			String clientKey = userJID;
//			String resource = ServiceAgentUtils.getAgentName(userJID);
//			String userName = StringUtils.substringBefore(userJID, "@localhost");
			
			DiscoveryServiceI discoveryService = handleDiscoveryServiceStart(userJID, clientKey, this.getXMPP(userJID));
			if(discoveryService == null) {
				throw new MRException(MRExceptionCodes.START_DISCOVERY_FAILED, "Can not start jmDNS Service");				
			}			
			mca.setDiscoveryService(discoveryService);
			MRXMLUtils.setValueXpath(result,"//UserJID", discoveryService.getUserJID());
			
		} catch (Exception e) {
            state = MRRequest.STATE_EXCEPTION;
            logdata = MRException.getStackTrace(e);
		}
		
	    return MRClientAgentUtils.createResponseXML(startDiscoveryRequestDoc, userJID, "", state, result, logdata);
	    	
	}
	
    public String handleStopDiscoveryServiceRequest(String userJID,String stopDiscoveryRequest){
		Document result =null;
		Document stopDiscoveryRequestDoc = null;
		int state = MRRequest.STATE_DONE;
		String logdata="";
		
		try {
			System.out.println("ServiceClient:handleStopDiscoveryServiceRequest");
			result =  MRXMLUtils.stringToDocument("<StopDiscoveryServiceResponseData></StopDiscoveryServiceResponseData>");
			stopDiscoveryRequestDoc = MRXMLUtils.stringToDocument(stopDiscoveryRequest);
			handleStopDiscoveryService(userJID);			
		} catch (Exception e) {		
			 state = MRRequest.STATE_EXCEPTION;
	         logdata = MRException.getStackTrace(e);
	         e.printStackTrace();
		}
		return MRClientAgentUtils.createResponseXML(stopDiscoveryRequestDoc, userJID, "", state, result, logdata);		
	}
	
	public void  handleStopDiscoveryService(String userJID) throws MRException {
		MRClientAgent client = getMRClientAgent(userJID);
		DiscoveryServiceI jmDNSSession = client.getDiscoveryService();
		client.setDiscoveryService(null);
		if(jmDNSSession!=null) {
			jmDNSSession.stopDiscoveryService();			
		} else {			
			throw new MRException(MRExceptionCodes.CAN_NOT_FIND_DISCOVERY_SERVICE, "Can not find Discovery Service, no Service running");
		}
	}
	
	public String login(String loginRequest) {
    	return login(loginRequest,null);
    }
    
    public String login(String xmppLoginRequest,String linkLocalRequest) {    	
        Document requestDoc = null;
        String userJID = "";
        Document result = null;
        int state = 1; // Should be MRReqest.STATE_DONE;
        String logdata = "";

        try {
        	result = MRXMLUtils.stringToDocument("<LoginResponseData><version>" + MRConst.Version + "</version></LoginResponseData>");
        	if(xmppLoginRequest!=null) {
        		requestDoc = MRXMLUtils.stringToDocument(xmppLoginRequest);
        	}
            Document linkLocalLoginRequestDoc =null;
            if(linkLocalRequest!=null) {
                linkLocalLoginRequestDoc = MRXMLUtils.stringToDocument(linkLocalRequest);
            }     
            userJID = this.handleLogin(requestDoc,linkLocalLoginRequestDoc);
        } catch (Exception e) {
            state = MRRequest.STATE_EXCEPTION;
            logdata = MRException.getStackTrace(e);
        }

        return MRClientAgentUtils.createResponseXML(requestDoc, userJID, "", state, result, logdata);
    }

    public String setServiceDomain(String userJID, String requestXML) {
        Document requestDoc = null;
        userJID = getUserJID(userJID);

        boolean resetDB = false;

        if ("com.ipvs.test.TestXMPPImpl".equals(this.xmppImplClassName)) {
            try{
                throw new Exception ("com.ipvs.test.TestXMPPImpl not implemented");
            }catch(Exception ex){
                ex.printStackTrace();
                return null;
            }
        }

        try {
            requestDoc = MRXMLUtils.stringToDocument(requestXML);
            MRClientAgent mca = this.getMRClientAgent(userJID);
            String requestNID = mca.handleSetServiceDomain(requestDoc, resetDB);
            return "<Request><requestNID>" + requestNID + "</requestNID></Request>";
            
        } catch (Exception e) {
        	System.out.println("Exception : "+e.toString());
        	e.printStackTrace();
        	return MRClientAgentUtils.createResponseXML(requestDoc, userJID, "", MRRequest.STATE_EXCEPTION, null,MRException.getStackTrace(e));
        }   
        
    }

    public void addServiceAgent(String userJID, ServiceAgentI serviceAgent) {
        this.addServiceAgent(userJID, serviceAgent.getClass().getName(), serviceAgent);
    }

    public void addServiceAgent(String userJID, String agentName, ServiceAgentI serviceAgent) {
        // System.out.println("Setting serviceAgent userJID=" + userJID);
        try {
            MRClientAgent mca = this.getMRClientAgent(userJID);
            mca.getServiceAgentFactory().addAgent(ServiceAgentUtils.getAgentName(agentName, userJID), serviceAgent);
        } catch (Exception e) {
            System.out.println("Adding serviceAgent=" + agentName + " as " + serviceAgent.getClass().getName());
            e.printStackTrace();
        }
    }

    public void removeServiceAgent(String userJID, String name) {
        try {
            MRClientAgent mca = this.getMRClientAgent(userJID);
            String agentName = ServiceAgentUtils.getAgentName(name, userJID);
            mca.getServiceAgentFactory().removeAgent(agentName);
        } catch (Exception e) {
            System.out.println("Removing serviceAgent=" + name);
            e.printStackTrace();
        }
    }

    public String logout(String userJID) {
    	return this.logout(userJID, null);
    }
    
    public String logout(String userJID, String message) {
    	int state = 1;
        String description = "Logout Successful";
        MRClientAgent mca = null;
        
    	try {
    		mca = this.shutdownXMPP(userJID, message);
	    } catch (Exception e) {
	        //e.printStackTrace();
	        // System.out.println("logout :" + e.toString());
	        if (mca!=null){
	        	mca.getLog().addInfo("logout :" + e.toString());
	        }
	        state = 0;
	        description = e.toString();
	    }

        return this.logout_response(userJID, message, description, state);
    }

    public MRClientAgent shutdownXMPP(String userJID, String message) throws Exception {
        // if(message != null)
        //  System.out.println(message);
        
        MRClientAgent mca = this.getMRClientAgent(userJID);
        if (mca!=null) {
        	userJID = mca.getUserJID();
        	
        	try {
				handleStopDiscoveryService(userJID);
			}catch(Exception e){
				//e.printStackTrace();
				mca.getLog().addInfo("stopping the link local service " + userJID);
			}
			
        	this.removeAgent(userJID);
			System.out.println("serviceLogout ...");
			mca.reset();
			try {
				mca.getLog().addInfo("Logout userJID=" + userJID);
				mca.getXmpp().logout();
			} catch (Exception e) {
				e.printStackTrace();
				mca.getLog().addInfo("Logout from xmpp " + userJID);
			}
			
			try{
			System.out.println("stopping xmppEventHandlerThread ...");
			mca.xmppEventHandlerThread.stopListening();
			} catch (Exception e){
				e.printStackTrace();
				mca.getLog().addInfo("stopping the xmppEventHandlerThread " + userJID);
			}
			
			try{
			System.out.println("stopping xmpp ...");
			mca.getXmpp().stop();
			System.out.println("done ...");
			}catch(Exception e){
				e.printStackTrace();
				mca.getLog().addInfo("stopping the xmpp " + userJID);
			}
        }     
        
        return mca; 
    }
    
    public String logout_response(String userJID, String message, String description, int state) {
    	String xml = null;

        try {
            Document requestDoc = MRXMLUtils.stringToDocument(message);
            Document resultDoc = MRXMLUtils.stringToDocument("<LogoutRequestData/>");
            xml = MRClientAgentUtils.createResponseXML(requestDoc, userJID, "norequestNID", state, resultDoc, description);
        } catch (Exception e) {
            //not a valid message
            xml = MRClientAgentUtils.formatResponseString("xmpp", "LogoutRequest", userJID, "", state, "", "", description,"", false);
        }

        // System.out.println(xml);
        return xml;

    }

    public String sendRequest(String userJID, String messageXML) {
        userJID = getUserJID(userJID);

        MRClientAgent mca = null;

        try {
            mca = getMRClientAgent(userJID);
            userJID = mca.getUserJID();

            try {
                //added to send log to pubsub
                logSend(mca, messageXML);
            } catch (Exception ex) {
            }           

            return mca.sendRequest(messageXML);
        } catch (Exception e) {
            String responseXML = "";
            e.printStackTrace();

            Document doc = null;

            try {
                doc = MRXMLUtils.stringToDocument(messageXML);
                responseXML = MRClientAgentUtils.createResponseXML(doc, userJID, "", MRLog.ERROR, doc, e.toString());
            } catch (Exception ex) {
                responseXML = MRClientAgentUtils.createResponseXML(doc, userJID, "", MRLog.ERROR, null, e.toString());
            }

            messageHandler.handleServiceMessage(userJID, responseXML);

            try {
                //added to send log to pubsub
                if (mca != null) {
                    logSend(mca, responseXML);
                }
            } catch (Exception ex) {
            }

            return responseXML;
        }
    }

    public static String generateResponseXML(MRRequest request, String logData) {
        Document result = request.getResult();
        int state = request.getState();

        if (state == MRRequest.STATE_EXCEPTION) {
            result = null;

            try {
                result = MRXMLUtils.stringToDocument("<Exception>" + request.getException().toString() + "</Exception>");

                StringWriter sw = new StringWriter();
                request.getException().printStackTrace(new PrintWriter(sw));
                logData = logData + "\r\n" + sw.toString();
            } catch (Exception e2) {
                logData = logData + "\r\nUnable to printStackTrace";
            }
        }

        Document requestDoc = request.getMessage().getDocument();

        return MRClientAgentUtils.createResponseXML(requestDoc, request.getUserJID(), request.getNID(), state, result, logData);
    }

    @Override
    public void handleXMPPEvent(String userJID, XMPPEvent event) {
        //System.out.println(userJID + event.hashCode() + " handleEvent start");
        try {
            MRClientAgent mca = this.getMRClientAgent(userJID);

            if (mca != null) {
                mca.handleXMPPEvent(event);
            }
        } catch (Throwable throwableObj) {
        	System.out.println("Message Recieved From userJID:="+event.getFrom()+" Message :="+event.getMessage());
        	throwableObj.printStackTrace();           
            String notification = MRClientAgentUtils.createErrorXML("Handling XMPPEvent=" + event.getMessage(), throwableObj);
            messageHandler.handleServiceMessage(userJID, notification);
        } 

        //System.out.println(userJID + event.hashCode() + " handleEvent done");
    }

    //@SuppressWarnings("unchecked")
    private void logSend(String userJID, String messageXML) {
        if (!IPVSConst.getInstance().isEnableLogging()) {
            return;
        }

        try {
            MRClientAgent mca = this.getMRClientAgent(userJID);
            logSend(mca, messageXML);
        } catch (Exception ex) {
        }
    }

    private void logSend(MRClientAgent mca, String messageXML) {
        if (!IPVSConst.getInstance().isEnableLogging()) {
            return;
        }

        try {
            if ((messageXML == null) || (mca == null)) {
                return;
            }

            if ((logNID == "") || logNID.equals("")) {
                logNID = mca.getDb().getServiceDomainRootNID(logNIDKey);
            }

            if (log_request == null) {
                // log_request = new MRRequest("LogUser", MRLog.INFO, "requestNID", "serviceName", "XMPPUserRequest", new MRMessage(MRXMLUtils.getDocument()), "");
                log_request = MRRequest.createRequest("LogUser");
            }

            //System.out.println("***** SAVE LOG setObjectData "+ logNID +" : " + messageXML );
            //messageXML = "<log><![CDATA[ " + messageXML + " ]]</log>";

            //mca.db.setObjectData(log_request, logNID, messageXML);
        } catch (Exception ex) {
        }
    }

    @SuppressWarnings("unused")
    private void logSendCached(MRClientAgent mca) {
        if (!IPVSConst.getInstance().isEnableLogging()) {
            return;
        }

        try {
            if (mca == null) {
                return;
            }

            Iterator<String> logs = mca.cachedLogs.iterator();

            while (logs.hasNext()) {
                logSend(mca, logs.next());
            }
        } catch (Exception ex) {
        	ex.printStackTrace();
        } finally {
        	if(mca!=null) {
        		mca.cachedLogs.clear();
        	}	
        }
    }

    /*@SuppressWarnings("unchecked")
    private void logAddToCache(MRClientAgent mca, String messageXML) {
        if (!IPVSConst.getInstance().isEnableLogging()) {
            return;
        }

        try {
            if ((messageXML == null) || (mca == null)) {
                return;
            }

            if (mca.cachedLogs == null) {
                mca.cachedLogs = new ArrayList();
            }

            mca.cachedLogs.add(messageXML);
        } catch (Exception ex) {
        }
    }*/

    @Override
    public void handleServiceMessage(String resultMessage, String logMessage) {
        this.messageHandler.handleServiceMessage(resultMessage, logMessage);

        // TODO Auto-generated method stub
        try {
            logSend(resultMessage, logMessage);
        } catch (Exception ex) {
        }
    }


    public boolean isDuplicateLogin(String user, String servername, String resource) throws MRException {
        /*String userJID = user + "@" + servername + "/" + resource;
        if (this.mrClientAgentTable.containsKey(userJID)) {
            throw new MRException(MRExceptionCodes.DUPLICATELOGIN,"Duplicate Login");
        }*/
        for (String jid : mrClientAgentTable.keySet()) {
            if(jid.indexOf("@") ==-1){
                if(jid.startsWith(user+".")){
                    throw new MRException(MRExceptionCodes.DUPLICATELOGIN,"Duplicate Login");
                }
            }else if(jid.startsWith(user+"@")){
                throw new MRException(MRExceptionCodes.DUPLICATELOGIN,"Duplicate Login");
            }
        }
        return false;
    }
    
    public static boolean isSessionServiceRequest(String serviceName) {
    	if(serviceName.equals(ServiceClient.SESSIONSERVICE)  || serviceName.equals(ServiceClient.SESSIONSERVICE_RESOURCE)
		|| serviceName.equals(ServiceClient.SESSIONSERVICE_SESSION) || serviceName.equals(ServiceClient.SESSIONSERVICE_WINDOW)
		|| serviceName.equals(ServiceClient.SESSIONSERVICE_SERVICE)) {
    		return true;    		
    	}
    	return false;
    	
    }

	@Override
	public XMPPI xmpp(String userJID) throws Exception {
		return this.getMRClientAgent(userJID).getXmpp();
	}

	@Override
	public ServiceAgentFactoryI sf(String userJID) throws Exception {
		return this.getMRClientAgent(userJID).getServiceAgentFactory();
	}
    
}
