package com.ipvs.sessionservice.window.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRServiceConst;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.features.FeatureBase;
import com.ipvs.sessionservice.handler.base.PortBaseRequestHandler;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;


public class SetPortDisplayConfigRequestHandler extends PortBaseRequestHandler implements MRRequestHandlerI{
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
			 log.addTraceBegin(request);

		        int authType = MediaRoomObject.ROOM_TYPEID;
		        int role = MediaRoomPEM.VISITOR_ROLE;
		        String token = "testCommonToken";
		        String authority = "testAuthority";
		        String roomPolicyNID = MRConst.NULL;
		        String streamPolicyNID = MRConst.NULL;
		        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		        log.addTraceEnd(request);
		        return auth;
	}

	
	public Document sendSetPortDisplayConfigRequestToDevice(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document portDoc, Element setPortDisplayConfigElement) throws Exception {
		
	    Element portElement = MRXMLUtils.getElement(portDoc.getRootElement(), SessionServiceConst.PORT);
		String portNID = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.NID, "");
		String portTitle = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.TITLE, "");
		Element portDisplayConfigEL = MRXMLUtils.stringToElement("<PortDisplayConfig></PortDisplayConfig>");
		portDisplayConfigEL.addAttribute(SessionServiceConst.NID, portNID);
		portDisplayConfigEL.addAttribute(SessionServiceConst.TITLE, portTitle);

		FeatureBase featureBase = getSupportedFeature(portElement);
		if(featureBase == null) {
			throw new MRException(SessionServiceExceptionCodes.FEATURE_NOT_SUPPORTED, "Port doesn't support this feature");
		} 
		Document deviceConfigurationRequestDoc =  featureBase.generateDeviceAgentRequestFromWindowAPI(request, xmpp, log,db,  resourceFactory,
				serviceAgentFactory, setPortDisplayConfigElement, portElement);
		
		//update device in database if device is offline
		String state = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.STATE,"");
		if(!state.equals(AMConst.ASSETSTATE_READY)){			
			featureBase.updateConfigInDatabase(request, xmpp, log, db, resourceFactory, serviceAgentFactory, deviceConfigurationRequestDoc, portElement);
			log.addInfo("SetPortDisplayConfigRequestHandler: Port Title:="+MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.TITLE)+" Offline. Update Display config in database",MRLog.INFO,MRLog.NOTIFY);
			return getResponseDocument(xmpp, log, db, resourceFactory, serviceAgentFactory, request.getMessage().getDocument(), request, null);			
		}
		
		String deviceAgentJID = MRXMLUtils.getAttributeValue(portElement, "ownerUserJID", "");
		String toAgentName = MRServiceConst.DEVICEADMINSERVERAGENT;
		String responseHandlerName = this.getClass().getName();
		this.sendRequestToAgent(request, xmpp, log, db, resourceFactory, serviceAgentFactory, MRXMLUtils.getAttributeValueXpath(deviceConfigurationRequestDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME,"")
				,deviceConfigurationRequestDoc, MRConst.DEVICE_ADMIN_SERVICE, deviceAgentJID, toAgentName, responseHandlerName);
		return null;

	}

	
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "");
		Document portDoc = getPortDoc (request,xmpp,log,db, resourceFactory,serviceAgentFactory, requestObject);
		return sendSetPortDisplayConfigRequestToDevice(request, xmpp, log,db, resourceFactory,	serviceAgentFactory, portDoc,  requestObject.getRequestElement());
    }
	
	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,Document userRequestdoc,	MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {
		return userRequest.createResponseUsingRequestRootName(null);  
	}
}
