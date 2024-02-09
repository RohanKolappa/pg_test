package com.barco.device.handler.Media;


import java.net.URLDecoder;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MediaAgentI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.utils.mr.MRXMLUtils;

public class StartStreamRequestHandler implements MessageHandlerI{
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
        String requestNID = MRXMLUtils.getValueXpath(doc, "//requestNID", "");
        String streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");
        
        // Backward compatibility
        String streamdatadocStr = MRXMLUtils.getElementXpath(doc, "//streamdatadoc").asXML();
        Document streamDataDoc = DocumentHelper.parseText(URLDecoder.decode(streamdatadocStr, "UTF-8"));
        
        String mediasourcedatadocStr = MRXMLUtils.getElementXpath(doc, "//mediasourcedatadoc").asXML();
        Document mediaSourceDataDoc = DocumentHelper.parseText(URLDecoder.decode(mediasourcedatadocStr, "UTF-8"));

        MediaAgentI mediaAgentImpl = device.getMediaAgentImpl();
        Element streamElement = mediaAgentImpl.startStream(device, requestNID, streamNID, streamDataDoc, mediaSourceDataDoc);        
        message.setResult(MRXMLUtils.elementToDocument(streamElement));
        
        // Testing exception handling
        // throw new ServiceException(ServiceExceptionCodes.HANDLERERROR, "StartStreamRequestHandler:handleMessage error");
	}
	
}
