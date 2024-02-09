package com.ipvs.mediaroomservice.handler;

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
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;


public class UpdateBookmarkRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        String bookMarkRequestXML = "Bookmark_BASE_ALL_UPDATE.xml";

        try {
            Document requestDoc = request.getMessage().getDocument();
            String bookmarkNID = MRXMLUtils.getValueXpath(requestDoc, "//" + MediaStoreServiceConst.BOOKMARK_NID, "");

            if (bookmarkNID.isEmpty()) {
                throw new MRException(MediaRoomServiceExceptionCodes.BOOKMARK_NID_NOT_FOUND, "Bookmark NID Missing");
            }

            //set the NID
            Element requestedbookmarkEL = MRXMLUtils.getElementXpath(requestDoc, "//" + AMConst.BOOKMARK);
            MRXMLUtils.setAttributeValue(requestedbookmarkEL, MediaStoreServiceConst.NID, bookmarkNID);

            //set the parent NID
            MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.BOOKMARK, log);
            String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
            MRXMLUtils.setAttributeValue(requestedbookmarkEL, MediaStoreServiceConst.PARENT_NID, parentNID);

            return MediaRoomServiceUtils.getAssetAdminResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, ServiceClient.ASSETADMINSERVICE, bookMarkRequestXML, bookmarkNID, null, (Element) requestedbookmarkEL.clone(), "Replace");
        } catch (MRException exp) {
            exp.printStackTrace();
            throw new MRException(exp.getCode(), exp.getMessage());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(MediaRoomServiceExceptionCodes.UPDATE_BOOK_MARK_FAILED, exp.toString());
        }
    }
}
