package com.ipvs.sessionservice.resource.handler;

import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceUtils;

public class UpdateRequestRequestHandler implements MRRequestHandlerI{

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

	/*UpdateRequestRequesthandler reads the xpath. 
	 * Gets the bookmarkList based on the xpath and updates all the bookmarks in the List.
	 *  
	 * Exception Codes
	 * 		public static final int SESSION_ID_DOES_NOTEXISTS = 4001;
	 * 		public static final int BOOKMARK_NOT_FOUND =  4003; 
	 */
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document doc = request.getMessage().getDocument();
		String xpath = MRXMLUtils.getAttributeValueXpath(doc, "//Where","xpath",null);
		Document bookmarkListDoc = SessionServiceUtils.handleGetBookMarkList(request, "",xpath, log, xmpp, db, resourceFactory, serviceAgentFactory);
		List<?> bookmarkList = MRXMLUtils.getListXpath(bookmarkListDoc, ".//Bookmark");
		Document response = requestUpdateBookmarkList(request, log, xmpp, db, resourceFactory, serviceAgentFactory, bookmarkList, doc, "UpdateRequest");
		
		Element responseEl = MRXMLUtils.getElementXpath(response, "//ResponseXML");
		if (responseEl.elements().size() == 0){
			throw new MRException (SessionServiceExceptionCodes.BOOKMARK_NOT_FOUND,"Bookmark Not Found");
		}
		Document updateResponsedoc = MRXMLUtils.stringToDocument("<UpdateResponse/>");
		updateResponsedoc.getRootElement().add((Element)response.getRootElement().clone());
		return request.createResponseData((Element)updateResponsedoc.getRootElement().detach());
	}

	public static Document requestUpdateBookmarkList (MRRequest request,MRLog log,XMPPI xmpp,MRDB db,MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, List<?> bookmarkList,Document data,String requestName) throws Exception{
    	Document bookmarkResponseDoc = MRXMLUtils.stringToDocument("<ResponseXML/>");
    	 String mediaStreamFileResourceNID = MRXMLUtils.getAttributeValueXpath(data, ".//MediaInfo","id",null);
    	 String mediaOffset = MRXMLUtils.getAttributeValueXpath(data, ".//MediaInfo","offset",null);
		 String mediaTime = MRXMLUtils.getAttributeValueXpath(data, ".//MediaInfo","TC",null);
		 String title = MRXMLUtils.getValueXpath(data, ".//Title", "");
		 String comments = MRXMLUtils.getValueXpath(data, ".//Comments", "");
		 String xpath = MRXMLUtils.getAttributeValueXpath(data, ".//Where","xpath",null);
		 Document response = null;
			for (Iterator<?> i = bookmarkList.iterator(); i.hasNext();) {
				Element bookmarkEL = (Element) i.next();
				MRXMLUtils.getElementXpath(bookmarkEL, "//MediaStreamFileResourceNID").setText(mediaStreamFileResourceNID);
				MRXMLUtils.getElementXpath(bookmarkEL, "//Offset").setText(mediaOffset);
				MRXMLUtils.getElementXpath(bookmarkEL, "//AtTC").setText(mediaTime);
				MRXMLUtils.getElementXpath(bookmarkEL, "//Title").setText(title);
				MRXMLUtils.getElementXpath(bookmarkEL, "//Comment").setText(comments);
	        	 response =   requestUpdateBookmarkInternal(request, xpath, mediaOffset,log, xmpp, db, bookmarkEL, resourceFactory, serviceAgentFactory);
	        	 bookmarkResponseDoc = SessionServiceUtils.createBookmarkResponse(response, log, bookmarkResponseDoc);
	           }
		return bookmarkResponseDoc;
    }
    
    public static Document requestUpdateBookmarkInternal (MRRequest request,String atTC,String streamOffset,MRLog log,XMPPI xmpp,MRDB db,Element bookmarkEl,MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception{
    	Document requestDoc = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.UPDATEBOOKMARK_XML, log);
    	String bookMarkNID = null;
    	if (bookmarkEl!=null){
    		bookMarkNID = MRXMLUtils.getAttributeValue(bookmarkEl,"NID");
    	}
    	String dateCreated = MRXMLUtils.getValueXpath(bookmarkEl, "//DateCreated", "");
    	String AtTC = MRXMLUtils.getValueXpath(bookmarkEl, "//AtTC", "");
    	String offset = MRXMLUtils.getValueXpath(bookmarkEl, "//Offset", "");
    	String MediaStreamFileResourceNID = MRXMLUtils.getValueXpath(bookmarkEl, "//MediaStreamFileResourceNID", "");
    	String title = MRXMLUtils.getValueXpath(bookmarkEl, "//Title", "");
    	String authorUserID = MRXMLUtils.getValueXpath(bookmarkEl, "//AuthorUserID", "");
    	String comments = MRXMLUtils.getValueXpath(bookmarkEl, "//Comment", "");
    	if ("".equals(AtTC)){
    		  AtTC = atTC;
    	}
    	if ("".equals(offset)){
    		  offset = streamOffset;
    	}
    	long xmpp_date = xmpp.getCurrentTimeMillis();
    	MRXMLUtils.setValueXpath(requestDoc, "//BookmarkNID",bookMarkNID);
    	MRXMLUtils.setValueXpath(requestDoc, "//MediaStreamFileResourceNID",MediaStreamFileResourceNID);
    	MRXMLUtils.setValueXpath(requestDoc, "//AtTC",AtTC);
    	MRXMLUtils.setValueXpath(requestDoc, "//Offset",offset);
    	MRXMLUtils.setValueXpath(requestDoc, "//DateCreated",dateCreated);
    	MRXMLUtils.setValueXpath(requestDoc, "//DateModified",String.valueOf(xmpp_date));
    	MRXMLUtils.setValueXpath(requestDoc, "//AuthorUserID",authorUserID);
    	MRXMLUtils.setValueXpath(requestDoc, "//Title",title);
    	MRXMLUtils.setValueXpath(requestDoc, "//Comment",comments);

    	String serviceName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");
  	    String requestName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
  	    MRMessage message = new MRMessage(requestDoc);
  	    String resourceGroup = "";
  		String handlerName = "com.ipvs.mediaroomservice.handler." + requestName + "Handler";
          MRRequestHandlerI handler = (MRRequestHandlerI) Class.forName(handlerName).newInstance();
          request = MRRequest.createRequest(request.getUserJID(), MRLog.ERROR, "requestNID", serviceName, requestName, message, resourceGroup);
          Document response = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
 		return response;

    }
}
