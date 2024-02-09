package com.barco.nimbus.sc;

import java.util.Collection;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;
import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.barco.nimbus.api.SCMessageHandlerI;
import com.barco.nimbus.api.SCServiceLocatorI;
import com.barco.nimbus.api.common.SCException;
import com.barco.nimbus.api.common.SCExceptionCodes;
import com.barco.nimbus.api.common.SCMessage;
import com.barco.nimbus.utils.common.XMLUtils;

public class SCMessageRunnable implements Runnable {
	SCMessage message;
	SCServiceLocatorI serviceLocator;
	BundleContext context;
	long startTime;
	
	SCMessageRunnable(SCMessage message, BundleContext context, SCServiceLocatorI serviceLocator) {
		this.message = message;
		this.context = context;
		this.serviceLocator = serviceLocator;
		this.startTime = System.currentTimeMillis();
	}

	
	private ServiceReference<SCMessageHandlerI> getServiceReference(SCMessage message) {
		String requestName = message.getRootName();
		
		// TBD : Override until the request handlers are implemented for SC 
		requestName = "Default";
		
		String filter = "(component.name=" + requestName + ")";
		Collection<ServiceReference<SCMessageHandlerI>> refs = null;
		try {
			refs = context.getServiceReferences(SCMessageHandlerI.class, filter);
		} catch (InvalidSyntaxException e) {
			Logger.getLogger(getClass()).error("Error while getServiceReference for messsage: " + message + e);
		}
		
		if((refs == null) || refs.isEmpty()) {
			return null;
		}
		
		return refs.iterator().next();
	}

	
	public void run() {
		
		ServiceReference<SCMessageHandlerI> ref = this.getServiceReference(message);
		if(ref == null) {
			handleError(message, new SCException(SCExceptionCodes.SYSTEMERROR, "SCMessageRunnable:run failed to find serviceReference for request=" + message.getRootName(), null, null));
			return;
		}
		SCMessageHandlerI handler = context.getService(ref);
		if(handler == null) {
			handleError(message, new SCException(SCExceptionCodes.SYSTEMERROR, "SCMessageRunnable:run failed to find handler for request=" + message.getRootName(), null, null));
			return;
		}
		
		long handlerStartTime = System.currentTimeMillis();
		try {
			handler.handleMessage(message, serviceLocator);
			handleResult(message);
		}
		catch(SCException se) {
			handleError(message, se);
		}
		catch(Exception e) {
			SCException scException = new SCException(SCExceptionCodes.SYSTEMERROR, e.toString(), e, null);
			handleError(message, scException);
		}
		long wc = System.currentTimeMillis();
		Logger.getLogger(SCMessageRunnable.class).info(":run " + message.getType() + "=" + message.getRootName() + " state=" + message.getState() + " from=" + message.getFrom() + 
				" totalTime=" + String.valueOf(wc - startTime) + " processTime=" + String.valueOf(wc - handlerStartTime));
		context.ungetService(ref);
	}

	private void handleResult(SCMessage message) throws Exception {
		if(!SCMessage.REQUEST.equals(message.getType()))
			return; // No response expected

		Document resultDoc = message.getResult();
		if(resultDoc == null) {
			return;
		}
		SCMessage responseMessage = this.serviceLocator.getMessageBus().createResponseUsingRequestMessage(message.getFrom(), message);
		this.serviceLocator.getMessageBus().sendMessage(responseMessage);
	}

	private void handleError(SCMessage message, SCException se) {
		Exception e = se;
		String data = se.toString();
		if(se.getOriginException() != null)
			e = se.getOriginException();
		if(se.getOriginData() != null)
			data = data + "\n" + se.getOriginData();
		Logger.getLogger(SCMessageRunnable.class).error(data, e);
		
		if(!SCMessage.REQUEST.equals(message.getType()))
			return; // No response expected
		try {
			Document result = XMLUtils.stringToDocument(se.toString());
			
			// Backwards Compatibility
			Element barcoError = XMLUtils.getElementXpath(result, "//BarcoError");
			Element barcoErrorLegacy = XMLUtils.stringToElement(
					"<Legacy>" + "<Code>" + se.getCode() + "</Code>"+"<Description>" + se.getData() + "</Description>"  +"<Log>" + se.getData() + "</Log>" +"</Legacy>");
			barcoError.add(barcoErrorLegacy);
			
			message.setError(result); 
			SCMessage responseMessage = this.serviceLocator.getMessageBus().createResponseUsingRequestMessage(message.getFrom(), message);
			this.serviceLocator.getMessageBus().sendMessage(responseMessage);
		}
		catch(Exception le) {
			Logger.getLogger(SCMessageRunnable.class).error("SCMessageRunnable:handleError  error=" + le.toString(), le);
		}
	}


}

