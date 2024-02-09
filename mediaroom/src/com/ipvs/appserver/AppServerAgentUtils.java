package com.ipvs.appserver;



import java.io.File;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.ResourcePresenceManager;
import com.ipvs.http.common.HttpAuthentication.AuthScheme;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.notification.ResourceManager;
import com.ipvs.sessionservice.impl.AutoStartManager;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.PVRManager;
import com.ipvs.sessionservice.impl.ScheduleManager;
import com.ipvs.sessionservice.impl.SessionManager;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.subscriptions.SubscriptionManager;
import com.ipvs.systemservice.handler.RoomGC;
import com.ipvs.utils.FileUtils;

public class AppServerAgentUtils {

	public static AppServerAgent getAppServerAgent(ServiceAgentFactoryI agentFactory) {
		return (AppServerAgent) (agentFactory.getServiceAgent(AppServerAgent.class.getName()));
	}

	public static PVRManager getPVRManager(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getPVRManager();
	}

	public static AutoStartManager getAutoStartManager(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getAutoStartManager();
	}

	public static ResourceManager getResourceManager(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getResourceManager();
	}

	public static SessionManager getSessionManager(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getSessionManager();
	}

	public static SubscriptionManager getSubscriptionManager(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getSubscriptionManager();
	}

	public static String getAppServerAgentJID(String userJID,ServiceAgentFactoryI agentFactory) throws MRException,Exception {
		return getAppServerAgent(agentFactory).getAppServerAgentJID(userJID);
	}

	public static boolean isServiceResourceUpdateEvent(String message,MRLog log,MRDB db,String userJID,ServiceAgentFactoryI agentFactory)throws Exception{  
		return getAppServerAgent(agentFactory).isServiceResourceUpdateEvent(message, log, db, userJID);
	}

	public static ScheduleManager getScheduleManager(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getScheduleManager();
	}

	public static AuthTokenCache getAuthTokenCache(ServiceAgentFactoryI agentFactory) throws Exception{
		return getAppServerAgent(agentFactory).getAuthTokenCache();
	}

	public static ResourcePresenceManager getResourcePresenceManager(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getResourcePresenceManager();
	}

	public static ServiceConfigManager getServiceConfigManager(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getServiceConfigManager();
	}

	public static RoomGC getRoomGC(ServiceAgentFactoryI agentFactory) {
		return getAppServerAgent(agentFactory).getRoomGC();
	}	   


	private static Document getMSResource(ServiceAgentFactoryI agentFactory,MRLog log,XMPPI xmpp,MRDB db,String dstPortTitle,String userJID,String authScheme) throws Exception {

		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + "GetMSResource" + SessionServiceConst.XQUERYXML;	    	   

		String query = FileUtils.readFileResource(xqueryFileName, log.getClass());
		
		query = query.replace("_DSTTITLE_", dstPortTitle);
		query = query.replace("_AUTHSCHEME_", authScheme);
		
		MRRequest request = MRRequest.createRequest(userJID);

		request.setSuperUser(true);
		return GetObjectUtils.executeQuery(query,request, xmpp, 
				log, db, agentFactory, null);
	}

	public static Element getMSResourceInfo(MRLog log,XMPPI xmpp,MRDB db,ServiceAgentFactoryI agentFactory,MRRequest request,Element ApiKeyTableEl,String dstTitle,String userJID) throws Exception {
		return getMSResourceInfo(log, xmpp, db, agentFactory, request, ApiKeyTableEl, dstTitle, userJID, AuthScheme.EXPIRY.toString());
	}


	public static Element getMSResourceInfo(MRLog log,XMPPI xmpp,MRDB db,ServiceAgentFactoryI agentFactory,MRRequest request,Element ApiKeyTableEl,String dstTitle,String userJID,String authScheme) throws Exception {

		Element resourceElement = MRXMLUtils.newElement("Resource");
		MRXMLUtils.setAttributeValue(resourceElement, "ipAddress",MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.MS_IPADDRESS, "") );
		
		//if destination is not provided get resource from logged in jid
		if(dstTitle.isEmpty()) {    
			MRXMLUtils.addElement(resourceElement,MRXMLUtils.newElement("Destination"));		
			return resourceElement;			    		
    	}
		
		
		Document queryResultDoc = getMSResource(agentFactory, log, xmpp, db,dstTitle,userJID,authScheme);
		if(queryResultDoc == null) {
			return resourceElement;
		}
		Element sourceElement = MRXMLUtils.getElementXpath(queryResultDoc, "//Source");
		Element destinationElement = MRXMLUtils.getElementXpath(queryResultDoc, "//Destination");
		Document deviceDoc = MRXMLUtils.getDocumentXpath(queryResultDoc, "//Device");

		String ipaddress =  StreamPolicyUtils.getResolvedIPAddressForURL(request, xmpp, log, db, destinationElement, sourceElement, deviceDoc);
		MRXMLUtils.setAttributeValue(resourceElement, "ipAddress",ipaddress);
		MRXMLUtils.setAttributeValue(resourceElement, "dstNID", MRXMLUtils.getAttributeValue(MRXMLUtils.getFirstChild(destinationElement), "NID",""));
		String destState  = MRXMLUtils.getValueXpath(destinationElement, ".//StateFlag", "");
		String templateTitle ="";
		if(destState.equals(MRClientUtils.ASSET_STATE_TEMPLATE)) {
			templateTitle = MRXMLUtils.getValueXpath(destinationElement, ".//"+SessionServiceConst.TITLE_ELEMENT, "");
		}
		MRXMLUtils.setAttributeValue(resourceElement, "dstNID", MRXMLUtils.getAttributeValue(MRXMLUtils.getFirstChild(destinationElement), "NID",""));
		MRXMLUtils.setAttributeValue(resourceElement, "templateTitle", templateTitle);
		MRXMLUtils.addElement(resourceElement, (Element)destinationElement.detach());
		return resourceElement;
	}
	
	public static Element getAPIKeyTable(){
			String apiKeyTable = "/var/status/APIKeyTable.xml";
			File file = new File(apiKeyTable);
			Element apiKeyTableEl = DocumentHelper.createElement("ApiKeyTable");
			if (!file.exists()) {
				MRLog.getLogger().info(MRExceptionCodes.XML_NOT_FOUND+" :APIKeyTable not found. ");
				return apiKeyTableEl;
	        }

	        try {
	        	String apiKeyTableStr = FileUtils.readFile(file); 
	        	apiKeyTableEl = MRXMLUtils.stringToElement(apiKeyTableStr);
	        	return apiKeyTableEl;
	        } catch (Exception ex) {
	        	MRLog.getLogger().info(MRExceptionCodes.XML_NOT_VALID+" : APIKeyTable not valid. ");
	        	return apiKeyTableEl;    
	        }
	}
}
