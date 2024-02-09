package com.ipvs.mediastoreservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.UpdateMediaStreamFileResourceStoreInfoRequestHandler;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class UpdateMediaStoreFileResponseHandler implements AgentResponseHandlerI {
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
    	handleResponse(responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }
    
    public static void handleResponse (MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)  throws MRException,Exception{
    	   Document responseDoc = request.getResult();
           int state = MediaStoreServiceConst.SUCCESS_STATE;
           Document mediaStreamFileDoc = null;
           String logData = "";
           Element responseXMLEL = MRXMLUtils.newElement("ResponseXML");

           try {
               //Validate the response doc and returns the media stream file document if the response is ok.
               MediaStoreServiceUtils.validateMediaStoreResponse(responseDoc,  MediaStoreServiceExceptionCodes.UPDATE_MEDIA_STORE_FILE_FAILED);
              
           } catch (MRException exp) {
               state = MRRequest.STATE_EXCEPTION;
               logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());
               request.setException(exp);
               exp.printStackTrace();
           } catch (Exception exp) {
               state = MRRequest.STATE_EXCEPTION;

               MRException me = new MRException(MediaStoreServiceExceptionCodes.UPDATE_MEDIA_STORE_FILE_FAILED, exp.toString());
               request.setException(me);
               logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(MediaStoreServiceExceptionCodes.UPDATE_MEDIA_STORE_FILE_FAILED), "Description", exp.toString());
               exp.printStackTrace();
           }
           String mediaStreamFileNID = MRXMLUtils.getValueXpath(responseDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
           mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);

           MRXMLUtils.addElement(responseXMLEL, (Element) mediaStreamFileDoc.getRootElement().clone());
           request.setState(state);

           MRRequest mediaStreamFilerequest = MediaStoreServiceUtils.createMediaStreamFileRequest(request, responseDoc, responseXMLEL, "ReplaceResponse", log);
           if(mediaStreamFilerequest == null) {               
        	   return;
           }
           //log audit event if the request is coming from update store info request 
           if(mediaStreamFilerequest.getRequestName().equals(MRRequest.getRequestName(UpdateMediaStreamFileResourceStoreInfoRequestHandler.class.getName()))) {
        	   	mediaStreamFilerequest.addAuditEvent(AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(mediaStreamFilerequest, xmpp, log, db, AuditEventUtils.getAuditEventStatus(mediaStreamFilerequest),  AuditEventUtils.getAuditEventLevel(mediaStreamFilerequest), 
       				AuditEventUtils.getAuditEventErrorDescription(mediaStreamFilerequest), AMConst.MEDIA_STREAM_FILE_RESOURCE, AMConst.REPLACE_REQUEST)); 
           }
           MediaStoreServiceUtils.updateMediaGroupOnMediaClipUpdate(mediaStreamFileDoc, request, xmpp, log, db, resourceFactory,
       			serviceAgentFactory);
           responseHandler.handleResponse(mediaStreamFilerequest, logData);
    }
}
