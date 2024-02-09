package com.barco.device.handler.Media;

import java.util.ArrayList;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class MediaPresenceEventHandler implements MessageHandlerI{

	protected void stopStreamList(Message message, DeviceI device, ArrayList<String> streamNIDList) {
		for(String streamNID : streamNIDList) {
			try {
				device.getMediaAgentImpl().stopStream(streamNID);			
			}
			catch(Exception e) {
				device.getLogger().info("Media.MediaPresenceEventHandler:stopStreamList streamNID=" + streamNID + " error=" + e.toString());		
			}
		}
	}
	
	protected void handleRoomPresence(Message message, DeviceI device, String roomNID, String value) {
		device.getLogger().info("Media.MediaPresenceEventHandler:handleRoomPresence value=" + value + " roomNID=" + roomNID);
		if(MediaAgentI.READY.equals(value)) {
			device.getMediaAgentImpl().addRoomNID(roomNID);
		}
		else {
			device.getMediaAgentImpl().deleteRoomNID(roomNID);
			ArrayList<String> streamNIDList = device.getMediaAgentImpl().getStreamNIDListByRoomNID(roomNID);
			this.stopStreamList(message, device, streamNIDList);
		}
	}
	
	protected void handleEntityPresence(Message message, DeviceI device, String jid, String value) {
		if(MediaAgentI.UNAVAILABLE.equals(value)) {
			device.getLogger().info("Media.MediaPresenceEventHandler:handleEntityPresence value=" + value + " jid=" + jid);
			ArrayList<String> streamNIDList = device.getMediaAgentImpl().getStreamNIDListByDestAgentJID(jid);
			this.stopStreamList(message, device, streamNIDList);
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
