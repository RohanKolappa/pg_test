package com.barco.nimbus.handler.mediaroom;

import org.dom4j.Document;
import org.osgi.service.component.annotations.Component;

import com.barco.nimbus.api.MRComponentI;
import com.barco.nimbus.api.SCMessageHandlerI;
import com.barco.nimbus.api.SCServiceLocatorI;
import com.barco.nimbus.api.common.SCException;
import com.barco.nimbus.api.common.SCMessage;

@Component(name = "Default")
public class DefaultRequestHandler implements SCMessageHandlerI{

	@Override
	public void handleMessage(SCMessage message, SCServiceLocatorI service)
			throws SCException, Exception {
		// Example of accessing the service - will be done within the handlers
		// Logger.getLogger(DefaultRequestHandler.class).debug("DefaultRequestHandler:getResponse");
		Document doc = message.getMessageDoc();
		long wc = System.currentTimeMillis();
		MRComponentI mrComponent = service.getMRComponent();
		mrComponent.handleMessage(message);
	}
}
