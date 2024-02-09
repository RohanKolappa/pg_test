package com.barco.nimbus.handler.system;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.osgi.service.component.annotations.Component;

import com.barco.nimbus.api.SCMessageHandlerI;
import com.barco.nimbus.api.SCServiceLocatorI;
import com.barco.nimbus.api.common.SCException;
import com.barco.nimbus.api.common.SCMessage;
import com.barco.nimbus.utils.common.XMLUtils;

@Component(name = "DeviceLogin")
public class DeviceLoginRequestHandler implements SCMessageHandlerI{


	@Override
	public void handleMessage(SCMessage message, SCServiceLocatorI service)
			throws SCException, Exception {
		// Example of accessing the service - will be done within the handlers
		Logger.getLogger(DeviceLoginRequestHandler.class).debug("DeviceLoginRequestHandler:getResponse");
		Document doc = message.getMessageDoc();
		long wc = System.currentTimeMillis();
		message.setResult(XMLUtils.stringToDocument(
				"<DeviceLoginResponseData wc=" + String.valueOf(wc) + "/>"));	
	}
}
