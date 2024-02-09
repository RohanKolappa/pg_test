package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;

public class GetDeviceStatusRequestHandler  implements MessageHandlerI{

	public void handleMessage(Message message, DeviceI device)
			throws Exception {
		
		Document messageDocument = message.getMessageDoc();
		Element requestElement = (Element) DocumentHelper.createXPath("//GetDeviceStatusRequestData").selectSingleNode(messageDocument);
		String xpath = requestElement.attributeValue("xpath");
		
    	Document deviceDoc = device.getDeviceAdminAgentImpl().getStatus(xpath); 
    	Element deviceStatus = (Element)DocumentHelper.createXPath("//DeviceStatus").selectSingleNode(deviceDoc);
    	if(deviceStatus!=null) {
    		message.setResult(DocumentHelper.createDocument((Element)deviceStatus.clone()));	
    	}
	}
	
	
}
