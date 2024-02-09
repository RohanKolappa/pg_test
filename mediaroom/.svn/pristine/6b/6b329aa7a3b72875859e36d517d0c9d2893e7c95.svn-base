package com.ipvs.sessionservice.resource.handler;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.policyadminservice.impl.PolicyManager;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.handler.base.GetBaseObjectListRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionObjectPermissions;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class GetDirRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI  {
	@Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
			 log.addTraceBegin(request);

		        int authType = MediaRoomObject.ROOM_TYPEID;
		        int role = MediaRoomPEM.VISITOR_ROLE;
		        String token = "testCommonToken";
		        String authority = "testAuthority";
		        String roomPolicyNID = MRConst.NULL;
		        String streamPolicyNID = MRConst.NULL;
		        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		        log.addTraceEnd(request);
		        return auth;
	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Document doc = request.getMessage().getDocument();		
		Element getDirElement = MRXMLUtils.getElementXpath(doc, "//GetDir");
		String whereParentDirNID = MRXMLUtils.getAttributeValue(getDirElement, "whereParentDirNID", "");
		 
		List<String> listOfWhereXpath = new ArrayList<String>();
		String whereParentDirNIDXPath = "";
		if (!"".equals(whereParentDirNID)) {
			whereParentDirNIDXPath = GetObjectUtils.generateXPath(whereParentDirNID, SessionServiceConst.WHERE_PARENT_DIR_NID, SessionServiceConst.DIR, SessionServiceConst.NID_CONST);
			if(!whereParentDirNIDXPath.isEmpty()) {
                listOfWhereXpath.add(whereParentDirNIDXPath);
            }		
		}
		
		
		Hashtable<String, String> customeTokenMappingTable= new Hashtable<String, String>();
	    String whereFilePermissions = MRXMLUtils.getAttributeValue(getDirElement, SessionServiceConst.WHERE_FILE_PERMISSIONS, "");
	    String wherePermissions = MRXMLUtils.getAttributeValue(getDirElement, SessionServiceConst.WHERE_PERMISSIONS, "");
	    String detail = MRXMLUtils.getAttributeValue(getDirElement, "selectFormat", "BASIC");
        String serachInFiles = SessionServiceConst.FALSE;
	    String mediaStreamFilePemXPath = SessionServiceConst.DUMMY_NOT_XPATH;
		if(!whereFilePermissions.isEmpty() && !request.isSuperUser()) {
			List<String> roleObjectList = PolicyManager.getPolicyRoleObjectListForGivenModelObject(AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
			Document policiesResponseDoc =  PolicyAdminServiceUtils.getTargetXpathResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roleObjectList, PolicyAdminConstants.READ_ROLE_ACTION);
			serachInFiles = SessionServiceConst.TRUE;
			try {
		    	List<String> roleObjectsFromViewObject = SessionObjectPermissions.getPolicyRoleObjectListForGivenViewObject(log, SessionServiceConst.FILE);
			    mediaStreamFilePemXPath = GetObjectUtils.validateTargetXPath(policiesResponseDoc, roleObjectList, AMConst.MEDIA_STREAM_FILE_RESOURCE, PolicyAdminConstants.READ_ROLE_ACTION, whereFilePermissions, roleObjectsFromViewObject,SessionServiceConst.FILE, request, xmpp, log, db, serviceAgentFactory, resourceFactory);
			    mediaStreamFilePemXPath = PolicyAdminServiceUtils.validateReadPemXpath( mediaStreamFilePemXPath, roleObjectList, AMConst.MEDIA_STREAM_FILE_RESOURCE, ServiceClient.ASSETADMINSERVICE, request, db, log);
			    if(mediaStreamFilePemXPath.isEmpty()) {
			    	mediaStreamFilePemXPath = SessionServiceConst.DUMMY_XPATH;
			    }
		    } catch(Exception exp) {
		    	log.addInfo("GetDirRequest: No Permissions Available to access file using whereFilePermissions:="+whereFilePermissions,MRLog.INFO,MRLog.NOTIFY);
		    	mediaStreamFilePemXPath = SessionServiceConst.DUMMY_NOT_XPATH;
		    }	        
		} 
		
		customeTokenMappingTable.put(SessionServiceConst.SEARCH_IN_FILES, serachInFiles);
		customeTokenMappingTable.put(AMConst.MEDIA_STREAM_FILE_RESOURCE+ SessionServiceConst.PEMXPATH, mediaStreamFilePemXPath);
		
		List<String> pemObjectList = new ArrayList<String>();
		
		//Either wherePermissions or whereFilePermissions are needed. If any one of them is given make sure other is not checking. 
		if(!request.isSuperUser() && wherePermissions.isEmpty() && !whereFilePermissions.isEmpty()) {
			customeTokenMappingTable.put(AMConst.MEDIA_STREAM_DIR_RESOURCE+ SessionServiceConst.PEMXPATH, SessionServiceConst.DUMMY_NOT_XPATH);
		}
		
		pemObjectList.add(AMConst.MEDIA_STREAM_DIR_RESOURCE);		
		
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.DIR + SessionServiceConst.XQUERYXML;
		String sortBy = MRXMLUtils.getAttributeValue(getDirElement, "sortBy", ""); 
		if (sortBy.equals(SessionServiceConst.TITLE)) {
        	sortBy =  MediaStoreServiceConst.INFO_ELEMENT_NAME+"/"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME;
        } else if (sortBy.equals(SessionServiceConst.DATE_CREATED)) {
        	sortBy =  MediaStoreServiceConst.DATE_CREATED + "/" + SessionServiceConst.NUMBER_CONST;
        } else if (sortBy.equals(SessionServiceConst.DATE_MODIFIED)) {
        	sortBy =  MediaStoreServiceConst.DATE_MODIFIED + "/" + SessionServiceConst.NUMBER_CONST;
        } else if (sortBy.equals(SessionServiceConst.STATE)) {
        	sortBy =  AMConst.STATEFLAG;
        } else if(sortBy.equalsIgnoreCase(SessionServiceConst.SIZE)){
            sortBy =  MediaStoreServiceConst.MEDIA_DIR_SIZE + "/" + SessionServiceConst.NUMBER_CONST;
        } else if(sortBy.equalsIgnoreCase(SessionServiceConst.SIZE_USED)){
            sortBy =  MediaStoreServiceConst.MEDIA_DIR_SIZE_USED + "/" + SessionServiceConst.NUMBER_CONST;
        } else {
            sortBy = "";
        }
		
		Document resultDoc = this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory,getDirElement ,SessionServiceConst.DIR,AMConst.MEDIA_STREAM_DIR_RESOURCE, sortBy, listOfWhereXpath, xqueryFileName,customeTokenMappingTable,pemObjectList);
		
		if(detail.equals(SessionServiceResourceConst.SELECTFORMAT_SNAPSHOTURL) || detail.equals(SessionServiceResourceConst.SELECTFORMAT_UPLOADURL)) {
			String expireTime = MRXMLUtils.getAttributeValue(getDirElement, SessionServiceResourceConst.EXPIRE_TIME,"");
			List<?> portList = MRXMLUtils.getListXpath(resultDoc, "//Port");
			String dstTitle = getDstTitle(getDirElement, detail, request);
			Element destinationElement = GetObjectUtils.getdstNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstTitle);
			if(destinationElement == null) {
				destinationElement = MRXMLUtils.newElement("Destination");
			}
			for(Object obj:portList) {
				Element portEl = (Element)obj;
				if(expireTime.isEmpty()) {
					expireTime = MRXMLUtils.getAttributeValueXpath(portEl, ".//ApiKey", "validInterval", String.valueOf(SessionServiceResourceConst.API_KEY_VALID_INTERVAL));
				}
				long now = xmpp.getCurrentTimeMillis();
		    	String timeStamp = (HttpAuthentication.INFINITE_EXPIRY.equals(expireTime))?HttpAuthentication.INFINITE_EXPIRY :String.valueOf(now + (Long.parseLong(expireTime) * 1000));
				String signature = HttpAuthentication.generateExpiryBasedSignature(portEl, timeStamp);
				String uploadURL= "";
				uploadURL = MRXMLUtils.getAttributeValue(portEl, SessionServiceResourceConst.SNAPSHOTURL_ATTRIBUTE, "");
				if (uploadURL.isEmpty()) {
                    uploadURL = MRXMLUtils.getAttributeValue(portEl, SessionServiceResourceConst.UPLOADURL_ATTRIBUTE, "");
                }
	    		if( !uploadURL.isEmpty()){	    		
	    			
	    			Element sourceElement = MRXMLUtils.getElementXpath(portEl, ".//Source");
	    			Document deviceDoc = MRXMLUtils.getDocument();
	    	    	deviceDoc.add(MRXMLUtils.getElementXpath(portEl, ".//Device").detach());	
	    	    
	    	    	String resolveIP = StreamPolicyUtils.getResolvedIPAddressForURL(request, xmpp, log, db, destinationElement, sourceElement, deviceDoc);
	    	    	uploadURL = uploadURL.replace("_IPADDRESS_",resolveIP);
	    	    
	    		    String deviceElementId = MRXMLUtils.getAttributeValue(portEl, SessionServiceResourceConst.DEVICE_ELEMENT_ID_ATTRIBUTE, "");
	    		    uploadURL = uploadURL.replaceAll(SessionServiceResourceConst.DIRID_CONST, deviceElementId);
	    		    uploadURL = uploadURL.replaceAll(SessionServiceResourceConst.TIMESTAMP_CONST, timeStamp);
	    		    uploadURL = uploadURL.replaceAll(SessionServiceResourceConst.SIGNATURE_CONST, signature);
	    		    Map<String,String>sparamList = new HashMap<String,String>();
	    		    sparamList.put("hls",""+ AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getServiceConfigManager().isHLSEnabled());
	    		  
	    		    //append global hls config information in system parameter
	    		    String sparam =  HttpAuthentication.generateSystemParamsSignature(portEl,sparamList);
	    		    uploadURL += "&sparam="+sparam;
	    		    if(detail.equals(SessionServiceResourceConst.SELECTFORMAT_SNAPSHOTURL) ) { 
                        MRXMLUtils.setAttributeValue(portEl, SessionServiceResourceConst.SNAPSHOTURL_ATTRIBUTE, uploadURL); //this will remove in future
                    } else{
	    		        MRXMLUtils.setAttributeValue(portEl, SessionServiceResourceConst.UPLOADURL_ATTRIBUTE, uploadURL);   
	    		    }
	    		}
	    		MRXMLUtils.removeChild(portEl, SessionServiceResourceConst.API_KEY_TABLE);
	    		MRXMLUtils.removeChild(portEl, "Source");
	    		MRXMLUtils.removeChild(portEl, "Device");
			}
			
		}		
		GetObjectUtils.subscribeUser(getDirElement, resultDoc, SessionServiceConst.DIR, request, serviceAgentFactory);
		return resultDoc;
		
	}
}
