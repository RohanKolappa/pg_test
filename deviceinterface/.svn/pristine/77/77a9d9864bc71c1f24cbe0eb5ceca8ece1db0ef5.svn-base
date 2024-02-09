package com.barco.device.handler.Media;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class StreamStatusEventHandler implements MessageHandlerI{
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		// Document doc = message.getMessageDoc();
		Document doc = message.getMessageDoc();
		Element streamStus = XMLUtils.getElementXpath(doc, "//StreamStatus");
		String roomNID = streamStus.attributeValue("roomNID");
		String streamNID = streamStus.attributeValue("streamNID");
		String state = XMLUtils.getValueXpath(doc, "//state", "");
		device.getLogger().info("StreamStatusEventHandler:handleMessage roomNID.streamNID.state=" + roomNID + "." + streamNID + "." +  state);
	}
	
}
