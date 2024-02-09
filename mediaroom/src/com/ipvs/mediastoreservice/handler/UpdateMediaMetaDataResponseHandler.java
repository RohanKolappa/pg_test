package com.ipvs.mediastoreservice.handler;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;


public class UpdateMediaMetaDataResponseHandler implements AgentResponseHandlerI {
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
       UpdateMediaStoreFileResponseHandler.handleResponse(responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
       
    }
}
