package com.ipvs.assetadminservice.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.MediaManagementUtils;
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
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class CleanOrphanDirRequestHandler implements MRRequestHandlerI {
 
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws Exception {
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
        throws Exception {
        Document requestDoc = request.getMessage().getDocument();
        String mediaStreamDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_NID, "");

        if (mediaStreamDirNID.isEmpty()) {
            throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "MediaStreamDirNID Missing");
        }
        try {        	
	        MediaStoreServiceUtils.updateMediaStreamDirResourceState(mediaStreamDirNID, MediaStoreServiceConst.DELETING, request, xmpp, log, db, resourceFactory, profileFactory);
	        List<?> listOfSubDir = MediaStoreServiceUtils.getSubDirList(mediaStreamDirNID, request, xmpp, log, db, resourceFactory, profileFactory);
	
	        if ((listOfSubDir != null) && !listOfSubDir.isEmpty()) {
	            Element subDirEL = null;
	            String subDirNID = null;
	
	            for (Object object : listOfSubDir) {
	                subDirEL = (Element) object;
	                subDirNID = MRXMLUtils.getAttributeValue(subDirEL, MediaStoreServiceConst.NID, "");
	                MediaManagementUtils.handleCleanOrphanDirRequest(subDirNID, request, db, log, xmpp, resourceFactory, profileFactory);
	            }
	        }
	        MediaManagementUtils.handleCleanOrphanDirRequest(mediaStreamDirNID, request, db, log, xmpp, resourceFactory, profileFactory);
        } catch(Exception exp){
        	MediaStoreServiceUtils.updateMediaStreamDirResourceState(mediaStreamDirNID, MediaStoreServiceConst.OFFLINE, request, xmpp, log, db, resourceFactory, profileFactory);
        	throw exp;
        }

        return request.createResponseData();
    }
}
