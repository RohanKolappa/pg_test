package com.ipvs.sessionservice.resource.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.DeviceAdminServerAgent;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.systemservice.handler.SystemServiceUtils;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public class UpgradeDeviceRequestHandler extends BaseDeviceRequestHandler implements MRRequestHandlerI {

    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        log.addTraceBegin(request);
        int authType = 1;
        int role = 2;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);
        return auth;

    }
    
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
         	
    		Document requestDoc = request.getMessage().getDocument();    	    		
    		Element requestEl = MRXMLUtils.getElementXpath(requestDoc, "//" + SystemServiceConst.UPGRADE_DEVICE_EL_NAME);

    		request.setSuperUser(true);
    		
    		Document deviceDoc =  this.getUpgradeDeviceList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestEl);
    		List<?> upgradeDeviceElList = MRXMLUtils.getListXpath(deviceDoc, "//" + SystemServiceConst.UPGRADE_DEVICE_EL_NAME + SystemServiceConst.LIST + "/" + AMConst.Device);  
		
    		    		
    		boolean isResponseReturned = false;
    		Document responseDoc = request.createResponseData();
    		Element upgradeDeviceEl = MRXMLUtils.addElementXpath(responseDoc, "//" + request.getResponseDataName(), MRXMLUtils.stringToElement("<" + SystemServiceConst.UPGRADE_DEVICE_EL_NAME + "/>"));            	
			
    		String upgradeURL = MRXMLUtils.getValue(requestEl, SystemServiceConst.URL, "");
    		if (upgradeURL.isEmpty())
    		{
    			AppServerAgent appServerAgent = AppServerAgentUtils.getAppServerAgent(serviceAgentFactory);
    			Element ApiKeyTableEl = appServerAgent.getAPIKey();
    			String agentName = ServiceAgentUtils.getAgentName(request.getUserJID());
    			Element appResourceInfo = AppServerAgentUtils.getMSResourceInfo(log,xmpp, db, serviceAgentFactory, request, ApiKeyTableEl,agentName, request.getUserJID());

    			String ipAddress = MRXMLUtils.getAttributeValue(appResourceInfo,"ipAddress", "");
    			
    			if (ipAddress.isEmpty())
					throw new MRException(SessionServiceExceptionCodes.MS_DEVICE_IP_ADDRESS_MISSING, "MSDevice IP address not found");
				
    			upgradeURL = SystemServiceConst.SOFTWARE_URL;   			   			
    			upgradeURL = upgradeURL.replace(SystemServiceConst._MS_IP_ADDRESS, ipAddress);
    		}
    		
    		for (Object obj : upgradeDeviceElList)
        	{    			
    			Element deviceEl = (Element) obj;
    			log.addInfo("Upgrade Device. DeviceEL:="+deviceEl.asXML(),MRLog.INFO,MRLog.NOTIFY);
    			Element responseDevEl = MRXMLUtils.stringToElement("<" + AMConst.Device + "/>");
    			
    			String NID = MRXMLUtils.getAttributeValue(deviceEl, SessionServiceResourceConst.ASSET_RESOURCE_NID_ATTRIBUTE);
    	    	String title = MRXMLUtils.getAttributeValue(deviceEl, SessionServiceConst.TITLE);
    	    	String deviceNID = MRXMLUtils.getAttributeValue(deviceEl, SessionServiceConst.NID);
    			
    	    	MRXMLUtils.addAttribute(responseDevEl, SystemServiceConst.NID, NID);
    	    	MRXMLUtils.addAttribute(responseDevEl, SystemServiceConst.TITLE, title);
    	    	
    			String serviceVersion = MRXMLUtils.getAttributeValue(deviceEl, SystemServiceConst.SERVICE_VERSION);
    			String softwareVersion = MRXMLUtils.getAttributeValue(deviceEl, SystemServiceConst.SOFTWARE_VERSION);
    			
    			boolean status = SystemServiceUtils.upgradeDeviceServiceVersionCheck(serviceVersion, SystemServiceConst.UPGRADE_FEATURE_SERVICE_VERSION);	
	    		boolean deprecated = !SystemServiceUtils.upgradeDeviceServiceVersionCheck(serviceVersion, SystemServiceConst.DEPRECATED_FEATURE_SERVICE_VERSION);
	    		
	    		MRXMLUtils.addAttribute(MRXMLUtils.getElement(requestDoc, "Header"), "deprecated", String.valueOf(deprecated) );
	    		
	    		String deviceType = MRXMLUtils.getAttributeValue(deviceEl, SystemServiceConst.TYPE);
	    		String upgradeDir= "";
	    		 if(SystemServiceConst.NSGD200_DEVICE.equals(deviceType)){
	    			 upgradeURL += "/" + SystemServiceConst.NGS_SOFTWARE_URL;
	    			 upgradeDir = SystemServiceConst.NGSD200_UPGRADE_DIR;
	    			 boolean validNGSUpgrade = SystemServiceUtils.validateNGSSoftwareVersion(softwareVersion, log);
	    			 if(!validNGSUpgrade)
	    				throw new MRException(SessionServiceExceptionCodes.UPGRADE_SOFTWARE_VERSION_UNSUPPORTED, "Cannot upgrade software version: " + softwareVersion 
	    						+ ". Device must be in version : " + SystemServiceConst.UPGRADE_SUPPORTED_SOFTWARE_VERSION 
	    						+ " and above. Please upgrade manually using following steps: " + "\n" 
	    						+ "1. Log in to NGS device dashboard on a web-browser (http://ngs_ip_address).\n"
	    						+ "2. In Administration -> Upgrade, use url: '" + upgradeURL + "' to upgrade device.");
	    		}else
 				{
 					upgradeURL+= "/"+ SystemServiceConst.DMS_SOFTWARE_URL;
 					upgradeDir= SystemServiceConst.DEVICE_UPGRADE_DIR;
 				}	
	    		
	    		
	    		if (!FileUtils.isFileExist(upgradeDir, SystemServiceConst.SERVER_VERSION_FILE_NAME))
	    			throw new MRException(SessionServiceExceptionCodes.SOFTWARE_IMAGE_MISSING, "Software image not found");
	    		
	    		MRXMLUtils.setValueXpath(requestDoc, "//" + SystemServiceConst.URL, upgradeURL);
	    		//service version empty means its new device upgrade new device through xmpp
	    		//deviceNID is empty means a self registered device
	    		if ((status  || serviceVersion.isEmpty()) || deviceType == SystemServiceConst.IPVS_CLIENT_DEVICE || deviceNID.isEmpty())
	    		{
	    			isResponseReturned = handleUpgradeDeviceOverXMPP(requestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory, deviceEl, responseDevEl);	    			
	    		}
	    		else
	    		{
	    			isResponseReturned = handleUpgradeDeviceOverSSH(requestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseDoc, deviceEl, responseDevEl, upgradeURL);
	    		}
	    		
	    		if (isResponseReturned)    		    			    			
	    			MRXMLUtils.addElement(upgradeDeviceEl, responseDevEl);
    		} 	
    		
    		return responseDoc;
    }

    private boolean handleUpgradeDeviceOverXMPP(Document requestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Element deviceEl, Element responseDevEl) throws Exception
    {       	    
    	log.addInfo("UpgradeDeviceRequestHandler::handleUpgradeDeviceOverXMPP",MRLog.INFO,MRLog.NOTIFY);
    	String ownerUserJID = MRXMLUtils.getAttributeValue(deviceEl, SystemServiceConst.OWNER_USER_JID);	    		
		if (ownerUserJID.isEmpty())
		{
			log.addInfo("UpgradeDeviceRequestHandler::handleUpgradeDeviceOverXMPP->Device unavailable",MRLog.INFO,MRLog.NOTIFY);
			addExceptionElement(request, responseDevEl, SessionServiceExceptionCodes.DEVICE_UNAVAILABLE, "Device unavailable");
			return true;
		}
		Document deviceRequestDoc = (Document)requestDoc.clone();    	
    	MRRequest deviceRequest = MRRequest.createRequest(request, deviceRequestDoc, request.getRequestName(), request.getServiceName());
    	MRXMLUtils.addElementXpath(deviceRequestDoc, "//" + SystemServiceConst.UPGRADE_DEVICE_EL_NAME, (Element) responseDevEl.clone());
    	
    	log.addInfo("UpgradeDeviceRequestHandler::handleUpgradeDeviceOverXMPP->Sending upgrade request to DeviceAdminServerAgent",MRLog.INFO,MRLog.NOTIFY);
    	log.addInfo("device Request Document:::::::::::::::::::::"+deviceRequestDoc.asXML());
    	
    	sendRequestToAgent(deviceRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, deviceRequest.getRequestName(), deviceRequestDoc, MRConst.DEVICE_ADMIN_SERVICE,
    			ownerUserJID, DeviceAdminServerAgent.class.getName(), this.getClass().getName());
    	    	
    	return false;
    }
    
    private boolean handleUpgradeDeviceOverSSH(Document requestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document responseDoc, Element deviceEl, Element responseDevEl, String upgradeURL) throws Exception
    {       
    	log.addInfo("UpgradeDeviceRequestHandler::handleUpgradeDeviceOverSSH",MRLog.INFO,MRLog.NOTIFY);
    	String password = MRXMLUtils.getAttributeValue(deviceEl, SystemServiceConst.PASSWORD);    	
    	if (password == null || password.isEmpty())
    		password = SystemServiceConst.ADMIN_PASSWORD;
    	
    	String ipAddress = MRXMLUtils.getAttributeValue(deviceEl, SystemServiceConst.IP_ADDRESS);    	    	
    	if (ipAddress == null || !Utils.validateIPAddress(ipAddress))
    	{    		
    		log.addInfo("UpgradeDeviceRequestHandler::handleUpgradeDeviceOverSSH->Invalid device IP address:" + ipAddress,MRLog.INFO,MRLog.NOTIFY);
    		addExceptionElement(request, responseDevEl, SessionServiceExceptionCodes.DEVICE_IP_ADDRESS_MISSING, "Device IP address not found");    		    		
    		return true;
    	}

    	String[] systemCmd = {"sh" , "/etc/scripts/remote_upgrade.sh", ipAddress, password, upgradeURL};        	
		if(MediaRoomServiceUtils.executeSystemCommand(systemCmd) != 0) 
		{
			log.addInfo("UpgradeDeviceRequestHandler::handleUpgradeDeviceOverSSH->Upgrade device over SSH failed",MRLog.INFO,MRLog.NOTIFY);
			addExceptionElement(request, responseDevEl, SessionServiceExceptionCodes.UPGRADE_DEVICE_OVER_SSH_FAILED, "Upgrade device over SSH failed");			        		    	
		}		
		
		return true;
    }
    
    private void addExceptionElement(MRRequest request, Element parent, int code, String description) throws Exception
    {
    	request.setState(MRRequest.STATE_EXCEPTION);							
		Element child = MRXMLUtils.getExceptionElement(code, description);     		
		MRXMLUtils.addElementFromOtherDoc(parent, child);
    }        
    
	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document userRequestdoc, MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {

		Document requestDoc = userRequest.getMessage().getDocument();
		Element deviceEl = MRXMLUtils.getElementXpath(requestDoc, "//" + AMConst.Device);
		
		Document responseDoc = userRequest.createResponseData(MRXMLUtils.stringToElement("<" + SystemServiceConst.UPGRADE_DEVICE_EL_NAME + "/>"));
		MRXMLUtils.addElementXpath(responseDoc, "//" + SystemServiceConst.UPGRADE_DEVICE_EL_NAME, (Element) deviceEl.clone());
		
		return responseDoc;
	}
	
}

