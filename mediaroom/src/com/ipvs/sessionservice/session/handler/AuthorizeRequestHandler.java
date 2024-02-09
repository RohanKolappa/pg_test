package com.ipvs.sessionservice.session.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
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
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.handler.ConnectionUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class AuthorizeRequestHandler implements MRRequestHandlerI {
	
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

	void authorizeConnection(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			String roomNID, String connId, String action) throws Exception {
		Document streamDoc = MediaRoomServiceUtils.readElementByXpath(roomNID, request, log, db,"//Stream[@id='"+connId+"']");
		
		Document mediaSourceDoc = MediaRoomServiceUtils.readElementByXpath(roomNID, request, log, db, "//MediaSource[@NID='"+MRXMLUtils.getValueXpath(streamDoc,"//mediaSourceNID", "")+"']");
		String xAuthAgentJID = MRXMLUtils.getValueXpath(mediaSourceDoc, "//xAuthAgentJID", "");
		if(!xAuthAgentJID.equals(request.getUserJID())) {
			throw new MRException(SessionServiceExceptionCodes.CAN_NOT_AUTHORIZE_CONNECTION_ACCESS_DENY, "Can not authorize the connection access denied");
		}
		
		
		String streamNID = MRXMLUtils.getAttributeValue(streamDoc.getRootElement(), SessionServiceConst.NID,"");
		String profileNID = MRXMLUtils.getValueXpath(streamDoc,"//profileNID", "");
		Element profileXML = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(streamDoc, "//profileXML"));	
		
		
		ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, 
				request.getRequestName(), roomNID, streamNID,  profileNID,  profileXML, MediaRoomServiceConst.AUTHORIZE,action);
	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document requestDoc = request.getMessage().getDocument();
		Element authorizeEl = MRXMLUtils.getElementXpath(requestDoc, "//"+MediaRoomServiceConst.AUTHORIZE);
		String sessionId = MRXMLUtils.getAttributeValue(authorizeEl, SessionServiceConst.SESSION_ID, "");
		String connId = MRXMLUtils.getAttributeValue(authorizeEl, SessionServiceConst.CONN_ID, "");
		String action = MRXMLUtils.getAttributeValue(authorizeEl, SessionServiceConst.ACTION);		
		
		if(sessionId.isEmpty() || connId.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Session Id and Conn Id required");
		}
		
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);	
		Element connectionEl = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='"+connId+"']");
		if(connectionEl == null) {
			throw new MRException(SessionServiceExceptionCodes.CONNECTION_NOT_FOUND, "Connection Not found");
		}
		
		this.authorizeConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roomNID, connId, action);
		String authEntityId = MRXMLUtils.getAttributeValue(connectionEl, SessionServiceConst.AUTH_ENTITY_ID, "");
		Element pvrConnectionEl = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@type='"+SessionServiceConst.PVR+
				"' and @" + SessionServiceConst.AUTH_ENTITY_ID + "='" + authEntityId + "']");
		if(pvrConnectionEl != null) {
			// String s = MRXMLUtils.elementToString(pvrConnectionEl);
			String pvrConnId = MRXMLUtils.getAttributeValue(pvrConnectionEl, SessionServiceConst.ID, "");
			this.authorizeConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roomNID, pvrConnId, action);
		}
		
		//get updated session doc
		sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		connectionEl = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='"+connId+"']");
		Element connectionInfoEl = AppServerAgentUtils.getResourceManager(serviceAgentFactory).generateAuthConnectionInfoElement(request, xmpp, log, db, connectionEl, sessionId, sessionDoc, roomNID);
	    Element authorizeResponseEl = MRXMLUtils.newElement(MediaRoomServiceConst.AUTHORIZE);
	    MRXMLUtils.addElement(authorizeResponseEl,connectionInfoEl);
	    return  request.createResponseData(authorizeResponseEl);
	}

	

}
