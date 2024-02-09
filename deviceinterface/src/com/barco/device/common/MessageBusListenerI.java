package com.barco.device.common;

import org.apache.log4j.Level;

/**
 * MessageListenerI implementors register with the agents to receive the messages
 * which the agents collect from the device servers
 */
public interface MessageBusListenerI {
	/**
	 * This callback is invoked when the bus is delivering a message to be handled by the device.
	 */
	void handleMessage(Message message) throws Exception;
	
	/**
	 * This callback is invoked when the bus received a message that is not wellformed,
	 * or when the bus get disconnected.
	 */
	void handleException(ServiceException exception);

	/*
	 * @param logMessage : log string
	 * @param level : log4j Level 
	 * @param ex : Throwable object, allows null.
	 */
	void handleLog(String logMessage, Level level, Throwable ex);
}
