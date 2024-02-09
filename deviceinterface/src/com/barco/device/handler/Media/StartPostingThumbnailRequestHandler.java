package com.barco.device.handler.Media;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class StartPostingThumbnailRequestHandler implements MessageHandlerI {

    @Override
    public void handleMessage(Message message, DeviceI device) throws Exception {
	device.getLogger().info("StartPostingThumbnailRequestHandler:: handleMessage Begin");
	Document doc = message.getMessageDoc();
	Element requestEl = XMLUtils.getElementXpath(doc, "//StartPostingThumbnailRequest");
	String sourceNID = XMLUtils.getAttributeValueWithDefault(requestEl, "sourceNID", "");
	String portId = XMLUtils.getAttributeValueWithDefault(requestEl, "portId", "");
	String type = XMLUtils.getAttributeValueWithDefault(requestEl, "type", "");	
	String publishURL = XMLUtils.getAttributeValue(requestEl, "publishURL");
	device.getLogger().info("StartPostingThumbnailRequestHandler:: type is: "+type);
	if("live".equals(type.trim())){
	    MediaAgentI mediaAgentImpl = device.getMediaAgentImpl();
	    mediaAgentImpl.startPostingThumbnail(sourceNID, portId, publishURL);
	}
	else{
	    MediaStoreAgentI mediaStoreAgentImpl = device.getMediaStoreAgentImpl();
	    mediaStoreAgentImpl.startPostingThumbnail(sourceNID, portId, publishURL);
	}
	device.getLogger().info("StartPostingThumbnailRequestHandler:: handleMessage End");

    }

}
