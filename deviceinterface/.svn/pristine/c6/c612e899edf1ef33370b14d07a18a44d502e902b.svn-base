package com.barco.device.base;

import com.barco.device.common.Message;

/**
 * When DeviceImpl.handleMessage needs to handle an event or message, it
 * uses reflection to instantiate a handler using Class.forName
 * handlerPackage + "." + message.getService() + "." + message.getRootName() + "Handler"
 * that is supposed to implement MeesgaHandlerI.
 */
public interface MessageHandlerI {
	
	void handleMessage(Message message, DeviceI device) throws Exception;

}
