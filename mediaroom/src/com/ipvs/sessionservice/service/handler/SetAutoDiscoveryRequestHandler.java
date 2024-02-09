package com.ipvs.sessionservice.service.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.impl.ConfigDeviceUtils;
import com.ipvs.sessionservice.handler.base.AsyncBaseRequestHandler;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;
import com.ipvs.sessionservice.resource.handler.GetSystemConfigRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;

public class SetAutoDiscoveryRequestHandler extends AsyncBaseRequestHandler implements MRRequestHandlerI {
	
	 @Override
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
	    
	    @Override
        public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
	    	
	    	log.addInfo("SetAutoDiscoveryRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
	    	
	    	Document requestDoc = request.getMessage().getDocument();
	    	String enable = MRXMLUtils.getAttributeValueXpath(requestDoc, "//SetAutoDiscovery", "enable", "false");
	    	Element groupsEL = MRXMLUtils.getElementXpath(requestDoc, "//"+SessionServiceResourceConst.GROUPS_ELEMENT);
	    	
	    	Element serviceElementsConfigEl = MRXMLUtils.newElement("ServiceElementsConfig");
	    	Element xmppServiceConfigEl = MRXMLUtils.newElement("XMPPServiceConfig");
	    	serviceElementsConfigEl.add(xmppServiceConfigEl);
	    	Element xmppServerEl = MRXMLUtils.newElement("XMPPServer");
	    	xmppServiceConfigEl.add(xmppServerEl);
			Element deviceDiscoveryEl = MRXMLUtils.newElement("DeviceDiscovery");
			MRXMLUtils.setAttributeValue(deviceDiscoveryEl, ConfigDeviceUtils.DIRTY_TAG, ConfigDeviceUtils.MODIFY_FLAG);
			Element enableEl = MRXMLUtils.setValue(MRXMLUtils.newElement("Enable"), enable);
			deviceDiscoveryEl.add(enableEl);
			deviceDiscoveryEl.add(groupsEL.detach());
			xmppServerEl.add(deviceDiscoveryEl);
			
			Document setDeviceConfigRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/sessionservice/resource/xml/SetDeviceConfigRequest.xml", log);
			Element deviceConfig = MRXMLUtils.getElementXpath(setDeviceConfigRequestDoc, "//DeviceConfig");
    		Element deviceConfigRequestEl = deviceConfig.getParent();
    		MRXMLUtils.removeContent(deviceConfigRequestEl);
    		MRXMLUtils.addElement(deviceConfigRequestEl,(Element)serviceElementsConfigEl.clone());
    		
    		Document getSystemConfigSampleRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/sessionservice/resource/xml/GetSystemConfigRequest.xml", log);
    		String requestName = MRRequest.getRequestName(GetSystemConfigRequestHandler.class.getName());
    		
    		Element getSystemConfigEL = MRXMLUtils.getElementXpath(getSystemConfigSampleRequestDoc,"//GetSystemConfig");
    		MRXMLUtils.setAttributeValue(getSystemConfigEL, SessionServiceConst.WHERE_CONFIG, "DeviceDiscovery");
    		
    		Document getSystemConfigRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, getSystemConfigEL);
    		MRRequest getSystemConfigRequest = MRRequest.createRequest(request, getSystemConfigRequestDoc, requestName, ServiceClient.SESSIONSERVICE_RESOURCE);	
    		getSystemConfigRequest.setSuperUser(true);
    		MRRequestHandlerI GetSystemConfigRequestHandler = new GetSystemConfigRequestHandler();
    		Document getSystemConfigResponse = GetSystemConfigRequestHandler.getResponse(getSystemConfigRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);		
    		String deviceJID = MRXMLUtils.getAttributeValueXpath(getSystemConfigResponse, "//Device", "jid", "");
    		if(deviceJID.isEmpty()) {
                throw new MRException(MRExceptionCodes.DEVICE_OFFLINE, "Device is offline.");
            }
    		
    		sendRequestToAgent(request, xmpp, log, db, resourceFactory, serviceAgentFactory, MRServiceConst.SETDEVICECONFIGREQUEST, setDeviceConfigRequestDoc, MRConst.DEVICE_ADMIN_SERVICE, deviceJID, MRConst.DEVICE_ADMIN_SERVICE, this.getClass().getName());
    		
	    	if(enable.equals("true")) {
	        	Element assetResourceEl = DBServiceUtils.getAssetResource(SessionServiceResourceConst.DISCOVERY_TEMPLATE, request, log, db);
	        	if(assetResourceEl == null) {
	        		Document assetResourceDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.AssetResource, log);
	        		MRXMLUtils.setValueXpath(assetResourceDoc, "//"+SessionServiceResourceConst.TEMPLATE_ASSET_RESOURCE_NID, SessionServiceResourceConst.TEMPLATE);
		    		MRXMLUtils.setValueXpath(assetResourceDoc, "//Info/Type", AMConst.ANY_OBJECTTYPE);
		    		MRXMLUtils.setValueXpath(assetResourceDoc, "//Info/Title", SessionServiceResourceConst.DISCOVERY_TEMPLATE);
		    		MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(assetResourceDoc, "//"+SessionServiceResourceConst.GROUPS_ELEMENT), groupsEL);
	        		AddObjectUtils.addObject(request, xmpp, log, db, resourceFactory, serviceAgentFactory, assetResourceDoc, AMConst.AssetResource);
	        	}
	        	else {
	        		MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(assetResourceEl, "./Info/"+SessionServiceResourceConst.GROUPS_ELEMENT), groupsEL);
	        		String parentElXPath = "//"+SessionServiceResourceConst.INFO_ELEMENT;
	        		String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID, "");
		    		UpdateObjectUtils.updateElement(request, xmpp, log, db, AMConst.AssetResource, assetResourceNID, assetResourceEl.getDocument(), parentElXPath);
	        	}
	        		
	    	}
	    return request.createResponseUsingRequestRootName(null);
	    }
}
