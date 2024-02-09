package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.sessionservice.handler.base.BaseReportRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.ReportManager;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.systemservice.handler.SystemServiceUtils;


public class DownloadReportRequestHandler extends BaseReportRequestHandler {
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		log.addInfo("DownloadReportRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.REPORT_OBJECT_TYPE);
    	
		Element requestEl = requestObject.getRequestElement();
		String toFileName = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.TO_FILE_NAME, "");
		
		log.addInfo("DownloadReportRequestHandler::Downloading report to local PC", MRLog.OK, MRLog.NOTIFY);
		
		Document exportReportInfoDoc = ReportManager.getInstance().runReport(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, SessionServiceConst.DOWNLOAD_REPORT);
		
		
		String liveDir = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.LIVE_DIR, "");
		String fromDirPath = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.OUTPUT_FILE_EL, "");
		String workAreaID = liveDir.replace(SessionServiceConst.REPORTS_DIR, "").replace("/WORKAREA_", "");
		String outputFileName = fromDirPath.substring(fromDirPath.lastIndexOf("/")+1); 
		
		if (fromDirPath.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.EXPORT_FILE_LOCATION_NOT_FOUND, "Location of file to export not found");
		} else if (toFileName.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.MISSING_DESTINATION_FILE_LOCATION, "Missing destination file location");
		}
		
		log.addInfo("DownloadReportRequestHandler::Download request sent to FileTransferAgent", MRLog.OK, MRLog.NOTIFY);
		String url = getUrl(request, xmpp, log, db, resourceFactory,workAreaID,outputFileName,
				serviceAgentFactory);
		
		
		String requestName = "StartDownloadingRequest";
		Document downloadDocument = SystemServiceUtils.getDownloadRequestDocument(requestName, log, toFileName, request.getUserJID(),url, "reports");
		sendRequestToAgent(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestName, downloadDocument, MRConst.FTP_SERVICE, request.getUserJID(), MRConst.FTP_SERVICE, this
				.getClass().getName());

		return null;
        
    }  
	
	
	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, Document userRequestdoc, MRRequest userRequest, Document agentResponseDoc) throws MRException, Exception {		
		
		RequestProcessor requestObject = new RequestProcessor(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.REPORT_OBJECT_TYPE);
    			
		Document reportObjectXMLDoc = GetObjectUtils.getObjectXMLFromDB(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				requestObject, "", "");
		userRequest.setLoggingDocument(reportObjectXMLDoc);
		
		return userRequest.createResponseData(MRXMLUtils.stringToElement("<" + SessionServiceConst.DOWNLOAD_REPORT_EL_NAME + "/>"));
	}
	
	
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String status, String level, String errorDescription)
    throws Exception {
    	return AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(log, request.getUserJID(), request.getMessage().getDocument(), request.getLoggingDocument(), 
    			request.getOperatorGroups(), status, level, errorDescription, SessionServiceConst.REPORT_OBJECT_TYPE, SessionServiceConst.DOWNLOAD_REPORT);
    }
	
	private String getUrl(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, String workAreaID, String outputFileName,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Element ApiKeyTableEl = AppServerAgentUtils.getAPIKeyTable();
		long now = xmpp.getCurrentTimeMillis();
		String timestamp = String.valueOf(now
				+ (SessionServiceResourceConst.REDIRECT_EXPIRE_TIME * 1000));
		String signature = HttpAuthentication.generateExpiryBasedSignature(ApiKeyTableEl,
				timestamp);
		String apiKey = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./"
				+ SessionServiceResourceConst.API_KEY_ELEMENT_NAME,
				SessionServiceResourceConst.KEY_ATTRIBUTE, "");

		String agentName = ServiceAgentUtils.getAgentName(request.getUserJID());
		Element appResourceInfo = AppServerAgentUtils.getMSResourceInfo(log,
				xmpp, db, serviceAgentFactory, request, ApiKeyTableEl,
				agentName, request.getUserJID());

		String msIPAddress = MRXMLUtils.getAttributeValue(appResourceInfo,
				"ipAddress", "");

		StringBuilder sb = new StringBuilder();
		sb.append("https://").append(msIPAddress)
				.append("/mediajs/reports/download?workAreaID=")
				.append(workAreaID).append("&fileName=").append(outputFileName)
				.append("&timestamp=").append(timestamp).append("&apiKey=")
				.append(apiKey).append("&signature=").append(signature)
				.append("&type=reports");

		return sb.toString();
	}
		
	
}
