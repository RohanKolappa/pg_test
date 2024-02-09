package com.ipvs.sessionservice.handler.base;

import java.net.URLDecoder;
import java.net.URLEncoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.utils.Utils;

public abstract class AsyncBaseRequestHandler implements MRRequestHandlerI,AgentResponseHandlerI,EventLoggerI {

	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,Document userRequestdoc,	MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {
		Element resultElement = MRXMLUtils.getElementXpath(agentResponseDoc, "//Data");
		return MRXMLUtils.elementToDocument(resultElement);	   
	}
	
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

	protected void sendRequestToAgent(MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String agentRequestName, Document agentRequestDoc,String agentServiceName, String toAgentJID,String toAgentName, String responseHandlerClass)  throws MRException,Exception {

		String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequest.getMessage().getDocument()), "UTF-8");		
		MRRequest agentRequest = MRRequest.createRequest(userRequest, agentRequestDoc, agentRequestName, agentServiceName , clientData);
		String dataStr = MRXMLUtils.elementToString(MediaRoomServiceUtils.cloneDataElement(agentRequest));
		String fromAgentName = AppServerAgent.class.getName();

		if ((toAgentJID == null) || (toAgentJID.trim().length() == 0)) {
			throw new MRException(MediaRoomServiceExceptionCodes.AGENT_NOT_FOUND, toAgentName + "Agent JID Not Found");
		}		
		//send request to agent
		String deprecated = MRXMLUtils.getAttributeValue(MRXMLUtils.getElement(agentRequestDoc, "Header"), MRConst.DEPRECATED, "false");
		
		AppServerAgent.sendDeprecatedAgentRequestForUserRequest(userRequest, xmpp, log, schema, resourceFactory, serviceAgentFactory, fromAgentName,
				toAgentJID, agentRequestName, agentServiceName, dataStr, responseHandlerClass, deprecated);   

	}

	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {	

		Document agentResponseDoc = request.getResult();		
		String clientData = MRXMLUtils.getValueXpath(agentResponseDoc, "//"+MRConst.CLIENT_DATA, "");
		Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));

		//create user request
		MRRequest userRequest = MRRequest.unMarshalRequestObject(mrRequestDoc, "", log);	
		Document userResponseDoc = this.getResponseDocument(xmpp,log,db,resourceFactory,serviceAgentFactory,userRequest.getMessage().getDocument(),userRequest,agentResponseDoc);
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc,MRConst.HEADER_XPATH,MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);
		MRException exception = null;
		String logData = "";
		if (state == MediaStoreServiceConst.FAILURE_STATE) {
			int exceptionCode = Utils.getIntValue(MRXMLUtils.getValueXpath(agentResponseDoc, "//Code", ""), SessionServiceExceptionCodes.AGENT_RESPONSE_FAILED);
			String description = MRXMLUtils.getValueXpath(agentResponseDoc, "//Description", "AgentResponseFailed");	        	
			exception =new MRException(exceptionCode,description );
			logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exceptionCode), "Description", description);
		}
		userRequest.setResult(userResponseDoc);
		userRequest.setState(state);
		userRequest.setException(exception);
		try {
			userRequest.addAuditEvent( this.getAuditEvent(userRequest, xmpp, log, db, AuditEventUtils.getAuditEventStatus(userRequest), AuditEventUtils.getAuditEventLevel(userRequest), AuditEventUtils.getAuditEventErrorDescription(userRequest)));
			responseHandler.handleResponse(userRequest, logData);
		}
		catch(Exception e) {
			// TBD put back the exception and handle
			log.addInfo("AsyncBaseRequestHandler:handleAgentResponse error handling agentResponse=" + e.toString(), MRLog.INFO, MRLog.NOTIFY);
		}
	}
	
	@Override
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String status, String level, String errorDescription)
			throws Exception {
	return null;
	}
}
