package com.ipvs.mediastoreservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
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
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class RepairMediaRequestHandler implements MRRequestHandlerI {
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
        MediaStoreServerI mediaStoreServer = agent.getMediaStoreServer();
        Document mediaStreamFileDocInDB = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);

        MediaStoreDirectoryInfo storeInfo = agent.getMediaStoreInfoBasedOnDirNID(MRXMLUtils.getValueXpath(mediaStreamFileDocInDB, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, ""));
        
        String mediaStreamFileDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, "");
        String rootDir = agent.getMediaStoreRootPathBasedOnDirNID(mediaStreamFileDirNID);
        
        int revision = mediaStoreServer.getNewRevisionNumber(rootDir);
        Document mediaStoreFileDoc = mediaStoreServer.getMediaStoreFileDoc(mediaStoreFileNID, rootDir);
        long playLength = 0;
        long size = 0;
        Document mediaStreamFileDoc = null;

        //If file not there in the blob directory create the file. If UUID does not exists in blob dir then create blob/UUID dir.
        if (mediaStoreFileDoc == null) {
            mediaStoreServer.repairMediaStoreFile(mediaStoreFileNID, revision, mediaStreamFileDocInDB, rootDir);
            mediaStoreFileDoc = mediaStoreServer.getMediaStoreFileDoc(mediaStoreFileNID, rootDir);
        } else {
            //Temporary Fix: Repair the media store index file. 
            mediaStoreServer.repairMediaStoreIndexFile(mediaStoreFileNID, rootDir);
            playLength = mediaStoreServer.getMediaPlayLegth(mediaStoreFileNID, rootDir);
            size = mediaStoreServer.getMediaSize(mediaStoreFileNID, rootDir);
        }

        //Get the media stream file document by replacing the common information from media store.
        Element dirGroupsEL  = MediaStoreServiceUtils.getMediaStreamDirGroupsEL(storeInfo.getMediaStoreDirNID(), request, db, log);
        mediaStreamFileDoc = mediaStoreServer.generateMediaStreamFileDocFromStoreFile(mediaStoreFileDoc, mediaStreamFileDocInDB, mediaStoreFileNID, storeInfo, dirGroupsEL, revision, false, null);
           
        //update the size, playLength and State of media stream file doc
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//PlayLength", String.valueOf(playLength));
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//Size", String.valueOf(size));
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.READY);

        MediaStoreServiceUtils.updateDateModified(mediaStreamFileDoc, xmpp);

        String mediaStoreFileDocStr = mediaStoreServer.generateMediaStoreFileDoc(mediaStreamFileDoc, revision, MediaStoreServiceConst.READY, log);
        mediaStoreFileDoc = MRXMLUtils.stringToDocument(mediaStoreFileDocStr);

        //update latest copy in media store
        log.addInfo("Repair Media Store File. MediaStoreFileNID:=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
        mediaStoreServer.updateSynMediaStoreFile(mediaStoreFileNID, revision, mediaStoreFileDoc, rootDir);

        //update latest copy in data base 
        log.addInfo("Repair Media Stream File. MediaStreamFileNID:=" + mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);
        MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, revision);
        MRXMLDBUtils.updateElement(request, log, db, mediaStreamFileNID, mediaStreamFileDoc.getRootElement());
              
        String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, ""); 
                
        // send play length and size info to app server        
        if(!mediaGroupID.isEmpty()) {
            MediaStoreServiceUtils.sendUpdateMediaGroupPlayLenghtAndSizeRequesToAppserver(mediaStreamFileNID, xmpp, db, log, request.getUserJID(), profileFactory);	
    	}
        
        Document responseDoc = MediaStoreServiceUtils.generateResponseDocument(MRRequest.getResponseName(request.getRequestName()), mediaStoreFileNID, mediaStreamFileNID, "", MediaStoreServiceConst.SUCCESS_STATE, "", request.getNID(), log);

        return responseDoc;
    }
}
