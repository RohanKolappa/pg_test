package com.ipvs.mediastoreservice.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class DeleteMediaStoreFileResponseHandler implements AgentResponseHandlerI {
    public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        Document responseDoc = request.getResult();
        log.addInfo("DeletMediaStoreFileResponse: Received Response From Media Store Admin. Delete the Media Stream File.", MRLog.INFO, MRLog.NOTIFY);

        int state = MRRequest.STATE_DONE;

        String logData = "";
        String mediaStreamFileNID = "";

        try {
        	//Validate the response doc and returns the media stream file document if the response is ok.
            MediaStoreServiceUtils.validateMediaStoreResponse(responseDoc, MediaStoreServiceExceptionCodes.DELETE_MEDIA_STORE_FILE_FAILED);
            mediaStreamFileNID = MRXMLUtils.getValueXpath(responseDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
            // Delete MediaStreamFile 
            MediaStoreServiceUtils.deleteMediaStreamFileDB(mediaStreamFileNID, request, log, db, xmpp, resourceFactory, serviceAgentFactory);

        } catch (MRException exp) {
            state = MRRequest.STATE_EXCEPTION;
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());
            request.setException(exp);
            exp.printStackTrace();
        } catch (Exception exp) {
            state = MRRequest.STATE_EXCEPTION;

            MRException me = new MRException(MediaStoreServiceExceptionCodes.DELETE_MEDIA_STORE_FILE_FAILED, exp.toString());
            request.setException(me);
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(MediaStoreServiceExceptionCodes.DELETE_MEDIA_STORE_FILE_FAILED), "Description", exp.toString());
            exp.printStackTrace();
        }

        MRRequestHandlerI handler = null;
        MRRequest userRequest = MediaStoreServiceUtils.getUserRequest(request, responseDoc, log);
        if(userRequest == null) {
        	return;
        }

        if (AMConst.EMPTY_DIR_REQUEST.equals(userRequest.getRequestName())) {
            Document userRequestDoc = userRequest.getMessage().getDocument();
            String dirNID = MRXMLUtils.getValueXpath(userRequestDoc, "//" + AMConst.MEDIA_STREAM_DIR_NID, "");

            Document assetRequestDoc = AssetAdminServiceUtils.createEmptyRecycleBinRequest(dirNID, "", log);
            MRRequest storedRequest = MediaStoreServiceUtils.getRequestObject(responseDoc, log);
            MRRequest assetRequest = MRRequest.createRequest(storedRequest.getUserJID(), storedRequest.getLogLevel(), storedRequest.getNID(), storedRequest.getServiceName(), storedRequest.getRequestName(), new MRMessage(assetRequestDoc), storedRequest.getResourceGroup(),
                    storedRequest.getAppServerJID(), storedRequest.getUserAgentName(), storedRequest.getClientData());

            handler = (MRRequestHandlerI) Class.forName(AssetAdminServiceUtils.getHandlerClassName(assetRequest.getRequestName())).newInstance();
            log.addInfo("DeletMediaStoreFileResponse: Call Empty Recycle Bin Handler to process with next file deletion or not.", MRLog.INFO, MRLog.NOTIFY);
            handler.getResponse(assetRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
        } else {
            request.setState(state);
            Element  responseEL = MRXMLUtils.stringToElement(MRXMLUtils.generateXML("ResponseXML", ""));
            Element mediaStreamFileEL = MRXMLUtils.stringToElement(MRXMLUtils.generateXML(AMConst.MEDIA_STREAM_FILE_RESOURCE, ""));

            MRXMLUtils.setAttributeValue(mediaStreamFileEL, MediaStoreServiceConst.NID, mediaStreamFileNID);
            MRXMLUtils.addElement(responseEL,(Element)mediaStreamFileEL.clone());
            MRRequest mediaStreamFilerequest = MediaStoreServiceUtils.createMediaStreamFileRequest(request, responseDoc, responseEL, "DeleteResponse", log);

            responseHandler.handleResponse(mediaStreamFilerequest, logData);
        }
    }
}
