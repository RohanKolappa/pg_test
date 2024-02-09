package com.ipvs.proxy.handler;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.proxy.impl.ProxyClientAppUtils;
import com.ipvs.proxy.impl.ProxyConstants;
import com.ipvs.proxy.impl.ProxyException;
import com.ipvs.proxy.impl.ProxyExceptionCodes;
import com.ipvs.proxy.impl.ProxySession;


public class StartRecordingRequestHandler implements MRRequestHandlerI {
    

    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.STREAM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        // If destNID is empty then use the request.getResourceGroup
        // as the destGroup to find the matching policy
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws Exception {
    	
    	log.addInfo("StartRecordingRequestHandler - Begin", MRLog.OK, MRLog.NOTIFY);
    	String userJID  = request.getUserJID();
    	String fileNID = null;
    	Document requestDoc = request.getMessage().getDocument();
    	String id = MRXMLUtils.getValueXpath(requestDoc.getRootElement(),"//ID", "");
    	ProxyClientAppUtils.recordingSession.setRecordID(id);
    	fileNID = ProxyClientAppUtils.recordingSession.getFileNID();
    	String fileStatus = null;
    	if (fileNID != null) {
			fileStatus = ProxyClientAppUtils.getFileStatus(fileNID, db, log, userJID);
    	}	
    	if(fileStatus!=null && (fileStatus.equals(MediaStoreServiceConst.ERROR))) {
    		throw new ProxyException (ProxyExceptionCodes.MEDIASTORE_ERROR, ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.MEDIASTORE_ERROR));
    	}
    	else if (fileStatus!=null && fileStatus.equals(MediaStoreServiceConst.BUSY)) {
			throw new ProxyException (ProxyExceptionCodes.RECORD_STARTED, ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.RECORD_STARTED));
		}
		//int resourceindex = userJID.indexOf("/");
		//String lab = userJID.substring(resourceindex + 1);
		sendCreateRecordStreamRequest(id, ProxyClientAppUtils.getSourceTitle(), request,log, db, xmpp, resourceFactory, serviceAgentFactory);
		
		ProxyClientAppUtils.addProxyRequestToRecordingSession(ProxyConstants.STARTRECORDINGREQUEST, id, requestDoc);
		log.addInfo("StartRecordingRequestHandler - End");
		return ProxyClientAppUtils.createResponseDoc(request,"Success");
		
    } 

    private static void sendCreateRecordStreamRequest(String recordID,
			String title, MRRequest request, MRLog log,
			MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String startRecordingXML = "CreateRecordingRequest.xml";
		String requestName = "CreateRecordingRequest";
		Document requestDoc = MRXMLUtils.loadXMLResource(
				ProxyConstants.MEDIAROOM_XMLFILEPATH + startRecordingXML, log);

		MRXMLUtils.setValueXpath(requestDoc, "//recordID", recordID);
		MRXMLUtils.setValueXpath(requestDoc, "//title", title);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request
				.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);
		ProxyClientAppUtils.recordingSession.setSessionState(ProxySession.WAIT_FOR_CREATE_RECORDING_RESPONSE);
	}
    
    


}
