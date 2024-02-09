package com.ipvs.mediastoreservice.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.UpgradeMediaStreamFileResourceRequestHandler;
import com.ipvs.client.ServiceClient;
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
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.utils.Utils;


public class UpgradeMediaStoreDirRequestHandler implements MRRequestHandlerI {
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

    @SuppressWarnings("unchecked")
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document requestDoc = request.getMessage().getDocument();
        String mediaStreamDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, "");
        boolean upgradeMediaStreamFiles = Utils.booleanValue(MRXMLUtils.getValueXpath(requestDoc, "//UpgradeMediaStreamFiles", "false"), false);
        MediaStoreAdminServerAgent storeAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(request.getUserJID());
        MediaStoreDirectoryInfo storeDirInfo = storeAgent.getMediaStoreInfoBasedOnDirNID(mediaStreamDirNID);
        MediaStoreServerI storeServer = storeAgent.getMediaStoreServer();

        //get the dir UUID from the drive.
        Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(mediaStreamDirNID, request, db, log);
        String dirUUID = storeServer.upgradeMediaStoreDir(storeDirInfo.getMediaStoreRootPath());

        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.UUID, dirUUID);
        //upgrade all files in directory...
        if (upgradeMediaStreamFiles) {
            log.addInfo(" UPGRADE FILES..............................UpgradeMediaStoreDirRequestHandler " + mediaStreamDirNID + " " + upgradeMediaStreamFiles, MRLog.INFO, MRLog.NOTIFY);
            try {
                List<Element> mediaFileList = (List<Element>) MediaStoreServiceUtils.getMediaStreamFileResourceNIDList_readonly(mediaStreamDirNID, request.getUserJID(), log, db);
                if (mediaFileList != null && !mediaFileList.isEmpty()) {
                    for (Element mediaFileEL : mediaFileList) {
                        String NID = MRXMLUtils.getAttributeValue(mediaFileEL, "NID", "").trim();
                        try {
                            callUpgradeMediaStreamFileResource(NID, request.getUserJID(), xmpp, log, db, resourceFactory, profileFactory);
                        } catch (Exception ex) {
                            log.addInfo("ERROR:FileSM:UpgradeMediaStoreDirRequestHandler: upgrade error NID :='" + NID+ "'", MRLog.INFO, MRLog.NOTIFY);
                            ex.printStackTrace();
                        }
                    }
                } else {
                    log.addInfo(" NO FILES TO UPGRADE ..............................UpgradeMediaStoreDirRequestHandler ", MRLog.INFO, MRLog.NOTIFY);
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        } else {
            log.addInfo(" DO NOT UPGRADE FILES..............................UpgradeMediaStoreDirRequestHandler " + mediaStreamDirNID + " " + upgradeMediaStreamFiles, MRLog.INFO, MRLog.NOTIFY);
        }

        MediaStoreServiceUtils.updateMediaStreamDirDocInDB(mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);

        return MediaStoreServiceUtils.generateResponseDocument(MRRequest.getResponseName(request.getRequestName()), "", "", "", MediaStoreServiceConst.SUCCESS_STATE, "", request.getNID(), log);
    }
    
    private static void callUpgradeMediaStreamFileResource(String NID, String userID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory ) throws Exception{
        String requestName = AMConst.UPGRADE_MEDIA_STREAM_FILE_REQUEST;//"UpgradeMediaStreamFileResourceRequest";
        String requestXMLFile = "/com/ipvs/assetadminservice/xml/"+requestName+".xml";
        Document doc = MRXMLUtils.loadXMLResource(requestXMLFile, log);
        MRXMLUtils.setValueXpath(doc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, NID);
        MRRequest request2 = MRRequest.createRequest(MRRequest.createRequest(userID), doc, requestName, ServiceClient.ASSETADMINSERVICE);
        UpgradeMediaStreamFileResourceRequestHandler handler = new UpgradeMediaStreamFileResourceRequestHandler();
        //log.addInfo("After Import : Updgrade clip NID :=" + NID, MRLog.INFO, MRLog.NOTIFY);
        handler.getResponse(request2, xmpp, log, db, resourceFactory, profileFactory);

    }
}
