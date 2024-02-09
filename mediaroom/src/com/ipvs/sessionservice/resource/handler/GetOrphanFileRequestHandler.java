package com.ipvs.sessionservice.resource.handler;

import org.dom4j.Document;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.sessionservice.handler.base.GetFileBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class GetOrphanFileRequestHandler extends GetFileBaseRequestHandler{


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.ORPHAN_FILE + SessionServiceConst.XQUERYXML;
		return super.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, xqueryFileName,SessionServiceConst.GET_ORPHAN_FILE);
	}
}
