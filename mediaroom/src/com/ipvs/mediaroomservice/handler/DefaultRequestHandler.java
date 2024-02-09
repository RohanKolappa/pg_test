package com.ipvs.mediaroomservice.handler;


import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;

public class DefaultRequestHandler implements MRRequestHandlerI {

	public MRAuthorization authorizeRequest(MRRequest request,XMPPI xmpp, MRLog log, MRDB schema, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    throw new MRException(MRExceptionCodes.INVALIDREQUEST, MRXMLUtils.generateErrorXML("request", request.getRequestName()));
	}

	public Document getResponse(MRRequest request,XMPPI xmpp, MRLog log, MRDB schema, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		throw new MRException(MRExceptionCodes.INVALIDREQUEST, MRXMLUtils.generateErrorXML("request", request.getRequestName()));
	}
	
}
