package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;

public class RevertToDefaultDeviceRequestHandler  implements MessageHandlerI{

	public void handleMessage(Message message, DeviceI device)
			throws Exception {
		
		Document messageDocument = message.getMessageDoc();
		Element requestElement = (Element) DocumentHelper.createXPath("//RevertToDefaultDevice").selectSingleNode(messageDocument);
		String keepMedia = requestElement.attributeValue("keepMedia");		
		String keepNetworkSettings = requestElement.attributeValue("keepNetworkSettings");
		boolean keepMediaBoolean = Boolean.getBoolean(keepMedia);
		boolean keepNetworkSettingsBoolean = Boolean.getBoolean(keepNetworkSettings);		
    
    	device.getDeviceAdminAgentImpl().revertToDefault(keepNetworkSettingsBoolean, keepMediaBoolean); 	
    	Document responseDoc = DocumentHelper.parseText("<RevertToDefaultDeviceResponse/>"); 
    	message.setResult(responseDoc);
	}
	
	
}
