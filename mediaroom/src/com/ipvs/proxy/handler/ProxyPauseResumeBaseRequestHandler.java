package com.ipvs.proxy.handler;

import org.dom4j.Document;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.proxy.impl.ProxyClientAppUtils;
import com.ipvs.proxy.impl.ProxyException;
import com.ipvs.proxy.impl.ProxyExceptionCodes;

public class ProxyPauseResumeBaseRequestHandler {

	
	public void sendRecordingPauseResumeRequest (String action, String errorStatus, int errorCode, String requestName, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		String streamNID = null;
		String fileNID = null;
    	Document requestDoc = request.getMessage().getDocument();
    	//String id = MRXMLUtils.getValueXpath(requestDoc.getRootElement(),"//ID", "");
    	fileNID = ProxyClientAppUtils.recordingSession.getFileNID();
    	String fileStatus = null;
    	if (fileNID != null) {
			fileStatus = ProxyClientAppUtils.getFileStatusFromRecordingSession(fileNID);
    	}	
    	streamNID = ProxyClientAppUtils.recordingSession.getStreamNID();
		if (fileStatus!=null && fileStatus.equals(errorStatus)) {
			throw new ProxyException (errorCode, ProxyExceptionCodes.getErrorDescription(errorCode));
		}
		ProxyClientAppUtils.sendRecordingPauseResumeRequest(streamNID,action, request, log, db,xmpp, resourceFactory, serviceAgentFactory);
		ProxyClientAppUtils.addProxyRequestToRecordingSession(requestName, fileNID, requestDoc);

	}
	
}
