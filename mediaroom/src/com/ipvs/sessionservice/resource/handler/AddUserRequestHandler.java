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
import com.ipvs.sessionservice.handler.base.UserBaseRequestHandler;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class AddUserRequestHandler extends UserBaseRequestHandler {
		
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws Exception {
        
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, profileFactory, SessionServiceConst.USER);
		
		Element requestEl = requestObject.getRequestElement();
		
        String fullName = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.FULL_NAME, "");        
        String loginName = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.LOGIN_NAME, "");
        String password = MRXMLUtils.getAttributeValue(requestEl, SessionServiceResourceConst.PASSWORD_ATTRIBUTE, "");        
        String externalAuthentication = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.EXTERNAL_AUTHENTICATION_ATTR_NAME, "");   
        String creationDate = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.CREATION_DATE_ATTR_NAME, "");        
        String modifiedDate = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.MODIFIED_DATE_ATTR_NAME, "");   
        Element groupsEL = MRXMLUtils.getElementXpath(requestEl, "./"+SessionServiceResourceConst.GROUPS_ELEMENT);
        addUserInDB(request, fullName, loginName, password, externalAuthentication, creationDate, modifiedDate, groupsEL, xmpp, log, db, SessionServiceConst.USER);
       	
        Document responseDoc = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, profileFactory, requestObject, 
        		SessionServiceConst.SELECTFORMAT_EXTENDED, GET_USER_REQUEST_XML, GetUserRequestHandler.class.getName());
        
        return responseDoc;
        		                      
    }
    
    public void addUserInDB(MRRequest request, String fullName, String loginName, String password, String externalAuthentication, String creationDate, 
    		String modifiedDate, Element groupsEL, XMPPI xmpp, MRLog log, MRDB db, String objectType) throws Exception {
		
		Document userObjectXMLDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, objectType, log);		
		
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.USERNAME, loginName);
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.PASSWORD_EL_NAME, password);
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.FULLNAME, fullName);
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.CREATION_DATE, creationDate);
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.MODIFIED_DATE, modifiedDate);
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.EXTERNAL_AUTHENTICATION, externalAuthentication);
		MRXMLUtils.setValueXpath(userObjectXMLDoc, "//" + SessionServiceConst.EXTERNAL_AUTHENTICATION, externalAuthentication);
		if(groupsEL != null)
			MRXMLUtils.replaceContent(MRXMLUtils.getElementXpath(userObjectXMLDoc, "//"+SessionServiceResourceConst.GROUPS_ELEMENT), groupsEL);
		
		AddObjectUtils.addObjectInDatabaseWithSuperuserPrivileges(request, xmpp, log, db, null, null, userObjectXMLDoc, objectType);
		
	}
}


