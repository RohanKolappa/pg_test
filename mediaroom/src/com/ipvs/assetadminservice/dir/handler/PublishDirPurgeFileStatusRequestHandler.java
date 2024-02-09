package com.ipvs.assetadminservice.dir.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
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
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.resource.handler.StartDirCapacityManagementRulesRequestHandler;

public class PublishDirPurgeFileStatusRequestHandler implements MRRequestHandlerI {
	@Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
            MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) throws Exception {
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
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Element requestEl =MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//PublishDirPurgeFileStatus");
		String dirNID = MRXMLUtils.getAttributeValue(requestEl,SessionServiceResourceConst.BY_NID_ATTRIBUTE,"");
		
		new PublishDirSizeRequestHandler().updateDirSize(requestEl, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				
		//resume dir ILM or threshold capacity management task.
		Document requestDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.START_DIR_CAPACITY_MANAGEMENT_RULES_REQUEST_XML, log);
		String requestName = MRRequest.getRequestName(StartDirCapacityManagementRulesRequestHandler.class.getName());
		
		Element getFileEL = MRXMLUtils.getElementXpath(requestDoc,"//"+MRRequest.getRequestRootName(requestName));
		MRXMLUtils.setAttributeValue(getFileEL, SessionServiceResourceConst.BY_NID_ATTRIBUTE, dirNID);
		
		Document callBackRequestDoc = SessionServiceUtils.getRequestXML(requestName, log,getFileEL);
		MRRequest callbackRequest = MRRequest.createRequest(request, callBackRequestDoc, requestName, ServiceClient.SESSIONSERVICE_RESOURCE);
		callbackRequest.setSuperUser(true);
		StartDirCapacityManagementRulesRequestHandler handler = new StartDirCapacityManagementRulesRequestHandler();
	    log.addInfo("CallBack StartDirCapacityManagementRules: byNID=" + dirNID, MRLog.INFO, MRLog.NOTIFY);
        handler.getResponse(callbackRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
				
		return request.createResponseData();
	}
}
