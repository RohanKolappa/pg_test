package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;

public class ShutdownDeviceRequestHandler  implements MessageHandlerI{

	public void handleMessage(Message message, DeviceI device)
			throws Exception {		
		
    	device.getDeviceAdminAgentImpl().shutdown();    	
    	Document responseDoc = DocumentHelper.parseText("<ShutdownDeviceResponse/>"); 
    	message.setResult(responseDoc);
	}
	
	
}