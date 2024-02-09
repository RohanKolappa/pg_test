package com.ipvs.sessionservice.window.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.SetPortCommandsBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class SetPortWindowPropertiesRequestHandler extends SetPortCommandsBaseRequestHandler {


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document responseDoc =  handleSetCommands(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.WINDOW_PROPERTIES);
		if(responseDoc!=null) {
			responseDoc = updateResponseDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory,responseDoc);
		}
		return responseDoc;
	}

	protected Document updateResponseDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory, Document responsedoc) throws MRException,Exception{
		Element windowPropertiesEl = MRXMLUtils.getElementXpath(responsedoc, "//"+SessionServiceConst.WINDOW_PROPERTIES);
		String windowId= MRXMLUtils.getAttributeValueWithAlternate(request.getMessage().getDocument().getRootElement(), SessionServiceWindowConst.BY_WINDOW_ID, SessionServiceConst.WHERE_WINDOW_ID, "");
		MRXMLUtils.addAttribute(windowPropertiesEl, SessionServiceConst.WINDOWID, windowId);
		return responsedoc;
	}

}
