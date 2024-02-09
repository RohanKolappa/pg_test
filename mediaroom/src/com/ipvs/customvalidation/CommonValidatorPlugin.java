package com.ipvs.customvalidation;

import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;

public class CommonValidatorPlugin {
	
	  public Boolean validateEmail(  String action, String serviceName,  String dbObjectName,  Element elReq, Element elCustomValidate,MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, 
				MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception    {
		  return Boolean.valueOf(true);
	  }	 
	
}
