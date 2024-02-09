package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.SetPortBaseRequestHandler;
import com.ipvs.sessionservice.impl.RequestProcessor;


public class SetPortObserverInfoRequestHandler extends SetPortBaseRequestHandler {
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		log.addInfo("SetPortObserverInfoRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "");
				
		// Get port objectXML doc 
		Document objectXMLDoc = this.getPortObjectXMLDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
		String parentElXPath = "//" + SessionServiceResourceConst.OBSERVER_INFO_ELEMENT;
			
		MRXMLUtils.replaceContent(objectXMLDoc, parentElXPath, requestObject.getRequestElement());
	
		return updatePort(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, objectXMLDoc, parentElXPath);
        
	}
	
}
