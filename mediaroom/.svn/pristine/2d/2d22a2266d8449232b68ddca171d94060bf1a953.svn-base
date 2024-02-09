package com.ipvs.sessionservice.handler.base;


import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;
import com.ipvs.sessionservice.session.handler.GetPortRequestHandler;



public abstract class SetPortBaseRequestHandler extends BaseDeviceRequestHandler {
	
	

	protected Document getPortObjectXMLDoc(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject) throws Exception {
				
		log.addInfo("SetPortBaseRequestHandler::Get object XML document from db", MRLog.OK, MRLog.NOTIFY);		
		Document objectXMLDoc = GetObjectUtils.getObjectXMLFromDB(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				requestObject, GET_PORT_REQUEST_XML, GetPortRequestHandler.class.getName());		
		
		Element rootEl = objectXMLDoc.getRootElement();		
		String NID = MRXMLUtils.getAttributeValue(rootEl, SessionServiceConst.NID);
		requestObject.setNID(NID);
		requestObject.setObjectType(rootEl.getName());		
		
		return objectXMLDoc;
	}
	
	protected Document updatePort (MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject,Document objectXMLDoc,String parentElXPath) throws Exception {		
		UpdateObjectUtils.updateElement(request, xmpp, log, db, requestObject.getObjectType(), requestObject.getNID(), objectXMLDoc,  parentElXPath);				
		
		return generatePortUpdateNotifications(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject);
	
	}
	
	protected Document generatePortUpdateNotifications(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject) throws Exception {
	   return generatePortUpdateNotifications(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject,  SessionServiceConst.UPDATE);
	}
	
	
	protected Document generatePortUpdateNotifications(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, RequestProcessor requestObject,String updateType) throws Exception {
		
		MRRequest newRequest = MRRequest.createRequest(request.getAppServerJID());
		newRequest.setSuperUser(true);
		Document responseDoc = GetObjectUtils.getResponse(newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, requestObject, 
				SessionServiceConst.SELECTFORMAT_BASICCONNECTIONINFO, GET_PORT_REQUEST_XML, GetPortRequestHandler.class.getName());
		
		Element portEl = MRXMLUtils.getElementXpath(responseDoc, "//Port");
		if(updateType.equals(SessionServiceConst.PORTTYPECHANGE)) {
			MRXMLUtils.setAttributeValue(portEl, "dirty", "true");
		}
		
		// queue port object notifications
		((AppServerAgent)(serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourceManager().queueObjectNotification(xmpp, request, log, db, SessionServiceConst.PORT, SessionServiceConst.PORT, requestObject.getNID(), updateType, responseDoc);

		//remove connection info from response
		MRXMLUtils.removeNodeXpath(responseDoc, "//ConnectionStatus");
		
		return responseDoc;
	}
		
}
