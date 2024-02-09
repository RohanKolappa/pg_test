package com.ipvs.mediastoreservice.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

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

public class UpdateMediaAutoStorageRulesRequestHandler implements MRRequestHandlerI {
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
            List<?> fileList = MRXMLUtils.getListXpath(requestDoc, "//MediaStoreFile");                 
           
            Element mediaStoreFileListEl = MRXMLUtils.getElementXpath(requestDoc, "//MediaStoreFileList");
           
            MediaStoreAdminServerAgent agent = MediaStoreAdminServerAgent.getMediaStoreAgent(request.getUserJID());

            String mediaStreamFileDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, "");
            String rootDir = agent.getMediaStoreRootPathBasedOnDirNID(mediaStreamFileDirNID);
            
            Document actionDocument = MRXMLUtils.getDocumentXpath(requestDoc, "//ActionData");
            MediaStoreServerI mediaStoreServer = agent.getMediaStoreServer();
            int revision = mediaStoreServer.getNewRevisionNumber(rootDir);
            for(Object object:fileList) {
            	Element fileEL= (Element) object;            
            	String mediaStoreFileNID = MRXMLUtils.getValueXpath(fileEL, "./" + MediaStoreServiceConst.MEDIA_FILE_STORE_NID, "");
            	String mediaStreamFileNID = MRXMLUtils.getValueXpath(fileEL, "./" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
            	mediaStoreServer.updateFileBypassAutoRules(mediaStoreFileNID, mediaStreamFileNID, revision, actionDocument, rootDir);
            }
        
            Document responseDoc = MediaStoreServiceUtils.generateResponseDocument(MRRequest.getResponseName(request.getRequestName()),(Element)mediaStoreFileListEl.detach(), "",
                    MediaStoreServiceConst.SUCCESS_STATE, "", request.getNID(), log);

            return responseDoc;
        }
        


}
