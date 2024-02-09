package com.ipvs.mediaroomservice.handler;

import java.util.List;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
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
import com.ipvs.policyadminservice.handler.MediaRoomPolicyHelper;
import com.ipvs.policyadminservice.impl.GetDestinationLib;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyManagementExceptionCodes;
import com.ipvs.utils.Utils;


public class GetDestMediaDirListRequestHandler implements MRRequestHandlerI {
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
            return getDirList(PolicyAdminConstants.RECORD_ROLE_ACTION, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        } catch (MRException e) {
            e.printStackTrace();
            throw new MRException(e.getCode(), e.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(MediaRoomServiceExceptionCodes.GET_DEST_MEDIA_DIR_FAILED, exp.toString());
        }
    }

    protected Document getDirList(String roleAction, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        XMPPTransaction.startBlock("getDirList");
        Document requestDoc = request.getMessage().getDocument();
        String title = "";
        Element titleElement = MRXMLUtils.getElementXpath(requestDoc, "//title");

        if (titleElement != null) {
            title = MRXMLUtils.getValue(titleElement);
        }

        String pemXPath = "";   

        String srcNID = "";
        Element srcElement = MRXMLUtils.getElementXpath(requestDoc, "//sourceNID");

        if (srcElement == null) {
            throw new MRException(PolicyManagementExceptionCodes.MISSING_VALUE, MediaRoomPolicyHelper.generateErrorMessage("data", "sourceNID"));
        }

        srcNID = MRXMLUtils.getValue(srcElement);
        
        String resourceXPath = GetDestinationLib.getDestinationXpath(srcNID, roleAction, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        //make sure we not retrieving the directories that does not have priority
        String storeSettingXpath = " ((.//MediaStorePortResourceNIDList/MediaStorePortResourceNID/@readOnly != 'true')  " + "and (.//MediaStorePortResourceNIDList/MediaStorePortResourceNID/@priority)  " + "and (.//MediaStorePortResourceNIDList/MediaStorePortResourceNID/@priority!=''))";

        String stateXpath = "[State/StateFlag = 'Ready']";
        String DiskFullThreasholdXpath = "((Properties/Size - Properties/SizeUsed ) >= MediaDirStorageConfig/MediaDirSpanning/@diskFullThreshold )";
        String writePermissionsXpath = "(Properties/Permissions/Write='true')";
        String parentDirXpath = "(//ParentMediaDirNID='"+MediaStoreServiceConst.DEFAULT_DIR+"')";

        String xPath = storeSettingXpath + " and " + DiskFullThreasholdXpath + " and "+writePermissionsXpath + " and " +parentDirXpath;

        StringBuffer whereXpath = new StringBuffer("//MediaStreamDirResource" + stateXpath + "/Info");

        if ((resourceXPath != null) && (resourceXPath.length() == 0) && !"".equals(title.trim())) {
            whereXpath.append("[Title=" + title.trim() + " and (" + xPath + ")]");
        } else if ((resourceXPath != null) && (resourceXPath.length() == 0) && "".equals(title.trim())) {
            whereXpath.append("[" + xPath + "]");
        } else if (!"".equals(title.trim())) {
            whereXpath.append("[Title=" + title.trim() + "' and  (" + resourceXPath + " ) " + " and " + xPath + "]");
        } else {
            whereXpath.append("[ (" + resourceXPath + " ) and " + xPath + "]");
        }

        pemXPath = whereXpath.toString();

        log.addInfo("DirList PEMWhereXPath=" + pemXPath, MRLog.INFO, MRLog.NOTIFY);
        
        Document responseDoc = MediaRoomServiceUtils.handleGetRequest(pemXPath, AMConst.MEDIA_STREAM_DIR_RESOURCE, ServiceClient.ASSETADMINSERVICE, request, xmpp, log, db, serviceAgentFactory,
                resourceFactory);
        
        //getPriorityBasedDirList(responseDoc);    
        XMPPTransaction.endBlock("getDirList");
        
        return responseDoc;
    }

    protected void getPriorityBasedDirList(Document responseDoc) throws MRException, Exception {
        //assuming always returns highest priority one first
XMPPTransaction.startBlock("getPriorityBasedDirList");
        Element highPriorityEL = MRXMLUtils.getElementXpath(responseDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE);

        if (highPriorityEL == null) {
            return;
        }

        //get list of priorities from  highest priority element
        List<?> priorityList = MRXMLUtils.getListXpath(MRXMLUtils.elementToDocument(highPriorityEL), "//MediaStorePortResourceNID/@priority");
        Attribute attribute = null;
        int priority = 0;
        StringBuffer priorityXpathB = new StringBuffer();
        String delim = "";

        //Generate priority xpath
        for (Object object : priorityList) {
            attribute = (Attribute) object;
            priority = Utils.getIntValue(attribute.getValue(), 0);
            priorityXpathB.append(delim + "(@priority='" + priority + "')");
            delim = " or ";
        }

        String priorityXpath = "//MediaStorePortResourceNID[" + priorityXpathB.toString() + "]";

        List<?> mediaStreamDirList = MRXMLUtils.getListXpath(responseDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE);
        Element dirEL = null;
        Element storePortEL = null;

        // check the priority of the element is there in the response document or not. If not then remove that dir from response
        for (Object object : mediaStreamDirList) {
            dirEL = (Element) object;
            storePortEL = MRXMLUtils.getElementXpath(MRXMLUtils.elementToDocument(dirEL), priorityXpath);

            if (storePortEL == null) {
                dirEL.detach();
            }
        }
        XMPPTransaction.endBlock("getPriorityBasedDirList");
    }
}
