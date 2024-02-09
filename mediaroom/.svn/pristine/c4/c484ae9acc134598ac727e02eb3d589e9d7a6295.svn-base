package com.ipvs.mediastoreservice.handler;

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
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class GetMediaStoreFileTrackRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document requestDoc = request.getMessage().getDocument();
        String mediaStoreFileNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID, "");
        String mediaStreamFileNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
        MediaStoreAdminServerAgent agent = MediaStoreAdminServerAgent.getMediaStoreAgent(request.getUserJID());
        Document mediaRoomRequestDocument = MRXMLUtils.getDocumentXpath(requestDoc, "//ActionData/*");
        MediaStoreServerI mediaStoreServer = agent.getMediaStoreServer();

        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);
        String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//MediaStoreFileInfo/Type", "");

        String rootDir = agent.getMediaStoreRootPath(mediaStreamFileDoc);
        String storeFileVersion = mediaStoreServer.getVersionOfMediaStoreFile(rootDir, mediaStoreFileNID);
        mediaStoreServer = agent.getMediaStoreServer(storeFileVersion);

        //Get Media Store Track 
        Element mediaStoreTarckEL = mediaStoreServer.getMediaStoreTrack(mediaStoreFileNID, mediaRoomRequestDocument, rootDir, mediaType);
        String responseXML = null;

        if (mediaStoreTarckEL == null) {
            responseXML = "";
        } else {
            responseXML = MRXMLUtils.elementChildrenToString(mediaStoreTarckEL);
        }

        Document responseDoc = MediaStoreServiceUtils.generateResponseDocument(MRRequest.getResponseName(request.getRequestName()), mediaStoreFileNID, mediaStreamFileNID, responseXML, MediaStoreServiceConst.SUCCESS_STATE, "", request.getNID(), log);

        return responseDoc;
    }
}
