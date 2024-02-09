package com.ipvs.proxy.handler;

import java.net.URLEncoder;

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
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.proxy.impl.ProxyClientAppUtils;
import com.ipvs.proxy.impl.ProxyConstants;


public class CreateBookmarkRequestHandler implements MRRequestHandlerI {
    

    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.STREAM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        // If destNID is empty then use the request.getResourceGroup
        // as the destGroup to find the matching policy
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    	log.addInfo("CreateBookmarkRequestHandler - Begin", MRLog.INFO,MRLog.NOTIFY);
    	String userJID  = request.getUserJID();
    	String user = userJID.substring(0, userJID.indexOf("@"));
		String fileNID = null;
    	Document requestDoc = request.getMessage().getDocument();
    	
    	String id = MRXMLUtils.getValueXpath(requestDoc.getRootElement(),"//ID", "");
    	String timestamp = MRXMLUtils.getValueXpath(requestDoc.getRootElement(), "//Timestamp","");
		try {
			if (new Long(timestamp).longValue() == 0) {
				MRRequestHandlerI handler = new MediaTimestampRequestHandler();
				handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				return ProxyClientAppUtils.createResponseDoc(request,"Success");
			}
		} catch (Exception e) {
			timestamp = String.valueOf(xmpp.getCurrentTimeMillis());
			MRXMLUtils.setValueXpath(requestDoc.getRootElement(), "//Timestamp", timestamp);
		}
		
		String notes = MRXMLUtils.getValueXpath(requestDoc.getRootElement(), "//Notes", "");

		String dateCreated = String.valueOf(xmpp.getCurrentTimeMillis());
		
		//fileNID = ProxyClientAppUtils.getIDMediaNIDMappingHash().get(id);
		fileNID = ProxyClientAppUtils.recordingSession.getFileNID();
		sendCreateBookMarkRequest(requestDoc, id, fileNID, timestamp, dateCreated, user, notes, request, log, db, xmpp, resourceFactory,serviceAgentFactory);
		log.addInfo("CreateBookmarkRequestHandler - End", MRLog.INFO,MRLog.NOTIFY);
		return ProxyClientAppUtils.createResponseDoc(request,"Success");
    } 

    
    
    
    private static void sendCreateBookMarkRequest(Document proxyRequestDoc, String recordID,
			String fileNID, String timestamp, String dateCreated, String user,
			String notes, MRRequest request, MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
    	//String userJID = request.getUserJID();
		//Document fileDoc = ProxyClientAppUtils.getMediaStreamFileResource(fileNID, db, log, userJID);
		String createBookMarkXML = "AddBookmarkRequest.xml";
		String requestName = "AddBookmarkRequest";
		String requestString = MRXMLUtils.documentToString(proxyRequestDoc);
		String clientdataString = URLEncoder.encode(requestString, "UTF-8");
		Document requestDoc = MRXMLUtils.loadXMLResource(
				ProxyConstants.MEDIAROOM_XMLFILEPATH + createBookMarkXML, log);
		
		Element bookMarkEL = MRXMLUtils.getElementXpath(requestDoc,
				"//Bookmark");

		if (bookMarkEL != null) {
			MRXMLUtils.setValueXpath(requestDoc, "//MediaStreamFileResourceNID", fileNID);
			MRXMLUtils.setValueXpath(requestDoc, "//AtTC", timestamp);
			MRXMLUtils.setValueXpath(requestDoc, "//DateCreated", dateCreated);
			MRXMLUtils.setValueXpath(requestDoc, "//DateModified", dateCreated);
			MRXMLUtils.setValueXpath(requestDoc, "//AuthorUserID", user);
			MRXMLUtils.setValueXpath(requestDoc, "//Title", dateCreated);
			MRXMLUtils.setValueXpath(requestDoc, "//Comment", notes);
		}
		Element clientData = MRXMLUtils.stringToElement("<clientdata></clientdata>");
		clientData.setText(clientdataString);
		Element headerElement = MRXMLUtils.getElementXpath(requestDoc, MRConst.HEADER_XPATH);
		MRXMLUtils.removeChild(headerElement,"clientdata");
		headerElement.add(clientData);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request
				.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);

	}

     
        


}
