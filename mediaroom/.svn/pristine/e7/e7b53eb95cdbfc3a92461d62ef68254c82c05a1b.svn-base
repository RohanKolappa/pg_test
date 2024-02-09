package com.ipvs.sessionservice.session.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.BookmarkBaseRequestHandler;
import com.ipvs.sessionservice.impl.DeleteObjectUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public class DeleteBookmarkRequestHandler extends BookmarkBaseRequestHandler  {
    @Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI profileFactory) throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.ROOM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token,
                authority);
        log.addTraceEnd(request);

        return auth;
    }

  
    @Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	    	
         List<String> listOfXpaths = getBookmarkXPathList(request, xmpp, log, db, resourceFactory, serviceAgentFactory,request.getMessage().getDocument(), SessionServiceConst.DELETE_BOOKMARK);
         String listOfXpathsString = GetObjectUtils.generateWhereXpathFromList(listOfXpaths,"and","Bookmark");
         String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.DELETE_BOOKMARK_QUERY + SessionServiceConst.XQUERYXML;
         String query = FileUtils.readFileResource(xqueryFileName, log.getClass());
         Document deleteBookmarkRequesDoc= request.getMessage().getDocument();
     	 Element bookmarkEL = MRXMLUtils.getElementXpath(deleteBookmarkRequesDoc, "//" + "DeleteBookmark");
         String detail = MRXMLUtils.getAttributeValue(bookmarkEL, "selectFormat", "BASIC");
 		 String wherePermissions = MRXMLUtils.getAttributeValue(bookmarkEL, "wherePermissions", "");	
		 query = query.replace("_XPATHAND_",listOfXpathsString);
		 request.setSuperUser(true);
		 Document queryResultDoc = GetObjectUtils.executeQuery(query,  AMConst.BOOKMARK, null, detail,wherePermissions, request, xmpp, log, db, serviceAgentFactory, resourceFactory, true);
		 List<?> bookmarkList = MRXMLUtils.getListXpath(queryResultDoc, "//Bookmark");
		 if (bookmarkList==null || bookmarkList.isEmpty()){
			 throw new MRException(SessionServiceExceptionCodes.NO_BOOKMARK_CLIPFILE_FOUND,"The file doesn't contain any bookmark");
		 }
		 Element bookmarkEl = (Element)bookmarkList.get(0);
		 String mediaNID =  MRXMLUtils.getAttributeValue(bookmarkEl, "mediaStreamFileNID", "");
			
		 Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaNID, request, log, db);
		 Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(mediaFileDoc, true, request, db, log);        
	     String dirNID = MediaStoreServiceUtils.getParentDirNID(mediaFileDoc, request, log, db);
	     String dirID=  MRXMLUtils.getAttributeValue(mediaStorePortElement, MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME, "");
	     
		Document deleteBookmarkDoc=  getDeleteBookmarkRequestDocument(request,log,queryResultDoc,dirNID, dirID, db, serviceAgentFactory, resourceFactory,xmpp);
		sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory,deleteBookmarkDoc, mediaStorePortElement, DeleteBookmarkRequestHandler.class.getName());
		
         return null;
    }
    
	public Document getDeleteBookmarkRequestDocument(MRRequest request, MRLog log,Document queryResultDoc,String dirNID, String dirID,MRDB db,ServiceAgentFactoryI serviceAgentFactory,MRResourceFactoryI resourceFactory,XMPPI xmpp) throws MRException, Exception {

		String requestname="DeleteBookmarkRequest";
		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document newRequestDoc = (Document)requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(newRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestname);
		MRXMLUtils.setAttributeValueXPath(newRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		MRXMLUtils.addAttribute(MRXMLUtils.getElementXpath(newRequestDoc, MRConst.HEADER_XPATH), MRConst.TYPE,  MRConst.REQUEST_TYPE_RPC);
		Element DataEL = MRXMLUtils.getElementXpath(newRequestDoc, "//Data");


		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(requestname + "Data"));
		List<?> bookmarkList = MRXMLUtils.getListXpath(queryResultDoc, "//Bookmark");
		Element requestEl = MRXMLUtils.newElement(requestname);
			MRXMLUtils.addAttribute(requestEl, SessionServiceConst.DIRID, dirID);
			MRXMLUtils.addAttribute(requestEl, SessionServiceConst.DIR_NID, dirNID);
		 for(Object obj: bookmarkList) {
			 Element bookmarkEl = (Element)obj;
			
				String bookmarkUUID =  MRXMLUtils.getAttributeValue(bookmarkEl, "bookmarkUUID", "");
				String mediaUUID =  MRXMLUtils.getAttributeValue(bookmarkEl, "mediaStreamFileUUID", "");
				String wherebookmarkNID =  MRXMLUtils.getAttributeValue(bookmarkEl, "NID", "");					
				
				Element requestEle = MRXMLUtils.newElement("DeleteBookmark");
				MRXMLUtils.addAttribute(requestEle, SessionServiceConst.BY_FILE_ID, bookmarkUUID);	
				MRXMLUtils.addAttribute(requestEle, SessionServiceConst.BY_ID, mediaUUID);
				MRXMLUtils.addAttribute(requestEle, SessionServiceConst.BY_FILE_NID, wherebookmarkNID);
				MRXMLUtils.addElement(requestEl, requestEle);
			}
		 MRXMLUtils.addElement(requestDataEl, requestEl);
		return newRequestDoc;
	}
    
    @Override
    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc,MRRequest userRequest,Document agentResponseDoc) throws MRException,Exception {
    	Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc,MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);
       	if (state == MediaStoreServiceConst.FAILURE_STATE) {
			return userRequest.createResponseData();
		}
        Element requestElement =  (Element) MRXMLUtils.getElementXpath(userRequest.getMessage().getDocument(), "//" + userRequest.getRequestName() + "Data").elements()
        .iterator().next();
		List<?> bookmarkList = MRXMLUtils.getListXpath(agentResponseDoc, "//Bookmark");
		for(Object obj: bookmarkList) {
			 Element bookmarkEl = (Element)obj;
			 String NID = MRXMLUtils.getAttributeValue(bookmarkEl, "NID", "");	
			 userRequest.setSuperUser(true);
				DeleteObjectUtils.deleteObjectByNID(userRequest, xmpp, log, db, AMConst.BOOKMARK, NID);	 		
		 }	
		Element bookmarks = MRXMLUtils.getElementXpath(agentResponseDoc, "//Bookmarks");
		bookmarks.setName(requestElement.getName());		
		Element responseData= DocumentHelper.createElement("Data");
		responseData.add(bookmarks.detach());
			
		return MRXMLUtils.elementToDocument(responseData);
	}
    
}
 
   
