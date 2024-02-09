package com.ipvs.systemservice.handler;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.app.IPVSClientApplet;
import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;


public class DeviceLogoutRequestHandler implements MRRequestHandlerI{

	@Override
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		AppServerAgent theAppServerAgent = (AppServerAgent) serviceAgentFactory.getServiceAgent(IPVSClientApplet.APPSERVERAGENT);
		Element requestDataElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), request.getRequestDataXPath());
		String userJID = requestDataElement.attributeValue("userJID");
        AuthTokenCache.UserResource userResource  = AppServerAgentUtils.getAuthTokenCache(serviceAgentFactory).getUserResource(StringUtils.substringBefore(userJID, "@localhost"));
        String resource = StringUtils.substringAfter(userJID, "@localhost");
     	//find user exists in REST cache do not call handlepresence unavaialble. We are handling rest user presence from logoutrequesthandler or user gc.
		if(userResource == null || !userResource.getResource().equals(resource)) {
			theAppServerAgent.handleDeviceUnavailable(userJID);
		}
		
		return request.createResponseData();
	}
}
