package com.ipvs.sessionservice.resource.handler;

import org.dom4j.Document;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;


public class RebootDeviceRequestHandler extends BaseDeviceRequestHandler{
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addInfo("RebootDeviceRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		sendAgentRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "RebootDeviceRequest");
		return request.createResponseData();
	}

	
}