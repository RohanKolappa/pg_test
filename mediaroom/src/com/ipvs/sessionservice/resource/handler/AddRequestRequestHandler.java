package com.ipvs.sessionservice.resource.handler;

import java.util.ArrayList;
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

public class AddRequestRequestHandler implements MRRequestHandlerI{

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

	/* AddRequestRequestHandler reads the sessionID .
	 * Gets the List of MediaStreamFileResourceNID's and adds the bookmarks to all of the files.
	 * Exception Codes
	 * 		public static final int SESSION_ID_DOES_NOTEXISTS = 4001;
	 * 		public static final int BOOKMARK_NOT_FOUND =  4003; 
	 */
	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document doc = request.getMessage().getDocument();
		String sessionID = MRXMLUtils.getAttributeValueXpath(doc, "//BookmarkTime","sessionId",null);
		Document roomdoc = SessionServiceUtils.getMediaRoomDoc(sessionID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		ArrayList<String> fileList = SessionServiceUtils.getFileList(roomdoc);
		Document response = requestAddBookmarkForFileList(request, log, xmpp, db, resourceFactory, serviceAgentFactory, fileList, doc, roomdoc);
		Element responseEl = MRXMLUtils.getElementXpath(response, "//ResponseXML");
		if (responseEl.elements().size() == 0){
			throw new MRException (SessionServiceExceptionCodes.BOOKMARK_NOT_FOUND,"Bookmark Not Found");
		}
		Document addResponsedoc = MRXMLUtils.stringToDocument("<AddResponse/>");
		addResponsedoc.getRootElement().add((Element)response.getRootElement().clone());
		return request.createResponseData((Element)addResponsedoc.getRootElement().detach());
	}
	
	 public static Document requestAddBookmarkForFileList (MRRequest request,MRLog log,XMPPI xmpp,MRDB db,MRResourceFactoryI resourceFactory,
				ServiceAgentFactoryI serviceAgentFactory,ArrayList<String> fileList,Document data, Document roomdoc) throws Exception{
	    	
			String title = MRXMLUtils.getValueXpath(data, ".//Title","");
			String comments = MRXMLUtils.getValueXpath(data, ".//Comments","");
			Document bookmarkResponseDoc = MRXMLUtils.stringToDocument("<ResponseXML/>");
			Document response = null;

			for (Iterator<?> i = fileList.iterator(); i.hasNext();) {
	             String fileId = (String) i.next();
	             String recordXpath = "data/destNID='"+fileId+"'" ; 
	             String playBackXpath =  "data/sourceNID='"+fileId+"'";

            	 List<?> streamList = MRXMLUtils.getListXpath(roomdoc, "//Stream[ " + recordXpath + "  or " + playBackXpath + " ]");
            	 if(streamList.isEmpty())
            		 continue;
            	 String roomNID = MRXMLUtils.getAttributeValue(roomdoc.getRootElement(),"NID");
            	 String streamNID = MRXMLUtils.getAttributeValue(((Element)streamList.get(0)),"NID");
	             response = requestAddBookmarkForFile(request, roomNID, streamNID, fileId, title, comments, log, xmpp, db, resourceFactory, serviceAgentFactory);
	            if (response !=null){
	            	bookmarkResponseDoc = SessionServiceUtils.createBookmarkResponse(response, log, bookmarkResponseDoc);
	            }
			}
			return bookmarkResponseDoc;
	    }
	    
	    public static Document requestAddBookmarkForFile(MRRequest request, String roomNID, String streamNID, String fileId,String title,String comments,MRLog log,XMPPI xmpp,MRDB db,MRResourceFactoryI resourceFactory,
				ServiceAgentFactoryI serviceAgentFactory) throws Exception{
	    	
	    	Document requestDoc = MRXMLUtils.loadXMLResource(SessionServiceUtils.ADDBOOKMARK_ACTION_XML, log);
	    	MRXMLUtils.setValueXpath(requestDoc, "//roomNID",roomNID);
	    	MRXMLUtils.setValueXpath(requestDoc, "//streamNID",streamNID);
	    	MRXMLUtils.setValueXpath(requestDoc, "//MediaStreamFileResourceNID",fileId);
	     	long xmpp_date = xmpp.getCurrentTimeMillis();
	     	MRXMLUtils.setValueXpath(requestDoc, "//DateCreated",String.valueOf(xmpp_date));
	     	MRXMLUtils.setValueXpath(requestDoc, "//DateModified",String.valueOf(xmpp_date));
	     	MRXMLUtils.setValueXpath(requestDoc, "//AuthorUserID",request.getUserJID());
	     	MRXMLUtils.setValueXpath(requestDoc, "//Title",title);
	     	MRXMLUtils.setValueXpath(requestDoc, "//Comment",comments);
	     	
	     	 String serviceName = MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");;
	 	    String requestName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");;
	 	    MRMessage message = new MRMessage(requestDoc);
	 	    String resourceGroup = "";
	 		String handlerName = "com.ipvs.mediaroomservice.handler." + requestName + "Handler";
	         MRRequestHandlerI handler = (MRRequestHandlerI) Class.forName(handlerName).newInstance();
	         request = MRRequest.createRequest(request.getUserJID(), MRLog.ERROR, "requestNID", serviceName, requestName, message, resourceGroup);
	         Document response = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	         
			return response;
	    	
	    }

}
