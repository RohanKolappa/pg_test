package com.ipvs.proxy.handler;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
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
import com.ipvs.proxy.impl.ProxyClientAppUtils;
import com.ipvs.proxy.impl.ProxyConstants;
import com.ipvs.proxy.impl.ProxyException;
import com.ipvs.proxy.impl.ProxyExceptionCodes;
import com.ipvs.proxy.impl.ProxySession;


public class StopRecordingRequestHandler implements MRRequestHandlerI {
    

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
    	
    	log.addInfo("StopRecordingRequestHandler - Begin", MRLog.INFO,MRLog.NOTIFY);
    	String fileNID = null;
    	Document requestDoc = request.getMessage().getDocument();
    	String id = MRXMLUtils.getValueXpath(requestDoc.getRootElement(),"//ID", "");
    
    	fileNID = ProxyClientAppUtils.recordingSession.getFileNID();
    	String roomNID = null;
    	roomNID = ProxyClientAppUtils.recordingSession.getRoomNID();
    	boolean isRequestsPending = ProxyClientAppUtils.isRecordingRequestsPending(fileNID,	log);

		if (!isRequestsPending) {
			// If there is no pending request check stream started or
			// not. If started send delete media room request to app
			// server.
			String streamNID = null;
			streamNID = ProxyClientAppUtils.recordingSession.getStreamNID();
			if (ProxyClientAppUtils.isStreamStarted(id, ProxyConstants.STOPRECORDINGREQUEST, streamNID)) {
				throw new ProxyException(ProxyExceptionCodes.STREAM_ID_NOT_FOUND, ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.STREAM_ID_NOT_FOUND));
			}
		}
		if(roomNID != null && !"".equalsIgnoreCase(roomNID)) {
			ProxyClientAppUtils.sendStopRecordingRequest(roomNID, request, log, db, xmpp,resourceFactory, serviceAgentFactory);
		}	
		ProxyClientAppUtils.recordingSession.setStreamNID(null);
		
		if (isRequestsPending) {
			ProxyClientAppUtils.addProxyRequestToRecordingSession(ProxyConstants.STOPRECORDINGREQUEST, fileNID,requestDoc);
			ProxyClientAppUtils.recordingSession.setSessionState(ProxySession.STATE_DONE);
			// If requests are pending abort the recording.
			// TBR : Once implement the time out on waiting for pub sub
			// events
			log.addInfo("Abort Recording: Pending requests are found for givne record ID ="	+ id, MRLog.DEBUG, MRLog.NOTIFY);
			throw new ProxyException(ProxyExceptionCodes.ABORT_RECORDING, ProxyExceptionCodes.getErrorDescription(ProxyExceptionCodes.ABORT_RECORDING));	
			
		}	
		ProxyClientAppUtils.addProxyRequestToRecordingSession(ProxyConstants.STOPRECORDINGREQUEST, fileNID,requestDoc);
		log.addInfo("StopRecordingRequestHandler - End", MRLog.INFO,MRLog.NOTIFY);
		return ProxyClientAppUtils.createResponseDoc(request,"Success");

    } 
    
}



