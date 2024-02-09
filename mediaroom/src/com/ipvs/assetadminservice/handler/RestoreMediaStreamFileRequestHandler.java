package com.ipvs.assetadminservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
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
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.resource.handler.DeleteFileRequestHandler;


public class RestoreMediaStreamFileRequestHandler implements MRRequestHandlerI {
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
        String mediaStreamFileNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
        Document resDoc = MRXMLUtils.getDocument();
        Element rootRes = MRXMLUtils.addElement(resDoc, "Handler");
        MRXMLUtils.addElement(rootRes, "ObjectType", AMConst.MEDIA_STREAM_FILE_RESOURCE);
        String pemXPath = PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.DELETE_ROLE_ACTION,AMConst.MEDIA_STREAM_FILE_RESOURCE, request, xmpp, log, db, resourceFactory, profileFactory);
    	resDoc= new DeleteFileRequestHandler().handleDeleteMediaStreamFile(request, xmpp, log, db, resourceFactory, profileFactory, pemXPath,mediaStreamFileNID,  true,request.getMessage().getDocument(),AMConst.MEDIA_RESTORE_REQUEST);    	
    	return resDoc;        
    }
}
