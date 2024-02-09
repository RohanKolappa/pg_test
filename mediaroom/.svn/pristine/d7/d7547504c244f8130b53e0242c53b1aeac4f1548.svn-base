package com.ipvs.assetadminservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
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

public class QueueStatusExportImportFileRequestHandler implements MRRequestHandlerI {
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

        // get request document
        Document requestDoc = request.getMessage().getDocument();

        // create response document
        Element respRoot = AssetAdminUtils.getResponseDoc(this.getClass());

        // get element from request
        Element reqEL = MRXMLUtils.getElementXpath(requestDoc.getRootElement(), "//" + respRoot.getName());
        if (reqEL == null) {
            // todo:MRException
            throw new Exception("Invalid request " + respRoot.getName());
        }
        //if not already executed ... try this first
        FileSMQueue.getInstance().timeoutOnReboot(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null);
        
        String queue_state = FileSMQueue.getInstance().getQueueState();
        MRXMLUtils.addElement(respRoot, "QueueState", queue_state + "");

        boolean isSuspended = FileSMQueue.getInstance().isSuspendGlobal();
        boolean isRunning = FileSMQueue.getInstance().isRunning();
        MRXMLUtils.addElement(respRoot, "Suspended", isSuspended + "");
        MRXMLUtils.addElement(respRoot, "Running", isRunning + "");
        
        // return response
        return respRoot.getDocument();
    }
}