package com.barco.nimbus.api;

import com.barco.nimbus.api.common.SCMessage;
import com.barco.nimbus.api.common.SCException;

/**
 * MessageListenerI implementors register with the agents to receive the messages
 * which the agents collect from the device servers
 */
public interface MessageBusListenerI {
	/**
	 * This callback is invoked when the bus is delivering a message to be handled by the device.
	 */
	void handleMessage(SCMessage message) throws Exception;
		
	/**
	 * This callback is invoked when the bus received a message that is not wellformed,
	 * or when the bus get disconnected.
	 */
	void handleException(SCException exception);

}
