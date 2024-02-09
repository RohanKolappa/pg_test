package com.ipvs.sessionservice.service.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.appserver.ServiceConfigManager;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;

public class SetWriterOptionsRequestHandler implements MRRequestHandlerI {

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		log.addInfo("SetWriterOptionsRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    
    	Document requestDoc = request.getMessage().getDocument();
    	
    	String requestElementXpath = "//SetWriterOptions";
		String enableHLS = MRXMLUtils.getAttributeValueXpath(requestDoc, requestElementXpath, "enableHLS", "false");
    	
    	ServiceConfigManager serviceConfigMgr = AppServerAgentUtils.getServiceConfigManager(serviceAgentFactory);
    	String serviceConfigNID = serviceConfigMgr.getServiceConfigNID();
    	Element serviceConfigEl = serviceConfigMgr.getServiceConfigElement();
    	Element writerOptionsEL = MRXMLUtils.getElementXpath(serviceConfigEl, ".//WriterOptions");
    	MRXMLUtils.setAttributeValue(writerOptionsEL, "enableHLS", enableHLS);
    	
    	MRXMLDBUtils.updateElement(request, log, db, serviceConfigNID, serviceConfigEl);
    	serviceConfigMgr.updateServiceConfigWriterOptions(writerOptionsEL);
    	return request.createResponseUsingRequestRootName(null);
	}

	
	@Override 
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
	        log.addTraceBegin(request);
	        int authType = 1;
	        int role = 2;
	        String token = "testCommonToken";
	        String authority = "testAuthority";
	        String roomPolicyNID = MRConst.NULL;
	        String streamPolicyNID = MRConst.NULL;

	        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
	        log.addTraceEnd(request);
	        return auth;

	    }

}