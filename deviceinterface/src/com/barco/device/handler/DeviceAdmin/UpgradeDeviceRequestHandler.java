package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;

public class UpgradeDeviceRequestHandler  implements MessageHandlerI{

	public void handleMessage(Message message, DeviceI device)
			throws Exception {
		
		Document messageDocument = message.getMessageDoc();
		Element requestElement = (Element) DocumentHelper.createXPath("//UpgradeDevice").selectSingleNode(messageDocument);
		String url = requestElement.elementText("URL");		
    	device.getDeviceAdminAgentImpl().upgrade(url); 	
    	Document responseDoc = DocumentHelper.parseText("<UpgradeDeviceResponse/>"); 
    	message.setResult(responseDoc);
	}
	
	
}
