package com.ipvs.sessionservice.handler.base;


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


public abstract class UserBaseRequestHandler implements MRRequestHandlerI {
	
	public static final String GET_USER_REQUEST_XML = "/com/ipvs/sessionservice/resource/xml/GetUserRequest.xml";
			
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

}
