package com.barco.nimbus.module.api;

import java.util.HashMap;
import java.util.List;



/**
 * @version 1.0
 * @author NVAT
 * 
 * This interface has to implemented by media management. this has to use along with device management api.
 *
 * @param <P> Port object (source/destination)
 * @param <PRF> Profile object (v2d/rtp/mpeg profile)
 * @param <D> Device object
 */

public interface MediaManagementI<P, PRF, D> {

	/**
	 * register port in the database and attach port to given device.
	 * only used in device porxy mode
	 * @param portId
	 * @param portObject
	 * @param device
	 * @return
	 */

	public P registerPort(String portId, P portObject,D device);

	/**
	 * used only in device proxy mode (Nexxis Dongles)
	 * Delete port from database and detach port from device
	 * @param portId
	 * @return
	 */
	public P deletePort(String portId,D device);
	

	/**
	 * return list of ports 
	 * @param queryParam
	 * @param topic
	 * @param portType
	 * @return
	 */

	public List<P> getPorts(HashMap<String,String> queryParam, String topic,String portType);

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
	 * @param policyGroups
	 * @param description
	 * @return
	 */
	public P setPortInfo(RequestQueryParams queryParam,String newTitle, HashMap<String,String> policyGroups,String description);


	/**
	 * enable/disable persistent pvr on source port
	 * @param byTitle
	 * @param byId
	 * @param enablePVR
	 * @param trimlength
	 * @param direcotryId
	 * @return
	 */
	public P setPortPVRSettings(RequestQueryParams queryParam,boolean enablePVR,long trimlength, String direcotryId);

	
	/**
	 * update source to destination or destination to source.  This is only applicable to NGSD200, older XP,I50
	 * @param queryParam
	 * @param newType
	 * @return
	 */
	public P setPortType(RequestQueryParams queryParam,String newType);

	/**
	 * update source broadcast settings and send message to source to start broadcasting.
	 * @param queryParam
	 * @param enableBroadcast
	 * @param profile
	 * @return
	 */
	public P setPortBroadcastSettings(RequestQueryParams queryParam,boolean enableBroadcast, PRF profile);

	/**
	 * update source monitor and xauth information 
	 * @param queryParam
	 * @param monitor
	 * @param xauth
	 * @param proxyUser
	 * @param proxyResource
	 * @return
	 */
	public P setPortObserverInfo(RequestQueryParams queryParam, boolean monitor, boolean xauth,String proxyUser, String proxyResource);

	/**
	 * update source or destination stream url information in database
	 * @param queryParam
	 * @param streamURL
	 * @return
	 */
	public P setPortStreamURL(RequestQueryParams queryParam,String streamURL);



}
