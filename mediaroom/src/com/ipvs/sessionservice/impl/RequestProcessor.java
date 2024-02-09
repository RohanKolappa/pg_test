package com.ipvs.sessionservice.impl;

import java.util.Hashtable;
import java.util.List;

import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;


public class RequestProcessor {
	
	private String NID = "";
	private String attributeType = "";
	private String attributeValue = "";
	private String attributeConstant = "";
	private String objectType = "";
	private String requestName = "";
	private Element requestEl = null;
	
	private List<String> listOfWhereXpath = null;
	
	private static Hashtable<String, String> attributeLookupObj = new Hashtable<String, String>();
		
	// Get	
	public String getRequestName() {
		return requestName;
	}
	
	public Element getRequestElement() {
		return requestEl;
	}
	
	public void setRequestElement(Element requestElement) {
		requestEl= requestElement;
	}
	
	public String getNID() {
		return NID;
	}
	
	public String getAttributeType() {
		return attributeType;
	}
	
	public String getAttributeValue() {
		return attributeValue;
	}
	
	public String getAttributeConstant() {
		return attributeConstant;
	}
	
	public String getObjectType() {
		return objectType;
	}
	
	public List<String> getListOfWhereXpath() {
		return listOfWhereXpath;
	}
	
	// Set	
	public void setNID(String NID) {
		this.NID = NID;
	}
	
	// Set attributeValue in case of Add API calls
	public void setAttributeValue(String attributeValue) {
		this.attributeValue = attributeValue;
	}
	
	public void setObjectType(String objectType) {
		this.objectType = objectType;
	}
	
	public static void initAttributeLookupTable() throws Exception {
		attributeLookupObj.put(SessionServiceConst.NAME_EL_NAME, SessionServiceConst.NAME_CONST);
		attributeLookupObj.put(SessionServiceConst.USERNAME, SessionServiceConst.USER_NAME_CONST);
		attributeLookupObj.put(SessionServiceResourceConst.TITLE, SessionServiceConst.TITLE_CONST);
	}
	
	public RequestProcessor(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, String objectType) throws Exception {
		
		this.objectType = objectType;
		requestName = MRRequest.getRequestRootName(request.getRequestName());		
		requestEl = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//" + requestName);

		NID = MRXMLUtils.getAttributeValueWithAlternate(requestEl, SessionServiceResourceConst.BY_NID_ATTRIBUTE, SessionServiceConst.WHERE_NID, "");
				
		if (objectType.equals(SessionServiceConst.REPORT_OBJECT_TYPE)) {
			attributeType = SessionServiceConst.NAME_EL_NAME;			
		} else if (objectType.equals(SessionServiceConst.USER)) {
			attributeType = SessionServiceConst.USERNAME;			
		} else {						
			attributeType = SessionServiceResourceConst.TITLE;						
		}
		
		// SetPort API calls have byTitle or whereTextSubStr as request attributes
		if (attributeType.equals(SessionServiceResourceConst.TITLE)) {
			attributeValue = MRXMLUtils.getAttributeValueWithAlternate(requestEl, SessionServiceResourceConst.BY_TITLE_ATTRIBUTE, SessionServiceConst.WHERE_TEXT_SUB_STR, "");
		} else {
			attributeValue = MRXMLUtils.getAttributeValueWithAlternate(requestEl, "by" + attributeType, "where" + attributeType, "");
		}
		
		// In case of Add API calls
		if (attributeValue.isEmpty()) {
			attributeValue = MRXMLUtils.getAttributeValue(requestEl, attributeType.toLowerCase(), "");
		}
		
		// Constant for xpath generation		
		attributeConstant = attributeLookupObj.get(attributeType);				
				
		// If byNID and byUserName are empty in case of 'User' the login username is picked as default. 
		if (!objectType.equals(SessionServiceConst.USER) && NID.isEmpty() && attributeValue.isEmpty()) {
			throw new MRException(SessionServiceExceptionCodes.NID_AND_TITLE_MISSING, "NID and title missing");
		}
		
	}
	
	public List<String> generateXPathBasedOnAttributeType(List<String> listOfWhereXpath, String objectType, String attributeType, 
			String attributeValue, String attributeConstant) throws Exception {
		
		if (listOfWhereXpath == null) {
			return null;
		} else {
			this.listOfWhereXpath = listOfWhereXpath;
		}
		
		String whereXPath = GetObjectUtils.generateXPath(attributeValue, attributeType, objectType, attributeConstant);
		if(!whereXPath.isEmpty()) {
			listOfWhereXpath.add(whereXPath);
		}
		
		return listOfWhereXpath;
	}
	
}
