package com.barco.device.vng.DeviceAdmin;

import org.dom4j.Document;
import org.dom4j.Element;

public interface DeviceAdminEngineI {	
	/**
	 * Add observer object to list. 
	 * @param deviceServerObserverObject
	 */
	
	public void addObserver(DeviceAdminEngineObserverI deviceServerObserverObject);
	/**
	 * Apply the given configuration  
	 * @param deviceConfigDoc
	 * @throws Exception
	 */
	public void setDeviceConfig(Document deviceConfigDoc)  throws Exception;
	
	/**
	 * Fix the document to contain up-to-date information (obtained from the
	 * running system).
	 */
	public void getDeviceStatus(Element tempDeviceStatusElement) throws Exception;
	
	/**
	 * Resets the server on logout
	 */
	public void resetServer();
	
	
	/**
	 * Reboot the device
	 * @throws Exception TODO
	 */
	public void reboot() throws Exception;
	
	/**
	 * upgrade the device with new software
	 * @param commandsParams
	 * @throws Exception TODO
	 */
	public void upgrade(String url) throws Exception;
	
	
	/**
	 * Revert the configuration to default
	 * @param revertNetworkConfig
	 * @param deleteMedia
	 * @throws Exception TODO
	 */
	public void revertToDefault(boolean revertNetworkConfig, boolean deleteMedia) throws Exception;
	
	
	/**
	 * save the current configuration 
	 * @throws Exception TODO
	 */
	public void saveConfiguration() throws Exception;
	
	/**
	 * Shutdown the device 
	 * @throws Exception TODO
	 */
	public void shutdown() throws Exception;
	
	/**
	 * HA takeover for device 
	 * @throws Exception TODO
	 */
	public void haTakeover() throws Exception;
	
	//public Element getStatus() throws Exception;
	
}
