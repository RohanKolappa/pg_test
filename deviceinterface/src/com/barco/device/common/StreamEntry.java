package com.barco.device.common;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.utils.XMLUtils;

public class StreamEntry {
	public String streamNID;
	public String destPortElementID;
	public String roomNID;
	public String sourceAgentJID;
	public Document streamDataDoc;
	public Document mediasourceDataDoc;
	public boolean preempt;
	public Element status;
	public StreamEntry(String streamNID, String destPortElementID, String roomNID, String sourceAgentJID, 
			Document streamDataDoc, Document mediasourceDataDoc, boolean preempt, Element status) {
		this.streamNID = streamNID;
		this.destPortElementID = destPortElementID;
		this.roomNID = roomNID;
		this.sourceAgentJID = sourceAgentJID;
		this.streamDataDoc = streamDataDoc;
		this.mediasourceDataDoc = mediasourceDataDoc;
		this.preempt = preempt;
		this.status = status;
	}
	
	
	public void setStatus(int status,DeviceI device,boolean expedite) throws Exception {
		this.status.element("state").setText(String.valueOf(status));
		publishStatus(device, expedite);
	}
	
	
	public void publishStatus(DeviceI device,boolean expedite) throws Exception {
		Element streamStatusEventElement = (Element)status.clone();
		streamStatusEventElement.setName(MediaAgentI.STREAMSTATUSEVENT + "Data");
		streamStatusEventElement.addAttribute("roomNID", roomNID);
		streamStatusEventElement.addAttribute("streamNID", streamNID);
		Message message = Message.createEvent(streamStatusEventElement,  
								device.getAppserverJID(), device.getMyJID(),
								MediaAgentI.PUBSUB_SERVICE, MediaAgentI.STREAMSTATUSEVENT, 
								String.valueOf(MediaAgentI.EVENT_LEVEL_OK), Level.INFO);
		int state = 0;
		try {
			state = Integer.parseInt(XMLUtils.getValueXpath(streamStatusEventElement, "//state", ""));
		}
		catch(Exception e) { 
			// Nothing to do
		}
		
		device.getLogger().info("StreamEntry:publishStatus state=" + state +" streamNID:="+streamNID);
		
		if(expedite)
			device.sendNotification(streamNID, message);
		else
			device.queueNotification(streamNID, message);
	}
	
	
}

