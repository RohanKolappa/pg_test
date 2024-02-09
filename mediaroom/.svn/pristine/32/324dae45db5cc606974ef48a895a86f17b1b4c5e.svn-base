package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRLog;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.BaseReportRequestHandler;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.ReportManager;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class AddReportRequestHandler extends BaseReportRequestHandler {
	
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	log.addInfo("AddReportRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.REPORT_OBJECT_TYPE);
    	
    	// Get the report object and delete the unnecessary element
		Document reportObjectXMLDoc = MRConfig.getObjectXMLDocument(ServiceClient.POLICYADMINSERVICE, SessionServiceConst.REPORT_OBJECT_TYPE, log);        
        
		Element requestEl = requestObject.getRequestElement();		
		String type = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.TYPE_ATTRIBUTE, "");
		
		if (type.equals(SessionServiceConst.POLICY))                    
        	MRXMLUtils.removeChild(reportObjectXMLDoc.getRootElement(), SessionServiceConst.REPORT_CONFIG_EL_NAME, SessionServiceConst.AUDIT_EL_NAME + SessionServiceConst.REPORT_CONFIG_EL_NAME);        	            
        else            
        	MRXMLUtils.removeChild(reportObjectXMLDoc.getRootElement(), SessionServiceConst.REPORT_CONFIG_EL_NAME, SessionServiceConst.POLICY + SessionServiceConst.REPORT_CONFIG_EL_NAME);
		
    	this.handleReportRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, reportObjectXMLDoc, SessionServiceConst.ADD, type);
    	
    	log.addInfo("AddReportRequestHandler::Validate for permissions and add Report to the database", MRLog.OK, MRLog.NOTIFY);            
        Document dbResponseDoc = AddObjectUtils.addObjectInDatabase(request, xmpp, log, db, resourceFactory, serviceAgentFactory, reportObjectXMLDoc, SessionServiceConst.REPORT_OBJECT_TYPE);
    	
        String NID = MRXMLUtils.getAttributeValueXpath(dbResponseDoc, "//" + requestObject.getObjectType(), SessionServiceConst.NID, "");
        requestObject.setNID(NID);       
        
        log.addInfo("AddReportRequestHandler::Adding report FTP task to the scheduler", MRLog.OK, MRLog.NOTIFY);
        ReportManager.getInstance().addReportTaskToScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, reportObjectXMLDoc, NID);
        
        log.addInfo("AddReportRequestHandler::handleResponse", MRLog.OK, MRLog.NOTIFY);
        Document responseDoc = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, 
        		SessionServiceConst.SELECTFORMAT_BASIC, GET_REPORT_REQUEST_XML, GetReportRequestHandler.class.getName()); 
		
        return responseDoc; 
        
    }
    
}