package com.ipvs.sessionservice.impl;


import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.sessionservice.handler.base.BaseDeviceRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.session.handler.GetPortRequestHandler;
import com.ipvs.sessionservice.session.handler.StartSessionRequestHandler;


public class PVRManager {
	
	public static final String GET_SESSION_REQUEST_XML = "/com/ipvs/sessionservice/session/xml/GetSessionRequest.xml";
	
	public void handleDirStateUpdate(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
			ServiceAgentFactoryI serviceAgentFactory, String mediaDirNID, boolean restartSession) throws Exception {
						
		log.addInfo("PVRManager::Handle dir state update and start persistent PVR sessions", MRLog.OK, MRLog.NOTIFY);
		
		Document requestDoc = MRXMLUtils.loadXMLResource(BaseDeviceRequestHandler.GET_PORT_REQUEST_XML, log);		
		Element requestEl = MRXMLUtils.getElementXpath(requestDoc, "//GetPort");		
		MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.SELECTFORMAT, SessionServiceConst.SELECTFORMAT_NATIVE);
		MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.TYPE_ATTRIBUTE, SessionServiceConst.SRC_PORT);
		MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.WHERE_PVR_MEDIA_STREAM_DIR_ATTRIBUTE, mediaDirNID);
		
		String requestName = MRRequest.getRequestName(GetPortRequestHandler.class.getName());							
		Document getPortRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, requestEl);				
		MRRequest getPortRequest = MRRequest.createRequest(request, getPortRequestDoc, requestName, ServiceClient.SESSIONSERVICE_SESSION);
		getPortRequest.setSuperUser(true);	// superuser='true' only for GetPortRequest
		
		GetPortRequestHandler handler = new GetPortRequestHandler();
		Document getPortResponseDoc = handler.getResponse(getPortRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);		
		List<?> dbObjectList = MRXMLUtils.getListXpath(getPortResponseDoc, "//GetPort/*"); 		
		for (Object obj : dbObjectList) {									
			Document objectXMLDoc = MRXMLUtils.elementToDocument((Element) obj);			
			String sessionId = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, "//" + SessionServiceResourceConst.PVR_INFO_ELEMENT, SessionServiceConst.SESSION_ID, "");				
			
			try {
				Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);				 
				String state = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//" + SessionServiceConst.CONNECTION, 
						SessionServiceConst.STATE, "");
				
				if (state.equals("SETUP_ERROR")) {
					restartSession = true;
				}
			} catch (Exception e) { // room not found exception or session not found exception then restart the PVR
			    restartSession = true;
			}
			
			startPersistentPVRSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, objectXMLDoc, restartSession);			
		}
				
	}
	
	public Document startPersistentPVRSession(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document objectXMLDoc, boolean restartSession) throws Exception {	   				
    		String parentElXPath = "//" + SessionServiceResourceConst.PVR_INFO_ELEMENT;    	
		String portNID = MRXMLUtils.getAttributeValue(objectXMLDoc.getRootElement(), SessionServiceConst.NID, "");
		String portTitle = MRXMLUtils.getValueXpath(objectXMLDoc, "//" + SessionServiceResourceConst.INFO_ELEMENT + "/" + SessionServiceResourceConst.TITLE, "");
		String enable = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, parentElXPath, SessionServiceResourceConst.ENABLE_ATTRIBUTE, SessionServiceConst.FALSE);
		String trimLength = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, parentElXPath, SessionServiceConst.TRIMLENGTH, MediaRoomServiceConst.DEFAULT_TRIMLENGTH);
		String dirNID = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, parentElXPath, SessionServiceResourceConst.MEDIA_STREAM_DIR_RESOURCE_NID_ATTRIBUTE, "");
			
		// Get sessionId from the objectXML document     	
		String sessionId = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, parentElXPath, SessionServiceConst.SESSION_ID, "");				
		if (!sessionId.isEmpty()) {
			if (restartSession) {				
				stopPersistentPVRSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, objectXMLDoc);
			} else {
				return null;
			}
		}
		
		// sessionId = Utils.getUUID();		
		// This should be replaced with the above commented statement once the UI has implemented the slider bar
		// to reflect the persistent PVR change in the client UI 
		sessionId = portNID + "." + SessionServiceConst.PVR + SessionServiceConst.SESSION;
		String sessionName = portTitle + "." + SessionServiceConst.PVR + SessionServiceConst.SESSION;
		
		log.addInfo("PVRManager::startPersistentPVRSession.sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		
		// SessionInstance xml
		Document sessionTemplateDoc = Session.getSessionTemplate(log);			
		Element sessionTemplateRootEl = sessionTemplateDoc.getRootElement();
		sessionTemplateRootEl.addAttribute(SessionServiceConst.NAME, sessionName);
		sessionTemplateRootEl.addAttribute(SessionServiceConst.GC, MRConst.TRUE);		
				
		// Add SessionInstance as child to StartSession element 
		Document startSessionRequestDoc = MRXMLUtils.stringToDocument("<StartSession/>");
		Element startSessionRequestEl = startSessionRequestDoc.getRootElement();
		startSessionRequestEl.addAttribute(SessionServiceConst.SESSION_ID, sessionId);		
		startSessionRequestEl.addAttribute(SessionServiceConst.GC, MRConst.TRUE);
		MRXMLUtils.addElement(startSessionRequestEl, (Element) sessionTemplateRootEl.clone());
				
		// Create directory element
		Element dirEl = UpdateSessionUtils.createDirElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirNID, "");
		String dirId = MRXMLUtils.getAttributeValue(dirEl, SessionServiceConst.ID);
		
		// Create source port entity element
		Document srcPortDoc = MRXMLUtils.stringToDocument("<" + SessionServiceConst.SRC_PORT + "/>");
		Element portEl = srcPortDoc.getRootElement();
		portEl.addAttribute("portNID", portNID);
		portEl.addAttribute("portTitle", portTitle);
		portEl.addAttribute(SessionServiceConst.PVRENABLE, enable);
		portEl.addAttribute(SessionServiceConst.TRIMLENGTH, trimLength);
		portEl.addAttribute(SessionServiceConst.DIRID, dirId);	
		Element srcPortEntityEl = UpdateSessionUtils.createSrcPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				srcPortDoc, sessionId);
		
		// Set attribute values and elements to the session document 			
		MRXMLUtils.addElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.DIRLIST, (Element) dirEl.clone());
		MRXMLUtils.addElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.ENTITYLIST, (Element) srcPortEntityEl.clone());
		
		MRRequest startSessionRequest = new MRRequest(request.getAppServerJID(), request.getUserAgentName(), request.getAppServerJID(), 
				request.getLogLevel(), request.getNID(), ServiceClient.SESSIONSERVICE_SESSION, "StartSessionRequest", new MRMessage(startSessionRequestDoc), 
				request.getResourceGroup(), request.getClientData());		
		StartSessionRequestHandler handler = new StartSessionRequestHandler();					
		Document startSessionResponseDoc = handler.getResponse(startSessionRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);

		// Update the sessionId in the db	
		Element rootEl = objectXMLDoc.getRootElement();
		String NID = MRXMLUtils.getAttributeValue(rootEl, SessionServiceConst.NID, "");
		MRXMLUtils.setAttributeValueXPath(objectXMLDoc, parentElXPath, SessionServiceConst.SESSION_ID, sessionId);		
		log.addInfo("PVRManager::Update sessionId=" + sessionId + " in db", MRLog.OK, MRLog.NOTIFY);
		boolean isSuperUser = request.isSuperUser();
		if (!isSuperUser) {
			request.setSuperUser(true);
		}
		UpdateObjectUtils.updateElementNoValidationCheck(request, xmpp, log, db, rootEl.getName(), NID, objectXMLDoc,  parentElXPath);
		request.setSuperUser(isSuperUser);
		
		return startSessionResponseDoc;
		
	}
	
	public Document stopPersistentPVRSession(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory, Document objectXMLDoc) throws Exception {
		
		String parentElXPath = "//" + SessionServiceResourceConst.PVR_INFO_ELEMENT;
		String sessionId = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, parentElXPath, SessionServiceConst.SESSION_ID, "");
			
		log.addInfo("PVRManager::stopPersistentPVRSession.sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		
		Document stopSessionRequestDoc = MRXMLUtils.stringToDocument("<StopSession/>");
		MRXMLUtils.setAttributeValue(stopSessionRequestDoc.getRootElement(), SessionServiceConst.SESSION_ID, sessionId);		
		
		MRRequest stopSessionRequest = new MRRequest(request.getAppServerJID(), request.getUserAgentName(), request.getAppServerJID(), 
				request.getLogLevel(), request.getNID(), ServiceClient.SESSIONSERVICE_SESSION, "StopSessionRequest", new MRMessage(stopSessionRequestDoc), 
				request.getResourceGroup(), request.getClientData());	
		// StopSessionRequestHandler handler = new StopSessionRequestHandler();
		
		Document stopSessionResponseDoc = null;
		try {
			stopSessionResponseDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).handleStopSession(stopSessionRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		} catch (MRException mrException) {
			if (mrException.getCode() != SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS) {
				throw mrException;
			}
		}
		
		// Remove the sessionId from the objectXML document	
		Element rootEl = objectXMLDoc.getRootElement();
		String NID = MRXMLUtils.getAttributeValue(rootEl, SessionServiceConst.NID, "");
		MRXMLUtils.setAttributeValueXPath(objectXMLDoc, parentElXPath, SessionServiceConst.SESSION_ID, "");
		log.addInfo("PVRManager::Update sessionId=" + sessionId + " in db", MRLog.OK, MRLog.NOTIFY);
		boolean isSuperUser = request.isSuperUser();
		if (!isSuperUser) {
			request.setSuperUser(true);
		}
		UpdateObjectUtils.updateElementNoValidationCheck(request, xmpp, log, db, rootEl.getName(), NID, objectXMLDoc,  parentElXPath);
		request.setSuperUser(isSuperUser);
		
		return stopSessionResponseDoc;
		
	}
}
