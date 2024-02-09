package com.barco.device.common;

import org.apache.log4j.Logger;

public interface MessageBusI {

	public static final int CONNECTED = 0;
	public static final int DISCONNECTED = 1;
	
	public abstract void init(MessageBusListenerI listener, Logger log);
	public abstract int getState();
	public abstract void sendMessage(Message message) throws Exception;
	public void login(String username,String password,String resource, String ipaddress,String appserverAgentJID) throws ServiceException,Exception;
	public void logout() throws ServiceException,Exception;
	public void sendSCDiscoveryMessage() throws ServiceException, Exception;
	
}
