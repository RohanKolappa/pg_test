package com.barco.nimbus.module.devicemanagement.api;

import java.util.List;
import java.util.Map;

/**
 * @version 1.0
 * @author NVAT
 * 
 * This interface gives the list of device management API can be used by integrator to manage devices.
 * 
 *  
 * @param <D> device object ( combination of deviceConfig + deviceStatus + list of ports)
 * @param <DC> device config object (n/w config,  storage config , streaming config etc)
 * @param <DPD> device private data  
 * @param <DS> device status reference (n/w status, storage status ,streaming status etc)
 * @param <P> port object reference (source port/destination port and reference id's)
 * @param <PRF>
 */
public interface DeviceManagementI<D,DC, DPD, DS, P, PRF> {


	
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

	public D addDevice(String deviceType, String deviceTitle, String devicePassword, String descripton,DPD devicePrivateData, Map<String,String> groupsInfo);	

	/**
	 *  update device description,  private data and policy groups
	 * @param byTitle
	 * @param byId
	 * @param groupsInfo
	 * @param descripton
	 * @param devicePrivateData
	 * @return Device object
	 */

	public D setDeviceInfo(RequestQueryParams queryParam, Map<String,String> groupsInfo, String descripton,DPD devicePrivateData);


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

	public List<D> getDevices(Map<String,String> queryParam,String topic);

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
	 * send upgrade request to device
	 * @param queryParam
	 */

	public void upgradeDevice(RequestQueryParams queryParam);

	/**
	 * send shutdown request to device
	 * @param queryParam
	 */

	public void shutdownDevice(RequestQueryParams queryParam);

	/**
	 *  send reboot request to device
	 * @param queryParam
	 */

	public void rebootDevice(RequestQueryParams queryParam);

	/**
	 * send reset configuration request to device
	 * @param queryParam
	 */

	public void resetDeviceConfig(RequestQueryParams queryParam);
	
	
	/**
	 * PORT API
	 */
	
	
	
	public List<P> getPorts(Map<String,String> queryParam, String topic,String portType);

	/**
	 * return port object
	 * @param queryParam input query
	 * @return
	 */
	public P getPort(RequestQueryParams queryParam);	

	/**
	 * update port description and policy groups in database
	 * @param queryParam
	 * @param newTitle
	 * @param policyGroups example Location=[OR1,OR2]
	 * @param description
	 * @return
	 */
	public P setPortInfo(RequestQueryParams queryParam,String newTitle, Map<String,List<String>> policyGroups,String description);


	/**
	 * enable/disable persistent pvr on source port
	 * @param byTitle
	 * @param byId
	 * @param enablePVR
	 * @param trimlength
	 * @param direcotryId
	 * @return
	 */
	public P setPortPVRSettings(RequestQueryParams queryParam,PVRSettings pvrSettings);	


	/**
	 * update source broadcast settings and send message to source to start broadcasting.
	 * @param queryParam
	 * @param enableBroadcast
	 * @param profile
	 * @return
	 */
	public P setPortBroadcastSettings(RequestQueryParams queryParam ,BroadcastSettings<PRF> broadCastSettings);

	/**
	 * update source monitor and xauth information 
	 * @param queryParam
	 * @param monitor
	 * @param xauth
	 * @param proxyUser
	 * @param proxyResource
	 * @return
	 */
	public P setPortObserverInfo(RequestQueryParams queryParam,ObserverSettings observerSettings);

	/**
	 * update source or destination stream url information in database
	 * @param queryParam
	 * @param streamURL
	 * @return
	 */
	public P setPortStreamURL(RequestQueryParams queryParam,String streamURL);
	
	
	public class ObserverSettings {
		 public boolean monitor;
		 public boolean xauth;
		 public String proxyUser;
		 public String proxyResource;
		public ObserverSettings(boolean monitor, boolean xauth, String proxyUser,String proxyResource){
			this.monitor = monitor;
			this.xauth = xauth;
			this.proxyUser = proxyUser;
			this.proxyResource = proxyResource;
		}
	}
	
	
	public class PVRSettings {
		public boolean enablePVR;
		public long trimlength;
		public String direcotryId;
		public PVRSettings(boolean enablePVR,long trimlength,String directoryId) {
			this.enablePVR = enablePVR;
			this.trimlength = trimlength;
			this.direcotryId = directoryId;
		}
	}

	
	public class BroadcastSettings<PRF> {
		public boolean enableBroadcast;
		public PRF profile;
		public BroadcastSettings(boolean enableBroadcast,PRF profile) {
			this.enableBroadcast = enableBroadcast;
			this.profile = profile;
		}
	}

}
