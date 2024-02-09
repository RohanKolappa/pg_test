package com.ipvs.sessionservice.impl;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.ConnectionUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomObserverI;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.notification.ResourceManager;




public class SessionManager implements MediaRoomObserverI {
	
	private Hashtable<String, Session> sessionTable = new Hashtable<String, Session>();
	private Hashtable<String, String>  roomSessionMapping = new Hashtable<String, String>();
	private ResourceManager resourceManager = null;
	private SessionNotifier sessionNotifier = null;
	
	public SessionManager(MRDB db,MRLog log,XMPPF xf, String userJID, ResourceManager resourceManager,AuthTokenCache authTokenCache) {
		this.resourceManager = resourceManager;		
		RoomDB.getRoomDB().addObserver(SessionServiceConst.SESSION,this);
		this.sessionNotifier = new  SessionNotifier(db,log, xf, userJID, authTokenCache);
		Thread sessionNotifierThread = new Thread(sessionNotifier);
		sessionNotifierThread.start();
	}
	

	public void handlePresenceChanged(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, Element mediaElement) {
		try {
			Session session = this.sessionTable.get(roomNID);
			session.handlePresenceChanged(xmpp, request, log, db, roomNID, mediaElement, sessionNotifier);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	public synchronized void handleRoomDeleted(XMPPI xmpp, MRRequestI request, MRLog log,
			MRDB db, String roomNID, Document roomDoc) throws Exception  {	
		Session session = this.sessionTable.get(roomNID);
		if(session == null) {
			return;
		}
		Document sessionDocument = session.sessionDoc.getDocument();
		List<?> connectionList = MRXMLUtils.getListXpath(sessionDocument, "//Connection");
		
		for(Object o : connectionList) {
			Element connectionElement = (Element)o;
			String srcEntityID = MRXMLUtils.getAttributeValue(connectionElement,SessionServiceConst.SRCENTITYID);
			String dstEntityID = MRXMLUtils.getAttributeValue(connectionElement,SessionServiceConst.DSTENTITYID);
			Element streamElement = MRXMLUtils.getElementXpath(roomDoc, "//Stream[@id='"+ MRXMLUtils.getAttributeValue(connectionElement, SessionServiceConst.ID)+"']");
			if(streamElement == null) {
				streamElement = MRXMLUtils.getElementXpath(roomDoc, "//MediaDest[@id='" + dstEntityID + "']");
				if((streamElement != null) && (MRXMLUtils.getAttributeValue(connectionElement, "type").equals(SessionServiceConst.PVR))) {
					String fileNID = MRXMLUtils.getValueXpath(streamElement, ".//destNID", "");
					try {
						log.addInfo("SessionManager:handleRoomDeleted Begin fileNID=" + fileNID, MRLog.OK, MRLog.NOTIFY);
						String requestName = AMConst.ASSET_DELETE_REQUEST;
						MRRequest req = (MRRequest)request;
						MRRequest assetRequest = MRRequest.createRequest(req.getAppServerJID(), req.getLogLevel(), request.getNID(), req.getServiceName(), req.getRequestName(), new MRMessage(req.getMessage().getDocument()), req.getResourceGroup(), req.getAppServerJID(), req.getUserAgentName(), req.getClientData());
						Document mediaStreamFileDoc = MRXMLDBUtils.readElement(assetRequest, log, db, fileNID);
						MediaStoreServiceUtils.sendRequestToMediaStoreAgent(requestName, mediaStreamFileDoc, assetRequest, xmpp, log, db, null, null);
						log.addInfo("SessionManager:handleRoomDeleted End fileNID=" + fileNID, MRLog.OK, MRLog.NOTIFY);
					}  catch(Exception e) {
						log.addInfo("SessionManager:handleRoomDeleted Failed fileNID=" + fileNID + " Exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
					}
				}
			}

			Element mediaSourceEl = MRXMLUtils.getElementXpath(roomDoc, "//MediaSource[@id='" + srcEntityID + "']");
			
			if(mediaSourceEl == null || streamElement == null) {
				continue;
			}
				
            String srcPortNID = MRXMLUtils.getValueXpath(mediaSourceEl, ".//sourceNID","");
            String dstPortNID = MRXMLUtils.getValueXpath(streamElement, ".//destNID","");
            String connId = MRXMLUtils.getAttributeValue(connectionElement, "id", "");
        	List<Element> observerList = SessionServiceUtils.getMonitorAgentList(mediaSourceEl, SessionServiceConst.PORT, log);
        	resourceManager.queueConnectionNotification(xmpp, request, log, db, srcPortNID, dstPortNID, session.sessionID, connId,this.sessionTable.get(roomNID).getData(), mediaSourceEl, streamElement, observerList, SessionServiceConst.DELETE);
        }        
		Document sessionDoc = this.sessionTable.remove(roomNID).getData();		
		String sessionID = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), "id");
		log.addInfo("SessionManager->handleRoomDeleted:: ThreadID=" + Thread.currentThread().getId() + " " + "roomNID=" + roomNID + " " + "sessionID=" + sessionID);
		this.roomSessionMapping.remove(sessionID);
		sessionNotifier.flushNotificationsOnRoomDelete(roomNID);
		session.sendSessionStateNotification(roomNID, SessionServiceConst.DELETED, xmpp, log, sessionNotifier);

	}

	public synchronized void handleSessionStarted(XMPPI xmpp, MRRequestI request, MRLog log,
			MRDB db, String roomNID) throws Exception  {
		// log.addInfo("SessionManager:handleSessionStarted roomNID=" + roomNID);
		Session session = this.sessionTable.get(roomNID);
		if(session == null) {
			return;
		}
		session.sendSessionStateNotification(roomNID, SessionServiceConst.CREATED, xmpp, log, sessionNotifier);
	}

	public void handleStreamStatusChanged(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, Element streamElement, Element sourceElement){
		try {
			Session session = this.sessionTable.get(roomNID);	
			XMPPTransaction.startBlock("sessionHandleStreamStatusChanged");
			session.handleStreamStatusChanged(xmpp, request, log, db, roomNID, streamElement, sourceElement, resourceManager, sessionNotifier);
			XMPPTransaction.endBlock("sessionHandleStreamStatusChanged");
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	public void handleSetupError(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, String id, Exception e) {
		try {
			Session session = this.sessionTable.get(roomNID);
			session.handleSetupError(log, id, e);
		}
		catch(Exception ex) {
			ex.printStackTrace();
		}
	}
	
	public synchronized void  addSession(MRRequest request, MRLog log, MRDB db,
			String roomNID, Document sessionDoc, Document roomDoc) throws Exception {
		//TBD: throw Exception if you found the same session id in the cache list.		
		String sessionID = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), "id");
		log.addInfo("SessionManager->addSession:: ThreadID=" + Thread.currentThread().getId() + " " + "roomNID=" + roomNID + " " + "sessionID=" + sessionID);

		if(this.roomSessionMapping.get(sessionID) != null) {
			throw new MRException(SessionServiceExceptionCodes.DUPLICATE_SESSION,"Duplicate SessionId=" + sessionID);
		}
		Document doc = (Document)sessionDoc.clone();
		this.sessionTable.put(roomNID, new Session(doc));				
		this.roomSessionMapping.put(sessionID,roomNID);		
	}

	public Element getData(MRRequestI request,XMPPI xmpp, 
			MRLog log, MRDB db, String roomNID) throws Exception {
		Session session = this.sessionTable.get(roomNID);
    	if(session == null){
    		throw new MRException(SessionServiceExceptionCodes.NO_SESSION_FOUND,"Session Not Found");
    	}
		return (Element)session.getData().getRootElement().clone();
	}

    public void handleTimeLineChanged(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, 
    		String roomNID, String trackNID, Document trackDataDoc, String requestedAction) {
    	try {
    		Element sessionElement = getData(request, xmpp, log, db, roomNID);
    		String sessionID = MRXMLUtils.getAttributeValue(sessionElement, "id", "");
    		Session session = this.sessionTable.get(roomNID);
	    	Element timeLineElement = SessionServiceUtils.createTimeLine(xmpp, request, log, db, sessionID, trackNID, trackDataDoc, requestedAction);
	    	session.sendSessionMessageToRoom(xmpp, timeLineElement, sessionNotifier);
	    }
    	catch(Exception e) {
    		e.printStackTrace();
    	}
    }
    


   
    public void handleStreamAdded(XMPPI  xmpp, MRRequest request, MRLog log, MRDB db, Element streamElement, 
    		Element mediaSourceElement)throws MRException, Exception{
    	//This method sends notifications to the monitor agent 
    	String roomNID = MRXMLUtils.getValueXpath(mediaSourceElement, ".//roomNID","");
    	String srcPortNID = MRXMLUtils.getValueXpath(mediaSourceElement, ".//sourceNID","");
    	String dstPortNID = MRXMLUtils.getValueXpath(streamElement, ".//destNID","");
    	
    	Session session = this.sessionTable.get(roomNID);
    	if(session == null){
    		throw new MRException(SessionServiceExceptionCodes.NO_SESSION_FOUND,"Session Not Found");
    	}
		String connId = MRXMLUtils.getAttributeValue(streamElement, "id", "");
		List<Element> observerList = SessionServiceUtils.getMonitorAgentList(mediaSourceElement, SessionServiceConst.PORT, log);
		resourceManager.queueConnectionNotification(xmpp, request, log, db, srcPortNID, dstPortNID, session.sessionID, connId,this.sessionTable.get(roomNID).getData(),mediaSourceElement,streamElement, observerList, SessionServiceConst.ADD);
    }

    
    public void handleStreamDeleted(XMPPI  xmpp, MRRequest request, MRLog log, MRDB db, 
    		String roomNID, Element streamElement, Element sourceElement, int state) throws Exception {
    	Session session = this.sessionTable.get(roomNID);
    	session.handleStreamDeleted(xmpp, request, log, db, roomNID, streamElement, sourceElement, state,resourceManager,sessionNotifier);
    }
    
    
    public Document getSessionList(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory ,int startFrom , int countToFetch, String whereXpath) throws Exception {
    	return this.getSessionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory,startFrom,countToFetch, true, whereXpath);
    }

    /**
	 * returns list of sessions matching for given wherexpath if not return every thing based on start from and count to fetch
	 * @param request
	 * @param xmpp
	 * @param log
	 * @param db
	 * @param resourceFactory
	 * @param serviceAgentFactory
	 * @param startFrom
	 * @param countToFetch -1 means fetch every thing.
	 * @param includeMediaRoomInfo
	 * @param whereXpath 
	 * @return
	 * @throws Exception
	 */
    
    public Document getSessionList(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, int startFrom , int countToFetch, boolean includeMediaRoomInfo, String whereXpath) throws Exception {
    	Document doc = MRXMLUtils.stringToDocument("<SessionList/>"); 
    	
     
    	List<String> matchingSessionList = new ArrayList<String>(); 
    
    	//if wherexpath if given apply xpath first and find the list of matching sessions
    	if(whereXpath!=null && !whereXpath.isEmpty()) {		
    		for(String sessionKey:this.sessionTable.keySet()) {
    			Document sessionDoc = sessionTable.get(sessionKey).getData();	
    			List<?> sessionList =  MRXMLUtils.getListXpath(sessionDoc, whereXpath);
    			if (sessionList == null || sessionList.isEmpty()) {
    				continue;    
    			}	
    			matchingSessionList.add(sessionKey);
    		}
    	} else {
    		matchingSessionList= new ArrayList<String>(sessionTable.keySet());
    	}
    	
    	int totalrecords = matchingSessionList.size();
    	
    	int newStartFrom=startFrom-1;    	    	
    	countToFetch = (countToFetch==-1 || ( (newStartFrom + countToFetch) > matchingSessionList.size() ) ) ? matchingSessionList.size() : (newStartFrom + countToFetch) ;
  	
    	if(countToFetch !=0) {
    		matchingSessionList = matchingSessionList.subList(newStartFrom, countToFetch);
    	}
    	   	
    	for(String sessionKey:matchingSessionList) {
    		Session session = this.sessionTable.get(sessionKey);
    		
    		//handling the case where if session deleted 
    		if(session==null) {
    			totalrecords--;
    			continue;
    		}
    		
    		Document sessionDoc = (Document)session.getData().clone();	
    		
    		if(includeMediaRoomInfo) {
    			try {
    				this.addMediaRoomInfo(request, xmpp, log, db,resourceFactory, serviceAgentFactory, sessionDoc);
    			}
    			catch(Exception e) {
    				log.addInfo("SessionManager:getSessionList error=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
    				e.printStackTrace();
    			}
    		} 
    		doc.getRootElement().add(sessionDoc.getRootElement());
    	}  

    	MRXMLUtils.addAttribute(doc.getRootElement(), "totalRecords", String.valueOf(totalrecords));
    	
    	return doc;
    }
    
    int relayLoopingcount = 10;
    private void addMediaRoomInfo(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI serviceAgentFactory, Document sessionDoc) throws Exception {
		String sessionId = sessionDoc.getRootElement().attributeValue(SessionServiceConst.ID);
		String roomNID = this.getRoomNID(sessionId);
		Document roomDoc = RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc();
		
		// Add the relay segments
		List<?> connectionList = MRXMLUtils.getListXpath(sessionDoc, "//Connection");
		for(Object o:connectionList) {			
			Element connection = (Element)o;			
			List<Element> connSegList = new ArrayList<Element>();
			String id = connection.attributeValue(SessionServiceConst.ID);
			Element stream = MRXMLUtils.getElementXpath(roomDoc, "//Stream[@id='" + id + "']");
			if(stream == null) {
				log.addInfo("SessionManager:addMediaRoomInfo error stream not found for id=" + id, MRLog.OK, MRLog.NOTIFY);
				continue;
			}
			try {
				this.addStreamRouteInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connSegList, stream, roomDoc,relayLoopingcount);
				for(int i = connSegList.size() - 1; i >= 0; i--){
					connection.add(connSegList.get(i));
				}
				// Set the connection timelineId to be always the streamNID
				connection.addAttribute(SessionServiceConst.TIMELINEID, stream.attributeValue("NID"));
			}
			catch(Exception e) {
				log.addInfo("SessionManager:addMediaRoomInfo exception found for id=" + id, MRLog.OK, MRLog.NOTIFY);
				e.printStackTrace();
			}
		}
		
		// Update the Timelines
		Element timelineList = MRXMLUtils.getElementXpath(sessionDoc, "//TimelineList");
		Element streamTrack = MRXMLUtils.getElementXpath(roomDoc, "//" + MediaRoomServiceConst.STREAMTRACK);
		String streamTrackNID = streamTrack.attributeValue(SessionServiceConst.NID);
		timelineList.add(SessionServiceUtils.createTimeLine(xmpp, request, log, db, sessionId, 
				streamTrackNID,
				MRXMLUtils.elementToDocument(streamTrack.element("data")), ""));
		sessionDoc.getRootElement().addAttribute("timelineId", streamTrackNID);
		Element recordTrack = MRXMLUtils.getElementXpath(roomDoc, "//" + MediaRoomServiceConst.RECORDTRACK);
		String recordTrackNID = recordTrack.attributeValue(SessionServiceConst.NID);
		timelineList.add(SessionServiceUtils.createTimeLine(xmpp, request, log, db, sessionId, 
				recordTrackNID, 
				MRXMLUtils.elementToDocument(recordTrack.element("data")), ""));
		sessionDoc.getRootElement().addAttribute("recordTimelineId", recordTrackNID);
		
		List<?> streamList = MRXMLUtils.getListXpath(roomDoc, "//Stream");
		for(Object o:streamList) {
			Element stream = (Element)o;
			Element streamTimeline = SessionServiceUtils.createConnectionTimeline(stream, log);
			timelineList.add(streamTimeline);
		}
    }
        
	private void addStreamRouteInfo(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI serviceAgentFactory,
    		List<Element> connSegList, Element stream, Document streamRoomDoc,int relayLoopingCount)throws Exception {
    	if(stream == null)
    		return; // No stream
    	String state = MRXMLUtils.getValueXpath(stream, ".//Status/state", "");
    	String stateStr = MediaRoomServiceUtils.getStateStr(log, Integer.parseInt(state));    	    	
    	String profileTitle = MRXMLUtils.getValueXpath(stream, ".//profileXML//Title", "");
    	String profileNID = MRXMLUtils.getValueXpath(stream, ".//profileNID", "");
    	String destNID = MRXMLUtils.getValueXpath(stream, ".//destNID", "");
    	String sourceNID = MRXMLUtils.getValueXpath(stream, ".//sourceNID", "");
    	String relayNID = MRXMLUtils.getValueXpath(stream, ".//relayNID", "");
    	String streamNID = MRXMLUtils.getAttributeValue(stream, "NID");
    	String mediaSourceNID = MRXMLUtils.getValueXpath(streamRoomDoc, "//Stream[@NID='"+streamNID+"']/data/mediaSourceNID", "");
    	String mediaDestNID = MRXMLUtils.getValueXpath(streamRoomDoc, "//Stream[@NID='"+streamNID+"']/data/mediaDestNID", "");
    	String mediaRelayNID = MRXMLUtils.getValueXpath(streamRoomDoc, "//Stream[@NID='"+streamNID+"']/data/mediaRelayNID", "");
    	Element srcEl = MRXMLUtils.getElementXpath(streamRoomDoc, "//MediaSource[@NID='"+mediaSourceNID+"']");
    	String srcTitle = MRXMLUtils.getAttributeValue(srcEl, "title");
    	String streamType = MRXMLUtils.getValueXpath(srcEl, ".//streamType","");
    	Element dstEl = MRXMLUtils.getElementXpath(streamRoomDoc, "//MediaDest[@NID='"+mediaDestNID+"']");
    	String dstTitle = MRXMLUtils.getAttributeValue(dstEl, "title");
    	MRXMLUtils.documentToString(streamRoomDoc);
    	if(!"".equals(relayNID)){
        	sourceNID = relayNID;
        	srcEl = MRXMLUtils.getElementXpath(streamRoomDoc, "//MediaRelay[@NID='"+mediaRelayNID+"']");
        	srcTitle = MRXMLUtils.getAttributeValue(srcEl, "title");
    	}
    	if(MediaRoomServiceConst.RTP.equalsIgnoreCase(streamType)) {
    		profileNID = SessionServiceConst.RTP_PROFILE_CONST;
    		profileTitle = SessionServiceConst.RTP_PROFILE_CONST;
    	}
    	Element connSegId = MRXMLUtils.stringToElement("<ConnSeg/>");
    	connSegId.addAttribute("state", stateStr);
    	connSegId.addAttribute("srcResourceTitle", srcTitle);
    	connSegId.addAttribute("dstResourceTitle", dstTitle);
    	connSegId.addAttribute("profileTitle", profileTitle);
    	connSegId.addAttribute("srcResourceNID", sourceNID);
    	connSegId.addAttribute("dstResourceNID", destNID);
    	connSegId.addAttribute("profileNID", profileNID);
    	connSegId.addAttribute("multicastIP", MRXMLUtils.getValueXpath(stream, ".//multicastIP", ""));
    	connSegList.add(connSegId);
		Element relayData = MRXMLUtils.getElementXpath(stream, ".//relayData");
		String relayRoomNID = relayData.attributeValue("relayRoomNID");
		String relayStreamNID = relayData.attributeValue("relayStreamNID");
		String roomNID = MRXMLUtils.getValueXpath(streamRoomDoc, ".//roomNID", "");
		
		if(roomNID.equals(relayRoomNID)){
			connSegId.addAttribute(SessionServiceConst.ERROR_CODE,""+MediaRoomServiceExceptionCodes.RELAY_CONFIGURATION_ERROR);
			connSegId.addAttribute(SessionServiceConst.ERROR_DATA, "Relay Looping to itself");
			connSegId.addAttribute(SessionServiceConst.STATE, "SETUP_ERROR");	    	
			return;
		}
		
		relayLoopingCount--;
		if(relayLoopingCount == 0) {			
			connSegId.addAttribute(SessionServiceConst.ERROR_CODE,""+MediaRoomServiceExceptionCodes.RELAY_CONFIGURATION_ERROR);
			connSegId.addAttribute(SessionServiceConst.ERROR_DATA, "Relay Configuration Error");
			connSegId.addAttribute(SessionServiceConst.STATE, "SETUP_ERROR");	 
			return;
		}

		int stateValue = 0;
		try { stateValue = Integer.parseInt(state); } catch(Exception e){}
		if(stateValue > 200) {
			String exceptionCode = MRXMLUtils.getValueXpath(stream, ".//Exception/Code", "");
			String exceptionDescription = MRXMLUtils.getValueXpath(stream, ".//Exception/Description", "");
			connSegId.addAttribute(SessionServiceConst.ERROR_CODE, exceptionCode);
			connSegId.addAttribute(SessionServiceConst.ERROR_DATA, exceptionDescription);
		}

		
		if("".equals(relayRoomNID))
			return;
		Document roomDoc = null;
		try {
			roomDoc = RoomDB.getRoomDB().getRoom(relayRoomNID, request, log, db).getRoomDoc();
		}
		catch(Exception e) {
			// Relay room not found
			return;
		}
		stream = MRXMLUtils.getElementXpath(roomDoc, "//Stream[@NID='" + relayStreamNID + "']");
		this.addStreamRouteInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connSegList, stream, roomDoc,relayLoopingCount);
    }
    
    public Document getSessionDocWithMediaRoomInfo(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI serviceAgentFactory, String sessionID) throws MRException,Exception{    	
    	return this.getSessionDocInternal(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID, true);
    }
    
    public Document getSessionDoc(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI serviceAgentFactory, String sessionID) throws MRException,Exception{    	
    	return this.getSessionDocInternal(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID, false);
    }
    
    private Document getSessionDocInternal(MRRequest request, XMPPI xmpp, MRLog log,
    			MRDB db, MRResourceFactoryI resourceFactory,
        		ServiceAgentFactoryI serviceAgentFactory, String sessionID, boolean includeMediaRoomInfo) throws MRException,Exception{    	
    	String roomNID = roomSessionMapping.get(sessionID);
    	if(roomNID == null) {
    		throw new MRException(SessionServiceExceptionCodes.NO_ROOM_FOUND_FOR_GIVEN_SESSION_ID,"No Room found for given sessionID:="+sessionID);
    	}
    	Session session = sessionTable.get(roomNID);
    	if(session == null){
    		throw new MRException(SessionServiceExceptionCodes.NO_SESSION_FOUND,"Session Not Found");
    	}
    	Document sessionDoc = (Document)session.getData().clone();
    	if(includeMediaRoomInfo)
    		this.addMediaRoomInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc);
    	return sessionDoc;
    }
    public boolean sessionExists(String roomNID) {	
    	return this.sessionTable.containsKey(roomNID);
    }
    
    public void deleteConnection(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, String roomNID, String connId) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.deleteConnection(xmpp, request, log, db, roomNID, connId,resourceManager,sessionNotifier);
    }
    
    public void addEntity(String roomNID, Element entity) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.addEntity(entity);
    }
    
    public void deleteEntity(String roomNID, String entityId) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.deleteEntity(entityId);
    }

    public void addGrpFile(String roomNID, Element grpFile) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.addGrpFile(grpFile);
    }
    
    public void addDir(String roomNID, Element dir) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.addDir(dir);
    }

    public void addConnection(String roomNID, Element connection) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.addConnection(connection);
    }
    
    public void addProfile(String roomNID, Element profileEL) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.addProfile(profileEL);
    }

    public void updateConnection(String roomNID, String connId, String newSrcEntityId, String newDstEntityId,String type,String connName,String canvasId,String windowId,String profileId) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.updateConnection(connId, newSrcEntityId, newDstEntityId,type,connName,canvasId,windowId,profileId);
    }
    
    public void updateProfileInfoInConnection(String roomNID, String connId,String profileId) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.updateProfileInfoInConnection(connId, profileId);
    }
    
    public void updateProfile(String roomNID, String profileId,String profileNID,String profileTitle, String ffTracks) throws Exception {
    	
    	Session session = sessionTable.get(roomNID);
    	session.updateProfileInfo( profileId, profileNID, profileTitle, ffTracks);
    }
    
    public void updatePVRInfo(String roomNID, String sessionID, String srcEntityID, String startTC, String trimLength, String connectionId, String ffTracks, String pvrSessionId) throws Exception {
	Session session = null;
	if(!roomNID.trim().isEmpty())
    		session = sessionTable.get(roomNID);
	else
	    	session = getSession(sessionID);
    	session.updatePVRInfo(srcEntityID, startTC,trimLength,connectionId,ffTracks,pvrSessionId);
    }  
    public void updateConnectionWindow(String roomNID, String connId, String canvasId,String windowId) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.updateConnectionWindow(connId, canvasId,windowId);
    }
    
    public void updateConnectionAuthEntityId(String roomNID, String connId, String authEntityId) throws Exception {
    	Session session = sessionTable.get(roomNID);
    	session.updateConnectionAuthEntityId(connId,authEntityId);
    }

    public void updateSessionName(String sessionId,String sessionName) throws Exception {
    	getSession(sessionId).updateSessionName(sessionName);
    }
    
    public void updateSessionType(String sessionId,String type) throws Exception {    	
    	getSession(sessionId).updateSessionType(type);
    }

    private Session getSession(String sessionId) throws MRException,Exception {
    	String roomNID = roomSessionMapping.get(sessionId);
    	if(roomNID == null) {
    		throw new MRException(SessionServiceExceptionCodes.NO_ROOM_FOUND_FOR_GIVEN_SESSION_ID,"No Room found for given sessionID:="+sessionId);
    	}
    	Session session = sessionTable.get(roomNID);
    	if(session == null){
    		throw new MRException(SessionServiceExceptionCodes.NO_SESSION_FOUND,"Session Not Found");
    	}
    	return session;
    }

    public synchronized String getRoomNID(String sessionID) {
    	return this.roomSessionMapping.get(sessionID);
    }
	
	public Document stopSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String sessionId) throws Exception {
		
		Document doc = request.getMessage().getDocument();
		String roomNID = getRoomNID(sessionId);
		if(roomNID == null) {
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, "Session not found sessionId=" + sessionId);
		}
		Document sessionDoc = getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		try {
			
			ConnectionUtils.deleteMediaRoom(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roomNID);
		}
		catch(Exception e) {
			log.addInfo("StopSessionRequestHandler:getResponse forcing delete : error=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
			e.printStackTrace();
			handleRoomDeleted(xmpp, request, log, db,roomNID, doc);
		}
		
		return request.createResponseData(sessionDoc.getRootElement());

	}
	
	public Document handleStopSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String sessionId) throws Exception {
		Document response = null;
		Exception exp = null;
		Semaphore lock = IPVSMonitor.getMonitor().getLock(sessionId, request.getNID());
		log.addInfo("StopSessionRequestHandler:getResponse got lock for=" + request.getNID() + " sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		try {
			response = this.stopSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		}
		catch(Exception e) {
			exp = e;
		}
		IPVSMonitor.getMonitor().releaseLock(sessionId, lock);
		log.addInfo("StopSessionRequestHandler:getResponse released lock for=" + request.getNID() + " sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		if(exp != null)
			throw exp;

		return response;
	}


	public void reset() {
		this.sessionNotifier.setReset(true);
		this.sessionNotifier = null;
	}

	public void setSessionNotificationListener(SessionNotificationListenerI listener) {
		this.sessionNotifier.setSessionNotificationListsner(listener);
	}

}
