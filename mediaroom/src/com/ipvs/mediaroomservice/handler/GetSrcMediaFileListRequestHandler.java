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
import com.ipvs.policyadminservice.handler.MediaRoomPolicyHelper;
import com.ipvs.policyadminservice.impl.GetSourceLib;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;


public class GetSrcMediaFileListRequestHandler implements MRRequestHandlerI {
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
        Document requestDoc = request.getMessage().getDocument();
        String title = "";
        Element titleElement = MRXMLUtils.getElementXpath(requestDoc, "//title");

        if (titleElement != null) {
            title = MRXMLUtils.getValue(titleElement);
        }
             
        String pemXPath = "";
       try {
            String destNID = "";
            Element destElement = MRXMLUtils.getElementXpath(requestDoc, "//destNID");

            if (destElement == null) {
                throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "destNID"));
            }

            String xPath = MRXMLUtils.getValueXpath(requestDoc, "//xPath", "").trim();
            String resourceXPath  = "";
            if ("".equals(xPath)) {
                if (destElement != null) {
                    destNID = MRXMLUtils.getValue(destElement);
                }
                resourceXPath  = GetSourceLib.getSourceXpath(destNID, PolicyAdminConstants.PLAYBACK_ROLE_ACTION, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            } else {
            	resourceXPath = xPath;
            }
                       
            if ((resourceXPath != null) && (resourceXPath.length() == 0) && !"".equals(title.trim())) {
                pemXPath = "//MediaStreamFileResource/Info[title=" + title.trim() + "]";
            } else if ((resourceXPath != null) && (resourceXPath.length() == 0) && "".equals(title.trim())) {
                pemXPath = "//MediaStreamFileResource";
            } else if (!"".equals(title.trim())) {
                pemXPath = "//MediaStreamFileResource/Info[title=" + title.trim() + "' and  " + resourceXPath + "]";
            } else {
                pemXPath = "//MediaStreamFileResource/Info[" + resourceXPath + "]";
            }    

            Document responseDoc = MediaRoomServiceUtils.handleGetRequest(pemXPath, AMConst.MEDIA_STREAM_FILE_RESOURCE, ServiceClient.ASSETADMINSERVICE, request, xmpp, log, db, serviceAgentFactory,
                    resourceFactory);

            return responseDoc;
        } catch (MRException e) {
            e.printStackTrace();
            pemXPath = "";
            throw new MRException(e.getCode(), e.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(PolicyManagementExceptionCodes.PEM_GET_FAILED, exp.toString());
        }
    }
}
