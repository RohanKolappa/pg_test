package com.ipvs.common.service;

import java.security.SecureRandom;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;


public final class MRClientUtils {
    public static final String ASSET_NID = "NID";
    public static final String STATE_XPATH = "//State/StateFlag";
    public static final String ASSET_STATE_UNMANAGED = "Unmanaged";
    public static final String ASSET_STATE_TEMPLATE = "Template";
    public static final String ASSET_STATE_OFFLINE = "Offline";
    public static final String ASSET_STATE_READY = "Ready";
    public static final String ASSET_STATE_NOT_READY = "NotReady"; 
    public static final String ASSET_STATE_NEEDS_UPGRADE = "NeedsUpgrade";
    public static final String ASSET_STATE_IDLE = "Idle";
    public static final String ASSET_STATE_RESERVED = "Reserved";
    public static final String ASSET_STATE_BUSY = "Busy";
    public static final String ASSET_STATE_ERROR = "Error";
    public static final String ASSET_PRIVATEDATA_XPATH = "//AssetPrivateData";
    public static final String ASSET_PRIVATEKEYDATA_XPATH = "//AssetPrivateKey/KeyData";
    public static final String TITLE_XPATH = "//Title";
    public static final String GROUP_XPATH = "//Groups";
    public static final String ASSET_TYPE_XPATH = "//Type";    
    public static final String ASSET_RESOURCE_NID_XPATH = "//AssetResourceNID";
    public static final String ASSET_TEMPLATE_NID_XPATH = "//TemplateAssetResourceNID";
    public static final String PORT_NID_VALUE_XPATH = "//MediaStreamDstPortResourceNID/NIDValue";
    public static final String ASSET_XML_PATH = "AssetResource_BASE_ALL_ADD.xml";
    public static final String MEDIA_STREAM_DST_PORT_XML_PATH = "MediaStreamDstPortResource_BASE_ALL_ADD.xml";
    public static final String IPVS_DEVICE_XML_PATH = "IPVSClient-Device_BASE_ALL_ADD.xml";
    public static final String ASSET_TEMPLATE = "TEMPLATE";
    public static final String PERSISTENCE = "Persistence";
    public static final String TEMPORARY = "Temporary";


   /* private static String getAssetRootNID(MRLog log, MRDB db) throws Exception {
        MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.AssetResource, log);

        return db.getServiceDomainRootNID(config.getParentNIDKey(log));
    }*/

    public static Document readAssetResourceInternal(String userJID, MRLog log, MRDB db, String title) {
        try {
            MRRequest request = MRRequest.createRequest(userJID);
            return DBServiceUtils.getAssetResource(title, request, log, db).getDocument();
        } catch (Exception e) {
            return null;
        }
    }

    private static void updateAssetResourceInternal(String userJID, MRLog log, MRDB db, String title, Document assetResource)
        throws Exception {
        log.addInfo("MRClientUtils:=" + userJID + ":updateAsset title=" + title, MRLog.OK, MRLog.NOTIFY);

        MRRequest request = MRRequest.createRequest(userJID);
        
        String assetResourceNID = MRXMLUtils.getAttributeValue(assetResource.getRootElement(), MRClientUtils.ASSET_NID,"");
        if(!"".equals(assetResourceNID)){
	        MRXMLDBUtils.updateElement(request, log, db, assetResourceNID, assetResource.getRootElement());	
	        // Document doc = MRClientUtils.readAssetResourceInternal(userJID, log, db, title);
	        // System.out.println(MRXMLUtils.documentToString(doc));
        } else{
        	throw new MRException(MRExceptionCodes.ASSET_RESOURCE_NOTAVAILABLE,"Failed To Update Asset Template in DB");
        }
    }
    
    public static boolean deleteTemporaryResource(Document assetResourceDoc,MRRequest request,XMPPI xmpp,MRDB db,MRLog log) throws MRException,Exception{

    	String persistance = MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(),PERSISTENCE);
    	String assetResourceTemplateNID = MRXMLUtils.getValueXpath(assetResourceDoc, ASSET_TEMPLATE_NID_XPATH, "");
        
    	if(!persistance.equals(TEMPORARY) || assetResourceTemplateNID.isEmpty() || assetResourceTemplateNID.equals(ASSET_TEMPLATE)){
    		return false;
    	}
    	log.addInfo("Delete Temporary Resource From Database. ResourceTitle:="+MRXMLUtils.getValueXpath(assetResourceDoc, TITLE_XPATH, ""));

    	Element nidEL = null;
    	String portResourceNID = null;
    	String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), AMConst.NID,"");
    	Document deviceDoc = AssetAdminServiceUtils.getDeviceResourcePointers(assetResourceNID, request, db, log, xmpp); 
    	//ServiceAgentUtils.getDeviceDoc(assetResourceDoc, request.getUserJID(), db, log);
    	List<?> listOfPorts = AssetAdminServiceUtils.getListOfPorts(deviceDoc,assetResourceNID,request,log,db);
    	if(deviceDoc != null) {
    		String deviceNID = MRXMLUtils.getAttributeValue(deviceDoc.getRootElement(),AMConst.NID,"");
    		if(!deviceNID.isEmpty() ) {
    			MRXMLDBUtils.deleteElement(request, log, db, deviceNID);
    		} 
    	}
    	if(listOfPorts!=null && !listOfPorts.isEmpty() ) {

    		for (Object object : listOfPorts) {
    			nidEL = (Element) object;
    			portResourceNID = MRXMLUtils.getValue(nidEL);

    			if (portResourceNID == null) {
    				continue;
    			}

    			MRXMLDBUtils.deleteElement(request, log, db, portResourceNID);
    		}
    	}
    	
    	MRXMLDBUtils.deleteElement(request, log, db, assetResourceNID);
    	return true;
    }


    private static Document createAssetResourceFromTemplate(String userJID, MRLog log, MRDB db, String resourceTitle, String assetTemplateTitle, Element assetPrivateData, String assetPrivateKeyData,
    		XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    	log.addInfo("MRClientUtils:=" + userJID + ":createAsset title=" + resourceTitle, MRLog.OK, MRLog.NOTIFY);
        String adminUser = MRServer.ADMIN+"@";
        
    	//null check is work around right now.  ui is sending this value in the template. 
    	if (userJID.startsWith(adminUser) && (assetTemplateTitle == null || assetTemplateTitle.isEmpty() || assetTemplateTitle.equals("null"))) {
        	log.addInfo("Skip CreateAssetResourceFromTemplate for admin if admin not provided template information. assetTemplateTitle:="+assetTemplateTitle,MRLog.INFO,MRLog.NOTIFY);
        	return null;
        }
        
    	
    	Document assetResourceTemplateDoc = MRClientUtils.readAssetResourceInternal(userJID, log, db, assetTemplateTitle);
    	if (assetResourceTemplateDoc == null) {
    		throw new MRException(MRExceptionCodes.ASSET_TEMPLATE_NOTFOUND, assetTemplateTitle);
    	}    

    	String assetResourceTemplateNID = MRXMLUtils.getValueXpath(assetResourceTemplateDoc, ASSET_TEMPLATE_NID_XPATH, "");
    	if(!assetResourceTemplateNID.equals(ASSET_TEMPLATE)){
    		throw new MRException(MRExceptionCodes.ASSET_TEMPLATE_NOTFOUND, "Template Not Found");        	
    	}


    	//Create Asset resource, ports and devices from template documents 
    	Document assetResourceDoc = createAssetResourceTemplateinDB(assetResourceTemplateDoc,resourceTitle,assetPrivateKeyData,userJID,db,xmpp,log,resourceFactory,serviceAgentFactory);
    	Document templateDeviceDoc =ServiceAgentUtils.getDeviceDoc(assetResourceTemplateDoc, userJID, db, log);
    	String assetResouceNID= MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), AMConst.NID,"");
    	Hashtable<String,String> listOfPorts = createPortsInDB(resourceTitle,assetResouceNID,MRXMLUtils.getAttributeValue(assetResourceTemplateDoc.getRootElement(), "NID",""), templateDeviceDoc,userJID,db,xmpp,log,resourceFactory,serviceAgentFactory);
    	if(templateDeviceDoc != null) {
    		createDeviceInDB(assetResouceNID,listOfPorts,templateDeviceDoc,userJID,db,xmpp,log,resourceFactory,serviceAgentFactory);
    	}
    	return assetResourceDoc;
    }

    private static Document createDeviceInDB(String assetResourceNID, Hashtable<String,String> listOfPorts,Document templateDeviceDoc, String userJID, MRDB db, XMPPI xmpp,
    		MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

    	String devicetype = templateDeviceDoc.getRootElement().getName();
    	Document deviceDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, devicetype, log);
    	MRXMLUtils.replaceContent(deviceDoc.getRootElement(), templateDeviceDoc.getRootElement());
    	MRRequest request = MRRequest.createRequest(userJID);    	
    	MRXMLUtils.setValueXpath(deviceDoc, ASSET_RESOURCE_NID_XPATH, assetResourceNID);
    	if(listOfPorts != null && !listOfPorts.isEmpty() ) {
    		Set<String> portTypeList = listOfPorts.keySet();    	
    		for(String portType: portTypeList) {
    			MRXMLUtils.setValueXpath(deviceDoc, "//"+portType+"NID"+"/NIDValue", listOfPorts.get(portType));    		
    		}
    	}
    	Document responseXML = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  ServiceClient.ASSETADMINSERVICE, IPVS_DEVICE_XML_PATH, null,
    			null, (Element) deviceDoc.getRootElement().clone(), "Add");

    	return  MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(responseXML, "//" + devicetype));
    }

    private static Hashtable<String,String> createPortsInDB(String resourceTitle,String assetResourceNID,String templateAssetResourceNID,Document templateDeviceDoc, String userJID, MRDB db, XMPPI xmpp, MRLog log,
    		MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {

    	List<?> listOfTemplatePorts = AssetAdminServiceUtils.getListOfPorts(templateDeviceDoc,templateAssetResourceNID,MRRequest.createRequest(resourceTitle),log,db);
    	System.out.println("templateAssetResourceNID:="+templateAssetResourceNID);
    	
    	System.out.println("listOfTemplatePorts:="+listOfTemplatePorts);
    	if(listOfTemplatePorts ==null || listOfTemplatePorts.isEmpty()){
    		return null;
    	}

    	Hashtable<String,String> listOfPorts = new Hashtable<String, String>();
    	Document templatePortDoc = null;
    	Document portDoc = null;
    	String portType = null;
    	MRRequest request = MRRequest.createRequest(userJID);
    	for (Object object : listOfTemplatePorts) {
    		templatePortDoc = MRXMLDBUtils.readElement(request, log, db, ((Element)object).getText());
    		portType = templatePortDoc.getRootElement().getName();	    
    		portDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, portType, log);
    		MRXMLUtils.replaceContent(portDoc.getRootElement(), templatePortDoc.getRootElement());
    		MRXMLUtils.setValueXpath(portDoc, TITLE_XPATH, resourceTitle);
    		MRXMLUtils.setValueXpath(portDoc, ASSET_RESOURCE_NID_XPATH, assetResourceNID);        		
    		Document responseXML = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  ServiceClient.ASSETADMINSERVICE, portType+"_BASE_ALL_ADD.xml",null,
    				null, (Element) portDoc.getRootElement().clone(), "Add");
    		listOfPorts.put(portType,MRXMLUtils.getAttributeValueXpath(responseXML, "//"+portType,AMConst.NID,""));            
    	}  

    	return listOfPorts ;

    }

    private static Document createAssetResourceTemplateinDB(Document templateAssetResourceDoc,String resourceTitle,String assetPrivateKeyData,
    		String userJID, MRDB db, XMPPI xmpp, MRLog log,MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
    	Document assetResourceDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.AssetResource, log);        
    	MRRequest request = MRRequest.createRequest(userJID);       
    	MRXMLUtils.replaceContent(assetResourceDoc.getRootElement(), templateAssetResourceDoc.getRootElement());
    	MRXMLUtils.setValueXpath(assetResourceDoc,ASSET_TEMPLATE_NID_XPATH, MRXMLUtils.getAttributeValue(templateAssetResourceDoc.getRootElement(), AMConst.NID));
    	MRXMLUtils.setValueXpath(assetResourceDoc, TITLE_XPATH, resourceTitle);
    	MRXMLUtils.setAttributeValue(assetResourceDoc.getRootElement(), PERSISTENCE,TEMPORARY);
    	MRXMLUtils.setValueXpath(assetResourceDoc, ASSET_PRIVATEKEYDATA_XPATH, assetPrivateKeyData);
    	Document responseXML = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.ASSETADMINSERVICE, ASSET_XML_PATH, null,
    			null, (Element) assetResourceDoc.getRootElement().clone(), "Add");

    	return MRXMLUtils.elementToDocument(MRXMLUtils.getElementXpath(responseXML, "//" + AMConst.AssetResource));

    }

	public static Document loginAssetResource(String userJID, MRLog log, MRDB db, String resourceTitle, String assetTemplateTitle, Element assetPrivateData, String assetPrivateKeyData, XMPPI xmpp,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document assetResource = MRClientUtils.readAssetResourceInternal(userJID, log, db, resourceTitle);
        if(assetResource != null) {
        	String stateFlag = MRXMLUtils.getValueXpath(assetResource, "//StateFlag", "");
        	if(stateFlag.equals(AMConst.ASSETSTATE_READY)) {

        		throw new MRException(MRExceptionCodes.DEVICE_ALREADY_IN_USE, "Device is already in use.");
        	}
        }
        
        String adminUser = MRServer.ADMIN+"@";        

        if (assetResource == null) {
            return MRClientUtils.createAssetResourceFromTemplate(userJID, log, db, resourceTitle, assetTemplateTitle, assetPrivateData, assetPrivateKeyData, xmpp, resourceFactory, serviceAgentFactory);
        }
        
        
        if (userJID.startsWith(adminUser)) {
        	log.addInfo("Skip Asset Resource Check for Admin",MRLog.INFO,MRLog.NOTIFY);
        	return assetResource;
        }
        
        String assetResourceTemplateNID = MRXMLUtils.getValueXpath(assetResource, ASSET_TEMPLATE_NID_XPATH, "");
    	if(assetResourceTemplateNID.equals(ASSET_TEMPLATE)){
    		throw new MRException(MRExceptionCodes.CAN_NOT_LOGIN_USING_TEMPLATE, "Can not login using template");        	
    	}

        log.addInfo("MRClientUtils:=" + userJID + ":loginAsset title=" + resourceTitle, MRLog.OK, MRLog.NOTIFY);
        
        // Set the assetPrivateKeyData if required
        String tAssetPrivateKeyData = MRXMLUtils.getValueXpath(assetResource, ASSET_PRIVATEKEYDATA_XPATH, "");
        tAssetPrivateKeyData = tAssetPrivateKeyData.trim();
       
        if ("".equals(tAssetPrivateKeyData)) {
            MRXMLUtils.setValueXpath(assetResource, MRClientUtils.ASSET_PRIVATEKEYDATA_XPATH, assetPrivateKeyData);
        } else if (!assetPrivateKeyData.equals(tAssetPrivateKeyData)) {        	
        	System.out.println("Mismatch db='" + tAssetPrivateKeyData + "' passed='" + assetPrivateKeyData + "'");
            throw new MRException(MRExceptionCodes.ASSET_RESOURCE_ACCESSDENIED, resourceTitle +
            		" check db=" + tAssetPrivateKeyData + " != key=" + assetPrivateKeyData);
        }
        
        //update api key and shared secret 
        String apikey = ""+ new SecureRandom().nextLong();
        String secret = ""+ new SecureRandom().nextLong();
        Element apiKeyElement = MRXMLUtils.getElementXpath(assetResource, "//"+SessionServiceResourceConst.API_KEY_ELEMENT_NAME);
        MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.KEY_ATTRIBUTE, apikey);
        MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.SHARED_SECRET_ATTRIBUTE, secret);
        MRXMLUtils.setAttributeValue(apiKeyElement, SessionServiceResourceConst.VALID_INTERVAL_ATTRIBUTE,String.valueOf(SessionServiceResourceConst.API_KEY_VALID_INTERVAL));        
        
        MRClientUtils.updateAssetResourceInternal(userJID, log, db, resourceTitle, assetResource);

        return assetResource;
    }

    public static void logoutAssetResource(String userJID, MRLog log, MRDB db, String resourceTitle)
        throws Exception {
        log.addInfo("MRClientUtils:=" + userJID + ":logoutAsset title=" + resourceTitle, MRLog.OK, MRLog.NOTIFY);

        /* Management server taking care of this
        Document assetResource = MRClientUtils.readAssetResourceInternal(userJID, log, db, resourceTitle);
        if(assetResource==null){
        	return;
        }
        MRXMLUtils.setValueXpath(assetResource, MRClientUtils.ASSET_STATE_XPATH, MRClientUtils.ASSET_STATE_OFFLINE);
        MRClientUtils.updateAssetResourceInternal(userJID, log, db, resourceTitle, assetResource);
        */
    }
    
    public static String getLinkLocalRequest(String userName,String clientKey,String resource,String node, boolean runDiscoveryService)
    
    {
    	  return "<LinkLocalLogin><userName>"+userName+"</userName><clientKey>"+clientKey+"</clientKey><resource>"+
  	    resource+"</resource><node>"+
  	    node+"</node><runDiscoveryService>"+
  	    String.valueOf(runDiscoveryService)+"</runDiscoveryService></LinkLocalLogin>";
    }
    
    public static boolean isLinkLocalJID(String userJID){
    	return StringUtils.contains(userJID, MRConst.LINK_LOCAL_DOMAIN);
    }
}
