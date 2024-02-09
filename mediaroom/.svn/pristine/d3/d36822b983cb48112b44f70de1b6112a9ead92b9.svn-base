package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;

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
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.handler.base.BaseReportRequestHandler;
import com.ipvs.sessionservice.impl.ReportManager;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class ExportReportRequestHandler extends BaseReportRequestHandler {
	
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.REPORT_OBJECT_TYPE);
		
		request.setSuperUser(true);
    	
    	log.addInfo("ExportReportRequestHandler::Initiating FTP process for report with NID = " + requestObject.getNID(), MRLog.INFO, MRLog.NOTIFY);
		exportToFTPServer(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
    			
        return request.createResponseData(MRXMLUtils.stringToElement("<" + SessionServiceConst.SUCCESS + "/>"));
    }
    
    private void exportToFTPServer(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject) throws Exception {
    			
		Document exportReportInfoDoc = null;
		
		String NID = requestObject.getNID();
		
		try	{			
			exportReportInfoDoc = ReportManager.getInstance().runReport(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, SessionServiceConst.FTP_REPORT);			
		} catch (Exception e) {			
			log.addInfo("ReportManager::Report generation process failed", MRLog.OK, MRLog.NOTIFY);				
			e.printStackTrace();
		}
		
		if (exportReportInfoDoc == null)
			return;
		
		String liveDir = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.LIVE_DIR, "");
		String workAreaDir = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.WORK_AREA_DIR_EL_NAME, "");		
		String exportURL = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.EXPORT_URL_EL_NAME, "");		
		String outputFile = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.OUTPUT_FILE_EL, "");
		String destFileName = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.DESTINATION_FILE_EL, "");
		String currentDate = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.CURRENT_DATE, "");
		String currentTime = MRXMLUtils.getValueXpath(exportReportInfoDoc, "//" + SessionServiceConst.CURRENT_TIME, "");
				
		log.addInfo("ExportReportRequestHandler::Execute FTP command to export report with NID = " + NID, MRLog.OK, MRLog.NOTIFY);
				
		String ftpState = ReportManager.getInstance().executeFTPCmd(outputFile, exportURL, destFileName, workAreaDir);				
		this.updateStatusElementsInObjectXML(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, 
				SessionServiceConst.LAST_EXPORT_STATUS_EL_NAME, ftpState, SessionServiceConst.FTP_REPORT, currentDate, currentTime);
		
		String[] systemCmd = {"rm", "-rf", liveDir};
		if(MediaRoomServiceUtils.executeSystemCommand(systemCmd) != 0) {
			log.addInfo("ExportReportRequestHandler::Deleting workarea folder failed. Report NID = " + NID, MRLog.OK, MRLog.NOTIFY);
		}
		
	}
    
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String status, String level, String errorDescription)
    throws Exception {
    	return AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(log, request.getUserJID(), request.getMessage().getDocument(), request.getLoggingDocument(), 
    			request.getOperatorGroups(), status, level, errorDescription, SessionServiceConst.REPORT_OBJECT_TYPE, SessionServiceConst.FTP_REPORT);
    }
		
    
}
