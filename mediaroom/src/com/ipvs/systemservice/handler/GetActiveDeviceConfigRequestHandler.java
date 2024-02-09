package com.ipvs.systemservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
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
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.systemservice.impl.HAUtils;

public class GetActiveDeviceConfigRequestHandler extends BaseDeviceRequestHandler {
/*
 *This request Handler get the device config from active device and if standby is in notready state, it sends the activedeviceconfigudpate event to standby 
 * 
 */

    @Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
	    MRDB db, MRResourceFactoryI resourceFactory,
	    ServiceAgentFactoryI serviceAgentFactory) throws Exception {
	Element serviceResourceElement = HAUtils.getServiceResourceElement(db, request, log);
	if(serviceResourceElement!=null) {	   
	    String standbyAssetResourceNID = HAUtils.getMsAssetResourceNID("standby", serviceResourceElement, log);		    
	    if(!standbyAssetResourceNID.isEmpty()){		    
		Element resourceEL = MRXMLDBUtils.readElement(request, log, db, standbyAssetResourceNID).getRootElement();
		if(resourceEL != null){
		    String standbyState = MRXMLUtils.getValueXpath(resourceEL, ".//"+AMConst.STATEFLAG, "");
		    if((SessionServiceConst.NOT_READY).equals(standbyState)){
			RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  AMConst.AssetResource);
			Document assetResourceDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, "", "");			
			String deviceJID = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.OWNERUSERJID, "");
			
			String state = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.STATEFLAG, "");
			if(!state.equals(MRClientUtils.ASSET_STATE_READY)) {
			    throw new MRException(MRExceptionCodes.DEVICE_OFFLINE, "Device is offline or not ready");
			}
			Element getDeviceConfigEl = (Element)MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//GetActiveDeviceConfig").clone();
			getDeviceConfigEl.setName("GetDeviceConfig");
			
			Element deviceAgentRequestEl = MRXMLUtils.getElementXpath(MRXMLUtils.loadXMLResource("/com/ipvs/sessionservice/resource/xml/GetDeviceConfigRequest.xml", log), "//GetDeviceConfigRequestData");
			MRXMLUtils.getElementXpath(deviceAgentRequestEl, ".//GetDeviceConfig").detach();			
			
			MRXMLUtils.setAttributeValueXPath(deviceAgentRequestEl, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME,MRConst.DEVICE_ADMIN_SERVICE);
			MRXMLUtils.setAttributeValueXPath(deviceAgentRequestEl, MRConst.HEADER_XPATH, MRConst.TYPE,MRConst.REQUEST_TYPE_GET);
			MRXMLUtils.setAttributeValueXPath(deviceAgentRequestEl, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME,"GetDeviceConfigRequest");
			MRXMLUtils.addElement(deviceAgentRequestEl, getDeviceConfigEl);			
			AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
				deviceJID, "GetDeviceConfigRequest", MRConst.DEVICE_ADMIN_SERVICE, deviceAgentRequestEl.asXML(), this.getClass().getName());
		    }
		}
	    }
	}
	return null;		
    }

    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,Document userRequestdoc, MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {

	Element deviceConfigElemenet = MRXMLUtils.getElementXpath(agentResponseDoc, "//DeviceConfig");
	Element deviceElement = MRXMLUtils.newElement("Device");
	MRXMLUtils.addChildElement(deviceElement, MRXMLUtils.getElementXpath(deviceConfigElemenet, ".//XMPPServiceConfig"));
	MRXMLUtils.addChildElement(deviceElement, MRXMLUtils.getElementXpath(deviceConfigElemenet, ".//HAServiceConfig"));

	Element serviceResourceElement = HAUtils.getServiceResourceElement(db, userRequest, log);
	if(serviceResourceElement!=null) {
	    String selectXML = "<Device>"
		    + "{root($VAR)/*/@NID}"
		    + "{$VAR//EthernetPortTableStatus}</Device>";

	    String whereXPath = "AssetResourceNID[.='"+HAUtils.getMsAssetResourceNID("active", serviceResourceElement, log)+"']";
	    Element activeServerDeviceEl = AssetAdminServiceUtils.getChildBasedOnSelectXML(AMConst.Device, selectXML, whereXPath,userRequest, db, log, xmpp);
	    MRXMLUtils.addChildElement(deviceElement, MRXMLUtils.getElementXpath(activeServerDeviceEl, ".//EthernetPortTableStatus"));
	    log.addInfo("*** GetActiveDeviceConfig: deviceElement:="+deviceElement.asXML());
	    String standbyAssetResourceNID = HAUtils.getMsAssetResourceNID("standby", serviceResourceElement, log);		    
	    if(!standbyAssetResourceNID.isEmpty()){
		MRRequest newRequest = MRRequest.createRequest(userRequest.getUserJID());
		HAUtils.sendPrimaryConfigUpdateEvent(newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, HAUtils.getDeviceJIDFromAssetResourceNID(newRequest, log, db, standbyAssetResourceNID), newRequest.getUserJID(), standbyAssetResourceNID, serviceResourceElement,null, deviceElement,"");
	    }


	}
	return null;
    }


}
