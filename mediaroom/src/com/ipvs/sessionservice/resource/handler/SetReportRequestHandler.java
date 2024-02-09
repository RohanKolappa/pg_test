package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.BaseReportRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.ReportManager;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;


public class SetReportRequestHandler extends BaseReportRequestHandler {
	
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	log.addInfo("SetReportRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.REPORT_OBJECT_TYPE);
		
		Document reportObjectXMLDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				requestObject, "", "");
        Element reportObjectXMLRootEl = reportObjectXMLDoc.getRootElement();
		
        String NID = MRXMLUtils.getAttributeValue(reportObjectXMLRootEl, SessionServiceConst.NID);
        String type = MRXMLUtils.getValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.TYPE_EL_NAME, "");
        
        requestObject.setNID(NID);
        
        this.handleReportRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, reportObjectXMLDoc, SessionServiceConst.SET, type);
        
        log.addInfo("SetReportRequestHandler::Check for permissions and update the database with the modified report object", MRLog.OK, MRLog.NOTIFY);            
        UpdateObjectUtils.updateObject(request, xmpp, log, db, reportObjectXMLRootEl, requestObject.getObjectType(), requestObject.getListOfWhereXpath(), requestObject.getRequestName());
        
        log.addInfo("SetReportRequestHandler::Updating report task in the scheduler", MRLog.OK, MRLog.NOTIFY);      
        ReportManager.getInstance().updateTaskInScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, reportObjectXMLDoc, NID);
        
        log.addInfo("SetReportRequestHandler::handleResponse", MRLog.OK, MRLog.NOTIFY);				
		Document responseDoc = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, 
				SessionServiceConst.SELECTFORMAT_BASIC, GET_REPORT_REQUEST_XML, GetReportRequestHandler.class.getName());
        
		return responseDoc; 
        
    }       

} 
