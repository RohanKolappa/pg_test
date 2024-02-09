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

public class SetPortLayoutPropertiesRequestHandler extends SetPortCommandsBaseRequestHandler {


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		Document responseDoc =  handleSetCommands(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.LAYOUT_PROPERTIES);
		if(responseDoc!=null) {
			responseDoc = updateResponseDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory,responseDoc);
		}
		return responseDoc;
	}

	protected Document updateResponseDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory, Document responsedoc) throws MRException,Exception{
		Element layoutPropertiesEl = MRXMLUtils.getElementXpath(responsedoc, "//"+SessionServiceConst.LAYOUT_PROPERTIES);
		String  layoutId= MRXMLUtils.getAttributeValueWithAlternate(request.getMessage().getDocument().getRootElement(), SessionServiceWindowConst.BY_LAYOUT_ID, SessionServiceConst.WHERE_LAYOUT_ID, "");
		MRXMLUtils.addAttribute(layoutPropertiesEl, SessionServiceConst.LAYOUT_ID, layoutId);
		return responsedoc;
	}

}
