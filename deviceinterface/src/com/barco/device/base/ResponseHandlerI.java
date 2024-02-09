package com.barco.device.base;

import org.dom4j.Document;

import com.barco.device.common.Message;

/**
 * When DeviceImpl.handleMessage gets a message.type == Message.RESPONSE
 * it instantiates an instance of ReponseHandlerI (using Class.forName
 * on Message.clientCallback which was set by the request sender).
 */
public interface ResponseHandlerI {
	
	void handleResponse(Message response, Document context, DeviceI device) throws Exception;
	void handleError(Message response, Document context, DeviceI device) throws Exception;

}
