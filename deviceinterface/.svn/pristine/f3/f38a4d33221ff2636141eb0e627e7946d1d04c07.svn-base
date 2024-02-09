package com.barco.device.common;

import org.apache.log4j.Logger;
import org.dom4j.Document;

import com.barco.device.base.LoginInfo;
import com.barco.device.base.ResponseHandlerI;




public interface DeviceFrameworkI extends  MessageBusListenerI{
	@Override
	public abstract void handleException(ServiceException exception);
	
	/**
	 * This callback is invoked when the bus is delivering a message to be handled by the device.
	 */
	@Override
	public abstract void handleMessage(Message message) throws Exception;

	/**
	 * Logs the message trace
	 */
	public abstract void messageTrace(String logMessage, Message message, Throwable ex);

	/**
	 * This function send a message via a bus.
	 */
	public abstract void sendMessage(Message m) throws Exception;

	/**
	 * Returns a log4j Logger instance
	 */
	public abstract Logger getLogger();
	
	/**
	 * Returns the service version of the Device
	 * @return
	 */
	public String getServiceVersion();
	
	public DeviceDBI getDeviceDB();
	
	public LoginInfo getLoginInfo() ;
	
	public String getAppserverJID();

	public String getMyJID();
	
	/**
	 * If message.type == Message.REQUEST, fills the message clientData and callbackData,
	 * then sends the message over the bus.
	 */
	public abstract void sendRequest(Message m, Document context, ResponseHandlerI responseHandler) throws Exception;

	
	
}
