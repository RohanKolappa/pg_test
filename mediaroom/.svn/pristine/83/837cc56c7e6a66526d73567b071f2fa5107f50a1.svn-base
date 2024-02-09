package com.ipvs.sessionservice.handler.base;


import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
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
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.FileUtils;


public abstract class BaseDeviceRequestHandler extends AsyncBaseRequestHandler implements MRRequestHandlerI {
	public static final String GET_DEVICE_REQUEST_XML_PATH = "/com/ipvs/sessionservice/resource/xml/GetDeviceRequest.xml";
	public static final String GET_PORT_REQUEST_XML = "/com/ipvs/sessionservice/session/xml/GetPortRequest.xml";
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

	    int authType = MediaRoomObject.ROOM_TYPEID;
	    int role = MediaRoomPEM.VISITOR_ROLE;
	    String token = "testCommonToken";
	    String authority = "testAuthority";
	    String roomPolicyNID = MRConst.NULL;
	    String streamPolicyNID = MRConst.NULL;
	    MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token,
	                authority);
	    log.addTraceEnd(request);

	    return auth;
	}
	
	
	public Document getUpgradeDeviceList(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element requestEl) throws Exception {
		
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AMConst.AssetResource);
		
		String nid = requestObject.getNID();
		String title = requestObject.getAttributeValue();
		
		List<String> listOfXPaths = new ArrayList<String>();
		
		requestObject.generateXPathBasedOnAttributeType(listOfXPaths, AMConst.Device, SessionServiceResourceConst.BY_NID_ATTRIBUTE, nid, 
				SessionServiceConst.NID_CONST);
		requestObject.generateXPathBasedOnAttributeType(listOfXPaths, AMConst.Device, requestObject.getAttributeType(), title, 
				requestObject.getAttributeConstant());
		
		String whereXPath = GetObjectUtils.generateWhereXpathFromList(listOfXPaths, "and", AMConst.Device);
		
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.GET_UPGRADE_DEVICE_INFO + SessionServiceConst.XQUERYXML;				
		String query = FileUtils.readFileResource(xqueryFileName, request.getClass());
		query = query.replace(SessionServiceConst.XPATHAND, whereXPath);		

		Document responseDoc = GetObjectUtils.executeQuery(query, request, xmpp, log, db, serviceAgentFactory, resourceFactory);			
		if (responseDoc == null)
			throw new NullPointerException();
		
		return responseDoc;		
		
	}
	
	public void updatePortInfo(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String title,String description,Element groupsElFromRequestDoc,Document objectXMLDoc,String objectType,String nid, String applyTagsToPort)  throws MRException,Exception {

		String parentElXPath = "//" + SessionServiceResourceConst.INFO_ELEMENT;
		// Set & update
		MRXMLUtils.setValueXpath(objectXMLDoc, parentElXPath + "/" + SessionServiceResourceConst.TITLE, title);
		MRXMLUtils.setValueXpath(objectXMLDoc, parentElXPath + "/" + SessionServiceConst.DESCRIPTION, description);
		if(applyTagsToPort != null && applyTagsToPort.equals(SessionServiceConst.TRUE) && groupsElFromRequestDoc!=null) {
			MRXMLUtils.replaceContent(objectXMLDoc, parentElXPath + "/" + SessionServiceResourceConst.GROUPS_ELEMENT, groupsElFromRequestDoc);
		}
		log.addInfo("SetPortInfoRequestHandler::Update element in db", MRLog.OK, MRLog.NOTIFY);
		UpdateObjectUtils.updateElement(request, xmpp, log, db, objectType,nid, objectXMLDoc, parentElXPath);
	}
	
	public Document sendAgentRequest(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String requestName) throws Exception {
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  AMConst.AssetResource);

		Document assetResourceDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, "", "");
		//PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.MODIFY_ROLE_ACTION,PolicyAdminConstants.DEVICE_OBJECT, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
		String deviceJID = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.OWNERUSERJID, "");
		String state = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.STATEFLAG, "");
		if(!state.equals(MRClientUtils.ASSET_STATE_READY)) {
			throw new MRException(MRExceptionCodes.DEVICE_OFFLINE, "Device is offline or not ready");
		}
		
		Document deviceAgentRequestDoc  = (Document)request.getMessage().getDocument().clone();
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.DEVICE_ADMIN_SERVICE);
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.TYPE, MRConst.REQUEST_TYPE_RPC);
		MRXMLUtils.setAttributeValueXPath(deviceAgentRequestDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
		
		//copy request root element objects to XXXDeviceRequestData
		Element requestDataElement = MRXMLUtils.getElementXpath(deviceAgentRequestDoc, "//" + requestName + "Data");
		
		if(requestName.equals("RevertToDefaultDeviceRequest")) {
			String keepMedia = MRXMLUtils.getAttributeValueXpath(deviceAgentRequestDoc, "//RevertToDefaultDevice", "keepMedia", SessionServiceConst.FALSE);	
			if(keepMedia.isEmpty()) 
				keepMedia = SessionServiceConst.FALSE;
			String keepNetworkSettings =  MRXMLUtils.getAttributeValueXpath(deviceAgentRequestDoc, "//RevertToDefaultDevice", "keepNetworkSettings", SessionServiceConst.FALSE);
			if(keepNetworkSettings.isEmpty())
				keepNetworkSettings = SessionServiceConst.FALSE;
			MRXMLUtils.addAttribute(requestDataElement, "keepMedia", keepMedia);
			MRXMLUtils.addAttribute(requestDataElement, "keepNetworkSettings", keepNetworkSettings);
		}
		
		AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AppServerAgent.class.getName(),
				deviceJID, requestName, MRConst.DEVICE_ADMIN_SERVICE, requestDataElement.asXML(), this.getClass().getName());
		
		return null;
	}
	
}