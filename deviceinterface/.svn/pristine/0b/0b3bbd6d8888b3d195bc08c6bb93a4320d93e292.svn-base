package com.barco.device.handler.Layout;

import java.util.ArrayList;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.StreamEntry;
import com.barco.utils.XMLUtils;

public class MediaPresenceEventHandler implements MessageHandlerI {
	
	protected void handleRoomPresence(Message message, DeviceI device, String roomNID, String value) {
		device.getLogger().info("Layout.MediaPresenceEventHandler:handleRoomPresence value=" + value + " roomNID=" + roomNID);
		if(MediaAgentI.UNAVAILABLE.equals(value)) {
			TeardownRequestHandler handler = new TeardownRequestHandler();
			ArrayList<String> streamNIDList = device.getDeviceDB().getStreamCache().getStreamNIDListByRoomNID(roomNID);
			for(String streamNID : streamNIDList) {
				try {
					handler.teardownStream(device, streamNID);
				}
				catch(Exception e) {
					device.getLogger().info("Layout.MediaPresenceEventHandler:handleRoomPresence streamNID=" + streamNID + " error=" + e.toString());		
				}
			}
		}
	}

	
	protected void handleEntityPresence(Message message, DeviceI device, String jid, String value) {
		ArrayList<String> streamNIDList = device.getDeviceDB().getStreamCache().getStreamNIDListBySourceAgentJID(jid);
		for(String streamNID : streamNIDList) {
			StreamEntry se = device.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
			if(MediaAgentI.UNAVAILABLE.equals(value)) {
				TeardownRequestHandler handler = new TeardownRequestHandler();
				handler.stopPlaying(device, streamNID);			
				try {	
					se.setStatus(MediaAgentI.WAITING_FOR_SOURCE, device, true);						
				} catch (Exception e) {
					device.getLogger().error("HandleEntityPresence: Publish Source Unavailable message " + e.toString());
				}				
			}
			else if(MediaAgentI.READY.equals(value)) {
				try {
					SetupRequestHandler handler = new SetupRequestHandler();
					handler.sendStartRequest(device, streamNID,MediaAgentI.WAITING_FOR_STREAMURL);
				}
				catch(Exception e) {
					device.getLogger().info("Layout.MediaPresenceEventHandler:handleRoomPresence streamNID=" + streamNID + " error=" + e.toString());		
				}
			}
		}
	}
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		Element presence = XMLUtils.getElementXpath(doc, "//" + MediaAgentI.MEDIAPRESENCEEVENTDATA);
		String type = presence.attributeValue(MediaAgentI.TYPE);
		String value = presence.attributeValue(MediaAgentI.VALUE);
		
		if(MediaAgentI.MEDIAROOM.equals(type))
			this.handleRoomPresence(message, device, presence.attributeValue(MediaAgentI.ROOMNID), value);
		else 
			this.handleEntityPresence(message, device, presence.attributeValue(MediaAgentI.JID), value);
	}


}
