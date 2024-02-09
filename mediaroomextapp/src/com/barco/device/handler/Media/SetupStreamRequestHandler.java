package com.barco.device.handler.Media;



import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.LoggerI;
import com.barco.device.common.Message;
import com.barco.utils.mr.MRXMLUtils;

public class SetupStreamRequestHandler implements MessageHandlerI, ResponseHandlerI{
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
        
        // Generate the start request
        String requestXML = doc.asXML();
		String startRequestXML = requestXML.replace("SetupStreamRequest", "StartStreamRequest");

		// Send the startRequest to the sourceAgentJID
		Message startStreamRequestMessage = Message.parseMessage(startRequestXML);
		Document startRequestDoc = startStreamRequestMessage.getMessageDoc();
		String streamNID = MRXMLUtils.getValueXpath(startRequestDoc, "//streamNID", "");
		String sourceAgentJID = MRXMLUtils.getValueXpath(startRequestDoc, "//sourceAgentJID", "");
		device.getMediaAgentImpl().addDest(streamNID, sourceAgentJID);
		startStreamRequestMessage.setTo(sourceAgentJID);
		device.sendRequest(startStreamRequestMessage, startRequestDoc, this);
        message.setResult(MRXMLUtils.stringToDocument("<SetupStreamResponse/>"));
	}

	@Override
	public void handleResponse(Message message, Document context, DeviceI device)
			throws Exception {
		Element streamElement = MRXMLUtils.getElementXpath(message.getMessageDoc(), "//Stream");
		String streamNID = streamElement.attributeValue("NID");
		Element outputElement = MRXMLUtils.getElementXpath(message.getMessageDoc(), "//Output");
		String streamURL = outputElement.attributeValue("streamURL");
		device.addLog("SetupStreamRequestHandler:handleResponse streamURL=" + streamURL + " streamNID=" + streamNID,
				LoggerI.DEBUG, LoggerI.DEBUG);
		//device.addLog("SetupStreamRequestHandler:handleResponse xml=" + message.getMessageDoc().asXML(), LoggerI.DEBUG, LoggerI.DEBUG);
	}

	@Override
	public void handleError(Message response, Document context,
			DeviceI device) throws Exception {
		// TODO Auto-generated method stub
		
	}
	
}
