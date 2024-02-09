package com.barco.device.handler.Layout;


import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.StreamEntry;
import com.barco.utils.XMLUtils;

public class RelayEventHandler implements MessageHandlerI{

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
		// device.getLogger().info("SourceEventHandler:handleMessage xml=" + doc.asXML());

		Element header = XMLUtils.getElementXpath(doc, "//Header");
	//	int eventLevel = getIntValue(header.attributeValue("eventLevel"), 0);
	//	int eventCode = XMLUtils.getIntValue(doc, "//eventCode", 0);
		String streamNID = XMLUtils.getValueXpath(doc, "//streamNID", "");
		
		if("".equals(streamNID)) { // TBD in RelayEvent
			String mediaSourceNID = XMLUtils.getValueXpath(doc, "//mediaSourceNID", "");
			String agentJID = header.attributeValue("eventAgentJID");
			List<String> streamNIDList = device.getDeviceDB().getStreamCache().getStreamNIDListBySourceAgentJID(agentJID);
			for(String s : streamNIDList) {
				StreamEntry e = device.getDeviceDB().getStreamCache().getStreamEntry(s);
				String msNID = XMLUtils.getValueXpath(e.streamDataDoc, "//mediaSourceNID", "");
				if(mediaSourceNID.equals(msNID)) {
					streamNID = s;
					break;
				}
			}
		}
		
		StreamEntry se = device.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
		if(se == null)
			return;
		
		// String xml = se.status.asXML();
		//if(eventLevel == MediaAgentI.EVENT_LEVEL_OK) {
	        Element relayData = se.status.element("relayData");
	        Element relayDataInEvent = XMLUtils.getElementXpath(doc, "//RelayData");
	        String relayRoomNID = relayDataInEvent.attributeValue("roomNID");
	        String relayStreamNID = relayDataInEvent.attributeValue("streamNID");
	        relayData.addAttribute("relayRoomNID", relayRoomNID);
	        relayData.addAttribute("relayStreamNID", relayStreamNID);
		//}
		
		se.publishStatus(device, false);
	}
	
}
