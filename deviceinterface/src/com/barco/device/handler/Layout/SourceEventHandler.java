package com.barco.device.handler.Layout;


import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.StreamEntry;
import com.barco.utils.XMLUtils;

public class SourceEventHandler implements MessageHandlerI{

	int getIntValue(String value, int defaultValue) {
		try { 
			return Integer.parseInt(value);
		} catch (Exception e) {
			return defaultValue;
		}
	
	}
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		//device.getLogger().info("SourceEventHandler:handleMessage xml=" + doc.asXML());
		
		Element header = XMLUtils.getElementXpath(doc, "//Header");
		int eventLevel = getIntValue(header.attributeValue("eventLevel"), 0);
		int eventCode = XMLUtils.getIntValue(doc, "//eventCode", 0);
		String streamNID = XMLUtils.getValueXpath(doc, "//streamNID", "");
		StreamEntry se = device.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
		if(se == null)
			return;
		boolean expedite = false;
		if(eventLevel == MediaAgentI.EVENT_LEVEL_OK) {
			if(eventCode == MediaAgentI.STREAM_START_TC || eventCode == MediaAgentI.SOURCE_CONNECTED || eventCode == MediaAgentI.TIMELINEUPDATEEVENT) { // Only handling stated for now
				se.status.element("state").setText(String.valueOf(MediaAgentI.STREAM_STARTED));
				//System.out.println("SourceEventHandler.handleMessage: " + se.status.asXML()+":: Doc value is"+doc.asXML());
				if(eventCode == MediaAgentI.STREAM_START_TC || eventCode == MediaAgentI.TIMELINEUPDATEEVENT){
        				Element mediaTimeline = XMLUtils.getElementXpathFromElement(se.status, ".//MediaTimeline");
        				mediaTimeline.remove(mediaTimeline.element("Last"));
        				mediaTimeline.add(XMLUtils.getElementXpath(doc, "//Last").detach());
				}			
				if(eventCode != MediaAgentI.TIMELINEUPDATEEVENT)
					updateLastEventData(se, doc);
				expedite =true;
			}			
		}
		else if(eventLevel != MediaAgentI.EVENT_LEVEL_OK) {
			//return; // Not being handled
			//Added by Padmaja: We have to propagate error messages to SC if relay room has some problem
			se.status.element("state").setText(String.valueOf(MediaAgentI.SOURCE_ERROR));
			Element lastEventData = se.status.element("lastEventData");
			if(lastEventData != null)
				se.status.remove(lastEventData);
			Element errorElement = XMLUtils.getElementXpath(doc, "//Exception");
			if(errorElement!=null) {
				se.status.add(errorElement.detach());
			}
		}
		se.publishStatus(device, expedite);
	}
	
	
	private void updateLastEventData(StreamEntry se, Document doc) throws Exception {
		se.status.remove(se.status.element("lastEventData"));
		Element lastEventFromDoc = (Element) XMLUtils.getElementXpath(doc, "//Data").detach();
		lastEventFromDoc.setName("lastEventData");
		se.status.add(lastEventFromDoc);
	}	
}
