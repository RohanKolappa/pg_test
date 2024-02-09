package com.ipvs.mediaroomservice.impl;

import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;

class Collaboration {
	
	Document collaborationDoc;
	Collaboration(Document data) {
		this.collaborationDoc = data;
	}

	Document getData() {
		return this.collaborationDoc;
	}
	

	void handleSetupError(MRLog log, String id, Exception e) throws Exception {
    	int state = MediaRoomServiceConst.SETUP_ERROR;
    	int code = MRExceptionCodes.SYSTEMERROR;
    	if(e instanceof MRException ) {
    		code = ((MRException)e).getCode();
    	}
        String exceptionDescription = MRException.getStackTrace(e);		
        Element status = MRXMLUtils.getElementXpath(collaborationDoc, "//*[@id='" + id + "']");
        MRXMLUtils.addAttribute(status,"state", "0");
        MRXMLUtils.addAttribute(status,"errorcode", String.valueOf(code));
        MRXMLUtils.addAttribute(status,"errordata", exceptionDescription);
        
        // For connection legacy also update the state element
        if("ConnectionState".equals(status.getName())) {
        	String stateStr = MediaRoomServiceUtils.getStateStr(log, state);
        	MRXMLUtils.getElementXpath(status, ".//streamNID").setText("");
        	MRXMLUtils.getElementXpath(status, ".//state").setText(stateStr);
        	MRXMLUtils.getElementXpath(status, ".//exceptionCode").setText(String.valueOf(code));
        	MRXMLUtils.getElementXpath(status, ".//exceptionDescription").setText(exceptionDescription);
        }
	}
	
	void handleStreamStatusChanged(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, Element streamElement) throws Exception {
		String id = MRXMLUtils.getAttributeValue(streamElement,MediaRoomServiceConst.ID);
		if("".equals(id))
			return;
        Element status = MRXMLUtils.getElementXpath(collaborationDoc, "//ConnectionStatus[@id='" + id + "']");
        String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");

        int state = Integer.parseInt(MRXMLUtils.getValueXpath(streamElement, ".//Status/state", ""));
    	String stateStr = MediaRoomServiceUtils.getStateStr(log, state);
    	MRXMLUtils.addAttribute(status,"state", stateStr);
        
        // For connection legacy also update the state element
        if("ConnectionState".equals(status.getName())) {
        	MRXMLUtils.getElementXpath(status, ".//streamNID").setText(streamNID);
        	MRXMLUtils.getElementXpath(status, ".//state").setText(stateStr);
        }
	}

	void handlePresenceChanged(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, Element mediaElement) throws Exception {
		String id = MRXMLUtils.getAttributeValue(mediaElement,MediaRoomServiceConst.ID);
		if("".equals(id)) {
			// System.out.println("CollaborationManager:handlePresenceChanged entity not found mediaElement: " + MRXMLUtils.elementToString(mediaElement));
			return;
		}
        Element status = MRXMLUtils.getElementXpath(collaborationDoc, "//EntityStatus[@id='" + id + "']");
        String presence = MRXMLUtils.getAttributeValue(mediaElement,MediaRoomServiceConst.PRESENCE);
        MRXMLUtils.addAttribute(status,MediaRoomServiceConst.PRESENCE, presence);
	}

}

public class CollaborationManager implements MediaRoomObserverI {

	private static CollaborationManager theCollaborationManager = null;
	private Hashtable<String, Collaboration> collaborationTable = new Hashtable<String, Collaboration>();
    public static final String COLLABORATION_INSTANCE_XML = "/com/ipvs/mediaroomservice/impl/CollaborationInstance.xml";
    public static final String CONNECTION_STATELIST_XML = "/com/ipvs/mediaroomservice/impl/ConnectionStateList.xml";
	public static final String CONNECTION_STATUS_XML = "/com/ipvs/mediaroomservice/impl/ConnectionStatus.xml";

    private static Document gConnectionStatusTemplate = null;
    
    
    
    public static Document getConnectionStatusTemplate(MRLog log) throws Exception  {
    	if(CollaborationManager.gConnectionStatusTemplate == null) {
    		CollaborationManager.gConnectionStatusTemplate = MRXMLUtils.loadXMLResource(CollaborationManager.CONNECTION_STATUS_XML, log);
    	}
    	return (Document)CollaborationManager.gConnectionStatusTemplate.clone();
    }

	public static CollaborationManager getCollaborationManager() {
		if(CollaborationManager.theCollaborationManager == null) {
			CollaborationManager.theCollaborationManager = new CollaborationManager();
			RoomDB.getRoomDB().addObserver(MediaRoomServiceConst.COLLABORATION, 
					CollaborationManager.theCollaborationManager);
		}
		return theCollaborationManager;
	}
	
	public void addCollaboration(MRRequest request, MRLog log, MRDB db,
			String roomNID, Element collaborationData, Document roomDoc) throws Exception {
		Document collaborationDoc = MRXMLUtils.loadXMLResource(CollaborationManager.COLLABORATION_INSTANCE_XML, log);
		Element data = MRXMLUtils.getElementXpath(collaborationDoc, "//CollaborationData");
		data.detach();
		data = (Element)collaborationData.clone();
		data.setName("CollaborationData");
		MRXMLUtils.addElement(collaborationDoc.getRootElement(),data);
		List<?>entityElementList = MRXMLUtils.getElementXpath(collaborationDoc, "//EntityList").elements();
    	Element entityStatusList = MRXMLUtils.getElementXpath(collaborationDoc, "//EntityStatusList");
		for(int i=0; i< entityElementList.size();i++) {
			Element entity = (Element)entityElementList.get(i);
			String entityID = MRXMLUtils.getAttributeValue(entity,MediaRoomServiceConst.ID);
	    	Element entityStatus = MRXMLUtils.stringToElement("<EntityStatus/>");
	    	MRXMLUtils.addAttribute(entityStatus,MediaRoomServiceConst.ID, entityID);
	    	MRXMLUtils.addAttribute(entityStatus,MediaRoomServiceConst.PRESENCE, "");
	    	MRXMLUtils.addElement(entityStatusList,entityStatus);
		}
		List<?>connectionElementList = MRXMLUtils.getElementXpath(collaborationDoc, "//ConnectionList").elements();
    	Element connectionStatusList = MRXMLUtils.getElementXpath(collaborationDoc, "//ConnectionStatusList");
		for(int i=0; i< connectionElementList.size();i++) {
			Element connectionElement = (Element)connectionElementList.get(i);
			String connectionID = MRXMLUtils.getAttributeValue(connectionElement,MediaRoomServiceConst.ID);
	        Document doc = CollaborationManager.getConnectionStatusTemplate(log);
	    	Element connectionStatus = doc.getRootElement();
	    	MRXMLUtils.addAttribute(connectionStatus,MediaRoomServiceConst.ID, connectionID);
	    	connectionStatusList.add(connectionStatus.detach());
		}
		this.collaborationTable.put(roomNID, new Collaboration(collaborationDoc));
		//System.out.println(MRXMLUtils.documentToString(collaborationDoc));
	}

	public void handlePresenceChanged(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, Element mediaElement) {
		try {
			Collaboration collaboration = this.collaborationTable.get(roomNID);
			collaboration.handlePresenceChanged(xmpp, request, log, db, roomNID, mediaElement);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	public void handleRoomDeleted(XMPPI xmpp, MRRequestI request, MRLog log,
			MRDB db, String roomNID, Document doc) {
		this.collaborationTable.remove(roomNID);
	}

	public void handleStreamStatusChanged(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, Element streamElement, Element sourceElement){
		try {
			Collaboration collaboration = this.collaborationTable.get(roomNID);
			collaboration.handleStreamStatusChanged(xmpp, request, log, db, roomNID, streamElement);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	public void handleSetupError(XMPPI xmpp, MRRequestI request,
			MRLog log, MRDB db, String roomNID, String id, Exception e) {
		try {
			Collaboration collaboration = this.collaborationTable.get(roomNID);
			collaboration.handleSetupError(log, id, e);
		}
		catch(Exception ex) {
			ex.printStackTrace();
		}
	}
	
	public void handleStreamAdded(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, Element streamElement, 
    		Element sourceElement) {
		// Dummy
	}
	
	public void handleStreamDeleted(XMPPI  xmpp, MRRequest request, MRLog log, MRDB db, String roomNID, 
			Element streamElement, Element sourceElement, int state) {
    	// Dummy
    }
	
	public Element getData(MRRequestI request,XMPPI xmpp, 
			MRLog log, MRDB db, String roomNID) throws Exception {
		Collaboration collaboration = this.collaborationTable.get(roomNID);
		return (Element)collaboration.getData().getRootElement().clone();
	}

    public void handleTimeLineChanged(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, 
    		String roomNID, String trackNID, Document trackDataDoc, String requestedAction) {
    	// Dummy
    }

	/*
 	public static void updateCollaborationConnectionStatus(String roomNID, String streamNID,
			XMPPI xmpp, MRRequest request, MRLog log, MRDB db) throws Exception {
    	Document streamDoc = theRoomDB.readElement(roomNID, request, log, db, streamNID);
    	String connectionID = streamDoc.getRootElement().getAttributeValue(MediaRoomServiceConst.ID);
    	int state = Integer.parseInt(MRXMLUtils.getValueXpath(streamDoc, "//Status/state", ""));
    	String exceptionCode = MRXMLUtils.getValueXpath(streamDoc, "//eventCode", "");
    	String exceptionDescription = MRXMLUtils.getValueXpath(streamDoc, "//Exception/Description", "");
        Element relayStatusList = MRXMLUtils.stringToElement("<relayStatusList/>");
        Element relayData = MRXMLUtils.getElementXpath(streamDoc, "//relayData");
        MediaRoomServiceUtils.updateRelayStatus(relayStatusList, relayData, request, log, db);
    	MediaRoomServiceUtils.updateCollaborationConnectionStatusInternal(
    			xmpp, request, db, log, roomNID, connectionID, 
    			streamNID, state, relayStatusList, exceptionCode, exceptionDescription);
	}

	public static void  updateCollaborationConnectionStatus(
			XMPPI xmpp, MRRequest request, MRDB db, MRLog log,
			String roomNID, String connectionID, 
			int requestState, int code, Exception e) throws Exception {
        String exceptionCode = "";
        String exceptionDescription = "";
        int state = MediaRoomServiceConst.STREAM_STOPPED;
        if(state == MRRequest.STATE_EXCEPTION) {
        	state = MediaRoomServiceConst.SETUP_ERROR;
            exceptionCode = String.valueOf(code);
            exceptionDescription = MRException.getStackTrace(e);
        }
        Element relayStatusList = MRXMLUtils.stringToElement("<relayStatusList/>");
    	MediaRoomServiceUtils.updateCollaborationConnectionStatusInternal(
    			xmpp, request, db, log, roomNID, connectionID, 
    			"", state, relayStatusList, exceptionCode, exceptionDescription);
	}
	
	private static void updateCollaborationEntityStatus(MRRequest request, MRLog log, MRDB db,
			String roomNID, String id, String presence) throws Exception {
    	Element entityStatus = MRXMLUtils.stringToElement("<EntityStatus/>");
    	entityStatus.setAttribute(MediaRoomServiceConst.ID, id);
    	entityStatus.setAttribute(MediaRoomServiceConst.PRESENCE, presence);
		MediaRoomServiceUtils.theRoomDB.updateCollaborationEntityStatus(roomNID, entityStatus);
	}

	public static void createCollaborationEntityStatus(MRRequest request, MRLog log, MRDB db,
			String roomNID, String NID, String id) throws Exception {
		Document doc = theRoomDB.readElement(roomNID, request, log, db, NID);
		String presence = doc.getRootElement().getAttributeValue(MediaRoomServiceConst.PRESENCE);
		MediaRoomServiceUtils.updateCollaborationEntityStatus(request, log, db, roomNID, id, presence);
	}

    private static void updateCollaborationConnectionStatusInternal(
    		XMPPI xmpp, MRRequest request, MRDB db, MRLog log,
    		String roomNID, String connectionID, 
			String streamNID, int state, Element relayStatusList, String exceptionCode, String exceptionDescription) throws Exception {
        Document doc = MediaRoomServiceUtils.getConnectionStatusTemplate(log);
        MRXMLUtils.getElementXpath(doc, "//streamNID").setText(streamNID);
    	String stateStr = "";
    	Element connectionState = MRXMLUtils.getElementXpath(
    			MediaRoomServiceUtils.getConnectionStateList(log), 
    			"//ConnectionState[@streamstate='" + String.valueOf(state) + "']");
    	if(connectionState != null)
    		stateStr = connectionState.getAttributeValue("state");
    	else 
    		stateStr = "ConnectionStateNotFoundFor streamstate=" + String.valueOf(state);
        MRXMLUtils.getElementXpath(doc, "//state").setText(stateStr);
        MRXMLUtils.getElementXpath(doc, "//exceptionCode").setText(exceptionCode);
        MRXMLUtils.getElementXpath(doc, "//exceptionDescription").setText(exceptionDescription);
        Element prevRelayStatusList = MRXMLUtils.getElementXpath(doc, "//relayStatusList");
        Element prevRelayStatusListParent = prevRelayStatusList.getParentElement();
        prevRelayStatusListParent.removeChild("relayStatusList");
        prevRelayStatusListParent.add(relayStatusList);
        Element connectionStatus = (Element)doc.getRootElement().clone();
        connectionStatus.getAttribute(MediaRoomServiceConst.ID).setValue(connectionID);
        MediaRoom room = MediaRoomServiceUtils.theRoomDB.getRoom(roomNID, request, log, db);
		room.updateCollaborationConnectionStatus(connectionStatus);
    	Document roomDoc = room.getRoomDoc();
    	String roomID = roomDoc.getRootElement().getAttributeValue("id");
		MediaRoomServiceUtils.handleConnectionUpdated(xmpp, request, log, db, roomNID, roomID, connectionID, connectionStatus);

	}
	*/


}
