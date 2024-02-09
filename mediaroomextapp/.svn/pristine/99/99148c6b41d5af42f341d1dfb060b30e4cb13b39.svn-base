package com.barco.device.handler.Layout;



import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.LoggerI;
import com.barco.device.common.Message;
import com.barco.device.handler.Media.SetupStreamRequestHandler;
import com.barco.utils.FilePath;
import com.barco.utils.FileUtils;
import com.barco.utils.mr.MRXMLUtils;

public class SetDisplayConfigRequestHandler implements MessageHandlerI, ResponseHandlerI {

	// TBD Remove the hack for getting the setupRequestXML
	
	private void sendStartRequest(Message message, DeviceI device) throws Exception {
        
        // TBD Generate the start request
		String startRequestXML = FileUtils.readFile(FilePath.startRequestPath);


		// Send the startRequest to the sourceAgentJID
		Message startStreamRequestMessage = Message.parseMessage(startRequestXML);
		Document startRequestDoc = startStreamRequestMessage.getMessageDoc();
		String sourceAgentJID = MRXMLUtils.getValueXpath(startRequestDoc, "//sourceAgentJID", "");
		startStreamRequestMessage.setTo(sourceAgentJID);
		device.sendRequest(startStreamRequestMessage, startRequestDoc, this);
        message.setResult(MRXMLUtils.stringToDocument("<SetDisplayConfigResponse/>"));
	}
	
	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception {
		Document doc = message.getMessageDoc();
		// Find the sources that need to be started and send start request for each source
		this.sendStartRequest(message, device);
	}

	@Override
	public void handleResponse(Message message, Document context, DeviceI device)
			throws Exception {
		Element outputElement = MRXMLUtils.getElementXpath(message.getMessageDoc(), "//Output");
		String streamURL = outputElement.attributeValue("streamURL");
		device.addLog("SetDisplayConfigRequestHandler:handleResponse streamURL=" + streamURL,
						LoggerI.DEBUG, LoggerI.DEBUG);

		//device.addLog("SetDisplayConfigRequestHandler:handleResponse xml=" + message.getMessageDoc().asXML(), 
		//		LoggerI.DEBUG, LoggerI.DEBUG);
	}

	@Override
	public void handleError(Message response, Document context,
			DeviceI device) throws Exception {
		// TODO Auto-generated method stub
		
	}
}
	
