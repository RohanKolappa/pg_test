package com.ipvs.assetadminservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;

public class DeleteExportImportFileRequestHandler implements MRRequestHandlerI, AgentResponseHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) throws Exception {
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
     public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {

        //get request document
        Document requestDoc = request.getMessage().getDocument();

        //create response document
        Element respRoot = AssetAdminUtils.getResponseDoc(this.getClass());

        //get element from request
        Element reqEL = MRXMLUtils.getElementXpath(requestDoc.getRootElement(), "//" + respRoot.getName());
        if (reqEL == null) {
            //todo:MRException
            throw new Exception("Invalid request " + respRoot.getName());
        }
        //get request params : where clause
        String whereDateCreated = FileSMUtils.getWhereRequestParam("@dateCreated", MRXMLUtils.getAttributeValue(reqEL, "whereDateCreated", "").trim());
        String whereState = FileSMUtils.getWhereRequestParam("@state", MRXMLUtils.getAttributeValue(reqEL, "whereState", "").trim());
        String whereStateDescription = FileSMUtils.getWhereRequestParam("@state_description", MRXMLUtils.getAttributeValue(reqEL, "whereStateDescription", "").trim());
        String whereOwnerJID = FileSMUtils.getWhereRequestParam("@ownerJID", MRXMLUtils.getAttributeValue(reqEL, "whereOwnerJID", "").trim());
        String whereType = FileSMUtils.getWhereRequestParam("@type", MRXMLUtils.getAttributeValue(reqEL, "whereType", "").trim());
        String wherePriority = FileSMUtils.getWhereRequestParam("@priority", MRXMLUtils.getAttributeValue(reqEL, "wherePriority", "").trim());
        String whereNID = FileSMUtils.getWhereRequestParam("@NID", MRXMLUtils.getAttributeValue(reqEL, "whereNID", "").trim());
        String whereQueueState = FileSMUtils.getWhereRequestParam("@queue_state", MRXMLUtils.getAttributeValue(reqEL, "whereQueueState", "").trim());

        //get Object MRConfig
        String[] arrWhereXpath = { whereDateCreated, whereState, whereOwnerJID, whereType, wherePriority, whereNID ,whereQueueState, whereStateDescription};//ui can delete any thing
        String[] arrWhereXpathRead = { whereDateCreated, whereState, whereOwnerJID, whereType, wherePriority, whereNID, whereQueueState, whereStateDescription, "@queue_state =('" + FileSMQueue.QUEUE_STATUS_ERROR + "','" + FileSMQueue.QUEUE_STATUS_NOT_STARTED + "','" + FileSMQueue.QUEUE_STATUS_ERROR_RESUME + "','" + FileSMQueue.QUEUE_STATUS_RUNNING + "')" };
        FileSMUtils.delete_queue(respRoot, arrWhereXpath, arrWhereXpathRead, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        //return response    
        return respRoot.getDocument();

    }
    @Override
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        Document responseDoc = request.getResult();
        String requestName = MRXMLUtils.getAttributeValueXpath(responseDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
        log.addInfo("DeleteExportImportFileRequestHandler:handleAgentResponse  :" + " after delete requestName=" + requestName, MRLog.OK, MRLog.NOTIFY);
    }
}
