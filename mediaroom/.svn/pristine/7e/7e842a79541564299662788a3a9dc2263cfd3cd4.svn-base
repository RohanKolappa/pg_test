package com.ipvs.common.service;

import java.util.Hashtable;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.mediaroomservice.impl.StreamResourceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.session.handler.GetPortRequestHandler;

public class ResourcePresenceManager {

	public  Document getDeviceResourcePointers(String assetResourceNID, MRRequest request, MRDB db, MRLog log, XMPPI xmpp)
			throws MRException, Exception {
		String whereXPath = "AssetResourceNID[.='"+assetResourceNID+"']";
		String selectXML = "<Device>" +"{root($VAR)/*/@NID}{$VAR//ResourcePointers}" + "</Device>";

		Element responseEL = AssetAdminServiceUtils.getChildBasedOnSelectXML(AMConst.Device,selectXML, whereXPath, request, db, log, xmpp);    	
		if(MRXMLUtils.getElementXpath(responseEL, "./Device")!=null) 
			return MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(responseEL, "./Device"));
		return null;

	}


	public  Element updateResourceContactAndState(String contactJID, boolean resetContact, String presence, String state, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
			throws MRException, Exception {
		String resourceTitle = ServiceAgentUtils.getAgentName(contactJID);
		Element assetResourceEl = DBServiceUtils.getAssetResource(resourceTitle, MRRequest.createRequest(contactJID), log, db);

		Element responseEl = MRXMLUtils.newElement("ResourceInfo");
		if (assetResourceEl == null) {
			return responseEl;
		}
		String assetOwnerJID = MRXMLUtils.getValueXpath(assetResourceEl, "./Contact/OwnerUserJID", "");

		if( presence.equals(AMConst.UNAVAILABLE) && !assetOwnerJID.isEmpty() && !assetOwnerJID.equals(contactJID)) {
			return responseEl;
		}

		MRXMLUtils.addElement(responseEl,(Element)assetResourceEl.clone());
		String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEl, "NID", "");
		if(MRClientUtils.deleteTemporaryResource(MRXMLUtils.elementToDocument(assetResourceEl), MRRequest.createRequest(contactJID), xmpp, db, log)) {
			return responseEl;
		}

		Document deviceDoc = getDeviceResourcePointers(assetResourceNID, MRRequest.createRequest(contactJID), db, log, xmpp);
		if(deviceDoc!=null)
			MRXMLUtils.addElement(responseEl,(Element)deviceDoc.getRootElement().clone());

		Element userListEl = MRXMLUtils.getElementXpath(assetResourceEl, ".//UserList");        

		if (resetContact) {
			Element userEl = MRXMLUtils.getElementXpath(userListEl,"./User[@jid='"+contactJID+"']");
			if(userEl != null) {
				userEl.detach();
			}
			updateResourceContactAndState(assetResourceEl, deviceDoc, "", presence, userListEl, state, db, log, xmpp, resourceFactory, serviceAgentFactory,false);
		} else {
			updateResourceContactAndState(assetResourceEl, deviceDoc, contactJID, presence, userListEl, state, db, log, xmpp, resourceFactory, serviceAgentFactory,false);
		}

		return responseEl;

	}

	public  void updateResourceContactAndState(Element assetResourceEL, Document deviceDoc, String contactJID, String presence, Element userListEl, String state,
			MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,boolean resetInstances) throws MRException, Exception {

		// If unmanaged or Template don't update state
		String assetResourceState = MRXMLUtils.getValueXpath(assetResourceEL, ".//State/StateFlag", "");
		if(!MRClientUtils.ASSET_STATE_UNMANAGED.equals(assetResourceState) && !MRClientUtils.ASSET_STATE_TEMPLATE.equals(assetResourceState)) {
			String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEL, AMConst.NID,"");
			updateContactAndState(assetResourceNID, contactJID, presence, userListEl, state, log, db, xmpp, resourceFactory, serviceAgentFactory);
		}

		if (deviceDoc == null) {
			return;
		}

		List<?> listOfPorts = AssetAdminServiceUtils.getListOfPorts(deviceDoc,MRXMLUtils.getAttributeValue(assetResourceEL, "NID",""),MRRequest.createRequest(contactJID),log,db);

		if ((listOfPorts == null) || listOfPorts.isEmpty()) {
			return;
		}

		Element nidEL = null;
		String portResourceNID = null;
		String portType = "";

		for (Object object : listOfPorts) {
			nidEL = (Element) object;
			portResourceNID = MRXMLUtils.getValue(nidEL);

			if (portResourceNID == null || portResourceNID.equals("") ) {
				log.addInfo("updateResourceContactAndState portResourceNID empty ? = '" + portResourceNID +"'", MRLog.OK, MRLog.NOTIFY);
				continue;
			}

			if(resetInstances){
				StreamResourceUtils.getStreamResourceUtils().initPortInstances(contactJID,xmpp, db, log, resourceFactory, serviceAgentFactory, portResourceNID);
			}

			// If unmanaged or Template don't update state
			if(MRClientUtils.ASSET_STATE_UNMANAGED.equals(assetResourceState) || MRClientUtils.ASSET_STATE_TEMPLATE.equals(assetResourceState)) {

				if(MRClientUtils.ASSET_STATE_UNMANAGED.equals(assetResourceState)){
					updateContact(portResourceNID, contactJID, presence, null, log, db, xmpp, resourceFactory, serviceAgentFactory);
				}
				continue;
			}
			portType=  AssetAdminServiceUtils.getPortResourceTypeFromDevice(nidEL.getParent().getName());
			updateContactAndState( portResourceNID, contactJID, presence, null, state, log, db, xmpp, resourceFactory, serviceAgentFactory);

			if (portType.equals(AMConst.MEDIA_STORE_PORT_RESOURCE) && !state.equals(MRClientUtils.ASSET_STATE_NEEDS_UPGRADE)) {
				MediaStoreServiceUtils.updateStorePresenceInDir(portResourceNID, MRRequest.createRequest(contactJID), xmpp, log, db, resourceFactory, serviceAgentFactory, state, presence);
			}
			if(portType.equals(AMConst.MEDIA_STREAM_RELAY_PORT_RESOURCE_OBJECTTYPE)){
				StreamPolicyUtils.handlePolicyChanged();
			}

		}
	}
	public  void updateContactAndState(String resourceNID, String contactJID, String presence, Element userListEl, String state, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
			throws MRException, Exception {

		if (resourceNID == null) {
			return;
		}

		updateContact(resourceNID, contactJID, presence, userListEl, log, db, xmpp, resourceFactory, serviceAgentFactory);
		updateState(resourceNID, state, "", contactJID, log, db, xmpp, resourceFactory, serviceAgentFactory);
	}

	public  Document updateState(String resourceNID,String state,String stateMessage,String contactJID, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)  throws MRException,Exception{
		MRRequestI request = MRRequest.createRequest(contactJID);
		String  xmlFilePath = "/com/ipvs/xsd/objectXml/"+AMConst.STATE+".xml";
		Document stateDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);

		MRXMLUtils.setValueXpath(stateDocument, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);
		MRXMLUtils.setValueXpath(stateDocument, "//" + AMConst.STATE + "/" + AMConst.STATEMSG, stateMessage);

		String childXpath = "//" + AMConst.STATE;
		return AssetAdminServiceUtils.updateChild(resourceNID, childXpath, stateDocument.getRootElement(), request, log, db, xmpp, resourceFactory, serviceAgentFactory);   
	}

	public  Document updateContact(String resourceNID,String contactJID,String presence, Element userListEl, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)  throws MRException,Exception{
		MRRequestI request = MRRequest.createRequest(contactJID);
		String xmlFilePath = "/com/ipvs/xsd/objectXml/"+AMConst.CONTACT+".xml";
		Document contactDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);

		MRXMLUtils.setValueXpath(contactDocument, "//" + AMConst.CONTACT + "/" + AMConst.OWNERUSERJID, contactJID);
		MRXMLUtils.setValueXpath(contactDocument, "//" + AMConst.CONTACT + "/" + AMConst.PRESENCE, presence);
		String childXpath = "//"+AMConst.CONTACT;
		if(userListEl != null)
			MRXMLUtils.addElementXpath(contactDocument, childXpath, (Element)userListEl.clone());
		return AssetAdminServiceUtils.updateChild(resourceNID, childXpath, contactDocument.getRootElement(), request, log, db, xmpp, resourceFactory, serviceAgentFactory);
	}



	public  void updateResourceListStateAsOffline(String userJID, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {

		log.addInfo("Reset Resources - START",MRLog.INFO,MRLog.NOTIFY);

		// reset sessionId in port object 
		//resetSessionIdInPortObject(log, db, userJID);

		//reset asset resource
		resetResource(AMConst.AssetResource, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		//reset Media stream src port
		resetResource(AMConst.MediaStreamSrcPortResource, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		//reset Media stream dst port
		resetResource(AMConst.MediaStreamDstPortResource, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		//reset Media stream io port
		resetResource(AMConst.MEDIA_STREAM_IO_PORT_RESOURCE_OBJECTTYPE, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		//reset Media store port
		resetResource(AMConst.MEDIA_STORE_PORT_RESOURCE, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		//reset Media Stream relay port
		resetResource(AMConst.MediaStreamRelayPortResource, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		//delete temporary resource
		deleteTemporaryResource(userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		log.addInfo("Reset Resources - END",MRLog.INFO,MRLog.NOTIFY);



		/*       
		 *  response = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, listName, AMConst.AssetResource, nid, childXPath,
	                null, SortByXpath, MRXMLDBUtils.SORT_ASCENDING, selectXML, response, startFrom, countToFetch);
	        List<?> assetResourceELList = MRXMLUtils.getListXpath(response, "./" + AMConst.AssetResource);
	        if ((assetResourceELList == null) || assetResourceELList.isEmpty()) {
	            return;
	        }

	        Element assetResourceEL = null;
	        String assetResourceNID = null;
	        Document deviceDoc = null;

	        for (Object object : assetResourceELList) {
	            assetResourceEL = (Element) object;          

	            assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceEL, AMConst.NID, "");            
	            if(MRClientUtils.deleteTemporaryResource(MRXMLUtils.elementToDocument(assetResourceEL), MRRequest.createRequest(userJID), xmpp, db, log)) {
	            	continue;
	            }
	            deviceDoc = getDeviceResourcePointers(assetResourceNID, request, db, log, xmpp);                                                
	            updateResourceContactAndState(assetResourceEL, deviceDoc, "", AMConst.UNAVAILABLE, MRClientUtils.ASSET_STATE_OFFLINE, db, log, xmpp,
	                resourceFactory, serviceAgentFactory,true);
	        }*/
	}

//	private  void resetSessionIdInPortObject(MRLog log, MRDB db, String userJID) throws Exception {
//		log.addInfo("AppserverAgent: Reset sessionId in port object for persistent PVR sessions ", MRLog.INFO, MRLog.NOTIFY);
//		String NID = "";
//		String listName = "";
//		String childXPath = "//" + SessionServiceResourceConst.PVR_INFO_ELEMENT + "/@" + SessionServiceConst.SESSION_ID;
//		String whereXPath = "[//" + SessionServiceResourceConst.PVR_INFO_ELEMENT + "/@" + SessionServiceResourceConst.ENABLE_ATTRIBUTE 
//				+ "='" + SessionServiceConst.TRUE + "']";
//		String data = " attribute " + SessionServiceConst.SESSION_ID + " {\"\"} ";
//
//		MRRequestI request = MRRequest.createRequest(userJID);        	
//		MRXMLDBUtils.replaceAttribute(MRXMLDBUtils.TIME_OUT, request, log, db, listName, SessionServiceConst.MEDIA_STREAM_SRC_PORT_RESOURCE, 
//				NID, childXPath, new String[]{"//" + SessionServiceConst.MEDIA_STREAM_SRC_PORT_RESOURCE + whereXPath}, data);
//		MRXMLDBUtils.replaceAttribute(MRXMLDBUtils.TIME_OUT, request, log, db, listName, SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE, 
//				NID, childXPath, new String[]{"//" + SessionServiceConst.MEDIA_STREAM_IO_PORT_RESOURCE + whereXPath}, data);
//	}

	private  void deleteTemporaryResource(String userJID, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

		String whereXpath[] = {"//AssetResource[@Persistence='"+MRClientUtils.TEMPORARY+"' and ( //TemplateAssetResourceNID [.!='' and .!='"+MRClientUtils.ASSET_TEMPLATE+"'])]"};
		MRRequestI request = MRRequest.createRequest(userJID);  
		String childXPath = "";
		String selectXML ="<AssetResource>" +"{root($VAR)/*/@NID}" + "</AssetResource>";
		Element response = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, null, AMConst.AssetResource, "", childXPath,
				whereXpath, new String[]{}, MRXMLDBUtils.SORT_ASCENDING,selectXML, null, 0, -1);

		List<?> assetResourceNIDList = MRXMLUtils.getValuesXpath(response.getDocument(),"//@NID");
		if(assetResourceNIDList ==null || assetResourceNIDList.isEmpty()){
			return;
		}

		// delete device and destination port 
		String assetNIDWhereXpath[] = {"//AssetResourceNID[contains('"+StringUtils.join(assetResourceNIDList, ",")+"',.)]"};
		deleteTemporaryResource(AMConst.Device, assetNIDWhereXpath, request, db, log, xmpp, resourceFactory, serviceAgentFactory);
		deleteTemporaryResource(AMConst.MediaStreamDstPortResource, assetNIDWhereXpath, request, db, log, xmpp, resourceFactory, serviceAgentFactory);	

		//delete asset resource
		String assetWhereXpath[] = {"//AssetResource/@NID[contains('"+StringUtils.join(assetResourceNIDList, ",")+"',.)]"};
		deleteTemporaryResource(AMConst.AssetResource, assetWhereXpath, request, db, log, xmpp, resourceFactory, serviceAgentFactory);
		log.addInfo("Delete Temporary Resources", MRLog.INFO, MRLog.NOTIFY);

	}

	private  void deleteTemporaryResource(String objectType,String[] assetWhereXpath,MRRequestI request,MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", objectType , "", "", assetWhereXpath);	
	}


	public  void resetState(String objectType,String userJID, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

		//update State
		String whereXpath = "//StateFlag[.!='"+MRClientUtils.ASSET_STATE_OFFLINE+"' and not(.='"+MRClientUtils.ASSET_STATE_UNMANAGED+"' or .='"+MRClientUtils.ASSET_STATE_TEMPLATE+"')]";
		MRRequestI request = MRRequest.createRequest(userJID);
		String  xmlFilePath = "/com/ipvs/xsd/objectXml/"+AMConst.STATE+".xml";
		Document stateDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);

		MRXMLUtils.setValueXpath(stateDocument, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, MRClientUtils.ASSET_STATE_OFFLINE);
		MRXMLUtils.setValueXpath(stateDocument, "//" + AMConst.STATE + "/" + AMConst.STATEMSG, "");
		String childXpath = "//" + AMConst.STATE;
		String listName ="";
		String nid="";
		MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, nid, childXpath, new String[]{whereXpath}, (Element)stateDocument.getRootElement().detach());

		log.addInfo("Reset State. ObjectType:="+objectType, MRLog.INFO, MRLog.NOTIFY);
	}


	public  void resetContact(String objectType,String userJID, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

		String whereXpath = "//Contact[Presence!='"+AMConst.UNAVAILABLE+"' or OwnerUserJID!='']";
		String xmlFilePath = "/com/ipvs/xsd/objectXml/"+AMConst.CONTACT+".xml";
		Document contactDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);
		MRRequestI request = MRRequest.createRequest(userJID);

		MRXMLUtils.setValueXpath(contactDocument, "//" + AMConst.CONTACT + "/" + AMConst.OWNERUSERJID, "");
		MRXMLUtils.setValueXpath(contactDocument, "//" + AMConst.CONTACT + "/" + AMConst.PRESENCE, AMConst.UNAVAILABLE);
		String childXpath = "//"+AMConst.CONTACT;
		MRXMLUtils.addElementXpath(contactDocument, childXpath, MRXMLUtils.newElement("UserList"));
		String listName ="";
		String nid="";

		MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, nid, childXpath, new String[]{whereXpath}, (Element)contactDocument.getRootElement().detach());

		log.addInfo("Reset Contact. ObjectType:="+objectType, MRLog.INFO, MRLog.NOTIFY);

	}

	public  void resetResource(String objectType,String userJID, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		log.addInfo("Reset Resource. ObjectType:="+objectType, MRLog.INFO, MRLog.NOTIFY);
		//update State
		resetState(objectType, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);  	

		//Update contact
		resetContact(objectType, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		// reset port instances
		if(!objectType.equals(AMConst.AssetResource)){
			StreamResourceUtils.getStreamResourceUtils().initPortInstancesOfResource(userJID, xmpp, db, log, resourceFactory, serviceAgentFactory, objectType);        	
		}  

		// rest state and presence of port in directory
		if(objectType.equals(AMConst.MEDIA_STORE_PORT_RESOURCE)){
			resetMediaDirResource( userJID,  db,  log,  xmpp,  resourceFactory, serviceAgentFactory);
		}        
	}

	private  void resetMediaDirResource(String userJID, MRDB db,MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

		//update State
		resetState(AMConst.MEDIA_STREAM_DIR_RESOURCE, userJID, db, log, xmpp, resourceFactory, serviceAgentFactory);

		//reset port presence in directory
		MRRequestI request = MRRequest.createRequest(userJID);
		String childXpath = "//MediaStorePortResourceNID/@"+MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME;
		String whereXpath[] = {"//MediaStreamDirResource[//ParentMediaDirNID='"+MediaStoreServiceConst.DEFAULT_DIR+"' and " +
				" //MediaStorePortResourceNID[@"+MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME +"!='"+AMConst.UNAVAILABLE+"']]"};
		String listName ="";
		String nid="";
		String data= MRClientUtils.ASSET_STATE_OFFLINE;
		MRXMLDBUtils.replaceAttribute(MRXMLDBUtils.TIME_OUT, request, log, db, listName, AMConst.MEDIA_STREAM_DIR_RESOURCE, nid, childXpath, whereXpath, data);
		log.addInfo("Reset Direcory State and Presence of Port", MRLog.INFO, MRLog.NOTIFY);

		// Remove MediaStorePortResourceNID's that doesn't exist and update DB
		Element dirListEL = MRXMLUtils.newElement("DirListEL");
		String whereXPath[] = {"//MediaStreamDirResource[//ParentMediaDirNID='"+MediaStoreServiceConst.DEFAULT_DIR+"']"};
		dirListEL = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, listName, AMConst.MEDIA_STREAM_DIR_RESOURCE, nid, "", whereXPath, null, "", "", dirListEL, 0, -1);
		List<?> dirList = MRXMLUtils.getListXpath(MRXMLUtils.elementToDocument(dirListEL), "//MediaStreamDirResource");

		Element mediaStorePortResourceListEL = MRXMLUtils.newElement("MediaStorePortResourceList");
		MRXMLUtils.elementToDocument(MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, listName, AMConst.MEDIA_STORE_PORT_RESOURCE, nid, "", null, null, "", "", mediaStorePortResourceListEL, 0, -1));

		for(Object dir:dirList) {
			Document dirDoc = MRXMLUtils.elementToDocument((Element)dir);
			String dirNID = MRXMLUtils.getAttributeValue(dirDoc.getRootElement(), AMConst.NID);
			List<?> mediaStorePortResourceNIDElList = MRXMLUtils.getListXpath(dirDoc, "//MediaStorePortResourceNID");
			for(Object mediaStorePortResourceNID:mediaStorePortResourceNIDElList) {
				Element mediaStorePortResourceNIDEL = (Element)mediaStorePortResourceNID;
				nid = MRXMLUtils.getValue(mediaStorePortResourceNIDEL);

				if(MRXMLUtils.getElementXpath(mediaStorePortResourceListEL, "./MediaStorePortResource[@NID='"+nid+"']") == null) {
					MRXMLUtils.removeNodeXpath(dirDoc, "//MediaStorePortResourceNID[.='"+nid+"']");
				}
			}
			MRXMLDBUtils.updateElement(request, log, db, dirNID, dirDoc.getRootElement());
		}

	}

	public  List<?> getListOfPorts(Document deviceDoc) throws MRException, Exception {
		return MRXMLUtils.getListXpath(deviceDoc, "//ResourcePointers/*/NIDValue");
	}


	public void addPortObjectNotificationsToQueue(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Hashtable<String, String> portNIDMap) throws Exception {
		
		String whereNID = StringUtils.join(portNIDMap.keySet(), ",");
		
		if(whereNID != null) {
			Class<?> classObject = Class.forName(GetPortRequestHandler.class.getName());
			String className = classObject.getName();
			String requestName = MRRequest.getRequestName(className);
			Element requestEl = MRXMLUtils.stringToElement("<GetPort/>");			
			MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.WHERE_NID, whereNID);
			MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.SELECTFORMAT, SessionServiceConst.SELECTFORMAT_BASICCONNECTIONINFO);
			
			Document getRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, requestEl);	
			MRRequest getRequest = MRRequest.createRequest(AppServerAgentUtils.getAppServerAgentJID(request.getUserJID(), serviceAgentFactory), getRequestDoc);
			getRequest.setSuperUser(true);
            Document responseDoc = new GetPortRequestHandler().getResponse(getRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
        
            if(responseDoc != null) {
            	log.addInfo("ResourcePresenceManager: addPortObjectNotificationsToQueue: Attempting to add Notifications to Queue", MRLog.INFO, MRLog.NOTIFY);
            	for(String nid : portNIDMap.keySet()) {
            		String action = portNIDMap.get(nid);
            		Element portEl = MRXMLUtils.getElementXpath(responseDoc, "//Port[@NID='"+ nid +"']");
           			
            		if(action.equals(SessionServiceConst.ADD))
            			MRXMLUtils.setAttributeValue(portEl, "dirty", "true");
            		
           			if(portEl != null) {
           				((AppServerAgent)(serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourceManager().queueObjectNotification(xmpp, getRequest, log, db, SessionServiceConst.PORT, SessionServiceConst.PORT, nid, action, MRXMLUtils.elementToDocument(portEl));
           			}
            	}
            }
		}
		
	}
}
