package com.ipvs.sessionservice.window.handler;

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
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class GetDisplayProfileRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI{

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
		Element getDisplayProfileElement = MRXMLUtils.getElementXpath(doc, "//GetDisplayProfile");
		String sortBy = MRXMLUtils.getAttributeValue(getDisplayProfileElement, "sortBy", "");
		
		List<String> listOfWhereXpath = new ArrayList<String>();
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.DISPLAYPROFILE + SessionServiceConst.XQUERYXML;
		Document resultDoc = this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getDisplayProfileElement, SessionServiceConst.DISPLAYPROFILE, sortBy,listOfWhereXpath, xqueryFileName);
		return resultDoc;
	}

}
