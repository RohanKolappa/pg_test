package com.ipvs.assetadminservice.file.handler;

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
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.GetFileRequestHandler;
import com.ipvs.utils.Utils;


public class AddFileRequestHandler implements MRRequestHandlerI {
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
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


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {

		Element requestElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//"+MRRequest.getRequestRootName(request.getRequestName()));
		String dirNID = MRXMLUtils.getAttributeValue(requestElement, AMConst.DIR_NID_ATTRIBUTE,"");
		String mediaStoreFileNID = MRXMLUtils.getAttributeValue(requestElement,AMConst.STORE_FILE_NID_ATTRIBUTE,"");
		Document mediaStoreFileDoc = MRXMLUtils.getDocumentXpath(request.getMessage().getDocument(), "//"+AMConst.MEDIA_STORE_FILE);
		String revision  = MRXMLUtils.getAttributeValue(requestElement, AMConst.REVISION_ATTRIBUTE,"");
		
		Document mediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
		
		String title = MRXMLUtils.getValueXpath(mediaStoreFileDoc,"//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME,"");
		String mediaType =MRXMLUtils.getValueXpath(mediaStoreFileDoc,"//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME,"");
		String description = MRXMLUtils.getValueXpath(mediaStoreFileDoc,"//"+MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME,"");
		String streamType = MRXMLUtils.getValueXpath(mediaStoreFileDoc,"//"+MediaStoreServiceConst.STREAM_TYPE,"");
		String readOnly = MRXMLUtils.getAttributeValue(requestElement, AMConst.READ_ONLY_ATTRIBUTE,"");
		Element dirGroupEL = MediaStoreServiceUtils.getMediaStreamDirGroupsEL(dirNID, request, db, log);
		String dateCreated = MRXMLUtils.getValueXpath(mediaStoreFileDoc,"//"+MediaStoreServiceConst.DATE_CREATED,"");
		String dateModified = MRXMLUtils.getValueXpath(mediaStoreFileDoc,"//"+MediaStoreServiceConst.DATE_MODIFIED,"");
		String state = MRXMLUtils.getValueXpath(mediaStoreFileDoc,"//"+MediaStoreServiceConst.STATE_EL_NAME,"");
		
		Element metaDataEl = MRXMLUtils.getElementXpath(mediaStoreFileDoc, ".//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
		
		MediaStoreServiceUtils.updateMediaStreamFileDocWithInfo(xmpp, mediaStreamFileDoc,mediaType, 
				title, description,	dirNID, dirGroupEL , mediaStoreFileNID,streamType, state, readOnly,metaDataEl,dateCreated,dateModified,"false", null);
		MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, Utils.getIntValue(revision, 0));
		
		//updateMediaGroup info
		Element mediaGroupEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME);
		Element storeFileMediaGroupEL = MRXMLUtils.getElementXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME);
		MRXMLUtils.replaceContent(mediaGroupEL, storeFileMediaGroupEL);
		
		//replace type element 
		Element mediaTypeEl = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//"+mediaType);
		Element storefileMediaTypeEL = MRXMLUtils.getElementXpath(mediaStoreFileDoc, "//"+mediaType);
		MRXMLUtils.replaceContent(mediaTypeEl, storefileMediaTypeEL);
		
		//update size 
		MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE, MRXMLUtils.getAttributeValue(requestElement, SessionServiceConst.SIZE,"0"));
		
		//add in database
	    MRXMLDBUtils.addWithXqueryPrefix(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.MediaStreamFileResource, "", "", "", null, mediaStreamFileDoc.getRootElement());
	    log.addInfo("AddMediaStreamFileRequestHandler: Added MediaStreamfile :="+title + " to dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
	    
	    //get file response
	    String selectFormat = MRXMLUtils.getAttributeValue(requestElement,SessionServiceConst.SELECTFORMAT,SessionServiceConst.SELECTFORMAT_NATIVE);
	    if(selectFormat.equals(SessionServiceConst.SELECTFORMAT_NATIVE)) {
	    	return request.createResponseData(mediaStreamFileDoc.getRootElement());
	    } else {	    	
	    
	    	Class<?> classObject = Class.forName(GetFileRequestHandler.class.getName());
			Object handleObject = classObject.newInstance();
			
			String className = classObject.getName();
			String requestName = MRRequest.getRequestName(className);
			Element requestEl = MRXMLUtils.stringToElement("<GetFile/>");			
			MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.WHERE_NID, MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), AMConst.NID,""));
			MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.SELECTFORMAT, selectFormat);
			Document getRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, requestEl);			
			MRRequestHandlerI requestHandlerObject = (MRRequestHandlerI) handleObject;
			
			MRRequest getRequest = MRRequest.createRequest(request, getRequestDoc, requestName, request.getServiceName());
			getRequest.setSuperUser(true);
			Document getObjectResponseDoc = requestHandlerObject.getResponse(getRequest, xmpp, log, db, resourceFactory, profileFactory);
			return request.createResponseData((Element)MRXMLUtils.getElementXpath(getObjectResponseDoc, "//"+SessionServiceConst.FILE).detach());
			
	    }
	}

}
