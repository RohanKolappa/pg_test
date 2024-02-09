package com.ipvs.proxy.app;

import java.io.File;
import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.app.ServiceUtils;
import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.XAuthServerAgent;
import com.ipvs.common.service.XAuthServerAgentObserverI;
import com.ipvs.config.IPVSConst;
import com.ipvs.device.impl.DeviceAppUtils;
import com.ipvs.mediaroomservice.impl.ConfigStreamAgent;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.proxy.impl.ProxyClientAppUtils;
import com.ipvs.proxy.impl.ProxyConstants;
import com.ipvs.utils.FileUtils;
import com.ipvs.xmpp.client.DBUpdateEvent;

public class ProxyClientApp implements ServiceMessageHandlerI , XAuthServerAgentObserverI, ProxyClientListenerI{


	
	public static final String LOGIN_REQUEST_XML_PATH = "/com/ipvs/test/xml/LoginRequest.xml";
	public static final String SETSERVICEDOMAIN_REQUEST_XML_PATH = "/com/ipvs/systemservice/xml/SetServiceDomainRequest.xml";
	String preferencesFile;
	Document preferencesDoc;
	protected File loginFile;
	protected File domainFile;
	protected String loginStatus = null; 
	protected MRDB db; 
	protected XMPPI xmpp; 
	protected MRResourceFactoryI resourceFactory; 
	protected ServiceAgentFactoryI serviceAgentFactory; 
	protected MRLog log = null; 
	protected String userJID = null; 
	protected XAuthServerAgent xAuthAgent;
	//private String proxyConfigFile = null; 
	private ProxyClient proxyClient = null;
	
	private boolean isServiceDomainSet;
	
	/*
	 * Dummy check in test
	 */

	public boolean setServiceDomain() {
		try { 
			Document setServiceDomainRequestDoc = MRXMLUtils.loadXMLResource(SETSERVICEDOMAIN_REQUEST_XML_PATH, log);
			
			if(setServiceDomainRequestDoc!= null) {
				String domain = MRXMLUtils.getValueXpath(preferencesDoc, "//servicedomain", "default");
				MRXMLUtils.setValueXpath(setServiceDomainRequestDoc, "//domain", domain);
				String resource = MRXMLUtils.getValueXpath(preferencesDoc, "//Login/resource", "");
				MRXMLUtils.setValueXpath(setServiceDomainRequestDoc, "//AssetTemplateTitle", resource);
			}	
			
			DeviceAppUtils.setServiceDomain(MRXMLUtils.documentToString(setServiceDomainRequestDoc), ServiceUtils.getServiceClient(this),log);
			return true;
		} catch (Exception e) { 
			e.printStackTrace(); 
		}
		return true;
	}	
	
	@SuppressWarnings("unchecked")
	public boolean login() { 

		String loginRequest = null; 
		try { 
			//loginRequest = FileUtils.readFile(loginFile);
			Document loginRequestDoc = MRXMLUtils.loadXMLResource(LOGIN_REQUEST_XML_PATH, log);
			if(loginRequestDoc != null) {
				
				List<Element>loginRequestElements = MRXMLUtils.getElementXpath(loginRequestDoc, "//LoginRequestData").elements();
				for(int i = 0;i<loginRequestElements.size();i++) {
					String loginRequestElementName = loginRequestElements.get(i).getName();
					String loginRequestElementValue = MRXMLUtils.getValueXpath(preferencesDoc, "//Login/" + loginRequestElementName, "");
					MRXMLUtils.setValueXpath(loginRequestDoc, "//" + loginRequestElementName, loginRequestElementValue);
				}
			}
			loginRequest = MRXMLUtils.documentToString(loginRequestDoc);
			String loginResponse = ServiceUtils.getServiceClient(this).login(loginRequest);
			//System.out.println(loginResponse);
			if(loginResponse!=null) {
				int state = DeviceAppUtils.getResponseState(loginResponse);
				userJID = ServiceUtils.getUserJID(loginResponse);
				//if(!"".equalsIgnoreCase(userJID)) {
				if(state != 0) {
					return true;
				}
				else {
					return false;
				}
			}	
		} catch (Exception e) { 
			log.addInfo("Caught Exception in processSession" + e.getMessage(), MRLog.INFO,MRLog.NOTIFY ); 
		} 
		return false;

	} 

	public void logout() {
		ServiceClient client = getServiceClient(); 
		client.logout(userJID); 
	}
	
	
	public ServiceClient getServiceClient() { 
		return ServiceUtils.getServiceClient(this); 
	} 

	public static void main(String[] args) { 
		ProxyClientApp proxyApp = new ProxyClientApp(); 
		proxyApp.processCmdLine(args); 

	} 

	ProxyClientApp () { 
		log = MRLog.getInstance("CONSOLE", true); 
		log.setLogLevel(MRLog.INFO); 
		MRClientAgent.setLegacyResponse(false); 
	} 

	public void processCmdLine(String[] args) { 
		boolean bStartAgent = true; 
		boolean bUsage = false; 
		if (args.length < 2) { 
			bUsage = true; 
		} 
		if (!bUsage && args[0].equals("preferences")) {
			File file = new File(args[1]);
			if(!file.exists()) {
				System.out.println("Unable to locate file " + args[1]);
				bStartAgent = false;
			}	
			else {
				try {
					preferencesDoc = MRXMLUtils.stringToDocument(FileUtils.readFile(args[1]));
					bStartAgent = true;
				}catch(Exception e) {
					System.out.println("Unable to read file " + args[1]);
					bStartAgent = false;
					e.printStackTrace();
				}
			}
		}	
		else {
			bUsage = true;
		}
		
		if (bUsage) { 
			System.out.println("Usage"); 
			System.out.println("preferences <preferencesfilename> "); 
			//System.exit(); //Findbugs 
			return; 
		} 
		
		if (bStartAgent) { 
			createProxyClient();
		} 
	} 

	
	public void createProxyClient() {
		String serverip = this.getProxyHostName();
		int port = this.getProxyPort();
		int heartbeatFrequency = this.getHeartBeatFrequency();
		proxyClient = new ProxyClient(this, serverip, port, heartbeatFrequency, log);
		proxyClient.run();
	}
	
	public void addDefaultAgents() { 
		ConfigStreamAgent  configStreamAgent = new ConfigStreamAgent();
		ServiceUtils.getServiceClient(this).addServiceAgent(userJID, ConfigStreamAgent.class.getName(), configStreamAgent);
		xAuthAgent = new XAuthServerAgent(); 
		xAuthAgent.addAgentObserver("ProxyClientApp", this);
		ServiceUtils.getServiceClient(this).addServiceAgent(userJID, MRServiceConst.XAUTHSERVERAGENT, xAuthAgent);
		

	} 

	
	public void checkSetServiceDomainResponse(String message) {
		if(ServiceAgentUtils.isSetServiceDomainFailed(message, log)) {
			this.setSetServiceDomainSuccessful(false);
		} else {
			this.setSetServiceDomainSuccessful(true);
		}
		synchronized (this) {
			isServiceDomainSet = true;
			notify();
		}	
	}
	public void handleServiceMessage(String userJID, String message) { 
		Document notifyMessageDoc = null;
		//System.out.println("handleServiceMessage " + Thread.currentThread().getName());
		//System.out.println(message);
		try { 	
			checkSetServiceDomainResponse (message);
			if (ServiceAgentUtils.isShutdownEvent(message)) { 
				//this.setMsDisconnected(true);
				proxyClient.setRestart(true);
				return; 
			} 
			notifyMessageDoc = MRXMLUtils.stringToDocument(message); 

				String agentMessage = MRXMLUtils.getValueXpath(notifyMessageDoc, "//notificationtype", ""); 

				if ((agentMessage == null) || (agentMessage.length() == 0)) { 
					return; 
				} 

				MRDB schema = ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getDb(); 
				XMPPI xmpp = ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getXmpp(); 
				ServiceAgentFactoryI serviceAgentFactory = ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getServiceAgentFactory(); 
				MRResourceFactoryI resourceFactory = ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getResourceFactory(); 

				switch (ProxyClientAppUtils.MessageType.valueOf(agentMessage)) { 
				case AgentMessage: 

					String action = MRXMLUtils.getValueXpath(notifyMessageDoc, "//action", ""); 
					String eventName = MRXMLUtils.getAttributeValueXpath(notifyMessageDoc,"//Event/header", "eventname","");
					String agentName = MRXMLUtils.getValueXpath(notifyMessageDoc, "//agent", ""); 
					String agentmessagetype = MRXMLUtils.getAttributeValueXpath(notifyMessageDoc, "//AgentMessage","type", "");
					if(DestStreamAgent.STREAMSTATUSEVENT.equalsIgnoreCase(eventName) && MediaRoomServiceConst.DESTINATIONEVENT.equalsIgnoreCase(action)) {
						ProxyClientAppUtils.validateAction(notifyMessageDoc, DestStreamAgent.STREAMSTATUSEVENT, null, schema, log, this.userJID, xmpp, resourceFactory, serviceAgentFactory);
					}
					if (agentName.equals(MRServiceConst.DEVICEADMINCLIENTAGENT) && agentmessagetype.equals(ServiceClient.RESPONSE)) { 
						ProxyClientAppUtils.processResponse(notifyMessageDoc, proxyClient, schema, log, this.userJID, xmpp, resourceFactory, serviceAgentFactory);
						return; 
					} 

					if (agentName.equals(MRServiceConst.MRCLIENTAGENT) && agentmessagetype.equals(ServiceClient.RESPONSE)) { 
						ProxyClientAppUtils.processResponse(notifyMessageDoc, proxyClient, schema, log, this.userJID, xmpp, resourceFactory, serviceAgentFactory);
						return; 
					} 

					/*if (action.trim().equals(MediaRoomServiceConst.STARTREQUEST)) { 

					}*/ else if (action.trim().equals(MediaRoomServiceConst.START)) {
						//System.out.println(MRXMLUtils.documentToString(notifyMessageDoc));
						ProxyClientAppUtils.handleStreamStart(notifyMessageDoc, MediaRoomServiceConst.STARTREQUEST, null, schema, proxyClient, log,   this.userJID,  xAuthAgent, xmpp, resourceFactory, serviceAgentFactory);
						ProxyClientAppUtils.validateAction(notifyMessageDoc, ProxyConstants.STREAM_STARTED, null, schema, log,  this.userJID, xmpp, resourceFactory, serviceAgentFactory); 
					} else if (action.trim().equals(MediaRoomServiceConst.SOURCE_STARTOUTPUT) && agentmessagetype.equals(ServiceClient.RESPONSE)) { 
						ProxyClientAppUtils.validateAction(notifyMessageDoc, MediaRoomServiceConst.SOURCE_STARTOUTPUT, null, schema, log,  this.userJID, xmpp, resourceFactory, serviceAgentFactory); 
					} else if (action.trim().equals(MediaRoomServiceConst.SOURCE_STOPOUTPUT) && agentmessagetype.equals(ServiceClient.RESPONSE)) { 
						ProxyClientAppUtils.validateAction(notifyMessageDoc, MediaRoomServiceConst.SOURCE_STOPOUTPUT, null, schema, log,  this.userJID, xmpp, resourceFactory, serviceAgentFactory); 
					} else if (action.trim().equals(MediaRoomServiceConst.NOTIFYRESPONSE) && agentmessagetype.equals(ServiceClient.RESPONSE)) { 
						ProxyClientAppUtils.validateAction(notifyMessageDoc, MediaRoomServiceConst.NOTIFYRESPONSE, null, schema, log,  this.userJID, xmpp, resourceFactory, serviceAgentFactory); 
					} 

					break; 

				case AgentEvent: 

					String presenceValue = MRXMLUtils.getAttributeValueXpath(notifyMessageDoc, "//presence", "type", ""); 
					if ("unavailable".equalsIgnoreCase(presenceValue)) {
						ProxyClientAppUtils.validateAction(notifyMessageDoc, ProxyConstants.USER_PRESENCE, null, schema, log,  this.userJID, xmpp, resourceFactory, serviceAgentFactory);
					}
					
					break; 

				case XMPPEvent: 
					if(AppServerAgentUtils.isServiceResourceUpdateEvent(message,log,ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getDb(),this.userJID,
							ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getServiceAgentFactory())) {
						System.out.println("Appserver resets the connection");
						proxyClient.setRestart(true);
	                    return;
	        		}
	        		DBUpdateEvent dbUpdateEvent = new DBUpdateEvent(notifyMessageDoc);
	        		if(dbUpdateEvent.getUpdateEventElement() !=null){
	        		String nodeID = dbUpdateEvent.getSubscribedNID();
                    
					if (IPVSConst.getInstance().isEnableNewPubSub()) { 
						if(dbUpdateEvent.isAddorUpdate())
						nodeID = dbUpdateEvent.getUpdateNID(); 
					} 

					ProxyClientAppUtils.validateAction(notifyMessageDoc, "HandleXMPPEvent", nodeID, schema, log,  this.userJID, xmpp, resourceFactory, serviceAgentFactory); 
                    }
					break; 
				} 
		} catch(Throwable throwableObj){        	 
			throwableObj.printStackTrace(); 
		} 
	} 

	private int getProxyPort() { 
		String proxyPort = null; 

		try { 
			proxyPort = MRXMLUtils.getValueXpath(preferencesDoc, ProxyConstants.ProxyPORTPATH, null); 

			return Integer.valueOf(proxyPort); 
		} catch (Exception e) { 
			e.printStackTrace(); 

			return 4360; 
		} 

	} 

	private String getProxyHostName() { 
		String proxyHostName = "127.0.0.1"; 
		try { 
			proxyHostName = MRXMLUtils.getValueXpath(preferencesDoc, ProxyConstants.ProxyHOSTNAMEPATH, null); 
		} catch (Exception e) { 
			e.printStackTrace(); 
		} 

		return proxyHostName; 


	} 

	private String getSourceTitle() { 
		String sourceTitle = ""; 
		
		try { 
			sourceTitle = MRXMLUtils.getValueXpath(preferencesDoc, ProxyConstants.SOURCE_TITLE,""); 
		} catch (Exception e) { 
			e.printStackTrace(); 
		} 

		return sourceTitle; 


	}
	
	private int getHeartBeatFrequency() { 
		String heartBeatFrequency = "1000"; 
		
		try { 
			heartBeatFrequency = MRXMLUtils.getValueXpath(preferencesDoc, ProxyConstants.HEARTBEAT_FREQUENCY,"1000"); 
		} catch (Exception e) { 
			e.printStackTrace(); 
		} 
		return Integer.valueOf(heartBeatFrequency); 


	}
	
	
	@Override
	public void handleXAuthCancelled(Document streamDataDoc)
			throws Exception {
		log.addInfo(":ProxyClientApp@handleXAuthCancelled", MRLog.INFO,MRLog.NOTIFY);
		String authNotifyMessage = ProxyClientAppUtils.generateProxyAuthNotifyMessage(streamDataDoc, null, ProxyConstants.STATE_CANCELLED, log, db);
		log.addInfo(":SendProxyResponseMessage", MRLog.INFO,MRLog.NOTIFY);
		ProxyClientAppUtils.sendProxyResponseMessage(ProxyConstants.AUTHNOTIFY,authNotifyMessage, userJID, proxyClient, log, db, xmpp, resourceFactory, serviceAgentFactory);

	}

	@Override
	public void handleXAuthDeleted(Document streamDataDoc)	throws Exception {
		log.addInfo(":ProxyClientApp@handleXAuthDeleted", MRLog.INFO,MRLog.NOTIFY);
		String authNotifyMessage = ProxyClientAppUtils.generateProxyAuthNotifyMessage(streamDataDoc, null, ProxyConstants.STATE_DISCONNECTED, log, db);
		log.addInfo(":SendProxyResponseMessage", MRLog.INFO,MRLog.NOTIFY);
		ProxyClientAppUtils.sendProxyResponseMessage(ProxyConstants.AUTHNOTIFY,authNotifyMessage, userJID, proxyClient, log, db, xmpp, resourceFactory, serviceAgentFactory);
		
	}

	@Override
	public void handleXAuthRequest(String message) throws MRException {
		try {
			log.addInfo(":In ProxyClientApp@handleXAuthRequest", MRLog.INFO,MRLog.NOTIFY);
			Document messageDoc = MRXMLUtils.stringToDocument(message);
			Element messageElement = MRXMLUtils.getElementXpath(messageDoc, "//message");
			String fromJID = MRXMLUtils.getAttributeValue(messageElement,"from");
			Document xAuthResponse = ProxyClientAppUtils.handleXAuthRequest(proxyClient, userJID, fromJID, message, log, db,xmpp, resourceFactory, serviceAgentFactory);
			if(xAuthResponse!= null) {
				handleXAuthResponse(xAuthResponse);
			}
		}catch(Exception e) {
			e.printStackTrace();
		}

		
	}

	@Override
	public void handleMessage(String message) throws MRException {
		// TODO Auto-generated method stub
		
	}

	public void handleXAuthResponse(Document response){
		try{
			log.addInfo(":Received XAuthResponse", MRLog.INFO,MRLog.NOTIFY);
			xAuthAgent.handleXAuthResponse(response, log, xmpp);
		}catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	@Override
	public void handleProxyClientRequest(Document request) {
        //log.addInfo("processproxyMessage : Got an XML verifying with XSD");
        try {
            MRXMLUtils.validateXML(MRXMLUtils.documentToString(request), ProxyConstants.XSDFILE);
        } catch (Exception e) {
        	e.printStackTrace();
            log.addInfo("handleProxyClientRequest :" + e.toString(), MRLog.INFO,MRLog.NOTIFY);
            return;
        }
        try {
        	String title = getSourceTitle(); 
    		ProxyClientAppUtils.setSourceTitle(title);
    		StringTokenizer st = ProxyClientAppUtils.handleRequest(request, proxyClient, title, userJID, xAuthAgent, log, db, xmpp, resourceFactory, serviceAgentFactory);
    		if(st!=null) {
    			proxyClient.sendResponse(st);
    		}	
    		
        } catch (Exception e) {
        	e.printStackTrace();
            log.addInfo("handleProxyClientRequest :" + e.toString(), MRLog.INFO,MRLog.NOTIFY);
            return;
        }
	}

	@Override
	public boolean loginProxyUser() {
		log.addInfo("Begin Login", MRLog.INFO,MRLog.NOTIFY);
		boolean isLogInSuccessful = login();
		if(!isLogInSuccessful) {
			proxyClient.setRestart(true);
			log.addInfo("Login Failed", MRLog.INFO,MRLog.NOTIFY);
			return false;
		}
		else {
			log.addInfo("Login Successful", MRLog.INFO,MRLog.NOTIFY);
			proxyClient.setRestart(false);
			addDefaultAgents(); 
			log.addInfo("Begin SetServiceDomain", MRLog.INFO,MRLog.NOTIFY);
			setServiceDomain();
		
			synchronized (this) {
				log.addInfo("Waiting for setservicedomain to complete .... ",
						MRLog.INFO, MRLog.NOTIFY);
				while (!isServiceDomainSet) {
					try {
						wait();
					} catch (InterruptedException e) {
						//upon Notify
					}
				}
			}
				
			if(isSetServiceDomainSuccessful()) {
				try {
					xmpp = ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getXmpp();
					db = ServiceUtils.getServiceClient(this).getMRClientAgent(userJID).getDb();
				} catch(Exception e) {
					e.printStackTrace();
				}
				return true;
			}
			logout();
			return false;
		}	
	}

	@Override
	public void handleProxyDisconnected() {
		logout();
		ProxyClientAppUtils.setRecordingSession(null);
	}

	private boolean setServiceDomainSuccessful = false;    

	private synchronized boolean isSetServiceDomainSuccessful() {
		return setServiceDomainSuccessful;
	}

	private synchronized void setSetServiceDomainSuccessful(
			boolean setServiceDomainSuccessful) {
		this.setServiceDomainSuccessful = setServiceDomainSuccessful;
	}
	
	
	
}
