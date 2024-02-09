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
import com.ipvs.mediaroomservice.handler.AddStreamActionRequestHandler;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class SetContentRequestHandler implements MRRequestHandlerI {

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

	boolean isDefined(String s) {
		return ((s != null) && !"".equals(s));
	}
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document requestDoc = request.getMessage().getDocument();
		Element contentElement = MRXMLUtils.getElementXpath(requestDoc, "//Content");
		String id = contentElement.attributeValue(SessionServiceConst.ID);
		String canvasId = contentElement.attributeValue(SessionServiceConst.CANVASID);		
		String destAgentJID = contentElement.attributeValue("agentJID");
		Element srcPortElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.SRC_PORT);
		String streamURL = srcPortElement.attributeValue(SessionServiceConst.STREAM_URL);
		Element dstPortElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.DST_PORT);
		String destPortElementID = dstPortElement.attributeValue("portId");
		String errorDesc = "";
		if(!isDefined(destPortElementID))
			errorDesc = SessionServiceConst.DST_PORT + " portElementID=" + destPortElementID;
		if(!isDefined(destAgentJID))
			errorDesc = SessionServiceConst.DST_PORT + " agentJID=" + destAgentJID;
		if(!"".equals(errorDesc))
			throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, errorDesc);

		AddStreamActionRequestHandler handler = new AddStreamActionRequestHandler();

		String roomNID = null;
		String streamNID = destPortElementID;
		if(isDefined(streamURL)) {
			Document streamDataDoc = MRXMLUtils.stringToDocument(
					"<data>" +
							"<destPortElementID>" + destPortElementID + "</destPortElementID>" +
							"<streamURL>" + streamURL + "</streamURL>" +
							"<canvasId>" + canvasId + "</canvasId>" +
							"<windowId>" + id + "</windowId>" +
					"</data>"
							);
			Document mediaSourceDataDoc = MRXMLUtils.stringToDocument("<data/>");
	
			handler.sendSetupRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					roomNID, streamNID, destAgentJID, streamDataDoc, mediaSourceDataDoc);
		}
		else
			handler.sendTeardownRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					roomNID, streamNID, destAgentJID);
		
		return MRXMLUtils.stringToDocument(MRXMLUtils.elementToString(contentElement));
	}


}
