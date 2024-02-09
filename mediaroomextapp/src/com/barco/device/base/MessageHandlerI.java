package com.barco.device.base;

import com.barco.device.common.Message;

public interface MessageHandlerI {
	
	void handleMessage(Message message, DeviceI device) throws Exception;

}
