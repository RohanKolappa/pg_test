package com.ipvs.sessionservice.resource.handler;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.sessionservice.handler.base.SetPortBaseRequestHandler;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;


public class SetPortInfoRequestHandler extends SetPortBaseRequestHandler {
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		log.addInfo("SetPortInfoRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "");
				
		Element requestEl = requestObject.getRequestElement();
		
		String title = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.TITLE, null);
		String description = MRXMLUtils.getAttributeValue(requestEl, SessionServiceConst.DESCRIPTION_ATTR_NAME, null);		
		Element groupsElFromRequestDoc = MRXMLUtils.getElementXpath(requestEl, "//" + SessionServiceResourceConst.GROUPS_ELEMENT);

		if (title == null && description == null && groupsElFromRequestDoc == null) {
			throw new MRException(MediaRoomServiceExceptionCodes.NO_VALUES_TO_UPDATE, "Update fields are missing");
		}
		
		// Get port objectXML doc 
		Document objectXMLDoc = this.getPortObjectXMLDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
		updatePortInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, title, description, groupsElFromRequestDoc, objectXMLDoc, requestObject.getObjectType(), requestObject.getNID(), SessionServiceConst.TRUE);
		
		//update policy cache if groups are updating
		if(groupsElFromRequestDoc !=null) {
			StreamPolicyUtils.handlePolicyChanged();
		}
				   
		log.addInfo("SetPortInfoRequestHandler::handleResponse", MRLog.OK, MRLog.NOTIFY);				
		
		// queue port update notifications
		Document responseDoc = generatePortUpdateNotifications(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
		
		return responseDoc;
				
	}
	
}
