package com.ipvs.policyadminservice.handler;


import org.dom4j.Document;

import com.ipvs.assetadminservice.impl.BaseManagementRequestHandler;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;

public class PolicyManagementRequestHandler extends BaseManagementRequestHandler implements MRRequestHandlerI {
    private static String serviceName = ServiceClient.POLICYADMINSERVICE;

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        log.addTraceBegin(request);

        return  GetResponseBasedOnAction(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, serviceName);
    }
	
}
