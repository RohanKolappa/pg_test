package com.ipvs.sessionservice.resource.handler;


import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;


public class AddDeviceRequestHandler extends BaseDeviceRequestHandler {
	
	public static final String ADD_DEVICE_MAPPING = "/com/ipvs/sessionservice/resource/xml/AddDeviceMapping.xml";
    @Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {	
    	log.addInfo("AddDeviceRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AMConst.AssetResource);		
    	Element addDeviceRequestElement = requestObject.getRequestElement();
		String deviceType = MRXMLUtils.getAttributeValue(addDeviceRequestElement, SessionServiceConst.TYPE_ATTRIBUTE,"");
    	Document addDeviceMappingDocument = MRXMLUtils.loadXMLResource(ADD_DEVICE_MAPPING, log);
    	String deviceXpath = "//Device[@type='"+deviceType+"']";
    	Element deviceMappingElement = MRXMLUtils.getElementXpath(addDeviceMappingDocument, deviceXpath);
    	
    	if(deviceMappingElement == null) {
    		throw new MRException(SessionServiceExceptionCodes.INVALID_DEVICE_TYPE, MRXMLUtils.generateErrorXML("type", deviceType));
    	}
    
    	String title = MRXMLUtils.getAttributeValue(addDeviceRequestElement, SessionServiceConst.TITLE,"");
    	String description = MRXMLUtils.getAttributeValue(addDeviceRequestElement, SessionServiceConst.DESCRIPTION.toLowerCase(),"");
    	String password = MRXMLUtils.getAttributeValue(addDeviceRequestElement, SessionServiceResourceConst.PASSWORD_ATTRIBUTE,"");
    	String unManaged = MRXMLUtils.getAttributeValue(addDeviceRequestElement, SessionServiceResourceConst.UNMANAGED_ATTRIBUTE,SessionServiceConst.FALSE);
    	String unManned = MRXMLUtils.getAttributeValue(addDeviceRequestElement, SessionServiceResourceConst.UNMANNED_ATTRIBUTE,SessionServiceConst.FALSE);
    	String applyTagsToPorts = MRXMLUtils.getAttributeValue(addDeviceRequestElement, SessionServiceResourceConst.APPLY_TAGS_TO_PORTS_ATTRIBUTE,SessionServiceConst.TRUE);
    	String devicePrivateData = MRXMLUtils.getValueXpath(addDeviceRequestElement, "./"+SessionServiceResourceConst.DEVICE_PRIVATE_DATA_ELEMENT,"");
    	Element groupsEL = MRXMLUtils.getElementXpath(addDeviceRequestElement, "./"+SessionServiceResourceConst.GROUPS_ELEMENT);
    	
    	//set state value
    	String assetTemplate = MRXMLUtils.getAttributeValue(deviceMappingElement, SessionServiceResourceConst.ASSET_TEMPLATE_ATTRIBUTE,SessionServiceConst.FALSE);
    	String state = MRClientUtils.ASSET_STATE_OFFLINE;
    	String presence = AMConst.UNAVAILABLE;
    	if(unManaged.equals(SessionServiceConst.TRUE)) {
    		state = MRClientUtils.ASSET_STATE_UNMANAGED;
    		presence = MRClientUtils.ASSET_STATE_UNMANAGED;
    	} if(assetTemplate.equals(SessionServiceConst.TRUE)){
    		state = MRClientUtils.ASSET_STATE_TEMPLATE;
    		presence = MRClientUtils.ASSET_STATE_TEMPLATE;
    	}
    	
    	
    	//add asset resource
    	String assetResourceNID = addAssetResource( request, xmpp, log, db, resourceFactory, serviceAgentFactory,deviceType,title, description, password, unManaged, unManned, groupsEL, devicePrivateData, state, presence, deviceMappingElement,assetTemplate);
    	
    	//add port resources
    	HashMap<String, String> portNIDMap = addPortResources( request, xmpp, log, db, resourceFactory, serviceAgentFactory,assetResourceNID, title, description, password, unManaged, unManned, applyTagsToPorts,groupsEL, devicePrivateData, state, presence, deviceMappingElement, deviceType);
    	
    	//add device 
    	String addDevice = MRXMLUtils.getAttributeValue(deviceMappingElement, SessionServiceResourceConst.ADD_DEVICE_ATTRIBUTE,SessionServiceConst.TRUE);
    	if(addDevice.equals(SessionServiceConst.TRUE)){
    		addDevice(request, xmpp, log, db, resourceFactory, serviceAgentFactory, assetResourceNID, portNIDMap, deviceType, title, description, password,unManned, deviceMappingElement);
    	}
    	StreamPolicyUtils.handlePolicyChanged();
    	log.addInfo("Added Device. Title:="+title,MRLog.INFO,MRLog.NOTIFY);    	
    	
    	Document responseDoc = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, 
        		SessionServiceConst.SELECTFORMAT_EXTENDED, GET_DEVICE_REQUEST_XML_PATH, GetDeviceRequestHandler.class.getName());
    	
    	// queue port object notifications
    	queuePortObjectNotifications(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portNIDMap);
    	
    	
    	return responseDoc; 
    }
    
    private void queuePortObjectNotifications(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			HashMap<String, String> portNIDMap) throws Exception {
    	
    	Hashtable<String, String> addNidMap = new Hashtable<String, String>();
		for(String nid : portNIDMap.keySet()) { addNidMap.put(nid, "Add"); }
		((AppServerAgent)(serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourcePresenceManager().addPortObjectNotificationsToQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, addNidMap);
	}

	private String addAssetResource(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory,
    		String deviceType,String title,String description,String password,String unManaged,String unManned,Element groupsEL,String devicePrivateData,
    		String state,String presence, Element deviceMappingElement,String assetTemplate) throws MRException,Exception {
    	
    	Document assetResourceDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.AssetResource, log);
    	setCommonValues( request, xmpp, log, db, resourceFactory, serviceAgentFactory,title, description,SessionServiceConst.TRUE, groupsEL, state, presence, assetResourceDoc);
    	//set bot user info if bot user enabled
    	String botUser = MRXMLUtils.getAttributeValue(deviceMappingElement, SessionServiceResourceConst.BOTUSER_ATTRIBUTE,SessionServiceConst.TRUE);    
    	String enableBotUser = SessionServiceConst.TRUE;
    	if((botUser.equals(SessionServiceConst.FALSE) && unManned.equals(SessionServiceConst.FALSE)) || unManaged.equals(SessionServiceConst.TRUE)) {
    		enableBotUser = SessionServiceConst.FALSE;
    	}
    	MRXMLUtils.setAttributeValueXPath(assetResourceDoc, "//"+SessionServiceResourceConst.BOT_USER_ELEMENT,SessionServiceResourceConst.ENABLE_ATTRIBUTE, enableBotUser);
    	
    	
    	//set password in asset key data
    	MRXMLUtils.setValueXpath(assetResourceDoc, "//"+SessionServiceConst.TYPE_ELEMENT, deviceType); 
    	
    	//set password in asset key data
    	MRXMLUtils.setValueXpath(assetResourceDoc, "//"+SessionServiceResourceConst.KEY_DATA_ELEMENT, password);   	
    	
    	//set device private dataG
    	MRXMLUtils.setValueXpath(assetResourceDoc, "//"+SessionServiceResourceConst.ASSET_PRIVATE_DATA_ELEMENT, devicePrivateData);
    	
    	//set template NID if assetTemplate
    	if(assetTemplate.equals(SessionServiceConst.TRUE)) {
    		MRXMLUtils.setValueXpath(assetResourceDoc, "//"+SessionServiceResourceConst.TEMPLATE_ASSET_RESOURCE_NID, SessionServiceResourceConst.TEMPLATE);
    	}
    	
    	if(unManned.equals(SessionServiceConst.TRUE)){
    		 Element featureEL = MRXMLUtils.getElementXpath(assetResourceDoc, "//"+SessionServiceResourceConst.FEATURE_ELEMENT);
    		 MRXMLUtils.setAttributeValue(featureEL, SessionServiceResourceConst.FEATURE_NAME_ATTRIBUTE, SessionServiceResourceConst.FEATURE_MODE);
    		 MRXMLUtils.setValue(featureEL, SessionServiceResourceConst.FEATURE_UNMANNED);
    	}
    	return AddObjectUtils.addObject(request, xmpp, log, db, resourceFactory, serviceAgentFactory, assetResourceDoc, AMConst.AssetResource);
    }
    
    
    
    private void setCommonValues(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory,
    		String title,String description,String applyTagsToPorts,Element groupsEL,String state,String presence, Document resourceDoc) throws MRException,Exception {
    	MRXMLUtils.setValueXpath(resourceDoc, "//"+AMConst.TITLE, title);
    	MRXMLUtils.setValueXpath(resourceDoc, "//"+AMConst.DESCRIPTION, description);
    	MRXMLUtils.setValueXpath(resourceDoc, "//"+AMConst.DESCRIPTION, description);
    	//set state & presence value
    	MRXMLUtils.setValueXpath(resourceDoc, "//"+AMConst.STATEFLAG, state);
    	MRXMLUtils.setValueXpath(resourceDoc, "//"+AMConst.PRESENCE, presence);  
    	if(applyTagsToPorts.equals(SessionServiceConst.TRUE)) {
    		MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(resourceDoc, "//"+SessionServiceResourceConst.GROUPS_ELEMENT), groupsEL);
    	}
    }
    
    
    private HashMap<String,String> addPortResources(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory,
    		String assetResourceNID,String title,String description,String password,String unManaged,String unManned,String applyTagsToPorts,Element groupsEL,
    		String devicePrivateData,String state,String presence, Element deviceMappingElement, String deviceType) throws MRException,Exception {
    	List<?> portList = MRXMLUtils.getListXpath(deviceMappingElement, "./PortList/Port");
    	HashMap<String,String> portNIDMap = new HashMap<String,String>();
    	if(portList.isEmpty()){
    		return portNIDMap;
    	}
    	
    	for(Object object:portList) {
    		Element portElement = (Element) object;
    		String objectName = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.NAME);
    		Document portResourceDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, objectName, log);
    		String titlePostFix = MRXMLUtils.getAttributeValue(portElement, SessionServiceResourceConst.TITLE_POST_FIX_ATTRIBUTE,"");
    		String portTitle = title;
    		if(!portTitle.isEmpty()) {
    			portTitle+=titlePostFix;
    		}
    		
    		//set common values title,description,groups,presence and state 
    		setCommonValues(request, xmpp, log, db, resourceFactory, serviceAgentFactory,portTitle, description, applyTagsToPorts,groupsEL, state, presence, portResourceDoc);
    		
    		//set assetResourceNID 
    		MRXMLUtils.setValueXpath(portResourceDoc, "//"+SessionServiceResourceConst.ASSET_RESOURCE_NID_ELEMENT,assetResourceNID);
    		
    		//set port type 
    		MRXMLUtils.setValueXpath(portResourceDoc, "//"+SessionServiceResourceConst.INFO_ELEMENT+"/"+SessionServiceConst.TYPE_ELEMENT, MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.TYPE_ATTRIBUTE,""));
        	
    		//set max instance count 
    		MRXMLUtils.setValueXpath(portResourceDoc, "//"+SessionServiceResourceConst.INSTANCE_INFO+"/"+SessionServiceResourceConst.MAX_INSTANCE_COUNT_ELEMEMNT, MRXMLUtils.getAttributeValue(portElement, SessionServiceResourceConst.MAX_INSTANCE_COUNT_ATTRIBUTE,""));
    		
    		//set device element id  
    		MRXMLUtils.setValueXpath(portResourceDoc, "//"+SessionServiceResourceConst.INFO_ELEMENT+"/"+SessionServiceResourceConst.DEVICE_ELEMENT_ID_ELEMENT, MRXMLUtils.getAttributeValue(portElement, SessionServiceResourceConst.DEVICE_ELEMENT_ID_ATTRIBUTE,""));
    		
    		//set remotely operated mode
    		String remotelyOperatedMode = MRXMLUtils.getAttributeValue(portElement, SessionServiceResourceConst.REMOTELY_OPERATED_MODE_ATTR, "");
    		if(!remotelyOperatedMode.isEmpty()) {
                MRXMLUtils.setValueXpath(portResourceDoc, "//"+SessionServiceResourceConst.INFO_ELEMENT+"/"+SessionServiceResourceConst.REMOTELY_OPERATED_MODE, remotelyOperatedMode);
            }
    		
    		//set stream type
    		String streamType = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.STREAM_TYPE,"");
    		StringTokenizer streamTypeTokenizer = new StringTokenizer(streamType, ",");
    		Element streamtypeListEl = MRXMLUtils.getElementXpath(portResourceDoc, "//"+SessionServiceResourceConst.STREAM_TYPE_LIST_ELEMENT);
    		Element streamTypeEl = MRXMLUtils.getElement(streamtypeListEl, SessionServiceResourceConst.STREAM_TYPE_ELEMENT);
    		streamTypeEl.detach();
    		while (streamTypeTokenizer.hasMoreTokens()) {
    	    	String token = streamTypeTokenizer.nextToken().trim();
    	    	MRXMLUtils.setValue(streamTypeEl, token);
    	    	MRXMLUtils.addElement(streamtypeListEl,(Element)streamTypeEl.clone());
    	    }
    		
    		Element resourceFeatureList = MRXMLUtils.getElementXpath(portResourceDoc,"//"+SessionServiceResourceConst.FEATURE_LIST_ELEMENT);
    		
    		if(resourceFeatureList != null) {
    			resourceFeatureList.clearContent();
    		}
    		
    		//update feature list for unmanned devices
    		String updateFeatureListIfUnamanned = MRXMLUtils.getAttributeValue(portElement, SessionServiceResourceConst.UPDATE_FEATURE_LIST_IF_UNMANNED_ATTRIBUTE,SessionServiceConst.FALSE);
    		if(unManned.equals(SessionServiceConst.TRUE) && updateFeatureListIfUnamanned.equals(SessionServiceConst.TRUE)){    		
    			updateFeatureList(portElement, SessionServiceResourceConst.UNMANNED_FEATURE_LIST_ELEMENT, resourceFeatureList);        		
    		}
    		
    		//update feature list for all the devices if the flag is enabled
    		String updateFeatureList = MRXMLUtils.getAttributeValue(portElement, SessionServiceResourceConst.UPDATE_FEATURE_LIST_ATTRIBUTE,SessionServiceConst.FALSE);        	
    		if(updateFeatureList.equals(SessionServiceConst.TRUE)) {    			
    			updateFeatureList(portElement, SessionServiceResourceConst.FEATURE_LIST_ELEMENT, resourceFeatureList);
    		}    		
    		
    		//update feature list for unmanaged device if the flag is provided
    		String updateFeatureListIfUnManaged = MRXMLUtils.getAttributeValue(portElement, SessionServiceResourceConst.UPDATE_FEATURE_LIST_IF_UNMANAGED_ATTRIBUTE,SessionServiceConst.FALSE);        	
    		if(unManaged.equals(SessionServiceConst.TRUE) && updateFeatureListIfUnManaged.equals(SessionServiceConst.TRUE)) {
    			updateFeatureList(portElement, SessionServiceResourceConst.UNMANAGED_FEATURE_LIST_ELEMENT, resourceFeatureList);
    		}
    		
    		//empty media stream list 
    		List<?> mediaStreamList  = MRXMLUtils.getListXpath(portResourceDoc, "//"+SessionServiceResourceConst.MEDIA_STREAM_LIST);
    		for(Object mediaStreamObject:mediaStreamList) {
    			MRXMLUtils.removeContent((Element)mediaStreamObject);
    		}
    		
    		//empty profile list
    		List<?> mediaStreamProfileInfoElList  = MRXMLUtils.getListXpath(portResourceDoc, "//"+SessionServiceResourceConst.MEDIA_STREAM_PROFILE_INFO);
    		List<String> streamTypeList = MRXMLUtils.getValuesXpath(portResourceDoc, "//StreamType");    		
    	
    		String portType = MRXMLUtils.getValueXpath(portResourceDoc, "//Type", "");    
    		//log.addInfo("AddDeviceRequestHandler::portType: "+portType, MRLog.OK, MRLog.NOTIFY);
    		for(Object mediaStreamProfileInfoObject:mediaStreamProfileInfoElList) {
    			Element mediaStreamProfileInfoEle = (Element)mediaStreamProfileInfoObject;  
    			List<?> mediaStreamProfiles=MRXMLUtils.getChildren(mediaStreamProfileInfoEle);
    			for(Object streamProfileInfoELeObj:mediaStreamProfiles){
    				Element streamProfileInfoELe = (Element)streamProfileInfoELeObj;    				
    				String type = streamProfileInfoELe.getName().substring(0, streamProfileInfoELe.getName().length()-SessionServiceResourceConst.MEDIA_STREAM_PROFILE_INFO.length());
    				if(!streamTypeList.contains(type)){
    					MRXMLUtils.removeChild(mediaStreamProfileInfoEle,type+SessionServiceResourceConst.MEDIA_STREAM_PROFILE_INFO);
    				} else{

    				    if(SessionServiceConst.STREAM_SRC.equals(portType) || SessionServiceConst.STREAM_DST.equals(portType)){    			
    				    	AssetAdminServiceUtils.setSourceDefaultParameters(portResourceDoc.getRootElement(), SessionServiceConst.MODE_DYNAMIC,log);
    				    	
    				    	//remove Recorderconfig 
    				    	//TODO modify this once the RecoderConfig spelling error bug is fixed
                            String recorderConfig ="";
                            if(type.equals("V2D")){
                                recorderConfig =type+"RecorderConfig";
                            }else{ 
                            if(type.contains("-")){
                                type = type.replace("-", "");
                            }
                            recorderConfig =type+"RecoderConfig";
                            }
                            log.addInfo("recorderConfig>>"+portResourceDoc.asXML());
                            Element recorderConfigEl = MRXMLUtils.getElementXpath(portResourceDoc, "//"+recorderConfig);
                            if(recorderConfigEl != null) {
                                recorderConfigEl.detach();
                             log.addInfo("recorderConfig>>"+portResourceDoc.asXML());
                            }
    				    }
    				    
    				}
    			}    			
    		}    		
    	   	String portNID = AddObjectUtils.addObject(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portResourceDoc, objectName);    	   	
    	   	portNIDMap.put(portNID, objectName);
    	}
    	
    	return portNIDMap;
    }
    
    private void updateFeatureList(Element portElement,String featureListName,Element resourceFeatureList) throws MRException,Exception {
    	Element featureList = MRXMLUtils.getElement(portElement,featureListName);
		if(featureList != null) { 
			MRXMLUtils.addChildrenToElement(resourceFeatureList, featureList);
		}
    }
    
    
    private String addDevice(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory,
    		String assetResourceNID,HashMap<String,String> portNIDMap,String deviceType,String title,String description,String password,String unManned,Element deviceMappingElement) throws MRException,Exception {
    	
    	Document deviceDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, deviceType, log);
    	MRXMLUtils.setValueXpath(deviceDoc, "//"+SessionServiceResourceConst.ASSET_RESOURCE_NID_ELEMENT, assetResourceNID);
    	Set<String> portNIDSet = portNIDMap.keySet();
    	for(String portNID : portNIDSet) {
    		String portObject = portNIDMap.get(portNID);
    		Element resourcPointerNIDValueElement = MRXMLUtils.getElementXpath(deviceDoc, "//"+portObject+SessionServiceConst.NID+"["+SessionServiceResourceConst.NID_VALUE+"='']/"+SessionServiceResourceConst.NID_VALUE);
    		MRXMLUtils.setValue(resourcPointerNIDValueElement, portNID);
    	}
    	
    	//set device name and device password
    	MRXMLUtils.setValueXpath(deviceDoc, "//"+SessionServiceResourceConst.DEVICE_NAME, title);
    	MRXMLUtils.setValueXpath(deviceDoc, "//"+SessionServiceResourceConst.DEVICE_PASSWORD, password);
    	
    	//set display configurable true if it is unmanned otherwise set false
    	String diplayconfigrable= SessionServiceConst.FALSE;
    	String enableUnamanedMode = MRXMLUtils.getAttributeValue(deviceMappingElement, SessionServiceResourceConst.ENABLE_UNMANNED_MODE,SessionServiceConst.FALSE);
       
    	if(unManned.equals(SessionServiceConst.TRUE) || enableUnamanedMode.equals(SessionServiceConst.TRUE)){
    		diplayconfigrable = SessionServiceConst.TRUE;
    	}    	
    	
    	String displayConfigNode = MRXMLUtils.getAttributeValue(deviceMappingElement, SessionServiceResourceConst.DISPLAY_CONFIG_NODE,"");
    	String displayStatusNode = MRXMLUtils.getAttributeValue(deviceMappingElement, SessionServiceResourceConst.DISPLAY_CONFIG_STATUS_NODE,"");
    	
    	if(!displayConfigNode.isEmpty()) {
    		MRXMLUtils.setAttributeValueXPath(deviceDoc, "//"+displayConfigNode+"/"+SessionServiceConst.DISPLAY, SessionServiceResourceConst.CONFIGURABLE_ATTRIBUTE, diplayconfigrable);
    	}
    	
    	if(!displayStatusNode.isEmpty()) {
    		MRXMLUtils.setAttributeValueXPath(deviceDoc, "//"+displayStatusNode+"/"+SessionServiceConst.DISPLAY, SessionServiceResourceConst.CONFIGURABLE_ATTRIBUTE, diplayconfigrable);
    	}
    	
    	//remove streaming service instance from status
    	Element streamingServiceInstanceList = MRXMLUtils.getElementXpath(deviceDoc, "//"+SessionServiceResourceConst.STREAMING_SERVICE_STATUS+"/"+SessionServiceResourceConst.INSTANCE_LIST);
    	if(streamingServiceInstanceList != null) {
    		MRXMLUtils.removeContent(streamingServiceInstanceList);
    	}
    	
    	//add empty software version node
     	String childXpath = "//SoftwareImageStatus";
    	String xmlFilePath = "/com/ipvs/xsd/objectXml/SoftwareImageStatus.xml";        
    	Document softwareImageStatusDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	    Element softwareImageStatusEL = softwareImageStatusDocument.getRootElement();
	        
	    MRXMLUtils.setValueXpath(softwareImageStatusEL, "Parameters/Parameter[@name='Version']", "");
	    MRXMLUtils.setValueXpath(softwareImageStatusEL, "Parameters/Parameter[@name='Service Version']", "");
	    Element deviceSoftwareImageStatusEl = MRXMLUtils.getElementXpath(deviceDoc, childXpath);
	    if(deviceSoftwareImageStatusEl!=null){
	    	MRXMLUtils.replaceContent(deviceSoftwareImageStatusEl, softwareImageStatusEL);
	    }
	    
    	return  AddObjectUtils.addObject(request, xmpp, log, db, resourceFactory, serviceAgentFactory, deviceDoc, deviceType);
    }    
    
}
