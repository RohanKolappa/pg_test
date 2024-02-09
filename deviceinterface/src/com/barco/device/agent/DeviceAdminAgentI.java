/*
 * 
 */
package com.barco.device.agent;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;

/**
 * The Interface DeviceAdminAgentI.
 */
public interface DeviceAdminAgentI extends AgentI {
	
	/**
	 * Returns the device configuration document based on given xpath. If xpath is not provided return whole device config xml.
	 * 
	 * @param xpath
	 *            the xpath
	 * @return Device configuration document. 
	 *            For reference xml, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/objectXML/">XML</a>.
	 *            From this folder pick device specific xml. Sample XP device V2D-XP200-Device.xml. In this file check DeviceConfig element.
	 *            For reference xsd, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/xsd/">XSD</a>.
	 *            From this pick device specific xsd file.Sample XP device mediaroom.assetadminservice.datamodel.Device.V2D.xsd
	 * @throws Exception
	 *             the exception
	 */
	public Document getDeviceConfig(String xpath) throws Exception;
	
	/**
	 * Apply the given configuration  
	 * @param deviceConfigDoc
	 * 			  For reference xml, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/objectXML/">XML</a>
	 *            From this folder pick device specific xml. Sample XP device V2D-XP200-Device.xml. In this file check DeviceConfig element.
	 *            For reference xsd, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/xsd/">XSD</a>
	 *            From this pick device specific xsd file.Sample XP device mediaroom.assetadminservice.datamodel.Device.V2D.xsd
	 * @throws Exception
	 */
	public void setDeviceConfig(Document deviceConfigDoc)  throws Exception;

	
	/**
	 * Returns list of command supported by device
	 * @return Device Command Document
	 *  		  For reference xml, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/objectXML/">XML</a>
	 *            From this folder pick device specific xml. Sample for xp device V2D-XP200-Device.xml. In this file check DeviceCommand element.
	 *            For reference xsd, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/xsd/">XSD</a>
	 *            From this pick device specific xsd file.Sample XP device mediaroom.assetadminservice.datamodel.Device.V2D.xsd
	 * @throws Exception
	 */
	public Document getDeviceCommands() throws Exception;
				
	/**
	 * Reboot the device.
	 * 
	 * @throws Exception
	 *             TODO
	 */
	public void reboot() throws Exception;
	
	/**
	 * upgrade the device with new software.
	 * 
	 * @param URL
	 *            the url
	 * @throws Exception
	 *             TODO
	 */
	public void upgrade(String URL) throws Exception;
	
	/**
	 * Revert the configuration to default.
	 * 
	 * @param keepNetworkSettings
	 *            the keep network settings
	 * @param keepMedia
	 *            TODO
	 * @throws Exception
	 *             TODO
	 */
	public void revertToDefault(boolean keepNetworkSettings, boolean keepMedia) throws Exception;
	
	/**
	 * save the current configuration.
	 * 
	 * @throws Exception
	 *             TODO
	 */
	public void saveConfiguration() throws Exception;
	
	/**
	 * Shutdown the device.
	 * 
	 * @throws Exception
	 *             TODO
	 */
	public void shutdown() throws Exception;
	

	public void handleConfigUpdate(DeviceI device, Element activeServerConfigEl) throws Exception;

	/**
	 * HATakeover device.
	 * 
	 * @throws Exception
	 *             TODO
	 */
	public void haTakeover() throws Exception;
	

	
}
