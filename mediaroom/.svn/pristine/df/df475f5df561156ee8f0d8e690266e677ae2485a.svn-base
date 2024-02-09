package com.ipvs.mediaroomservice.handler;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;


public class FileActionRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.STREAM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        // If destNID is empty then use the request.getResourceGroup
        // as the destGroup to find the matching policy
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID,
                streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    //TBD : Move all the file operations add dest file, add book mark , update book mark ,update meta data and sdelete dest file
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {      
        return request.createResponseData("", "");
    }
}
