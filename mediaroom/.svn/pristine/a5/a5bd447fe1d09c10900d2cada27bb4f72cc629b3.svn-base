package com.ipvs.sessionservice.impl;

import org.dom4j.Document;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.BaseManagementRequestHandler;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;


public class AddObjectUtils {   
		
	public static final void addObjectInDatabaseWithSuperuserPrivileges(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document objectDoc, String objectType)
        throws MRException, Exception {        
        
    	request.setSuperUser(true);
    	addObjectInDatabase(request, xmpp, log, db, resourceFactory, serviceAgentFactory, objectDoc, objectType);
    	request.setSuperUser(false);
                
    }
	
	public static final Document addObjectInDatabase(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document objectDoc, String objectType)
        throws MRException, Exception {        
        
    	String listName = null;
        String parentNID = null;
        String childXpath = "";
        String[] arrWhereXpath = null;
        
        boolean bCustomValidation = new BaseManagementRequestHandler().customValidateReq("Add", MRConfig.getObjectServiceName(objectType), objectType, null, objectDoc.getRootElement(), request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        if (!bCustomValidation) {
            throw new MRException(AMExceptionCodes.CUSTOM_VALIDATIONS_FAILED, MRXMLUtils.generateErrorXML("dbobjecttype", objectType));
        }
        
        PolicyAdminServiceUtils.validateAddPEMXPath(request, xmpp, log, db, resourceFactory, serviceAgentFactory, request.getServiceName(), objectDoc.getRootElement(), objectType);
        
        Document response = MRXMLDBUtils.add(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, parentNID, childXpath, arrWhereXpath,
             objectDoc.getRootElement());
        
        return response;

    }
	public static final Document addObjectInDatabaseNoValidationCheck(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	            ServiceAgentFactoryI serviceAgentFactory, Document objectDoc, String objectType)
	        throws MRException, Exception {        
	        
	    	String listName = null;
	        String parentNID = null;
	        String childXpath = "";
	        String[] arrWhereXpath = null; 
	        
	        Document response = MRXMLDBUtils.add(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, parentNID, childXpath, arrWhereXpath,
	             objectDoc.getRootElement());
	        
	        return response;

	    }
	
    public static String addObject(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory,Document resourceDoc,String objectType) throws MRException,Exception {
    	Document responseDoc = AddObjectUtils.addObjectInDatabase(request, xmpp, log, db, resourceFactory, serviceAgentFactory, resourceDoc, objectType);
    	return MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(responseDoc, "//"+objectType), SessionServiceConst.NID);
    }
}
