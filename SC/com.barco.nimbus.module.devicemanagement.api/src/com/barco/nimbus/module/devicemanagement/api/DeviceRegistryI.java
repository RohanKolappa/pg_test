package com.barco.nimbus.module.devicemanagement.api;

import java.util.Map;

/**
 * @version 1.0
 * @author NVAT
 * 
 * This interface gives the list of device management API.
 * Devices will use these API's to login to SC or publish any status/config updates to SC
 * 
 * @param <D> device object ( combination of deviceConfig + deviceStatus + list of ports)
 * @param <DC> device config object (n/w config,  storage config , streaming config etc)
 * @param <DPD> device private data  
 * @param <DS> device status reference (n/w status, storage status ,streaming status etc)
 * @param <P> port object reference (source port/destination port and reference id's)
 */
public interface DeviceRegistryI<D,DC, DPD, DS, P> {

	

	/**
	 * login device and mark the device online
	 * @param deviceTitle 
	 * @param deviceConfig  
	 * @param deviceStatus 
	 * @param listOfPorts
	 * @return device object
	 */
	
	public D loginDevice(String deviceTitle, DC deviceConfig, DS deviceStatus, Map<String,P> listOfPorts);
	
	
	
	/**
	 * login device and mark the device online
	 * @param deviceTitle 
	 * @param deviceConfig  
	 * @param deviceStatus 
	 * @param listOfPorts
	 * @return device object
	 */
	public D logoutDevice(String deviceTitle);
	
	

	/**
	 * Self register device. Add the list of ports given to the system
	 * @param deviceJID
	 * @param deviceConfig
	 * @param deviceStatus
	 * @param listOfPorts
	 * @return device object
	 */

	public D registerDevice(String deviceJID, DC deviceConfig, DS deviceStatus, Map<String,P> listOfPorts);	

	
	
	/**
	 * Update given device configuration in database and broadcast update to subscribers.
	 * @param byId
	 * @param configInfo
	 */

	public void setDeviceConfig(RequestQueryParams queryParam, DC configInfo);
	

	/**
	 * update device status in the database and broadcast update to subscribers.
	 * @param byId
	 * @param statusInfo
	 */
	public void setDeviceStatus(RequestQueryParams queryParam, DS statusInfo);
	
	
	/**
	 * PORT API
	 */
	
	
	/**
	 * register port in the database and attach port to given device.
	 * only used in device porxy mode
	 * @param portId
	 * @param portObject
	 * @param device
	 * @return
	 */

	public P addPort(String portId, P portObject,D device);

	/**
	 * used only in device proxy mode (Nexxis Dongles)
	 * Delete port from database and detach port from device
	 * @param portId
	 * @return
	 */
	public P deletePort(String portId,D device);
	

	
	/**
	 * update source to destination or destination to source.  This is only applicable to NGSD200, older XP,I50
	 * @param queryParam
	 * @param newType
	 * @return
	 */
	public P setPortType(RequestQueryParams queryParam,String newType);
	

}
