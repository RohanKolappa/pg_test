package com.barco.nimbus.module.api;

import java.util.HashMap;
import java.util.List;

/**
 * @version 1.0
 * @author NVAT
 * 
 * This interface gives the list of device management API
 * to be implemented by DeviceManagement module.
 * 
 * @param <D> device object ( combination of deviceConfig + deviceStatus + list of ports)
 * @param <DC> device config object (n/w config,  storage config , streaming config etc)
 * @param <DPD> device private data  
 * @param <DS> device status reference (n/w status, storage status ,streaming status etc)
 * @param <P> port object reference (source port/destination port and reference id's)
 */
public interface DeviceManagementI<D,DC, DPD, DS, P> {


	/**
	 * login device and mark the device online
	 * @param deviceTitle 
	 * @param deviceConfig  
	 * @param deviceStatus 
	 * @param listOfPorts
	 * @return device object
	 */
	public D login(String deviceTitle, DC deviceConfig, DS deviceStatus, HashMap<String,P> listOfPorts);

	/**
	 * Self register device. Add the list of ports given to the system
	 * @param deviceJID
	 * @param deviceConfig
	 * @param deviceStatus
	 * @param listOfPorts
	 * @return device object
	 */

	public D registerDevice(String deviceJID, DC deviceConfig, DS deviceStatus, HashMap<String,P> listOfPorts);

	/**
	 * Add device in the database for given device type
	 * @param deviceType (NGSD200/DMS1500 etc)
	 * @param deviceTitle
	 * @param devicePassword
	 * @param descripton
	 * @param devicePrivateData
	 * @param groupsInfo (Policy groups information)
	 * @return Device object
	 */

	public D addDevice(String deviceType, String deviceTitle, String devicePassword, String descripton,DPD devicePrivateData, HashMap<String,String> groupsInfo);	

	/**
	 *  update device description,  private data and policy groups
	 * @param byTitle
	 * @param byId
	 * @param groupsInfo
	 * @param descripton
	 * @param devicePrivateData
	 * @return Device object
	 */

	public D setDeviceInfo(RequestQueryParams queryParam, HashMap<String,String> groupsInfo, String descripton,DPD devicePrivateData);


	/**
	 *  Delete device from database. 
	 * @param queryParam
	 * @return
	 */
	public D deleteDevice(RequestQueryParams queryParam);

	/**
	 * return the requested device object
	 * @param queryParam
	 * @return device object 
	 */

	public D getDevice(RequestQueryParams queryParam);


	/**
	 * Return list of devices matching to given query
	 * @param queryParam
	 * @return list of Device Object
	 */

	public List<D> getDevices(HashMap<String,String> queryParam,String topic);

	/**
	 * return device configuration 
	 * @param queryParam
	 * @return
	 */

	public DC getDeviceConfig(RequestQueryParams queryParam);

	/**
	 * publish device config update request to device.
	 * @param queryParam
	 * @param configInfo
	 */

	public void publishDeviceConfig(RequestQueryParams queryParam, DC configInfo);

	/**
	 * Update given device configuration in database and broadcast update to subscribers.
	 * @param byId
	 * @param configInfo
	 */

	public void updateDeviceConfig(RequestQueryParams queryParam, DC configInfo);

	/**
	 * update device status in the database and broadcast update to subscribers.
	 * @param byId
	 * @param statusInfo
	 */
	public void updateDeviceStatus(RequestQueryParams queryParam, DS statusInfo);

	/**
	 * send upgrade request to device
	 * @param queryParam
	 */

	public void upgrade(RequestQueryParams queryParam);

	/**
	 * send shutdown request to device
	 * @param queryParam
	 */

	public void shutdown(RequestQueryParams queryParam);

	/**
	 *  send reboot request to device
	 * @param queryParam
	 */

	public void reboot(RequestQueryParams queryParam);

	/**
	 * send reset configuration request to database
	 * @param queryParam
	 */

	public void resetConfig(RequestQueryParams queryParam);

}
