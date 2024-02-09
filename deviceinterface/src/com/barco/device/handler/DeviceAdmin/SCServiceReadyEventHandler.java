package com.barco.device.handler.DeviceAdmin;

import java.util.List;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.device.handler.Layout.TeardownRequestHandler;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;


public class SCServiceReadyEventHandler implements MessageHandlerI,ResponseHandlerI{

	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
		
		sendDeviceLoginRequest(device);
	}
	
	
	public void sendDeviceLoginRequest(DeviceI device) throws Exception{

		
		Document deviceLoginRequest = XMLUtils.loadXMLResource("/com/ipvs/systemservice/xml/DeviceLoginRequest.xml");
		Element deviceLoginEl = XMLUtils.getElementXpath(deviceLoginRequest, "//DeviceLoginRequestData");
		XMLUtils.addAttribute(deviceLoginEl, "serviceVersion", device.getServiceVersion());
		XMLUtils.setValueXpath(deviceLoginRequest,"//DevicePrivateKeyData", device.getLoginInfo().getPassword());		
		Document deviceAgentInfoDoc = device.getDeviceAdminAgentImpl().getPublishInfo();

		XMLUtils.addAttribute(deviceLoginEl, DeviceAdminConst.TYPE_ATTRIBUTE_NAME,XMLUtils.getAttributeValueWithDefault(deviceAgentInfoDoc.getRootElement(), DeviceAdminConst.TYPE_ATTRIBUTE_NAME,""));
		XMLUtils.addAttribute(deviceLoginEl, DeviceAdminConst.SOFTWARE_VERSION_ATTRIBUTE_NAME,XMLUtils.getAttributeValueWithDefault(deviceAgentInfoDoc.getRootElement(), DeviceAdminConst.SOFTWARE_VERSION_ATTRIBUTE_NAME,""));

		Element deviceAgentPortListEl = XMLUtils.getElementXpath(deviceAgentInfoDoc, "//"+DeviceAdminConst.PORT_LIST_ELEMENT_NAME);
		XMLUtils.replaceContent( XMLUtils.getElementXpath(deviceLoginRequest, "//"+DeviceAdminConst.DEVICE_ADMIN_SERVICE_INFO),(Element) deviceAgentPortListEl.detach());		
		if(device.getMediaAgentImpl()!=null) {
			Document mediaAdminAgentInfoDoc = device.getMediaAgentImpl().getPublishInfo();
			XMLUtils.replaceContent( XMLUtils.getElementXpath(deviceLoginRequest, "//"+DeviceAdminConst.MEDIA_SERVICE_INFO),(Element) mediaAdminAgentInfoDoc.getRootElement().detach());
		}

		if(device.getLayoutAgentImpl()!=null) {
			Document layoutAgentDoc = device.getLayoutAgentImpl().getPublishInfo();		
			XMLUtils.replaceContent( XMLUtils.getElementXpath(deviceLoginRequest, "//"+DeviceAdminConst.LAYOUT_SERVICE_INFO),(Element) layoutAgentDoc.getRootElement().detach());
		}

		if(device.getMediaStoreAgentImpl()!=null) {
			Document mediaStoreAgentDoc = device.getMediaStoreAgentImpl().getPublishInfo();		
			XMLUtils.replaceContent( XMLUtils.getElementXpath(deviceLoginRequest, "//"+DeviceAdminConst.MEDIASTORE_SERVICE_INFO),(Element) mediaStoreAgentDoc.getRootElement().detach());
		} else {
			XMLUtils.getElementXpath(deviceLoginRequest, "//"+DeviceAdminConst.MEDIASTORE_SERVICE_INFO).clearContent();
		}
		
		Document deviceConfigDoc = device.getDeviceAdminAgentImpl().getDeviceConfig("");
		Document deviceStatus = device.getDeviceAdminAgentImpl().getStatus(null);
		String haConfigEnable = XMLUtils.getValueXpath(deviceConfigDoc, "//HAServerEnable", "false");
		
		if("true".equals(haConfigEnable)){
		    XMLUtils.addAttribute(deviceLoginEl, "haStatus", XMLUtils.getValueXpath(deviceStatus, "//HAServerStatus/Parameters/Parameter[@name='Resource Status']" , ""));		   	
		    XMLUtils.addAttribute(deviceLoginEl, "primary", XMLUtils.getValueXpath(deviceConfigDoc, "//HAServer/Primary", "false"));
		    
		}
		XMLUtils.addAttribute(deviceLoginEl, "xmppServerStatus", XMLUtils.getValueXpath(deviceConfigDoc, "//XMPPServerEnable", "false"));
		XMLUtils.addAttribute(deviceLoginEl, "sshAdminPassword", XMLUtils.getValueXpath(deviceConfigDoc, "//AdminServiceConfig/Password", ""));		
		
		String assetState = XMLUtils.getAttributeValueWithDefault(deviceAgentInfoDoc.getRootElement(), DeviceAdminConst.READY_ATTRIBUTE_NAME,"");
		XMLUtils.addAttribute(deviceLoginEl, DeviceAdminConst.READY_ATTRIBUTE_NAME, assetState);
		
		device.getLogger().debug("DeviceImpl: Send DeviceLoginRequest");
		
		Message deviceLoginRequestMessage =  Message.createRequest(deviceLoginRequest, device.getAppserverJID() , device.getMyJID());		
		device.sendRequest(deviceLoginRequestMessage, deviceLoginRequest, this);
	}
	
	@Override
	public void handleResponse(Message response, Document context,
			DeviceI device) throws Exception {
		Document resultDoc = response.getMessageDoc();
		device.getDeviceDB().updateAPIKey(resultDoc);
		
		try {

			Element deviceEl = XMLUtils.getElementXpath(resultDoc, "//Device");
			device.getDeviceDB().setDeviceResourceNID(XMLUtils.getAttributeValueWithDefault(deviceEl, "NID",""));
			List<?> portList = XMLUtils.getListOfNodesXpath(resultDoc, "//Port");
			for(Object object:portList) {
				Element portElement = (Element) object;
				device.getDeviceDB().addPortResource(XMLUtils.getAttributeValueWithDefault(portElement, DeviceAdminConst.TYPE_ATTRIBUTE_NAME, "") +"_"+ XMLUtils.getAttributeValueWithDefault(portElement, DeviceAdminConst.ID_ATTRIBUTE_NAME, ""),(Element) portElement.detach());

				String portType = XMLUtils.getAttributeValueWithDefault(portElement, "type", "");
				String portId = XMLUtils.getAttributeValueWithDefault(portElement, "id", "");

				//find list of GC streams 
				String xpath = "";
				if(DeviceAdminConst.SRC_PORT.equals(portType) ){
					xpath=".//MediaServiceInfo/StreamInfo[..//sourcePortElementID='"+portId+"']";					
				}
				else if( DeviceAdminConst.DST_PORT.equals(portType)){
					xpath=".//LayoutServiceInfo/StreamInfo[..//destPortElementID='"+portId+"']";						
				} else {
					xpath="";
				}
				if(!xpath.isEmpty()) {

					List<?> listOfDiscardStreams = XMLUtils.getListOfNodesXpath(resultDoc, xpath);
					if(listOfDiscardStreams!=null) {
						for(Object gcStreamObject:listOfDiscardStreams){
							Element gcStreamEl = (Element) gcStreamObject;

							String streamNID = XMLUtils.getAttributeValue(gcStreamEl, "streamNID");
							device.getLogger().debug("DeviceLoginResponse: GCStreamList portId:="+portId +" portType:="+portType +" StreamNID:="+streamNID);

							//if it is source port call stop streams. If destination port call stop playing methods
							if(portType.equals(DeviceAdminConst.SRC_PORT)){
								device.getMediaAgentImpl().stopStream(streamNID);
							} else {
								new TeardownRequestHandler().teardownStream(device, streamNID);
							}

						}
					}
				}
			}
			device.getDeviceDB().setDeviceXMLNID(XMLUtils.getAttributeValueWithDefault(XMLUtils.getElementXpath(resultDoc, "//DeviceXML"), "NID", ""));
			
			//update directory information
			if(device.getMediaStoreAgentImpl() != null) {
				List<?> dirList = XMLUtils.getListOfNodesXpath(resultDoc, "//Dir");				
				for(Object object:dirList) {
					Element dirElement = (Element) object;
	
					//fill  dir information in cache
					String dirId = XMLUtils.getAttributeValueWithDefault(dirElement, "id","");
					String dirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "dirNID","");
					String pvrDirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "pvrDirNID","");
					String recycleDirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "recyclebinDirNID","");
					String dirThreshold = XMLUtils.getAttributeValueWithDefault(dirElement, MediaStoreServiceConst.MEDIA_DIR_DSIK_FULL_THRESHOLD,"");
					String state = XMLUtils.getAttributeValueWithDefault(dirElement, "state","");
					
					device.getMediaStoreAgentImpl().updateDBInformation(dirId, dirNID, pvrDirNID, recycleDirNID, Utils.getLongValue(dirThreshold, 0), state);
					device.getLogger().debug("Update Directory inforamtion in Media Store Cache ID:="+dirId +" state:="+state);
				}
			}
			
			//update device config
			Document deviceConfigDoc = device.getDeviceDB().getDeviceConfig();
			if(deviceConfigDoc != null) {
				Element deviceConfigEl = XMLUtils.getElementXpath(deviceConfigDoc, "//DeviceConfig");
				if(deviceConfigEl != null) {
					Document requestDoc = XMLUtils.stringToDocument("<Device/>");
					XMLUtils.addAttribute(requestDoc.getRootElement(), "xpath", "//DeviceConfig");
					XMLUtils.addAttribute(requestDoc.getRootElement(), "operation", "Update");
					requestDoc.getRootElement().add((Element)deviceConfigEl.clone());
					Message deviceConfigMessage = Message.createEvent(
							(Element)requestDoc.getRootElement().detach(), device.getMyJID(),
							device.getMyJID(),
							DeviceAdminConst.DEVICE_ADMIN_SERVICE,
							DeviceAdminConst.DEVICE_CONFIG_UPDATE_EVENT,
							String.valueOf(Level.INFO), Level.INFO);
					new DeviceConfigUpdateEventHandler().handleDeviceUpdateEvent(deviceConfigMessage, device, "SetServiceClientConfig", "Device");
				}
			}
			
			device.getLogger().debug("deviceDB NID info resourceNID:="+device.getDeviceDB().getDeviceResourceNID() +" \t deviceNID="+ device.getDeviceDB().getDeviceXMLNID());

		} catch (Exception e) {	
			e.printStackTrace();
		}
	}

	@Override
	public void handleError(Message response, Document context, DeviceI device)
			throws Exception {
		Document messageDoc = response.getMessageDoc();
		String code = XMLUtils.getValueXpath(messageDoc, "//Code", "");
		//handle the case where we need to start sycn
		if(code.equals("1042")){
			List<?> dirList = XMLUtils.getListOfNodesXpath(response.getMessageDoc(), "//Log/.//Dir");
			device.getLogger().info("DeviceLogin Request failed. Start Sync process.");			
			for(Object object:dirList) {
				Element dirElement = (Element) object;

				startSyncProcess(dirElement,device);
			}
			sendDeviceLoginRequest(device);

		} else if(code.equals("1200")){
			device.getLogger().debug("DeviceLogin Request failed. Service Version Mismatch. Wait for upgrade device request.");
		} else {
			device.getLogger().debug("DeviceLogin Request failed. Reason:"+XMLUtils.getValueXpath(messageDoc, "//Description", ""));
		}
	}
	
	public void startSyncProcess(Element dirElement,DeviceI device)throws Exception{
            //fill  dir information in cache
            String dirId = XMLUtils.getAttributeValueWithDefault(dirElement, "id","");
            String dirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "dirNID","");
            String pvrDirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "pvrDirNID","");
            String recycleDirNID = XMLUtils.getAttributeValueWithDefault(dirElement, "recyclebinDirNID","");
            String maxRevision = XMLUtils.getAttributeValueWithDefault(dirElement, "mediaStreamFileMaxRevision","");
            
            device.getMediaStoreAgentImpl().startSynch(dirId, maxRevision, dirNID, pvrDirNID, recycleDirNID, 0);
	}
	
}
