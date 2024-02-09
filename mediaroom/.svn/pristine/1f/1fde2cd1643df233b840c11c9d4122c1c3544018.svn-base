package com.ipvs.common.service;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;

public interface MRRequestHandlerI {

	public abstract MRAuthorization authorizeRequest(MRRequest request,
			XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI profileFactory) throws Exception;
	
	public abstract Document getResponse(MRRequest request, 
			XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws Exception;

}
