package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRLog;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.BaseReportRequestHandler;
import com.ipvs.sessionservice.impl.DeleteObjectUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class DeleteReportRequestHandler extends BaseReportRequestHandler {
	
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	
    	log.addInfo("DeleteReportRequestHandler::Validate permissions and delete the report object from the db", MRLog.OK, MRLog.NOTIFY);		
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.REPORT_OBJECT_TYPE);
    	Document responseDoc = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, SessionServiceConst.SELECTFORMAT_EXTENDED, 
    			GET_REPORT_REQUEST_XML, GetReportRequestHandler.class.getName());	
		Document deleteResponseDoc = DeleteObjectUtils.deleteSingleObject(request, xmpp, log, db, requestObject);
		
		String NID = "";
		if (NID.isEmpty()) {			
			NID = MRXMLUtils.getAttributeValueXpath(deleteResponseDoc, "//" + requestObject.getObjectType(), SessionServiceConst.NID, "");
		}
		requestObject.setNID(NID);
		
		log.addInfo("DeleteReportRequestHandler::Deleting report task from the scheduler", MRLog.OK, MRLog.NOTIFY);		
		AppServerAgentUtils.getScheduleManager(serviceAgentFactory).deleteTaskFromScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, NID);
		
		return responseDoc;
    } 
    
}