package com.ipvs.proxy.handler;

import java.net.URLEncoder;

import org.dom4j.Document;
import org.dom4j.Element;

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
import com.ipvs.proxy.impl.ProxyClientAppUtils;


public class RecordingStatusRequestHandler implements MRRequestHandlerI {
    

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
    	log.addInfo("RecordingStatusRequestHandler - Begin", MRLog.INFO,MRLog.NOTIFY);
    	Document requestDoc = request.getMessage().getDocument();
    	String id = MRXMLUtils.getValueXpath(requestDoc.getRootElement(),"//ID", "");
		sendRecordingStatusRequest(id, requestDoc, request, log, db, xmpp,resourceFactory, serviceAgentFactory);
		//ProxyClientAppUtils.addProxyRequestMessageToCache(ProxyConstants.RECORDINGSTATUSREQUEST, id, requestDoc);
		//ProxyClientAppUtils.recordingSession.setRecordID(id);
		log.addInfo("RecordingStatusRequestHandler - End", MRLog.INFO,MRLog.NOTIFY);
		return ProxyClientAppUtils.createResponseDoc(request,"Success");
    } 
    
    
    public static void sendRecordingStatusRequest(String recordID, Document proxyRequestDoc, MRRequest request, MRLog log, MRDB db, XMPPI xmpp,MRResourceFactoryI resourceFactory,	ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
		String requestName = "GetSrcMediaFileListRequest";
		String requestString = MRXMLUtils.documentToString(proxyRequestDoc);
		String clientdataString = URLEncoder.encode(requestString, "UTF-8");
		//String clientdataString = requestString;
		
		
		Document requestDoc = MediaRoomServiceUtils.getMediaFileListRequestDoc(recordID, log);
		Element clientData = MRXMLUtils.stringToElement("<clientdata></clientdata>");
		clientData.setText(clientdataString);
		Element headerElement = MRXMLUtils.getElementXpath(requestDoc, "//header");
		MRXMLUtils.removeChild(headerElement,"clientdata");
		headerElement.add(clientData);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);

	}

}
