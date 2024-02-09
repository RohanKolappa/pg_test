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
import com.ipvs.proxy.impl.ProxyConstants;


public class MediaTimestampRequestHandler implements MRRequestHandlerI {
    

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
    	log.addInfo("MediaTimestampRequestHandler - Begin", MRLog.INFO,MRLog.NOTIFY);
    	Document requestDoc = request.getMessage().getDocument();
		String streamNID = ProxyClientAppUtils.recordingSession.getStreamNID();
		sendGetNewBookMarkRequest(requestDoc,  streamNID, request, log, db, xmpp, resourceFactory,serviceAgentFactory);
		log.addInfo("MediaTimestampRequestHandler - End", MRLog.INFO,MRLog.NOTIFY);
		return ProxyClientAppUtils.createResponseDoc(request,"Success");
    } 

    
    
    
    private static void sendGetNewBookMarkRequest(Document proxyRequestDoc, String streamNID,
			MRRequest request, MRLog log, MRDB db, XMPPI xmpp,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,
			Exception {
		String getNewBookMarkXML = "GetNewBookmarkRequest.xml";
		String requestName = "GetNewBookmarkRequest";
		String requestString = MRXMLUtils.documentToString(proxyRequestDoc);
		String clientdataString = URLEncoder.encode(requestString, "UTF-8");
		Document requestDoc = MRXMLUtils.loadXMLResource(ProxyConstants.MEDIAROOM_XMLFILEPATH + getNewBookMarkXML, log);
		MRXMLUtils.setValueXpath(requestDoc, "//streamNID", streamNID);
		Element clientData = MRXMLUtils.stringToElement("<ClientData></ClientData>");
		clientData.setText(clientdataString);
		Element headerElement = MRXMLUtils.getElementXpath(requestDoc, MRConst.HEADER_XPATH);
		MRXMLUtils.removeChild(headerElement, "clientdata");
		headerElement.add(clientData);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request
				.getUserJID(), requestDoc, log, requestName, serviceAgentFactory);

	}



}
