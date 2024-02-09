package com.barco.device.agent;

import org.dom4j.Document;

import com.barco.device.common.DeviceFrameworkI;


/**
 * The Interface AgentI.
 */
public interface AgentI  {

	/**
	 * Inits the agent.
	 * 
	 * @param deviceFramework
	 *            the device framework
	 */
	public void initAgent(DeviceFrameworkI deviceFramework);
	/**
	 * Returns the information needs to publish to SC
	 * @return
	 * @throws Exception TODO
	 */
	public Document getPublishInfo() throws Exception;
	
	/**
	 * Gets the status.
	 * 
	 * @param xpath
	 *            the xpath
	 * @return Device Status Document.
	 * 			   For reference xml, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/objectXML/">XML</a>.
	 *             From this folder pick device specific xml. Sample XP device V2D-XP200-Device.xml. In this file check DeviceStatus element.
	 *             For reference xsd, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/xsd/">XSD</a>.
	 *             From this pick device specific xsd file.Sample XP device mediaroom.assetadminservice.datamodel.Device.V2D.xsd
	 * @throws Exception
	 *             the exception
	 */
	public Document getStatus(String xpath) throws Exception;
	
	public void reset();
}
