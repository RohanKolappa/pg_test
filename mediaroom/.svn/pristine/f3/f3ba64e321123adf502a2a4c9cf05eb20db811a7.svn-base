package com.ipvs.sessionservice.handler.base;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;


public abstract class BaseReportRequestHandler extends AsyncBaseRequestHandler implements MRRequestHandlerI {
	
	public static final String GET_REPORT_REQUEST_XML = "/com/ipvs/sessionservice/resource/xml/GetReportRequest.xml";
		
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

	    int authType = MediaRoomObject.ROOM_TYPEID;
	    int role = MediaRoomPEM.VISITOR_ROLE;
	    String token = "testCommonToken";
	    String authority = "testAuthority";
	    String roomPolicyNID = MRConst.NULL;
	    String streamPolicyNID = MRConst.NULL;
	    MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token,
	                authority);
	    log.addTraceEnd(request);

	    return auth;
	}
	
	public void updateStatusElementsInObjectXML(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject, String elementName, String state, 
            String action, String date, String time) throws Exception
	{
		log.addInfo("BaseReportRequestHandler::Update status elements in the report object XML", MRLog.OK, MRLog.NOTIFY);
				
		Document reportObjectXMLDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				requestObject, "", "");		

		MRXMLUtils.setAttributeValueXPath(reportObjectXMLDoc, "//" + elementName, SessionServiceConst.DATE, date);
		MRXMLUtils.setAttributeValueXPath(reportObjectXMLDoc, "//" + elementName, SessionServiceConst.TIME, time);
		MRXMLUtils.setAttributeValueXPath(reportObjectXMLDoc, "//" + elementName, SessionServiceConst.STATE, state);
				
		if (state.equals(SessionServiceConst.SUCCESS)) {
			MRXMLUtils.setAttributeValueXPath(reportObjectXMLDoc, "//" + SessionServiceConst.LAST_EXPORTED_EL_NAME, SessionServiceConst.DATE, date);
			MRXMLUtils.setAttributeValueXPath(reportObjectXMLDoc, "//" + SessionServiceConst.LAST_EXPORTED_EL_NAME, SessionServiceConst.TIME, time);
		}

		request.setLoggingDocument(reportObjectXMLDoc);
		
		
		log.addInfo("BaseReportRequestHandler::Update the report object XML in the db", MRLog.OK, MRLog.NOTIFY);
		UpdateObjectUtils.updateObject(request, xmpp, log, db, reportObjectXMLDoc.getRootElement(), SessionServiceConst.REPORT_OBJECT_TYPE, requestObject.getListOfWhereXpath(), requestObject.getRequestName());
	}
	
	protected void handleReportRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject, Document reportObjectXMLDoc, String action, String type) throws Exception {
		
		Element requestEl = requestObject.getRequestElement();
		
		// Get all the attributes from the request element    	
    	String description = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.DESCRIPTION.toLowerCase(), "");    	
    	String format = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.FORMAT.toLowerCase(), "");    	
    	String enableExport = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.ENABLE_EXPORT_ATTR_NAME, "");
    	String exportSchedule = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.EXPORT_SCHEDULE_ATTR_NAME, "");
    	String exportURL = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.EXPORT_URL_ATTR_NAME, "");
    	   	
    	Element getReportTypeEl = MRXMLUtils.getElementXpath(requestEl, "//" + type);
    		
    	String properties = MRXMLUtils.getAttributeValue(getReportTypeEl, SessionServiceConst.PROPERTIES_ATTR_NAME, "");    	
    	String permissions = MRXMLUtils.getAttributeValue(getReportTypeEl, SessionServiceConst.PERMISSIONS_ATTR_NAME, "");
    	String whereTarget = MRXMLUtils.getAttributeValue(getReportTypeEl, SessionServiceConst.WHERE_TARGET_ATTR_NAME, "");
    	String whereTextSubStr = MRXMLUtils.getAttributeValue(getReportTypeEl, SessionServiceConst.WHERE_TEXT_SUB_STR, "");
    	String whereTagExpr = MRXMLUtils.getAttributeValue(getReportTypeEl, SessionServiceConst.WHERE_TAG_EXPR, "");
    	String whereTagExprMode = MRXMLUtils.getAttributeValue(getReportTypeEl, SessionServiceConst.WHERE_TAG_EXPR_MODE, "");
    	String mode = MRXMLUtils.getAttributeValueXpath(getReportTypeEl, SessionServiceConst.FORMAT_XSL_EL_NAME, SessionServiceConst.MODE, SessionServiceConst.ADVANCED_MODE);
    	String FormatXSL = MRXMLUtils.getValue(getReportTypeEl, SessionServiceConst.FORMAT_XSL_EL_NAME, "");    	
	    	
    	if (reportObjectXMLDoc == null)
    		throw new MRException(SessionServiceExceptionCodes.MISSING_REPORT_OBJECT_XML, "Missing Report ObjectXML");
    	
    	if (action.equals(SessionServiceConst.ADD))    	    	    	
    		MRXMLUtils.setValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.TYPE_EL_NAME, type);
    	    	    	
    	// Set values to the fields of the object XML
    	
	   	MRXMLUtils.setValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.NAME_EL_NAME, requestObject.getAttributeValue());
    	MRXMLUtils.setValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.DESCRIPTION, description);
        MRXMLUtils.setValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.ENABLE_EXPORT_EL_NAME, enableExport);
        MRXMLUtils.setValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.EXPORT_SCHEDULE_EL_NAME, exportSchedule);
        MRXMLUtils.setValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.EXPORT_URL_EL_NAME, exportURL);        
        MRXMLUtils.setValueXpath(reportObjectXMLDoc, "//" + SessionServiceConst.SELECT + SessionServiceConst.FORMAT_XSL_EL_NAME, FormatXSL);
        MRXMLUtils.setAttributeValueXPath(reportObjectXMLDoc, "//" + SessionServiceConst.SELECT + SessionServiceConst.FORMAT_XSL_EL_NAME, SessionServiceConst.MODE, mode);
        
        Element reportTypeEl = MRXMLUtils.getElementXpath(reportObjectXMLDoc, "//" + type + SessionServiceConst.REPORT_CONFIG_EL_NAME);
        
        MRXMLUtils.setAttributeValue(reportTypeEl, SessionServiceConst.SELECTFORMAT, format);
        MRXMLUtils.setAttributeValue(reportTypeEl, SessionServiceConst.SELECT_PROPERTIES_ATTR_NAME, properties);
        
        if (type.equals(SessionServiceConst.POLICY))
        {
        	MRXMLUtils.setAttributeValue(reportTypeEl, SessionServiceConst.SELECT_PERMISSIONS_ATTR_NAME, permissions);
        	MRXMLUtils.setAttributeValue(reportTypeEl, SessionServiceConst.WHERE_TARGET_ATTR_NAME, whereTarget);
        	MRXMLUtils.setAttributeValue(reportTypeEl, SessionServiceConst.WHERE_TEXT_SUB_STR, whereTextSubStr);
        	MRXMLUtils.setAttributeValue(reportTypeEl, SessionServiceConst.WHERE_TAG_EXPR, whereTagExpr);
        	MRXMLUtils.setAttributeValue(reportTypeEl, SessionServiceConst.WHERE_TAG_EXPR_MODE, whereTagExprMode);
        }
        
    }
	 
}
