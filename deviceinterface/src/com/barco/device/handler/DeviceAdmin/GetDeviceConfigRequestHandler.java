package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;

public class GetDeviceConfigRequestHandler  implements MessageHandlerI{

	public void handleMessage(Message message, DeviceI device)
			throws Exception {
		
		Document messageDocument = message.getMessageDoc();
		Element requestElement = (Element) DocumentHelper.createXPath("//GetDeviceConfigRequestData").selectSingleNode(messageDocument);
		String xpath = "";
		if(requestElement != null)
		  xpath = requestElement.attributeValue("xpath","");
		
    	Document deviceDoc = device.getDeviceAdminAgentImpl().getDeviceConfig(xpath);
    	message.setResult(deviceDoc);
	}
	
	
}
