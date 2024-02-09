package com.ipvs.sessionservice.resource.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class GetSubscriptionsRequestHandler implements MRRequestHandlerI {
	
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
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
			Document doc = request.getMessage().getDocument();				
			Element getSubscriptionsEl = MRXMLUtils.getElementXpath(doc, "//GetSubscriptions");
			
			String byJID = MRXMLUtils.getAttributeValue(getSubscriptionsEl, SessionServiceResourceConst.BY_JID_ATTRIBUTE, "");
			String selectFormat = MRXMLUtils.getAttributeValue(getSubscriptionsEl, SessionServiceConst.SELECTFORMAT, "");
				
			if(selectFormat.isEmpty()) 
				selectFormat = SessionServiceConst.SELECTFORMAT_BASIC;
			
			Document resultDoc = null;
			resultDoc = MRXMLDBUtils.getSubscriptions(request, log, db, byJID, selectFormat);

//			if(!selectFormat.isEmpty()) {
//				if(selectFormat.equals(SessionServiceConst.NIDLIST)) {
//					resultDoc = MRXMLDBUtils.getSubscriptionList(request, log, db, whereJID);
//				}else if(selectFormat.equals(SessionServiceConst.JIDLIST)) {
//					resultDoc = MRXMLDBUtils.getSubscriptionList(request, log, db, whereNID);
//				}
//			}else {
//				resultDoc = MRXMLDBUtils.getSubscriptionList(request, log, db, whereNID);
//			}
			
			return request.createResponseData(resultDoc.getRootElement());
									
	}

}
