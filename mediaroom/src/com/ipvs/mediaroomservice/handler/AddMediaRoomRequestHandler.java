package com.ipvs.mediaroomservice.handler;

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
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.RoomDB;

public class AddMediaRoomRequestHandler implements MRRequestHandlerI {
	
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
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
	

	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

	  com.ipvs.common.XMPPTransaction.startBlock("AddMediaRoomRequest");

      Document doc = request.getMessage().getDocument();
      // System.out.println("AddMediaRoomRequest doc=" + MRXMLUtils.documentToString(doc));

      String id = MRXMLUtils.getValueXpath(doc, "//" + MediaRoomServiceConst.ID, "");
	  String title = MRXMLUtils.getValueXpath(doc, "//Title", "");
	  String description = MRXMLUtils.getValueXpath(doc, "//Description", "");
	  String roomType = MRXMLUtils.getValueXpath(doc, "//roomType", "");
	  String syncData = MRXMLUtils.getValueXpath(doc, "//" + MediaRoomServiceConst.SYNCDATA, "");
	  String roomOwnerJID = request.getUserJID();
	  String streamSyncData = syncData;
	  String recordSyncData = syncData;
	  Element group  = MRXMLUtils.getElementXpath(doc, "//Groups");
	  
	  // String parentNID = db.getRoomRootNID();
	  long wallClock = xmpp.getCurrentTimeMillis();
	  String roomNID = RoomDB.getRoomDB().createMediaRoomDocument(id, title, description, roomOwnerJID, 
			  streamSyncData, recordSyncData, wallClock, group, roomType, request, log, db);

      // xmpp.addRoom(roomNID, ServiceAgentUtils.getRoomNickname(request.getAppServerJID()));
	  RoomDB.getRoomDB().sendInvite(xmpp, log, roomNID, request.getUserJID(), "");
	  com.ipvs.common.XMPPTransaction.endBlock("AddMediaRoomRequest");

	  return request.createResponseData("roomNID", roomNID);

	}

}
