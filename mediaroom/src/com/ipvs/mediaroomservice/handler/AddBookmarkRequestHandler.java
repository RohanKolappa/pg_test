package com.ipvs.mediaroomservice.handler;

import java.net.URLDecoder;
import java.net.URLEncoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class AddBookmarkRequestHandler implements MRRequestHandlerI, AgentResponseHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        String bookMarkRequestXML = "Bookmark_BASE_ALL_ADD.xml";

        try {
            Document requestDoc = request.getMessage().getDocument();
            Element requestedbookmarkEL = MRXMLUtils.getElementXpath(requestDoc, "//" + AMConst.BOOKMARK);
           
            //newly added code 
            String mediaStreamFileNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
            String TC = MRXMLUtils.getValueXpath(requestDoc, "//AtTC", "");
            String offset = MRXMLUtils.getValueXpath(requestDoc, "//Offset" , "");
            String WC = MRXMLUtils.getValueXpath(requestDoc, "//WC", "");
            String streamNID = MRXMLUtils.getValueXpath(requestDoc, "//streamNID", "");
            String roomNID = MRXMLUtils.getValueXpath(requestDoc, "//roomNID", "");
            Document streamDoc = null;
            String trackName = "";
            String recordXpath = "data/destNID='"+mediaStreamFileNID+"'" ; 
            String playBackXpath =  "data/sourceNID='"+mediaStreamFileNID+"'";
            String streamXpath = "//Stream[ " + recordXpath + "  or " + playBackXpath + " ]";
            if("".equals(roomNID)) {
            	if(!"".equals(streamNID))
            		roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, streamNID);
            	else
            		roomNID = MediaRoomServiceUtils.getRoomNIDWithXPath(request, xmpp, log, db, streamXpath);
            }
            
            if (!"".equals(mediaStreamFileNID)&& "".equals(streamNID)){
            	 streamDoc =MediaRoomServiceUtils.readElementByXpath(roomNID, request, log, db, streamXpath);
            }
            if (!"".equals(streamNID)){
            	streamDoc =MediaRoomServiceUtils.readElement(roomNID, request, log, db, streamNID);
            }
            
            if (streamDoc!=null){
	             streamNID = MRXMLUtils.getAttributeValue(streamDoc.getRootElement(),"NID");
	             Element tagEl = MRXMLUtils.getElementXpath(streamDoc, ".//streamGroups/Tag");
	             if (tagEl!=null){
	            	 trackName = MRXMLUtils.getAttributeValue(tagEl,"Value");
	            	 trackName = trackName.split("\\.")[1] ;
	             }
	        }
            
            if ((streamNID == null) || (streamNID.trim().length() == 0)) {
                throw new MRException(MediaRoomServiceExceptionCodes.STREAMNIDNOTFOUND, "StreamNID Not Found");
            }
       	 	
            String trackOffsetAtWC = MRXMLUtils.getValueXpath(streamDoc, "//trackOffsetAtStart", "");

            if("".equals(WC)){
            	WC=String.valueOf(xmpp.getCurrentTimeMillis()); 
            }
            roomNID = MRXMLUtils.getValueXpath(streamDoc, "//roomNID", "");
            String trackNID = MediaRoomServiceUtils.getTrackNID(roomNID, trackName);
            
            Document trackDataDoc =MediaRoomServiceUtils.readElement(roomNID, request, log, db, trackNID);
            MediaTimeline trackTimeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc, log);
            String trackState = trackTimeline.getLastState();

            long currentTrackOffset = trackTimeline.getLastOffset();
            
            long lastActionWallClock = trackTimeline.getLastActionWallClock();

            if (trackState.equals(MediaRoomServiceConst.TRACK_STARTED_CURRENT) || trackState.equals(MediaRoomServiceConst.TRACK_STARTED_LOOKBACK)){
            	currentTrackOffset = currentTrackOffset + (Long.parseLong(WC) - lastActionWallClock);
            	trackOffsetAtWC = String.valueOf(currentTrackOffset);
            }

            if ("".equals(offset) && "".equals(TC)){
            	if (trackName.equals(MediaRoomServiceConst.RECORDTRACK)){
            		MRXMLUtils.setValueXpath(requestDoc, "//AtTC", WC);
            		TC = WC;
            	} else if (trackName.equals(MediaRoomServiceConst.STREAMTRACK)){
            		MRXMLUtils.setValueXpath(requestDoc, "//Offset", trackOffsetAtWC);
            		offset = trackOffsetAtWC;
            	}
            }
            Document response = MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.ASSETADMINSERVICE, bookMarkRequestXML, null, null, (Element) requestedbookmarkEL.clone(), "Add");
            //I have posted the review
            if ("".equals(offset) || "".equals(TC)){
            	Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db); 
              	String mediaStorePortNID = MediaStoreServiceUtils.getMediaStorePortResourceNIDFromDirRead(mediaStreamFileDoc, request, db, log);  
                
   	            String mediaStoreStreamAgentJID = MediaRoomServiceUtils.getContactJIDForPort(request, log, db, mediaStorePortNID); 
   	            String mediaUUID = MediaStoreServiceUtils.getMediaStoreFileNID(mediaStreamFileDoc, request, log, db);
   	            String parentDirNID = MediaStoreServiceUtils.getParentDirNID(mediaStreamFileDoc, request, log, db);
   	        
   	            String getOffsetAndTCRequestName = ""; 
   	            Element bookmarkEl=MRXMLUtils.getElementXpath(request.getMessage().getDocument(), ".//Data"); 
   	            Element bookmarkResponseEl=MRXMLUtils.getElementXpath(response, ".//Bookmark"); 
   	            if(bookmarkEl!=null){ 
   	               MRXMLUtils.replaceContentWithElement(bookmarkEl, bookmarkResponseEl);
   	            } 
   	            if ("".equals(TC)){
   	            	getOffsetAndTCRequestName = MRRequest.getRequestName(GetTimeCodeAtOffsetRequestHandler.class.getName());
   	            } else {
   	            	getOffsetAndTCRequestName = MRRequest.getRequestName(GetOffsetAtTimecodeRequestHandler.class.getName());
   	            }
   	      
   		        Document mediaroomRequestDoc = MediaRoomServiceUtils.getMediaRoomRequest(log, ServiceClient.MEDIAROOMSERVICE, getOffsetAndTCRequestName); 
   		        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(request.getMessage().getDocument()), "UTF-8"); 
   		 
   		        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT; 
   		        String toAgentJID = mediaStoreStreamAgentJID; 
   		        String toAgentName = MediaRoomServiceConst.SOURCESTREAMAGENT; 
   		        MRRequest mediaroomRequest = MRRequest.createRequest(request, mediaroomRequestDoc, getOffsetAndTCRequestName, MRConst.MEDIA_SERVICE, clientData); 
   		        Element getOffsetAtTimeCodeRequestDataEL = MRXMLUtils.getElementXpath(mediaroomRequestDoc, mediaroomRequest.getRequestDataXPath()); 
   		        MRXMLUtils.addElement(getOffsetAtTimeCodeRequestDataEL, MRXMLUtils.newElement("mediaUUID"));
   		        MRXMLUtils.addElement(getOffsetAtTimeCodeRequestDataEL, MRXMLUtils.newElement("prentDirNID")); 
   		        MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//timecode", TC); 
   		        MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//offset", offset); 
   		        MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//mediaUUID", mediaUUID); 
   		        MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//streamNID", streamNID);
   		        MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//prentDirNID", parentDirNID);
   		 
   		        String dataStr = MRXMLUtils.elementToString(getOffsetAtTimeCodeRequestDataEL); 
   		        AppServerAgent.sendAgentRequestForUserRequest(mediaroomRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, fromAgentName, toAgentJID, toAgentName, dataStr, this.getClass().getName()); 

            }
            return response;
        } catch (MRException exp) {
            exp.printStackTrace();
            throw new MRException(exp.getCode(), exp.getMessage());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(MediaRoomServiceExceptionCodes.ADD_BOOK_MARK_FAILED, exp.toString());
        }
    }
   
    
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    	Document responseDoc = request.getResult();
    	String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
        Document requestXML = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));

        String getCurrentTimeStampRequestDocClientData = MRXMLUtils.getValueXpath(requestXML, "//"+MRConst.CLIENT_DATA, "");
        Document userRequestDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(getCurrentTimeStampRequestDocClientData, "UTF-8"));
        String requestName =  MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.REQUEST_NAME, "");
        Element bookmarkEl = MRXMLUtils.getElementXpath(userRequestDoc, "//Bookmark");
        String bookmarkNID = MRXMLUtils.getAttributeValue(bookmarkEl,"NID");
        Document bookmarkDoc = MRXMLDBUtils.readElement(request, log, db, bookmarkNID);
        if (requestName.equals(MediaRoomServiceConst.GETTIMECODEATOFFSETREQUEST)){
        	 String atTC = MRXMLUtils.getValueXpath(responseDoc, "//StreamCurrentTimeStamp", "");
             MRXMLUtils.getElementXpath(bookmarkDoc, "//AtTC").setText(atTC);
        } else if (requestName.equals(MediaRoomServiceConst.GETOFFSETATTIMECODEREQUEST)){
        	 String offset = MRXMLUtils.getValueXpath(responseDoc, "//StreamOffset", "");
             MRXMLUtils.getElementXpath(bookmarkDoc, "//Offset").setText(offset);
        }
        MRXMLDBUtils.updateElement(request, log, db, bookmarkNID, bookmarkDoc.getRootElement());
        // System.out.println("AddBookmarkRequestHandler:handleAgentResponse updated Bookmark doc=" +
        // 		MRXMLUtils.documentToString(bookmarkDoc));
        log.addInfo("AddBookmarkRequestHandler:handleAgentResponse updated bookmark NID=" + bookmarkNID, 
        		MRLog.INFO, MRLog.NOTIFY);
         /*
         String mediaStreamFileNID = MRXMLUtils.getValueXpath(userRequestDoc, "//MediaStreamFileResourceNID", "");
         MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);
         Element bookmarkEl = MRXMLUtils.getElementXpath(userRequestDoc, "//Bookmark");
		if(bookmarkEl!=null){
			Document updateResponseDoc = MediaRoomServiceUtils.requestUpdateBookMarkList(request,atTC,streamOffset,log, xmpp, db,bookmarkEl, resourceFactory, serviceAgentFactory);
			//System.out.println(atTC+"UpdateResponse$$$$$$$$$$$$"+MRXMLUtils.documentToString(updateResponseDoc));
		}
		*/
    }
  
}
