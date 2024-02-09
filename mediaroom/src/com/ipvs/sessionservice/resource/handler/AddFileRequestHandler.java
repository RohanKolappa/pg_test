package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class AddFileRequestHandler extends FileBaseRequestHandler {
	
	@Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {	
    	log.addInfo("AddDeviceRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	
    	Element requestElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(),"//"+MRRequest.getRequestRootName(request.getRequestName()));
    	String dirNID = MRXMLUtils.getAttributeValue(requestElement, SessionServiceResourceConst.BY_DIR_NID,"");
    	
    	if(dirNID.isEmpty()) {
            throw new MRException(MediaRoomServiceExceptionCodes.PARENT_MEDIA_DIR_MISSING, "Parent Media Dir NID Missing");
    	}
	
    	String type = MRXMLUtils.getAttributeValue(requestElement, SessionServiceConst.TYPE_ATTRIBUTE);
    	if(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.valueOf(type) == null) {
        	throw new MRException(MediaStoreServiceExceptionCodes.INVALID_MEDIA_TYPE, MRXMLUtils.generateErrorXML("mediatype", type));
    	}    
    	
    	Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
    	
     	Document mediaStoreRequestDoc = (Document) request.getMessage().getDocument().clone();
     	MRXMLUtils.setAttributeValueXPath(mediaStoreRequestDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
    	
     	Element mediaStorePortResourceEL = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaStreamDirDoc, true, db, log);
		String dirId = MRXMLUtils.getAttributeValue(mediaStorePortResourceEL, MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME, "");
		
     	MRXMLUtils.setAttributeValueXPath(mediaStoreRequestDoc, "//AddFile", "byDirID", dirId);
     	
     	
    	Element mediaStorePortElement =MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaStreamDirDoc, true, db, log,true);
		sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStoreRequestDoc, mediaStorePortElement, this.getClass().getName());
		
    	return null; 
    }
	
	
	@Override
    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc,MRRequest userRequest,Document agentResponseDoc) throws MRException,Exception {
		
		Element groupsEl = MRXMLUtils.getElementXpath(userRequestdoc, "//"+MediaStoreServiceConst.GROUPS_EL_NAME);	
		Document responseDoc = createOrUpdateMediaStreamFileInDB(userRequest, agentResponseDoc, groupsEl, true, xmpp, log, db, resourceFactory, serviceAgentFactory);
        String fileNID = MRXMLUtils.getAttributeValueXpath(responseDoc, "//File", "NID", "");
        MRXMLUtils.setAttributeValueXPath(responseDoc, "//File", "dirty", "true");
        ((AppServerAgent)(serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourceManager().queueObjectNotification(xmpp, userRequest, log, db, SessionServiceConst.FILE, SessionServiceConst.FILE, fileNID, SessionServiceConst.ADD, responseDoc);
        return responseDoc;
	}
}

	
 
