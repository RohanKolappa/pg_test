package com.ipvs.sessionservice.handler.base;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
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
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.window.handler.SessionServiceWindowConst;

public abstract class SetPortCommandsBaseRequestHandler extends AsyncBaseRequestHandler implements MRRequestHandlerI {
	
	protected abstract Document updateResponseDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory,Document responsedoc) throws MRException,Exception; 
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
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

	protected Document handleSetCommands(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String commandResponseElName) throws MRException,Exception {
		String  byNID=  "";
		
		String requestName = MRRequest.getRequestRootName(request.getRequestName());		
		Element windowCommandEl = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//" + requestName);

		byNID = MRXMLUtils.getAttributeValueWithAlternate(windowCommandEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, SessionServiceConst.WHERE_NID, "");
		
		//if where port nid is empty then find the port nid from the session
		if(byNID.isEmpty()){
			String bySessionID = MRXMLUtils.getAttributeValueWithAlternate(windowCommandEl, SessionServiceWindowConst.BY_SESSION_ID, SessionServiceConst.WHERE_SESSION_ID, "");
			String byConnId = MRXMLUtils.getAttributeValueWithAlternate(windowCommandEl, SessionServiceWindowConst.BY_CONN_ID, SessionServiceConst.WHERE_CONN_ID, "");
			if (bySessionID.isEmpty() || byConnId.isEmpty()) {
				throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Either Session and connection Id or port NID needed");
			}
			Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, bySessionID);
			Element connEL = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='"+byConnId+"']");
			String destEntityId = MRXMLUtils.getAttributeValue(connEL, SessionServiceConst.DSTENTITYID,"");
			Element destEntityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+destEntityId+"']");
			String destNID = MRXMLUtils.getAttributeValueXpath(destEntityEL, "./Port","resourceNID", "");				
			if(destNID.isEmpty()){
				throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Either Session and connection Id or port NID needed");					
			}
			byNID = destNID;
		}

		// get the port document from database
		String selectXML = "<Port>" +"{root($VAR)/*/@NID}{$VAR//Contact} {$VAR//FeatureList}" + "</Port>";
		Element portEl = AssetAdminServiceUtils.getChildBasedOnSelectXMLNID(byNID, selectXML, "", request, db, log, xmpp);

		//make sure device is ready to accept windowing commands
		String ownerJID = MRXMLUtils.getValueXpath(portEl, ".//OwnerUserJID", "");
		if(ownerJID.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.DEVICE_NOT_READY,"Device Not Ready");
		}

		// if the requested port is not ipvs client display config feature just ignore
		Element featureEl = MRXMLUtils.getElementXpath(portEl, ".//Feature[@Name='"+SessionServiceConst.IPVSCLIENTDISPLAYCONFIG+"']");
		if(featureEl == null) {
			throw new MRException(SessionServiceExceptionCodes.FEATURE_NOT_SUPPORTED, "Port doesn't support this feature");
		}

		// send device configuration request to destination
		Document deviceConfigurationRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/systemservice/xml/DeviceConfigurationRequest.xml", log);
		Element deviceConfig = MRXMLUtils.getElementXpath(deviceConfigurationRequestDoc, "//DeviceConfig");
		Element deviceConfigRequestEl = deviceConfig.getParent();
		MRXMLUtils.removeContent(deviceConfigRequestEl);
		MRXMLUtils.addElement(deviceConfigRequestEl,(Element)windowCommandEl.clone());
		sendDeviceAdminRequest(ownerJID, deviceConfigurationRequestDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		//uncomment for synch calls
		//return null;		


		return null;

	}
	
//	//create response document 
//	private Document createResponseDoc(MRRequest request,Element windowCommandEl,String commandResponseElName,MRException exp,String portNID) throws Exception, MRException {
//		
//		if(windowCommandEl == null) {
//			throw new MRException(SessionServiceExceptionCodes.COMMAND_NOT_FOUND, "Command Not Found");
//		}
//		
//		Element cmdResponseEL = null;
//		cmdResponseEL = MRXMLUtils.newElement(windowCommandEl.getName());
//	
//		windowCommandEl.addAttribute(SessionServiceConst.NID, MRXMLUtils.getAttributeValue(windowCommandEl, portNID));
//		
//		Element commandResultEL = MRXMLUtils.newElement(commandResponseElName);
//		if(exp == null) {
//			MRXMLUtils.setValue(commandResultEL, SessionServiceConst.SUCCESS);
//		} else {
//			Element exceptionElement = PortBaseRequestHandler.generateExceptionElement(exp.getCode(),exp.getData());
//			MRXMLUtils.addElement(commandResultEL,exceptionElement);
//		}
//		
//		MRXMLUtils.addElement(cmdResponseEL, commandResultEL);
//		return request.createResponseData(cmdResponseEL);
//	}
	
	// send device configuration request to port agent jid
	private void sendDeviceAdminRequest(String deviceAgentJID,Document deviceConfigurationRequestDoc ,MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
		
		String toAgentName = MRConst.DEVICE_ADMIN_SERVICE;		
	    String requestName = MRServiceConst.DEVICECONFIGURATIONREQUEST;	    
	    sendRequestToAgent(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, deviceConfigurationRequestDoc, MRConst.DEVICE_ADMIN_SERVICE, deviceAgentJID, toAgentName, this.getClass().getName());
	    
	}
	
	
	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,Document userRequestdoc,	MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {
		return userRequest.createResponseUsingRequestRootName(null);   
	}



}
