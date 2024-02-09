package com.ipvs.mediaroomservice.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestCache;
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
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;

public class CreateRelayConnectionRequestHandler implements MRRequestHandlerI {

	public MRAuthorization authorizeRequest(MRRequest request,XMPPI xmpp, MRLog log, MRDB schema, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addTraceBegin(request);
		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL,
				authType, role, roomPolicyNID, streamPolicyNID,
				token, authority);
		log.addTraceEnd(request);
		return auth;
	}

	public Document getResponse(MRRequest request,XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
//		System.out.println("=========================================== Create Relay Connection");
		request.setCache(new MRRequestCache());
		Document doc = ConnectionUtils.CreateConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "CreateRelayConnectionRequest", MediaRoomServiceConst.RELAY_CONNECTION);
		String roomNID = MRXMLUtils.getValueXpath(doc,"//roomNID", "");
		String streamNID = MRXMLUtils.getValueXpath(doc,"//streamNID", "");
		String profileNID = ""; // TBD
		Element profileXML = MRXMLUtils.stringToElement("<profileXML/>"); // TBD

		ConnectionUtils.addStreamAction(request,xmpp, log, db,resourceFactory, serviceAgentFactory, 
				request.getRequestName(), roomNID, streamNID,  profileNID,  profileXML, MediaRoomServiceConst.START);

		return doc;
	}
	
}
