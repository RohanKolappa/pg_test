package com.ipvs.sessionservice.session.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
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
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.handler.base.BookmarkBaseRequestHandler;
import com.ipvs.sessionservice.handler.base.GetBaseObjectListRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class GetBookmarkRequestHandler extends BookmarkBaseRequestHandler {
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

    /**
     * GetBookmarksRequestHandler handles the two objecttypes. Gets the List of
     * the Bookmarks based on the xpath and returns the response.
     *
     * Exception Codes public static final int GET_BOOKMARK_LIST_FAILED = 2254;
     */
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        List<String> listOfXpaths = getBookmarkXPathList(request, xmpp, log, db, resourceFactory, serviceAgentFactory,request.getMessage().getDocument(), SessionServiceConst.GET_BOOKMARK);

        //No PEM check is need for bookmarks. Once Groups are included in bookmark then we have to do pem check        
        request.setSuperUser(true);

        Element requestElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//" + SessionServiceConst.GET_BOOKMARK);
        String sortBy = MRXMLUtils.getAttributeValue(requestElement, "sortBy", "");
        
        String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + AMConst.BOOKMARK + SessionServiceConst.XQUERYXML;

        if (sortBy.equals(SessionServiceConst.TC)) {
            sortBy =  SessionServiceConst.AT_TC;
        } else {
            sortBy =  MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME;
        }

        return new GetBaseObjectListRequestHandler().getObjectListRequest(request, xmpp, log, db, serviceAgentFactory, resourceFactory, requestElement, AMConst.BOOKMARK, sortBy,
                listOfXpaths, xqueryFileName);
    }
    
    
}
