package com.ipvs.sessionservice.resource.handler; 

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.handler.base.AsyncBaseRequestHandler;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;

public class OffloadFileRequestHandler extends FileBaseRequestHandler {
   
	@Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document requestDoc = (Document) request.getMessage().getDocument().clone();
		Element requestElement = MRXMLUtils.getElementXpath(requestDoc, "//OffloadFile");
		String dirNID = MRXMLUtils.getAttributeValue(requestElement, SessionServiceResourceConst.BY_DIR_NID,"");
		Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(dirNID, request, db, log);
		String dirID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElementXpath(mediaDirDoc, "//MediaStorePortResourceNID"), "deviceElementId");
		MRXMLUtils.addAttribute(requestElement, SessionServiceResourceConst.BY_DIR_ID_ATTRIBUTE, dirID);
		Document offloadRequestDoc = (Document) requestDoc.clone();
     	MRXMLUtils.setAttributeValueXPath(offloadRequestDoc, MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, true, db, log);
		sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, offloadRequestDoc, mediaStorePortElement, AsyncBaseRequestHandler.class.getName());
        return null;
	}
}
