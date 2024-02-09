package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.BaseManagementRequestHandler;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;


public class SetFileInfoRequestHandler extends FileBaseRequestHandler {

	@Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		log.addInfo("SetFileInfoRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		Document requestDoc = request.getMessage().getDocument();
		String mediaStreamFileNID = MRXMLUtils.getAttributeValueXpath(requestDoc, "//" + MediaStoreServiceConst.SET_FILE_INFO_ELEMENT, SessionServiceResourceConst.BY_NID_ATTRIBUTE, "");
        String title = MRXMLUtils.getAttributeValueXpath(requestDoc, "//" + MediaStoreServiceConst.SET_FILE_INFO_ELEMENT, SessionServiceConst.TITLE, "");
        String description = MRXMLUtils.getAttributeValueXpath(requestDoc, "//" + MediaStoreServiceConst.SET_FILE_INFO_ELEMENT, SessionServiceConst.DESCRIPTION_ATTR_NAME, "");
        Element groupsEL = MRXMLUtils.getElementXpath(requestDoc, "//" + MediaStoreServiceConst.GROUPS_EL_NAME);
       
        Document responseDoc = SetFileWithValidation(request, mediaStreamFileNID, title, description, groupsEL,"", xmpp, log, db, resourceFactory, serviceAgentFactory,true);
     
        if(responseDoc!=null) {
        	return getFileResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamFileNID, "","");
        }
        
        return null;
         
        
	}	
	
	public Document SetFile(MRRequest request, String mediaStreamFileNID, String title, String description, Element groupsEL, String state,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception{
		return SetFileWithValidation(request, mediaStreamFileNID, title, description, groupsEL, state, xmpp, log, db, resourceFactory, serviceAgentFactory, false);
	}
	
	public Document SetFileWithValidation(MRRequest request, String mediaStreamFileNID, String title, String description, Element groupsEL, String state,XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, boolean validateFileInput) throws Exception{
		
		if ((title.isEmpty()) && (description.isEmpty()) && (groupsEL == null) && (state.isEmpty())) {
            throw new MRException(AMExceptionCodes.MSSING_INPUT_VALUE, "No Data Found to Update Media Stream File");
        }

        Document mediaStreamFileDoc = MediaStoreServiceUtils.checkMediaStreamFileState(mediaStreamFileNID, log, db, request);

        Element mediaStreamFileInfoEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.INFO_ELEMENT_NAME);

        if (groupsEL != null) {
            Element mediaStreamFileGroupsEL = MRXMLUtils.getElement(mediaStreamFileInfoEL, MediaStoreServiceConst.GROUPS_EL_NAME);
            MRXMLUtils.replaceContent(mediaStreamFileGroupsEL, groupsEL);
            if(title.isEmpty() && description.isEmpty()) {
            	UpdateObjectUtils.updateElement(request, xmpp, log, db, SessionServiceConst.MEDIA_STREAM_FILE_RESOURCE, mediaStreamFileNID, mediaStreamFileDoc, "//" + MediaStoreServiceConst.GROUPS_EL_NAME);
                return mediaStreamFileDoc;
            }
        }
        
        if (!title.isEmpty()) {
            Element mediaFileTitleEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME);
            MRXMLUtils.setValue(mediaFileTitleEL, title);
        }

        if (!description.isEmpty()) {
            Element mediaDescriptionEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME);
            MRXMLUtils.setValue(mediaDescriptionEL, description);
        }
        
        if (!state.isEmpty()) {
            Element mediaStateEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_STATE_ELEMENT);
            MRXMLUtils.setValue(mediaStateEL, state);
        }
        
        if(validateFileInput) {

            boolean bCustomValidation = new BaseManagementRequestHandler().customValidateReq("Replace", MRConfig.getObjectServiceName(AMConst.MEDIA_STREAM_FILE_RESOURCE), AMConst.MEDIA_STREAM_FILE_RESOURCE, null, mediaStreamFileDoc.getRootElement(), request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            if (!bCustomValidation) {
                throw new MRException(AMExceptionCodes.CUSTOM_VALIDATIONS_FAILED, MRXMLUtils.generateErrorXML("dbobjecttype", AMConst.MEDIA_STREAM_FILE_RESOURCE));
            }
            
            checkPEMPermissions(mediaStreamFileNID, mediaStreamFileDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        }
        
        
        this.sendSetFileRequestToMediaStore(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  
        		mediaStreamFileDoc, title, description, state, "", null, this.getClass().getName());
        
		return null;
	}


	@Override
    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc, MRRequest userRequest, Document agentResponseDoc) throws MRException,Exception {		
		Element groupsEl = MRXMLUtils.getElementXpath(userRequestdoc, "//"+MediaStoreServiceConst.GROUPS_EL_NAME);
	    return createOrUpdateMediaStreamFileInDB(userRequest, agentResponseDoc, groupsEl, false, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	
	
}
