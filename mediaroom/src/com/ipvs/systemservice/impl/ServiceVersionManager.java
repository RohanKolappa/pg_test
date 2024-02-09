package com.ipvs.systemservice.impl;

import org.dom4j.Document;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.systemservice.handler.SetServiceDomainRequestHandler;
import com.ipvs.utils.FileUtils;

public class ServiceVersionManager {

    protected static String serviceVersion = null;
   
    public static boolean equals(String requestedServiceVersion,MRLog log) {
	if(serviceVersion == null) {
	    getServiceVersion(log);
	}
	return serviceVersion.equals(requestedServiceVersion);
    }

    public static String getServiceVersion(MRLog log) {
	if(serviceVersion == null) {
	    try {
		String serviceVersionXML = MRXMLUtils.loadXMLResource(SetServiceDomainRequestHandler.SERVICE_VERSION_FILE_PATH, log).asXML() ;
				//FileUtils.readFileResource(SetServiceDomainRequestHandler.SERVICE_VERSION_FILE_PATH,log.getClass());    	
		if(serviceVersionXML == null || serviceVersionXML.isEmpty()) {
		    serviceVersion = "0";
		}
		Document serviceVersionDoc = MRXMLUtils.stringToDocument(serviceVersionXML);
		String schemaVersionStr  = MRXMLUtils.getAttributeValue(serviceVersionDoc.getRootElement(), MRConst.VERSION,"0");    	  
		serviceVersion =  schemaVersionStr;
		MRLog.getInstance(MRLog.CONSOLE).addInfo("Current Service Version = "+serviceVersion,MRLog.INFO,MRLog.NOTIFY);		
	    } catch(Exception exp) {
		MRLog.getInstance(MRLog.CONSOLE).addInfo("Exception Occur during intialization of VersionManager."+exp.getMessage(),MRLog.INFO,MRLog.NOTIFY);
		exp.printStackTrace();
	    }
	}
	return serviceVersion;
    }    
}
