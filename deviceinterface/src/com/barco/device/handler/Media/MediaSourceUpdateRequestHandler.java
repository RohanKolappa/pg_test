package com.barco.device.handler.Media;

import org.dom4j.Document;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;

public class MediaSourceUpdateRequestHandler implements MessageHandlerI {

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		MediaAgentI mediaAgentImpl = device.getMediaAgentImpl();
		mediaAgentImpl.handleMediaSourceUpdate(device, doc);
	}

}
