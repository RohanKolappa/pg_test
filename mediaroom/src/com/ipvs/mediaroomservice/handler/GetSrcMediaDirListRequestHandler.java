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


public class GetSrcMediaDirListRequestHandler implements MRRequestHandlerI {
    @Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws Exception {
        // TODO Auto-generated method stub
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

            destNID = MRXMLUtils.getValue(destElement);

            String resourceXPath = GetSourceLib.getSourceXpath(destNID, PolicyAdminConstants.PLAYBACK_ROLE_ACTION, request, xmpp, log, db, resourceFactory, serviceAgentFactory);


            String priorityXpath = ".//MediaStorePortResourceNIDList/MediaStorePortResourceNID/@priority and .//MediaStorePortResourceNIDList/MediaStorePortResourceNID/@priority!=''";

            String stateXpath = "[State/StateFlag = 'Ready']";
            StringBuffer assetWhereXpath = new StringBuffer("//MediaStreamDirResource" + stateXpath + "/Info");

            if ((resourceXPath != null) && (resourceXPath.length() == 0) && !"".equals(title.trim())) {
                assetWhereXpath.append("[Title='" + title.trim() + "' and (" + priorityXpath + ")]");
            } else if ((resourceXPath != null) && (resourceXPath.length() == 0) && "".equals(title.trim())) {
                assetWhereXpath.append("[(" + priorityXpath + ")]");
            } else if (!"".equals(title.trim())) {
                assetWhereXpath.append("[Title='" + title.trim() + "' and ( " + resourceXPath + ") and (" + priorityXpath + ")]");
            } else {
                assetWhereXpath.append("[( " + resourceXPath + ") and (" + priorityXpath + ")]");
            }

            pemXPath = assetWhereXpath.toString();

            Document responseDoc = MediaRoomServiceUtils.handleGetRequest(pemXPath, AMConst.MEDIA_STREAM_DIR_RESOURCE, ServiceClient.ASSETADMINSERVICE, request, xmpp, log, db, serviceAgentFactory,
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
