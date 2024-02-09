package com.ipvs.mediaroomservice.impl;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRXMLUtils;

class AssetRoster {

    private Hashtable<String, ArrayList<String>> assetTable = new Hashtable<String, ArrayList<String>>(MediaRoomServiceConst.ASSET_INITIAL_CAPACITY);
    private Hashtable<String, ArrayList<String>> unavailableAssetResourceNIDTable = new Hashtable<String, ArrayList<String>>(MediaRoomServiceConst.ASSET_INITIAL_CAPACITY);
    public static String UNAVAILABLE = "Unavailable";
    public static String READY = "Ready";
    public static String ENTITY = "Entity";
    public static String MEDIAROOM = "MediaRoom";

    private String getAssetTitle(String userJID) {
    	StringTokenizer st = new StringTokenizer(userJID, "/");
    	if(!st.hasMoreTokens())
    		return null;
    	st.nextToken();
    	if(!st.hasMoreTokens())
    		return null;
    	return st.nextToken();
    }
    
    private void sendMessage(XMPPI xmpp, MRLog log,  ArrayList<String> toUserJIDList, String message) throws Exception {
    	for(String userJID : toUserJIDList) {
    		if(!xmpp.getXMPPUserJid().equals(userJID) && !this.isReady(userJID))
    			continue;
    		String roomNID = null;
    		String fromAgentClassName = AppServerAgent.class.getName();
    		String toAgentJID = userJID;
    		String serviceName = "*";
    		int eventLevel = MRLog.OK ;
    		String eventAgentJID = "managementserver.localhost";
    		long eventWallclock = System.currentTimeMillis();
    		String eventName = MediaRoomServiceConst.MEDIA_PRESENCE_EVENT;
    		Element origEventData = MRXMLUtils.stringToElement(message);
    		String logStr = "";
    		String info = "AssetRoster:sendMessage userJID=" + userJID;
    		ServiceAgentUtils.sendAgentEvent(xmpp, log, roomNID, fromAgentClassName, toAgentJID, serviceName, 
    				eventLevel, eventAgentJID, eventWallclock, eventName, origEventData, logStr, info);
	        // xmpp.sendMessage(userJID, dataStr, IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS);
			log.addInfo("AssetRoster:sendMessage toUserJID=" + userJID + " " + message);
    	}
    }
    
    private void sendEntityPresenceEvent(XMPPI xmpp, MRLog log, ArrayList<String> toUserJIDList, String presenceUserJID, String value) throws Exception {
		String presenceEvent = "<MediaPresence type='" + ENTITY + "' jid='" + presenceUserJID + "' value='" + value + "'/>";
		this.sendMessage(xmpp, log, toUserJIDList, presenceEvent);
    }

    private void sendRoomPresenceEvent(XMPPI xmpp, MRLog log, ArrayList<String> toUserJIDList, String roomNID, String value) throws Exception {
		String presenceEvent = "<MediaPresence type='" + MEDIAROOM + "' roomNID='" + roomNID + "' value='" + value + "'/>";
		this.sendMessage(xmpp, log, toUserJIDList, presenceEvent);
    }


    public void addUnavailableAssetResourceNID(MRLog log, String assetResourceNID, String roomNID) {
    	ArrayList<String> roomNIDList = this.unavailableAssetResourceNIDTable.get(assetResourceNID);
    	if(roomNIDList == null) {
    		roomNIDList = new ArrayList<String>();
    		this.unavailableAssetResourceNIDTable.put(assetResourceNID, roomNIDList);
    	}
    	if(!roomNIDList.contains(roomNID))
    		roomNIDList.add(roomNID);
		log.addInfo("AssetRoster:addUnavailableAssetResourceNID assetResourceNID=" + assetResourceNID + " roomNID=" + roomNID + " size=" + this.unavailableAssetResourceNIDTable.size());
    }
    
    private void removeUnavailableAssetResourceNID(MRLog log, String roomNID, String assetResourceNID) {
    	ArrayList<String> roomNIDList = this.unavailableAssetResourceNIDTable.get(assetResourceNID);
    	if(roomNIDList == null)
    		return;
		roomNIDList.remove(roomNID);
		if(roomNIDList.isEmpty())
			this.unavailableAssetResourceNIDTable.remove(assetResourceNID);
		log.addInfo("AssetRoster:removeUnavailableAssetResourceNID assetResourceNID=" + assetResourceNID + " roomNID=" + roomNID + " size=" + this.unavailableAssetResourceNIDTable.size());
    }

    // This will move entries from the unavailableAssetResourceNIDTable to the assetTable
    public ArrayList<String> handleAssetResourceNIDAvailable(XMPPI xmpp, MRLog log, String assetResourceNID, String userJID) throws Exception {
    	if(!this.unavailableAssetResourceNIDTable.containsKey(assetResourceNID)) { // Store this in the assetTable 
        	ArrayList<String> roomNIDList = new ArrayList<String>();
    		return roomNIDList;
    	}
    	
    	ArrayList<String> roomNIDList = this.unavailableAssetResourceNIDTable.get(assetResourceNID);
    	this.unavailableAssetResourceNIDTable.remove(assetResourceNID);
    	
    	String assetTitle = getAssetTitle(userJID);
    	this.assetTable.put(assetTitle, roomNIDList);
    	
    	/* This is to be done from GetMediaRoomInvites
    	// Send the room invites to the userJID
		ArrayList<String> userJIDList = new ArrayList<String>();
		userJIDList.add(userJID);
		for(String roomNID : roomNIDList) {
			try {
				sendRoomPresenceEvent(xmpp, log, userJIDList, roomNID, AssetRoster.READY);
			}
			catch(Exception e) {
				log.addInfo("AssetRoster:add sendRoomAvailable userJID=" + userJID + " roomNID=" + roomNID + " exception=" + e.toString());
			}
		}
    	
    	*/
    	
		log.addInfo("AssetRoster:handleAssetResourceNIDAvailable assetResourceNID='" + assetResourceNID + "' userJID=" + userJID + " AssetRoster size=" + this.assetTable.size());
		return roomNIDList;
    }
    
    public void publishRoomPresenceToAsset(XMPPI xmpp, MRLog log, String roomNID, String userJID) throws Exception {
		ArrayList<String> userJIDList = new ArrayList<String>();
		userJIDList.add(userJID);
		sendRoomPresenceEvent(xmpp, log, userJIDList, roomNID, AssetRoster.READY);    	
    }

    public void removeAsset(MRLog log, String userJID) {
		log.addInfo("AssetRoster:removeAsset userJID=" + userJID + " AssetRoster size=" + this.assetTable.size());
    	String assetTitle = getAssetTitle(userJID);
    	this.assetTable.remove(assetTitle);
    }
    
    public boolean isReady(String userJID) {
    	String assetTitle = getAssetTitle(userJID);
    	if(assetTitle == null)
    		return false;
    	return this.assetTable.containsKey(assetTitle);
    }

    public void addRoomNID(XMPPI xmpp, MRLog log, String userJID, String roomNID) {
    	String assetTitle = getAssetTitle(userJID);
    	if(assetTitle == null)
    		return;
    	ArrayList<String> roomNIDList = this.assetTable.get(assetTitle);
    	if(roomNIDList == null) {
    		roomNIDList = new ArrayList<String>();
    		this.assetTable.put(assetTitle, roomNIDList);
    	}
    	if(!roomNIDList.contains(roomNID)) {
    		ArrayList<String> userJIDList = new ArrayList<String>();
    		userJIDList.add(userJID);
    		try {
    			sendRoomPresenceEvent(xmpp, log, userJIDList, roomNID, AssetRoster.READY);
    		}
    		catch(Exception e) {
    			log.addInfo("AssetRoster:add sendRoomAvailable userJID=" + userJID + " roomNID=" + roomNID + " exception=" + e.toString());
    		}
    		roomNIDList.add(roomNID);
    	}
		log.addInfo("AssetRoster:add userJID=" + userJID + " roomNID=" + roomNID + " size=" + this.assetTable.size());
    }
    
    private void removeRoomNID(MRLog log, String userJID, String roomNID) {
    	String assetTitle = getAssetTitle(userJID);
    	if(assetTitle == null)
    		return;
    	if(!this.assetTable.containsKey(assetTitle))
    		return;
    	ArrayList<String> roomNIDList = this.assetTable.get(assetTitle);
		roomNIDList.remove(roomNID);
		if(roomNIDList.isEmpty())
			this.assetTable.remove(assetTitle);
		log.addInfo("AssetRoster:remove userJID=" + userJID + " roomNID=" + roomNID + " size=" + this.assetTable.size());
    }
    
    public String getSize() {
    	/*
    	StringBuffer sb = new StringBuffer();
    	Enumeration<String> an = this.assetTable.keys();
    	while(an.hasMoreElements()) {
    		String title = an.nextElement();
    		sb.append("+" + title + ":");
    		ArrayList<String> roomNIDList = this.assetTable.get(title);
    		Iterator<String> ri = roomNIDList.iterator();
    		while(ri.hasNext())
    			sb.append(ri.next() + ",");
    			
    	}
    	return this.assetTable.size() + "/" + this.unavailableAssetResourceNIDTable.size() + " : " + sb.toString();
    	*/
    	return this.assetTable.size() + "/" + this.unavailableAssetResourceNIDTable.size();
    }
    
    // TBD Nimbus
    public void handleRoomDeleted(XMPPI xmpp, MRLog log, String roomNID, Document roomdoc, ArrayList<String> assetResourceNIDList) {
    	try {
    		// String s = roomdoc.asXML();
    		List<?> streamList = MRXMLUtils.getListXpath(roomdoc, "//Stream");
    		ArrayList<String> userJIDList = new ArrayList<String>();
    		for(Object o : streamList) {
    			Element stream = (Element)o;
    			String sourceAgentJID= MRXMLUtils.getValueXpath(stream, ".//sourceAgentJID", "");
    			String destAgentJID= MRXMLUtils.getValueXpath(stream, ".//destAgentJID", "");
    			if(!userJIDList.contains(sourceAgentJID))
    				userJIDList.add(sourceAgentJID);
    			if(!userJIDList.contains(destAgentJID))
    				userJIDList.add(destAgentJID);
    		}
    		List<?> sourceList = MRXMLUtils.getListXpath(roomdoc, "//MediaSource");
    		for(Object o : sourceList) { // Relayed Sources
    			Element mediasource = (Element)o;
    			String sourceAgentJID= MRXMLUtils.getValueXpath(mediasource, ".//sourceAgentJID", "");
    			if(!userJIDList.contains(sourceAgentJID))
    				userJIDList.add(sourceAgentJID);
    		}
			sendRoomPresenceEvent(xmpp, log, userJIDList, roomNID, AssetRoster.UNAVAILABLE );

    		for(String userJID : userJIDList) {
    			removeRoomNID(log, userJID, roomNID);
    		}
    		for(String assetResourceNID : assetResourceNIDList) {
    			this.removeUnavailableAssetResourceNID(log, roomNID, assetResourceNID);
    		}
    	}
    	catch(Exception e) {
    		log.addInfo("AssetRoster:handleRoomDeleted    exception=" + e.toString());    		
    	}    	
    }
    
//ask for explaination
    public void publishAssetPresenceInRoom(XMPPI xmpp, MRLog log, String presenceUserJID, Document roomdoc, String value)  {
		String xpath = "//Stream/data[sourceAgentJID='"+ presenceUserJID + "' or destAgentJID='" + presenceUserJID + "']";

		//to avoid duplicates in the user list going with "Set" collection. This will make sure its sending only one presence event to each user.
		Set<String> toUserJIDList = new HashSet<String>();
		try {
			List<?> streamDataList = MRXMLUtils.getListXpath(roomdoc, xpath);
			for(Object o : streamDataList) {
				Element streamData = (Element)o;
				String sourceAgentJID= (streamData.element("sourceAgentJID")).getText();
				String destAgentJID= (streamData.element("destAgentJID")).getText();
				if(!presenceUserJID.equals(sourceAgentJID))
					toUserJIDList.add(sourceAgentJID);
				else if(!presenceUserJID.equals(destAgentJID))
					toUserJIDList.add(destAgentJID);
			}
			this.sendEntityPresenceEvent(xmpp, log, new ArrayList<String>(toUserJIDList), presenceUserJID, value);
		}
		catch(Exception e) {
			log.addInfo("AssetRoster:handleUserUnavailable stream xpath exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
		}
    }

	public ArrayList<String> getRoomNIDList(String userJID) {
		String assetTitle = getAssetTitle(userJID);
		if( (assetTitle == null) || !this.assetTable.containsKey(assetTitle))
			return new ArrayList<String>();
		return this.assetTable.get(assetTitle);
	}


}
