package com.ipvs.systemservice.handler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.assetadminservice.impl.MediaManagementUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.mediaroomservice.handler.ConnectionUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoom;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.Session;
import com.ipvs.sessionservice.impl.SessionManager;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateSessionUtils;
import com.ipvs.sessionservice.resource.handler.DeleteFileRequestHandler;
import com.ipvs.systemservice.impl.HAUtils;
import com.ipvs.systemservice.impl.ServiceVersionManager;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public class BaseLoginRequestHandler {


	public static final String GET_RESOURCES_XQUERY_PATH ="/com/ipvs/systemservice/handler/xquery/GetResourcesQuery.xml";
	public static final String _RESOURCETITLE_ = "_RESOURCETITLE_";	
	public static final String _UUIDXPATH_ = "_UUIDXPATH_";
	public static final String _CHECKFORDIR_ = "_CHECKFORDIR_";
	public Element serviceResourceElement = null;
	
	
	public Element getServiceResourceElement() {
	    return serviceResourceElement;
	}

	public void setServiceResourceElement(Element serviceResourceElement) {
	    this.serviceResourceElement = serviceResourceElement;
	}

	protected void checkSchemaVersion(MRRequest request,MRDB db,MRLog log,XMPPI xmpp,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory, String clientServiceVersion, String clientSoftwareVersion) throws MRException, Exception {
		try {
			String serverVersion = ServiceVersionManager.getServiceVersion(log);
			if (!serverVersion.equals(clientServiceVersion)) {
				throw new MRException(MRExceptionCodes.SERVICE_VERSION_MISMATCH, "Service Version Mismatch clientVersion=" + 
						clientServiceVersion + " serverVersion=" + serverVersion);
			}
		}
		catch(Exception e){
			updateResourcesOnServiceVersionMissMatch(request, db, log, xmpp, resourceFactory, serviceAgentFactory, clientSoftwareVersion, clientServiceVersion);
			throw e;
		}
	}

	protected Document serviceLogin(MRRequest request, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRLog log, MRDB db, XMPPI xmpp)
			throws MRException, Exception {
		Document requestDoc = request.getMessage().getDocument();
		String assetTemplateTitle = MRXMLUtils.getValueXpath(requestDoc, "//" + MRConst.ASSET_TEMPLATE_TITLE, "");
		Element assetPrivateData = MRXMLUtils.getElementXpath(requestDoc, "//" + MRConst.ASSET_PRIVATE_DATA);
		String assetPrivateKeyData = MRXMLUtils.getValueXpath(requestDoc, "//" + MRConst.ASSET_PRIVATE_KEY_DATA, "");
		String assetResource = ServiceAgentUtils.getAgentName(request.getUserJID());
		if(IPVSConst.getInstance().isComponentLogin() && request.getUserJID().indexOf("@") == -1 ){
			return MRXMLUtils.stringToDocument("<" + AMConst.AssetResource + "><component_login/></" + AMConst.AssetResource + ">");
		}
		Document assetResourceDoc = MRClientUtils.loginAssetResource(request.getUserJID(), log, db, assetResource, assetTemplateTitle, assetPrivateData, assetPrivateKeyData, xmpp, resourceFactory, serviceAgentFactory);

		return assetResourceDoc;
	}	

	protected void updateResourcesOnServiceVersionMissMatch(MRRequest request, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory, String softwareVersion, String serviceVersion ) throws MRException,Exception {
		Element responseEl  = AppServerAgentUtils.getResourcePresenceManager(serviceAgentFactory).updateResourceContactAndState(request.getUserJID(), false, AMConst.AVAILABLE, MRClientUtils.ASSET_STATE_NEEDS_UPGRADE, db, log, xmpp, resourceFactory, serviceAgentFactory);
		Element deviceEl = MRXMLUtils.getElementXpath(responseEl, ".//Device");
		if(deviceEl!=null) {
			updateSoftwareVersion(MRXMLUtils.elementToDocument(deviceEl), request, xmpp, log, db, resourceFactory, serviceAgentFactory, softwareVersion, serviceVersion );
		}
	}

	protected Element getSoftwareStatusElement(String softwareVersion,String serviceVersion,MRLog log,MRRequest request) throws Exception {

		if(softwareVersion == null || softwareVersion.isEmpty()) {
			return null;
		}

		String xmlFilePath = "/com/ipvs/xsd/objectXml/SoftwareImageStatus.xml";        
		Document softwareImageStatusDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);
		Element softwareImageStatusEL = softwareImageStatusDocument.getRootElement();
		MRXMLUtils.setValueXpath(softwareImageStatusEL, "Parameters/Parameter[@name='Version']", softwareVersion);
		MRXMLUtils.setValueXpath(softwareImageStatusEL, "Parameters/Parameter[@name='Service Version']", serviceVersion);
		log.addInfo("Update Software Version  UserJID = " + request.getUserJID() +" softwareVersion:="+softwareVersion +" ServiceVersion:="+serviceVersion, MRLog.INFO, MRLog.NOTIFY);
		return softwareImageStatusEL;
	}

	protected void updateSoftwareVersion(Document deviceDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String softwareVersion, String serviceVersion ) throws Exception {

		if(deviceDoc == null) {
			return;
		}

		String childXpath = "//SoftwareImageStatus";
		String deviceNID = deviceDoc.getRootElement().attributeValue("NID");
		Element softwareImageStatusEL = getSoftwareStatusElement( softwareVersion, serviceVersion, log, request);
		if(softwareImageStatusEL != null) {
			AssetAdminServiceUtils.updateChild(deviceNID, childXpath, softwareImageStatusEL, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
		}

	}


	protected List<?> getListOfPorts(Document deviceDoc) throws MRException, Exception {
		if (deviceDoc == null) {
			return null;
		}

		return MRXMLUtils.getListXpath(deviceDoc, "//ResourcePointers/*/NIDValue");
	}

	protected void updateEthernetPortIPAddress(Document deviceDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		if(deviceDoc == null) {
			return;
		}
		String childXpath = "";
		Element ethernetPortConfigEL=null;
		Document requestDoc = request.getMessage().getDocument();
		String serviceDomainIPAddress = MRXMLUtils.getValueXpath(requestDoc, "//IPAddress", "");
		log.addInfo("BaseLoginRequestHandler.updateEthernetPortIPAddressserviceDomainIPAddressr=" + serviceDomainIPAddress, MRLog.OK, MRLog.NOTIFY);
		String deviceNID = deviceDoc.getRootElement().attributeValue("NID");
		if(serviceDomainIPAddress!=null && !"".equalsIgnoreCase(serviceDomainIPAddress)) {
			//read Ethernet port config if the primary is enabled 
			childXpath = "//EthernetPortConfig[IsPrimary='" + MRConst.TRUE + "'" + "and EnableInterface='" + MRConst.TRUE +  "']";
			ethernetPortConfigEL = AssetAdminServiceUtils.getChild(deviceNID, childXpath, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
			List<?> ethernetPortConfigList = MRXMLUtils.getListXpath(ethernetPortConfigEL, "./EthernetPortConfig");
			if(ethernetPortConfigList!=null && ethernetPortConfigList.size()>0) {
				//remove the Ethernet port status if the primary is enabled
				childXpath =  "//EthernetPortStatus[Parameters/Parameter[@name = 'Primary' and . = 'True']]";            			
				AssetAdminServiceUtils.deleteChild(deviceNID, childXpath, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
				Element ethernetPortConfig = (Element)ethernetPortConfigList.get(0);
				String portID = MRXMLUtils.getValueXpath(ethernetPortConfig, "PortID", "");
				//add new Ethernet port status element for given IPAddress
				addEthernetPortStatusElement(deviceNID, serviceDomainIPAddress, MRConst.TRUE, "", portID, request,xmpp,log,db,resourceFactory,serviceAgentFactory);
			}	
		} else {
			
			//read the Ethernet port config if the interface is enabled.  
			childXpath = "//EthernetPortConfig[.//UseDHCP='" + MRConst.FALSE + "'" + " and .//EnableInterface='" + MRConst.TRUE +  "']";
			ethernetPortConfigEL = AssetAdminServiceUtils.getChild(deviceNID, childXpath, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
			List<?> ethernetPortConfigList = MRXMLUtils.getListXpath(ethernetPortConfigEL, ".//EthernetPortConfig");
			//reset the Ethernet port status table only if DHCP is not enabled 
			if(ethernetPortConfigList != null && ethernetPortConfigList.size() > 0){
				childXpath =  "//EthernetPortStatus";
				AssetAdminServiceUtils.deleteChild(deviceNID, childXpath, request, log, db, xmpp, resourceFactory, serviceAgentFactory);
			}
			for(int i=0;ethernetPortConfigList!=null && i<ethernetPortConfigList.size();i++) {
				Element ethernetPortConfig = (Element)ethernetPortConfigList.get(i);
				String portID = MRXMLUtils.getValueXpath(ethernetPortConfig, "PortID", "");
				String isPrimary = MRXMLUtils.getValueXpath(ethernetPortConfig, "IsPrimary", "");
				String ipAddress = MRXMLUtils.getValueXpath(ethernetPortConfig, "IPConfig/IPAddress", "");
				String netmask = MRXMLUtils.getValueXpath(ethernetPortConfig, "IPConfig/Netmask", "");
				//copy the config to status and add to the device xml
				addEthernetPortStatusElement(deviceNID, ipAddress, isPrimary, netmask, portID,request,xmpp,log,db,resourceFactory,serviceAgentFactory);            			
			}            			
		}        
	}

	protected void addEthernetPortStatusElement(String deviceNID,String ipAddress, String isPrimary, String netmask, String portID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		String xmlFilePath = "/com/ipvs/xsd/objectXml/EthernetPortStatus.xml";
		Document ethernetPortStatusDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);
		Element ethernetPortStatusEL = ethernetPortStatusDocument.getRootElement();
		MRXMLUtils.setValueXpath(ethernetPortStatusEL, "Parameters/Parameter[@name='LAN']", portID);
		MRXMLUtils.setValueXpath(ethernetPortStatusEL, "Parameters/Parameter[@name='IPAddress']", ipAddress);
		MRXMLUtils.setValueXpath(ethernetPortStatusEL, "Parameters/Parameter[@name='Netmask']", netmask);
		if(isPrimary!=null && !"".equalsIgnoreCase(isPrimary)) {
			isPrimary = isPrimary.substring(0,1).toUpperCase() + isPrimary.substring(1);
		}
		MRXMLUtils.setValueXpath(ethernetPortStatusEL, "Parameters/Parameter[@name='Primary']", isPrimary);
		String childXpath = "//EthernetPortTableStatus";
		AssetAdminServiceUtils.addChild(deviceNID, childXpath, ethernetPortStatusEL, request, log, db, xmpp, resourceFactory, serviceAgentFactory);		
	}


	//get resources
	protected Document getResources(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String resourceTitle,Element requestDataElement) throws MRException,Exception {
		String query = FileUtils.readFileResource(GET_RESOURCES_XQUERY_PATH, request.getClass());
		query = query.replaceAll(_RESOURCETITLE_, resourceTitle);
		List<String> dirIDList = MRXMLUtils.getValuesXpath(requestDataElement, ".//Dir/@uuid");
		
		String whereXpath = "";
		String dirCheck = "false";
		if(dirIDList!=null && !dirIDList.isEmpty()) {
			dirCheck = "true";
			StringBuffer stringBuffer = new StringBuffer("[.//UUID[");
			String delim = "";
			for(String uuid: dirIDList) {
				stringBuffer.append(delim+ ".='"+uuid+"' ");
				delim = " or ";
			}
			stringBuffer.append("]]");
			whereXpath = stringBuffer.toString();
		}
		
		query = query.replaceAll(_UUIDXPATH_, whereXpath);
		query = query.replaceAll(_CHECKFORDIR_, dirCheck);
		
		String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);		
		return MRXMLUtils.stringToDocument(responseXML);
	}

	//update state and presence
	protected void updateStateAndPresence(Element element,String userJID,Element requestElement,MRLog log, boolean useRequestedPresence) throws Exception,MRException {
		//update state and contact
		MRXMLUtils.setValueXpath(element, ".//"+AMConst.OWNERUSERJID, userJID);
		MRXMLUtils.setValueXpath(element, ".//"+AMConst.PRESENCE, AMConst.AVAILABLE);
		String ready = MRXMLUtils.getAttributeValue(requestElement, SessionServiceConst.READY.toLowerCase(),SessionServiceConst.TRUE);
		if(useRequestedPresence && (ready.isEmpty() || ready.equals(SessionServiceConst.TRUE))){
			MRXMLUtils.setValueXpath(element, ".//"+AMConst.STATEFLAG, MRClientUtils.ASSET_STATE_READY);
		} else {
			MRXMLUtils.setValueXpath(element, ".//"+AMConst.STATEFLAG, MRClientUtils.ASSET_STATE_NOT_READY);
		}

		log.addInfo("BaseLginRequestHandler: Update State and Presence of User= " + userJID +" State:="+MRXMLUtils.getValueXpath(element, ".//"+AMConst.STATEFLAG,""), MRLog.INFO, MRLog.NOTIFY); 

	}

	//subscribe for user presence
	protected void subscribeForUserPresence(MRRequest request,XMPPI xmpp,MRLog log) throws MRException,Exception {
		String userJID = request.getUserJID();
		xmpp.subscribePresence(userJID);
		log.addInfo("BaseLginRequestHandler: Subscribe for User Presence : UserJID = " + userJID, MRLog.INFO, MRLog.NOTIFY);    	
	}


	//service login asset resource
	protected void validateAssetResource(Element assetResourceEl, MRRequest request,String deviceType,Element requestDataElement,String resourceTitle) throws Exception {

		if(request.isSuperUser()) {
			return;
		}

		if(assetResourceEl == null) {
			throw new MRException(MRExceptionCodes.ASSET_RESOURCE_NOTAVAILABLE, "Resource Not found");
		}

		String resourceType = MRXMLUtils.getValueXpath(assetResourceEl, ".//Type","");
		//TODO: Fix this code for auto discovery
		if(!deviceType.equals(resourceType)){
			throw new MRException(MRExceptionCodes.CAN_NOT_LOGIN_DEVICE_TYPE_MISMATCH,"Can not login device, type mismatch");
		}

		String state = MRXMLUtils.getValueXpath(assetResourceEl, ".//"+AMConst.STATEFLAG,"");
		if(state.equals(MRClientUtils.ASSET_STATE_READY)){
			throw new MRException(MRExceptionCodes.DEVICE_ALREADY_IN_USE, "Device Already in Use");
		}

		Document assetResourceDoc = MRXMLUtils.elementToDocument((Element)assetResourceEl.clone());

		//check asset private key
		String tAssetPrivateKeyData = MRXMLUtils.getValueXpath(assetResourceDoc, MRClientUtils.ASSET_PRIVATEKEYDATA_XPATH, "");
		tAssetPrivateKeyData = tAssetPrivateKeyData.trim();

		String assetPrivateKeyData = MRXMLUtils.getValueXpath(requestDataElement, ".//DevicePrivateKeyData", "");
		if ("".equals(tAssetPrivateKeyData)) {
			MRXMLUtils.setValueXpath(assetResourceDoc, MRClientUtils.ASSET_PRIVATEKEYDATA_XPATH, assetPrivateKeyData);
		} else if (!assetPrivateKeyData.equals(tAssetPrivateKeyData)) {        	
			System.out.println("Mismatch db='" + tAssetPrivateKeyData + "' passed='" + assetPrivateKeyData + "'");
			throw new MRException(MRExceptionCodes.ASSET_RESOURCE_ACCESSDENIED, resourceTitle +
					" check db=" + tAssetPrivateKeyData + " != key=" + assetPrivateKeyData);
		}   
	}
	protected void updateResourceContactJIDInMediaRoom(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			List<String> resourceNIDList, String contactJID) throws Exception {

		RoomDB roomDB = RoomDB.getRoomDB();

		StringBuffer mediaSourceXPathB = new StringBuffer();
		StringBuffer mediaDestXPathB = new StringBuffer();
		StringBuffer mediaRelayXPathB = new StringBuffer();
		String delim = "";		
		//for(Iterator<?> i = resourceNIDList.iterator();i.hasNext();) {
		for(String resourceNID: resourceNIDList){
			//String resourceNID = MRXMLUtils.getValue(((Element)i.next()));
			mediaDestXPathB.append(delim + "destPortNID='" + resourceNID + "'");
			mediaSourceXPathB.append(delim + "sourcePortNID='" + resourceNID + "'");
			mediaRelayXPathB.append(delim + "relayPortNID='" + resourceNID + "'");       
			delim = " or ";
		}
		String mediaSourceXPath = "//MediaSource/data[" + mediaSourceXPathB.toString() + "]";
		String mediaDestXPath = "//MediaDest/data[" + mediaDestXPathB.toString() + "]";
		String mediaRelayXPath = "//MediaRelay/data[" + mediaRelayXPathB.toString() + "]";

		// String xpath = "//MediaRoom["+ mediaSourceXPath + " " +  delim + mediaDestXPath +" "+ delim + mediaRelayXPath + " ]";    	
		// Document roomList = MediaRoomServiceUtils.getMediaRoomList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null, xpath, false);
		// List<?> myRooms = MRXMLUtils.getListXpath(roomList, "//" + AMConst.MEDIA_ROOM);

		// Update the contactJID in each of the rooms
		String mediaSourceNIDXpath = ""; 
		String mediaDestNIDXpath = "";
		String mediaRelayNIDXpath = "";  

		ArrayList<String> roomNIDList = RoomDB.getRoomDB().getRoomNIDListForAsset(contactJID);
		List <?> roomNIDs =  MRXMLUtils.getValuesXpath(request.getMessage().getDocument(), "//roomNID");
		for(String roomNID : roomNIDList) {
			try {
				StringBuffer streamXpathB  = new StringBuffer();
				delim = "";
				Document roomDoc = RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc();
				log.addInfo("BaseLoginRequestHandler:  Updating mediaroom for contactJID=" + contactJID + " roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);


				//update media source,destination and relay contact jid        	
				roomDB.updateMediaSourceContactJID(request, xmpp, log, db, roomNID, mediaSourceXPath, "sourceAgentJID", contactJID);
				roomDB.updateMediaDestContactJID(request, xmpp, log, db, roomNID, mediaDestXPath, "destAgentJID", contactJID);
				roomDB.updateMediaRelayContactJID(request, xmpp, log, db, roomNID, mediaRelayXPath, "relayAgentJID", contactJID);


				//generate stream xpath related to media source , media destination and media relay
				// String s =  RoomDB.getRoomDB().getRoom(roomNID, request, log, db).getRoomDoc().asXML();

				mediaSourceNIDXpath = getStreamXpath(MRXMLUtils.getListXpath(roomDoc, mediaSourceXPath),"mediaSourceNID");          
				mediaDestNIDXpath = getStreamXpath(MRXMLUtils.getListXpath(roomDoc, mediaDestXPath),"mediaDestNID");
				mediaRelayNIDXpath = getStreamXpath(MRXMLUtils.getListXpath(roomDoc, mediaRelayXPath),"mediaRelayNID");

				//update stream source agent jid  ,destination agent jid and relay agent jid based on media source and media destination
				if(!mediaSourceNIDXpath.isEmpty()){
					streamXpathB.append( delim + mediaSourceNIDXpath );
					delim = " or ";
					// mediaSourceNIDXpath = "//Stream/data["  + mediaSourceNIDXpath +" ]";
					mediaSourceNIDXpath = "//Stream/data["  + mediaSourceNIDXpath + " and (relayNID='')]";

					roomDB.updateStreamContactJID(request, xmpp, log, db, roomNID, mediaSourceNIDXpath, "sourceAgentJID", contactJID);

				}
				if(!mediaDestNIDXpath.isEmpty()){
					streamXpathB.append( delim + mediaDestNIDXpath );
					delim = " or ";
					mediaDestNIDXpath = "//Stream/data["  + mediaDestNIDXpath +" ]";
					roomDB.updateStreamContactJID(request, xmpp, log, db, roomNID, mediaDestNIDXpath, "destAgentJID", contactJID);          		
				}

				if(!mediaRelayNIDXpath.isEmpty()){
					streamXpathB.append( delim + mediaRelayNIDXpath );
					delim = " or ";
					mediaRelayNIDXpath = "//Stream/data["  + mediaRelayNIDXpath +" ]";
					roomDB.updateStreamContactJID(request, xmpp, log, db, roomNID, mediaRelayNIDXpath, "sourceAgentJID", contactJID);          		
				}   
				
				//List <?> roomNIDs =  MRXMLUtils.getValuesXpath(request.getMessage().getDocument(), "//roomNID");
				if(roomNIDs!= null){
				    if (!roomNIDs.contains(roomNID)) {
					log.addInfo("BaseLoginRequestHandler.updateResourceContactJIDInMediaRoom RoomNID="+roomNID+" present in the request skipping publishAssetPresenceInRoom");
					RoomDB.getRoomDB().publishAssetPresenceInRoom(xmpp, log, roomNID, contactJID);

				    }

				}
				else{
				    RoomDB.getRoomDB().publishAssetPresenceInRoom(xmpp, log, roomNID, contactJID);
				}

				// Get list of all the affected streams
				String streamXpath = streamXpathB.toString();
				if (streamXpath.isEmpty()) {
				    return; // This should not happen
				} else {
				    streamXpath = "//Stream/data[" + streamXpath + "]";
				}

				// Start all the streams in this room that are waiting for contactJIDS related to this asset
				List<?> streamList = MRXMLUtils.getListXpath(roomDoc, streamXpath);        
				for(Object o: streamList) {
					Element streamDataElement = ((Element)o);
					Element streamElement = streamDataElement.getParent();
					int state = MediaRoomServiceConst.STREAM_STOPPED;
					try { 
						state = Integer.parseInt(streamElement.element("Status").element("state").getText());
						log.addInfo("BaseLoginRequestHandler.updateResourceContactJIDInMediaRoom state =" + state, MRLog.OK, MRLog.NOTIFY);
					}
					catch(Exception e) { e.printStackTrace();}			

					String profileNID = streamDataElement.element("profileNID").getText();
					Element profileXML = streamDataElement.element("profileXML");

					if(state == MediaRoomServiceConst.WAITING_FOR_DEST) {				
						String destType = MRXMLUtils.getValueXpath(streamElement, "./data/destType", "");
						if(MediaRoomServiceConst.OUTPUT_RECORD.equals(destType) || MediaRoomServiceConst.OUTPUT_PVR.equals(destType)) {				    	
							String streamType = MRXMLUtils.getValueXpath(streamElement, "./data/streamType", "");
							String destNID = MRXMLUtils.getValueXpath(streamElement, "./data/destNID", "");
							MediaRoomServiceUtils.updateDestFileStreamProfile(destNID, streamType, profileXML, roomNID, "","",
									xmpp, log, db,request.getUserJID(),request,serviceAgentFactory,resourceFactory);
							Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(destNID, request, log, db);
							String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
							if(!mediaGroupID.isEmpty()) {
								Document mediaGroupFileDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db); 
								MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.BUSY);
								String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaGroupFileDoc.getRootElement(), MediaStoreServiceConst.NID, "");
								MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaGroupFileDoc, mediaStreamFileNID, request.getUserJID(), xmpp, log, db, serviceAgentFactory);
							}

						}
					} else if(state != MediaRoomServiceConst.WAITING_FOR_CONTACTJID) {
						continue;
					}

					String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");

					log.addInfo("BaseLoginRequestHandler start streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
					ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, "AddStreamActionRequest", 
							roomNID, streamNID,  profileNID,  profileXML ,MediaRoomServiceConst.START);
				}
			} catch (Exception e) {
				e.printStackTrace();
				log.addInfo("BaseLoginRequestHandler: Error while recovering device streams. roomNID:="+roomNID +" contactJID:="+contactJID);
			}
		}

	}
	
	public void cleanAllPVRFiles(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String mediaDirNID, String pvrDirNID) throws MRException, Exception {

	   List<?> mediaList = MediaManagementUtils.getListOfFilesInDir(request.getUserJID(), pvrDirNID, db, log, xmpp);
	   MRRequest assetRequest = MRRequest.createRequest(request.getAppServerJID(), request.getLogLevel(), request.getNID(), request.getServiceName(), request.getRequestName(), new MRMessage(request.getMessage().getDocument()), request.getResourceGroup(), request.getAppServerJID(), request.getUserAgentName(), request.getClientData());
	    if ((mediaList == null) || mediaList.isEmpty()) {
		return;
	    }
	    for (Object object : mediaList) {
		Document mediaStreamFileDoc = MRXMLUtils.elementToDocument((Element) object);
		String mediaStreamFileNID = MediaStoreServiceUtils.getMediaStreamFileNID(mediaStreamFileDoc);
		if (MediaManagementUtils.isFileInRoom(mediaStreamFileNID, assetRequest, xmpp, log, db)) {    		
		    continue;
		}
		
		  try {
		        log.addInfo("BaseLoginRequestHandler:cleanAllPVRFiles Begin fileNID=" + mediaStreamFileNID, MRLog.OK, MRLog.NOTIFY);		        		 
		        assetRequest.setSuperUser(true);		
		        new DeleteFileRequestHandler().deleteFile(mediaStreamFileDoc, assetRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, new DeleteFileRequestHandler().getClass().getName());		          
		        log.addInfo("BaseLoginRequestHandler:cleanAllPVRFiles End fileNID=" + mediaStreamFileNID, MRLog.OK, MRLog.NOTIFY);
	        }
	        catch(Exception e) {
	        	// e.printStackTrace();
		        log.addInfo("BaseLoginRequestHandler:cleanAllPVRFiles Failed fileNID=" + mediaStreamFileNID + " Exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
	        }
	    }
	}
	protected String getStreamXpath (List<?> mediaList,String nidXpath) {
		String delim = "";
		StringBuffer mediaNIDXpath = new StringBuffer();
		for(Object object : mediaList){
			Element mediaDataEL = (Element) object; 
			String mediaNID = MRXMLUtils.getAttributeValue(MRXMLUtils.getParentElement(mediaDataEL), "NID", "");
			mediaNIDXpath.append( delim + nidXpath + "='"+mediaNID +"'");
			delim = " or ";
		}
		return mediaNIDXpath.toString();
	}

	protected void handleStreamInfo(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
		ServiceAgentFactoryI serviceAgentFactory,Element streamInfoEle, boolean isLayout) throws Exception,MRException {
	    
	    String mediaSourceNID = this.getMediaSourceNID(streamInfoEle);
	    String mediaDestNID = this.getMediaDestNID(streamInfoEle);	  
	    String destNID = this.getDestNID(streamInfoEle);
	    String roomNID = this.getRoomNID(streamInfoEle);	    
	    String sourceAgentJID = MRXMLUtils.getValueXpath(streamInfoEle, ".//sourceAgentJID",  "");
	    RoomDB.getRoomDB().sendInvite(xmpp, log, roomNID, sourceAgentJID, "");
	    if(("").equals(destNID)|| isLayout){ //autostart session of source or dest		    
		String connectionId= getConnectionId(streamInfoEle);				
		
		log.addInfo("BaseLoginRequestHandler:handleSCRestart::roomNID= "+roomNID, MRLog.OK, MRLog.NOTIFY);
		SessionManager sessionManager = AppServerAgentUtils
			.getSessionManager(serviceAgentFactory);	
		String wallClock = "";
		wallClock = MRXMLUtils.getAttributeValueXpath(streamInfoEle, ".//Start", "WC", "");
		log.addInfo("BaseLoginRequestHandler:handleSCRestart::wallClock= "+wallClock, MRLog.OK, MRLog.NOTIFY);
		if("".equals(wallClock))
		 // use current system time
		    wallClock = String.valueOf(System.currentTimeMillis());
		String roomType = "Session";
		if("".equals(connectionId)) {
		    roomType = MediaRoomServiceConst.RELAY_CONNECTION;		    
		}

		// If relay then this has to be set to empty
		Semaphore lock = IPVSMonitor.getMonitor().getLock(roomNID, request.getNID());	
		try{
		    XMPPTransaction.startBlock("addMediaRoom");
		    log.addInfo("BaseLoginRequestHandler.handleSCRestart got lock for=" + request.getNID() + " roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
		    Document mediaRoomDoc = recoverMediaRoom(xmpp, sourceAgentJID,streamInfoEle,connectionId, mediaDestNID, mediaSourceNID, roomNID,wallClock, request, db, log, isLayout, serviceAgentFactory);				
		    log.addInfo("BaseLoginRequestHandler:handleSCRestart::mediaRoomDoc created: mediaRoomNID: "	+ roomNID, MRLog.OK, MRLog.NOTIFY);
		    XMPPTransaction.endBlock("addMediaRoom");	
		    // Update the AssetRoster

		    String destAgentJID = MRXMLUtils.getValueXpath(streamInfoEle, ".//destAgentJID",  "");			 
		    RoomDB.getRoomDB().sendInvite(xmpp, log, roomNID, destAgentJID, "");

		    if("Session".equals(roomType)) {
			boolean isPersistentPVR = false;
			String sourceNID  = MRXMLUtils.getValueXpath(streamInfoEle, ".//" + SessionServiceConst.SOURCE_NID, "");
			String sessionId = MRXMLUtils.getAttributeValueXpath(mediaRoomDoc, "//MediaRoom", "id", "");
			Semaphore lockSession = IPVSMonitor.getMonitor().getLock(sessionId, request.getNID());	
			try{
			    
			    isPersistentPVR = this.isPersistentPVRSource(sourceNID, log, request, db);
			    recoverSession(request, db, resourceFactory, serviceAgentFactory, xmpp, sessionManager, streamInfoEle, mediaSourceNID, mediaDestNID, roomNID, mediaRoomDoc,sessionId, connectionId, wallClock,destNID,isPersistentPVR, log);				
			    log.addInfo("BaseLoginRequestHandler:handleSCRestart::sessionDoc created: "	+ sessionId, MRLog.OK, MRLog.NOTIFY);
			}catch(Exception ex){
			    log.addInfo("BaseLoginRequestHandler:handleSCRestart::Exception in sessionDoc creation: "+ sessionId, MRLog.OK, MRLog.NOTIFY);			   
			}
			finally{
			    IPVSMonitor.getMonitor().releaseLock(sessionId, lockSession);
			}

			log.addInfo("BaseLoginRequestHandler.handleSCRestart released lock for=" + request.getNID() + " roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
		   }
		    //Update the streamURL in the port for autostart source 
		    if(!isLayout){
			String streamURL = MRXMLUtils.getValueXpath(streamInfoEle, ".//" + SessionServiceConst.URL, "");
			Element mediaStreamURLEl = MRXMLUtils.stringToElement("<MediaStreamURL>"+streamURL+"</MediaStreamURL>");
			String sourceNID  = MRXMLUtils.getValueXpath(streamInfoEle, ".//" + SessionServiceConst.SOURCE_NID, "");
			Document mediaSrcPortDoc = MRXMLDBUtils.readElement(request, log, db,sourceNID);
			MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, "", mediaSrcPortDoc.getRootElement().getName(), sourceNID, "//"+SessionServiceConst.MEDIA_STREAM_URL, null, mediaStreamURLEl);
		    }

		}
		catch(Exception e){ }
		finally{
		    IPVSMonitor.getMonitor().releaseLock(roomNID, lock);
		}
	    }
	    else{ //dynamic source : collect room NID and streams for GC
		
		log.addInfo("BaseLoginRequestHandler.handleSCRestart RoomGC called", MRLog.OK, MRLog.NOTIFY);
		String streamNID = this.getStreamNID(streamInfoEle);
		RoomGC roomGC = AppServerAgentUtils.getRoomGC(serviceAgentFactory);
		roomGC.addStream(roomNID, streamNID, this.getSourceAgentJID(streamInfoEle),MRXMLUtils.getElementXpath(streamInfoEle, ".//StreamDataDoc"), log, request, db);
	    }

	}
	
    protected void handleSCRestart(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	    ServiceAgentFactoryI serviceAgentFactory, Element requestDataElement, Element portResourceListElement) throws Exception, MRException {
	RoomGC roomGC = AppServerAgentUtils.getRoomGC(serviceAgentFactory);
	roomGC.addTime(RoomGC.addedDelay);//adds delay to roomgc scheduler everytime new device logs in
	
	XMPPTransaction.startBlock("BaseLoginRequestHandler.handleSCRestart");
	//Element StreamInfoListDoc = MRXMLUtils.getElementXpath(requestDataElement, ".//LayoutServiceInfo");
	List<?> streamInfoList = MRXMLUtils.getListXpath(requestDataElement, ".//LayoutServiceInfo/.//StreamInfo");
	if(streamInfoList!= null){
	    if (streamInfoList.size() > 0) {	
		for (Object streamInfo : streamInfoList) {
		    Element streamInfoEle = (Element) streamInfo;
		    this.handleStreamInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, streamInfoEle, true);
		}

	    }	
	}
	// if source
	//	/StreamInfoListDoc = MRXMLUtils.getElementXpath(requestDataElement, ".//MediaServiceInfo");
	streamInfoList = MRXMLUtils.getListXpath(requestDataElement, ".//MediaServiceInfo/.//StreamInfo");
	if(streamInfoList!= null){
	    if (streamInfoList.size() > 0) {	
		for (Object streamInfo : streamInfoList) {
		    Element streamInfoEle = (Element) streamInfo;
		    this.handleStreamInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, streamInfoEle, false);
		}

	    }
	}
	XMPPTransaction.endBlock("BaseLoginRequestHandler.handleSCRestart");
	
    }

    public Document recoverMediaRoom(XMPPI xmpp, String sourceAgentJID, Element streamDataDoc, String connectionId, String mediaDestNID, String mediaSourceNID, String roomNID,
	    String wallClockStr, MRRequest request, MRDB db, MRLog log, boolean isLayout, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
	MediaRoom room = null;
	if (RoomDB.getRoomDB().roomExists(roomNID)) {
	    room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
	} else {
	    String id = this.getSessionId(streamDataDoc);	    
	    createMediaroomDoc(streamDataDoc, id, connectionId, mediaDestNID, mediaSourceNID, roomNID, wallClockStr, request, db, log);
	    room = RoomDB.getRoomDB().getRoom(roomNID, request, log, db);
	}
	String streamTrackNID = MediaRoomServiceUtils.getTrackNID(roomNID, MediaRoomServiceConst.STREAMTRACK);
	// add mediasource
	if (!room.objectExists(request, db, log, mediaSourceNID)) {
	    XMPPTransaction.startBlock("BaseLoginRequestHandler.addMediaSourceElement");  

	    Element mediaSource = MRXMLUtils.getElementXpath(streamDataDoc, "./MediaSourceDataDoc/MediaSource");
	    if(mediaSource == null){ // dest		   		   		    
		room.addMediaSourceElement(request, log, db,createMediaSourceElement(xmpp, roomNID, sourceAgentJID, request, log, db,streamDataDoc, mediaSourceNID) , streamTrackNID);
	    }
	    else{		
		room.addMediaSourceElement(request, log, db, MRXMLUtils.getElementXpath(streamDataDoc, "./MediaSourceDataDoc/MediaSource"), streamTrackNID);
	    }

	    XMPPTransaction.endBlock("BaseLoginRequestHandler.addMediaSourceElement");
	}
	// add mediastreamlist
	String mediaStreamNID = this.getStreamNID(streamDataDoc);
	if (!room.objectExists(request, db, log, mediaStreamNID)) {
	    XMPPTransaction.startBlock("BaseLoginRequestHandler.addStreamElement");
	    room.addStreamElement(request, log, db, createMediaStreamElement(streamDataDoc, connectionId, mediaStreamNID, log), mediaSourceNID);
	    XMPPTransaction.endBlock("BaseLoginRequestHandler.addStreamElement");
	}
	// add media dest element 
	if(isLayout){ //do not add dest element for autostart room
	    if (!room.objectExists(request, db, log, mediaDestNID)) {
		XMPPTransaction.startBlock("BaseLoginRequestHandler.addDestElement");
		room.addMediaDestElement(request, log, db, createMediaDestElement(request, db,streamDataDoc, roomNID, mediaDestNID, log), streamTrackNID);
		XMPPTransaction.endBlock("BaseLoginRequestHandler.addDestElement");
	    }
	}
	RoomGC roomGC = AppServerAgentUtils.getRoomGC(serviceAgentFactory);
	roomGC.deleteStream(roomNID, this.getStreamNID(streamDataDoc), log);
	return room.getRoomDoc();
    }

    public void recoverSession(MRRequest request, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, XMPPI xmpp,
	    SessionManager sessionManager, Element streamDataDoc, String mediaSourceNID, String mediaDestNID, String roomNID, Document mediaRoomDoc,
	    String sessionID, String connectionId, String wallClock, String dstResourceNID,boolean isPersistentPVR, MRLog log) throws Exception {	
	sessionManager = AppServerAgentUtils.getSessionManager(serviceAgentFactory);	
	Document sessionDoc = null;
	boolean isNewSession = false;
	if (sessionManager.sessionExists(roomNID.trim())) {	 
	    
	    sessionDoc = sessionManager.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);	   
	} 
	else {

	    sessionDoc = this.createSessionDoc(request, streamDataDoc, mediaSourceNID, mediaDestNID, roomNID, sessionID, connectionId, wallClock, log);
	    isNewSession = true;
	}
	Document mediaSourceDoc = MRXMLUtils.getDocumentXpath(mediaRoomDoc, "//MediaSource[@NID='"+mediaSourceNID+"']");
	String srcEntityId = MRXMLUtils.getAttributeValue(mediaSourceDoc.getRootElement(), MediaRoomServiceConst.ID);
	String dstEntityId = "";
	String destType = MRXMLUtils.getValueXpath(streamDataDoc, ".//destType", "");
	String pvrEnabled = this.getPVREnableValue(request, log, db, streamDataDoc, destType, dstResourceNID);//MRXMLUtils.getValueXpath(streamDataDoc, ".//pvrEnabled", "false");
	
	String trimLength = MRXMLUtils.getValueXpath(streamDataDoc, ".//trimLength", "");
	if(SessionServiceConst.TRUE.equals(pvrEnabled) && "".equals(trimLength)){
	    trimLength = MRXMLUtils.getValueXpath(streamDataDoc, ".//lookbackSourceFilePlayLength", "");
	}
	String streamType = MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.STREAM_TYPE, "");
	
	Element entityList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.ENTITYLIST);
	String srcresourceNID = MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.SOURCE_NID, "");
	
	Element srcEntity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[*/@resourceNID='" + srcresourceNID + "']");
	if (srcEntity == null) {
	    String resourceTitle = MRXMLUtils.getAttributeValue(mediaSourceDoc.getRootElement(), MediaRoomServiceConst.TITLE,SessionServiceConst.TITLE_NOT_AVAILABLE);	
	    String presence = MRXMLUtils.getAttributeValue(mediaSourceDoc.getRootElement(), MediaRoomServiceConst.PRESENCE,MediaRoomServiceConst.UNAVAILABLE);
	    if(SessionServiceConst.LIVE.equals(getSessionType(streamDataDoc))){
		srcEntity = this.addEntity(sessionID, SessionServiceConst.SRC_PORT, MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.SOURCE_NID, ""),
			srcEntityId, resourceTitle,presence, streamType, pvrEnabled, trimLength, streamDataDoc, connectionId,isPersistentPVR, request,db,resourceFactory, serviceAgentFactory, xmpp,log);		
	    }
	    else if(SessionServiceConst.PLAYBACK.equals(getSessionType(streamDataDoc)))
	    {
		//create playback srcentity
		log.addInfo("BaseLoginRequestHandler.recoverSession PLAYBACK ", MRLog.OK, MRLog.NOTIFY);
		srcEntity = this.createDstFileEntity(srcEntityId, presence, resourceTitle,resourceTitle,MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.SOURCE_NID, ""),
			SessionServiceConst.PLYFILE,streamType);		
		Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.SOURCE_NID,""), request, log, db);		

		String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
		//if the file type is not group then find corresponding group file
		if(mediaType.equals(MediaRoomServiceConst.MEDIACLIP)) {
		    String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
		    Document mediaGroupFileDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);
		    if(mediaGroupFileDoc != null){			
			String grpFileNID = MRXMLUtils.getAttributeValue(mediaGroupFileDoc.getRootElement(), SessionServiceConst.NID);
			Element grpFile = UpdateSessionUtils.getFile(request, xmpp, log, db, resourceFactory, serviceAgentFactory, request.getMessage().getDocument(), grpFileNID);			
			Element grpFileList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.GRPFILELIST);
			String id = "GrpFile_"+mediaGroupID;
			String fileTitle = MRXMLUtils.getAttributeValue(grpFile, MediaRoomServiceConst.TITLE);
			if(MRXMLUtils.getElementXpath(sessionDoc, "//GrpFile[@id='" + id + "']") == null) {
			    grpFileList.add(this.createGrpFileElement(request, log, db, grpFile, id, fileTitle));
			}			

		    }
		}    	

	    }

	    if (isNewSession)
		entityList.add(srcEntity);
	    else{

		sessionManager.addEntity(roomNID, srcEntity);
		sessionDoc = sessionManager.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
	    }
	}
	Element dstEntity = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[*/@resourceNID='" + dstResourceNID + "']");	
	if(!"".equals(dstResourceNID)){ //if autostart session no dest is present	    
	    Document mediaDestDoc = MRXMLUtils.getDocumentXpath(mediaRoomDoc, "//MediaDest[@NID='"+mediaDestNID+"']");
	    dstEntityId =MRXMLUtils.getAttributeValue(mediaDestDoc.getRootElement(), MediaRoomServiceConst.ID);
	    String presence = MRXMLUtils.getAttributeValue(mediaDestDoc.getRootElement(), MediaRoomServiceConst.PRESENCE,MediaRoomServiceConst.UNAVAILABLE);
	    String resourceTitle = MRXMLUtils.getAttributeValue(mediaDestDoc.getRootElement(), MediaRoomServiceConst.TITLE,SessionServiceConst.TITLE_NOT_AVAILABLE);
	    if (dstEntity == null ) {
		if (MediaRoomServiceConst.OUTPUT_DECODER.equals(destType) || MediaRoomServiceConst.OUTPUT_STREAM.equals(destType)) {
		    dstEntity = this.addEntity(sessionID, SessionServiceConst.DST_PORT, MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.DEST_NID, ""),
			    dstEntityId, resourceTitle,presence, streamType, "", "",streamDataDoc, connectionId,isPersistentPVR, request,db,resourceFactory, serviceAgentFactory, xmpp,log);
		} else if (MediaRoomServiceConst.OUTPUT_RECORD.equals(destType)) {
		    dstEntity = this.createDstFileEntity(dstEntityId, presence, resourceTitle,resourceTitle,
			    MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.DEST_NID, ""), SessionServiceConst.RECFILE, streamType);
		} else if (MediaRoomServiceConst.OUTPUT_PVR.equals(destType)) {	

		    //Uncomment following if exact pvr entity name is to be recovered
		    /*Document mediaSrcPortDoc = MRXMLDBUtils.readElement(request, log, db,MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.DEST_NID, ""));
		    resourceTitle = MRXMLUtils.getAttributeValueXpath(mediaSrcPortDoc, "//MediaGroupID","ClipID",SessionServiceConst.TITLE_NOT_AVAILABLE);*/
		    //update the PVR element values in srcport entity if PVR is present	
		    if(!isNewSession && !(sessionID.contains("PVRSession"))){
			trimLength = MRXMLUtils.getValueXpath(streamDataDoc, ".//trimLength", "");		    
			String ffTracks = MRXMLUtils.getValueXpath(streamDataDoc, ".//"+SessionServiceConst.FF_TRACKS_ELEMENT, "");
			String startTC = "";
			startTC = MRXMLUtils.getAttributeValueXpath(streamDataDoc, ".//Start", "TC", "");
			if("".equals(startTC))// use current system time
			    startTC = String.valueOf(System.currentTimeMillis());
			sessionManager.updatePVRInfo(roomNID, "",MRXMLUtils.getAttributeValue(srcEntity, "id"), startTC, trimLength, connectionId,ffTracks,"");
		    }
		    dstEntity = this.createDstFileEntity(dstEntityId, presence, resourceTitle,resourceTitle+"_PVR",MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.DEST_NID, ""),
			    SessionServiceConst.PVRFILE, streamType);
		}
		if (isNewSession)
		    entityList.add(dstEntity);
		else{		   
		    sessionManager.addEntity(roomNID, dstEntity);
		    sessionDoc = sessionManager.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
		}
	    }
	}	
	
	Element connection = UpdateSessionUtils.createConnection(connectionId, dstEntityId, srcEntityId, "", sessionDoc,  getCanvasId(streamDataDoc),getWindowId(streamDataDoc), "");	

	int state = Integer.parseInt(MRXMLUtils.getValueXpath(streamDataDoc, ".//StreamStatus/state", ""));
	
	String stateStr = MediaRoomServiceUtils.getStateStr(log, state);
	MRXMLUtils.addAttribute(connection, "state", stateStr);
	//log.addInfo("BaseLoginRequestHandler.recoverSession State :"+stateStr+":: StreamDataDoc = "+streamDataDoc.asXML(), MRLog.OK, MRLog.NOTIFY);
	if (isNewSession) {
	    Element connectionList = MRXMLUtils.getElementXpath(sessionDoc, "//" + SessionServiceConst.CONNECTIONLIST);
	    connectionList.add(connection);
	    XMPPTransaction.startBlock("addSession");	    
	    //update session Type 
	    List<?> srcPortList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY + "[@type = '" + SessionServiceConst.SRCPORT + "']");		
	    List<?> plyFileList = MRXMLUtils.getListXpath(sessionDoc, "//" + SessionServiceConst.ENTITY + "[@type = '" + SessionServiceConst.PLYFILE + "']");
	    String sessionType = SessionServiceConst.SESSION_SYSTEM;
	    if(!srcPortList.isEmpty() && dstResourceNID.isEmpty()) 
		sessionType = SessionServiceConst.TYPE_SYSTEM_READ_ONLY;
	    else if(!srcPortList.isEmpty() && !dstResourceNID.isEmpty() && !(sessionID.contains("PVRSession") && (MediaRoomServiceConst.OUTPUT_PVR.equals(destType)))) 
		sessionType = SessionServiceConst.LIVE;
	    else if(!plyFileList.isEmpty())
		sessionType = SessionServiceConst.PLAYBACK;
	    //TODO: Add persistent pvr case. if sessionId is persistent pvr session id and destination is pvr then set the type as SYSTEM.

	    MRXMLUtils.setAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.TYPE_ATTRIBUTE, sessionType);	    
	    sessionManager.addSession(request, log, db, roomNID, sessionDoc, mediaRoomDoc);
	    XMPPTransaction.endBlock("addSession");
	} else {			
	    sessionManager.addConnection(roomNID, connection);	    
	}	

    }

	public Document createSessionDoc(MRRequest request, Element streamDataDoc,
			String mediaSourceNID, String mediaDestNID, String roomNID,
			String sessionID, String connectionId, String wallClock, MRLog log)
					throws Exception {
		XMPPTransaction.startBlock("BaseLoginRequestHandler.createSessionDoc");

		Document sessionTemplateDoc = Session.getSessionTemplate(log);
		sessionTemplateDoc.getRootElement().addAttribute(
				SessionServiceConst.NAME, sessionID);
		sessionTemplateDoc.getRootElement().addAttribute(
				SessionServiceConst.GC, MRConst.TRUE);
		sessionTemplateDoc.getRootElement().addAttribute(
				SessionServiceConst.ID, sessionID); // timelineId=""

		sessionTemplateDoc.getRootElement().addAttribute(
				SessionServiceConst.TYPE_ATTRIBUTE, this.getSessionType(streamDataDoc));

		sessionTemplateDoc.getRootElement().addAttribute("timelineId",	MediaRoomServiceUtils.getTrackNID(roomNID, "StreamTrack"));
		sessionTemplateDoc.getRootElement().addAttribute("recordTimelineId",MediaRoomServiceUtils.getTrackNID(roomNID, "RecordTrack"));
		MRXMLUtils.getElementXpath(sessionTemplateDoc, "//Properties");
		MRXMLUtils.setAttributeValueXPath(sessionTemplateDoc, "//Properties","sessionConfigNID", sessionID);
		MRXMLUtils.setAttributeValueXPath(sessionTemplateDoc, "//Properties","dateCreated", wallClock);
		MRXMLUtils.setAttributeValueXPath(sessionTemplateDoc, "//Properties","ownerUserJID", request.getAppServerJID());
		MRXMLUtils.setAttributeValueXPath(sessionTemplateDoc, "//Session","state", SessionServiceConst.STARTED);
		XMPPTransaction.endBlock("BaseLoginRequestHandler.createSessionDoc");
		return sessionTemplateDoc;
	}

	public void createMediaroomDoc(Element streamDataDoc, String id,
			String connectionId, String mediaDestNID, String mediaSourceNID,
			String roomNID, String wallClockStr, MRRequest request, MRDB db,
			MRLog log) throws Exception {


		// generated streamTrackNID as original not known
		XMPPTransaction.startBlock("BaseLoginRequestHandler.createMediaroomDoc");
		Document mediaRoomDoc = RoomDB.getMediaRoomInstanceXML(log);
		MRXMLUtils.addAttribute(mediaRoomDoc.getRootElement(), "NID", roomNID);

		MRXMLUtils.addAttribute(mediaRoomDoc.getRootElement(),
				MediaRoomServiceConst.ID, id);
		MRXMLUtils.addAttribute(mediaRoomDoc.getRootElement(), "serviceVer",MRXMLUtils.getServiceVersion(log));
		Element groups = MRXMLUtils.getElementXpath(streamDataDoc, "//Groups");
		if (groups != null) {
			MRXMLUtils.removeChild(MRXMLUtils.getElementXpath(mediaRoomDoc,	"//MediaRoom/data/Info"), "Groups");
			MRXMLUtils.getElementXpath(mediaRoomDoc, "//MediaRoom/data/Info").add((Element) groups.clone());
		}
		MRXMLUtils.getElementXpath(mediaRoomDoc, "//MediaRoom/data/Info/Title")	.setText(roomNID + "_title");
		MRXMLUtils.getElementXpath(mediaRoomDoc,"//MediaRoom/data/Info/Description").setText(MediaRoomServiceConst.RECOVERED_ROOM);
		MRXMLUtils.getElementXpath(mediaRoomDoc,"//MediaRoom/data/roomOwnerJID").setText(MRXMLUtils.getAttributeValueXpath(streamDataDoc, ".//"+MediaRoomServiceConst.CONTEXT, MediaRoomServiceConst.ROOMOWNERJID, ""));		
		MRXMLUtils.getElementXpath(mediaRoomDoc, "//MediaRoom/data/roomType").setText(this.getRoomType(streamDataDoc, mediaSourceNID));

		Long wallClock = Utils.getLongValue(wallClockStr,0);
		Document trackDoc = RoomDB.getTrackInstanceXML(log,	MediaRoomServiceConst.STREAMTRACK);
		RoomDB roomDB = RoomDB.getRoomDB();
		roomDB.addTrackElement(mediaRoomDoc, trackDoc,MediaRoomServiceConst.STREAMTRACK, roomNID, "", wallClock, log);
		roomDB.addTrackElement(mediaRoomDoc, trackDoc,MediaRoomServiceConst.RECORDTRACK, roomNID, "", wallClock, log);
		roomDB.addTrackElement(mediaRoomDoc, trackDoc,MediaRoomServiceConst.PVRTRACK, roomNID, "", wallClock, log);
		MRXMLUtils.getElementXpath(mediaRoomDoc,"//MediaRoom/data/creationWallClock").setText(String.valueOf(wallClock));

		XMPPTransaction.startBlock("BaseLoginRequestHandler.addRoom");
		RoomDB.getRoomDB().addRoom(roomNID, request, log, db, mediaRoomDoc);
		XMPPTransaction.endBlock("BaseLoginRequestHandler.addRoom");
		XMPPTransaction.endBlock("BaseLoginRequestHandler.createMediaroomDoc");
	}

	private String getMediaSourceNID(Element streamDataDoc) throws Exception {
		return MRXMLUtils.getValueXpath(streamDataDoc, ".//mediaSourceNID", "");
	}

	private String getMediaDestNID(Element streamDataDoc) throws Exception {
		return MRXMLUtils.getValueXpath(streamDataDoc, ".//mediaDestNID", "");
	}

	private String getStreamNID(Element streamDataDoc) throws Exception {
		return MRXMLUtils.getAttributeValue(streamDataDoc, "streamNID", "");
	}

	private String getRoomNID(Element streamDataDoc) throws Exception {
		return MRXMLUtils.getValueXpath(streamDataDoc, ".//roomNID", "");
	}

	private String getSessionId(Element streamDataDoc) throws Exception {
	    return MRXMLUtils.getAttributeValueXpath(streamDataDoc, ".//"+MediaRoomServiceConst.CONTEXT, MediaRoomServiceConst.ROOMID, "");
	}

	private String getConnectionId(Element streamDataDoc) throws Exception {
		return MRXMLUtils.getAttributeValueXpath(streamDataDoc, ".//"+MediaRoomServiceConst.CONTEXT, MediaRoomServiceConst.ID, "");// MRXMLUtils.getValueXpath(streamDataDoc, ".//windowId", "");
	}
	private String getWindowId(Element streamDataDoc) throws Exception {
		return MRXMLUtils.getValueXpath(streamDataDoc, ".//windowId", "");// MRXMLUtils.getValueXpath(streamDataDoc, ".//windowId", "");
	}
	private String getCanvasId(Element streamDataDoc) throws Exception {
		return MRXMLUtils.getValueXpath(streamDataDoc, ".//canvasId", "");// MRXMLUtils.getValueXpath(streamDataDoc, ".//windowId", "");
	}
	private Element addEntity(String sessionID, String type, String resourceNID,String mediaSourceOrDestNID, String resourceTitle,String presence, String streamType, String pvrEnabled, String trimLength, Element streamDataDoc, String connectionId,boolean isPersistentPVR, MRRequest request, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, XMPPI xmpp,MRLog log)
		throws Exception {
	    XMPPTransaction.startBlock("BaseLoginRequestHandler.addEntity");
	    Element entity = MRXMLUtils.stringToElement("<"
		    + SessionServiceConst.ENTITY + "/>");
	    // In the mode that is in use the GC is always false, however this can
	    // change
	    entity.addAttribute(SessionServiceConst.GC, MRConst.FALSE);
	    entity.addAttribute(SessionServiceConst.ID, mediaSourceOrDestNID);
	    entity.addAttribute(SessionServiceConst.TYPE_ATTRIBUTE, type);
	    entity.addAttribute(SessionServiceConst.NAME, resourceTitle);
	    entity.addAttribute(SessionServiceConst.STREAM_TYPE, streamType);
	    entity.addAttribute(SessionServiceConst.PRESENCE, presence);
	    if (SessionServiceConst.TRUE.equals(pvrEnabled)) {
		Element pvrEle = MRXMLUtils.stringToElement("<"
			+ SessionServiceConst.PVR + "/>");
		pvrEle.addAttribute(SessionServiceConst.ENABLE, pvrEnabled);
		pvrEle.addAttribute(SessionServiceConst.TRIMLENGTH, trimLength);
		pvrEle.addAttribute(SessionServiceConst.DIRID, "");
		pvrEle.addAttribute(SessionServiceConst.ID, mediaSourceOrDestNID+"PVRConnection");
		
		String startTC ="";
		String ffTracks = "";
		if(isPersistentPVR && !sessionID.contains("PVRSession"))
		{
		    Element persistentPVREL = this.getPersistentPVREl(request, db, resourceFactory, serviceAgentFactory, xmpp, log, resourceNID);
		    if(persistentPVREL!= null){
			startTC =  MRXMLUtils.getAttributeValue(persistentPVREL, SessionServiceConst.START_TC_ATTRIBUTE, "");
			ffTracks =  MRXMLUtils.getAttributeValue(persistentPVREL, SessionServiceConst.FF_TRACKS, "");
			connectionId = MRXMLUtils.getAttributeValue(persistentPVREL, SessionServiceConst.CONN_ID, "");
		    }
		}
		
		if("".equals(startTC)){		  
		    startTC = MRXMLUtils.getAttributeValueXpath(streamDataDoc, ".//Start", "TC", "");
		    if("".equals(startTC))
			startTC = MRXMLUtils.getValueXpath(streamDataDoc, ".//lookbackSourceFileStartTC", "");
		    if("".equals(startTC))
			startTC = String.valueOf(System.currentTimeMillis());
		}		
		
		if(sessionID.contains("PVRSession")){
		    ffTracks = MRXMLUtils.getValueXpath(streamDataDoc, ".//"+SessionServiceConst.FF_TRACKS_ELEMENT,"");
		    this.updateStartTCOfPVRSessions(startTC, sessionID,connectionId, trimLength, ffTracks, resourceNID, request, db, resourceFactory, serviceAgentFactory, xmpp, log);
		}
		pvrEle.addAttribute(SessionServiceConst.START_TC_ATTRIBUTE, startTC);
		pvrEle.addAttribute(SessionServiceConst.CONN_ID, connectionId);
		pvrEle.addAttribute(SessionServiceConst.FF_TRACKS, ffTracks);
		entity.add(pvrEle);
	    }
	    else if(!sessionID.contains("AutoStartSession") && !SessionServiceConst.DST_PORT.equals(type)){ // do not add pvr element for autostart and dest port entity
		Element pvrEle = MRXMLUtils.stringToElement("<"
			+ SessionServiceConst.PVR + "/>");
		pvrEle.addAttribute(SessionServiceConst.ENABLE, SessionServiceConst.FALSE);
		pvrEle.addAttribute(SessionServiceConst.TRIMLENGTH, "");
		pvrEle.addAttribute(SessionServiceConst.DIRID, "");
		entity.add(pvrEle);
	    }

	    String resourceElementName = SessionServiceConst.PORT;
	    Element resourceElement = MRXMLUtils.stringToElement("<"+ resourceElementName + "/>");
	    entity.add(resourceElement);
	    resourceElement.addAttribute(SessionServiceConst.RESOURCENID,resourceNID);
	    resourceElement.addAttribute(SessionServiceConst.RESOURCETITLE,	resourceTitle);
	    XMPPTransaction.endBlock("BaseLoginRequestHandler.addEntity");
	    return entity;
	}

	private String getRoomType(Element streamDataDoc, String mediaSourceNID)
			throws Exception {
		String userMediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc,
				".//userRoomMediaSourceNID", "");
		if (userMediaSourceNID.equals(mediaSourceNID)
				|| userMediaSourceNID.isEmpty()) {
			return SessionServiceConst.SESSION;
		} else
			return MediaRoomServiceConst.RELAY_ROOM;
	}
	
	private String getDestPortNID(Element streamDataDoc) throws Exception{
	    return MRXMLUtils.getValueXpath(streamDataDoc, ".//destPortNID", "");
	}
	
	private String getDestNID(Element streamDataDoc) throws Exception{
	    return MRXMLUtils.getValueXpath(streamDataDoc, ".//"+SessionServiceConst.DEST_NID, "");
	}
	
	private String getSessionType(Element streamDataDoc)throws Exception {
		String sourceType = MRXMLUtils.getValueXpath(streamDataDoc,".//sourceType", "");
		if(sourceType.equals(MediaRoomServiceConst.INPUT_ENCODER))
		   return SessionServiceConst.LIVE;		
		else if(sourceType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE))
			return SessionServiceConst.PLAYBACK;
		return "";
	}
	private Element createMediaSourceElement(XMPPI xmpp, String roomNID,String sourceAgentJID, MRRequest request, MRLog log, MRDB db,Element streamDataDoc,
			String mediaSourceNID) throws Exception {
	    String state = MediaRoomServiceConst.UNAVAILABLE;
	      if (RoomDB.getRoomDB().isReady(xmpp, roomNID, sourceAgentJID, request.getUserJID(),  request.getAppServerJID()))
	    	  state = MediaRoomServiceConst.AVAILABLE; 
		Element mediaSourceEle = MRXMLUtils
				.newElement(MediaRoomServiceConst.MEDIASOURCE);
		MRXMLUtils.addAttribute(mediaSourceEle, MediaRoomServiceConst.NID, mediaSourceNID);
		MRXMLUtils.addAttribute(mediaSourceEle, MediaRoomServiceConst.ID,
			Utils.getUUID()); 
		MRXMLUtils.addAttribute(mediaSourceEle, MediaRoomServiceConst.PRESENCE, state);
		MRXMLUtils.addAttribute(mediaSourceEle,MediaRoomServiceConst.TITLE,
			getPortTitle(request, log, db,MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.SOURCE_NID, "")));

		Element MediaSourceDataEle = MRXMLUtils.getElementXpath(streamDataDoc,".//MediaSourceDataDoc");
		mediaSourceEle.add((Element) (MRXMLUtils.getChild(MediaSourceDataEle,"data")).clone());
		return mediaSourceEle;
	}

	private Element createMediaStreamElement(Element streamDataDoc,	String connectionId, String mediaStreamNID, MRLog log) throws Exception {	    	
		Element streamEle = MRXMLUtils.newElement("Stream");
		MRXMLUtils.addAttribute(streamEle, "NID", mediaStreamNID);
		MRXMLUtils.addAttribute(streamEle, "id", connectionId);
		Element StreamDataEle = MRXMLUtils.getElementXpath(streamDataDoc,".//StreamDataDoc");		
		streamEle.add((Element) (MRXMLUtils.getElementXpath(StreamDataEle, ".//data")).clone());
		
		Element statusEle;		
		statusEle = (Element) (MRXMLUtils.getChild(StreamDataEle,"StreamStatus")).clone();
		statusEle.setName("Status");
		streamEle.add(statusEle);
		return streamEle;
	}

	private Element createMediaDestElement(MRRequest request, MRDB db,Element streamDataDoc,
			String roomNID, String mediaDestNID, MRLog log) throws Exception {
		String desttype = MRXMLUtils.getValueXpath(streamDataDoc,".//destType", "");

		String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc,".//destAgentJID", "");
		String destFileUUID = MRXMLUtils.getValueXpath(streamDataDoc,".//destFileUUID", "");
		String destFileParentDirNID = MRXMLUtils.getValueXpath(streamDataDoc,".//destFileParentDirNID", "");
		String destPortNID = this.getDestPortNID(streamDataDoc);
		String destPortElementID = MRXMLUtils.getValueXpath(streamDataDoc,".//destPortElementID", "");// not equal to mediadest
		// destAssetType is unknown
		// presence is unknown
		String trickPlay = MRXMLUtils.getValueXpath(streamDataDoc,".//trickPlay", "");
		if (RoomDB.gMediaDestElementTemplate == null) {
			RoomDB.gMediaDestElementTemplate = RoomDB.getMediaDestInstanceXML(log);
		}
		Document doc = (Document) RoomDB.gMediaDestElementTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(doc, "//MediaDest", "NID",mediaDestNID);
		MRXMLUtils.setAttributeValueXPath(doc, "//MediaDest",MediaRoomServiceConst.PRESENCE, "");
		MRXMLUtils.setAttributeValueXPath(doc, "//MediaDest",MediaRoomServiceConst.TITLE,getPortTitle(request, log, db,MRXMLUtils.getValueXpath(streamDataDoc, ".//" + SessionServiceConst.DEST_NID, "")));
		MRXMLUtils.setAttributeValueXPath(doc, "//MediaDest",MediaRoomServiceConst.ID, Utils.getUUID());
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/roomNID", roomNID);
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destNID", this.getDestNID(streamDataDoc));
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destType", desttype);
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destPortNID",destPortNID);
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destAgentJID",destAgentJID);
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destIP", "");
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destUDPStartPort", "");
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/callbackPort", "");
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destFileUUID",destFileUUID);
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destFileParentDirNID",destFileParentDirNID);
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destFileRelayNID", "");
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destPortElementID",	destPortElementID);
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destAssetType", "");
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/destStreamURL", "");
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/trimLength", "");
		MRXMLUtils.setValueXpath(doc, "//MediaDest/data/trickPlay", trickPlay);

		Element mediaDestEle = MRXMLUtils.getElementXpath(doc, "//MediaDest");
		return mediaDestEle;
	}
	private Element createDstFileEntity(String id, String presence, String resourceTitle, String name, String resourceNID, String type, String streamType) throws Exception{
		Element entity = MRXMLUtils.stringToElement("<" + SessionServiceConst.ENTITY + "/>");
		MRXMLUtils.addAttribute(entity, SessionServiceConst.GC, MRConst.TRUE);
		MRXMLUtils.addAttribute(entity, SessionServiceConst.ID, id);
		MRXMLUtils.addAttribute(entity, SessionServiceConst.TYPE_ATTRIBUTE, type);
		MRXMLUtils.addAttribute(entity, SessionServiceConst.NAME, name);
		
		entity.addAttribute(SessionServiceConst.STREAM_TYPE, streamType);
		entity.addAttribute(SessionServiceConst.PRESENCE, presence);

		Element fileElement = MRXMLUtils.stringToElement("<" + SessionServiceConst.FILE + "/>");
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.RESOURCETITLE, resourceTitle);
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.GRPFILEID,"");
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.KEY, "");		
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.ACTION, "");
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.KEY_METADATA_TAG_NAME, "");
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.DIRID, "");
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.CLIPID, "");
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.RESOURCENID, resourceNID);

		MRXMLUtils.addElement(entity,fileElement);	

		return entity;
	}
	public String getPortTitle(MRRequest request, MRLog log, MRDB db, String NID) throws Exception{
	    
	    Document mediaSrcPortDoc = MRXMLDBUtils.readElement(request, log, db,NID);	    
	    return MRXMLUtils.getValueXpath(mediaSrcPortDoc,"//Title", "");
	}
	public String getSourceAgentJID(Element streamDataDoc) throws Exception{
	    //sourceAgentJID
	    return MRXMLUtils.getValueXpath(streamDataDoc,".//sourceAgentJID", "");
	}
	public Element createGrpFileElement(MRRequest request, MRLog log, MRDB db, Element grpFileElement, String grpId, String fileTitle) throws Exception  {
	    String grpFileNID = grpFileElement.attributeValue("NID");
	    String grpFileTitle = grpFileElement.attributeValue("title");
	    Element sessionGrpFileElement = MRXMLUtils.stringToElement("<" + SessionServiceConst.GRPFILE + "/>");
	    sessionGrpFileElement.addAttribute(SessionServiceConst.RESOURCENID, grpFileNID);
	    if((grpFileTitle == null) || grpFileTitle.isEmpty()) {
		grpFileTitle = fileTitle;
	    }
	    sessionGrpFileElement.addAttribute(SessionServiceConst.RESOURCETITLE, grpFileTitle);
	    sessionGrpFileElement.addAttribute(SessionServiceConst.GC,MRConst.FALSE);
	    sessionGrpFileElement.addAttribute(SessionServiceConst.ID,grpId);
	    sessionGrpFileElement.addAttribute(SessionServiceConst.TYPE_ATTRIBUTE, SessionServiceConst.SYNCPLY);
	    return sessionGrpFileElement;	
	}
	
	private String getPVREnableValue(MRRequest request, MRLog log, MRDB db, Element streamDataDoc, String destType, String destNID) throws Exception{
	    String pvrEnabled = MRXMLUtils.getValueXpath(streamDataDoc, ".//pvrEnabled", SessionServiceConst.FALSE);	  

	    if( (MediaRoomServiceConst.OUTPUT_PVR.equals(destType)) || !"".equals(MRXMLUtils.getValueXpath(streamDataDoc, ".//lookbackSourceNID","")))
		pvrEnabled = SessionServiceConst.TRUE;

	    log.addInfo("BaseLoginRequestHandler.getPVREnableValue: "+pvrEnabled);
	    return pvrEnabled;
	}
	
	private boolean isPersistentPVRSource(String sourceNID, MRLog log, MRRequest request, MRDB db) throws Exception{
	    Document mediaSrcPortDoc = MRXMLDBUtils.readElement(request, log, db,sourceNID);	   
	    return (SessionServiceConst.TRUE.equals(MRXMLUtils.getAttributeValueXpath(mediaSrcPortDoc, "//PVRInfo", "enable", SessionServiceConst.FALSE)))? true : false;
	}

	private Element getPersistentPVREl(MRRequest request, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, XMPPI xmpp,MRLog log, String sourceNID) throws Exception{
	    
	    Element pvrEL = null;
	    Document mediaSrcPortDoc = MRXMLDBUtils.readElement(request, log, db,sourceNID);
	    String sessionID = MRXMLUtils.getAttributeValueXpath(mediaSrcPortDoc, "//PVRInfo", "sessionId", "").trim();		
	    SessionManager sessionManager = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
	    try{
		Document sessionDoc = sessionManager.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
		pvrEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@type='"+SessionServiceConst.SRC_PORT+"']/PVR");	//(sessionDoc, "//Entity[@type='"+SessionServiceConst.RECFILE+"'	
		if(pvrEL == null) {
		    return null;
		}		

	    }
	    catch(MRException ex){
		log.addInfo("BaseLoginRequestHandler.getStartTCValue: MREXCEPTION PERSISTENT PVR SESSION IS NOT STARTED");
		return null;
	    }

	    return pvrEL;
	}
	
	private void updateStartTCOfPVRSessions(String startTC, String pvrSessionID, String connId, String trimlength, String ffTracks, String sourceNID, MRRequest request, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, XMPPI xmpp,MRLog log) throws Exception {	
	    String whereXpath="//Entity[@type='"+SessionServiceConst.SRC_PORT+"' and Port/@"+SessionServiceConst.RESOURCENID+"[.='"+sourceNID+"' ] and PVR/@"+SessionServiceConst.ENABLE+"[.='"+SessionServiceConst.TRUE+"']]";
		Document sessionListDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 1,-1,whereXpath);		
	    List<?> sessionPVRList = MRXMLUtils.getListXpath(sessionListDoc, "//Entity");

	    for(Object entityObj : sessionPVRList){
		Element entity = (Element)entityObj;
		Element session = entity.getParent().getParent();

		String sessionID = MRXMLUtils.getAttributeValue(session, SessionServiceConst.ID);
		String srcEntityID = MRXMLUtils.getAttributeValue(entity, SessionServiceConst.ID);			
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).updatePVRInfo("",sessionID, srcEntityID, startTC, trimlength, connId, ffTracks,pvrSessionID);
		log.addInfo("BaseLoginRequestHandler.updateStartTCOfPVRSessions::srcEntity: "+srcEntityID+ " :: startTC: "+startTC+":: pvrsessionID: "+pvrSessionID);


	    }
	}
	
	protected  void updateServiceResourceInDB(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String userJID, Element assetResourceEl, String elementName, String state) throws Exception {

	    MRRequest surequest = MRRequest.createRequest(userJID);   
	 //   MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.SERVICE_RESOURCE, log);
	    /*String rootNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));              	            	    									

	    String whereXPath[] = {"//ServiceResource/Info[Type='" + AMConst.APP_SERVICE_TYPE+ "']"};
	    Element serviceResourceElement = MRXMLDBUtils.readElement(1, null, whereXPath, rootNID, 1, null, null, surequest, db, log);	 */
	    Element serviceResourceElement = HAUtils.getServiceResourceElement(db, surequest, log);
	    String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID,"");	   
	    if(serviceResourceElement!=null) {
	    	String serviceResourceNID = MRXMLUtils.getAttributeValue(serviceResourceElement, "NID");
	    	Semaphore lock = IPVSMonitor.getMonitor().getLock(serviceResourceNID, request.getNID());
	    	try {
	    		Element dbServiceResourceEl = HAUtils.getServiceResourceElement(db, surequest, log);
		    	if(!"standby".equals(state))
		    		MRXMLUtils.setValueXpath(dbServiceResourceEl, ".//AssetResourceNID", assetResourceNID);		   
		    	if("".equals(state))
		    		resetServiceResourceWithHAState(userJID, log, db, xmpp, assetResourceEl, dbServiceResourceEl);		
		    	else
		    		updateServiceResourceWithHAState(userJID, log, db, xmpp, elementName, state, assetResourceEl, dbServiceResourceEl);

		    	MRXMLDBUtils.updateElement(surequest, log, db, serviceResourceNID, dbServiceResourceEl);
		    	setServiceResourceElement(dbServiceResourceEl);
	    	
			} catch (Exception e) {
				log.addInfo("BaseLoginRequestHandler: Error Updating the service resource element in DB. serviceResourceNID= " + serviceResourceNID + " assetResourceNID=" + assetResourceNID);
				log.addInfo(e.getMessage());
			} finally {
				IPVSMonitor.getMonitor().releaseLock(serviceResourceNID, lock);
			}
	    	
	    }
		else {
		    log.addInfo("######## NO ServiceREsource Element Found Creating one #########");
		    String xmlFilePath = "ServiceResource_BASE_ALL_ADD.xml";
		    Document msgdoc = AssetAdminServiceUtils.getAssetAdminRequest(log, xmlFilePath);		    
		    MRXMLUtils.setValueXpath(msgdoc, "//StateFlag", "Ready");
		    MRXMLUtils.setValueXpath(msgdoc, "//Type", AMConst.APP_SERVICE_TYPE);
		    MRXMLUtils.setValueXpath(msgdoc, "//Description", "App Service");
		    if(!"standby".equals(state))
			MRXMLUtils.setValueXpath(msgdoc, "//AssetResourceNID", assetResourceNID);
		    if(!"".equals(state))			
			updateServiceResourceWithHAState(userJID, log, db, xmpp, elementName, state, assetResourceEl, msgdoc.getRootElement());
		    surequest = MRRequest.createRequest(userJID);
		    surequest.setSuperUser(true);
		    MediaRoomServiceUtils.getAssetAdminResponse(surequest, xmpp, log, db, resourceFactory, serviceAgentFactory, "assetadminservice", "AssetManagementRequest", msgdoc);
		}
	}
	
	protected void updateServiceResourceWithHAState(String userJID, MRLog log, MRDB db, XMPPI xmpp,String elementName, String state, Element assetResourceEl,Element serviceResourceElement) throws Exception {
		if(serviceResourceElement!= null){
			String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID,"");		
			Element haEL = MRXMLUtils.getElementXpath(serviceResourceElement, ".//"+elementName);
			MRXMLUtils.setAttributeValue(haEL, "MSAssetResourceNID", assetResourceNID );               
			MRXMLUtils.setAttributeValue(haEL, "state", state);		

			log.addInfo("BaseLoginRequestHandler:updateServiceResourceWithHAState: UserJID" + userJID +" elementname " + elementName +" State:" + state +" AssetResourceNID:" + assetResourceNID);
		}
	}
	protected void resetServiceResourceWithHAState(String userJID, MRLog log, MRDB db, XMPPI xmpp,Element assetResourceEl,Element serviceResourceElement) throws Exception {
	    if(serviceResourceElement!= null){
		String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEl, AMConst.NID,"");
		
		Element haEL = MRXMLUtils.getElementXpath(serviceResourceElement, ".//HA/*[@MSAssetResourceNID='" + assetResourceNID + "']");
		if (haEL != null) {
		    log.addInfo("HAUtils:resetServiceResourceWithHAState Reset HA on MS with userJID= " + userJID);
		    MRXMLUtils.setAttributeValue(haEL, "MSAssetResourceNID", "");               
		    MRXMLUtils.setAttributeValue(haEL, "state", "");
		}            
		log.addInfo("BaseLoginRequestHandler:resetServiceResourceWithHAState: UserJID" + userJID +" AssetResourceNID:" + assetResourceNID);
	    }
	}
	

    protected class PortUpdateNotifier implements Runnable {

	MRRequest request;
	XMPPI xmpp;
	MRLog log;
	MRDB db;
	MRResourceFactoryI resourceFactory;
	ServiceAgentFactoryI serviceAgentFactory;
	Hashtable<String, String> portObjNIDList;

	public PortUpdateNotifier(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
		ServiceAgentFactoryI serviceAgentFactory, Hashtable<String, String> portObjNIDList) {
	    this.request = request;
	    this.xmpp = xmpp;
	    this.log = log;
	    this.db = db;
	    this.resourceFactory = resourceFactory;
	    this.serviceAgentFactory = serviceAgentFactory;
	    this.portObjNIDList = portObjNIDList;
	}

	public void run() {
	    try {
		log.addInfo("PortUpdateNotifier: Starting the notifier thread for port updates");
		((AppServerAgent) (serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourcePresenceManager()
			.addPortObjectNotificationsToQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portObjNIDList);
		log.addInfo("PortUpdateNotifier: End the notifier thread for port updates");
	    } catch (Exception e) {
		e.printStackTrace();
		return;
	    }

	}

    }
}
