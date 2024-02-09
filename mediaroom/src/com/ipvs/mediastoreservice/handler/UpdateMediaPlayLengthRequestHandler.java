package com.ipvs.mediastoreservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.AsynchDBRequestResponseListenerI;
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
import com.ipvs.mediastoreservice.impl.MediaStoreAsynchCallBackData;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class UpdateMediaPlayLengthRequestHandler implements MRRequestHandlerI,AsynchDBRequestResponseListenerI {
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
        Document actionData = MRXMLUtils.getDocumentXpath(requestDoc, "//ActionData");
        MediaStoreAdminServerAgent agent = MediaStoreAdminServerAgent.getMediaStoreAgent(request.getUserJID());

        MediaStoreServerI mediaStoreServer = agent.getMediaStoreServer();
        
        String mediaStreamFileDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, "");
        MediaStoreDirectoryInfo storeDirInfo = agent.getMediaStoreInfoBasedOnDirNID(mediaStreamFileDirNID);
        String rootDir = storeDirInfo.getMediaStoreRootPath();

        int revision = mediaStoreServer.getNewRevisionNumber(rootDir);
        MediaStoreAsynchCallBackData asynCallBackDataObject = new MediaStoreAsynchCallBackData(request, log, db, xmpp, resourceFactory, profileFactory);        
        mediaStoreServer.updateMediaPlayLength(mediaStoreFileNID, mediaStreamFileDirNID, revision, actionData, rootDir,this,asynCallBackDataObject); 
        return null;
    }

	
    public void handleAsynchDBRequestResponse(String dbAction,Element responseElement, Object callBackDataObject, int status, String logData, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {	
    	MediaStoreAsynchCallBackData asynchCallBackDataObject  = (MediaStoreAsynchCallBackData) callBackDataObject;
    	
    	
    	MRRequest request = asynchCallBackDataObject.getRequest();
    	Document requestDoc =request.getMessage().getDocument();
    	MediaStoreAdminServerAgent agent = MediaStoreAdminServerAgent.getMediaStoreAgent(request.getUserJID());

    	String mediaStreamFileDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, "");
    	MediaStoreDirectoryInfo storeDirInfo = agent.getMediaStoreInfoBasedOnDirNID(mediaStreamFileDirNID);

    	//update media dir size
    	long sizeUsed = MediaStoreServiceUtils.updateMediaDirSizeUsed(storeDirInfo.isInternal(), storeDirInfo.getStoreDeviceElementID(), storeDirInfo.getMediaStoreDirNID(), 
    			request, asynchCallBackDataObject.getXmpp(), asynchCallBackDataObject.getLog(), asynchCallBackDataObject.getDb(), 
    			asynchCallBackDataObject.getResourceFactory(), asynchCallBackDataObject.getProfileFactory());

    	if (sizeUsed != 0) {
    		storeDirInfo.setSizeUsed(sizeUsed);
    	}
    	//send request to app server to update group file 
        String mediaStreamFileNID = MRXMLUtils.getAttributeValueXpath(responseElement, ".//" + AMConst.MEDIA_STREAM_FILE_RESOURCE,MediaStoreServiceConst.NID, "");
        MediaStoreServiceUtils.sendUpdateMediaGroupPlayLenghtAndSizeRequesToAppserver(mediaStreamFileNID,asynchCallBackDataObject.getXmpp(),asynchCallBackDataObject.getDb(),asynchCallBackDataObject.getLog(),request.getUserJID(), serviceAgentFactory);
    	asynchCallBackDataObject.handleAsynchRequestResponse(responseElement,status, logData);
    }
}
