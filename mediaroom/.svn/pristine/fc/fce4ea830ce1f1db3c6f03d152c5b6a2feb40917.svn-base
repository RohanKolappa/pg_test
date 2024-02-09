package com.ipvs.mediastoreservice.handler;

import org.dom4j.Document;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.handler.UpgradeMediaStreamFileResourceRequestHandler;
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
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class UpgradeMediaResponseHandler implements AgentResponseHandlerI {
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        handleResponse(responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }

    public static void handleResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Document responseDoc = request.getResult();
        MRRequest userRequest = MediaStoreServiceUtils.getUserRequest(request, responseDoc, log);
        int state = MRRequest.STATE_DONE;
        String logData = null;

        try {
            MediaStoreServiceUtils.validateMediaStoreResponse(responseDoc, AMExceptionCodes.UPGRADE_MEDIA_STREAM_FILE_FAILED);

            
            UpgradeMediaStreamFileResourceRequestHandler requestHandler = new UpgradeMediaStreamFileResourceRequestHandler();
            Document userResponseDoc = requestHandler.getResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);

            if (userResponseDoc == null) {
                return;
            }

            userRequest.setResult(userResponseDoc);
        } catch (MRException exp) {
            state = MRRequest.STATE_EXCEPTION;
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());
            userRequest.setException(exp);
            exp.printStackTrace();
        } catch (Exception exp) {
            state = MRRequest.STATE_EXCEPTION;

            MRException me = new MRException(AMExceptionCodes.UPGRADE_MEDIA_STREAM_FILE_FAILED, exp.toString());
            userRequest.setException(me);
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(AMExceptionCodes.UPGRADE_MEDIA_STREAM_FILE_FAILED), "Description", exp.toString());
            exp.printStackTrace();
        }

        userRequest.setState(state);
        responseHandler.handleResponse(userRequest, logData);
    }
}
