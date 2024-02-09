package com.ipvs.assetadminservice.handler;

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
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.utils.Utils;


public class UpgradeMediaStreamDirResourceRequestHandler implements MRRequestHandlerI {
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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document requestDoc = request.getMessage().getDocument();
        String mediaStreamDirNID = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, "");
        boolean upgradeMediaStreamFiles = Utils.booleanValue(MRXMLUtils.getValueXpath(requestDoc, "//UpgradeMediaStreamFiles", "false"), false);

        if (mediaStreamDirNID.isEmpty()) {
            throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "MediaStreamDirResourceNID Missing");
        }

        MediaStoreServiceUtils.sendMediaStreamDirRequestToMediaStoreAdmin(request, mediaStreamDirNID, true, requestDoc, upgradeMediaStreamFiles, xmpp, log, db);

        return null;
    }
}
