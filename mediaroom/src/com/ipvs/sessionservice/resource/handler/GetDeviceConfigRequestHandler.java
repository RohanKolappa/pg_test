package com.ipvs.sessionservice.resource.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class GetDeviceConfigRequestHandler extends BaseDeviceRequestHandler {


	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  AMConst.AssetResource);

		Document assetResourceDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, "", "");

		PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.READ_ROLE_ACTION,PolicyAdminConstants.DEVICE_OBJECT, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

		String botUser  = MRXMLUtils.getAttributeValueXpath(assetResourceDoc, "//BotUser", "enable","false");
		Element unmannedDevice = MRXMLUtils.getElementXpath(assetResourceDoc, "//Feature[@Name = 'Mode' and .='UnManned']");
		
		if(botUser.equals(SessionServiceConst.FALSE) || unmannedDevice !=null) {
			log.addInfo("GetDeviceConfig: Read Config From Database.",MRLog.INFO,MRLog.NOTIFY);
			Document responseDocument = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					requestObject, SessionServiceConst.SELECTFORMAT_RAW, GET_DEVICE_REQUEST_XML_PATH, GetDeviceRequestHandler.class.getName());
			Element deviceConfigElemenet = MRXMLUtils.getElementXpath(responseDocument, "//DeviceConfig");
			Element deviceElement = MRXMLUtils.getElementXpath(responseDocument, "//Device");
			deviceElement.clearContent();
			MRXMLUtils.addElement(deviceElement,deviceConfigElemenet);			
			return request.createResponseUsingRequestRootName((Element)deviceElement.detach());
		} 

		String deviceJID = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.OWNERUSERJID, "");
		String state = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.STATEFLAG, "");
		if(!state.equals(MRClientUtils.ASSET_STATE_READY)) {
			throw new MRException(MRExceptionCodes.DEVICE_OFFLINE, "Device is offline or not ready");
		}

		Document deviceAgentRequestDoc  =(Document) request.getMessage().getDocument().clone();
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME,MRConst.DEVICE_ADMIN_SERVICE);
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.TYPE,MRConst.REQUEST_TYPE_GET);
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME,"GetDeviceConfigRequest");
	
		AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
				deviceJID, "GetDeviceConfigRequest", MRConst.DEVICE_ADMIN_SERVICE, MRXMLUtils.getElementXpath(deviceAgentRequestDoc, "//GetDeviceConfigRequestData").asXML(), this.getClass().getName());
		
		return null;		
	}

	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,Document userRequestdoc,	MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {
		RequestProcessor requestObject = new RequestProcessor(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, AMConst.AssetResource);
		Document assetResourceDoc = GetObjectUtils.getObjectXMLFromDB(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, "", "");
		System.out.println("agentResponseDoc:="+agentResponseDoc.asXML());
		Element deviceConfigElemenet = MRXMLUtils.getElementXpath(agentResponseDoc, "//DeviceConfig");
		Element deviceElement = MRXMLUtils.newElement("Device");
		deviceElement.addAttribute(SessionServiceConst.NID, MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), SessionServiceConst.NID));
		deviceElement.addAttribute(SessionServiceConst.TYPE_ATTRIBUTE, MRXMLUtils.getValueXpath(assetResourceDoc, "//Type", ""));
		deviceElement.addAttribute(SessionServiceConst.TITLE, MRXMLUtils.getValueXpath(assetResourceDoc,"//Title",""));
		MRXMLUtils.addElement(deviceElement,(Element)deviceConfigElemenet.detach());
		return userRequest.createResponseUsingRequestRootName(deviceElement);
	}


}
