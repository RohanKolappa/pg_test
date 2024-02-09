package com.barco.device.handler.Media;


import org.dom4j.Document;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class StopRequestHandler implements MessageHandlerI{
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
        // String requestNID = XMLUtils.getValueXpath(doc, "//requestNID", "");
        String streamNID = XMLUtils.getValueXpath(doc, "//streamNID", "");
        MediaAgentI mediaAgentImpl = device.getMediaAgentImpl();
        Document streamStatusDoc = mediaAgentImpl.stopStream(streamNID);
        message.setResult(streamStatusDoc);
	}
	
}
