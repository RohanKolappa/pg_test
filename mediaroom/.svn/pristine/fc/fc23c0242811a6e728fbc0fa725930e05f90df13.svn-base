package com.ipvs.sessionservice.session.handler;

import java.util.ArrayList;
import java.util.List;

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
import com.ipvs.sessionservice.handler.base.GetBaseObjectListRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class GetSessionConfigRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI  {
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
		Element getFileElement = MRXMLUtils.getElementXpath(doc, "//GetSessionConfig");
		List<String> listOfWhereXpath = new ArrayList<String>();
		String sortBy = MRXMLUtils.getAttributeValue(getFileElement, "sortBy", "");
		if(sortBy.equalsIgnoreCase(SessionServiceConst.TITLE)) {
			sortBy = SessionServiceConst.SESSION_CONFIG_TITLE;
		} else {
			sortBy = "";
		}
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.SESSIONCONFIG + SessionServiceConst.XQUERYXML;
		Document resultDoc = this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getFileElement, SessionServiceConst.SESSIONCONFIG, sortBy, listOfWhereXpath, xqueryFileName);
		GetObjectUtils.subscribeUser(getFileElement, resultDoc, SessionServiceConst.SESSIONCONFIG, request, serviceAgentFactory);
		return resultDoc;
		
		
	}
	
	
	
}
