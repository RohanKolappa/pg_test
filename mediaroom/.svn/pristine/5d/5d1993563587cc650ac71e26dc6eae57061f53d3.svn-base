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
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceUtils;

public class DeleteRequestRequestHandler implements MRRequestHandlerI{

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

	/* DeleteRequestRequestHandler reads the xpath from the request.
	 *  Gets the List of the Bookmarks using the xpath and deletes all the List of bookmarks. 
	 *  
	 * Exception Codes
	 * 		public static final int BOOKMARK_NOT_FOUND =  4003; 
	 */
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		
		Document doc = request.getMessage().getDocument();
		String xpath = MRXMLUtils.getAttributeValueXpath(doc, "//Where","xpath", null);
		Document bookmarkListDoc = SessionServiceUtils.handleGetBookMarkList(request, "",xpath, log, xmpp, db, resourceFactory, serviceAgentFactory);
		List<?> bookmarkList = MRXMLUtils.getListXpath(bookmarkListDoc, ".//Bookmark");
		Document response = requestDeleteBookmarkList(request, log, xmpp, db, resourceFactory, serviceAgentFactory, bookmarkList, doc);
		
		Element responseEl = MRXMLUtils.getElementXpath(response, "//ResponseXML");
		if (responseEl.elements().size() == 0){
			throw new MRException (SessionServiceExceptionCodes.BOOKMARK_NOT_FOUND,"Bookmark Not Found");
		}
		Document deleteResponsedoc = MRXMLUtils.stringToDocument("<DeleteResponse/>");
		deleteResponsedoc.getRootElement().add((Element)response.getRootElement().clone());
		return request.createResponseData((Element)deleteResponsedoc.getRootElement().detach());
	}
	
	public static Document requestDeleteBookmarkList (MRRequest request,MRLog log,XMPPI xmpp,MRDB db,MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, List<?> bookmarkList,Document data) throws Exception{
    	Document bookmarkResponseDoc = MRXMLUtils.stringToDocument("<ResponseXML/>");
		 Document response = null;
			for (Iterator<?> i = bookmarkList.iterator(); i.hasNext();) {
				Element bookmarkEL = (Element) i.next();
				String NID = MRXMLUtils.getAttributeValue(bookmarkEL,"NID");
	        	response =   requestDeleteBookmark(request, NID,log, xmpp, db, bookmarkEL, resourceFactory, serviceAgentFactory);
	        	if (response !=null){
	        		bookmarkResponseDoc = SessionServiceUtils.createBookmarkResponse(MRXMLUtils.elementToDocument(bookmarkEL), log, bookmarkResponseDoc);
	        	}
	        }
		return bookmarkResponseDoc;
    }
    
    public static Document requestDeleteBookmark(MRRequest request,String NID,MRLog log,XMPPI xmpp,MRDB db,Element bookmarkEL, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception{
    	
    	Document requestDoc = MRXMLUtils.loadXMLResource(SessionServiceUtils.DELETEBOOKMARK_XML, log);
    	MRXMLUtils.setValueXpath(requestDoc, "//BookmarkNID",NID);
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
