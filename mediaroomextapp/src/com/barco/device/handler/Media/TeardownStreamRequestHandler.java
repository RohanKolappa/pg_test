package com.barco.device.handler.Media;



import org.dom4j.Document;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MediaAgentI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.LoggerI;
import com.barco.device.common.Message;
import com.barco.device.handler.Media.impl.BaseMediaAgentImpl;
import com.barco.device.xmpp.XMPPMessageBus;
import com.barco.utils.mr.MRXMLUtils;

public class TeardownStreamRequestHandler implements MessageHandlerI, ResponseHandlerI{

	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
        
        // Generate the stop request
        String requestXML = doc.asXML();
		String stopRequestXML = requestXML.replace("TeardownStreamRequest", "StopStreamRequest");

		// Send the stopRequest to the sourceAgentJID
		Message stopStreamRequestMessage = Message.parseMessage(stopRequestXML);
		Document stopRequestDoc = stopStreamRequestMessage.getMessageDoc();
		String streamNID = MRXMLUtils.getValueXpath(stopRequestDoc, "//streamNID", "");

		String sourceAgentJID = device.getMediaAgentImpl().removeDest(streamNID);
		stopStreamRequestMessage.setTo(sourceAgentJID);
		device.sendRequest(stopStreamRequestMessage, stopRequestDoc, this);
        message.setResult(MRXMLUtils.stringToDocument("<TeardownStreamResponse/>"));
	}

	@Override
	public void handleResponse(Message message, Document context, DeviceI device)
			throws Exception {
		// device.addLog("TeardownStreamRequestHandler:handleResponse xml=" + message.getMessageDoc().asXML(), LoggerI.DEBUG);
	}
	@Override
	public void handleError(Message response, Document context,
			DeviceI device) throws Exception {
		// TODO Auto-generated method stub
		
	}
	
}
