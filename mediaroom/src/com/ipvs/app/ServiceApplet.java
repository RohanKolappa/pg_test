package com.ipvs.app;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.MRClientAgentUtils;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.ApplicationContext;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.DummySourceStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.systemservice.impl.FileTransferAgent;
import com.ipvs.systemservice.impl.ServiceVersionManager;



public class ServiceApplet extends BaseApplet implements ServiceMessageHandlerI {
  
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	private String userJID = null;
	
    private void init_local() {
        // MRLog.enableSysLog=false;
        // MRLog.setLogger_category(MRLog.CONSOLE);
        ApplicationContext.setApplicationContext(ApplicationContext.APPLET_CONTEXT);
        
        //MRLog log = MRLog.getInstance(MRLog.CONSOLE); 
        
        try {
            version = ServiceVersionManager.getServiceVersion(MRLog.getInstance(MRLog.CONSOLE));
            
            MRLog.println("ServiceApplet Application Context=" + ApplicationContext.getApplicationContext());
            MRLog.println("ServiceApplet Version=" + version);
            Document doc = MRXMLUtils.stringToDocument("<Version>" + version + "</Version>");
            String message = MRXMLUtils.documentToString(doc);
            callJSMethod(this.initCallback, message);
        } catch (Exception e) {
            MRLog.println("callJSMethod failed for:" + this.callback);
            e.printStackTrace();
        }
    }

    private String login_local(String request) {
        MRLog.println("Version: " + version);
        try {
            Document doc = MRXMLUtils.stringToDocument(request);
            Element user = MRXMLUtils.getElementXpath(doc, "//user");
            if ("dummy".equals(MRXMLUtils.getValue(user))) {
                ServiceUtils.setDummyXMPPImpl(true);
                MRLog.println("setDummyXMPPImpl=true");
            } else
                MRLog.println("setDummyXMPPImpl=false");
        } catch (Exception e) {
            e.printStackTrace();
        }
        
        String loginResponse = ServiceUtils.getServiceClient(this).login(request);
        this.userJID = ServiceUtils.getUserJID(loginResponse);
        ServiceUtils.getServiceClient(this).addServiceAgent(this.userJID, new DestStreamAgent());
        ServiceUtils.getServiceClient(this).addServiceAgent(this.userJID, MediaRoomServiceConst.SOURCESTREAMAGENT, new DummySourceStreamAgent(this));       
        ServiceUtils.getServiceClient(this).addServiceAgent(this.userJID, new ConfigStreamAgent());
        ServiceUtils.getServiceClient(this).addServiceAgent(this.userJID, new FileTransferAgent());

        MRLog.println("Resetting win object");
        if(this.jsNotifier != null)
        	this.jsNotifier.resetWindow();
        return loginResponse;
    }

    private String setServiceDomain_local(String userJID, String request) {
        String response = ServiceUtils.getServiceClient(this).setServiceDomain(userJID, request);       
        return response;
    }

    private String logout_local(String userJID, String message) {
        return ServiceUtils.getServiceClient(this).logout(userJID, message);
    }
    
    private String startDiscoveryService_local(String userJID, String startDiscoveryRequest) {
        return ServiceUtils.getServiceClient(this).handleStartDiscoveryServiceRequest(userJID, startDiscoveryRequest);
    }
    
    
    private String stopDiscoveryService_local(String userJID, String stopDiscoveryRequest) {
        return ServiceUtils.getServiceClient(this).handleStopDiscoveryServiceRequest(userJID, stopDiscoveryRequest);
    }

    
    private String sendRequest_local(String userJID, String message) {
        //MRLog.println("Received Request=" + message);
    	message = MRClientAgentUtils.convertLegacyRequestsToLatetFormat(message);
    	String requestName = "";
    	try {
			Document messageDoc = MRXMLUtils.stringToDocument(message);
			requestName = MRXMLUtils.getAttributeValueXpath(messageDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
		} catch (Exception e1) { 
			e1.printStackTrace();
		}    	
    			
        if (requestName.equals("LoginRequest")){
            MRLog.println("LoginRequest");
            String response = this.login_local(message);
            this.handleServiceMessage("*", response);
            return response;
        } else if (requestName.equals("LogoutRequest")) {
            MRLog.println("LogoutRequest");
            String response = this.logout_local(userJID, message);
            this.handleServiceMessage(userJID, response);
            return response;
        } else if (requestName.equals("SetServiceDomainRequest")) {
            MRLog.println("SetServiceDomainRequest");
            String response =this.setServiceDomain_local(userJID, message);           
            try {
                Document responseDoc= MRXMLUtils.stringToDocument(response);
                if(responseDoc.getRootElement().getName().equals("Response")){
                    this.handleServiceMessage(userJID, response);
                }
            } catch (Exception e) {         
                e.printStackTrace();
            }
            
            return response;
        }  else if (requestName.equals("StartDiscoveryServiceRequest")) {
            MRLog.println("StartDiscoveryService");
            String response = this.startDiscoveryService_local(userJID, message);
            this.handleServiceMessage(userJID, response);
            return response;
        }	else if (requestName.equals("StopDiscoveryServiceRequest")){
            MRLog.println("StopDiscoveryServiceRequest");
            String response = this.stopDiscoveryService_local(userJID, message);
            this.handleServiceMessage(userJID, response);
            return response;
        }  else {
            MRLog.println("ApplicationRequest");
        }
        return ServiceUtils.getServiceClient(this).sendRequest(userJID, message);
    }

    
    public void handleServiceMessage(String userJID, String response) {
        //MRLog.println("Received <<:'" + userJID +"':  "+ response);
        try {                	
            callJSMethod(this.callback, MRClientAgentUtils.convertLatestMessagetoLegecyFormat(response));
        } catch (Exception e) {
            MRLog.println(userJID + response);
            e.printStackTrace();
        }
    }
    
    protected void killAllThreads() {    	
    	MRLog.println("ServiceApplet:KillAllThreads.userJID=" + userJID);
    	try {
    		ServiceUtils.getServiceClient(this).shutdownXMPP(this.userJID, "ServiceApplet:Killed all stale threads");
    	} catch (Exception e) {
    		e.printStackTrace();
    	}
    }

    /////////////////////////// wrap around AccessController.doPrivileged call ///////////////////
    /*
     http://jontayler.blogspot.com/2006/08/java-signed-applet-security-insanity.html 
     http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=5011139
     https://bugs.launchpad.net/ubuntu/+source/firefox-3.0/+bug/331962
     https://bugs.launchpad.net/ubuntu/+source/firefox-3.0/+bug/383067
     //large data tofrom java and js
     http://bugs.sun.com/view_bug.do;jsessionid=d7ceca7e597395643c68d1e9931b?bug_id=6763545
     */
    
    protected String handleJSCall(String methodName, String methodArg1, String methodArg2){
    	if(methodName.endsWith("init")){
    		this.init_local();
    		return "";
    	}
    	if(methodName.endsWith("login")){
    		return this.login_local(methodArg1);
    	}
    	if(methodName.endsWith("setServiceDomain")){
    		return this.setServiceDomain_local(methodArg1, methodArg2);
    	}            
    	if(methodName.endsWith("logout")){
    		return this.logout_local(methodArg1, methodArg2);
    	} 
    	if(methodName.endsWith("sendRequest")){
    		return this.sendRequest_local(methodArg1, methodArg2);
    	} 
    	if(methodName.endsWith("handleServiceMessage")){
    		this.handleServiceMessage(methodArg1, methodArg2);
    		return "";
    	}            
    	return "";
    }
    
    

}
