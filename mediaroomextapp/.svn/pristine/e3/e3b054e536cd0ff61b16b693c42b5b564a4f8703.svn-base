package com.barco.device.handler.DeviceAdmin.impl;

import org.dom4j.Document;
import org.dom4j.Element;

public interface DeviceServerI {	
	/**
	 * Apply the given configuration  
	 * @param deviceConfigDoc
	 * @throws Exception
	 */
	public void setDeviceConfig(Document deviceConfigDoc)  throws Exception;
	
	/**
	 * Returns the Device Status 
	 * @param tempDeviceStatusDoc
	 * @throws Exception
	 */
	public void getDeviceStatus(Element tempDeviceStatusElement) throws Exception;
	
	/**
	 * Resets the server on logout
	 */
	public void resetServer();
	
}
