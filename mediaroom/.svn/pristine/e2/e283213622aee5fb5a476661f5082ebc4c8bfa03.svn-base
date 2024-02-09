package com.ipvs.sessionservice.resource.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
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

public class SetDeviceConfigRequestHandler extends BaseDeviceRequestHandler
		 {

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  AMConst.AssetResource);

		Document assetResourceDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, "", "");
		PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.MODIFY_ROLE_ACTION,PolicyAdminConstants.DEVICE_OBJECT, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
		
	
		String botUser  = MRXMLUtils.getAttributeValueXpath(assetResourceDoc, "//BotUser", "enable","false");
		Element unmannedDevice = MRXMLUtils.getElementXpath(assetResourceDoc, "//Feature[@Name = 'Mode' and .='UnManned']");
		
		if(botUser.equals(SessionServiceConst.FALSE) || unmannedDevice !=null) {
			log.addInfo("SetDeviceConfig: Update Config In Database.",MRLog.INFO,MRLog.NOTIFY);	
			String xpath  = MRXMLUtils.getAttributeValue(requestObject.getRequestElement(), "path","");
			Element deviceConfigEl = MRXMLUtils.getElementXpath(requestObject.getRequestElement(), xpath);
			
		    String listName = "";
	        String NID = "";
	        String childXpath = xpath;
	        String[] arrWhereXpath = { "//AssetResourceNID='"+MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), SessionServiceConst.NID,"")+"'"};
	        MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, listName, AMConst.Device, NID, childXpath, arrWhereXpath, deviceConfigEl);			
			return request.createResponseUsingRequestRootName(null);
		} 

		
		String deviceJID = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.OWNERUSERJID, "");
		String state = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.STATEFLAG, "");
		if(!state.equals(MRClientUtils.ASSET_STATE_READY)) {
			throw new MRException(MRExceptionCodes.DEVICE_OFFLINE, "Device is offline or not ready");
		}

		Document deviceAgentRequestDoc  = (Document)request.getMessage().getDocument().clone();
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME,MRConst.DEVICE_ADMIN_SERVICE);
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.TYPE,MRConst.REQUEST_TYPE_RPC);
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME,"SetDeviceConfigRequest");
		
		//copy request root element objects to setDeviceconfigReqestData
		Element requestDataElement = MRXMLUtils.getElementXpath(deviceAgentRequestDoc, "//SetDeviceConfigRequestData");
		MRXMLUtils.addAttribute(requestDataElement, SessionServiceResourceConst.DO_NOT_SAVE_TO_FLASH_ATTRIBUTE, 
				MRXMLUtils.getAttributeValue(requestObject.getRequestElement(), SessionServiceResourceConst.DO_NOT_SAVE_TO_FLASH_ATTRIBUTE,SessionServiceConst.FALSE));
		
		AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
				deviceJID, "SetDeviceConfigRequest", MRConst.DEVICE_ADMIN_SERVICE, requestDataElement.asXML(), this.getClass().getName());
		
		return null;
	}

}