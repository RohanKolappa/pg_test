package com.ipvs.sessionservice.window.handler;

import org.dom4j.Document;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.sessionservice.handler.base.SetBaseServiceClientDisplayRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class SetServiceClientDisplayStatusRequestHandler extends SetBaseServiceClientDisplayRequestHandler{


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		return updateDisplay(request, xmpp, log, db, resourceFactory, serviceAgentFactory,SessionServiceConst.SET_SERVICE_CLIENT_DISPLAY_STATUS, SessionServiceConst.IPVSCLIENTDISPLAYSTATUS);
	}

}
