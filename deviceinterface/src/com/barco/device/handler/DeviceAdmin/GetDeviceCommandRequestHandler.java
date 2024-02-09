package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;

public class GetDeviceCommandRequestHandler  implements MessageHandlerI{

	public void handleMessage(Message message, DeviceI device)
			throws Exception {		
		
    	Document deviceDoc = device.getDeviceAdminAgentImpl().getDeviceCommands();
    	
    	Document responseDoc = DocumentHelper.parseText("<GetDeviceCommandResponse/>"); 
    	responseDoc.add((Element)deviceDoc.getRootElement().clone());
    	message.setResult(responseDoc);
	}
	
	
}
