package com.barco.device.common;

public interface MessageBusI {

	public static final int CONNECTED = 0;
	public static final int DISCONNECTED = 1;
	
	public abstract void setListener(MessageBusListenerI listener);
	public abstract int getState();
	public abstract void sendMessage(Message message) throws Exception;
}
