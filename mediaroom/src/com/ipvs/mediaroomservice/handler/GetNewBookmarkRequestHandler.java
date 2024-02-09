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
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.utils.Utils;


public class GetNewBookmarkRequestHandler implements MRRequestHandlerI, AgentResponseHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        String streamNID = null;
        Element data = MediaRoomServiceUtils.cloneDataElement(request);
        streamNID = MRXMLUtils.getValueXpath(data, "streamNID", "");

        if ((streamNID == null) || (streamNID.trim().length() == 0)) {
            throw new MRException(MediaRoomServiceExceptionCodes.STREAMNIDNOTFOUND, "StreamNID Not Found");
        }
        
        String roomNID = MRXMLUtils.getValueXpath(data, ".//roomNID", "");
  	  	if("".equals(roomNID))
  	  		roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, streamNID);

        Document streamDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID));

        Document mediaSrcDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID", "")));

        String sourceType = MRXMLUtils.getValueXpath(mediaSrcDoc, "//sourceType", "");
        String toAgentJID = null;
        String toServiceName = MRConst.MEDIA_SERVICE;

   
        if (sourceType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)) {
            String trackNID = MRXMLUtils.getValueXpath(mediaSrcDoc, "//trackNID", "");

            Document trackDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(MRXMLUtils.getValueXpath(mediaSrcDoc, "//roomNID", ""), request, db, log, trackNID));
          
            long lastTrackOffset = Utils.getLongValue(MRXMLUtils.getValueXpath(trackDataDoc, "//lastTrackOffset",""), 0);
            long now = xmpp.getCurrentTimeMillis();
            long lastActionWallClock =Utils.getLongValue(MRXMLUtils.getValueXpath(trackDataDoc, "//lastActionWallClock",""), 0);
      
            lastTrackOffset = lastTrackOffset + (now - lastActionWallClock);
     
            return getNewBookmarkResponseDoc(xmpp, "", String.valueOf(lastTrackOffset), request, log);
        } else {
            toAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//sourceAgentJID", "");

            if ((toAgentJID == null) || (toAgentJID.trim().length() == 0)) {
                throw new MRException(MediaRoomServiceExceptionCodes.AGENT_NOT_FOUND, "Agent JID Not Found");
            }

            String getCurrentTimeStampRequestName = MRRequest.getRequestName(GetCurrentTimeStampRequestHandler.class.getName());
            Document mediaroomRequestDoc = MediaRoomServiceUtils.getMediaRoomRequest(log, ServiceClient.MEDIAROOMSERVICE, getCurrentTimeStampRequestName);
            String clientData = URLEncoder.encode(MRXMLUtils.documentToString(request.getMessage().getDocument()), "UTF-8");
            MRXMLUtils.setValueXpath(mediaroomRequestDoc, "//streamNID", streamNID);

            String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
            String responseHandlerName = this.getClass().getName();
            MRRequest mediaroomRequest = MRRequest.createRequest(request, mediaroomRequestDoc, getCurrentTimeStampRequestName, ServiceClient.MEDIAROOMSERVICE, clientData);
            Element getCurrentTimeStampRequestData = MediaRoomServiceUtils.cloneDataElement(mediaroomRequest);
            String dataStr = MRXMLUtils.elementToString(getCurrentTimeStampRequestData);

            AppServerAgent.sendAgentRequestForUserRequest(mediaroomRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, fromAgentName, toAgentJID, toServiceName, dataStr, responseHandlerName);

            return null;
        }
    }

    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document responseDoc = request.getResult();

        String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");

        //get the MRRequest.xml document        
        Document requestXML = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));

        //get the GetNewBookmarkRequest document
        String getCurrentTimeStampRequestDocClientData = MRXMLUtils.getValueXpath(requestXML, "//"+MRConst.CLIENT_DATA, "");
        Document userRequestDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(getCurrentTimeStampRequestDocClientData, "UTF-8"));

        //get the current time stamp request object
        MRRequest getCurrentTimeStampRequest = MRRequest.unMarshalRequestObject(requestXML, null, "", log);

        String getNewBookmarkRequestName = MRRequest.getRequestName(GetNewBookmarkRequestHandler.class.getName());

        MRRequest userRequest = MRRequest.createRequest(getCurrentTimeStampRequest, userRequestDoc, getNewBookmarkRequestName, ServiceClient.MEDIAROOMSERVICE, MRXMLUtils.getValueXpath(userRequestDoc, "//"+MRConst.CLIENT_DATA, ""));

        //get the user request from get current time stamp request client data      
        int state = Integer.parseInt(MRXMLUtils.getAttributeValueXpath(responseDoc, MRConst.HEADER_XPATH,MRConst.STATE, "0"));
        String logData = MRXMLUtils.getValueXpath(responseDoc, "//Log", "");
        userRequest.setState(state);

        if (state == MRRequest.STATE_EXCEPTION) {
            String codeStr = MRXMLUtils.getValueXpath(responseDoc, "//Code", String.valueOf(MediaRoomServiceExceptionCodes.AGENTREQUESTFAILED));
            int code = Utils.getIntValue(codeStr, MediaRoomServiceExceptionCodes.AGENTREQUESTFAILED);
            String description = MRXMLUtils.getValueXpath(responseDoc, "//Description", "AgentRequestFailed");
            MRException mrException = new MRException(code, description);
            userRequest.setException(mrException);
        } else {
            //generate response element         
            String atTC = MRXMLUtils.getValueXpath(responseDoc, "//StreamCurrentTimeStamp", "");
            Document userResponseDoc = getNewBookmarkResponseDoc(xmpp, atTC, "", userRequest, log);
            userRequest.setResult(userResponseDoc);
        }

        responseHandler.handleResponse(userRequest, logData);
    }

    public Document getNewBookmarkResponseDoc(XMPPI xmpp, String atTC, String offset, MRRequest request, MRLog log)
        throws MRException, Exception {
        Document bookmarkDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.BOOKMARK, log);
        long xmpp_date = xmpp.getCurrentTimeMillis();      
        MRXMLUtils.setValueXpath(bookmarkDoc, "//AtTC", atTC);
        MRXMLUtils.setValueXpath(bookmarkDoc, "//Offset",offset);
        MRXMLUtils.setValueXpath(bookmarkDoc, "//" + MediaStoreServiceConst.DATE_CREATED, String.valueOf(xmpp_date));
        MRXMLUtils.setValueXpath(bookmarkDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, String.valueOf(xmpp_date));

        Element streamNIDEL = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//streamNID");

        Document responseDoc = request.createResponseData();
        Element userResponseRootEL = responseDoc.getRootElement();
        MRXMLUtils.addElement(userResponseRootEL, (Element) streamNIDEL.clone());
        MRXMLUtils.addElement(userResponseRootEL, (Element)bookmarkDoc.getRootElement().detach());

        return responseDoc;
    }
}
