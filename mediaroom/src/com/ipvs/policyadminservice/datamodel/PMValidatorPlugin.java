package com.ipvs.policyadminservice.datamodel;

import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.ValidationUtils;

public class PMValidatorPlugin {
	
	 public Boolean validateTextForSpecialCharacters(String action, String serviceName, String dbObjectName, Element elReq, 
			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
			 ServiceAgentFactoryI serviceAgentFactory) throws Exception {		 
		 String value = MRXMLUtils.getValue2(elReq, "").trim();         	
		 ValidationUtils.containsSpecialCharacters(value);		 	 	 
		 return true;			 
     }   
	 
	 public Boolean validateDuplicateName(String action, String serviceName, String dbObjectName, Element elReq, 
			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
			 ServiceAgentFactoryI serviceAgentFactory) throws Exception {		 
		 String value = MRXMLUtils.getValue2(elReq, "").trim();
		 ValidationUtils.isDuplicateName(request, xmpp, log, schema, dbObjectName, value);		 	 	 
		 return true;			 
     }
	
	 public Boolean validateScheduleFormatAndRange(String action, String serviceName, String dbObjectName, Element elReq, 
			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
			 ServiceAgentFactoryI serviceAgentFactory) throws Exception {		 
		 String value = MRXMLUtils.getValue2(elReq, "").trim();
		 ValidationUtils.isCorrectFormatAndRange(value);		 
         return true;			 
     }
	 
	 public Boolean validateTitleForWhiteSpaces(String action, String serviceName, String dbObjectName, Element elReq, 
			 Element elCustomValidate, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, 
			 ServiceAgentFactoryI serviceAgentFactory) throws Exception {
   	  	String value = MRXMLUtils.getValue2(elReq, "").trim();
   	  	if (value.equals("")) {
   	  		log.addInfo(" " + dbObjectName + ": Please provide value for '" + elReq.getName() + "' ", MRLog.ERROR, MRLog.ERROR);
   	  		throw new MRException(AMExceptionCodes.VALID_ERROR_EMPTY_NAME, MRXMLUtils.generateErrorXML("value", value));
   	  	}
 		ValidationUtils.containsWhiteSpaces(value, dbObjectName, log);
        return true;
     }  
	 
}
