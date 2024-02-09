package com.barco.device.vng.DeviceAdmin;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.device.vng.DeviceAdmin.HAService.ReplicationStatus;
import com.barco.utils.XMLUtils;

public class MSDeviceAdminAgentImpl extends BaseDeviceAdminAgentImpl{
    private enum UPDATE_CHILD_XPATH {
	IPVSDbService("//XMPPServiceConfig/IPVSDBService"),   	
	XMPPServer("//XMPPServiceConfig/XMPPServer"),
	MaxMediaClients("//ServicePreferences/Login"),
	HAServer("//ServiceElementsConfig/HAServiceConfig/HAServer");

	private String value;

	private UPDATE_CHILD_XPATH(String value) {
	    this.value = value;
	}

	public String toString() {
	    return this.value;
	}

    };
    private String HA_CONFIG_CLI_COMMAND = "//ServiceElementsConfig/HAServiceConfig/HAServer";    
    private enum HA_USER_CONFIGURABLE_VALUES {HAServerEnable,PeerIP,Primary,PeerName};
    private String CONFIG_HANDLER_XML = "/com/ipvs/xsd/xml/confighandler.xml";

	public MSDeviceAdminAgentImpl() {
		super();
		HAUtils.updateHAProperties();
	}

    @Override
    protected List<Element> getPortResources(Document configDocument,
	    Document statusDocument) throws Exception {
	//No ports for MS device
	return null;
    }


    @Override
    public Document getPublishInfo() throws Exception {
	Document publishDoc = super.getPublishInfo();
	String assetReadyState = "";
	ReplicationStatus repStatus = HAUtils.getReplicationStatus();

	// Check HA enabled && replication is enabled
	if (HAUtils.isServerInStandby()) {
	    if (repStatus.equals(ReplicationStatus.NotStarted)) {
	    	// Invalidate the thumbnail cache on standby server. 
	    	// This will be re-built when the standby takes over.
	    	HAUtils.invalidateFileThumbnailCache(this.deviceFrameWork.getLogger());
		
	    	// Send DownloadDB request to active HA server
	    	HAUtils.sendDownloadDBRequest(this.deviceFrameWork,	new DownloadDBResponseHandler());
	    	// Mark asset as not ready
	    	assetReadyState = "false";
	    } else if (repStatus.equals(ReplicationStatus.Done)) {
	    	// Mark asset as ready
	    	assetReadyState = "true";
	    }
	}

	// Set asset state for Device Login
	publishDoc.getRootElement().addAttribute(DeviceAdminConst.READY_ATTRIBUTE_NAME, assetReadyState);

	return publishDoc;
    }


    /**
     * Compares the configurable parameter specified in confighandler.xml for given cli command
     * @param standByServerConfigEl
     * @param activeServerConfigEl
     * @param command
     * @return list of mismatched element names
     * @throws MRException
     * @throws Exception
     */
    public List<String> compareAndUpdateConfigElements(DeviceI device, Element standByServerConfigEl, Element activeServerConfigEl, List<UPDATE_CHILD_XPATH> xpathList, boolean isHAServerConfig) throws Exception {
	// TODO Auto-generated method stub
	List<String> missMatchElementNameList = new ArrayList<String>();
	Document configHandlerDoc = XMLUtils.loadXMLResource(CONFIG_HANDLER_XML);
	List<String> modifiedXpathList = new ArrayList<String> ();	
	for (UPDATE_CHILD_XPATH triggerName : xpathList) {
	    if(!isHAServerConfig && HA_CONFIG_CLI_COMMAND.equals(triggerName.toString()))
		break;
	    Element configHandlerEl = XMLUtils.getElementXpath(configHandlerDoc, "//handler[triggername='" + triggerName.toString()+ "']");
	    if(configHandlerEl != null) {
		String command = XMLUtils.getValueXpath(configHandlerEl, "cmd", "");
		device.getLogger().debug("MSDeviceAdminAgentImpl.handleConfigUpdate: Command: "+command);
		Element dbConfigCommandEL = XMLUtils.getElementXpath(configHandlerDoc, "//handler[cmd='"+command+"']");
		List<?> paramEntry =  XMLUtils.selectNodesXpath(dbConfigCommandEL, ".//paramentry");

		if(paramEntry == null || paramEntry.isEmpty()) {
		    return missMatchElementNameList;
		}

		for(Object paramEntryObject : paramEntry) {
		    String paramVal = XMLUtils.getValueXpath((Element)paramEntryObject, "./paramval", "");	    		
		    String xpath = ".//"+paramVal;    	
		    String activeServerDbConfigValue = XMLUtils.getValueXpath(activeServerConfigEl, xpath, "NA");
		    if(!activeServerDbConfigValue.equals("NA")){
			String standByServerDbConfigValue = XMLUtils.getValueXpath(standByServerConfigEl, xpath, "");			    		    
			if(!activeServerDbConfigValue.equals(standByServerDbConfigValue)){
			    missMatchElementNameList.add(paramVal);
			    if(!isHAServerConfig){
				XMLUtils.setValueXpathElement(standByServerConfigEl, xpath, activeServerDbConfigValue);
				Element modifiedElement = XMLUtils.getElementXpathFromElement(standByServerConfigEl,"."+triggerName.toString());

				modifiedElement.addAttribute("Dirty", "MODIFY");
				modifiedXpathList.add(triggerName.toString());
			    }
			}
		    }
		}			    	
	    }	    
	}
	if(!modifiedXpathList.isEmpty() && !isHAServerConfig){
	    Element serviceElementConfig= DocumentHelper.createElement("ServiceElementsConfig");
	    for(UPDATE_CHILD_XPATH allXpath: UPDATE_CHILD_XPATH.values()){
		if(!modifiedXpathList.contains(allXpath.toString())){
		    Element detacheElement = XMLUtils.getElementXpathFromElement(standByServerConfigEl, "."+allXpath.toString());
		    if(detacheElement != null)
			detacheElement.detach();
		}		   
	    }
	    device.getLogger().debug("ActiveDeviceConfigUpdateRequestHandler: Active and Stand By server ServiceElement Configuration Not matched replacing stand by server configuration with active server configuration");
	    serviceElementConfig.add((Element)standByServerConfigEl.detach());	    
	    device.getDeviceAdminAgentImpl().setDeviceConfig(XMLUtils.elementToDocument(serviceElementConfig));
	    device.getDeviceAdminAgentImpl().saveConfiguration();
	}
	return missMatchElementNameList;
    }
    @Override
    public void handleConfigUpdate(DeviceI device, Element activeServerConfigEl) throws Exception {
	String xpathChanged = XMLUtils.getAttributeValue(activeServerConfigEl, "xpath");
	Element standByServerConfigEl = getDeviceConfigElement(device);
	Element activerServerHAConfigEL = XMLUtils.getElementXpathFromElement(activeServerConfigEl, ".//HAServer");
	if(activerServerHAConfigEL != null) {	    
	    compareAndUpdateHAServiceConfiguration(device, activeServerConfigEl, standByServerConfigEl);		    
	    activerServerHAConfigEL.detach();
	}
	Element standByServerHAConfigEl = XMLUtils.getElementXpathFromElement(standByServerConfigEl, ".//HAServiceConfig");
	if(standByServerHAConfigEl != null) {
	    standByServerHAConfigEl.detach();
	}
	if(activeServerConfigEl != null) {
	    List<UPDATE_CHILD_XPATH> listOfChangedXpath = null;
	    if(!xpathChanged.isEmpty()){
		listOfChangedXpath = Arrays.asList(getXpathEnumFromString(xpathChanged));
	    }
	    else
	    {
		listOfChangedXpath = Arrays.asList(UPDATE_CHILD_XPATH.values());		

	    }
	    compareAndUpdateConfigElements(device, standByServerConfigEl, activeServerConfigEl, listOfChangedXpath, false);
	}
    }
    private boolean compareAndUpdateHAServiceConfiguration(DeviceI device,Element activeDeviceEl,
	    Element standByDeviceEl) throws Exception {	
	Element activerServerHAConfigEL = XMLUtils.getElementXpathFromElement(activeDeviceEl, ".//HAServer");
	if(activerServerHAConfigEL == null) {
	    return false;
	}

	Element standByServerHAconfigEl = XMLUtils.getElementXpathFromElement(standByDeviceEl, ".//HAServer");
	List<String> misMatchConfigList = compareAndUpdateConfigElements(device, standByServerHAconfigEl, activerServerHAConfigEL,Arrays.asList(getXpathEnumFromString(HA_CONFIG_CLI_COMMAND)), true);
	boolean updateHAConfig = false;

	if(!misMatchConfigList.isEmpty()) {
	    for(String misMatchParam : misMatchConfigList) {			
		boolean ignoreMatch = false;
		for(HA_USER_CONFIGURABLE_VALUES userConfigurableParam: HA_USER_CONFIGURABLE_VALUES.values()) {
		    if(misMatchParam.equals(userConfigurableParam.name())){
			ignoreMatch = true;
			break;
		    }
		}

		if(!ignoreMatch) {	    			    
		    XMLUtils.setValueXpathElement(standByServerHAconfigEl, ".//"+misMatchParam, XMLUtils.getValueXpath(activerServerHAConfigEL, ".//"+misMatchParam, ""));
		    updateHAConfig = true;
		}

	    }

	}

	String activeServerPeerIP = XMLUtils.getValueXpath(standByServerHAconfigEl, ".//PeerIP", "");
	String activeServerPeerName = XMLUtils.getValueXpath(standByServerHAconfigEl, ".//PeerName", "");
	Element parameterEl = XMLUtils.getElementXpathFromElement(activeDeviceEl, "./EthernetPortStatus/Parameters[Parameter[@name='IPAddress' or .='"+activeServerPeerIP+"']]");
	if(parameterEl != null){
	    String activeServerName = XMLUtils.getValueXpath(parameterEl, "./Parameter[@name='Name']", "");	    	
	    if(!activeServerPeerName.isEmpty() && !activeServerPeerName.equals(activeServerName)){
		XMLUtils.setValueXpathElement(standByServerHAconfigEl, ".//PeerName", activeServerName);
		updateHAConfig = true;
	    }
	}


	if(updateHAConfig) {
	    device.getLogger().debug("ActiveDeviceConfigUpdateRequestHandler: Active and Stand By server HA Service Configuration Not matched replacing stand by server configuration with active server configuration");	    		
	    Element serviceElementConfig= DocumentHelper.createElement("ServiceElementsConfig");
	    serviceElementConfig.add((Element)standByServerHAconfigEl.detach());	   
	    device.getDeviceAdminAgentImpl().setDeviceConfig(XMLUtils.elementToDocument(serviceElementConfig));
	    device.getDeviceAdminAgentImpl().saveConfiguration();
	}

	return updateHAConfig;
    }
    public static UPDATE_CHILD_XPATH getXpathEnumFromString(String text) {
	if (text != null) {
	    for (UPDATE_CHILD_XPATH xpath : UPDATE_CHILD_XPATH.values()) {
		if (text.equalsIgnoreCase(xpath.toString())) {
		    return xpath;
		}
	    }
	}
	return null;
    }

    private Element getDeviceConfigElement(DeviceI device) throws Exception{
	Element standByDeviceEl = device.getDeviceAdminAgentImpl().getDeviceConfig("").getRootElement();
	Element standByDeviceConfigEl = DocumentHelper.createElement("DeviceConfig");
	standByDeviceConfigEl.add((Element)XMLUtils.getElementXpathFromElement(standByDeviceEl, "//HAServiceConfig").detach());
	standByDeviceConfigEl.add((Element)XMLUtils.getElementXpathFromElement(standByDeviceEl, "//XMPPServiceConfig").detach());

	return standByDeviceConfigEl;
    }

    @Override
    public void haTakeover() throws Exception {
	deviceEngine.haTakeover();
    }

}
