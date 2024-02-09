package com.ipvs.sessionservice.resource.handler;

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
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.handler.base.GetBaseObjectListRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.FileUtils;

public class GetSystemConfigRequestHandler extends GetBaseObjectListRequestHandler implements MRRequestHandlerI  {
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

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document requestDoc = request.getMessage().getDocument();
		Element getSystemConfigEl = MRXMLUtils.getElementXpath(requestDoc, "//"+SessionServiceConst.GET_SYSTEM_CONFIG);
		String whereConfig = MRXMLUtils.getAttributeValue(getSystemConfigEl, SessionServiceConst.WHERE_CONFIG);
		
		if(whereConfig ==null || whereConfig.isEmpty()) {
			whereConfig = "ALL";
		}
		
		PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.READ_ROLE_ACTION,PolicyAdminConstants.SYSTEM_OBJECT, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
		request.setSuperUser(true);
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.SYSTEM_CONFIG + SessionServiceConst.XQUERYXML;		
		String query = FileUtils.readFileResource(xqueryFileName, request.getClass());
		query = query.replace(SessionServiceConst._WHERE_CONFIG_, whereConfig);
		Document queryResultDoc = GetObjectUtils.executeQuery(query, request, xmpp, log, db, serviceAgentFactory, resourceFactory);
		return request.createResponseData((Element)queryResultDoc.getRootElement().detach());

	}



}
