package com.ipvs.mediaroomservice.handler;

import org.dom4j.Document;

import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;


public class GetPVRDestMediaDirListRequestHandler extends GetDestMediaDirListRequestHandler {
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
        throws MRException, Exception {
        try {
            Document responseDoc = getDirList(PolicyAdminConstants.PVR_ROLE_ACTION, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            //getPVRDirList(responseDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

            return responseDoc;
        } catch (MRException e) {
            e.printStackTrace();
            throw new MRException(e.getCode(), e.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(MediaRoomServiceExceptionCodes.GET_PVR_DEST_MEDIA_DIR_FAILED, exp.toString());
        }
    }

    /*private void getPVRDirList(Document responseDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        List<?> listOfMediaStreamDir = MRXMLUtils.getListXpath(responseDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE);

        if ((listOfMediaStreamDir == null) || listOfMediaStreamDir.isEmpty()) {
            return;
        }

        StringBuffer parentDirNIDXpathB = new StringBuffer();
        Element dirEL = null;
        String parentDirNID = null;
        String delim = "";

        for (Object object : listOfMediaStreamDir) {
            dirEL = (Element) object;
            parentDirNID = MRXMLUtils.getAttributeValue(dirEL, MediaStoreServiceConst.NID, "");
            parentDirNIDXpathB.append(delim + " " + MediaStoreServiceConst.PROPERTIES_EL_NAME + "/" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME + " ='" + parentDirNID + "'");
            delim = " or ";
        }

        String whereXpath = "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "[(" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME + "='" + MediaStoreServiceConst.PVR_DIR_NAME + "') and (" + parentDirNIDXpathB.toString() + ")]";
        log.addInfo("PVR DIR Where Xpath :=" + whereXpath, MRLog.INFO, MRLog.NOTIFY);

        List<?> pvrDirList = MediaStoreServiceUtils.getMediaStreamDirList(whereXpath, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        Element responseXMLEL = MRXMLUtils.getElementXpath(responseDoc, "//ResponseXML");
        responseXMLEL.removeContent();

        for (Object object : pvrDirList) {
            MRXMLUtils.addElement(responseXMLEL, (Element) ((Element) object).detach());
        }
    }*/
}
