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


public class GetMediaStoreFileTrackResponseHandler implements AgentResponseHandlerI {
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document mediaStoreAdminResponseDoc = request.getResult();
        int state = MRRequest.STATE_DONE;
        String logData = "";
        Element responseXML = null;

        try {
            MediaStoreServiceUtils.validateMediaStoreResponse(mediaStoreAdminResponseDoc, MediaStoreServiceExceptionCodes.GET_MEDIA_STORE_FILE_TRACK_FAILED);
            //Get the Book Mark XML
            responseXML = MRXMLUtils.getElementXpath(mediaStoreAdminResponseDoc, "//ResponseXML");
        } catch (MRException exp) {
            state = MRRequest.STATE_EXCEPTION;
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());
            request.setException(exp);
            exp.printStackTrace();
        } catch (Exception exp) {
            state = MRRequest.STATE_EXCEPTION;

            MRException me = new MRException(MediaStoreServiceExceptionCodes.GET_MEDIA_STORE_FILE_TRACK_FAILED, exp.toString());
            request.setException(me);
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(MediaStoreServiceExceptionCodes.GET_MEDIA_STORE_FILE_TRACK_FAILED), "Description", exp.toString());
            exp.printStackTrace();
        }

        request.setState(state);

        MRRequest mediaStreamFilerequest = MediaStoreServiceUtils.createMediaStreamFileRequest(request, mediaStoreAdminResponseDoc, responseXML, "GetResponse", log);

        responseHandler.handleResponse(mediaStreamFilerequest, logData);
    }

   /* private void getListObjectResponseeElement(Document requestDoc) throws MRException,Exception {
        String xml = " <GetListObjectResponseData><ObjectCount>0</ObjectCount>" 
        	       + "<ObjectLastWhereEndCount>0</ObjectLastWhereEndCount>" 
        	       + "<CountToFetch>50000</CountToFetch>" 
        	       + "</GetListObjectResponseData>";
        Document listObjectResponseDoc = MRXMLUtils.stringToDocument(xml);
      //  MRXMLUtils.
    }*/
}
