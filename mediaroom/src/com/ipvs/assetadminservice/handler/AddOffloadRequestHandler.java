package com.ipvs.assetadminservice.handler;

import org.dom4j.Document;

import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;

public class AddOffloadRequestHandler extends ExportOffloadBaseRequestHandler {
	
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	return super.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, FileSM.TYPE_OFFLOAD);
    }
	
	@Override
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		super.handleAgentResponse(responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory, FileSM.TYPE_OFFLOAD);
    }

}
