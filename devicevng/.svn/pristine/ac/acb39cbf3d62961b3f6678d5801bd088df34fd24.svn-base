package com.barco.device.vng.DeviceAdmin;

import java.util.List;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.DeviceAdminAgentI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.device.vng.DeviceAdmin.deviceengine.DeviceAdminEngineImpl;
import com.barco.utils.XMLUtils;

/**
 * The agent delegates commands (reboot, ...) to an engine
 * {@link DeviceAdminEngineImpl}.
 * <p>
 * It also registers an engine observer, and handles engine-notifications by
 * delegating to DeviceAdmin.*EventHandler (though DeviceImpl.handleMessage).
 * <p>
 * It collects config and status info, to produce the getPublishInfo().
 */
public abstract class BaseDeviceAdminAgentImpl implements DeviceAdminAgentI,DeviceAdminEngineObserverI{	

	DeviceFrameworkI deviceFrameWork;
	DeviceAdminEngineI deviceEngine;

	public BaseDeviceAdminAgentImpl(){		
		
	}

	/**
	 * Instantiates the engine, and registers an engine observer that delegates
	 * to DeviceAdmin.[notificationType]Handler: eg
	 * DeviceStatusUpdateEventHandler, DeviceConfigUpdateEventHandler,
	 * *EventHandler
	 */
	@Override
    public void initAgent(DeviceFrameworkI deviceFrameWork) {
		this.deviceFrameWork = deviceFrameWork;
		if(deviceEngine!=null) {
			return;
		}
		deviceEngine = new DeviceAdminEngineImpl(deviceFrameWork.getLogger(), this.deviceFrameWork.getDeviceDB());
		// deviceEngine = new DummyDeviceEngineImpl(deviceFrameWork.getLogger(), this.deviceFrameWork.getDeviceDB());
		deviceEngine.addObserver(this);
	}

	/**
	 * Updates the deviceDB.DeviceStatus (node //DeviceStatus), by obtaining up
	 * to date information from the running system.
	 * 
	 * @param xpath
	 *            ignored
	 * @return the DeviceStatus
	 */
	@Override
	public Document getStatus(String xpath) throws Exception {

		Element deviceStatusElement = null; 
		
		this.deviceFrameWork.getLogger().debug("BaseDeviceAdminAgentImpl:getStatus getDeviceStatus begin ...");
		Document deviceDoc = deviceFrameWork.getDeviceDB().getDeviceStatus();
		this.deviceFrameWork.getLogger().debug("BaseDeviceAdminAgentImpl:getStatus getDeviceStatus end");
		
		deviceStatusElement = XMLUtils.getElementXpath(deviceDoc,"//DeviceStatus");

		//update device status
		this.deviceFrameWork.getLogger().debug("BaseDeviceAdminAgentImpl:getStatus getDeviceEngineStatus begin ...");
		deviceEngine.getDeviceStatus(deviceStatusElement);
		this.deviceFrameWork.getLogger().debug("BaseDeviceAdminAgentImpl:getStatus getDeviceEngineStatus end");

		//update device status on disk
		deviceFrameWork.getDeviceDB().setDeviceStatus(deviceDoc);
		
		return deviceDoc;
		
	}
    
	

	@Override
	public void setDeviceConfig(Document deviceConfigDoc) throws Exception {	
		deviceEngine.setDeviceConfig(deviceConfigDoc);		
	}

	@Override
	public void reboot() throws Exception {
		deviceEngine.reboot();
	}

	@Override
	public void upgrade(String url) throws Exception {
		deviceEngine.upgrade(url);
	}

	@Override
	public void revertToDefault(boolean keepNetworkSettings, boolean keepMedia) throws Exception {
		deviceEngine.revertToDefault(keepNetworkSettings, keepMedia);
	}

	@Override
	public void saveConfiguration() throws Exception {
		deviceEngine.saveConfiguration();
	}
	
	@Override
	public void shutdown() throws Exception {
		deviceEngine.shutdown();
	}

	/**
	 * @return clone of (a subsection of) the deviceDB.getDeviceConfig()
	 */
	@Override
	public Document getDeviceConfig(String xpath) throws Exception {
		Document deviceDoc = this.deviceFrameWork.getDeviceDB().getDeviceConfig();
		Element configEl = null;
		if(xpath !=null && !xpath.isEmpty()){
			configEl = XMLUtils.getElementXpath(deviceDoc,xpath);
		} else {
			configEl = XMLUtils.getElementXpath(deviceDoc,"//DeviceConfig");
		}

		return DocumentHelper.createDocument((Element)configEl.clone());
	}


	@Override
	public Document getDeviceCommands() throws Exception {
		Document deviceDoc =this.deviceFrameWork.getDeviceDB().getDeviceConfig();
		Element commandEl = XMLUtils.getElementXpath(deviceDoc,"//DeviceCommand");	
		return DocumentHelper.createDocument((Element)commandEl.clone());
	}

	/**
	 * Hands off the doc to (DeviceImpl.handleMessage such that it gets handled by)
	 * DeviceAdmin.[notificationType]Hander
	 */
	@Override
	public void handleEngineNotification(String notificationType, Document notificationDocument) {
		
		try {
		    String servicename ="";
		    // All the mediastore related system events will be converted to MediaStoreEvents.
		    if(notificationType.equals("NASEvent") || "MediaFileConverterEvent".equals(notificationType) || "NfsUploadEvent".equals(notificationType)){
		        servicename = DeviceAdminConst.MEDIA_STORE_SERVICE; 
		        notificationType = "MediaStoreEvent";
		       	notificationDocument = convertSystemEventToMediaStoreEvent(notificationDocument);
		    }else {
		        servicename = DeviceAdminConst.DEVICE_ADMIN_SERVICE;
		    }
			Message eventMessage = Message.createEvent(notificationDocument.getRootElement(),deviceFrameWork.getMyJID() , deviceFrameWork.getMyJID(),
			        servicename, notificationType, String.valueOf(Level.INFO), Level.INFO);
			deviceFrameWork.handleMessage(eventMessage);
		} catch (Exception e) {		
			e.printStackTrace();
		}
	}
	
	
	/**
	 * Updates the deviceDB.DeviceStatus (by querying the running system), and combines that
	 * info with the DeviceConfig, into a Device-document containing the device ports.  
	 * 
	 * @return XML
	 * <Device type='XPI/DMS/MS' softwareVersion=''>
	     <PortList>
	       <Port type='SrcPort/DstPort/NetworkPort/RelayPort/StorePort' id='1/2'>
				<NetworkStatus />
				<DisplayConfig />
				<DisplayStatus />					
			</Port>
	     </PortList>
	    </Device>
	 *    
	 */
	@Override
	public Document getPublishInfo() throws Exception {
	
		Document deviceConfigDoc =deviceFrameWork.getDeviceDB().getDeviceConfig();
		
		Document deviceStatusDoc = this.getStatus(null);

		
		Element deviceElement = DocumentHelper.createElement(DeviceAdminConst.DEVICE_ELEMENT_NAME);
		deviceElement.addAttribute(DeviceAdminConst.TYPE_ATTRIBUTE_NAME, deviceConfigDoc.getRootElement().getName());		  
		deviceElement.addAttribute(DeviceAdminConst.SOFTWARE_VERSION_ATTRIBUTE_NAME, XMLUtils.getValueXpath(deviceStatusDoc, DeviceAdminConst.SOFTWARE_VERSION_XPATH, ""));
		Element portListElement  = DocumentHelper.createElement(DeviceAdminConst.PORT_LIST_ELEMENT_NAME);
		deviceElement.add(portListElement);
		
		
		//add network ports 
		List<?> networkPortList = XMLUtils.getListOfNodesXpath(deviceConfigDoc, DeviceAdminConst.ETHERNETPORTCONFIG_XPATH);
		
		for(Object object: networkPortList) {
			Element networkEl = (Element) object;
			Element portElement= DocumentHelper.createElement(DeviceAdminConst.PORT_ELEMENT_NAME);
			portElement.addAttribute(DeviceAdminConst.TYPE_ATTRIBUTE_NAME, DeviceAdminConst.NETWORK_PORT);
			String portID = XMLUtils.getValueXpath(networkEl,"./"+ DeviceAdminConst.PORT_ID_ELEMENT_NAME,"");
			portElement.addAttribute(DeviceAdminConst.ID_ATTRIBUTE_NAME, portID);
			Element networkStatusEl = DocumentHelper.createElement(DeviceAdminConst.NETWORK_STATUS_ELEMENT_NAME);
			portElement.add(networkStatusEl);
			List<?> statusEl = XMLUtils.getListOfNodesXpath(deviceStatusDoc, "//EthernetPortStatus/.//Parameters[Parameter[@name='LAN' and .='"+portID+"']]");
			if(statusEl!=null && statusEl.size() !=0) {
				for(Object object1:statusEl) {
					networkStatusEl.add((Element)((Element)object1).clone());
				}
			}
			portListElement.add(portElement);
		}
		
		//add device specific ports 
		List<Element> portResources = getPortResources(deviceConfigDoc, deviceStatusDoc);
		if(portResources != null){
		    for(Element object : portResources){
			portListElement.add(object);	
		    }
		}
			
		/*
		 * add xmppserviceconfig here?
		 */
		return DocumentHelper.createDocument(deviceElement);		
	}
	
	
	protected abstract List<Element> getPortResources(Document configDocument, Document statusDocument) throws Exception; 
	
	protected Document convertSystemEventToMediaStoreEvent(Document systemEvent){
	    Document mediastoreEventDoc = null;
	    try {
            String type = XMLUtils.getValueXpath(systemEvent, "//EventSource", "")+"Event";
            String state = XMLUtils.getValueXpath(systemEvent, "//EventType", "");
            String name = XMLUtils.getValueXpath(systemEvent, "//ID", "");
            String index = XMLUtils.getValueXpath(systemEvent, "//Index", "");
            String error = XMLUtils.getValueXpath(systemEvent, "//error", "");
            if(state.equals(MediaStoreServiceConst.MOUNT)){
                state=MediaStoreServiceConst.ONLINE;
            }else if (state.equals(MediaStoreServiceConst.UNMOUNT)||state.equals(MediaStoreServiceConst.ERROR)||state.equals(MediaStoreServiceConst.DELETE)){
                state=MediaStoreServiceConst.OFFLINE;
            }
            String mediastoreEvent = "";
            if(type.equals("MediaFileConverterEvent")) {
            	String dirNID = XMLUtils.getValueXpath(systemEvent, "//dirNID", "");
        		String fileUUID = XMLUtils.getValueXpath(systemEvent, "//fileUUID", "");
        		String mediaFormat = XMLUtils.getValueXpath(systemEvent, "//mediaFormat", "");
        		//String error = XMLUtils.getValueXpath(systemEvent, "//error", "");
        		String clientID = XMLUtils.getValueXpath(systemEvent, "//clientID", "");
        		String queueID = XMLUtils.getValueXpath(systemEvent, "//queueID", "");
        		String groupFileUUID = XMLUtils.getValueXpath(systemEvent, "//groupFileUUID", "");
        		String isGroupFilePresent = XMLUtils.getValueXpath(systemEvent, "//isGroupFilePresent", "");
        		String typeOfRequest = XMLUtils.getValueXpath(systemEvent, "//typeOfRequest", "");
        		String prepareFileName = XMLUtils.getValueXpath(systemEvent, "//prepareFileName", "");
        		String size = XMLUtils.getValueXpath(systemEvent, "//size", "");

            	mediastoreEvent = "<MediaStoreEvent clientID ='"+clientID+"' type='"+type+"' state='"+state+"' dirNID='"+dirNID+"' fileUUID='"+fileUUID
            			+"' mediaFormat='"+mediaFormat+"' error='"+error+"' dirName='"+name+"' index='"+index+"' queueID='"+queueID+"'  typeOfRequest='"+ typeOfRequest+"' prepareFileName='"+prepareFileName+"'  groupFileUUID='"+ groupFileUUID+"' isGroupFilePresent='"+isGroupFilePresent+"' size='" +size+"'></MediaStoreEvent>";
            } else if ("NfsUploadEvent".equals(type)) {
            	type="OffloadEvent";
            	String clientID = XMLUtils.getValueXpath(systemEvent, "//clientID", "");
            	String dirNID = XMLUtils.getValueXpath(systemEvent, "//dirNID", "");
        		String fileUUID = XMLUtils.getValueXpath(systemEvent, "//fileUUID", "");
            	mediastoreEvent = "<MediaStoreEvent type='"+type+"' state='"+state+"' dirNID='"+dirNID+"' fileUUID='"+fileUUID+"' error='"+error+"' clientID='"+clientID+"'></MediaStoreEvent>";
            } else {
            	mediastoreEvent = "<MediaStoreEvent type='"+type+"' state='"+state+"' dirName='"+name+"' index='"+index+"' error='"+error+"'></MediaStoreEvent>";
            }
            
            mediastoreEventDoc = XMLUtils.stringToDocument(mediastoreEvent);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
	    return mediastoreEventDoc;
	}

	public void haTakeover() throws Exception {}
	
	@Override
	public void reset() {
		// TODO Auto-generated method stub
		
	}	
	
}
