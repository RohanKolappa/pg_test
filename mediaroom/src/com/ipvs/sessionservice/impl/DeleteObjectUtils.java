package com.ipvs.sessionservice.impl;


import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;


public class DeleteObjectUtils {
	
	public static void checkForMultipleRecordsException (String strToCheckForTokens, String objectType) throws Exception {
		StringTokenizer token = new StringTokenizer(strToCheckForTokens, ",");
		if(token.countTokens() > 1) {
			throw new MRException(SessionServiceExceptionCodes.MATCHED_MULTIPLE_NODES, "Can not Delete " + objectType + " matched multiple nodes");
		}		
	}
	
	public static Document deleteSingleObject(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, RequestProcessor requestObject) 
	throws Exception {
		
		String NID = requestObject.getNID();
		String attrValue = requestObject.getAttributeValue();
		String objectType = requestObject.getObjectType();
		
    	checkForMultipleRecordsException(NID, objectType);    	
    	
    	Document responseDoc = null;
    	if (!NID.isEmpty()) {
    		responseDoc = deleteObjectByNID(request, xmpp, log, db, objectType, NID);
    	} else {    		    		
    		checkForMultipleRecordsException(attrValue, objectType);    		    		
    		List<String> listOfWhereXpath = requestObject.generateXPathBasedOnAttributeType(new ArrayList<String>(), objectType, 
					requestObject.getAttributeType(), requestObject.getAttributeValue(), requestObject.getAttributeConstant());	    	    		
			responseDoc = deleteObject(request, xmpp, log, db, objectType, listOfWhereXpath, requestObject.getRequestName());
		}
    
		return responseDoc;
	}
			
	public static Document deleteObject(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String objectType, List<String> listOfXpaths,
    		String requestName) throws Exception {
        Document requestDoc = request.getMessage().getDocument();
        Element requestEL = MRXMLUtils.getElementXpath(requestDoc, "//" + requestName);

        String listName = "";        
        String childXpath = "";
        String[] arrWhereXpath = {  };

        String byNID = MRXMLUtils.getAttributeValueWithAlternate(requestEL, SessionServiceResourceConst.BY_NID_ATTRIBUTE, SessionServiceConst.WHERE_NID, "");
        String whereDateRange = MRXMLUtils.getAttributeValue(requestEL, SessionServiceConst.WHERE_DATE_RANGE, "");
        String whereTagExpr = MRXMLUtils.getAttributeValue(requestEL, SessionServiceConst.WHERE_TAG_EXPR, "");
        String whereTextSubStr = MRXMLUtils.getAttributeValue(requestEL, SessionServiceConst.WHERE_TEXT_SUB_STR, "");

        GetObjectUtils.generateXPath(whereDateRange, byNID, whereTagExpr, whereTextSubStr, "",listOfXpaths, objectType);
        String pemXpath = PolicyAdminServiceUtils.validateDeletePEMXPath(request, xmpp, log, db, null, null, request.getServiceName(), objectType);
        if(!pemXpath.isEmpty()) {
        	listOfXpaths.add(pemXpath);
        }
        if ((listOfXpaths != null) && !listOfXpaths.isEmpty()) {
            arrWhereXpath = listOfXpaths.toArray(arrWhereXpath);        
        } else {
            throw new MRException(SessionServiceExceptionCodes.NO_INFORMATION_FOUND_TO_DELETE,
                "No information found to Delete the Object.ObjecType:=" + objectType);
        }
        
        Document response = MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, "", childXpath, arrWhereXpath);
        Element responseEL = MRXMLUtils.newElement(requestName);

        if ((response != null) && response.getRootElement() !=null) {
            responseEL = (Element)response.getRootElement().detach();
            responseEL.setName(requestName);
        }

        return request.createResponseData(responseEL);
    }
	
	
	public static Document deleteObjectByNID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String dbObjectType,String NID) throws Exception {
		String pemXpath = PolicyAdminServiceUtils.validateDeletePEMXPath(request, xmpp, log, db, null, null, request.getServiceName(), dbObjectType);
		String[] whereXpath ={pemXpath};
        Document response = MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", dbObjectType, NID, "", whereXpath);
        return response;
    }
}
