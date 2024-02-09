package com.ipvs.mediaroomservice.handler;

import org.dom4j.Document;

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
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class GetSrcMediaFileTrackRequestHandler implements MRRequestHandlerI {
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
        throws MRException, Exception {
    	 try {
             Document requestDoc = request.getMessage().getDocument();
             String mediaStreamFileNID = MRXMLUtils.getValueXpath(requestDoc,"//"+ MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
             Document mediaStreamFileDoc = MediaStoreServiceUtils.checkMediaStreamFileState(mediaStreamFileNID, log, db, request);            
             MediaStoreServiceUtils.sendRequestToMediaStoreAgent(request.getRequestName(), mediaStreamFileDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);            
             return null;
         } catch (MRException exp) {
             exp.printStackTrace();
             throw new MRException(exp.getCode(), exp.getMessage());
         } catch (Exception exp) {
             exp.printStackTrace();
             throw new MRException(MediaStoreServiceExceptionCodes.GET_MEDIA_STORE_FILE_TRACK_FAILED, exp.toString());
         }
    }
    
     

}
