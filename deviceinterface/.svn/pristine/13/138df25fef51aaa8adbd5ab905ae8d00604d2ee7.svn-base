
package com.barco.device.agent;

import org.dom4j.Document;

/**
 * The Interface LayoutAgentI.
 */
public interface LayoutAgentI extends AgentI{

	/**
	 * Update display config.
	 * 
	 * @param displayConfig document.
	 *            For reference xml, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/objectXML/">XML</a>.
	 *            From this folder pick device specific xml. Sample XP device V2D-XP200-Device.xml. In this file check XP200DisplayConfig element.
	 *            For reference xsd, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/xsd/">XSD</a>.
	 *            From this pick device specific xsd file.Sample XP device mediaroom.assetadminservice.datamodel.Device.V2D.xsd
	 */
	public void updateDisplayConfig(Document displayConfig);
	
		
	/**
	 * 
	 * @return displayconfig document.
	 * 			  For reference xml, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/objectXML/">XML</a>.
	 *            From this folder pick device specific xml. Sample XP device V2D-XP200-Device.xml. In this file check XP200DisplayConfig element.
	 *            For reference xsd, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/xsd/">XSD</a>.
	 *            From this pick device specific xsd file.Sample XP device mediaroom.assetadminservice.datamodel.Device.V2D.xsd
	 */
	public Document getDisplayConfig();
	
	/**
	 * Start playing.
	 * 
	 * @param streamNID
	 *            the stream nid
	 * @param destPortElementID
	 *            the dest port element id
	 * @param URL
	 *            the url
	 */
	public void startPlaying(String streamNID, String destPortElementID, String URL);
	
	/**
	 * Stop playing.
	 * 
	 * @param streamNID
	 *            the stream nid
	 */
	public void stopPlaying(String streamNID);
	
	
	/**
	 * handle device config updates 
	 * @param doc
	 * @throws Exception
	 */
	public void handleConfigUpdated(Document doc) throws Exception;
	
}
