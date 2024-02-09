package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;

public class SaveConfigDeviceRequestHandler  implements MessageHandlerI{

	public void handleMessage(Message message, DeviceI device)
			throws Exception {		
		
    	device.getDeviceAdminAgentImpl().saveConfiguration(); 	
    	Document responseDoc = DocumentHelper.parseText("<SaveConfigDeviceResponse/>"); 
    	message.setResult(responseDoc);
	}
	
	
}
