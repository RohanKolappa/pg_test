package com.ipvs.sessionservice.impl;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.notification.BaseNotifier;
import com.ipvs.notification.ResourceManager;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public class Session {
	
	Document sessionDoc;
	String sessionID;
    private static Document gSessionTemplate = null;   
    private static Document gConnStateTemplate = null;   
    private static Document gPresenceTemplate = null;    

	
	Session(Document data) {
		this.sessionDoc = data;
		try {
			this.sessionID = MRXMLUtils.getAttributeValue(this.sessionDoc.getRootElement(),SessionServiceConst.ID);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	Document getData() {
		return this.sessionDoc;
	}


    public static Document getSessionTemplate(MRLog log) throws Exception  {
    	if(Session.gSessionTemplate == null) {
    		Session.gSessionTemplate = MRXMLUtils.loadXMLResource(SessionServiceUtils.SESSION_INSTANCE_XML, log);
    	}
    	return (Document)Session.gSessionTemplate.clone();
    }
    
    public static Document getConnStateTemplate(MRLog log) throws Exception  {
    	if(Session.gConnStateTemplate == null) {
    		Session.gConnStateTemplate = MRXMLUtils.loadXMLResource(SessionServiceUtils.CONNSTATE_INSTANCE_XML, log);
    	}
    	return (Document)Session.gConnStateTemplate.clone();
    }
    
    public static Document getPresenceTemplate(MRLog log) throws Exception  {
    	if(Session.gPresenceTemplate == null) {
    		Session.gPresenceTemplate = MRXMLUtils.loadXMLResource(SessionServiceUtils.ENTITYPRESENCE_INSTANCE_XML, log);
    	}
    	return (Document)Session.gPresenceTemplate.clone();
    }


	synchronized void handleSetupError(MRLog log, String id, Exception e) throws Exception {
        Element element = MRXMLUtils.getElementXpath(sessionDoc, "//*[@id='" + id + "']");
        SessionServiceUtils.updateSetupError(log, element, e);
	}

	private void sendConnStateNotification(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID1, String id, int state, Element streamElement, Element mediaSourceEl,ResourceManager resourceManager,SessionNotifier sessionNotifier) throws Exception {
        Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + id + "']");
        if(connection == null)
        	return; // Must have been removed
        
        
        String srcPortNID = MRXMLUtils.getValueXpath(mediaSourceEl, ".//sourceNID", "");
        String dstPortNID = MRXMLUtils.getValueXpath(streamElement, ".//destNID", "");
        
        List<Element> observerList = SessionServiceUtils.getMonitorAgentList(mediaSourceEl, SessionServiceConst.PORT, log);
    	String stateStr = MediaRoomServiceUtils.getStateStr(log, state);   
    	MRXMLUtils.addAttribute(connection,"state", stateStr);
    	String sessionID = MRXMLUtils.getAttributeValue(this.sessionDoc.getRootElement(),SessionServiceConst.ID);
        Document connStateDoc = Session.getConnStateTemplate(log);
		// Document connStateDoc = MRXMLUtils.loadXMLResource(SessionServiceUtils.CONNSTATE_INSTANCE_XML, log);
        
        MRXMLUtils.setAttributeValueXPath(connStateDoc, "//ConnState","sessionId",sessionID);
		MRXMLUtils.setAttributeValueXPath(connStateDoc, "//ConnState","connId",id);
		MRXMLUtils.setAttributeValueXPath(connStateDoc, "//ConnState","value",stateStr);
		
		Element mediaTimeline = MRXMLUtils.getElementXpath(streamElement, ".//MediaTimeline");
		if(mediaTimeline != null) {
			MRXMLUtils.getElementXpath(connStateDoc, "//ConnState").add(SessionServiceUtils.createConnectionTimeline(streamElement, log));
		}
		
		//update startTc for pvr connection
		String lastEventCode = MRXMLUtils.getValueXpath(streamElement, ".//lastEventData//eventCode", "");
		String connType = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.TYPE_ATTRIBUTE,"");
		if(!lastEventCode.isEmpty() && lastEventCode.equals(""+MediaRoomServiceConst.STREAM_START_TC) && connType.equals(SessionServiceConst.PVR)) {
			Element srcEntity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+MRXMLUtils.getAttributeValue(connection, SessionServiceConst.SRCENTITYID,"")+"']");
			Element pvrElement = MRXMLUtils.getElement(srcEntity, SessionServiceConst.PVR);
			String startTc = MRXMLUtils.getValueXpath(streamElement, ".//lastEventData//startTC", "");
			MRXMLUtils.addAttribute(pvrElement, SessionServiceConst.START_TC_ATTRIBUTE, startTc);
		}

		String streamURL = MRXMLUtils.getValueXpath(streamElement, ".//Status/URL", "");
		
		//backward compatibility for applets 
		if(streamURL.isEmpty()) {
			streamURL = MRXMLUtils.getValueXpath(streamElement, ".//lastEventData/streamURL", "");
		}
		
		
		//update stream URL only if available in the stream event data.
		if(!"".equals(streamURL)) {
    		MRXMLUtils.addAttribute(connection, "streamURL", streamURL);
		}
    	if(MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED == state) {
    		streamURL = MRXMLUtils.getAttributeValue(connection, "streamURL","");
	    	MRXMLUtils.setAttributeValueXPath(connStateDoc, "//ConnState","streamURL",streamURL);
    		
    	}
    	else if(MediaRoomServiceConst.STREAM_STOPPED == state) {
    		log.addInfo("Session:handleStreamStatusChanged stopped state=" + state + " deleting connection id=" + id, 
    				MRLog.OK, MRLog.NOTIFY);
    		MRXMLUtils.setAttributeValueXPath(connStateDoc, "//ConnState","value",SessionServiceConst.STOPPED);
    		resourceManager.queueConnectionNotification(xmpp, request, log, db, srcPortNID,dstPortNID, sessionID, 
        			id, sessionDoc, mediaSourceEl, streamElement,observerList, SessionServiceConst.DELETE);
    	}
    	else if(state > 200) { // Error state
    		String exceptionCode = MRXMLUtils.getValueXpath(streamElement, ".//Exception/Code", "");
    		String exceptionDescription = MRXMLUtils.getValueXpath(streamElement, ".//Exception/Description", "");

    		// Update the connection element
    		MRXMLUtils.addAttribute(connection, SessionServiceConst.ERROR_CODE, exceptionCode);
    		MRXMLUtils.addAttribute(connection, SessionServiceConst.ERROR_DATA, exceptionDescription);

    		// Update the ConnState
    		MRXMLUtils.addAttribute(connStateDoc.getRootElement(), SessionServiceConst.ERROR_CODE, exceptionCode);
    		MRXMLUtils.addAttribute(connStateDoc.getRootElement(), SessionServiceConst.ERROR_DATA, exceptionDescription);
;    		
    	}
    	
		
		Element connStateElement = connStateDoc.getRootElement();
    	String sessionmessage = "<x xmlns='" + IPVSMessageExtension.IPVS_SESSION_XMLNS + "'>" +
    								MRXMLUtils.elementToString(connStateElement) +
    							"</x>";
    	/*
    	xmpp.sendRoomMessage(roomNID, sessionmessage,
    			IPVSMessageExtension.IPVS_SESSION_XMLNS);
		*/
    	
    	String notificationId = sessionID + "." + id;
    	Element properties = MRXMLUtils.getElementXpath(sessionDoc, "//Properties");
    	String notificationData = properties.attributeValue("ownerUserJID");
    	if(		(state == MediaRoomServiceConst.END_OF_MEDIA) || 
    			(MediaRoomServiceConst.WAITING_FOR_STREAMSTARTED == state)) {
    		sessionNotifier.expediteNotification(BaseNotifier.ROOM_NOTIFICATION_TYPE, notificationData, notificationId, sessionmessage, sessionID);    		
    	}
    	else {
    		sessionNotifier.queueNotification(BaseNotifier.ROOM_NOTIFICATION_TYPE, notificationData, notificationId, sessionmessage, sessionID);
    	}
    	
    	String srcObjectType = MRXMLUtils.getValueXpath(mediaSourceEl, ".//sourceType","");
		String dstObjectType = MRXMLUtils.getValueXpath(streamElement, ".//destType","");
		
					
		resourceManager.queueConnectionStateNotification(xmpp, request, log, db, srcPortNID, dstPortNID, srcObjectType, dstObjectType, sessionID, connStateElement, 
        	observerList, id);
		
}
	
	private void sendPresenceNotification(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, String id, String jid, String presence, Element mediaElement, SessionNotifier sessionNotifier) throws Exception{
		
		Document presenceDoc = Session.getPresenceTemplate(log);
		MRXMLUtils.setAttributeValueXPath(presenceDoc, "//EntityPresence","sessionId",this.sessionID);
		MRXMLUtils.setAttributeValueXPath(presenceDoc, "//EntityPresence","entityId",id);
		MRXMLUtils.setAttributeValueXPath(presenceDoc, "//EntityPresence","jid",jid);
		MRXMLUtils.setAttributeValueXPath(presenceDoc, "//EntityPresence","value",presence);
		
		Element presenceElement = presenceDoc.getRootElement();
		sendSessionMessageToRoom(xmpp, presenceElement, sessionNotifier);    	
    }

	synchronized void handleStreamStatusChanged(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, Element streamElement, Element sourceElement,ResourceManager resourceManager,SessionNotifier sessionNotifier) throws Exception {
		String id = MRXMLUtils.getAttributeValue(streamElement,MediaRoomServiceConst.ID);
		if("".equals(id))
			return;		
        int state = Integer.parseInt(MRXMLUtils.getValueXpath(streamElement, ".//Status/state", ""));
		XMPPTransaction.startBlock("sendConnStateNotification");
		this.sendConnStateNotification(xmpp, request, log, db, roomNID, id, state, streamElement, sourceElement,resourceManager,sessionNotifier);
		XMPPTransaction.endBlock("sendConnStateNotification");
	}
	

	synchronized void handlePresenceChanged(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, Element mediaElement, SessionNotifier sessionNotifier) throws Exception {
		String id = MRXMLUtils.getAttributeValue(mediaElement,MediaRoomServiceConst.ID);
		if("".equals(id)) {
			// System.out.println("Session:handlePresenceChanged entity not found mediaElement: " + MRXMLUtils.elementToString(mediaElement));
			return;
		}
        Element entity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + id + "']");
        String presence = MRXMLUtils.getAttributeValue(mediaElement,MediaRoomServiceConst.PRESENCE);
        
        String jid = "";
        String mediaStr = MRXMLUtils.elementToString(mediaElement);
        Document mediaDoc = MRXMLUtils.stringToDocument(mediaStr);
        
        if("MediaSource".equals(mediaElement.getName())){
        	jid = MRXMLUtils.getValueXpath(mediaDoc, "//sourceAgentJID", "");
        }
        else if("MediaDest".equals(mediaElement.getName())){
        	jid = MRXMLUtils.getValueXpath(mediaDoc, "//destAgentJID", "");
        }
        else{
        	jid = "";
        }
        
        MRXMLUtils.addAttribute(entity,MediaRoomServiceConst.PRESENCE, presence);        
        MRXMLUtils.addAttribute(entity,MediaRoomServiceConst.JID, jid);        
        this.sendPresenceNotification(xmpp, request, log, db, roomNID, id, jid, presence, mediaElement, sessionNotifier);
	}
	
	synchronized void handleStreamDeleted(XMPPI  xmpp, MRRequest request, MRLog log, MRDB db, String roomNID, 
			Element streamElement, Element mediaSourceEl, int state,ResourceManager resourceManager,SessionNotifier sessionNotifier) throws Exception {
		String id = MRXMLUtils.getAttributeValue(streamElement, MediaRoomServiceConst.ID, "");
		if("".equals(id))
			return;		
		this.sendConnStateNotification(xmpp, request, log, db, roomNID, id, state, streamElement, mediaSourceEl, resourceManager,sessionNotifier);
    }
	
	synchronized void deleteConnection(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, String connId,ResourceManager resourceManager,SessionNotifier sessionNotifier) throws Exception {
		this.sendConnStateNotification(xmpp, request, log, db, roomNID, connId, MediaRoomServiceConst.STREAM_DELETED, null, null,resourceManager,sessionNotifier);
        Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");		
        connection.detach();
	}

	synchronized void addEntity(Element entity) throws Exception {
		Element entityList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.ENTITYLIST);
		String id = entity.attributeValue(SessionServiceConst.ID);
		if(MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + id + "']") != null) {
        	throw new MRException(SessionServiceExceptionCodes.DUPLICATE_ENTITY, "Duplicate Entity id=" + id);
		}			

		entityList.add((Element)entity.clone());
	}

	synchronized void deleteEntity(String id) throws Exception {
		Element entityList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.ENTITYLIST);
		Element entity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='" + id + "']");
		entityList.remove(entity);
	}

	synchronized void addGrpFile(Element grpFile) throws Exception {
		Element grpFileList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.GRPFILELIST);
		String id = grpFile.attributeValue(SessionServiceConst.ID);
		if(MRXMLUtils.getElementXpath(sessionDoc, "//GrpFile[@id='" + id + "']") != null) {
        	throw new MRException(SessionServiceExceptionCodes.DUPLICATE_ENTITY, "Duplicate Entity id=" + id);
		}			

		grpFileList.add((Element)grpFile.clone());
	}

	synchronized void addConnection(Element connection) throws Exception {
		Element connectionList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.CONNECTIONLIST);
		connectionList.add((Element)connection.clone());
	}
	
	synchronized void addProfile(Element profileEL) throws Exception {
		Element profileList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.PROFILE_LIST );
		if(profileList == null)
			profileList = MRXMLUtils.addElementXpath(sessionDoc, "//" + SessionServiceConst.SESSION, SessionServiceConst.PROFILE_LIST, "");
		profileList.add((Element)profileEL.clone());
	}
	
	synchronized void updateConnectionWindow(String connId, String canvasId, String windowId) throws Exception {
	    Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");

	    if(!windowId.isEmpty()){
			MRXMLUtils.addAttribute(connection, "windowId", windowId);
	    }
	    if(!canvasId.isEmpty()){
			MRXMLUtils.addAttribute(connection, "canvasId", canvasId);
	    }
	}
	
	
	synchronized void updateConnectionAuthEntityId(String connId, String authEntityId) throws Exception {
	    Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");	   
		MRXMLUtils.addAttribute(connection, SessionServiceConst.AUTH_ENTITY_ID, authEntityId);
	}
	
	synchronized void updateConnection(String connId, String newSrcEntityId, String newDstEntityId,String type,String connName,String canvasId,String windowId,String profileId) throws Exception {
	    Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");
	    MRXMLUtils.addAttribute(connection,SessionServiceConst.SRCENTITYID, newSrcEntityId);
	    MRXMLUtils.addAttribute(connection,SessionServiceConst.DSTENTITYID, newDstEntityId);
	    MRXMLUtils.addAttribute(connection,SessionServiceConst.TYPE_ATTRIBUTE, type);
	    MRXMLUtils.addAttribute(connection,SessionServiceConst.STATE, "");
	    MRXMLUtils.addAttribute(connection, SessionServiceConst.ERROR_CODE, "");
	    MRXMLUtils.addAttribute(connection, SessionServiceConst.ERROR_DATA, "");
	    MRXMLUtils.addAttribute(connection, SessionServiceConst.STREAM_URL, "");
	    if(!connName.isEmpty()){
		MRXMLUtils.addAttribute(connection, SessionServiceConst.NAME, connName);
	    }
	    if(!windowId.isEmpty()){
		MRXMLUtils.addAttribute(connection, SessionServiceConst.WINDOWID, windowId);
	    }
	    if(!canvasId.isEmpty()){
		MRXMLUtils.addAttribute(connection, SessionServiceConst.CANVASID, canvasId);
	    }
		MRXMLUtils.addAttribute(connection, SessionServiceConst.PROFILE_ID, profileId);
		Element entityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+newSrcEntityId+"']");
		MRXMLUtils.setAttributeValue(connection, SessionServiceConst.STREAM_TYPE, MRXMLUtils.getAttributeValue(entityEL, SessionServiceConst.STREAM_TYPE, ""));
	}
	
	
	synchronized void updateProfileInfoInConnection(String connId,String profileId) throws Exception {
	   Element connection = MRXMLUtils.getElementXpath(sessionDoc, "//Connection[@id='" + connId + "']");
	   if(!profileId.isEmpty()){
			MRXMLUtils.setAttributeValue(connection, "profileId", profileId);
		}
	}
	
	
	synchronized void updateSessionName(String sessionName) throws MRException,Exception {
		MRXMLUtils.setAttributeValue(sessionDoc.getRootElement(),SessionServiceConst.NAME, sessionName);			
	}
	
	synchronized void updateSessionType(String type) throws MRException,Exception {
		   MRXMLUtils.setAttributeValue(sessionDoc.getRootElement(),SessionServiceConst.TYPE_ATTRIBUTE, type);		
	}
	
	synchronized void addDir(Element dir) throws Exception {
		Element dirList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.DIRLIST);
		String id = dir.attributeValue(SessionServiceConst.ID);
		if(MRXMLUtils.getElementXpath(sessionDoc, "//Dir[@id='" + id + "']") != null) {
        	throw new MRException(SessionServiceExceptionCodes.DUPLICATE_ENTITY, "Duplicate Entity id=" + id);
		}			

		dirList.add((Element)dir.clone());
	}
	
	synchronized void updateProfileInfo( String profileId, String profileNID, String profileTitle, String ffTracks) throws Exception {
	    Element profileEL = MRXMLUtils.getElementXpath(sessionDoc, "//Profile[@id='"+profileId+"']");
	    if(profileEL == null) {
	    	return;
	    }
	   	if(!profileNID.isEmpty())
	   		MRXMLUtils.setAttributeValue(profileEL, SessionServiceConst.PROFILE_NID, profileNID);
		if(!profileTitle.isEmpty())
			MRXMLUtils.setAttributeValue(profileEL, SessionServiceConst.NAME, profileTitle);
		MRXMLUtils.setAttributeValue(profileEL, SessionServiceConst.FF_TRACKS, ffTracks);
	}
	
	synchronized void updatePVRInfo( String srcEntityID, String startTC, String trimLength, String connectionId, String ffTracks, String pvrSessionId) throws Exception {
	    Element pvrEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityID+"']/PVR");
	    if(startTC!=null && !startTC.trim().isEmpty())
	    {
		MRXMLUtils.setAttributeValue(pvrEL, SessionServiceConst.START_TC_ATTRIBUTE, startTC);	
	    }	   
	    if(ffTracks != null && !ffTracks.trim().isEmpty()){
		MRXMLUtils.setAttributeValue(pvrEL, SessionServiceConst.FF_TRACKS, ffTracks);
	    }
	    if(trimLength != null && !trimLength.trim().isEmpty()){

		MRXMLUtils.setAttributeValue(pvrEL, SessionServiceConst.TRIMLENGTH, trimLength);
	    }
	    if(connectionId != null && !connectionId.trim().isEmpty()){
		MRXMLUtils.setAttributeValue(pvrEL, SessionServiceConst.CONN_ID, connectionId);
	    }
	    if(pvrSessionId != null && !pvrSessionId.trim().isEmpty()){
		MRXMLUtils.setAttributeValue(pvrEL, SessionServiceConst.SESSION_ID, pvrSessionId);
	    }
	}	
	
	
	public void sendSessionStateNotification(String roomNID, String value, XMPPI xmpp, MRLog log, SessionNotifier sessionNotifier) throws Exception{
        Document requestDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.SESSION_STATE_UPDATE_XML, log);
        Element sessionStateEl = requestDoc.getRootElement();
        MRXMLUtils.setAttributeValue(sessionStateEl, "sessionId", sessionID);
        MRXMLUtils.setAttributeValue(sessionStateEl, "value", value);
        ArrayList<String> userJIDList = new ArrayList<String>();
        List<?> entityList = MRXMLUtils.getListXpath(this.sessionDoc, "//Entity");
        for(Object entity : entityList) {
        	String jid = ((Element)entity).attributeValue(MediaRoomServiceConst.JID);
        	if((jid != null) && !jid.isEmpty() && !userJIDList.contains(jid))
        		userJIDList.add(jid);
        }        	
        log.addInfo("SendSessionStateNotification. SessionId:="+sessionID+" RoomNID:="+roomNID+" State:="+value + " to=" + userJIDList.toString(),MRLog.INFO,MRLog.NOTIFY);
        SessionServiceUtils.sendSessionMessageToList(xmpp, userJIDList, sessionStateEl, sessionNotifier);
	}
   
    public void sendSessionMessageToRoom(XMPPI xmpp, Element sessionStateEl, SessionNotifier sessionNotifier) throws MRException,Exception {
        ArrayList<String> userJIDList = new ArrayList<String>();
        List<?> entityList = MRXMLUtils.getListXpath(sessionDoc, "//Entity[@type='DstPort']");
        for(Object entity : entityList) {
        	String jid = ((Element)entity).attributeValue(MediaRoomServiceConst.JID);
        	if((jid != null) && !jid.isEmpty() && !userJIDList.contains(jid))
        		userJIDList.add(jid);
        }
    	Element properties = MRXMLUtils.getElementXpath(sessionDoc, "//Properties");
    	String ownerUserJID = properties.attributeValue("ownerUserJID");
    	if(!userJIDList.contains(ownerUserJID))
    		userJIDList.add(ownerUserJID);    		
        SessionServiceUtils.sendSessionMessageToList(xmpp, userJIDList, sessionStateEl, sessionNotifier);
    	// xmpp.sendRoomMessage(roomNID, sessionMessage, IPVSMessageExtension.IPVS_SESSION_XMLNS);
    }
}
