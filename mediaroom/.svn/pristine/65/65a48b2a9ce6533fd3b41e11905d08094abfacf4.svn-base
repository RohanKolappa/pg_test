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
import com.ipvs.common.service.XAuthServerAgent;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.proxy.impl.ProxyClientAppUtils;


public class AuthorizationUpdateRequestHandler implements MRRequestHandlerI {
    
	XAuthServerAgent xAuthServerAgent = null;

	
	public AuthorizationUpdateRequestHandler(XAuthServerAgent xAuthServerAgent) {
		this.xAuthServerAgent = xAuthServerAgent;
	}
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
    	log.addInfo(System.currentTimeMillis() + ":AuthorizationUpdateRequestHandler - Begin", MRLog.INFO,MRLog.NOTIFY);
    	Document requestDoc = request.getMessage().getDocument();
		String destAgentJID = requestDoc.getRootElement().element("data").element("StreamData").element("destInfo").elementText("JID");
		if ((destAgentJID == null) || destAgentJID.trim().equals("")) {
			return ProxyClientAppUtils.createResponseDoc(request,"Invalid destination NID");
		}
		xAuthServerAgent.handleRevokeXAuth(destAgentJID, log, xmpp);
		log.addInfo(System.currentTimeMillis() + ":AuthorizationUpdateRequestHandler - End", MRLog.INFO,MRLog.NOTIFY);
		return ProxyClientAppUtils.createResponseDoc(request,"Success");

    } 



}
