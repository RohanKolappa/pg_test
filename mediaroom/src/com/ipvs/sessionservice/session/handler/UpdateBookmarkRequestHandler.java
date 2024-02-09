package com.ipvs.sessionservice.session.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
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
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;


public class UpdateBookmarkRequestHandler extends BookmarkBaseRequestHandler {
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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        Document requestDoc = request.getMessage().getDocument();
        Element updateBookmarkEL = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.UPDATE_BOOKMARK);
        String bookmarkNID = MRXMLUtils.getAttributeValue(updateBookmarkEL, SessionServiceConst.WHERE_NID, "");

        if (bookmarkNID.isEmpty()) {
            throw new MRException(SessionServiceExceptionCodes.BOOKMARK_NID_MISSING, "NID not found");
        }

        Document bookmarkObjectDoc = MRXMLDBUtils.readElement(request, log, db, bookmarkNID);

        String strTC = MRXMLUtils.getAttributeValueXpath(updateBookmarkEL, "./" + AMConst.BOOKMARK, SessionServiceConst.TC, "");
        String strOffset = MRXMLUtils.getAttributeValueXpath(updateBookmarkEL, "./" + AMConst.BOOKMARK, SessionServiceConst.OFFSET, "");

        if (strTC.isEmpty() && strOffset.isEmpty()) {
            strTC = MRXMLUtils.getValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.AT_TC, "");
            strOffset = MRXMLUtils.getValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.BOOKMARK_OFFSET, "");
        }

        String bookmarkFileNID = MRXMLUtils.getValueXpath(bookmarkObjectDoc,
                "//" + AMConst.BOOKMARK + "/" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
        
        checkFileNID(bookmarkFileNID);

        bookmarkObjectDoc = createBookmarkObjectDocument(bookmarkObjectDoc, requestDoc, strTC, strOffset, request.getUserJID(), bookmarkFileNID,
                xmpp, log);

     
        sendAddUpdateBookmarkRquestToDMS(bookmarkObjectDoc, bookmarkFileNID, strTC, strOffset, null, null, "", null, request,"UpdateBookmarkRequest", log, xmpp, db, resourceFactory, serviceAgentFactory);
 
        return null;
    }
    
    
    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc,MRRequest userRequest,Document agentResponseDoc) throws MRException,Exception {
		
		return createOrUpdateBookmarkInDB(userRequest, agentResponseDoc, userRequestdoc, true, SessionServiceConst.UPDATE_BOOKMARK,xmpp, log, db, resourceFactory, serviceAgentFactory);

	}
}
