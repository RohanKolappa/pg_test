package com.ipvs.sessionservice.service.handler;

import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.handler.base.GetTagsBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public class PerfTestRequestHandler extends GetTagsBaseRequestHandler{


	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

		int authType = MediaRoomObject.ROOM_TYPEID;
		int role = MediaRoomPEM.VISITOR_ROLE;
		String token = "testCommonToken";
		String authority = "testAuthority";
		String roomPolicyNID = MRConst.NULL;
		String streamPolicyNID = MRConst.NULL;
		MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
		log.addTraceEnd(request);
		return auth;
	}

	private static final String PERF_TEST = "PerfTest"; 
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document requestDoc = request.getMessage().getDocument();
		Element perfEl  = MRXMLUtils.getElementXpath(requestDoc, "//"+PERF_TEST);
		String contactJIDList = MRXMLUtils.getAttributeValue(perfEl, SessionServiceConst.WHERE_CONTACT_JID,"");
		long now = System.currentTimeMillis();
		StringTokenizer st = new StringTokenizer(contactJIDList, ",");
		long parseDelay = System.currentTimeMillis() - now;
		String ts = MRXMLUtils.getAttributeValue(perfEl, "ts","");
    	String dataStr = "<x xmlns='" + IPVSMessageExtension.IPVS_SESSION_XMLNS + "'>" +
    	"<PerfTrace ts='" + ts + "' now='" + now + "' delay='" + parseDelay + "'/>" +	"</x>";

    	now = System.currentTimeMillis();
		log.addInfo("PerfTest:getResponse perfTrace=" + dataStr,MRLog.OK, MRLog.NOTIFY);
		while(st.hasMoreTokens()) {
	        String toAgentJID = st.nextToken();
	        // AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fromAgentName, toAgentJID, toAgentName, dataStr, responseHandlerName);
	        xmpp.sendMessage(toAgentJID, dataStr, IPVSMessageExtension.IPVS_SESSION_XMLNS);
		}
		long sendDelay = System.currentTimeMillis() - now;
        log.addInfo("PerfTestRequestHandler parseDelay='" + parseDelay + "' sendDelay='" + sendDelay +
        		"' startSend='" + now + "' endSend='" + System.currentTimeMillis() + "'",MRLog.INFO,MRLog.NOTIFY);
        return null;
	}

}
