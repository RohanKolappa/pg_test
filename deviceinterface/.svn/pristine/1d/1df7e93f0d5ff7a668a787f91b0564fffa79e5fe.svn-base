package com.barco.device.handler.Media;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class StopPostingThumbnailRequestHandler implements MessageHandlerI{

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		device.getLogger().info("StopPostingThumbnailRequestHandler:: handleMessage Begin");
		Document doc = message.getMessageDoc();
		Element requestEl = XMLUtils.getElementXpath(doc, "//StopPostingThumbnailRequest");
		String sourceNID = XMLUtils.getAttributeValueWithDefault(requestEl, "sourceNID", "");
		String portId = XMLUtils.getAttributeValueWithDefault(requestEl, "portId", "");
		MediaAgentI mediaAgentImpl = device.getMediaAgentImpl();
        mediaAgentImpl.stopPostingThumbnail(sourceNID, portId);        
        device.getLogger().info("StopPostingThumbnailRequestHandler:: handleMessage End");
	}

}
