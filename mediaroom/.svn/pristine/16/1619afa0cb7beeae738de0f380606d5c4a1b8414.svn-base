package com.ipvs.systemservice.handler;

import java.util.Date;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants.DeviceTypeList;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants.DisplayTypeList;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants.PolicyObjectList;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants.StreamTypeList;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.AddDeviceRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;


public class SetServicePolicyRequestHandler implements MRRequestHandlerI {

	
	
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        log.addTraceBegin(request);
        int authType = 1;
        int role = 2;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);
        return auth;

    }
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
            MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
            throws Exception {
		
			try
			{				
				Document requestDoc = request.getMessage().getDocument();				
				Element setServicePolicyEL = MRXMLUtils.getElementXpath(requestDoc, request.getRequestDataXPath()); 
		
				String type = MRXMLUtils.getValue(setServicePolicyEL, PolicyAdminConstants.TYPE, "");     
				String bandwidth = MRXMLUtils.getValue(setServicePolicyEL, AMConst.BANDWIDTH, "");
				String relay = MRXMLUtils.getValue(setServicePolicyEL, PolicyAdminConstants.RELAY, "");
				
				// Get the NID of the TagObjectFilter		
				Element tagObjectALLFilterEL = getMatchingElement(request, log, db, ServiceClient.POLICYADMINSERVICE, PolicyAdminConstants.TAG_OBJECT_FILTER_OBJECTYPE, "", PolicyAdminConstants.NAME, PolicyAdminConstants.ALL_FILTER);		
				Element objectFilterEL = getMatchingElement(request, log, db, ServiceClient.POLICYADMINSERVICE, PolicyAdminConstants.OBJECT_FILTER_OBJECTYPE, "", PolicyAdminConstants.NAME, PolicyAdminConstants.OBJECT_FILTER_NAME);
				String tagObjectALLFilterNID = MRXMLUtils.getAttributeValue(tagObjectALLFilterEL, PolicyAdminConstants.NID, "");
				String objectFilterNID = MRXMLUtils.getAttributeValue(objectFilterEL, PolicyAdminConstants.NID, "");
								
				String roleObjectArr[] = null;
				String roleActionArr[] = null;										
								
				// List of Policies				
				for (PolicyObjectList policyObjectList : PolicyObjectList.values()) {
					 roleActionArr = new String[]{PolicyAdminConstants.ADD_ROLE_ACTION, PolicyAdminConstants.DELETE_ROLE_ACTION, PolicyAdminConstants.READ_ROLE_ACTION, PolicyAdminConstants.MODIFY_ROLE_ACTION};										
					 
					String whereRoleXPath = "";
					
					switch(policyObjectList)
					{
						case UserAdmin:	
							roleObjectArr = new String[]{PolicyAdminConstants.USER};
							break;
						case AssetAdmin:
							roleObjectArr = new String[]{PolicyAdminConstants.ASSET_RESOURCE_OBJECT, PolicyAdminConstants.DEVICE_OBJECT, PolicyAdminConstants.MEDIA_STREAM_SRC_PORT_RESOURCE,
									PolicyAdminConstants.MEDIA_STREAM_DST_PORT_RESOURCE,PolicyAdminConstants.MEDIA_STREAM_RELAY_PORT_RESOURCE_OBJECT,
									PolicyAdminConstants.MEDIA_STREAMPROFILE_OBJECT, PolicyAdminConstants.MEDIA_STORE_PORT_RESOURCE_OBJECT, 
									PolicyAdminConstants.DISPLAY_PROFILE_OBJECT};
							break;
						case MediaStoreAdmin:
							roleObjectArr = new String[]{PolicyAdminConstants.MEDIA_STREAM_FILE_RESOURCE, PolicyAdminConstants.MEDIA_STREAM_DIR_RESOURCE, PolicyAdminConstants.EXPORT_IMPORT_FILE_LIST_ITEM};
							roleActionArr = new String[]{PolicyAdminConstants.ADD_ROLE_ACTION, PolicyAdminConstants.DELETE_ROLE_ACTION, PolicyAdminConstants.READ_ROLE_ACTION, 
									 PolicyAdminConstants.MODIFY_ROLE_ACTION, PolicyAdminConstants.IMPORT_ROLE_ACTION,PolicyAdminConstants.EXPORT_ROLE_ACTION};									
								
							break;
						case MediaRoom:
							roleObjectArr = new String[]{PolicyAdminConstants.MEDIA_ROOM_OBJECT};
							roleActionArr = new String[]{PolicyAdminConstants.VIEW_ROLE_ACTION, PolicyAdminConstants.PLAYBACK_ROLE_ACTION,PolicyAdminConstants.RECORD_ROLE_ACTION, PolicyAdminConstants.PVR_ROLE_ACTION};
							break;
						case MediaStream:
							roleObjectArr = null;
							roleActionArr = null;
							break;
						case ViewFromOrStreamToPorts:	
							roleObjectArr = new String[]{PolicyAdminConstants.MEDIA_STREAM_SRC_PORT_RESOURCE,PolicyAdminConstants.MEDIA_STREAM_DST_PORT_RESOURCE};
							roleActionArr = new String[]{PolicyAdminConstants.VIEW_FROM_ROLE_ACTION,PolicyAdminConstants.STREAM_TO_ROLE_ACTION, PolicyAdminConstants.READ_ROLE_ACTION};							
							whereRoleXPath = "((//" + PolicyAdminConstants.ROLE_ACTION + "='" + PolicyAdminConstants.VIEW_FROM_ROLE_ACTION + "')" + " and " + 					        			 
					        			 "(//" + PolicyAdminConstants.ROLE_ACTION + "='" + PolicyAdminConstants.STREAM_TO_ROLE_ACTION + "'))";					        			
							break;
						case PlaybackFiles:	
							roleObjectArr = new String[]{PolicyAdminConstants.MEDIA_STREAM_FILE_RESOURCE,PolicyAdminConstants.MEDIA_STREAM_DIR_RESOURCE};
							roleActionArr = new String[]{PolicyAdminConstants.PLAYBACK_ROLE_ACTION, PolicyAdminConstants.READ_ROLE_ACTION};
							whereRoleXPath = "(//" + PolicyAdminConstants.ROLE_ACTION + "='" + PolicyAdminConstants.PLAYBACK_ROLE_ACTION + "')";
							break;
						case RecordOrPVRToDirectories:	
							roleObjectArr = new String[]{PolicyAdminConstants.MEDIA_STREAM_DIR_RESOURCE};
							roleActionArr = new String[]{PolicyAdminConstants.RECORD_ROLE_ACTION, PolicyAdminConstants.PVR_ROLE_ACTION, PolicyAdminConstants.READ_ROLE_ACTION};
							whereRoleXPath = "((//" + PolicyAdminConstants.ROLE_ACTION + "='" + PolicyAdminConstants.RECORD_ROLE_ACTION + "')" + " and " + 					        			 
		        			 			 "(//" + PolicyAdminConstants.ROLE_ACTION + "='" + PolicyAdminConstants.PVR_ROLE_ACTION + "'))";
							break;
						case ConfigureSessions:	
							roleObjectArr = new String[]{PolicyAdminConstants.SESSION_CONFIG_OBJECT};
							roleActionArr = new String[]{PolicyAdminConstants.ADD_ROLE_ACTION, PolicyAdminConstants.DELETE_ROLE_ACTION, 
									 PolicyAdminConstants.READ_ROLE_ACTION, PolicyAdminConstants.MODIFY_ROLE_ACTION, PolicyAdminConstants.START_ROLE_ACTION};										
							whereRoleXPath = "(//" + PolicyAdminConstants.ROLE_OBJECT + "='" + PolicyAdminConstants.SESSION_CONFIG_OBJECT + "')";				 			 			 
							break;
						case PolicyAdmin: 
							roleObjectArr = new String[]{PolicyAdminConstants.GROUPTAGDEF_OBJECTTYPE, PolicyAdminConstants.FILTER_OBJECT_TYPE, PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE,
									PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE,PolicyAdminConstants.USER_ADMIN_POLICY_OBJECTTYPE,
									PolicyAdminConstants.POLICY_ADMIN_POLICY_OBJECTTYPE, PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE, PolicyAdminConstants.MEDIA_ROOM_POLICY_OBJECTTYPE,
									PolicyAdminConstants.SYSTEM_OBJECT, PolicyAdminConstants.REPORT_OBJECTTYPE, PolicyAdminConstants.NETWORK_ADDRESS_RULE};
							roleActionArr = new String[]{PolicyAdminConstants.ADD_ROLE_ACTION, PolicyAdminConstants.DELETE_ROLE_ACTION, 
									PolicyAdminConstants.READ_ROLE_ACTION, PolicyAdminConstants.MODIFY_ROLE_ACTION,
									PolicyAdminConstants.UPGRADE_ROLE_ACTION,PolicyAdminConstants.RESTORE_ROLE_ACTION,
									PolicyAdminConstants.BACKUP_ROLE_ACTION,PolicyAdminConstants.SESSION_MANAGEMENT_ROLE_ACTION};
							break;													
					}
					addPoliciesInDB(requestDoc, request, xmpp, log, db, type, relay, policyObjectList, roleObjectArr, roleActionArr, whereRoleXPath, tagObjectALLFilterNID,objectFilterNID);
				}
					
				// Add user
				addUserInDB(requestDoc, request, xmpp, log, db, PolicyAdminConstants.USER);
				
				//List of Clients
				for (DeviceTypeList deviceTypeList : DeviceTypeList.values()) {					
					addDeviceInDB(requestDoc, request, xmpp, log, db,resourceFactory, serviceAgentFactory, PolicyAdminConstants.ASSET_RESOURCE_OBJECT, deviceTypeList);
				}
				
				// List of Display Profiles
				for (DisplayTypeList displayTypeList : DisplayTypeList.values()) {					
					addDisplayProfilesInDB(requestDoc, request, xmpp, log, db, PolicyAdminConstants.DISPLAY_PROFILE_OBJECT, displayTypeList);
				}
				
				// List of Profiles
				for (StreamTypeList streamTypeList : StreamTypeList.values()) {					
					addStreamProfilesInDB(requestDoc, request, xmpp, log, db, bandwidth, streamTypeList.toString());
				}						
			}
			catch (Exception e)
			{
				e.printStackTrace();
				throw e;
			}
			
			Document responseDoc = request.createResponseData(MRXMLUtils.stringToElement("<SetServicePolicy/>"));
			
			return responseDoc;
			
        }
	
	private void addPoliciesInDB(Document requestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String type, String relay, PolicyObjectList objectType, String roleObjectArr[], String roleActionArr[], String whereRoleXPath, String tagObjectALLFilterNID,String objectFilterNID) throws Exception {
						
		String ruleName = null;
		String dbObjectName = null;
		Document policyMgmtObjectXMLDoc = null;
		
		String objectValue = objectType.toString();
		String policyName = PolicyAdminConstants.POLICY_TITLE;		
				
		switch (objectType)
		{
			case MediaStream:  
				ruleName = PolicyAdminConstants.NAME;
				dbObjectName = PolicyAdminConstants.MEDIA_STREAM_FILTER_OBJECTTYPE;				
				break;
			case ViewFromOrStreamToPorts:
			case ConfigureSessions:	
				ruleName = PolicyAdminConstants.RULE_NAME;
				dbObjectName = PolicyAdminConstants.ASSET_ADMIN_POLICY_OBJECTTYPE;
				objectValue = PolicyObjectList.AssetAdmin.toString();
				break;
			case PlaybackFiles:	
			case RecordOrPVRToDirectories:
				ruleName = PolicyAdminConstants.RULE_NAME;	
				dbObjectName = PolicyAdminConstants.MEDIA_STORE_ADMIN_POLICY_OBJECTTYPE;
				objectValue = PolicyObjectList.MediaStoreAdmin.toString();
				break;
			default:
				ruleName = PolicyAdminConstants.RULE_NAME;
				dbObjectName = objectValue + PolicyAdminConstants.POLICY_RULE_OBJECT_TYPE;
				break;
		}		
		
		Element objectEL = getMatchingElement(request, log, db, ServiceClient.POLICYADMINSERVICE, dbObjectName, whereRoleXPath, ruleName, policyName);	
		log.addInfo("Check Default Policy Exists in Db or not. dbObjectName :="+dbObjectName + " policyName :="+policyName,MRLog.INFO,MRLog.NOTIFY);		
		if (objectEL != null) {			
			return;
		}
		
		policyMgmtObjectXMLDoc = MRConfig.getObjectXMLDocument(ServiceClient.POLICYADMINSERVICE, dbObjectName, log);
		
		// Set rulename
		MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + ruleName, policyName);
		
		switch (objectType)
		{
			case MediaStream:  
				
				// Set the ProfileFilter			
				MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.PROFILE_FILTER + "/" + PolicyAdminConstants.STREAM_PROFILE_FILTER_NID, tagObjectALLFilterNID);
			
				if (relay.equalsIgnoreCase(PolicyAdminConstants.TRUE))
				{
					// Set the RelayFilter				
					MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.RELAY_FILTER + "/" + PolicyAdminConstants.USER_FILTER_NID, tagObjectALLFilterNID);
					MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" +  PolicyAdminConstants.RELAY_FILTER + "/" + PolicyAdminConstants.STREAM_RESOURCE_FILTER_NID, tagObjectALLFilterNID);
				} else {
					MRXMLUtils.removeContent(MRXMLUtils.getElementXpath(policyMgmtObjectXMLDoc,"//" + PolicyAdminConstants.RELAY_FILTER_SET));
				}
			
				// Set SrcFilterSet & DstFilterSet 			
				setFilterNID(policyMgmtObjectXMLDoc, tagObjectALLFilterNID);
				break;
				
			default: 
			
				// Set order, valid, allow parameters
				MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.ORDER, PolicyAdminConstants.ORDER_VALUE);				
				MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.VALID, PolicyAdminConstants.TRUE);
				MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.ALLOW, PolicyAdminConstants.TRUE);
			
				if (type.equalsIgnoreCase(PolicyAdminConstants.DENY_ALL))		
					MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.ALLOW, PolicyAdminConstants.FALSE);
			
				// Set the ServiceOperatorFilter 
				MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER_SET + "/" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER + "/" + PolicyAdminConstants.USER_FILTER_NID, tagObjectALLFilterNID);
				MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER_SET + "/" + PolicyAdminConstants.SERVICE_OPERATOR_FILTER + "/" + PolicyAdminConstants.RESOURCE_FILTER_NID, tagObjectALLFilterNID);			
				
				Element policyAdminObjectRoleEL = MRXMLUtils.getElementXpath(policyMgmtObjectXMLDoc, "//" + objectValue + PolicyAdminConstants.OBJECT_ROLE);				
				
				if (objectValue.equals(PolicyObjectList.PolicyAdmin.toString())) {
					MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + objectValue + PolicyAdminConstants.OBJECT_FILTER_NID, objectFilterNID);
				} else {
					MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + objectValue + PolicyAdminConstants.OBJECT_FILTER_NID, tagObjectALLFilterNID);
				}
			
				setRoles(policyMgmtObjectXMLDoc, (Element) policyAdminObjectRoleEL.detach(), roleObjectArr, roleActionArr, objectValue);
				
				// Set SrcFilterSet & DstFilterSet 
				if (objectType == PolicyObjectList.MediaRoom)
					setFilterNID(policyMgmtObjectXMLDoc, tagObjectALLFilterNID);
				break;
		}
		
		log.addInfo("Add Default Policy. dbObjectName :="+dbObjectName + " policyName :="+policyName,MRLog.INFO,MRLog.NOTIFY);
		
		AddObjectUtils.addObjectInDatabaseWithSuperuserPrivileges(request, xmpp, log, db, null, null, policyMgmtObjectXMLDoc, dbObjectName);
		
	}
	
	private void addStreamProfilesInDB(Document requestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String bandwidth, String objectType) throws Exception {
		
		String streamObjectType = objectType + AMConst.MEDIA_STREAM_PROFILE_OBJECTTYPE;
		
		Element objectEL = getMatchingElement(request, log, db, ServiceClient.ASSETADMINSERVICE, streamObjectType, "", AMConst.TITLE, objectType);
        if (objectEL != null) {
        	return;
        }
		
		Document streamProfileObjectXMLDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, streamObjectType, log);
				
		MRXMLUtils.setValueXpath(streamProfileObjectXMLDoc, "//" + AMConst.TITLE, objectType);
		MRXMLUtils.setValueXpath(streamProfileObjectXMLDoc, "//" + AMConst.DESCRIPTION, objectType);
		MRXMLUtils.setValueXpath(streamProfileObjectXMLDoc, "//" + AMConst.STREAM_TYPE, objectType);
		MRXMLUtils.setValueXpath(streamProfileObjectXMLDoc, "//" + AMConst.PRIORITY, "1");
		
		if (objectType.equals(StreamTypeList.V2D.toString())) {		
			MRXMLUtils.setValueXpath(streamProfileObjectXMLDoc, "//" + AMConst.BANDWIDTH, bandwidth);
			MRXMLUtils.setValueXpath(streamProfileObjectXMLDoc, "//" + AMConst.ENABLE_KBM, PolicyAdminConstants.TRUE);
			//MRXMLUtils.setValueXpath(streamProfileObjectXMLDoc, "//" + AMConst.IS_MULTICAST, PolicyAdminConstants.TRUE);
		}
		
		if (objectType.equals(StreamTypeList.RTP.toString())) {		
			MRXMLUtils.setValueXpath(streamProfileObjectXMLDoc, "//" + SessionServiceResourceConst.HLSCONFIG + "/" + AMConst.ENABLE, PolicyAdminConstants.TRUE);
		}
		
		AddObjectUtils.addObjectInDatabaseWithSuperuserPrivileges(request, xmpp, log, db, null, null, streamProfileObjectXMLDoc, streamObjectType);
		
	}
	
	private void addUserInDB(Document requestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String objectType) throws Exception {
		
		Element objectEL = getMatchingElement(request, log, db, ServiceClient.ASSETADMINSERVICE, objectType, "", SessionServiceConst.USERNAME, "barco");
        if (objectEL != null) {
        	return;
        }
		
		Document userObjectXMLDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, objectType, log);		
				
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.USERNAME, "barco");
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.PASSWORD_EL_NAME, "b4rc0,BCD");
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.FULLNAME, "barco");
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.EMAIL, "barco");
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.CREATION_DATE, new Date().toString());
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.MODIFIED_DATE, new Date().toString());
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.EXTERNAL_AUTHENTICATION, "No");
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.USER_PRIVATE_DATA, "<EndPointControllerData><EndPointTableConfig Configurable='true'></EndPointTableConfig><DefaultTag>All</DefaultTag><SaveLogin>true</SaveLogin></EndPointControllerData>");
		
		AddObjectUtils.addObjectInDatabaseWithSuperuserPrivileges(request, xmpp, log, db, null, null, userObjectXMLDoc, objectType);
		
	}
	
	public void addDeviceInDB(Document requestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String objectType, DeviceTypeList deviceList) throws Exception {
		String title = deviceList.getTitle();
    	MRConfig mrConfig = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, objectType, log);
        String parentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));              	            	    									
    
      	String whereXPath[] = {"//"+objectType+"/Info[Title='" + title + "']"};
    	Element assetResourceElement = MRXMLDBUtils.readElement(0, null, whereXPath, parentNID, null, request, log, db);   
    	
    	if(assetResourceElement != null) {
    		return;
    	}
    	
		Document addDeviceSampleRequestDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.ADD_DEVICE_REQUEST_XML, log);
		String requestName = MRRequest.getRequestName(AddDeviceRequestHandler.class.getName());
		
		Element addDeviceEL = MRXMLUtils.getElementXpath(addDeviceSampleRequestDoc,"//AddDevice");
		
		MRXMLUtils.setAttributeValue(addDeviceEL, SessionServiceConst.TITLE, title);
		MRXMLUtils.setAttributeValue(addDeviceEL, SessionServiceConst.TYPE_ATTRIBUTE, deviceList.getObjectType());
		MRXMLUtils.setAttributeValue(addDeviceEL, SessionServiceResourceConst.UNMANNED_ATTRIBUTE, deviceList.getUnManned());
		
		if(deviceList.getUnManned().equals(PolicyAdminConstants.TRUE))
			MRXMLUtils.setAttributeValue(addDeviceEL, SessionServiceResourceConst.PASSWORD_ATTRIBUTE, "b4rc0,BCD");
				
		Document addDeviceRequestDoc = SessionServiceUtils.getRequestXML(requestName, log,addDeviceEL);
		MRRequest addDeviceRequest = MRRequest.createRequest(request, addDeviceRequestDoc, requestName, ServiceClient.SESSIONSERVICE_RESOURCE);	
		addDeviceRequest.setSuperUser(true);
		MRRequestHandlerI addDeviceRequestHandler = new AddDeviceRequestHandler();
		addDeviceRequestHandler.getResponse(addDeviceRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);	
		
	}
	
	private void addDisplayProfilesInDB(Document requestDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String objectType, DisplayTypeList displayList) throws Exception {
		String title = displayList.getTitle();
		Element objectEL = getMatchingElement(request, log, db, ServiceClient.ASSETADMINSERVICE, objectType, "", SessionServiceResourceConst.TITLE, title);
		if (objectEL != null) {
			return;
		}
				
		Document diplayProfileObjectXMLDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, objectType, log);
		Element windowEl = null;
				
		MRXMLUtils.setValueXpath(diplayProfileObjectXMLDoc, "//" + SessionServiceResourceConst.TITLE, title);
		MRXMLUtils.setAttributeValueXPath(diplayProfileObjectXMLDoc, "//Canvas", "hres", displayList.getHres());
		MRXMLUtils.setAttributeValueXPath(diplayProfileObjectXMLDoc, "//Canvas", "vres", displayList.getVres());
				
		windowEl = (Element)MRXMLUtils.getElementXpath(diplayProfileObjectXMLDoc, "//Window").detach();
		MRXMLUtils.removeChildren(MRXMLUtils.getElementXpath(diplayProfileObjectXMLDoc, "//Canvas"), "Window");
		Element canvasEl = MRXMLUtils.getElement(diplayProfileObjectXMLDoc, "Canvas");
		        
		for(int i=1; i<=displayList.getNumWindows(); i++) {
			MRXMLUtils.setAttributeValue(windowEl, "id", "Win"+i);
		    MRXMLUtils.setAttributeValueXPath(windowEl, "./Properties", "vpos", displayList.getVpos(i));
			MRXMLUtils.setAttributeValueXPath(windowEl, "./Properties", "hpos", displayList.getHpos(i));
			MRXMLUtils.setAttributeValueXPath(windowEl, "./Properties", "vres", displayList.getVres(i));
			MRXMLUtils.setAttributeValueXPath(windowEl, "./Properties", "hres", displayList.getHres(i));
			MRXMLUtils.addElement(canvasEl, (Element)windowEl.clone());
		}
		AddObjectUtils.addObjectInDatabaseWithSuperuserPrivileges(request, xmpp, log, db, null, null, diplayProfileObjectXMLDoc, objectType);
				
	}
	
	private Element getMatchingElement(MRRequest request, MRLog log, MRDB db, String serviceType, String dbObjectName, String whereRoleXPath, String name, String value) throws Exception
	{
		MRConfig mrConfig = MRConfig.getInstance(serviceType, dbObjectName, log);
        String parentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        
        String whereTitleXPath = "(//" + dbObjectName + "[" + name + "='" + value + "'])";
        String whereXPathArr[] = { whereTitleXPath };        
        if (!whereRoleXPath.isEmpty()) {
        	whereXPathArr = new String[] { whereTitleXPath + " and " + whereRoleXPath };
        }
        
        return MRXMLDBUtils.readElement(0, null, whereXPathArr, parentNID, null, request, log, db);		
	}
	
	private void setRoles(Document policyMgmtObjectXMLDoc, Element policyAdminObjectRoleEL, String roleObjectArr[], String roleActionArr[], String objectType) throws Exception
	{
		for (int i = 0; i < roleObjectArr.length; i++)
		{
			for (int j = 0; j < roleActionArr.length; j++)
			{
				if(!roleObjectArr[i].equals(PolicyAdminConstants.SYSTEM_OBJECT)  && (roleActionArr[j].equals(PolicyAdminConstants.UPGRADE_ROLE_ACTION) ||
					roleActionArr[j].equals(PolicyAdminConstants.RESTORE_ROLE_ACTION) || roleActionArr[j].equals(PolicyAdminConstants.BACKUP_ROLE_ACTION) || 
					roleActionArr[j].equals(PolicyAdminConstants.SESSION_MANAGEMENT_ROLE_ACTION))) {
					continue;
				} else if(roleObjectArr[i].equals(PolicyAdminConstants.SYSTEM_OBJECT)  && (roleActionArr[j].equals(PolicyAdminConstants.ADD_ROLE_ACTION) ||
						 roleActionArr[j].equals(PolicyAdminConstants.DELETE_ROLE_ACTION))) {
						continue;
				} else if((roleObjectArr[i].equals(PolicyAdminConstants.MEDIA_STREAM_FILE_RESOURCE) || roleObjectArr[i].equals(PolicyAdminConstants.MEDIA_STREAM_DIR_RESOURCE))
						&& (roleActionArr[j].equals(PolicyAdminConstants.ADD_ROLE_ACTION))) {
					continue;
				} else if(!roleObjectArr[i].equals(PolicyAdminConstants.MEDIA_STREAM_FILE_RESOURCE)  && roleActionArr[j].equals(PolicyAdminConstants.EXPORT_ROLE_ACTION)) {
						continue;
				} else if(!roleObjectArr[i].equals(PolicyAdminConstants.MEDIA_STREAM_DIR_RESOURCE)  && (roleActionArr[j].equals(PolicyAdminConstants.IMPORT_ROLE_ACTION ))) {
						continue;
				} else if(roleObjectArr[i].equals(PolicyAdminConstants.MEDIA_STREAM_DIR_RESOURCE)  && roleActionArr[j].equals(PolicyAdminConstants.PLAYBACK_ROLE_ACTION)) {
						continue;
				} else 	if(!roleObjectArr[i].equals(PolicyAdminConstants.MEDIA_STREAM_SRC_PORT_RESOURCE)  && (roleActionArr[j].equals(PolicyAdminConstants.VIEW_FROM_ROLE_ACTION))) {
					continue;
			    } else if(!roleObjectArr[i].equals(PolicyAdminConstants.MEDIA_STREAM_DST_PORT_RESOURCE)  && (roleActionArr[j].equals(PolicyAdminConstants.STREAM_TO_ROLE_ACTION))) {
					continue;
			    }
				
				policyAdminObjectRoleEL = (Element) policyAdminObjectRoleEL.clone();
				MRXMLUtils.addElementXpath(policyMgmtObjectXMLDoc, "//" + objectType + PolicyAdminConstants.ROLE_SET, policyAdminObjectRoleEL);
				MRXMLUtils.setValueXpath(policyAdminObjectRoleEL, "./" + PolicyAdminConstants.ROLE_OBJECT, roleObjectArr[i]);				
				MRXMLUtils.setValueXpath(policyAdminObjectRoleEL, "./" + PolicyAdminConstants.ROLE_ACTION, roleActionArr[j]);									
			}
		}		
	}
	
	private void setFilterNID(Document policyMgmtObjectXMLDoc, String tagObjectALLFilterNID) throws Exception
	{		
		MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.SRC_FILTER + "/" + PolicyAdminConstants.USER_FILTER_NID, tagObjectALLFilterNID);
		MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.SRC_FILTER + "/" + PolicyAdminConstants.STREAM_RESOURCE_FILTER_NID, tagObjectALLFilterNID);
		MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.DST_FILTER + "/" + PolicyAdminConstants.USER_FILTER_NID, tagObjectALLFilterNID);
		MRXMLUtils.setValueXpath(policyMgmtObjectXMLDoc, "//" + PolicyAdminConstants.DST_FILTER + "/" + PolicyAdminConstants.STREAM_RESOURCE_FILTER_NID, tagObjectALLFilterNID);
	}
}
