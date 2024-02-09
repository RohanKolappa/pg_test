package com.ipvs.systemservice.handler;

import java.security.SecureRandom;
import java.util.Hashtable;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.app.IPVSClientApplet;
import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.device.handler.SetServiceClientConfigRequestHandler;
import com.ipvs.assetadminservice.dir.handler.PublishDirRequestHandler;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.sessionservice.impl.AutoStartManager.Action;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.resource.handler.SetDirStorageConfigRequestHandler;
import com.ipvs.systemservice.impl.HAUtils;

public class DeviceLoginRequestHandler extends BaseLoginRequestHandler implements MRRequestHandlerI{

	private static final String SERVICE_VERSION_ATTRIBUTE_NAME = "serviceVersion";
	private static final String SOFTWARE_VERSION_ATTRIBUTE_NAME = "softwareVersion";
	private static String STREAMSRC = "StreamSrc";
	private static String STREAMDST = "StreamDst";
	private static String STOREPORT = "StorePort";
	private static String RELAYPORT = "RelayPort";
	private static String NETWORKPORT = "NetworkPort";
	public static final String GET_PORT_REQUEST_XML = "/com/ipvs/sessionservice/session/xml/GetPortRequest.xml";

	private enum PortTypeEnum { SrcPort,DstPort,StorePort,RelayPort,NetworkPort;
	public static String getPortType(String portType) {
		switch(PortTypeEnum.valueOf(portType)){

		case SrcPort: return "StreamSrc"; 
		case DstPort:return "StreamDst";

		case StorePort: return "StorePort";
		case RelayPort: return "RelayPort";

		case NetworkPort:return "NetworkPort";

		default: return "";
		}
	}
	}


	private enum PortTypeToDbObjectMapping { StreamSrc,StreamDst,StorePort,RelayPort,NetworkPort;
	public static String getDbObjectName(String portType,String deviceType) {
		switch(PortTypeToDbObjectMapping.valueOf(portType)){

		case StreamSrc: if(deviceType.equals(AMConst.V2D_TX_DEVICE_OBJECTYPE) || deviceType.equals(AMConst.RTP_TX_DEVICE_OBJECTYPE) ) {
            return AMConst.MEDIA_STREAM_SRC_PORT_RESOURCE_OBJECTTYPE;
        } else {
            return AMConst.MEDIA_STREAM_IO_PORT_RESOURCE_OBJECTTYPE;
        } 
		case StreamDst: if(deviceType.equals(AMConst.V2D_RX_DEVICE_OBJECTYPE) || deviceType.equals(AMConst.IPVS_CLIENT_DEVICE_OBJECTYPE)) {
            return AMConst.MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE;
        } else {
            return AMConst.MEDIA_STREAM_IO_PORT_RESOURCE_OBJECTTYPE;
        } 

		case StorePort: return AMConst.MEDIA_STORE_PORT_RESOURCE;
		case RelayPort: return AMConst.MEDIA_STREAM_RELAY_PORT_RESOURCE_OBJECTTYPE;

		case NetworkPort:return "not supported";

		default: return "";
		}
	}
	}

	@Override
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {

		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		return auth;
	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		//XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());
		XMPPTransaction.startBlock("DeviceLoginRequestHandler.getResponse");
		Element requestDataElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), request.getRequestDataXPath());
		

		String serviceVersion = MRXMLUtils.getAttributeValue(requestDataElement,SERVICE_VERSION_ATTRIBUTE_NAME,"" );
		String softwareVersion = MRXMLUtils.getAttributeValue(requestDataElement, SOFTWARE_VERSION_ATTRIBUTE_NAME,"");	


		//subscribe for user presence
		subscribeForUserPresence(request, xmpp, log);

		//add device type check here
		String deviceType = MRXMLUtils.getAttributeValue(requestDataElement,SessionServiceConst.TYPE_ATTRIBUTE,"" );		
		
		if(!AMConst.V2D_XPi_DEVICE_OBJECTYPE.equals(deviceType)) { //skip version check for NGSD200
			//check the schema version of client and server
			checkSchemaVersion(request,db, log, xmpp, resourceFactory, serviceAgentFactory, serviceVersion, softwareVersion);
		}

		//read all resources use xquery    	
		String resourceTitle = ServiceAgentUtils.getAgentName(request.getUserJID());
		Document resourcesDoc = getResources(request, xmpp, log, db, resourceFactory, serviceAgentFactory,resourceTitle,requestDataElement);
		Element assetResourceEl = MRXMLUtils.getElementXpath(resourcesDoc, "//"+AMConst.AssetResource);
		
		//Update assetResourceNID in ServiceResource Doc in the database  
		Boolean xmppServerEnable = Boolean.valueOf(MRXMLUtils.getAttributeValue(requestDataElement, "xmppServerStatus", "false"));
		String haState = MRXMLUtils.getAttributeValue(requestDataElement, "haStatus", "").trim().toLowerCase();		
		if(xmppServerEnable) {
		    String haType ="";
		  //update HA info if HA is enabled
		    if(!"".equals(haState))		   
			    haType = "true".equals(MRXMLUtils.getAttributeValue(requestDataElement, "primary", "false")) ? "Primary":"Secondary".trim();		    
		    updateServiceResourceInDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, request.getUserJID(), assetResourceEl,haType,haState);		    
			//update Admin Password
		    updateAdminPassword(request, xmpp, log, db, resourceFactory, serviceAgentFactory, request.getUserJID(), requestDataElement, resourcesDoc);
		}
		
				
		String resourceType = MRXMLUtils.getValueXpath(assetResourceEl, ".//"+AMConst.TYPE, "");
		boolean isSelfRegisteredDevice = false;
		if(resourceType.equals(AMConst.ANY_OBJECTTYPE)){
			//update type to device type if device is self registered
			isSelfRegisteredDevice = true;
			MRXMLUtils.setValueXpath(assetResourceEl, ".//"+AMConst.TYPE, deviceType);
			Element templateResourceEl =  MRXMLUtils.getElementXpath(resourcesDoc, "//Template/"+AMConst.AssetResource);

			//Group Element
			Element templateGroupElement = MRXMLUtils.getElementXpath(templateResourceEl, ".//"+SessionServiceResourceConst.GROUPS_ELEMENT);
			Element groupElement = MRXMLUtils.getElementXpath(assetResourceEl, ".//"+SessionServiceResourceConst.GROUPS_ELEMENT);
			MRXMLUtils.replaceContent(groupElement, templateGroupElement);
		}

		//make sure we no need to sync directories
		List<?> dirList = MRXMLUtils.getListXpath(requestDataElement, ".//Dir");
		if((dirList==null || dirList.isEmpty())) {	//Update based on request document states			
			return handleDeviceLogin(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, resourcesDoc, deviceType,  resourceTitle, isSelfRegisteredDevice, true,assetResourceEl,
					serviceVersion, xmppServerEnable, haState);
		} else if(isSelfRegisteredDevice) { //self registered device and needs to sync directory. Add device information first and sync the directory
			handleDeviceLogin(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, resourcesDoc, deviceType, resourceTitle, isSelfRegisteredDevice, false, assetResourceEl, serviceVersion, xmppServerEnable, haState);
			resourcesDoc = getResources(request, xmpp, log, db, resourceFactory, serviceAgentFactory,resourceTitle,requestDataElement);
		}

		//verify sync if sync required startSync will throw error. If sync not needed device will login to the system
		new PublishDirRequestHandler().startSync(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, resourcesDoc,dirList,resourceTitle,deviceType);
		return handleDeviceLogin(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, resourcesDoc, deviceType, resourceTitle, isSelfRegisteredDevice, true, assetResourceEl, serviceVersion, xmppServerEnable, haState);

	}

	private Document handleDeviceLogin(MRRequest request, XMPPI xmpp,	MRLog log, MRDB db, MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory,
			Element requestDataElement, Document resourcesDoc,	String deviceType, String resourceTitle, boolean isSelfRegisteredDevice, boolean useRequestedPresence, 
			Element assetResourceEl,String serviceVersion, boolean xmppServerEnable, String haState) throws Exception {
		//validate asset resource
		validateAssetResource(assetResourceEl, request, deviceType, requestDataElement, resourceTitle);

		//update/add ports
		Element portResourceListElement = updatePorts(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, resourcesDoc, deviceType, isSelfRegisteredDevice, useRequestedPresence);

		//update Device 
		Element deviceElement = updateDevice(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, resourcesDoc, deviceType, serviceVersion, assetResourceEl,portResourceListElement, xmppServerEnable);

		//update resource
		updateResource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, assetResourceEl,useRequestedPresence);
		

		if(useRequestedPresence) {
			String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID,"");
			AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getThreadPoolManager().getAutoStartFixedPool().execute(new HandleStreamRestart(assetResourceNID,portResourceListElement, requestDataElement,resourcesDoc, request,db, log, xmpp, resourceFactory, serviceAgentFactory));
			//new Thread(new HandleStreamRestart(assetResourceNID,portResourceListElement, requestDataElement,resourcesDoc, request,db, log, xmpp, resourceFactory, serviceAgentFactory)).start();
		}
		
		
		if("active".equals(haState)){ // check if standby logged in before active
		    String standbyAssetResourceNID = HAUtils.getMsAssetResourceNID("standby", serviceResourceElement, log);		    
		    if(!standbyAssetResourceNID.isEmpty()){	
			MRRequest newRequest = MRRequest.createRequest(request.getUserJID());  
			String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID,"");	
			sendGetActiveDeviceConfigRequest(newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, assetResourceNID);
			
		    }
		    
		}
		else if("standby".equals(haState)){
		    String activeAssetResourceNID = HAUtils.getMsAssetResourceNID("active", serviceResourceElement, log);		    
		    if(!activeAssetResourceNID.isEmpty()){
			MRRequest newRequest = MRRequest.createRequest(request.getUserJID());
			sendGetActiveDeviceConfigRequest(newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, activeAssetResourceNID);
		    }
		}
		XMPPTransaction.endBlock("DeviceLoginRequestHandler.getResponse");
		return createResponse(request,requestDataElement,assetResourceEl,portResourceListElement,deviceElement);

	}
	private void sendGetActiveDeviceConfigRequest(MRRequest request,  XMPPI xmpp,	MRLog log, MRDB db, MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory, String assetResourceNID) throws Exception{
	    String requestName = MRRequest.getRequestName(GetActiveDeviceConfigRequestHandler.class.getName());
		Document getActiveDeviceConfigRequestDoc = MRXMLUtils.loadXMLResource("/com/ipvs/systemservice/xml/GetActiveDeviceConfigRequest.xml", log);
		Element getActiveDeviceConfigEL = MRXMLUtils.getElementXpath(getActiveDeviceConfigRequestDoc, "//GetActiveDeviceConfig");
		MRXMLUtils.addAttribute(getActiveDeviceConfigEL, SessionServiceResourceConst.BY_NID_ATTRIBUTE, assetResourceNID);	

		Document getActiveDeviceConfigDoc = SessionServiceUtils.getRequestXML(requestName, log, getActiveDeviceConfigEL);
		MRRequest getActiveDeviceConfigRequest = MRRequest.createRequest(request, getActiveDeviceConfigDoc, requestName, ServiceClient.SYSTEMSERVICE);

		GetActiveDeviceConfigRequestHandler getActiveDeviceConfigRequestHandler  = new GetActiveDeviceConfigRequestHandler();
		getActiveDeviceConfigRequestHandler.getResponse(getActiveDeviceConfigRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	private Document createResponse(MRRequest request,Element requestDataElement,	Element assetResourceEl, Element portResourceElement, Element deviceElement) throws Exception {
		Element responseDeviceElement = MRXMLUtils.newElement(AMConst.Device);
		MRXMLUtils.addAttribute(responseDeviceElement, AMConst.NID, MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID,""));
		Element portListEL = (Element)MRXMLUtils.getElement(requestDataElement, "DeviceAdminServiceInfo").clone();
		portListEL.setName("PortList");
		MRXMLUtils.addElement(responseDeviceElement,portListEL);
		Element deviceXMLElement = MRXMLUtils.newElement("DeviceXML");
		MRXMLUtils.addAttribute(deviceXMLElement, AMConst.NID, MRXMLUtils.getAttributeValue(deviceElement, AMConst.NID,""));		
		MRXMLUtils.addElement(responseDeviceElement, deviceXMLElement);
		//add directory information
		MRXMLUtils.addContent(responseDeviceElement, MRXMLUtils.getListXpath(requestDataElement, ".//Dir"));
		//add GC Stream List		
		MRXMLUtils.addElement(responseDeviceElement,(Element)MRXMLUtils.getElementXpath(portResourceElement, ".//DiscardStreamList").clone());
		MRXMLUtils.addElement(responseDeviceElement, (Element)MRXMLUtils.getElementXpath(assetResourceEl, ".//"+SessionServiceResourceConst.API_KEY_TABLE).clone());
		return request.createResponseData(responseDeviceElement);
	}

	private void updateResource(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Element requestDataElement,Element assetResourceEl,boolean userRequestedPresence) throws Exception,MRException {
		
		Element apiKeyElement = MRXMLUtils.getElementXpath(assetResourceEl, ".//"+SessionServiceResourceConst.API_KEY_ELEMENT_NAME);		
		if("".equals(MRXMLUtils.getAttributeValue(apiKeyElement, SessionServiceResourceConst.KEY_ATTRIBUTE, "").trim())){
			String apikey = ""+ new SecureRandom().nextLong();
			String secret = ""+ new SecureRandom().nextLong();
			MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.KEY_ATTRIBUTE, apikey);
			MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.SHARED_SECRET_ATTRIBUTE, secret);
			MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.VALID_INTERVAL_ATTRIBUTE,String.valueOf(SessionServiceResourceConst.API_KEY_VALID_INTERVAL));
		}
		//update state and contact
		updateStateAndPresence(assetResourceEl, request.getUserJID(), requestDataElement,log,userRequestedPresence);

		//update asset resource		
		MRXMLDBUtils.updateElement(request, log, db, MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID), assetResourceEl);

	}

	private Element updateDevice(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Element requestDataElement,Document resourcesDoc,String deviceType, 
			String serviceVersion,Element assetResourceEl,Element portResourceElement, boolean xmppServerEnable) throws Exception,MRException {
		Element deviceElement = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(resourcesDoc, "//Device"));

		//TODO: Fix this code for auto discovery feature 
		if(deviceElement == null) {
			Document deviceDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, deviceType, log);
			if(deviceDoc == null) {
				return null;
			}
			List<Element> portElements = MRXMLUtils.getChildren(portResourceElement);
			for(Element portElement : portElements) {
				String portObject = portElement.getName();
				String portNID = MRXMLUtils.getAttributeValue(portElement, AMConst.NID,"");
				Element resourcPointerNIDValueElement = MRXMLUtils.getElementXpath(deviceDoc, "//"+portObject+SessionServiceConst.NID+"["+SessionServiceResourceConst.NID_VALUE+"='']/"+SessionServiceResourceConst.NID_VALUE);
				MRXMLUtils.setValue(resourcPointerNIDValueElement, portNID);
			}

			MRXMLUtils.setValueXpath(deviceDoc, "//AssetResourceNID", MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID,""));
			deviceElement = deviceDoc.getRootElement();
		}
		//update network status
		//TODO: Temporary fix to include network status. Fix this code when device xml is removing from database and adding network status
		List<?> ethernetStatusList = MRXMLUtils.getListXpath(deviceElement, ".//EthernetPortStatus");
		for(int index = 0; index<ethernetStatusList.size();index++) {
			Element ethernetStatusElement  = (Element)ethernetStatusList.get(index);   				
			MRXMLUtils.replaceContent(ethernetStatusElement, MRXMLUtils.getElementXpath(requestDataElement.getDocument(), "//Port[@type='NetworkPort' and @id='"+(index+1)+"']/NetworkStatus"));
		}

		//update software version 	
		String softwareVersion = MRXMLUtils.getAttributeValue(requestDataElement, "softwareVersion","");
		if(!softwareVersion.isEmpty()) {		
			Element softwareVersionEl = getSoftwareStatusElement(softwareVersion, serviceVersion, log, request);		
			MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(deviceElement, ".//SoftwareImageStatus"),softwareVersionEl);
		}
		
		//update xmpp Server Enable
		MRXMLUtils.setValueXpath(deviceElement, ".//XMPPServerEnable", String.valueOf(xmppServerEnable));
		
		//update Admin Password
		MRXMLUtils.setValueXpath(deviceElement, ".//AdminServiceConfig/Password", MRXMLUtils.getAttributeValue(requestDataElement, "sshAdminPassword", ""));
		
		List<?> dstPortList = MRXMLUtils.getListXpath(requestDataElement, ".//Port[@type='"+PortTypeEnum.DstPort+"']");

		for(Object object:dstPortList) {
			Element portElement = (Element) object;
			String xpathPrefix = "";
			//TODO: find better way to know element name 
			if(deviceType.startsWith("V2D-XP")){
				xpathPrefix ="XP200";
			} else if (deviceType.startsWith("MNA-")){
				xpathPrefix ="MNA";
			} else if (deviceType.startsWith("V2D-Rx")){
				continue;
			} else {
				xpathPrefix = "IPVSClient";
			}
			String portId = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.ID,"");			String configXpath = ".//AVCodecElementsConfig/*"+"[PortID='"+portId+"']/"+xpathPrefix+"DisplayConfig";
			String statusXpath = ".//AVCodecElementsStatus/*"+"[PortID='"+portId+"']/"+xpathPrefix+"DisplayStatus";


			Element DisplayConfigEl = MRXMLUtils.getElementXpath(deviceElement, configXpath);
			if(DisplayConfigEl != null){
				MRXMLUtils.replaceContent(DisplayConfigEl, MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(portElement, "./DisplayConfig")));				
			}
			Element DisplayStatusEl = MRXMLUtils.getElementXpath(deviceElement, statusXpath);
			if(DisplayStatusEl != null){
				MRXMLUtils.replaceContent(DisplayStatusEl, MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(portElement, "./DisplayStatus")));				
			}
		}

		//update disk status		
		List<?> storePortList = MRXMLUtils.getListXpath(requestDataElement, ".//Port[@type='"+PortTypeEnum.StorePort+"']");
		for(Object object:storePortList) {
			Element storePort = (Element) object;
			Element diskTableStatus = MRXMLUtils.getElement(storePort, "HardDiskTableStatus");
			if(diskTableStatus == null) {
				continue;
			}
			Element diskTableStatusInDatabase = MRXMLUtils.getElementXpath(deviceElement, ".//HardDiskTableStatus");
			if(diskTableStatusInDatabase==null) {
				continue;
			}

			MRXMLUtils.replaceContent(diskTableStatusInDatabase, diskTableStatus);
		}


		//update device XML
		String deviceNID = MRXMLUtils.getAttributeValue(deviceElement, SessionServiceConst.NID,"");
		if(deviceNID.isEmpty()) {
			log.addInfo("DeviceLoginRequestHandler: Add Device userJID:="+request.getUserJID(),MRLog.INFO,MRLog.NOTIFY);
			MRXMLDBUtils.add(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.Device, "", "", null, deviceElement);
		} else {
			log.addInfo("DeviceLoginRequestHandler: Update Device userJID:="+request.getUserJID(),MRLog.INFO,MRLog.NOTIFY);
			MRXMLDBUtils.updateElement(request, log, db, deviceNID, deviceElement);
		}



		return deviceElement;
	}
	
	protected  void updateAdminPassword(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID, Element requestDataElement, Document resourcesDoc) throws Exception {
		Element deviceEL = MRXMLUtils.getFirstChild(MRXMLUtils.getElementXpath(resourcesDoc, "//Device"));
		if(deviceEL == null) {
            return;
        }
		String deviceNID = MRXMLUtils.getAttributeValue(deviceEL, SessionServiceConst.NID,"");
		String xpath = "//AdminServiceConfig";
		Element updateElement = MRXMLUtils.getElementXpath(deviceEL, "."+xpath);
		if(updateElement != null) {
			MRXMLUtils.setValueXpath(updateElement, ".//Password", MRXMLUtils.getAttributeValue(requestDataElement, "sshAdminPassword"));
			request.setSuperUser(true);
			new SetServiceClientConfigRequestHandler().updateAdminPassword(request, xmpp, log, db, serviceAgentFactory, resourceFactory, deviceNID, updateElement, xpath);
		}
	}


	private Element updatePorts(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Element requestDataElement,Document resourcesDoc,String deviceType, boolean isSelfRegisteredDevice,boolean useRequestedPresence) throws Exception,MRException  {
		//TODO: temporary fix for network resource
		List<?> portList = MRXMLUtils.getListXpath(requestDataElement, ".//Port[@type!='"+NETWORKPORT+"']");
		Element devicePortList = MRXMLUtils.newElement("PortList");
		Element discardStreamList = MRXMLUtils.newElement("DiscardStreamList");
		Element layoutServiceInfoEl = MRXMLUtils.newElement("LayoutServiceInfo");
		Element mediaServiceInfoEl = MRXMLUtils.newElement("MediaServiceInfo");
		MRXMLUtils.addElement(discardStreamList,mediaServiceInfoEl);
		MRXMLUtils.addElement(discardStreamList,layoutServiceInfoEl);		
		MRXMLUtils.addElement(devicePortList, discardStreamList);
		Hashtable<String, String> portObjNIDList = new Hashtable<String, String>();
		for(Object object:portList){
			Element portElement = (Element) object;
			String portType = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.TYPE_ATTRIBUTE,"");
			String portId = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.ID,"1");
			String maxInstanceCount = MRXMLUtils.getAttributeValue(portElement, SessionServiceResourceConst.MAX_INSTANCE_COUNT_ATTRIBUTE,"1");
			String thumbnailSupport = MRXMLUtils.getAttributeValue(portElement, "thumbnail","false");
			String portTypeEnum = PortTypeEnum.getPortType(portType);
			String portXpath = "";
			String xpath = "";
			Element parentEL=null;
			if(portTypeEnum.equals(STREAMSRC) || portTypeEnum.equals(STREAMDST)){
				portXpath = "//IOPortList/*";
			} else if(portTypeEnum.equals(STOREPORT)) {
				portXpath = "//StorePortList/*";
			} else if(portTypeEnum.equals(RELAYPORT)) {
				portXpath = "//RelayPortList/*";
			} //TODO: include network resource


			portXpath +="[.//DeviceElementID='"+portId+"']";
			Element portResourceElement = MRXMLUtils.getElementXpath(resourcesDoc, portXpath);
			String dbObjectType = "";

			String operation = "";

			//Add port if not exists in db
			dbObjectType = PortTypeToDbObjectMapping.getDbObjectName(portTypeEnum, deviceType);
			Document portResourceDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, dbObjectType, log);
			if(portResourceElement == null) {

				if(portResourceDoc == null) {
					return null;
				}
				Element assetResourceEl = MRXMLUtils.getElementXpath(resourcesDoc, "//"+AMConst.AssetResource);
				//set title
				String assetTitle = MRXMLUtils.getValueXpath(assetResourceEl, ".//Title", "");
			
				
				String portTitle = assetTitle;
				
				if(!deviceType.startsWith("DMS")){ 	
					
					//we are using last 6 letters of asset resource title for port title				
					if(assetTitle.length() > 6) {
						assetTitle = StringUtils.substring(assetTitle, assetTitle.length()-6,assetTitle.length());
					}

					portTitle=assetTitle + "_CHANNEL"+portId;
					
				}
				
				MRXMLUtils.setValueXpath(portResourceDoc, "//Title", portTitle);
				MRXMLUtils.setValueXpath(portResourceDoc, "//AssetResourceNID", MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID,""));

				//update tags
				MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(portResourceDoc, ".//Groups"), MRXMLUtils.getElementXpath(assetResourceEl, "//Groups"));

				//update port type and id 
				MRXMLUtils.setValueXpath(portResourceDoc, "//Type", portTypeEnum);
				MRXMLUtils.setValueXpath(portResourceDoc, "//DeviceElementID", portId); 
				//update stream type
				String[] streamTypes = StringUtils.split(MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.STREAM_TYPE,""),",");
				if(streamTypes != null && streamTypes.length !=0) {
					Element streamTypeListElement = MRXMLUtils.getElementXpath(portResourceDoc, "//"+SessionServiceResourceConst.STREAM_TYPE_LIST_ELEMENT);
					Element streamTypeEL = (Element)MRXMLUtils.getElement(streamTypeListElement, SessionServiceResourceConst.STREAM_TYPE_ELEMENT).detach();
					for(String streamType:streamTypes) {
						MRXMLUtils.setValue(streamTypeEL, streamType);
						MRXMLUtils.addElement(streamTypeListElement,(Element)streamTypeEL.clone());
					}
				}				

				portResourceElement = portResourceDoc.getRootElement();
				//find is there any streams we have to GC if it is new object we are adding.
				if(portTypeEnum.equals(STREAMSRC) ){
					xpath=".//MediaServiceInfo/StreamInfoList/StreamInfo[.//sourcePortElementID='"+portId+"']";
					parentEL = mediaServiceInfoEl;
				}
				else if( portTypeEnum.equals(STREAMDST)){
					xpath=".//LayoutServiceInfo/StreamInfoList/StreamInfo[.//destPortElementID='"+portId+"']";
					parentEL=layoutServiceInfoEl;

				} else {
					xpath="";
				}


				List<?> mediaStreamProfileInfoElList  = MRXMLUtils.getListXpath(portResourceDoc, "//"+SessionServiceResourceConst.MEDIA_STREAM_PROFILE_INFO);
				List<String> streamTypeList = MRXMLUtils.getValuesXpath(portResourceDoc, "//StreamType");    		

				//log.addInfo("AddDeviceRequestHandler::portType: "+portType, MRLog.OK, MRLog.NOTIFY);
				for(Object mediaStreamProfileInfoObject:mediaStreamProfileInfoElList) {
					Element mediaStreamProfileInfoEle = (Element)mediaStreamProfileInfoObject;  
					List<?> mediaStreamProfiles=MRXMLUtils.getChildren(mediaStreamProfileInfoEle);
					for(Object streamProfileInfoELeObj:mediaStreamProfiles){
						Element streamProfileInfoELe = (Element)streamProfileInfoELeObj;    				
						String type = streamProfileInfoELe.getName().substring(0, streamProfileInfoELe.getName().length()-SessionServiceResourceConst.MEDIA_STREAM_PROFILE_INFO.length());
						if(!streamTypeList.contains(type)){
							MRXMLUtils.removeChild(mediaStreamProfileInfoEle,type+SessionServiceResourceConst.MEDIA_STREAM_PROFILE_INFO);
						}
					}    			
				}    	


			} else {

				//update port type in database. Applicable only source or destination ports
				if(portTypeEnum.equals(STREAMSRC) || portTypeEnum.equals(STREAMDST)){
					MRXMLUtils.setValueXpath(portResourceElement, ".//Type", portTypeEnum);
				}

				String portNID = MRXMLUtils.getAttributeValue(portResourceElement, SessionServiceConst.NID,"");
				if(portTypeEnum.equals(STREAMSRC) ){
					xpath=".//MediaServiceInfo/StreamInfoList/StreamInfo[.//sourcePortElementID='"+portId+"' and .//sourcePortNID!='"+portNID+"']";
					parentEL = mediaServiceInfoEl;
				}
				else if( portTypeEnum.equals(STREAMDST)){
					xpath=".//LayoutServiceInfo/StreamInfoList/StreamInfo[.//destPortElementID='"+portId+"' and .//destPortNID!='"+portNID+"']";
					parentEL=layoutServiceInfoEl;

				} else {
					xpath="";
				}

			}
			
			//if port is destination set operated mode as REMOTE
			if(portTypeEnum.equals(STREAMDST)){
			    MRXMLUtils.setValueXpath(portResourceDoc, "//RemotelyOperatedMode", "true");
			}
			//we should not accept any streams if object does not exist in database
			if(!xpath.isEmpty()) {

				List<?> listOfGCStreams = MRXMLUtils.getListXpath(requestDataElement, xpath);	
				log.addInfo("DeviceLoginRequestHandler: Found State Stream for xpath :="+xpath +" size:="+listOfGCStreams.size(),MRLog.INFO,MRLog.NOTIFY);
				if(listOfGCStreams!=null) {
					for(Object gcStreamObject:listOfGCStreams){
						Element discardStreamEl = (Element) gcStreamObject;
						discardStreamEl.detach();
						parentEL.add(discardStreamEl);
					}
				}
			}
			String portNID = MRXMLUtils.getAttributeValue(portResourceElement, SessionServiceConst.NID,"");		

			//update state and contact
			updateStateAndPresence(portResourceElement, request.getUserJID(), portElement,log,useRequestedPresence);
			updateFeatureListInfo(portResourceElement, portType, deviceType,thumbnailSupport);
			//update autostart config for self registered devices
			if(isSelfRegisteredDevice && (portTypeEnum.equals(STREAMSRC)||(portTypeEnum.equals(STREAMDST)))){
				Element streamTypeEle = MRXMLUtils.getElementXpath(portResourceDoc, "//StreamTypeList[StreamType='"+MediaRoomServiceConst.V2D
			    		+ "' or StreamType='"+MediaRoomServiceConst.RTP_RAW
			    		+ "' or StreamType='"+MediaRoomServiceConst.RTP_PCM+"']");
			    if(streamTypeEle!= null){				
			    	AssetAdminServiceUtils.setSourceDefaultParameters(portResourceElement,SessionServiceConst.MODE_AUTO_START, log);			    		
			    }
			    
			    // update thumbnail config to false if streamType is not V2D or RTP-RAW
			  /*  streamTypeEle = MRXMLUtils.getElementXpath(portResourceDoc, "//StreamTypeList[not(StreamType='"+MediaRoomServiceConst.RTP_RAW + "')]");
			    if(streamTypeEle != null) {
			    	if(deviceType.equals(AMConst.V2D_XPi_DEVICE_OBJECTYPE)	|| ( !deviceType.startsWith("MNA") && !deviceType.startsWith("V2D-XP"))) {
			    		MRXMLUtils.setValueXpath(portResourceDoc.getRootElement(), ".//ThumbnailConfig/Enable", SessionServiceConst.FALSE);
			    	}
			    }*/
				
			}

			//update max instance count
			MRXMLUtils.setValueXpath(portResourceElement, ".//"+SessionServiceResourceConst.MAX_INSTANCE_COUNT_ELEMEMNT, maxInstanceCount);


			MRXMLUtils.setValueXpath(portResourceElement, ".//"+SessionServiceResourceConst.MEDIA_STREAM_URL_ELEMENT, MRXMLUtils.getValueXpath(portElement, "./StreamURL", ""));

			if(portNID.isEmpty()) {
				MRXMLDBUtils.add(MRXMLDBUtils.TIME_OUT, request, log, db, "", dbObjectType, "", "", null, portResourceElement);
				log.addInfo("DeviceLoginRequestHandler: Add Port userJID:="+request.getUserJID()+" objectType:="+dbObjectType,MRLog.INFO,MRLog.NOTIFY);	
				operation = "Add";
			} else {				
				//update port resource
				MRXMLDBUtils.updateElement(request, log, db, portNID, portResourceElement);
				operation = "Update";
			}
			
			if(portTypeEnum.equals(RELAYPORT)){
				StreamPolicyUtils.handlePolicyChanged();
			}

			if(portResourceElement!=null) {
				devicePortList.add(portResourceElement.detach());
				String nid = MRXMLUtils.getAttributeValue(portResourceElement, AMConst.NID,"");
				MRXMLUtils.addAttribute(portElement, AMConst.NID, nid);
				portObjNIDList.put(nid, operation);
			}
		}

		if(portObjNIDList != null){
			AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getThreadPoolManager().getNotificationFixedPool().execute(new PortUpdateNotifier(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portObjNIDList));
			//new Thread(new PortUpdateNotifier(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portObjNIDList)).start();
		}

		return devicePortList;
	}

	private void updateFeatureListInfo(Element portResourceElement, String portType,String deviceType, String thumbnailSupport) throws Exception {
	    String portTypeEnum = PortTypeEnum.getPortType(portType);
	    if(portTypeEnum.equals(STREAMSRC) && thumbnailSupport.equals("true")){
	        //add thumbnail feature
	        Element featureListEL = MRXMLUtils.getElementXpath(portResourceElement, ".//FeatureList");
	        MRXMLUtils.removeNodeXpath(featureListEL.getDocument(), "//Feature[@Name='']");
	        Element featureEL = MRXMLUtils.getElementXpath(portResourceElement, ".//Feature[@Name='"+SessionServiceConst.THUMBNAIL+"']");
	        System.out.println("featureEL>>>"+featureEL);
	        if(featureEL == null) {
	            featureEL = MRXMLUtils.addElement("Feature");       
	            MRXMLUtils.setAttributeValue(featureEL, "Name", SessionServiceConst.THUMBNAIL);
	            MRXMLUtils.addElement(featureListEL, featureEL);
	        }
	    }
		if(!portType.equals(STREAMDST) && !deviceType.startsWith("V2D-XP")){
			return;
		}
		//add Display config feature
		Element featureListEL = MRXMLUtils.getElementXpath(portResourceElement, ".//FeatureList");
		MRXMLUtils.removeNodeXpath(featureListEL.getDocument(), "//Feature[@Name='']");
		Element featureEL = MRXMLUtils.getElementXpath(portResourceElement, ".//Feature[@Name='"+SessionServiceConst.XP200DISPLAYCONFIG+"']");

		if(featureEL == null) {
			featureEL =	MRXMLUtils.addElement("Feature");		
			MRXMLUtils.setAttributeValue(featureEL, "Name", SessionServiceConst.XP200DISPLAYCONFIG);
			MRXMLUtils.addElement(featureListEL, featureEL);
		}

		//add trick play feature 
		featureEL = MRXMLUtils.getElementXpath(portResourceElement, ".//Feature[@Name='"+ SessionServiceResourceConst.TRICK_PLAY_FEATURE_NAME+"']");
		if(featureEL == null) {
			featureEL = MRXMLUtils.addElement("Feature");   
			MRXMLUtils.setAttributeValue(featureEL, "Name", SessionServiceResourceConst.TRICK_PLAY_FEATURE_NAME);
			MRXMLUtils.setValue(featureEL, SessionServiceConst.TRUE);
			MRXMLUtils.addElement(featureListEL, featureEL);
		}
	}

	private void autoStartDevice(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,Element requestDataElement, Document resourceDoc) throws Exception{

		Element ethernetPortConfigEL = MRXMLUtils.getElementXpath(resourceDoc, "//EthernetPortStatus/Parameters[Parameter[@name='Status']='True' and Parameter[@name='Primary']='True']/Parameter[@name='IPAddress']");
		if(ethernetPortConfigEL != null) {
            log.addInfo("DeviceLoginRequestHandler:autoStartDevice ethernetPortConfigEL="+ethernetPortConfigEL.asXML());
        }
		String ipAddress ="";
		if(ethernetPortConfigEL!=null) {
			//System.out.println("In get IP Address based on url scheme " + MRXMLUtils.elementToString(ethernetPortConfigEL));
			ipAddress = ethernetPortConfigEL.getText();
		}
		log.addInfo("DeviceLoginRequestHandler:autoStartDevice ipaddress="+ipAddress);
		List<?> portList = MRXMLUtils.getListXpath(requestDataElement, ".//*[Info/Type='"+STREAMSRC+"']");
		for(Object object:portList){
			Element portElement = (Element) object;			

			String portNID = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.NID,"");
			String enableAutoStart = MRXMLUtils.getAttributeValueXpath(portElement, ".//Instance", "mode", SessionServiceConst.MODE_DYNAMIC);
			// Update the cache
			if (request.getCache() == null ) {
                request.setCache(new MRRequestCache());
            }
			request.getCache().put(portNID, MRXMLUtils.elementToDocument(portElement));
			if(enableAutoStart.equalsIgnoreCase(SessionServiceConst.MODE_AUTO_START)){
				request.setSuperUser(true);
				AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNID, Action.CONTINUE_AUTOSTART_SESSION, ipAddress);			
			}
			else{
				request.setSuperUser(true);
				AppServerAgentUtils.getAutoStartManager(serviceAgentFactory).handleAutoStartConfigUpdate(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNID, Action.STOP_AUTOSTART_SESSION,"");
			}

		}
	}	

    private class HandleStreamRestart implements Runnable {
	String assetResourceNID;
	MRRequest request;
	XMPPI xmpp;
	MRLog log;
	Element portResourceListElement;
	MRDB db;
	MRResourceFactoryI resourceFactory;
	ServiceAgentFactoryI serviceAgentFactory;
	Element requestDataElement;
	Document resourceDoc;

	private HandleStreamRestart(String assetResourceNID, Element portResourceListElement, Element requestDataElement, Document resourceDoc,
		MRRequest request, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) {
	    this.assetResourceNID = assetResourceNID;
	    this.portResourceListElement = portResourceListElement;
	    this.request = request;
	    this.xmpp = xmpp;
	    this.log = log;
	    this.db = db;
	    this.resourceFactory = resourceFactory;
	    this.serviceAgentFactory = serviceAgentFactory;
	    this.requestDataElement = requestDataElement;
	    this.resourceDoc = resourceDoc;

	}

	@Override
	public void run() {
	    // TODO Auto-generated method stub
	    try {
		// XMPPTransaction.setDEBUG(true,
		// Thread.currentThread().getId());
		XMPPTransaction.startBlock("DeviceLoginRequestHandler.HandleStreamRestart.run");
		RoomDB.getRoomDB().handleAssetResourceNIDAvailable(xmpp, log, assetResourceNID, request.getUserJID());
		Element portResourcEList = portResourceListElement;
		// call to add rooms and restart sessions
		log.addInfo("DeviceLoginRequestHandler.HandleStreamRestart", MRLog.INFO, MRLog.NOTIFY);
		List<String> portList = MRXMLUtils.getValuesXpath(portResourcEList, "./*/@NID");
		//List<String> mediaDirNIDList = MRXMLUtils.getValuesXpath(requestDataElement, ".//Dir/@dirNID");
		List <?> dirList=MRXMLUtils.getListXpath(requestDataElement, ".//Dir");
		
		try {
		    if (portList != null && !portList.isEmpty()) {
			handleSCRestart(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDataElement, portResourceListElement);
			updateResourceContactJIDInMediaRoom(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portList, request.getUserJID());
			// handle persistent pvr session recovery
			for (Object object: dirList){
			    Element dirElement = (Element)object;
			    String pvrDirNID= MRXMLUtils.getAttributeValue(dirElement, "pvrDirNID");
			    String mediaDirNID=MRXMLUtils.getAttributeValue(dirElement, "dirNID");
			    log.addInfo("DeviceLoginRequestHandler.HandleStreamRestart: dirNID:  "+mediaDirNID+":: pvrDirNID: "+pvrDirNID,MRLog.INFO, MRLog.NOTIFY);
			    AppServerAgent theAppServerAgent = (AppServerAgent) serviceAgentFactory.getServiceAgent(IPVSClientApplet.APPSERVERAGENT);
			    theAppServerAgent.handlePersistentPVRSessionRestart(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaDirNID,
				    false);
			    Document dirDoc = MRXMLDBUtils.readElement(request, log, db, mediaDirNID);
			    SetDirStorageConfigRequestHandler.addDirTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirDoc,
				    mediaDirNID);
			    cleanAllPVRFiles(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaDirNID,pvrDirNID);
			}
			// clean the pvr files
			/*for (String mediaDirNID : mediaDirNIDList) {
			    Document dirDoc = MRXMLDBUtils.readElement(request, log, db, mediaDirNID);
			    SetDirStorageConfigRequestHandler.addDirTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirDoc,
				    mediaDirNID);
			    cleanAllPVRFiles(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaDirNID);
			}*/
		    }
		} catch (Exception e) {
			e.printStackTrace();
		    log.addInfo("DeviceLoginRequestHandler:HandleStreamRestart stream recovery failed Failed assetResourceNID=" + assetResourceNID + " Exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
		}

		autoStartDevice(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portResourceListElement, resourceDoc);

		XMPPTransaction.endBlock("DeviceLoginRequestHandler.HandleStreamRestart.run");
		// XMPPTransaction.setDEBUG(false,
		// Thread.currentThread().getId());

	    } catch (Exception e) {
		log.addInfo("DeviceLoginRequestHandler:HandleStreamRestart stream recovery failed Failed assetResourceNID=" + assetResourceNID + " Exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
	    }
	}

    }
}
