package com.ipvs.sessionservice.session.handler;

import java.util.List;

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
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.sessionservice.impl.SessionManager;
import com.ipvs.sessionservice.impl.SessionMediaRoomUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;

public class KickRequestHandler implements MRRequestHandlerI{

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

	void deleteSrcEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc, String entityId,
			String roomNID, Document roomDoc, String sessionId) throws Exception {
		// System.out.println("KickRequestHandler:deleteSrcEntity roomdoc=" + MRXMLUtils.documentToString(roomDoc));
		Element entity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + entityId + "']");
		if(entity == null)
			return;

		// Delete the source in the stream track
		Element mediaSource = MRXMLUtils.getElementXpath(roomDoc, "//MediaSource[@id='" + entityId + "']");
		String NID = mediaSource.attributeValue(MediaRoomServiceConst.NID);
		SessionMediaRoomUtils.deleteSrcEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionDoc, entityId, roomNID, roomDoc, sessionId);

		// Delete the record mediasource
		List<?> recordMediaSourceList = MRXMLUtils.getListXpath(roomDoc, "//RecordTrack/mediasourcelist/MediaSource[data/streamTrackMediaSourceNID='" + NID + "']");
		for(Object o : recordMediaSourceList) {
			Element recordMediaSource = (Element)o;
			String recordMediaSourceNID = recordMediaSource.attributeValue(MediaRoomServiceConst.NID);
			RoomDB.getRoomDB().getRoom(roomNID, request, log, db).deleteElement(request, log, db, recordMediaSourceNID);
		}
	}
	
	public void deleteDstEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document sessionDoc, String entityId,
			String roomNID, Document roomDoc) throws Exception {
		SessionMediaRoomUtils.deleteDstEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionDoc, entityId, roomNID, roomDoc);
	}

	
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document doc = request.getMessage().getDocument();
		Element kickElement = MRXMLUtils.getElementXpath(doc, "//Kick");
		String sessionId = kickElement.attributeValue("sessionId", null);
		String entityId = kickElement.attributeValue("entityId", null);
		SessionManager sm = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
		
		// Get the sessionDoc
		String roomNID = sm.getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);

		// Get the Entity
		Element entity = MRXMLUtils.getElementXpath(sessionDoc, ".//Entity[@id ='"+ entityId + "']");
		if(entity == null) {
			throw new MRException(SessionServiceExceptionCodes.OBJECTTYPE_DOES_NOTEXISTS, "Entity not found entityId=" + entityId);
		}
		String connectionRole = SessionServiceConst.DSTENTITYID;
		if(	SessionServiceConst.SRCPORT.equals(entity.attributeValue(SessionServiceConst.TYPE_ATTRIBUTE)) ||
			SessionServiceConst.PLYFILE.equals(entity.attributeValue(SessionServiceConst.TYPE_ATTRIBUTE))) {
			connectionRole = SessionServiceConst.SRCENTITYID;
		}
		
		// Get the list of connections for this entity
		List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, ".//Connection[@"+
				connectionRole + "='"+ entityId +"' and @type!='PVR']");
		
		// Delete the connections
		for(Object o : connectionList) {
			String connId = ((Element)o).attributeValue(SessionServiceConst.ID);
			SessionMediaRoomUtils.deleteConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					connId, sessionId);
		}

		// Delete the entity
		sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		Document roomDoc = RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc();
		if(SessionServiceConst.SRCENTITYID.equals(connectionRole))
			this.deleteSrcEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionDoc, entityId, roomNID, roomDoc, sessionId);
		else
			this.deleteDstEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionDoc, entityId, roomNID, roomDoc);
		
		return request.createResponseData(sm.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId).getRootElement());
	}

}
