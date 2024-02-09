package com.ipvs.sessionservice.impl;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.BaseManagementRequestHandler;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;


public class UpdateObjectUtils {
	
	public static Document updateObject(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, Element updateEl, String objectType, List<String> listOfXpaths,
	        	String requestName) throws Exception {
	        Document requestDoc = request.getMessage().getDocument();
	        Element requestEL = MRXMLUtils.getElementXpath(requestDoc, "//" + requestName);
	        
	        String listName = "";
	        String NID = "";
	        String childXpath = "";
	        String[] arrWhereXpath = {  };
	        
	        String whereNIDOrByNID = MRXMLUtils.getAttributeValueWithAlternate(requestEL, SessionServiceResourceConst.BY_NID_ATTRIBUTE, SessionServiceConst.WHERE_NID, "");
	        String whereDateRange = MRXMLUtils.getAttributeValue(requestEL, SessionServiceConst.WHERE_DATE_RANGE, "");
	        String whereTagExpr = MRXMLUtils.getAttributeValue(requestEL, SessionServiceConst.WHERE_TAG_EXPR, "");
	        String whereTextSubStr = MRXMLUtils.getAttributeValue(requestEL, SessionServiceConst.WHERE_TEXT_SUB_STR, "");
	        //childXpath = MRXMLUtils.getAttributeValue(requestEL, SessionServiceConst.WHERE_XPATH, "");
	        

	        GetObjectUtils.generateXPath(whereDateRange, "", whereTagExpr, whereTextSubStr,"", listOfXpaths, objectType);
	       
	        if (whereNIDOrByNID.isEmpty() && ((listOfXpaths != null) && !listOfXpaths.isEmpty())) {
	            arrWhereXpath = listOfXpaths.toArray(arrWhereXpath);
	        } else if (!whereNIDOrByNID.isEmpty()) {
	            NID = whereNIDOrByNID;
	        } else {
	            throw new MRException(SessionServiceExceptionCodes.NO_INFORMATION_FOUND_TO_UPDATE,
	                "No information found to Update the Object.ObjecType:=" + objectType);
	        }
	                
        	PolicyAdminServiceUtils.validateRepalcePEMXPath(request, xmpp, log, db, null, null, request.getServiceName(), null, updateEl, objectType);	        
	        
	        Document response = MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, NID, childXpath, arrWhereXpath, updateEl);
	        Element responseEL = MRXMLUtils.newElement(requestName);

	        if ((response != null) && response.getRootElement() !=null) {
	            responseEL = (Element)response.getRootElement().detach();
	            responseEL.setName(requestName);
	        }

	        return request.createResponseData(responseEL);
	    }
					
	    public static Document updateElement(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String objectType, String NID,
				Document objectXMLDoc, String childXpath) throws Exception {
			
	    	boolean bCustomValidation = new BaseManagementRequestHandler().customValidateReq("Replace", MRConfig.getObjectServiceName(objectType), objectType, null, objectXMLDoc.getRootElement(), request, xmpp, log, db, null, null);
	        if (!bCustomValidation) {
	            throw new MRException(AMExceptionCodes.CUSTOM_VALIDATIONS_FAILED, MRXMLUtils.generateErrorXML("dbobjecttype", objectType));
	        }
	        
			// Validate permissions	  	    	
	    	PolicyAdminServiceUtils.validateRepalcePEMXPath(request, xmpp, log, db, null, null, request.getServiceName(), null, objectXMLDoc.getRootElement(), objectType);
	    	
			Element updateEl = null;			
			if (childXpath.isEmpty()) {
				updateEl = objectXMLDoc.getRootElement();
			} else {
				updateEl = MRXMLUtils.getElementXpath(objectXMLDoc, childXpath);
				if (updateEl == null) {
					throw new MRException(SessionServiceExceptionCodes.INVALID_CHILD_XPATH, "Invalid childXpath while updating the object." +
							"ObjecType:=" + objectType);
				}
			}
			
			Document response = MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, "", objectType, NID, childXpath, null, updateEl);	
			return response;
		}
	    
	    public static boolean validateElementBeforeUpdate(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String objectType, String NID,
				Document objectXMLDoc, String childXpath) throws Exception {
			
	    	boolean bCustomValidation = new BaseManagementRequestHandler().customValidateReq("Replace", MRConfig.getObjectServiceName(objectType), objectType, null, objectXMLDoc.getRootElement(), request, xmpp, log, db, null, null);
	        if (!bCustomValidation) {
	            throw new MRException(AMExceptionCodes.CUSTOM_VALIDATIONS_FAILED, MRXMLUtils.generateErrorXML("dbobjecttype", objectType));
	        }
	        
			// Validate permissions	  	    	
	    	PolicyAdminServiceUtils.validateRepalcePEMXPath(request, xmpp, log, db, null, null, request.getServiceName(), null, objectXMLDoc.getRootElement(), objectType);
	    	
	    	return true;
	    	
		}
	    
	    public static Document updateElementNoValidationCheck(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String objectType, String NID,
				Document objectXMLDoc, String childXpath) throws Exception {		
	    	
	    	
			Element updateEl = null;			
			if (childXpath.isEmpty()) {
				updateEl = objectXMLDoc.getRootElement();
			} else {
				updateEl = MRXMLUtils.getElementXpath(objectXMLDoc, childXpath);
				if (updateEl == null) {
					throw new MRException(SessionServiceExceptionCodes.INVALID_CHILD_XPATH, "Invalid childXpath while updating the object." +
							"ObjecType:=" + objectType);
				}
			}
			
			Document response = MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, "", objectType, NID, childXpath, null, updateEl);	
			return response;
		}
		
}
