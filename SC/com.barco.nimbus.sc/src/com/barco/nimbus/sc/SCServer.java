package com.barco.nimbus.sc;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.RejectedExecutionException;

import org.apache.log4j.Logger;
import org.osgi.service.component.ComponentContext;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.osgi.service.component.annotations.Reference;

import com.barco.nimbus.api.MessageBusListenerI;
import com.barco.nimbus.api.SCServiceLocatorI;
import com.barco.nimbus.api.common.SCMessage;
import com.barco.nimbus.api.common.SCException;

@Component
public class SCServer implements MessageBusListenerI {

	ComponentContext context;
	SCServiceLocatorI serviceLocator;
	ExecutorService fixedPool;

	@Reference
	public void setServiceLocator(SCServiceLocatorI scServiceLocator) {
		this.serviceLocator = scServiceLocator;
		this.serviceLocator.getMessageBus().registerListener(this);
		Logger.getLogger(SCServer.class).info("SCServer:setServiceLocator");
	}
	
	public void unsetServiceLocator(SCServiceLocatorI scServiceLocator) {
		this.serviceLocator = null;
		Logger.getLogger(SCServer.class).info("SCServer:unsetServiceLocator");
	}

	@Activate
	public void start(ComponentContext context) throws Exception {
		Logger.getLogger(SCServer.class).info("SCServer:start");
		this.context = context;
		fixedPool = Executors.newFixedThreadPool(10,new EDPThreadFactory("SCServerPool"));
	    serviceLocator.getMRComponent().startService();
	}

	@Deactivate
	public void stop(ComponentContext context) throws Exception {
		Logger.getLogger(SCServer.class).info("SCServer:stop");
		fixedPool.shutdownNow(); //shutdown pool when stoping server.
	}
	
	@Override
	public void handleMessage(SCMessage message) throws Exception {
		// Logger.getLogger(SCServer.class).info("SCServer:handleMessage message=" + message.getRootName() + " from=" + message.getFrom());
		SCMessageRunnable runnable = new SCMessageRunnable(message, context.getBundleContext(), serviceLocator);
		Thread t = new Thread(runnable);
		try {
			fixedPool.execute(t);
		} catch (RejectedExecutionException e) {
			Logger.getLogger(SCServer.class).error("This Request is rejectect : the reason may be pool is shut ");
		}
	}

	@Override
	public void handleException(SCException exception) {
		Logger.getLogger(SCServer.class).info("SCServer:handleException exception=" + exception.toString());
	}

}
