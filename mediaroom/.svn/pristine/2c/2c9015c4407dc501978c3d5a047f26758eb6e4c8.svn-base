package com.ipvs.systemservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.EventLoggerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.service.handler.BackupDatabaseRequestHandler;

public class ExportServiceDBRequestHandler implements MRRequestHandlerI,
		EventLoggerI {

	BackupDatabaseRequestHandler handler = new BackupDatabaseRequestHandler();
	private final String BACKUP_DB_REQUEST_NAME = "BackupDatabaseRequest";

	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		log.addTraceBegin(request);
		MediaRoomAuthorization auth = (MediaRoomAuthorization) handler
				.authorizeRequest(request, xmpp, log, db, resourceFactory,
						serviceAgentFactory);
		log.addTraceEnd(request);
		return auth;
	}

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		handler.getResponse(getRequest(request, BACKUP_DB_REQUEST_NAME, log),
				xmpp, log, db, resourceFactory, serviceAgentFactory);
		return null;
	}

	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, String status, String level, String errorDescription)
			throws Exception {
		return handler.getAuditEvent(
				getRequest(request, BACKUP_DB_REQUEST_NAME, log), xmpp, log,
				db, status, level, errorDescription);
	}

	private MRRequest getRequest(MRRequest request, String requestName,
			MRLog log) throws Exception {
		Document reqDoc = getRequestDocument(request, requestName, log);
		return MRRequest.createRequest(request, reqDoc, requestName,
				MRConst.SESSION_SERVICE_SERVICE);
	}

	private Document getRequestDocument(MRRequest request, String requestName,
			MRLog log) throws Exception {

		Document requestDoc = request.getMessage().getDocument();
		String toFileName = MRXMLUtils.getValueXpath(requestDoc,
				"//LocalFileName", "default");

		Document requestDocTemplate = MRXMLUtils.loadXMLResource(
				SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document newRequestDoc = (Document) requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(newRequestDoc.getRootElement(),
				MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
		MRXMLUtils.setAttributeValueXPath(newRequestDoc.getRootElement(),
				MRConst.HEADER_XPATH, MRConst.SERVICE_NAME,
				MRConst.SESSION_SERVICE_SERVICE);
		Element DataEL = MRXMLUtils.getElementXpath(newRequestDoc, "//Data");
		Element requestDataEl = MRXMLUtils.addElement(DataEL,
				MRXMLUtils.newElement(requestName + "Data"));
		Element reqEle = MRXMLUtils.addElement(requestDataEl,
				MRXMLUtils.newElement(requestName.replace("Request", "")));
		Element localFileNameEle = MRXMLUtils.addElement("LocalFileName",
				toFileName);
		MRXMLUtils.addElement(reqEle, localFileNameEle);
		log.addInfo("Converted request to support new handler: " + newRequestDoc.asXML());
		return newRequestDoc;
	}
	
	
}
