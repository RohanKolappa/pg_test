package com.ipvs.mediastoreservice.handler;

import org.dom4j.Document;

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
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class AddMediaStoreFileRequestHandler implements MRRequestHandlerI {
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
        Document mediaStreamFileDoc = MRXMLUtils.getDocumentXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
        MediaStoreAdminServerAgent agent = MediaStoreAdminServerAgent.getMediaStoreAgent(request.getUserJID());
        MediaStoreServerI mediaStoreServer = agent.getMediaStoreServer();
        String rootDir = agent.getMediaStoreRootPath(mediaStreamFileDoc);      
        int revision = mediaStoreServer.getNewRevisionNumber(rootDir);

        //Create Media Store File in blob dir.
        mediaStoreServer.createMediaStoreFile(mediaStoreFileNID, revision, mediaStreamFileDoc, rootDir);

        //Update latest revision in MediaStreamFileResource in Data Base
        MediaStoreServiceUtils.updateMediaStreamFileDoc(mediaStreamFileNID, revision, MediaStoreServiceConst.CREATED, request.getUserJID(), mediaStoreFileNID, xmpp, log, db, profileFactory);

        Document responseDoc = MediaStoreServiceUtils.generateResponseDocument(MRRequest.getResponseName(request.getRequestName()), mediaStoreFileNID, mediaStreamFileNID, "",
                MediaStoreServiceConst.SUCCESS_STATE, "", request.getNID(), log);

        return responseDoc;
    }
}
