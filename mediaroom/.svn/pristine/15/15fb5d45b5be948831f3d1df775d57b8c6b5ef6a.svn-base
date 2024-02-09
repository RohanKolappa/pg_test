package com.ipvs.systemservice.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRServer;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.FileUtils;


public class SetServiceDomainRequestHandler extends BaseLoginRequestHandler implements MRRequestHandlerI {
    private static final String SERVICE_VERSION_FILE_NAME = "service.version";
    public static final String SERVICE_VERSION_FILE_PATH = "/com/ipvs/xsd/objectXml/" + SERVICE_VERSION_FILE_NAME;
    
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws Exception {
        int authType = MediaRoomObject.ROOM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);

        return auth;
    }

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
       	
    
    	// Enforce max number of clients
        enforceMaxMediaClients(request, resourceFactory, serviceAgentFactory, log, db, xmpp);
        
        //subscribe for user presence
        subscribeForUserPresence(request, xmpp, log);
        
        String clientServiceVersion = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//" + MRConst.SERVICE_VERSION, "0");        
        String clientSoftwareVersion = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//" + MRConst.SOFTWARE_VERSION, "0");
		
        //check the schema version of client and server
    	checkSchemaVersion(request,db, log, xmpp, resourceFactory, serviceAgentFactory, clientServiceVersion, clientSoftwareVersion);
    	
        
        Document assetResourceDoc = null;
     
        //Service login 
        assetResourceDoc = serviceLogin(request, resourceFactory, serviceAgentFactory, log, db, xmpp);

        

        Document deviceDoc = null;
        Element portResourceNIDListEL = null;
        if(IPVSConst.getInstance().isComponentLogin() && request.getUserJID().indexOf("@") == -1 ){
           return createResponseDocument(assetResourceDoc, portResourceNIDListEL, deviceDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        }
        if (assetResourceDoc != null) {
        	String assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), AMConst.NID,"");
        	
            deviceDoc = AssetAdminServiceUtils.getDeviceResourcePointers(assetResourceNID, request, db, log, xmpp);
            
            //updateResource contact and Presence
            portResourceNIDListEL = updateResourceContact(assetResourceDoc, deviceDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
      
            //update IPAddress in device document 
            updateEthernetPortIPAddress(deviceDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            
           
            //update software version in device document 
            
            updateSoftwareVersion(deviceDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory, clientSoftwareVersion, clientServiceVersion);
            
        	RoomDB.getRoomDB().handleAssetResourceNIDAvailable(xmpp, log, assetResourceNID, request.getUserJID());
        }
        
        return createResponseDocument(assetResourceDoc, portResourceNIDListEL, deviceDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }    
       
    private Document createResponseDocument(Document assetResourceDoc, Element portResourceNIDListEL, Document deviceDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Element schemaElement = MRXMLDBUtils.getMRSchema(db, log, MRConst.MRSCHEMAPATH);

        if (schemaElement == null) {
            schemaElement = MRXMLUtils.stringToElement("</ServiceRoot>");
        }

        Document responseDoc = request.createResponseData(schemaElement);
        Element responseDocRootEL = responseDoc.getRootElement();

        if (assetResourceDoc == null) {
            assetResourceDoc = MRXMLUtils.stringToDocument("<" + AMConst.AssetResource + "/>");
        }

        MRXMLUtils.addElement(responseDocRootEL, (Element)assetResourceDoc.getRootElement().detach());

        if (portResourceNIDListEL == null) {
            portResourceNIDListEL = MRXMLUtils.newElement("PortResourceNIDList");
        }

        MRXMLUtils.addElement(responseDocRootEL, (Element) portResourceNIDListEL.clone());

        Element deviceNIDEL = MRXMLUtils.newElement("deviceNID");

        if (deviceDoc != null) {
            MRXMLUtils.setValue(deviceNIDEL, MRXMLUtils.getAttributeValue(deviceDoc.getRootElement(), "NID", ""));
        }

        MRXMLUtils.addElement(responseDocRootEL, (Element) deviceNIDEL.clone());

        Element appServerAgentJIDElement = MRXMLUtils.stringToElement("<appServerAgentJID/>");
        appServerAgentJIDElement.setText(AppServerAgentUtils.getAppServerAgentJID(request.getUserJID(),serviceAgentFactory));
        MRXMLUtils.addElement(responseDocRootEL, (Element) appServerAgentJIDElement.clone());
        String version = MRXMLUtils.getValueXpath(SystemServiceUtils.getSoftwareImageStatus(log), "//Parameter[@name='Version']", "");
        Element softwareVersionElement = MRXMLUtils.addElement("softwareVersion", version);
        MRXMLUtils.addElement(responseDocRootEL, softwareVersionElement);

        //add NGSD200 device software image version if present.
        String deviceSoftwareImageVersion = SystemServiceUtils.getDeviceSoftwareVersionOnMS(SystemServiceConst.NSGD200_DEVICE, log);
        Element deviceSWVersionEl = MRXMLUtils.addElement(responseDocRootEL, SystemServiceConst.DEVICE_SOFTWARE_VERSION, deviceSoftwareImageVersion);
        MRXMLUtils.addAttribute(deviceSWVersionEl, "deviceType", SystemServiceConst.NSGD200_DEVICE);

		return responseDoc;
    }
    

    private Element updateResourceContact(Document assetResourceDoc, Document deviceDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String assetResourceNID = null;

        if (assetResourceDoc != null) {
            assetResourceNID = MRXMLUtils.getAttributeValue(assetResourceDoc.getRootElement(), "NID", null);
        }

        if ((assetResourceNID == null) && !request.getUserJID().startsWith(MRServer.ADMIN)) {
            throw new MRException(MRExceptionCodes.NODENOTFOUND, "Asset ResourceNode not found");
        } else if (assetResourceNID == null) {
            //if the user is admin just send empty list.
            return null;
        }
        
        List<?> listOfPorts = AssetAdminServiceUtils.getListOfPorts(deviceDoc, assetResourceNID, request, log, db);
        
        String assetState = MRClientUtils.ASSET_STATE_NOT_READY;
        String deviceType = MRXMLUtils.getValueXpath(assetResourceDoc, "//"+AMConst.TYPE, "");
        //don't set ms device as ready. 
        if((listOfPorts == null || listOfPorts.isEmpty()) && !(deviceType.equals(AMConst.MS_DEVICE_OBJECTYPE) || deviceType.equals(AMConst.MS_25_DEVICE_OBJECTYPE))) {
        	log.addInfo("No Ports found in db. Set the Asset State to Ready");
        	assetState = MRClientUtils.ASSET_STATE_READY;
        }
        
        if(deviceType.equals(AMConst.WEB_CLIENT_DEVICE)) {
        	assetState = MRClientUtils.ASSET_STATE_READY;
        }
        
        
       // String assetResourceType = MRXMLUtils.getValueXpath(assetResourceDoc, "//Type", "");
    	Element userListEl = MRXMLUtils.getElementXpath(assetResourceDoc, "//UserList");

        if(deviceType.equals(AMConst.IPVS_CLIENT_DEVICE_OBJECTYPE)){
    			// Add user
    			Document requestDoc = request.getMessage().getDocument();
    			String appName = MRXMLUtils.getValueXpath(requestDoc, "//AppName", "");

    			Element userEL = MRXMLUtils.newElement("User");
    			MRXMLUtils.addAttribute(userEL, "appName", appName);
    			MRXMLUtils.addAttribute(userEL, "jid", request.getUserJID());
    			if(userListEl != null) {
    				//remove this step when you are allowing multiple login
    				MRXMLUtils.removeContent(userListEl);
    				MRXMLUtils.addElement(userListEl,userEL);
    			}

    	}
        
        AppServerAgentUtils.getResourcePresenceManager(serviceAgentFactory).updateContactAndState(assetResourceNID, request.getUserJID(), AMConst.AVAILABLE, userListEl, assetState, log, db, xmpp, resourceFactory, serviceAgentFactory);
      
        if ((listOfPorts == null) || listOfPorts.isEmpty()) {
            return null;
        }

        Element portResourceNIDListEL = MRXMLUtils.newElement("PortResourceNIDList");

        Element nidEL = null;
        String portResourceNID = null;       
        String portElName ="";
        String portType = "";

        for (Object object : listOfPorts) {
            nidEL = (Element) object;
            portResourceNID = MRXMLUtils.getValue(nidEL);
            
            if (portResourceNID == null) {
                continue;
            }
            
            portElName =nidEL.getParent().getName();
            portType = AssetAdminServiceUtils.getPortResourceTypeFromDevice(portElName);         

            portResourceNIDListEL.add(MRXMLUtils.stringToElement("<resourceNID type=\"" + portType + "\">" + portResourceNID + "</resourceNID>"));
            if(portType.equals(AMConst.MEDIA_STREAM_RELAY_PORT_RESOURCE_OBJECTTYPE)){
         		StreamPolicyUtils.handlePolicyChanged();
         	}
            
            AppServerAgentUtils.getResourcePresenceManager(serviceAgentFactory).updateContactAndState( portResourceNID, request.getUserJID(), AMConst.AVAILABLE, null, (deviceType.equals(AMConst.WEB_CLIENT_DEVICE)?MRClientUtils.ASSET_STATE_READY:MRClientUtils.ASSET_STATE_NOT_READY), log, db, xmpp, resourceFactory, serviceAgentFactory);
            
            //AssetAdminServiceUtils.updateContactAndState(resourceDoc, portResourceNID, request.getUserJID(), AMConst.AVAILABLE, MRClientUtils.ASSET_STATE_NOT_READY, log, db);
        }

        return portResourceNIDListEL;
    }   
    
    private void enforceMaxMediaClients(MRRequest request, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRLog log, MRDB db, XMPPI xmpp) throws MRException, Exception{
		int availableLogins = 0;
    	String userJID = request.getUserJID();
    	String resourceTitle = ServiceAgentUtils.getAgentName(userJID);
		Document assetResource = MRClientUtils.readAssetResourceInternal(userJID, log, db, resourceTitle);
		String assetResourceType = MRXMLUtils.getValueXpath(assetResource, "//Type", "");
		String appName = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//AppName", "");
		if(assetResourceType.equals(AMConst.IPVS_CLIENT_DEVICE_OBJECTYPE) && !appName.equals(AMConst.IPVSAdmin)) {
			request.setSuperUser(true);
			String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.GET_AVAILABLE_CLIENTS + SessionServiceConst.XQUERYXML;		
			String query = FileUtils.readFileResource(xqueryFileName, request.getClass());
			Document queryResultDoc = GetObjectUtils.executeQuery(query, request, xmpp, log, db, serviceAgentFactory, resourceFactory);
			
			try {
				availableLogins = Integer.parseInt(MRXMLUtils.getValueXpath(queryResultDoc, "//AvailableLogins", ""));
				if(availableLogins <= 0)
					throw new MRException(MRExceptionCodes.MAX_MEDIA_CLIENTS_EXCEEDED, "The maximum number of Media Client logins has been exceeded.");
			}
			catch(NumberFormatException e) {
				System.err.println("Unable to parse Available Logins value: " + availableLogins);
			}
		}
    }
}
