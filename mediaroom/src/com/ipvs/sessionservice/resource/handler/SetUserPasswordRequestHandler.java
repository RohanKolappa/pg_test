package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.handler.base.UserBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;


public class SetUserPasswordRequestHandler extends UserBaseRequestHandler {
	
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws Exception {
        
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, profileFactory, SessionServiceConst.USER);
		
		Element requestEl = requestObject.getRequestElement();
		
        String oldPassword = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.OLD_PASSWORD_ATTRIBUTE, "");        
        if (oldPassword.isEmpty()) {
            throw new MRException(SessionServiceExceptionCodes.OLD_PASSWORD_MISSING, "Old Password Value Missing");
        }

        String newPassword = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.NEW_PASSWORD_ATTRIBUTE, "");        
        if (newPassword.isEmpty()) {
            throw new MRException(SessionServiceExceptionCodes.NEW_PASSWORD_MISSING, "New Password Value Missing");
        }
        
       	Document objectXMLDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, profileFactory, 
       			requestObject, "", "");
       	
        String username = requestObject.getAttributeValue();	                       
        
        updateUserPasswordInDB(request, username, oldPassword, newPassword, objectXMLDoc, xmpp, log, db);
        
        Document responseDoc = GetObjectUtils.getResponse(request, xmpp, log, db, resourceFactory, profileFactory, requestObject, 
        		SessionServiceConst.SELECTFORMAT_EXTENDED, GET_USER_REQUEST_XML, GetUserRequestHandler.class.getName());
        
        return responseDoc;
        		                      
    }
    
    public void updateUserPasswordInDB(MRRequest request, String username, String oldPassword, String newPassword, Document objectXMLDoc, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
    	String parentElXPath = "//" + SessionServiceConst.PASSWORD_EL_NAME;
       	
        String userPassword = MRXMLUtils.getValueXpath(objectXMLDoc, parentElXPath, "");
        if (!userPassword.equals(oldPassword)) {
            throw new MRException(SessionServiceExceptionCodes.USER_PASSWORD_NOT_MATCHING, "User Password is not matching");
        }
        
        MRXMLUtils.setValueXpath(objectXMLDoc, parentElXPath, newPassword);
        
        if (request.getUserJID().contains(username)) {
        	request.setSuperUser(true);        	        	      
        }
        
        String NID = MRXMLUtils.getAttributeValue(objectXMLDoc.getRootElement(), SessionServiceConst.NID, "");
        UpdateObjectUtils.updateElement(request, xmpp, log, db, SessionServiceConst.USER, NID, objectXMLDoc, parentElXPath);
    }
}
