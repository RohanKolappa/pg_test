package com.barco.device.common;

/*
 * MessageListenerI implementors register with the agents to receive the messages
 * which the agents collect from the device servers
 */
public interface MessageBusListenerI {
	/*
	 * @param source indicates the source of the message
	 * @param message the message received
	 */
	void handleMessage(Message message) throws Exception;
	
	/*
	 * @param source indicates the source of the message
	 * @param message the message received
	 */
	void handleException(ServiceException exception);

	/*
	 * @param s : log string
	 */
	void handleLog(String s, int level);
}
