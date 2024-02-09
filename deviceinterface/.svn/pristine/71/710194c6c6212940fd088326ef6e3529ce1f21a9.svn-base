package com.barco.device.handler.DeviceAdmin;

import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

public class ActiveDeviceConfigUpdateRequestHandler implements MessageHandlerI {
   
    public void handleMessage(Message message, DeviceI device)
	    throws Exception {
	Element requestElement = XMLUtils.getElementXpath(message.getMessageDoc(), "//ActiveDeviceConfigUpdate");				
	
	device.getDeviceAdminAgentImpl().handleConfigUpdate(device, requestElement);
    }

 
}
