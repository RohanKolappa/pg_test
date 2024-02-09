package com.barco.device.handler.Layout;



import org.dom4j.Document;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.device.test.TestConst;
import com.barco.utils.XMLUtils;

public class SetDisplayConfigRequestHandler implements MessageHandlerI, ResponseHandlerI {

	// TBD Remove the hack for getting the setupRequestXML
	
	private void sendStartRequest(Message message, DeviceI device) throws Exception {
        
        // TBD Generate the start request
		String startRequestXML = XMLUtils.loadXMLResource(TestConst.StartRequestPath).asXML();


		// Send the startRequest to the sourceAgentJID
		Message startStreamRequestMessage = Message.parseMessage(startRequestXML);
		Document startRequestDoc = startStreamRequestMessage.getMessageDoc();
		String sourceAgentJID = XMLUtils.getValueXpath(startRequestDoc, "//sourceAgentJID", "");
		startStreamRequestMessage.setTo(sourceAgentJID);
		device.sendRequest(startStreamRequestMessage, startRequestDoc, this);
        message.setResult(XMLUtils.stringToDocument("<SetDisplayConfigResponse/>"));
	}
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		// Document doc = message.getMessageDoc();
		// Find the sources that need to be started and send start request for each source
		this.sendStartRequest(message, device);
	}

	@Override
	public void handleResponse(Message message, Document context, DeviceI device)
			throws Exception {
		// Element statusElement = XMLUtils.getElementXpath(message.getMessageDoc(), "//StreamStatus");
		// String streamNID = statusElement.attributeValue("NID");
		String URL = XMLUtils.getValueXpath(message.getMessageDoc(), "//URL", "");
		device.getLogger().debug("SetDisplayConfigRequestHandler:handleResponse streamURL=" + URL);
		//device.addLog("SetDisplayConfigRequestHandler:handleResponse xml=" + message.getMessageDoc().asXML(), 
		//		LoggerI.DEBUG, LoggerI.DEBUG);
	}

	@Override
	public void handleError(Message response, Document context,
			DeviceI device) throws Exception {
		// TODO Auto-generated method stub
		
	}
}
	
