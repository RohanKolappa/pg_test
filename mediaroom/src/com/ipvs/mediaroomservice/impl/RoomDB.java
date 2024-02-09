package com.ipvs.mediaroomservice.impl;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class RoomDB {
	
    public static final String MEDIAROOM_INSTANCE_XML = "/com/ipvs/mediaroomservice/datamodel/MediaRoomInstance.xml";
    public static final String MEDIATRACK_INSTANCE_XML = "/com/ipvs/mediaroomservice/datamodel/MediaTrackInstance.xml";
    public static final String STREAM_INSTANCE_XPATH = "//StreamTrack/streamlist/Stream";
    public static final String MEDIARELAY_INSTANCE_XPATH = "//StreamTrack/mediarelaylist/MediaRelay";
    public static final String MEDIADEST_INSTANCE_XPATH = "//StreamTrack/mediadestlist/MediaDest";
    public static final String MEDIASOURCE_INSTANCE_XPATH= "//StreamTrack/mediasourcelist/MediaSource";
    public static final String SYSTEM_TRACK_XPATH= "//systemtracklist";
    public static final String MEDIA_SOURCE_LIST_XPATH = "//mediasourcelist";
    public static final String MEDIA_DEST_LIST_XPATH = "//mediadestlist";
    public static final String MEDIA_RELAY_LIST_XPATH = "//mediarelaylist";
    public static final String STREAM_LIST_XPATH = "//streamlist";
    
    public static Document gMediaSourceElementTemplate = null;
    public static Document gMediaDestElementTemplate = null;
    public static Document gMediaRelayElementTemplate = null;
    public static Document gStreamElementTemplate = null;
    public static Document gMediaRoomElementTemplate = null;

    private static RoomDB theRoomDB = new RoomDB();
    private Hashtable<String, MediaRoom> roomTable = new Hashtable<String, MediaRoom>();
    private Hashtable<String, MediaRoomObserverI> roomObserverTable = new Hashtable<String, MediaRoomObserverI>();
    private AssetRoster roster = new AssetRoster();

    public static RoomDB getRoomDB() {
    	return RoomDB.theRoomDB;
    }
    private RoomDB(){}
    public boolean roomExists(String roomNID) {
    	return this.roomTable.containsKey(roomNID);
    }
    
    public void addObserver(String type, MediaRoomObserverI observer) {
    	this.roomObserverTable.put(type, observer);
    }
    
    private synchronized ArrayList<String> getRoomNIDList() {
    	ArrayList<String> roomNIDList = new ArrayList<String>();
		Iterator<String> i = this.roomTable.keySet().iterator();
		while(i.hasNext())
			roomNIDList.add(i.next());
		return roomNIDList;
    }
    
    public Document getMediaRoomList(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, 
    		String xPath, boolean includeObserverData) throws Exception {
    	return this.getMediaRoomListWithFilter(request, xmpp, log, db, xPath, includeObserverData, null);
    }
    
    public Document getMediaRoomListWithFilter(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, 
    		String xPath, boolean includeObserverData, ArrayList<String> filterRoomNIDList) throws Exception {
    	// System.out.println("Applying xPath=" + xPath);
        Document responsedoc = MRXMLUtils.stringToDocument(
        		"<GetResponse>" +
        			"<ResponseID>myTest1</ResponseID>" +
        			"<ResponseXML/>" +
        			"<GetListObjectResponseData>" + 
        				"<ObjectCount/>" +
        				"<ObjectLastWhereEndCount/>" +
    				"</GetListObjectResponseData>" + 
        		"</GetResponse>");
        Element responseXML = MRXMLUtils.getElementXpath(responsedoc, "//ResponseXML");
    	int count = 0;
    	
    	ArrayList<String> roomNIDList = filterRoomNIDList;
    	if(roomNIDList == null)
    		roomNIDList = this.getRoomNIDList();
    	for(String roomNID:roomNIDList) {
			MediaRoom room = this.roomTable.get(roomNID);
			if(room == null) {
				continue; // Room must have been deleted
			}
			Document roomdoc = room.getRoomDoc();
			Element roomElement = MRXMLUtils.getElementXpath(roomdoc, xPath);
			if(roomElement != null) {
				Element rootElement = (Element)roomdoc.getRootElement().detach();
				if(includeObserverData) {
			    	MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
			    	if(observer != null) {
			    		Element applicationData = MRXMLUtils.stringToElement("<" + MediaRoomServiceConst.APPLICATIONDATA + "/>");
			    		rootElement.add(applicationData);
			    		applicationData.add(observer.getData(request, xmpp, log, db, roomNID));
			    	}
				}
				responseXML.add(rootElement);
				count++;
			}
		}
		MRXMLUtils.getElementXpath(responsedoc, "//ObjectCount").setText(String.valueOf(count));
		MRXMLUtils.getElementXpath(responsedoc, "//ObjectLastWhereEndCount").setText(String.valueOf(count));
        return responsedoc;
    }
    
    public synchronized String addRoom(String roomNID, MRRequestI request, MRLog log, MRDB db, 
    		Document roomdoc)    throws Exception {
    	String type = MRXMLUtils.getValueXpath(roomdoc, "//roomType", "");
    	MediaRoom mediaRoom = new MediaRoom((Document)roomdoc.clone(), type);
		this.roomTable.put(roomNID, mediaRoom);
		log.addInfo("RoomDB:addRoom room roomNID=" + roomNID + " size=" + this.roomTable.size() + " AssetRoster size=" + this.roster.getSize());    	
		return roomNID;
    }
    
    public synchronized MediaRoom getRoom(String roomNID, MRRequestI request, MRLog log, MRDB db)
    throws Exception {
    	MediaRoom mediaRoom = this.roomTable.get(roomNID);
    	if(roomNID == null)
    		roomNID = "null";
    	if(mediaRoom == null)
    		throw new MRException(MediaRoomServiceExceptionCodes.ROOMNOTFOUND, roomNID);
    	return mediaRoom;
    }

    public synchronized void deleteRoom(String roomNID, XMPPI xmpp, MRRequestI request, MRLog log, MRDB db)
    throws Exception {
    	MediaRoom room = this.roomTable.get(roomNID);
    	if(room == null)
    		return;
		this.roomTable.remove(roomNID);
    	MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
    	if(observer != null)
    		observer.handleRoomDeleted(xmpp, request, log, db, roomNID, room.roomdoc);
    	this.roster.handleRoomDeleted(xmpp, log, roomNID, room.roomdoc, room.getUnavailableAssetResourceNIDList());
		log.addInfo("RoomDB:deleteRoom room roomNID=" + roomNID + " size=" + this.roomTable.size() + " AssetRoster size=" + this.roster.getSize());    	
    }
    
    public void deleteStream(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, String roomNID, 
    		String streamNID) throws Exception {
    	MediaRoom room = this.getRoom(roomNID, request, log, db);
        Element streamElement = room.deleteElement(request, log, db, streamNID);
        String mediaSourceNID = MRXMLUtils.getValueXpath(streamElement, ".//mediaSourceNID", "");
		Element mediaSourceEl = MRXMLUtils.getElementXpath(room.getRoomDoc(), "//MediaSource[@NID='" + mediaSourceNID + "']");
    	MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
    	if(observer != null)
    		observer.handleStreamDeleted(xmpp, request, log, db, roomNID, streamElement, mediaSourceEl, MediaRoomServiceConst.STREAM_DELETED);
    }

    public void updatePresence(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, String roomNID, String NID, String presence) throws Exception {
        MediaRoom room = this.getRoom(roomNID, request, log, db);
        Element element = room.updatePresence(request, log, db, NID, presence);
    	MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
    	if(observer != null)
    		observer.handlePresenceChanged(xmpp, request, log, db, roomNID, element);
    }

    public void updateStreamStatus(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, String roomNID, String streamNID, Element streamStatus) throws Exception {
		MediaRoom room = this.getRoom(roomNID, request, log, db);
	 	XMPPTransaction.startBlock("updateStatus");
	 	
	 	Element element = room.updateStreamStatus(request, log, db, streamNID, streamStatus);
    	if(element == null)
    		return;
		XMPPTransaction.endBlock("updateStatus");
		XMPPTransaction.startBlock("updateCollaborationConnectionStatus");
		String mediaSourceNID = MRXMLUtils.getValueXpath(element, ".//mediaSourceNID", "");
		Element sourceElement = MRXMLUtils.getElementXpath(room.getRoomDoc(), "//MediaSource[@NID='" + mediaSourceNID + "']");
		MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
    	if(observer != null)
    		observer.handleStreamStatusChanged(xmpp, request, log, db, roomNID, element, sourceElement);
		XMPPTransaction.endBlock("updateCollaborationConnectionStatus");
    }

    public void updateStreamState(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, String roomNID, String streamNID, String state) throws Exception {
		MediaRoom room = this.getRoom(roomNID, request, log, db);
	
		XMPPTransaction.startBlock("updateState");
    	Element element = room.updateStreamState(request, log, db, streamNID, state);
    	XMPPTransaction.endBlock("updateState");
    	
    	if(element == null) {
    		return;
    	}
    	XMPPTransaction.startBlock("updateCollaborationConnectionStatus");
		String mediaSourceNID = MRXMLUtils.getValueXpath(element, ".//mediaSourceNID", "");
		Element sourceElement = MRXMLUtils.getElementXpath(room.getRoomDoc(), "//MediaSource[@NID='" + mediaSourceNID + "']");
		MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
    	if(observer != null)
    		observer.handleStreamStatusChanged(xmpp, request, log, db, roomNID, element, sourceElement);
		XMPPTransaction.endBlock("updateCollaborationConnectionStatus");
    }

    public void updateStreamContactJID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String roomNID, String streamXPath, String streamDataElementName, String contactJID) throws Exception {
        MediaRoom room = this.getRoom(roomNID, request, log, db);
    	room.updateStreamContactJID(streamXPath, streamDataElementName, contactJID);
    }

    public void updateMediaSourceContactJID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String roomNID, String mediaSourceXPath, String mediaSourceDataElementName, String contactJID) throws Exception {
        MediaRoom room = this.getRoom(roomNID, request, log, db);
    	List<Element> mediaSourceList = room.updateMediaSourceContactJID(mediaSourceXPath, mediaSourceDataElementName, contactJID);
    	handlePresenceUpdate(request, xmpp, log, db, roomNID, mediaSourceList, room);    	
    }
    
    private void handlePresenceUpdate(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String roomNID,List<Element> mediaList,MediaRoom room) {
    	for(Element mediaEl:mediaList) {
    		MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
        	if(observer != null)
        		observer.handlePresenceChanged(xmpp, request, log, db, roomNID, mediaEl);
    	}
    }

    public void updateMediaDestContactJID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String roomNID, String mediaDestXPath, String mediaDestDataElementName, String contactJID) throws Exception {
        MediaRoom room = this.getRoom(roomNID, request, log, db);
        List<Element> mediaDestinationList = room.updateMediaDestContactJID(mediaDestXPath, mediaDestDataElementName, contactJID);
    	handlePresenceUpdate(request, xmpp, log, db, roomNID, mediaDestinationList, room);
    }
    
    public synchronized void updateMediaRelayContactJID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String roomNID, String mediaRelayXPath, String mediaRelayDataElementName, String contactJID) throws Exception {
        MediaRoom room = this.getRoom(roomNID, request, log, db);
    	room.updateMediaRelayContactJID(mediaRelayXPath, mediaRelayDataElementName, contactJID);
    }

    private String getUUID() {
    	// return "uuid-" + Utils.getUUID(); // UI fixed so not required
    	return Utils.getUUID();
    }
    
    public void addTrackElement(Document roomDoc, Document trackDoc, String trackName,
    		String roomNID, String syncData, long wallClock, MRLog log) throws Exception {
    	String trackNID = MediaRoomServiceUtils.getTrackNID(roomNID, trackName);
	    MRXMLUtils.getElementXpath(trackDoc, "//roomNID").setText(roomNID);
	    MRXMLUtils.getElementXpath(trackDoc, "//" + MediaRoomServiceConst.SYNCDATA).setText(syncData);
	    MRXMLUtils.getElementXpath(trackDoc, "//pvrEnabled").setText("false");
	    Element mediaTimelineElement = MRXMLUtils.getElementXpath(trackDoc, "//TrackMediaTimeline");
		mediaTimelineElement.addAttribute("id", trackNID);
	    mediaTimelineElement.element("Start").addAttribute("WC", String.valueOf(wallClock));
	    mediaTimelineElement.element("Start").addAttribute("Offset", "0");
	    MediaTimeline.updateMediaTimelineElement(mediaTimelineElement, wallClock, 0, 
	    		0, 1, MediaRoomServiceConst.START, MediaRoomServiceConst.TRACK_STARTED_CURRENT, MediaRoomServiceConst.START, "");

    	Element trackElement = (Element)trackDoc.getRootElement().clone();
    	trackElement.setName(trackName);
    	MRXMLUtils.addAttribute(trackElement,"NID", trackNID); 
    	MRXMLUtils.getElementXpath(roomDoc, "//systemtracklist").add(trackElement);
    }



    public String createMediaRoomDocument(String id, String title, String description, 
    		String roomOwnerJID, String streamSyncData, String recordSyncData, long wallClock, Element groups,
    		String roomType,  
    		MRRequest request, MRLog log, MRDB db)
        throws Exception {
        // Build the object
  	  	String roomNID = this.getUUID();
        Document doc = getMediaRoomInstanceXML(log);
        MRXMLUtils.addAttribute(doc.getRootElement(),"NID", roomNID);
        MRXMLUtils.addAttribute(doc.getRootElement(), MediaRoomServiceConst.ID, id);    
        if(groups!=null) {
            MRXMLUtils.removeChild(MRXMLUtils.getElementXpath(doc, "//MediaRoom/data/Info"), "Groups");
            MRXMLUtils.getElementXpath(doc, "//MediaRoom/data/Info").add((Element)groups.clone());
        }
        MRXMLUtils.getElementXpath(doc, "//MediaRoom/data/Info/Title").setText(title);
        MRXMLUtils.getElementXpath(doc, "//MediaRoom/data/Info/Description").setText(description);
        MRXMLUtils.getElementXpath(doc, "//MediaRoom/data/roomOwnerJID").setText(roomOwnerJID);
        MRXMLUtils.getElementXpath(doc, "//MediaRoom/data/roomType").setText(roomType);
        
        Document trackDoc = getTrackInstanceXML(log, MediaRoomServiceConst.STREAMTRACK);
        this.addTrackElement(doc, trackDoc, MediaRoomServiceConst.STREAMTRACK, roomNID, 
        		streamSyncData, wallClock, log);
        this.addTrackElement(doc, trackDoc, MediaRoomServiceConst.RECORDTRACK, roomNID, 
        		recordSyncData, wallClock, log);
        this.addTrackElement(doc, trackDoc, MediaRoomServiceConst.PVRTRACK, roomNID, 
        		"", wallClock, log);
        MRXMLUtils.getElementXpath(doc, "//MediaRoom/data/creationWallClock").setText(String.valueOf(wallClock));
        this.addRoom(roomNID, request, log, db, doc);
        // System.out.println(MRXMLUtils.documentToString(doc));
        return roomNID;
    }

    public String createMediaSourceElement(String roomNID, String id, String state,
    		String trackNID, String sourceNID, String sourcePortNID, String sourcePortElementID, String title,
    		String sourceType, String sourceAgentJID, String startOffset, long startTimecode,
        String lookbackSourceNID, String lookbackSourceAgentJID, String lookbackSourceFileUUID,String lookbackSourceParentDirNID, 
        String lookbackSourceFilePlayLength,String lookbackStartTC, String streamType, String streamURL, String streamProfile, String userRoomMediaSourceNID, 
        String streamTrackMediaSourceNID, String xAuthAgentJID,String monitorAgentJID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
        // Build the object
    	String mediaSourceNID = this.getUUID();
	
		if(RoomDB.gMediaSourceElementTemplate == null) {
			RoomDB.gMediaSourceElementTemplate = getMediaSourceInstanceXML(log);
		}

		Document doc = (Document)RoomDB.gMediaSourceElementTemplate.clone();
        
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaSource","NID", mediaSourceNID);
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaSource",MediaRoomServiceConst.ID, id);
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaSource",MediaRoomServiceConst.PRESENCE, state);
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaSource",MediaRoomServiceConst.TITLE, title);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/roomNID",roomNID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/trackNID",trackNID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/sourceNID",sourceNID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/sourcePortNID",sourcePortNID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/sourcePortElementID",sourcePortElementID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/sourceType",sourceType);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/sourceAgentJID",sourceAgentJID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/startOffset",startOffset);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/startTimecode",String.valueOf(startTimecode));
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/lookbackSourceNID",lookbackSourceNID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/lookbackSourceAgentJID",lookbackSourceAgentJID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/lookbackSourceFileUUID",lookbackSourceFileUUID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/lookbackSourceFileParentDirNID",lookbackSourceParentDirNID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/lookbackSourceFilePlayLength",lookbackSourceFilePlayLength);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/lookbackSourceFileStartTC",lookbackStartTC);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/streamType",streamType);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/streamURL",streamURL);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/streamProfile",streamProfile);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/userRoomMediaSourceNID",userRoomMediaSourceNID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/streamTrackMediaSourceNID",streamTrackMediaSourceNID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/xAuthAgentJID",xAuthAgentJID);
        MRXMLUtils.setValueXpath(doc, "//MediaSource/data/monitorAgentJID",monitorAgentJID);

        Element msElement = MRXMLUtils.getElementXpath(doc, "//MediaSource");
        MediaRoom room = this.getRoom(roomNID, request, log, db);
        room.addMediaSourceElement(request, log, db, msElement, trackNID);
        // System.out.println(MRXMLUtils.documentToString(doc));
        MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
    	if(observer != null) {
    		observer.handlePresenceChanged(xmpp, request, log, db, roomNID, msElement);
    	}                       
        return mediaSourceNID;
    }
    
    public String createMediaDestElement(String roomNID, String id, String state,
    		String destNID, String destType, String destPortNID, String destAgentJID, String title, 
    		String destIP, String destUDPStartPort, String callbackPort, String trackNID, 
    		String destFileUUID, String destFileParentDirNID, String destFileRelayNID, 
    		String destPortElementID, String destAssetType, String destStreamURL, String trimLength, 
    		String trickPlay,Element fileInfoELement, MRRequest request,  XMPPI xmpp, MRLog log, MRDB db) throws Exception {
        // Build the object
    	String mediaDestNID = this.getUUID();

    	if(RoomDB.gMediaDestElementTemplate == null) {
    		RoomDB.gMediaDestElementTemplate = getMediaDestInstanceXML(log);
    	}
    	Document doc = (Document)RoomDB.gMediaDestElementTemplate.clone();

        	
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaDest","NID", mediaDestNID);
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaDest",MediaRoomServiceConst.PRESENCE, state);
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaDest",MediaRoomServiceConst.TITLE, title);
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaDest",MediaRoomServiceConst.ID, id);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/roomNID",roomNID);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destNID",destNID);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destType",destType);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destPortNID",destPortNID);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destAgentJID",destAgentJID);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destIP",destIP);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destUDPStartPort",destUDPStartPort);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/callbackPort",callbackPort);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destFileUUID",destFileUUID);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destFileParentDirNID",destFileParentDirNID);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destFileRelayNID",destFileRelayNID);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destPortElementID",destPortElementID);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destAssetType",destAssetType);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destStreamURL",destStreamURL);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/trimLength",trimLength);
        MRXMLUtils.setValueXpath(doc, "//MediaDest/data/trickPlay", trickPlay);
        MRXMLUtils.removeNodeXpath(doc, "//"+MediaRoomServiceConst.FILE_INFO_ELEMENT);
        if(fileInfoELement!=null) {
        	MRXMLUtils.addElementXpath(doc, "//MediaDest/data", (Element)fileInfoELement.clone());
        }

        Element mdElement = MRXMLUtils.getElementXpath(doc, "//MediaDest");
        MediaRoom room = this.getRoom(roomNID, request, log, db);
        room.addMediaDestElement(request, log, db, mdElement, trackNID);
        // System.out.println(MRXMLUtils.documentToString(doc));
        MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
    	if(observer != null) {
    		observer.handlePresenceChanged(xmpp, request, log, db, roomNID, mdElement);
    	}
        return mediaDestNID;
    }
    
    public String createMediaRelayElement(String roomNID, String state,
    		String relayNID, String relayAgentJID, String trackNID, String title, MRRequest request, MRLog log, MRDB db) throws Exception {
        // Build the object
    	String mediaRelayNID = this.getUUID();
    	if(RoomDB.gMediaRelayElementTemplate == null) {
    		RoomDB.gMediaRelayElementTemplate = getMediaRelayInstanceXML(log);
    	}
    	Document doc = (Document)RoomDB.gMediaRelayElementTemplate.clone();
        
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaRelay","NID", mediaRelayNID);
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaRelay",MediaRoomServiceConst.PRESENCE, state);
        MRXMLUtils.setAttributeValueXPath(doc, "//MediaRelay",MediaRoomServiceConst.TITLE, title);
        MRXMLUtils.setValueXpath(doc, "//MediaRelay/data/roomNID",roomNID);
        MRXMLUtils.setValueXpath(doc, "//MediaRelay/data/relayNID",relayNID);
        MRXMLUtils.setValueXpath(doc, "//MediaRelay/data/relayPortNID",relayNID);
        MRXMLUtils.setValueXpath(doc, "//MediaRelay/data/relayAgentJID",relayAgentJID);
        Element mdElement = MRXMLUtils.getElementXpath(doc, "//MediaRelay");
        MediaRoom room = this.getRoom(roomNID, request, log, db);
        room.addMediaRelayElement(request, log, db, mdElement, trackNID);
        //  System.out.println("relayDoc"+MRXMLUtils.documentToString(doc));
        return mediaRelayNID;
    }


    public Element createStreamElement(String roomId, String id, String roomOwnerJID, String mediaSourceNID, String mediaDestNID,String mediaRelayNID, String description, 
    	String profileXML, String profileNID, String roomNID, String ownerUserJID, String sourceNID, String sourcePortNID,
    	String sourceAgentJID, String destNID, String destPortNID, String destPortElementID,
    	String destType, String destAgentJID,String  destFileUUID ,String destFileParentDirNID,String relayNID, 
    	String xAuthAgentJID,  String streamType, String streamURL, String destIP, String destUDPStartPort, String callbackPort, String destStreamURL,
    	String streamURLType, String trackNID, String trackState, long trackOffset, String resolvedSourceIP, String resolvedDestIP, String publishedSourceIP, 
    	String publishedDestIP, Element sourceNetworkAddressRule, Element destNetworkAddressRule, String windowId, String canvasId, String trimLength, String trickPlay, 
    	String streamNID,String multicastIP,Element fileInfoElement, XMPPI xmpp, MRRequest request, MRLog log, MRDB db,ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        // Build the object
    	Element streamElement;
		try {
		
			
			if(RoomDB.gStreamElementTemplate == null) {
				RoomDB.gStreamElementTemplate = getStreamInstanceXML(log);
			}
			Document doc = (Document)RoomDB.gStreamElementTemplate.clone();
			
			MRXMLUtils.setAttributeValueXPath(doc, "//Stream","NID", streamNID);
			MRXMLUtils.setAttributeValueXPath(doc, "//Stream",MediaRoomServiceConst.ID, id);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/mediaSourceNID",mediaSourceNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/mediaDestNID",mediaDestNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/mediaRelayNID",mediaRelayNID);
			MRXMLUtils.setAttributeValueXPath(doc, "//Stream/data/context",MediaRoomServiceConst.ROOMID, roomId);
			MRXMLUtils.setAttributeValueXPath(doc, "//Stream/data/context",MediaRoomServiceConst.ID, id);
			MRXMLUtils.setAttributeValueXPath(doc, "//Stream/data/context",MediaRoomServiceConst.ROOMOWNERJID, roomOwnerJID);
			MRXMLUtils.copyElement(doc, "//Stream/data/profileXML",MRXMLUtils.removeSystemAttributesFromXML(profileXML));
			MRXMLUtils.setValueXpath(doc, "//Stream/data/profileNID",profileNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/roomNID",roomNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/streamOwnerUserJID",ownerUserJID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/sourceNID",sourceNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/sourcePortNID",sourcePortNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/sourceAgentJID",sourceAgentJID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destNID",destNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destPortNID",destPortNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destPortElementID",destPortElementID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destType",destType);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destAgentJID",destAgentJID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destFileUUID",destFileUUID);        
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destFileParentDirNID",destFileParentDirNID);        
			MRXMLUtils.setValueXpath(doc, "//Stream/data/relayNID",relayNID);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/xAuthAgentJID",xAuthAgentJID);      
			MRXMLUtils.setValueXpath(doc, "//Stream/data/streamType",streamType);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/streamURL",streamURL);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destIP",destIP);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destUDPStartPort",destUDPStartPort);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/callbackPort",callbackPort);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/destStreamURL",destStreamURL);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/streamURLType",streamURLType);
			MRXMLUtils.setValueXpath(doc, "//Stream/Status/state",String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
			MRXMLUtils.setAttributeValueXPath(doc, "//Stream/Status/relayData","relayRoomNID", "");
			MRXMLUtils.setAttributeValueXPath(doc, "//Stream/Status/relayData","relayStreamNID", "");
			
			MRXMLUtils.setValueXpath(doc, "//Stream/data/startWallClock","0");
			MRXMLUtils.setValueXpath(doc, "//Stream/data/trackStateAtStart",trackState);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/trackOffsetAtStart",String.valueOf(trackOffset));
			MRXMLUtils.setValueXpath(doc, "//Stream/data/resolvedSourceIP",resolvedSourceIP);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/resolvedDestIP",resolvedDestIP);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/publishedSourceIP",publishedSourceIP);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/publishedDestIP",publishedDestIP);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/multicastIP", multicastIP);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/trimLength",trimLength);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/trickPlay", trickPlay);
			
			MRXMLUtils.removeNodeXpath(doc, "//"+MediaRoomServiceConst.FILE_INFO_ELEMENT);
			if(fileInfoElement != null) {
				MRXMLUtils.addElementXpath(doc, "//Stream/data", (Element)fileInfoElement.clone());
			}
			
			if(sourceNetworkAddressRule!= null) {
				MRXMLUtils.getElementXpath(doc, "//Stream/data/sourceNetworkAddressRule").add(sourceNetworkAddressRule.detach());
			}
			if(destNetworkAddressRule!= null) {
				MRXMLUtils.getElementXpath(doc, "//Stream/data/destNetworkAddressRule").add(destNetworkAddressRule.detach());
			}
			
			MRXMLUtils.setValueXpath(doc, "//Stream/data/windowId",windowId);
			MRXMLUtils.setValueXpath(doc, "//Stream/data/canvasId",canvasId);
			
			Element groupTag = MRXMLUtils.getElementXpath(doc, "//streamGroups/Tag");
			MRXMLUtils.addAttribute(groupTag,MediaRoomServiceConst.NAME, MediaRoomServiceConst.TRACK);
			MRXMLUtils.addAttribute(groupTag,MediaRoomServiceConst.VALUE, trackNID);

			streamElement = MRXMLUtils.getElementXpath(doc, "//Stream");         
			
			MediaRoom room = this.getRoom(roomNID, request, log, db);
			room.addStreamElement(request, log, db, streamElement,mediaSourceNID);
			
			Element sourceElement = MRXMLUtils.getElementXpath(room.getRoomDoc(), "//MediaSource[@NID='" + mediaSourceNID + "']");
			MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
			if(observer != null)
				observer.handleStreamAdded(xmpp, request, log, db, streamElement, sourceElement);
		} catch (Exception e) {
			//TBD: Release multicast ipaddress
			throw e;
		}
        
        return streamElement;
    }


    public void updateTimeLine(XMPPI xmpp,MRRequest request,MRLog log,MRDB db, 
    		String roomNID, String trackNID, Document trackDataDoc, MediaTimeline timeline) throws Exception {
        MediaRoom room = this.getRoom(roomNID, request, log, db);
        String requestedAction = timeline.getLastRequestedAction();
	    Element mediaTimelineElement = MRXMLUtils.getElementXpath(trackDataDoc, "//TrackMediaTimeline");
	    MediaTimeline.updateMediaTimelineElement(mediaTimelineElement, 
	    		timeline.getLastActionWallClock(), timeline.getLastOffset(), 0, timeline.getLastTimescale(), 
	    		timeline.getLastAction(), timeline.getLastState(), timeline.getLastRequestedAction(), timeline.getLastSourceAction());

	    room.setObjectData(request, db, log, trackNID, MRXMLUtils.documentToString(trackDataDoc));
    	MediaRoomObserverI observer = this.roomObserverTable.get(room.getType());
    	if(observer != null)
    		observer.handleTimeLineChanged(xmpp, request, log, db, roomNID, trackNID, trackDataDoc, requestedAction);
    }
    
    public static Document getMediaRoomTemplate(MRLog log) throws MRException,Exception {
    	if(gMediaRoomElementTemplate == null) {
	    gMediaRoomElementTemplate = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_ROOM, log);
    	}
    	return gMediaRoomElementTemplate;
    }

    public static Document getStreamInstanceXML(MRLog log) throws MRException,Exception {	
    	return MRXMLUtils.getDocumentXpath(getMediaRoomTemplate(log), STREAM_INSTANCE_XPATH);
    }
    
    public static Document getMediaRelayInstanceXML(MRLog log) throws MRException,Exception {		
    	return MRXMLUtils.getDocumentXpath(getMediaRoomTemplate(log), MEDIARELAY_INSTANCE_XPATH);
    }
    
    public static Document getMediaDestInstanceXML(MRLog log) throws MRException,Exception {		
    	return MRXMLUtils.getDocumentXpath(getMediaRoomTemplate(log), MEDIADEST_INSTANCE_XPATH);
    }
    
    public static Document getMediaSourceInstanceXML(MRLog log) throws MRException,Exception {
    	return MRXMLUtils.getDocumentXpath(getMediaRoomTemplate(log), MEDIASOURCE_INSTANCE_XPATH);
    }
    
    public static Document getMediaRoomInstanceXML(MRLog log) throws MRException,Exception {
    	Document roomDoc = (Document) getMediaRoomTemplate(log).clone();
    	Element systemTrackEL = MRXMLUtils.getElementXpath(roomDoc, SYSTEM_TRACK_XPATH);
    	MRXMLUtils.removeContent(systemTrackEL);
    	return roomDoc;
    }
    
    public static Document getTrackInstanceXML(MRLog log,String trackName) throws MRException,Exception {
    	Document roomDoc = (Document) getMediaRoomTemplate(log).clone();
    	Document trackDoc = MRXMLUtils.getDocumentXpath(roomDoc, "//"+trackName);	
    	MRXMLUtils.removeContent(MRXMLUtils.getElementXpath(trackDoc, STREAM_LIST_XPATH));
    	MRXMLUtils.removeContent(MRXMLUtils.getElementXpath(trackDoc, MEDIA_DEST_LIST_XPATH));
    	MRXMLUtils.removeContent(MRXMLUtils.getElementXpath(trackDoc, MEDIA_RELAY_LIST_XPATH));
    	MRXMLUtils.removeContent(MRXMLUtils.getElementXpath(trackDoc, MEDIA_SOURCE_LIST_XPATH));	
    	return trackDoc;
    }
    
    /*
     * Overview of the Roster management
     * 1. AssetRoster maintains an assetTable(title based) for all Ready assets with pointer to corresponding roomNIDs
     * 2. AssetRoster maintains a table of unavailableAssetResourceNIDs(no title)
     * 3. MediaRoom maintains a list of unavailableAssetResourceNIDs
     * 4. When an asset is ready, it is added to the assetTable(title)
     * 5. - If it was part of a room, it is removed from the unavailableList(AssetRoster & MediaRoom) and assetTable is updated
     * Roster related functions : this has an assetTable of all the assets which are READY in one or more Rooms
     * sendInvite : if JID present addRoom to the asset's roster else add to unavailableList(in Roster and Room)
     * handleAssetResourceNIDAvailable : Remove from unavailableList(Roster and Room) and add to the assetTable 
     * publishAssetPresenceInRoom : Notify all peers in the room about the presence
     * handleAssetUnavailable : Remove from assetTable and add to unavailableList(in Roster and Room)
     * isReady : Checks if the asset is in the assetTable
     * handleRoomDeleted : Removes the roomFrom the AssetRoster
     */
    
    // Adds the roomNID to the asset's Roster
    public void sendInvite(XMPPI xmpp, MRLog log, String roomNID, String userJID, String assetResourceNID) throws Exception {
		// log.addInfo("RoomDB:sendInvite before room size=" + this.roomTable.size() + " AssetRoster size=" + this.roster.getSize());    	
    	if(!"".equals(userJID)) {
    		//xmpp.sendInvite(roomNID, userJID);
			this.roster.addRoomNID(xmpp,log, userJID, roomNID);
    	}
    	else if(!"".equals(assetResourceNID)) {
    		this.roster.addUnavailableAssetResourceNID(log, assetResourceNID, roomNID);
			MediaRoom room = this.roomTable.get(roomNID);
			room.addUnavailableAssetResourceNID(assetResourceNID);
    	}
		log.addInfo("RoomDB:sendInvite after room size=" + this.roomTable.size() + " AssetRoster size=" + this.roster.getSize());    	
    }

    // A previously referenced asset is now available so remove it form the list of unavailableAssets in the room
    public void handleAssetResourceNIDAvailable(XMPPI xmpp, MRLog log, String assetResourceNID, String userJID) throws Exception {
		// log.addInfo("RoomDB:handleAssetResourceNIDAvailable before room size=" + this.roomTable.size() + " AssetRoster size=" + this.roster.getSize());    	
		ArrayList<String> roomList = this.roster.handleAssetResourceNIDAvailable(xmpp, log, assetResourceNID, userJID);
		for(String roomNID : roomList) {
			MediaRoom room = this.roomTable.get(roomNID);
			if(room != null) {
				room.removeUnavailableAssetResourceNID(assetResourceNID);
			}
			else {
				log.addInfo("RoomDB:handleAssetResourceNIDAvailable userJID=" + userJID + " not found roomNIDe=" + roomNID);
			}
    	}
		log.addInfo("RoomDB:handleAssetResourceNIDAvailable after room size=" + this.roomTable.size() + " AssetRoster size=" + this.roster.getSize());    	

    }
    
    // Publish that the asset is ready to peers in the room
    public void publishAssetPresenceInRoom(XMPPI xmpp, MRLog log, String roomNID, String userJID) throws Exception {
    	// xmpp.sendInvite(roomNID, userJID);
    	this.roster.publishRoomPresenceToAsset(xmpp, log, roomNID, userJID);

    	MediaRoom room = this.roomTable.get(roomNID);
		this.roster.publishAssetPresenceInRoom(xmpp, log, userJID, room.roomdoc, AssetRoster.READY);
		log.addInfo("RoomDB:publishAssetPresenceInRoom userJID=" + userJID + " roomNID=" + roomNID + " AssetRoster size=" + this.roster.getSize());
    }


    // Publish that the asset is unavailable to peers in the room, move to unavailableAssetList and remove from assetTable
    public ArrayList<String> handleAssetUnavailable(XMPPI xmpp, MRLog log, String assetResourceNID, String userJID) throws Exception {
		// log.addInfo("RoomDB:handleAssetUnavailable before room size=" + this.roomTable.size() + " AssetRoster size=" + this.roster.getSize());
    	ArrayList<String> roomNIDList = this.roster.getRoomNIDList(userJID);
    	for(String roomNID : roomNIDList) {
    		MediaRoom room = this.roomTable.get(roomNID);
    		if(room == null) {
				log.addInfo("RoomDB:handleAssetResourceNIDAvailable userJID=" + userJID + " not found roomNIDe=" + roomNID);
    			continue;
    		}
    		this.roster.publishAssetPresenceInRoom(xmpp, log, userJID, room.roomdoc, AssetRoster.UNAVAILABLE);
    
    		// Move the assetResourceNID into the unavailableList for the MediaRoom and the AssetRoster
    		this.roster.addUnavailableAssetResourceNID(log, assetResourceNID, roomNID);
			room.addUnavailableAssetResourceNID(assetResourceNID);
			log.addInfo("RoomDB:handleAssetUnavailable room unavailableList roomNID=" + roomNID + " size=" + room.unavailableAssetResourceNIDList.size());
    	}
    	this.roster.removeAsset(log, userJID);
		log.addInfo("RoomDB:handleAssetUnavailable after room size=" + this.roomTable.size() + " AssetRoster size=" + this.roster.getSize());
    	return roomNIDList;
    }

    public ArrayList<String> getRoomNIDListForAsset(String userJID) {
    	//Make sure we always clone the array list. otherwise we will see concurrent modification exceptions. 
    	//To resolve concurrent modification exception better approach is use CopyOnWriteArrayList. 
    	//But this class is very expensive as all the time any update this will copy the array list. 
    	//This method only get called while device goes offline. Its ok to copy the array list instead of overloading memory every time.
    	return new ArrayList<String>(roster.getRoomNIDList(userJID));
    }

    public boolean isReady(XMPPI xmpp, String roomNID, String userJID, String myJID,String appServerJID) throws MRException,Exception {
    	boolean result = false;
    	if(myJID.equals(userJID) || appServerJID.equals(userJID))
    		result = true;
    	else
    		result = this.roster.isReady(userJID);
    	return result;
    }


}
