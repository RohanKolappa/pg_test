package com.ipvs.mediastoreservice.handler;

import org.dom4j.Document;

import com.ipvs.app.IPVSClientApplet;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class UpgradeMediaStoreDirResponseHandler implements AgentResponseHandlerI {
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document responseDoc = request.getResult();

        int state = MRRequest.STATE_DONE;
        String logData = "";

        try {
            //Validate the response 
            MediaStoreServiceUtils.validateMediaStoreResponse(responseDoc, MediaStoreServiceExceptionCodes.UPGRADE_MEDIA_STREAM_DIR_FAILED);
        } catch (MRException exp) {
            state = MRRequest.STATE_EXCEPTION;
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());
            request.setException(exp);
            exp.printStackTrace();
        } catch (Exception exp) {
            state = MRRequest.STATE_EXCEPTION;

            MRException me = new MRException(MediaStoreServiceExceptionCodes.UPGRADE_MEDIA_STREAM_DIR_FAILED, exp.toString());
            request.setException(me);
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(MediaStoreServiceExceptionCodes.UPGRADE_MEDIA_STREAM_DIR_FAILED), "Description", exp.toString());
            exp.printStackTrace();
        }

        MRRequest userRequest = MediaStoreServiceUtils.getUserRequest(request, responseDoc, log);

        userRequest.setState(state);
        userRequest.setException(request.getException());
        userRequest.setResult(userRequest.createResponseData());
        responseHandler.handleResponse(userRequest, logData);
     
        // Restart persistent pvr sessions        
        String mediaStreamDirNID = MRXMLUtils.getValueXpath(userRequest.getMessage().getDocument(), "//" + MediaStoreServiceConst.MEDIA_STREAM_DIR_RESOURCE_NID, "");
        if (!mediaStreamDirNID.isEmpty()) {        	        	
        	AppServerAgent theAppServerAgent = (AppServerAgent) serviceAgentFactory.getServiceAgent(IPVSClientApplet.APPSERVERAGENT);
        	theAppServerAgent.handlePersistentPVRSessionRestart(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamDirNID, true);
        }
    }
}
