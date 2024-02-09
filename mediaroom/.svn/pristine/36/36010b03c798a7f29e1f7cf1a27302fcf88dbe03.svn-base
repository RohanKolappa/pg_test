package com.ipvs.sessionservice.session.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.handler.base.BookmarkBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class NewBookmarkRequestHandler extends BookmarkBaseRequestHandler {
	
	public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	        ServiceAgentFactoryI profileFactory) throws Exception {
		log.addTraceBegin(request);

	    int authType = MediaRoomObject.ROOM_TYPEID;
	    int role = MediaRoomPEM.VISITOR_ROLE;
	    String token = "testCommonToken";
	    String authority = "testAuthority";
	    String roomPolicyNID = MRConst.NULL;
	    String streamPolicyNID = MRConst.NULL;
	    MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token,
	                authority);
	    log.addTraceEnd(request);

	    return auth;
	}
	
	protected Element getNewBookmark(MRLog log) throws MRException, Exception {
		Element bookmarkEL = MRXMLUtils.newElement(AMConst.BOOKMARK);
		Document bookmarkObjectDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.BOOKMARK, log);
		MRXMLUtils.addAttribute(bookmarkEL, SessionServiceConst.TITLE,"");
		MRXMLUtils.addAttribute(bookmarkEL, SessionServiceConst.TC, "");
		MRXMLUtils.addAttribute(bookmarkEL, SessionServiceConst.BOOKMARK_OFFSET.toLowerCase(),"");
		
		MRXMLUtils.addElement(bookmarkEL, SessionServiceConst.DESCRIPTION,"");
		MRXMLUtils.addElement(bookmarkEL, (Element) MRXMLUtils.getElement(bookmarkObjectDoc.getRootElement(), MediaStoreServiceConst.META_DATA_ELEMENT_NAME).clone());
		return bookmarkEL;
	}
  	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		String requestedTC = "";
		String requestedOffset = "";
		Element bookmarkEL = getNewBookmark(log);
		Element requestEl = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//"+SessionServiceConst.NEW_BOOKMARK);
		handleNewBookmark(request, xmpp, log, db, resourceFactory, serviceAgentFactory,requestEl, requestedTC, requestedOffset, bookmarkEL, SessionServiceConst.NEW);
		Element bookmarkResponseEL = MRXMLUtils.newElement(SessionServiceConst.NEW_BOOKMARK);
		MRXMLUtils.addElement(bookmarkResponseEL, bookmarkEL);
		return request.createResponseData(bookmarkResponseEL);
	}
	
}
