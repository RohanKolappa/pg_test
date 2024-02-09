package com.barco.nimbus.api;

import com.barco.nimbus.api.common.SCMessage;

public interface MessageBusI {

	public static final int CONNECTED = 0;
	public static final int DISCONNECTED = 1;

	/**
	 * Registers a listener for incoming messages on the messageBus
	 * 
	 * @param listener
	 */
	public void registerListener(MessageBusListenerI listener);

	/**
	 * Indicates whether it is connected or not
	 * 
	 * @return the state of the MessageBus
	 */
	public int getState();

	/**
	 * Creates a response Message based on the request and the result
	 * 
	 * @param to
	 * @param m
	 *            request message
	 * @return the response message
	 * @throws Exception
	 */
	public SCMessage createResponseUsingRequestMessage(String to, SCMessage m)
			throws Exception;

	/**
	 * Sends a message on the MessageBus to the user indicated in the to field
	 * of the message
	 * 
	 * @param message
	 *            to be sent
	 * @throws Exception
	 */
	public void sendMessage(SCMessage message) throws Exception;

	/**
	 * Subscribe to the presence of a user so that we receive notifications when
	 * the user logs in/logs out
	 * 
	 * @param userJID
	 * @throws Exception
	 */
	public void subscribeUserPresence(String userJID) throws Exception;

	/**
	 * Unsubscribe to the presence of the user
	 * 
	 * @param userJID
	 * @throws Exception
	 */
	public void unSubscribeUserPresence(String userJID) throws Exception;

	/**
	 * Self JID on the message bus
	 * 
	 * @return
	 */
	public String getJID();
	
	
	/**
	 * Add Disco feature 
	 * http://xmpp.org/extensions/xep-0030.html
	 * 
	 * @return
	 */
	public void addDiscoFeature(String feature);
	
}
