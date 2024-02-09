package com.ipvs.sessionservice.resource.handler;

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

public class GetObjectRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI  {
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
		Element getObjectElement = MRXMLUtils.getElementXpath(doc, "//GetObject");
		String whereXPath = MRXMLUtils.getAttributeValue(getObjectElement, "whereXPath", "");
		String type = MRXMLUtils.getAttributeValue(getObjectElement, "type", "");
		if(whereXPath == null) {
			whereXPath = "";
		}
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.OBJECT + SessionServiceConst.XQUERYXML;
		List<String> listOfWhereXpath = new ArrayList<String>();
		String sortBy = "";
		Document resultDoc = this.getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, getObjectElement, type, sortBy,listOfWhereXpath,xqueryFileName);
		MRXMLUtils.getElementXpath(resultDoc, "//GetObject").addAttribute("type", type);
		return resultDoc;
	}
	
	
	
}
