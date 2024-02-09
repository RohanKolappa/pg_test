package com.ipvs.mediastoreservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

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
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class AddMediaStoreFileResponseHandler implements AgentResponseHandlerI {
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document responseDoc = request.getResult();
        int state = MRRequest.STATE_DONE;
        Document mediaStreamFileDoc = null;
        String logData = "";  
        Element responseXMLEL = MRXMLUtils.newElement("ResponseXML");

        try {
            // Validate the response Document and check the media stream file state 
            mediaStreamFileDoc = MediaStoreServiceUtils.checkMediaStreamFileState(responseDoc, MediaStoreServiceExceptionCodes.ADD_MEDIA_STORE_FILE_FAILED, log, db, request);
            MRXMLUtils.addElement(responseXMLEL,(Element)mediaStreamFileDoc.getRootElement().clone());
            
             
        } catch (MRException exp) {
            state = MRRequest.STATE_EXCEPTION;
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());
            request.setException(exp);
            exp.printStackTrace();
        } catch (Exception exp) {
            state = MRRequest.STATE_EXCEPTION;

            MRException me = new MRException(MediaStoreServiceExceptionCodes.ADD_MEDIA_STORE_FILE_FAILED, exp.toString());
            request.setException(me);
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(MediaStoreServiceExceptionCodes.ADD_MEDIA_STORE_FILE_FAILED), "Description", exp.toString());
            exp.printStackTrace();
        }

        request.setState(state);

        MRRequest mediaStreamFilerequest = MediaStoreServiceUtils.createMediaStreamFileRequest(request, responseDoc,  responseXMLEL, "AddResponse", log);

        responseHandler.handleResponse(mediaStreamFilerequest, logData);
    }
}
