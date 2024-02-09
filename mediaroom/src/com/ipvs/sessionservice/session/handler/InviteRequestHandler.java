package com.ipvs.sessionservice.session.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.impl.SessionManager;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateSessionUtils;
import com.ipvs.utils.Utils;

public class InviteRequestHandler implements MRRequestHandlerI {
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
	// Override to set tge GC flag
	/*
	protected Element createPortEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Element  portElement , String resourceType) throws MRException, Exception {
		Element entity = super.createPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				portElement, resourceType);
        entity.addAttribute(SessionServiceConst.GC,MRConst.FALSE);
        return entity;
	}
	*/
	
	private boolean elementExists(Document requestDoc, String elementName){
		try {
			Element port = MRXMLUtils.getElementXpath(requestDoc, "//" + elementName);
			if(port == null)
				return false;
			String id = MRXMLUtils.getAttributeValue(port, SessionServiceConst.ID,"");
			if(id.isEmpty()) {
				MRXMLUtils.setAttributeValue(port, SessionServiceConst.ID,Utils.getUUID());
			}
			return true;
		}
		catch(Exception e) {
			return false;
		}
	}

	boolean getSessionPVREnable(Document sessionDoc, boolean defaultValue) throws Exception {
	    List<?> srcPortList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY + "[@type = '" + SessionServiceConst.SRCPORT + "']");		
	    if(srcPortList.isEmpty())
	    	return defaultValue;
    	Element pvrElement = MRXMLUtils.getElementXpath((Element)srcPortList.get(0), ".//PVR");
    	if(pvrElement == null)
    		return false;
    	return (SessionServiceConst.TRUE.equals(pvrElement.attributeValue("enable", "")));
    }
	
	// Check if the sessionType matches	
	public void validateSessionType(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,String sessionId,String requestedSessionType) throws MRException,Exception {
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		String sessionType = sessionDoc.getRootElement().attributeValue(SessionServiceConst.TYPE_ATTRIBUTE);			
		if(!requestedSessionType.equals(sessionType))
			throw new MRException(SessionServiceExceptionCodes.NOT_SUPPORTED, requestedSessionType +" Sources can only be added to "+requestedSessionType+" sessions");
	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		request.setCache(new MRRequestCache());
		Document requestDoc = request.getMessage().getDocument();
		Element inviteElement = MRXMLUtils.getElementXpath(requestDoc, "//Invite");
		String sessionId = inviteElement.attributeValue("sessionId", null);
		SessionManager sm = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
		
		// Get the sessionDoc
		String roomNID = sm.getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		
		// Add the source Entity to the session
		if(elementExists(requestDoc, SessionServiceConst.SRC_PORT)) {
			
			validateSessionType(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId, SessionServiceConst.LIVE);

			// Check if the sessionPVRType matches
			Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
			Element port = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.SRC_PORT);
			boolean pvrEnable = SessionServiceConst.TRUE.equals(port.attributeValue(SessionServiceConst.PVRENABLE));
			boolean sessionPVREnable = getSessionPVREnable(sessionDoc, pvrEnable);
			if(pvrEnable != sessionPVREnable)
					throw new MRException(SessionServiceExceptionCodes.NOT_SUPPORTED, "Session PVR=" + sessionPVREnable +" does not match source PVR=" + pvrEnable);
			
			UpdateSessionUtils.addSrcPortToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, roomNID);
		} else 	
		// Add the dest Entity to the session
		if(elementExists(requestDoc, SessionServiceConst.DST_PORT)) {
			UpdateSessionUtils.addDstPortToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, roomNID);
		} else 
		// Add the src file Entity to the session
		if(elementExists(requestDoc, SessionServiceConst.SRCFILE)) {
			validateSessionType(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId, SessionServiceConst.PLAYBACK);
			UpdateSessionUtils.addSrcFileToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, roomNID);			
		} else 
		// Add the grp file Entity to the session
		if(elementExists(requestDoc, SessionServiceConst.GRPFILE)) {
			validateSessionType(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId, SessionServiceConst.PLAYBACK);
			UpdateSessionUtils.addGrpFileToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, roomNID);			
		} else {
			throw new MRException(SessionServiceExceptionCodes.NOT_SUPPORTED, "Not Supported");
		}
		
		return request.createResponseData(sm.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId).getRootElement());

	}
}
