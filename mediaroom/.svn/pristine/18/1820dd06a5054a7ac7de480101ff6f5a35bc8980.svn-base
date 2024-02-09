package com.ipvs.sessionservice.session.handler;

import java.util.List;
import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.policyadminservice.impl.SessionPolicyUtils;
import com.ipvs.sessionservice.impl.Session;
import com.ipvs.sessionservice.impl.SessionAuditEventUtils;
import com.ipvs.sessionservice.impl.SessionManager;
import com.ipvs.sessionservice.impl.SessionMediaRoomUtils;
import com.ipvs.sessionservice.impl.SessionResourceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateSessionUtils;
import com.ipvs.utils.Utils;

public abstract class UpdateSessionRequestHandler implements MRRequestHandlerI,EventLoggerI {

	protected abstract void addSourcesAndConnections(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,
			Document startSessionRequestDoc, String sessionId, String connId, Element updateElement, String dstEntityId) throws Exception;	
	
	protected abstract String addSrcEntityToLiveSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc, String sessionId, String roomNID) throws Exception;
	
	protected boolean addingNewSources(Document requestDoc) throws Exception {
		Element srcPortElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.SRCPORT);
		if(srcPortElement == null)
			return false;
		String portNID = srcPortElement.attributeValue("portNID");
		String portTitle = srcPortElement.attributeValue("portTitle");
		return  ( (portNID != null) && (!"".equals(portNID)) ) ||
		        ( (portTitle != null) && (!"".equals(portTitle)) );
	}

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
	
	
	

	protected String addDstEntityToLiveSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc, String sessionId, String roomNID,String srcEntityId) throws Exception {
        return UpdateSessionUtils.addDstPortToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, roomNID);		
	}
	
	protected Element createConnection(String connId, String dstEntityId, String srcEntityId,
			String connName, Document sessionDoc, String canvasId, String windowId) throws Exception {
	   return UpdateSessionUtils.createConnection(connId, dstEntityId, srcEntityId, connName, sessionDoc, canvasId, windowId, "");
	}
		
	public void updateProfileInfo(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			String sessionId, Element updateElement,Document startSessionRequestDoc) throws MRException,Exception {
		
		Element profileEL= 	getProfileElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, updateElement);
		if(profileEL == null) {
			return;
		}
		
		MRXMLUtils.addElement(MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.PROFILE_LIST),profileEL);
		String profileId = MRXMLUtils.getAttributeValue(profileEL, SessionServiceConst.ID);
		
		//update profile id in connection
		List<?> connectionList = MRXMLUtils.getListXpath(startSessionRequestDoc,"//" +SessionServiceConst.CONNECTION);
		Element connEL = null;
		for(Object object:connectionList) {
			connEL = (Element) object ;
			MRXMLUtils.setAttributeValue(connEL, SessionServiceConst.PROFILE_ID, profileId);
		}
	}
	
	protected Element  getProfileElement(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Element updateElement) throws MRException,Exception {
		
		//check user requested any profile or not
		String profileNID = MRXMLUtils.getAttributeValueXpath(updateElement,"./"+SessionServiceConst.PROFILE,SessionServiceConst.PROFILE_NID,"");
		String profileTitle = MRXMLUtils.getAttributeValueXpath(updateElement,"./"+SessionServiceConst.PROFILE,SessionServiceConst.PROFILE_TITLE,"");
		if(profileNID.isEmpty() && profileTitle.isEmpty()) {
			return null;
		}
		
		String profileId = MRXMLUtils.getAttributeValueXpath(updateElement,"./Profile","id",""); 
		if(profileId.isEmpty()) {
			profileId = Utils.getUUID();
		}
		
		//add profile element to profile list
		Element profileEL = MRXMLUtils.addElement(SessionServiceConst.PROFILE);
		MRXMLUtils.addAttribute(profileEL, SessionServiceConst.ID, profileId);
		MRXMLUtils.addAttribute(profileEL, SessionServiceConst.PROFILE_NID, profileNID);
		MRXMLUtils.addAttribute(profileEL, SessionServiceConst.NAME, profileTitle);
		return profileEL;
	}

	Document handleStartNewSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document doc, String sessionId, String connId, Element updateElement) throws Exception {
        log.addInfo("UpdateSessionRequestHandler:handleStartNewSession sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		Document sessionTemplateDoc = Session.getSessionTemplate(log);
		sessionTemplateDoc.getRootElement().addAttribute(SessionServiceConst.GC, MRConst.TRUE);
		Document startSessionRequestDoc = MRXMLUtils.stringToDocument("<StartSession/>");
		startSessionRequestDoc.getRootElement().addAttribute(SessionServiceConst.SESSION_ID, sessionId);
		startSessionRequestDoc.getRootElement().add((Element)sessionTemplateDoc.getRootElement().clone());
		startSessionRequestDoc.getRootElement().addAttribute(SessionServiceConst.GC, MRConst.TRUE);
		
		//XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());
		XMPPTransaction.startBlock("UpdateSessionRequestHandler");
		XMPPTransaction.startBlock("CreatePortEntity");
		// Create the Dest Entity
		Element entity = UpdateSessionUtils.createPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				doc, sessionId, SessionServiceConst.DST_PORT);
		XMPPTransaction.endBlock("CreatePortEntity");
		Element entityList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.ENTITYLIST);
		entityList.add((Element)entity.clone());
		String dstEntityId = entity.attributeValue(SessionServiceConst.ID);
		
		XMPPTransaction.startBlock("addSourcesAndConnections");
		// Create the Src Entities and Connections
		this.addSourcesAndConnections(request, xmpp, log, db, resourceFactory, serviceAgentFactory, doc,
				startSessionRequestDoc, sessionId, connId, updateElement, dstEntityId);
		XMPPTransaction.endBlock("addSourcesAndConnections");
		
		XMPPTransaction.startBlock("updateProfileInfo");
		//update profile in connection if user requested.
		updateProfileInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId, updateElement, startSessionRequestDoc);
		XMPPTransaction.endBlock("updateProfileInfo");
		
		XMPPTransaction.startBlock("startSessionRequest");
		// Start the session
		MRRequest startSessionRequest = MRRequest.createRequest(request, startSessionRequestDoc, 
				request.getRequestName(), request.getServiceName());
		StartSessionRequestHandler handler = new StartSessionRequestHandler();
		XMPPTransaction.endBlock("startSessionRequest");
		
		Document responseDoc =  handler.startSession(startSessionRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		XMPPTransaction.endBlock("UpdateSessionRequestHandler");
		//XMPPTransaction.setDEBUG(false, Thread.currentThread().getId());
		
		//copy start session request to user request 
		request.setCache(startSessionRequest.getCache());
		
		return responseDoc;
	}


	private void handleUpdateSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc,String sessionId,String connId, Element updateElement) throws Exception {
		
		String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}	
		
		String srcEntityId = updateElement.attributeValue(SessionServiceConst.SRCENTITYID, "");
		String dstEntityId = updateElement.attributeValue(SessionServiceConst.DSTENTITYID, "");
		String connName = updateElement.attributeValue(SessionServiceConst.NAME, "");
	    String sessionName = updateElement.attributeValue(SessionServiceConst.SESSION_NAME, "");

	    String canvasId = updateElement.attributeValue(SessionServiceConst.CANVASID, "");
	    String windowId = updateElement.attributeValue(SessionServiceConst.WINDOWID, "");
	    String profileId = updateElement.attributeValue(SessionServiceConst.PROFILE_ID, "");

		SessionManager sm = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
		Document sessionDoc = sm.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		SessionPolicyUtils.enforcePoliciesOnUpdateSession(request,xmpp,log,db,resourceFactory,serviceAgentFactory,sessionDoc,updateElement,srcEntityId,dstEntityId);		
		
		try {
			// SourceEntity Processing
			if("".equals(srcEntityId)){
				// Create SrcEntity(gcFlag = true)
			    srcEntityId = this.addSrcEntityToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
			    		requestDoc, sessionId, roomNID);
				sessionDoc = sm.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
				
			}
			else {
				SessionMediaRoomUtils.validateSrcEntityId(sessionDoc, srcEntityId);
			}
			
			// DestEntity Processing
			if("".equals(dstEntityId)) {
				// Create DstEntity(gcFlag = true)
			  dstEntityId = this.addDstEntityToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
			    		requestDoc, sessionId,  roomNID,srcEntityId);
				sessionDoc = sm.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
				
			}
			else {
				SessionMediaRoomUtils.validateDstEntityId(sessionDoc, dstEntityId);
			}
			
			//check for user requested profile
			if(!profileId.isEmpty()){
				SessionMediaRoomUtils.validateProfileId(sessionDoc, profileId);
			} else {
				Element profileEl = getProfileElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  updateElement);
				if(profileEl != null) {
					sm.addProfile(roomNID, profileEl);
					profileId = MRXMLUtils.getAttributeValue(profileEl, SessionServiceConst.ID,"");
				}
			}
		
			//update session name if requested session name is not empty	     
			if(!sessionName.isEmpty()) {	
			    sm.updateSessionName(sessionId, sessionName);
	        }
			
			//update session Type if the session type is empty in the session doc 
	        String sessionType = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.TYPE_ATTRIBUTE,"");
	        
	    	if(sessionType.isEmpty()) {        		
	     	   sessionType = SessionServiceConst.LIVE;
	     	   sm.updateSessionType(sessionId, sessionType);
	     	} 
			
			// Connection Processing
			Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");
			if(connection == null) {
				connection = UpdateSessionUtils.createConnection(connId, dstEntityId, srcEntityId, connName, sessionDoc, canvasId, windowId,profileId);
				// System.out.println("connection=" + MRXMLUtils.elementToString(connection));
		        SessionMediaRoomUtils.addConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connection, sessionId);
				// Start Connection
			}
			else {
				String connectionType = SessionResourceUtils.getConnectionType(connection, dstEntityId, srcEntityId, sessionDoc);
				
				// Switch Connection
				SessionMediaRoomUtils.switchConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connId, 
						srcEntityId, dstEntityId, sessionId, connection,connectionType,connName,canvasId,windowId,profileId);
				
			}
		}
		catch(Exception e) {
			sessionDoc = sm.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
			Document roomDoc = RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc();
			SessionMediaRoomUtils.gcSrcEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					sessionDoc, srcEntityId, roomNID, roomDoc, sessionId);
			SessionMediaRoomUtils.gcDstEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					sessionDoc, dstEntityId, roomNID, roomDoc);
			throw e;
		}
	}
	
	protected Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String updateElementName) throws Exception {
		
		request.setCache(new MRRequestCache());
		Document requestDoc = request.getMessage().getDocument();
		
		
		// System.out.println("UpdateSessionRequestHandler:getResponse " + MRXMLUtils.documentToString(doc));
		Element updateElement = MRXMLUtils.getElementXpath(requestDoc, "//" + updateElementName);
		String sessionId = updateElement.attributeValue(SessionServiceConst.SESSION_ID, "");
		String connId = updateElement.attributeValue(SessionServiceConst.ID, "");
		if(sessionId.isEmpty())
			sessionId = Utils.getUUID();		
		if(connId.isEmpty())
			connId = Utils.getUUID();			
	
		//update connid information in logger document
		request.setLoggingDocument(MRXMLUtils.stringToDocument("<Log "+SessionServiceConst.CONN_ID+"='"+connId+"'/>"));
		SessionManager sm = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
		
		boolean restartSession = false;
		String restartSessionString = updateElement.attributeValue(SessionServiceConst.RESTART_SESSION, "");
		if(MRConst.TRUE.equals(restartSessionString))
			restartSession = true;

		if(addingNewSources(requestDoc) && !restartSession && (sm.getRoomNID(sessionId) != null))
			throw new MRException(SessionServiceExceptionCodes.NOT_SUPPORTED,
						"Adding source File/Port to existing session not supported");
		
		Document responseDoc = null;
		// Document sessionDoc = null;
		Exception exp = null;
		
		// Turn on the debug if it is given sessionId
		// try {if(316 == Integer.parseInt(sessionId))	XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());}  catch(Exception e) {}
		
		XMPPTransaction.startBlock("UpdateSessionRequestHandler:getResponse");
		XMPPTransaction.startBlock("UpdateSessionRequestHandler:getLock");
		long now = System.currentTimeMillis();
		Semaphore lock = IPVSMonitor.getMonitor().getLock(sessionId, request.getNID());
		long duration = System.currentTimeMillis() - now;
		XMPPTransaction.endBlock("UpdateSessionRequestHandler:getLock");
        log.addInfo("UpdateSessionRequestHandler:getResponse got lock for=" + request.getNID() + " duration=" + duration + " sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		XMPPTransaction.startBlock("UpdateSessionRequestHandler:startSession");
		try {
			if((sm.getRoomNID(sessionId) != null) && restartSession)
				SessionMediaRoomUtils.stopViewSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId);
			if(sm.getRoomNID(sessionId) == null) {
				responseDoc = this.handleStartNewSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, connId, updateElement);
			}
			else {
				this.handleUpdateSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc,sessionId,connId, updateElement);
				Document sessionDoc = sm.getSessionDocWithMediaRoomInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
				responseDoc = request.createResponseData(sessionDoc.getRootElement());
			}
		}
		catch(Exception e) {
			exp = e;
		}
		XMPPTransaction.endBlock("UpdateSessionRequestHandler:startSession");
		XMPPTransaction.startBlock("UpdateSessionRequestHandler:releaseLock");
		IPVSMonitor.getMonitor().releaseLock(sessionId, lock);
		XMPPTransaction.endBlock("UpdateSessionRequestHandler:releaseLock");
		XMPPTransaction.endBlock("UpdateSessionRequestHandler:getResponse");

		// Turn off the debug if it is given sessionId
		// try {if(316 == Integer.parseInt(sessionId))	XMPPTransaction.setDEBUG(false, Thread.currentThread().getId());}  catch(Exception e) {}

		log.addInfo("UpdateSessionRequestHandler:getResponse released lock for=" + request.getNID() + " sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		if(exp != null)
			throw exp;

		return responseDoc;
	}
	
	
	
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, String status, String level, String errorDescription) throws Exception {
		String eventType = MRRequest.getRequestRootName(request.getRequestName());		
		if(request.getException()!=null) {
			return new AuditEvent.Builder(eventType,AuditEventLogger.NAME_SOURCE, request.getUserJID(), level).status(status).
			descriptionText(errorDescription).operatorEl(request.getOperatorGroups()).build();
		}
		
		String connId = MRXMLUtils.getAttributeValue(request.getLoggingDocument().getRootElement(), SessionServiceConst.CONN_ID, "");
		Element connectionElement = MRXMLUtils.getElementXpath(request.getResult(), "//Connection[@id='"+connId+"']");	
		if(connectionElement == null) {
			return null;
		}
		
		return SessionAuditEventUtils.getSourceBasedAuditEventUsingConnectionElement(request.getResult(), connectionElement,request, log);
	}	

}
