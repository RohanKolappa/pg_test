package com.ipvs.mediastoreservice.impl;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;

public class MediaStoreAsynchCallBackData {
	private MRRequest request = null;	
	private MRLog log= null;	
	private MRDB db= null;
	private XMPPI xmpp = null; 
	private MRResourceFactoryI resourceFactory;
	private ServiceAgentFactoryI profileFactory;    
    
	
	public MediaStoreAsynchCallBackData(MRRequest request,MRLog log,MRDB db,XMPPI xmpp,MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) {
    	this.request = request;
    	this.log = log;
    	this.db =db;
    	this.xmpp = xmpp;
    	this.resourceFactory = resourceFactory;
    	this.profileFactory = profileFactory;
    }
    public MRRequest getRequest() {
		return request;
	}
    
    public MRLog getLog() {
		return log;
	}    

    public MRDB getDb() {
		return db;
	}
    
    public XMPPI getXmpp() {
		return xmpp;
	}
    
    public MRResourceFactoryI getResourceFactory() {
		return resourceFactory;
	}
    
    public ServiceAgentFactoryI getProfileFactory() {
		return profileFactory;
	}
    
    public void handleAsynchRequestResponse(Element responseElement,int status,String logData) throws MRException,Exception {    	
    	MRRequest request = getRequest();
		Document requestDoc = request.getMessage().getDocument();
        String mediaStoreFileNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID, "");
        String mediaStreamFileNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");    
        if(status != MRRequest.STATE_DONE) {
			getLog().addInfo(""+ request.getRequestName() +" failed. mediaStoreFileNID="+mediaStoreFileNID + "  mediaStreamFileNID:="+mediaStreamFileNID);
            MediaStoreServiceUtils.updateMediaStreamFileState(mediaStreamFileNID, MediaStoreServiceConst.ERROR, mediaStoreFileNID, mediaStreamFileNID, getXmpp(), getLog(), getDb());
 		}        
        Document responseDoc = MediaStoreServiceUtils.generateResponseDocument(MRRequest.getResponseName(request.getRequestName()), mediaStoreFileNID, mediaStreamFileNID, "", MediaStoreServiceConst.SUCCESS_STATE, "", request.getNID(), getLog());
        if(responseDoc != null) {
        	MediaStoreServiceUtils.sendMediaStoreResponseMessage(requestDoc, responseDoc, request.getRequestName(), AppServerAgentUtils.getAppServerAgentJID(request.getUserJID(),getProfileFactory()), status, logData, getXmpp(), getLog(), request.getUserJID());
        }
    }

}
