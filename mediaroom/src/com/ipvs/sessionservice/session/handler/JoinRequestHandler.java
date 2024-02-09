package com.ipvs.sessionservice.session.handler;

import org.dom4j.Document;
import org.dom4j.Element;

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
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.sessionservice.impl.SessionServiceUtils;

public class JoinRequestHandler implements MRRequestHandlerI{

	@Override
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
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

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document doc = request.getMessage().getDocument();
		String sessionID = MRXMLUtils.getAttributeValueXpath(doc, "//Join","sessionId", null);
		Document roomDoc = SessionServiceUtils.getMediaRoomDoc(sessionID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    	Element roomElement = MRXMLUtils.getElementXpath(roomDoc, "//MediaRoom");
		if (roomElement==null){
			throw new MRException(MediaRoomServiceExceptionCodes.ROOMIDNOTFOUND, MRXMLUtils.generateErrorXML("sessionID", sessionID));
		}
		String roomNID = MRXMLUtils.getAttributeValue(roomElement,"NID");
		RoomDB.getRoomDB().sendInvite(xmpp, log, roomNID, request.getUserJID(), null);
		return request.createResponseData("roomNID",roomNID);

	}

}
