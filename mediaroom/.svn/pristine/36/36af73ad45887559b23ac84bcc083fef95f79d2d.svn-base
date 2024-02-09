package com.ipvs.assetadminservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBServiceUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;


public class ChangePasswordRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
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

    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws Exception {
        Document requestDoc = request.getMessage().getDocument();

        String oldPassword = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.OLD_PASSWORD_EL_NAME, "");

        if (oldPassword.isEmpty()) {
            throw new MRException(AMExceptionCodes.OLD_PASSWORD_MISSING, "Old Password Value Missing");
        }

        String newPassword = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.NEW_PASSWORD_EL_NAME, "");

        if (newPassword.isEmpty()) {
            throw new MRException(AMExceptionCodes.NEW_PASSWORD_MISSING, "New Password Value Missing");
        }

        String retypePassword = MRXMLUtils.getValueXpath(requestDoc, "//" + AMConst.RETYPE_PASSWORD_EL_NAME, "");

        if (!newPassword.equals(retypePassword)) {
            throw new MRException(AMExceptionCodes.NEW_PASSWORD_AND_RETYPE_PASSWORD_NOT_MATCHING, "New Password and Retype Password is not matching");
        }

        Element userEl = DBServiceUtils.getUser(request, db, log);
        Document userDoc = null;

        if (userEl == null) {
            throw new MRException(AMExceptionCodes.NO_USER_FOUND, "No User Found");
        }

        userDoc = MRXMLUtils.elementToDocument(userEl);

        String userPassword = MRXMLUtils.getValueXpath(userDoc, "//" + AMConst.PASSWORD_EL_NAME, "");

        if (!userPassword.equals(oldPassword)) {
            throw new MRException(AMExceptionCodes.USER_PASSWORD_NOT_MATCHING, "User Password is not matching");
        }

        MRXMLUtils.setValueXpath(userDoc, "//" + AMConst.PASSWORD_EL_NAME, newPassword);

        String userNID = MRXMLUtils.getAttributeValue(userDoc.getRootElement(), "//" + AMConst.NID, "");

        request.setSuperUser(true);

        Document responseDoc = AssetAdminServiceUtils.updateAssetAdminObjectInDB(AMConst.User, userNID, (Element) userDoc.getRootElement().clone(), request, log, db, xmpp, resourceFactory, profileFactory);

        request.setSuperUser(false);

        return responseDoc;
    }
}
