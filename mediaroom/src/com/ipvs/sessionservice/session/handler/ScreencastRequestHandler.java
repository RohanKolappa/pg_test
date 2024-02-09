package com.ipvs.sessionservice.session.handler;

import java.util.ArrayList;
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
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.sessionservice.impl.Session;
import com.ipvs.sessionservice.impl.SessionManager;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateSessionUtils;

public class ScreencastRequestHandler extends ViewRequestHandler {

	/*
	 * Start(sessionId, portTitle) : setPortType(portTitle); startSession
	 * View(sessionId, portTitle) : view(sessionId, srcEntityId)
	 * Share(sessionId, portTitle) : 
	 * 		kick(prevSrc); kick(portTitle);
	 * 		setPortType(prevSrc, dst); setPortType(portTitle, src);
	 * 		invite(portTitle, src)
	 * 		view(sessionId, [dstEntityId])
	 * Quit(sessionId, portTitle) : if(current=portTitle) stopSession else kick(portTitle)
	 * @see com.ipvs.sessionservice.handler.UpdateSessionRequestHandler#authorizeRequest(com.ipvs.common.service.MRRequest, com.ipvs.common.XMPPI, com.ipvs.common.MRLog, com.ipvs.common.service.MRDB, com.ipvs.common.factory.MRResourceFactoryI, com.ipvs.common.factory.ServiceAgentFactoryI)
	 * 
	 * 
	 * <login ip='10.1.5.190' user='apiuser' password='Device123' resource='api'/>
<iq><query><ServiceLogin clientVersion='2.28.11'/></query></iq>
<iq><query><GetSession selectFormat='ADVANCED'/></query></iq>
<iq><query><SetPortIOType portTitle='boston' ioType='StreamSrc'/></query></iq>
<iq><query><SetPortIOType portTitle='boston' ioType='StreamDst'/></query></iq>
<iq><query><ScreenShare sessionId='sunnyvale' portTitle='sunnyvale' whereTextSubStr='CHANNEL'/></query></iq>
<iq><query><Kick sessionId='sunnyvale' entityId='sunnyvale_CHANNEL1.SrcPort'/></query></iq>
<iq><query><Kick sessionId='sunnyvale' entityId='boston_CHANNEL1.DstPort'/></query></iq>
<iq><query><Invite sessionId='sunnyvale'><SrcPort id='sunnyvale_CHANNEL1.SrcPort' portTitle='sunnyvale_CHANNEL1' pvrEnable='true'/></Invite></query></iq>
<iq><query><Invite sessionId='sunnyvale'><DstPort id='boston_CHANNEL1.DstPort' portTitle='boston_CHANNEL1'/></Invite></query></iq>
<iq><query><View sessionId='sunnyvale' id='sunnyvale-boston' srcEntityId='sunnyvale_CHANNEL1.SrcPort' dstEntityId='boston_CHANNEL1.DstPort'/></query></iq>
<iq><query><View sessionId='sunnyvale' id='sunnyvale.RecordingConnection' srcEntityId='sunnyvale_CHANNEL1.SrcPort' dstEntityId='sunnyvale.Recording'/></query></iq>
<iq><query><StopSession sessionId='sunnyvale'/></query></iq>
<iq><query><GetRequestRequest><ObjectList type='MediaRoom'/></GetRequestRequest></query></iq>

	 */
	@Override
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
	
	void handleStart(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portTitle) {
		// Send the message to the corresponding device to set the ioType
	}
	void handlePlayback(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portTitle, String fileNID) {
		// Send the message to the corresponding device to set the ioType
	}

	void handleJoin(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portTitle) {
		// Send the message to the corresponding device to set the ioType
	}
	void handleShare(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portTitle) {
		// Send the message to the corresponding device to set the ioType
	}
	void handleLeave(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portTitle) {
		// Send the message to the corresponding device to set the ioType
	}
	void handleBookmark(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portTitle) {
		// Send the message to the corresponding device to set the ioType
	}
	void handleJump(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portTitle, String bookmarkNID) {
		// Send the message to the corresponding device to set the ioType
	}
	void setPortIOType(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String portNID, String ioType) {
		// Send the message to the corresponding device to set the ioType
	}

	private List<?> getPortList(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String whereTextSubStr) throws Exception {
		Document getPortRequestDoc = MRXMLUtils.stringToDocument("<GetPort whereTextSubStr='" + whereTextSubStr + "'/>");
		MRRequest getPortRequest = MRRequest.createRequest(request, getPortRequestDoc, 
				request.getRequestName(), request.getServiceName());
		GetPortRequestHandler handler = new GetPortRequestHandler();
		Document response = handler.getResponse(getPortRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		return MRXMLUtils.getListXpath(response, "//GetPort/Port");
	}
	
	Document createScreenShareSessionDoc(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element screenShareElement) throws Exception {
		String sourceTitle = screenShareElement.attributeValue("portTitle", null);
		if( (sourceTitle == null) || sourceTitle.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, "Missing portTitle");
		}
		
		Document sessionTemplateDoc = Session.getSessionTemplate(log);
		String srcEntityId = "";
		List<String> dstEntityIdList = new ArrayList<String>();

		// Get all the IO ports 
		String whereTextSubStr = screenShareElement.attributeValue("whereTextSubStr", null);
		List<?>portList = this.getPortList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, whereTextSubStr);
		Element portElement = MRXMLUtils.stringToElement("<Port portTitle='' portNID=''/>");
		for(Object o : portList) {
			Element port = (Element)o;
			// Add them all to the session as both source and dest
			String portTitle = port.attributeValue("title");
			String portNID = port.attributeValue("NID");
			portElement.addAttribute("portTitle", portTitle);
			portElement.addAttribute("portNID", portNID);
			String resourceType = SessionServiceConst.DST_PORT;
			String ioType = MediaRoomServiceConst.STREAMDST;
			if(portTitle.contains(sourceTitle)) {
				ioType = MediaRoomServiceConst.STREAMSRC;
				resourceType = SessionServiceConst.SRC_PORT;
			}
			// Set the portIOType if required
			if(!ioType.equals(port.attributeValue(SessionServiceConst.PORT_TYPE)))
				this.setPortIOType(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNID, ioType);
			
			// Add the portEntity
			String id = portTitle + "." + resourceType;
			portElement.addAttribute("id", id);
			Element entity = UpdateSessionUtils.createPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portElement, resourceType);
	        entity.addAttribute(SessionServiceConst.GC,MRConst.TRUE);
			if(SessionServiceConst.SRC_PORT.equals(resourceType)) {
				entity.add(MRXMLUtils.stringToElement("<PVR enable='" + MRConst.TRUE + "' id='" + sourceTitle + ".PVR'/>"));
				srcEntityId = id;
			}
			else 
				dstEntityIdList.add(id);

			Element entityList = MRXMLUtils.getElementXpath(sessionTemplateDoc, "//" + SessionServiceConst.ENTITYLIST);
			entityList.add((Element)entity.clone());

		}

		// Add the recording Entity
		String id = sourceTitle + ".Recording";
		Element entityList = MRXMLUtils.getElementXpath(sessionTemplateDoc, "//" + SessionServiceConst.ENTITYLIST);
		entityList.add(MRXMLUtils.stringToElement(
				"<Entity id='" + id + "' type='RecFile'><File resourceTitle='" + sourceTitle + "'/></Entity>"));
		dstEntityIdList.add(id);

		// Connect all the dest to the source corresponding to the portTitle
		for(String dstEntityId : dstEntityIdList) {
			String connId = dstEntityId + "Connection";
			String connName = connId;
			String canvasId = "";
			String windowId = "";
			Element connection = this.createConnection(connId, dstEntityId, srcEntityId, connName, sessionTemplateDoc, canvasId, windowId);
			Element connectionList = MRXMLUtils.getElementXpath(sessionTemplateDoc, "//" + SessionServiceConst.CONNECTIONLIST);
			connectionList.add((Element)connection.clone());
		}
				
		return sessionTemplateDoc;
	}

	public Document getResponse1(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		request.setCache(new MRRequestCache());
		Document doc = request.getMessage().getDocument();
		Element screenShareElement = MRXMLUtils.getElementXpath(doc, "//ShareRequest");
		String sessionId = screenShareElement.attributeValue("sessionId", null);
		Document sessionDoc = null;
		SessionManager sm = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
		if((sm.getRoomNID(sessionId) == null)) {
			Document sessionTemplateDoc = this.createScreenShareSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, screenShareElement);
			sessionTemplateDoc.getRootElement().addAttribute(SessionServiceConst.GC, MRConst.TRUE);
			Document startSessionRequestDoc = MRXMLUtils.stringToDocument("<StartSession/>");
			startSessionRequestDoc.getRootElement().addAttribute(SessionServiceConst.SESSION_ID, sessionId);
			startSessionRequestDoc.getRootElement().add((Element)sessionTemplateDoc.getRootElement().clone());
			startSessionRequestDoc.getRootElement().addAttribute(SessionServiceConst.GC, MRConst.TRUE);
			// Start the session
			MRRequest startSessionRequest = MRRequest.createRequest(request, startSessionRequestDoc, 
					request.getRequestName(), request.getServiceName());
			StartSessionRequestHandler handler = new StartSessionRequestHandler();
			sessionDoc = handler.getResponse(startSessionRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		}
		else {
			sessionDoc = sm.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		}
		/*
		String connId = screenShareElement.attributeValue("connId", null);
		String newSrcEntityId = screenShareElement.attributeValue("newSrcEntityId", null);
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory,sessionId);
		Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");
		if(connection == null) {
			throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, "Connection not found connId=" + connId);
		}
		SessionMediaRoomUtils.validateSrcEntityId(sessionDoc, newSrcEntityId);
		
		if(!SessionResourceUtils.isUsedInConnection(sessionDoc, newSrcEntityId)) { // Add the mediaSource
			String roomNID = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getRoomNID(sessionId);
			SessionMediaRoomUtils.createMediaSource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					newSrcEntityId, sessionDoc, roomNID);
		}
		
		String newDstEntityId = connection.attributeValue(SessionServiceConst.DSTENTITYID);
		String connectionType=  SessionResourceUtils.getConnectionType(connection, newDstEntityId, newSrcEntityId, sessionDoc);
		String connectionName = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.NAME,"");
		String canvasId = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.CANVASID,"");
		String windowId = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.WINDOWID,"");
		String profileId = ""; // Profile Id not supported in SwitchSrc
		
		connection = SessionMediaRoomUtils.switchConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				connId, newSrcEntityId, newDstEntityId, sessionId, connection,connectionType,connectionName,canvasId,windowId,profileId);
		
		return request.createResponseData((Element)connection.detach());
		*/
		sessionDoc = sm.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		return request.createResponseData(sessionDoc.getRootElement());

	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		request.setCache(new MRRequestCache());
		Document doc = request.getMessage().getDocument();
		
		Element screencastElement = MRXMLUtils.getElementXpath(doc, "//Screencast");
		String sessionId = screencastElement.attributeValue("sessionId", null);
		String action = screencastElement.attributeValue("action", null);
		String portTitle = screencastElement.attributeValue("portTitle", null);
		String fileNID = screencastElement.attributeValue("fileNID", null);
		String bookmarkNID = screencastElement.attributeValue("bookmarkNID", null);
		
		if("start".equals(action))
			this.handleStart(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portTitle);
		else if("playback".equals(action))
			this.handlePlayback(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portTitle, fileNID);
		else if("join".equals(action))
			this.handleJoin(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portTitle);
		else if("share".equals(action))
			this.handleShare(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portTitle);
		else if("leave".equals(action))
			this.handleShare(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portTitle);
		else if("bookmark".equals(action))
			this.handleBookmark(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portTitle);
		else if("jump".equals(action))
			this.handleJump(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portTitle, bookmarkNID);
		else
			throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Screencast action not found action=" + action);			

		return request.createResponseData(MRXMLUtils.stringToElement("<Screencast/>"));
	}
}
