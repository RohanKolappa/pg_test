package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
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
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;


public class SetFileMetaDataRequestHandler extends FileBaseRequestHandler {

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		log.addInfo("SetFileMetaDataRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
    	RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, AMConst.MEDIA_STREAM_FILE_RESOURCE);
		
		Document mediaStreamFileDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, 
				serviceAgentFactory, requestObject, "", "");
		if(mediaStreamFileDoc == null) 
			throw new MRException(SessionServiceExceptionCodes.OTHER, "NID or Title is incorrect!");
		Element metaDataEl = MRXMLUtils.getElementXpath(requestObject.getRequestElement(), ".//MetaData");
		Element actionEL = MediaRoomServiceUtils.cloneDataElement(request);
		//String actionData = MRXMLUtils.elementToString(actionEL);
		
		new BaseManagementRequestHandler().customValidateReq("Replace", MRConfig.getObjectServiceName(AMConst.MEDIA_STREAM_FILE_RESOURCE), AMConst.MEDIA_STREAM_FILE_RESOURCE, null, actionEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
		
		this.sendSetFileRequestToMediaStore(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamFileDoc,
				"", "", "", "", (Element)metaDataEl.clone(), this.getClass().getName());
		return null;
	}	


	protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc,MRRequest userRequest,Document agentResponseDoc) throws MRException,Exception {		
		
		return createOrUpdateMediaStreamFileInDB(userRequest, agentResponseDoc, null, false, xmpp, log, db, resourceFactory, serviceAgentFactory);
				
	}
}
