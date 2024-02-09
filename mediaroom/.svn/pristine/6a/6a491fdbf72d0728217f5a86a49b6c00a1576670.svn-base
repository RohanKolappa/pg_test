package com.ipvs.sessionservice.resource.handler;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.GetFileBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class GetFileRequestHandler extends GetFileBaseRequestHandler  implements EventLoggerI{


	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Document doc = request.getMessage().getDocument();
		Element getFileElement = MRXMLUtils.getElementXpath(doc, "//"+MRRequest.getRequestRootName(request.getRequestName()));

		String whereNID = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_NID,"");
		String detail = MRXMLUtils.getAttributeValue(getFileElement, "selectFormat", "BASIC");
		if(!whereNID.isEmpty() && !whereNID.contains(",")) { //execute alternative query if whereNID provided
			return getFileAlternativeForWhereNID(request, xmpp, log, db, resourceFactory, serviceAgentFactory, whereNID, detail);
		} else {
			String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceConst.FILE + SessionServiceConst.XQUERYXML;
			return super.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, xqueryFileName,SessionServiceConst.GET_FILE);
		}
	}
	
	@Override
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String status, String level, String errorDescription)
	throws Exception {
		Element getFileElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//"+ MRRequest.getRequestRootName(request.getRequestName()));
		String whereTextSubStr = MRXMLUtils.getAttributeValue(getFileElement, SessionServiceConst.WHERE_TEXT_SUB_STR, "");
		
		if(!whereTextSubStr.isEmpty()) {
			return new AuditEvent.Builder(AuditEventLogger.MEDIA_EVENT, AuditEventLogger.NAME_SEARCH, request.getUserJID(), level).operatorEl(request.getOperatorGroups()).
			target(whereTextSubStr).status(status).descriptionText(""+ errorDescription).build();
		}
		
		return null;
	}
}
