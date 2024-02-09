package com.barco.device.handler.Media;


import java.net.URLDecoder;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MediaAgentI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.handler.Media.impl.BaseMediaAgentImpl;
import com.barco.utils.mr.MRXMLUtils;

public class StopStreamRequestHandler implements MessageHandlerI{
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
        String requestNID = MRXMLUtils.getValueXpath(doc, "//requestNID", "");
        String streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");        
        MediaAgentI mediaAgentImpl = device.getMediaAgentImpl();
        Element streamElement = mediaAgentImpl.stopStream(streamNID);
        message.setResult(MRXMLUtils.elementToDocument(streamElement));
	}
	
}
