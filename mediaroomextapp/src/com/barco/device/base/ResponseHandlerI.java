package com.barco.device.base;

import org.dom4j.Document;

import com.barco.device.common.Message;

public interface ResponseHandlerI {
	
	void handleResponse(Message response, Document context, DeviceI device) throws Exception;
	void handleError(Message response, Document context, DeviceI device) throws Exception;

}
