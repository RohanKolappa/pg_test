package com.ipvs.sessionservice.impl;

import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.session.handler.AutoStartRequestHandler;

public class AutoStartManager {
    public enum Action {CONTINUE_AUTOSTART_SESSION, RESTART_AUTOSTART_SESSION, STOP_AUTOSTART_SESSION};
	/*
	 * This method is responsible for starting / restarting/ stopping an auto start session depending upon the action provided
	 */
    public void handleAutoStartConfigUpdate(MRRequest request, XMPPI xmpp, MRLog log,
	    MRDB db, MRResourceFactoryI resourceFactory,
	    ServiceAgentFactoryI serviceAgentFactory,String portNID, Action action, String ipAddress) throws Exception{
	String parentElXPath = "//" + SessionServiceResourceConst.INSTANCE_LIST+"/Instance";
	Document srcPortDoc = MRXMLDBUtils.readElement(request, log, db,
		portNID);

	String sessionId = MRXMLUtils.getAttributeValueXpath(srcPortDoc, parentElXPath, "sessionId", "");
	if(!sessionId.isEmpty()){
	    Semaphore lock = IPVSMonitor.getMonitor().getLock(sessionId, request.getNID());
	    try{		    
		switch (action){
		case CONTINUE_AUTOSTART_SESSION:		    					    	
		    //if session exists return otherwise create session
		    try {
			SessionManager sessionManager = AppServerAgentUtils.getSessionManager(serviceAgentFactory);
			Document sessionDoc = sessionManager.getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
			String roomNID = sessionManager.getRoomNID(sessionId);
			if(roomNID != null) {
			    //resend start request if session already present
			    AutoStartRequestHandler requestHandler = new AutoStartRequestHandler();
			    Document roomdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, roomNID);		

			    String srcEntityId= MRXMLUtils.getAttributeValueXpath(sessionDoc, "//Entity[@type='SrcPort']", SessionServiceConst.ID, "");
			    Element mediaSourceEl = MRXMLUtils.getElementXpath(roomdoc, "//MediaSource[@id='" + srcEntityId + "']");
			    String connId = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//Connection[@type='VIEW']", SessionServiceConst.ID, "");
			    Element streamElement = MRXMLUtils.getElementXpath(roomdoc, "//Stream[@id='"+connId+"']");							
			    String currentIPAddress = MRXMLUtils.getValueXpath(streamElement, ".//publishedSourceIP", "");
			    //check if the IP address of source is same
			    if (ipAddress.equals(currentIPAddress) || ipAddress.equals("")){
				String streamNID = MRXMLUtils.getAttributeValue(streamElement, SessionServiceConst.NID);
				log.addInfo("AutoStartManager: Found session running Resend StartRequest. sourceNID:"+MRXMLUtils.getAttributeValue(srcPortDoc.getRootElement(), SessionServiceConst.NID,"")+" sessionID:="+sessionId
					+" roomNID:="+roomNID+" StreamNID:="+streamNID+": currentIPAddress:"+currentIPAddress,MRLog.INFO,MRLog.NOTIFY);

				requestHandler.sendStartRequest(xmpp, request, db, log, serviceAgentFactory, resourceFactory, roomNID,streamNID, 
					MRXMLUtils.elementToDocument(streamElement), MRXMLUtils.elementToDocument(mediaSourceEl), MediaRoomServiceUtils.getAgentJID(request, srcPortDoc));


				return;
			    }
			}
		    } catch (Exception e) {
			e.printStackTrace();
		    }
		    //if IP address change restart the session
		    stopPersistentAutoStartSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcPortDoc);		    								

		    break;
		case STOP_AUTOSTART_SESSION:
		    stopPersistentAutoStartSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcPortDoc);
		    return;
		case RESTART_AUTOSTART_SESSION:
		    stopPersistentAutoStartSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcPortDoc);
		    break;
		default:
		    return;
		}
	    }catch(Exception e){}
	    finally{
		IPVSMonitor.getMonitor().releaseLock(sessionId, lock);
	    }
	} else {
	    switch (action){	    	
	    case STOP_AUTOSTART_SESSION:	
		return;
	    default: 
	    }
	}
	if(!action.equals(Action.STOP_AUTOSTART_SESSION)){

	    String portTitle = MRXMLUtils.getValueXpath(srcPortDoc,"//Info/Title","");	
	    sessionId = portTitle + "." + SessionServiceConst.MODE_AUTO_START + SessionServiceConst.SESSION;
	    Semaphore lock = IPVSMonitor.getMonitor().getLock(sessionId, request.getNID());
	    try{
		String requestName = MRRequest.getRequestName(AutoStartRequestHandler.class.getName());
		Document autoStartStreamSampleRequestDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.AUTO_START_STREAM_REQUEST_XML, log);
		Element autoStartStreamEL = MRXMLUtils.getElementXpath(autoStartStreamSampleRequestDoc, "//AutoStart");
		MRXMLUtils.setAttributeValue(autoStartStreamEL, SessionServiceResourceConst.PORT_NID, portNID);		

		Document autoStartStreamRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, autoStartStreamEL);
		MRRequest autoStartStreamRequest = MRRequest.createRequest(request, autoStartStreamRequestDoc, requestName, ServiceClient.SESSIONSERVICE_SESSION);

		AutoStartRequestHandler autoStartStreamRequestHandler = new AutoStartRequestHandler();
		autoStartStreamRequestHandler.getResponse(autoStartStreamRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
	    }
	    catch(Exception e){
		Document portDoc = MRXMLDBUtils.readElement(request, log, db,
			portNID);
		sessionId = MRXMLUtils.getAttributeValueXpath(portDoc, parentElXPath, "sessionId", "");
		if(!sessionId.isEmpty()){
		    stopPersistentAutoStartSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portDoc);
		}
		throw e;
	    }
	    finally{
		IPVSMonitor.getMonitor().releaseLock(sessionId, lock);
	    }
	}
    }
	
	/*
	 * This method is responsible for stopping an auto start session
	 */
    public Document stopPersistentAutoStartSession(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
	    ServiceAgentFactoryI serviceAgentFactory, Document objectXMLDoc) throws Exception {

	String parentElXPath = "//" + SessionServiceResourceConst.INSTANCE_LIST+"/Instance";
	String sessionId = MRXMLUtils.getAttributeValueXpath(objectXMLDoc, parentElXPath, SessionServiceConst.SESSION_ID, "");		

	log.addInfo("AutoStartManager::stopAutoStartSession.sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);

	Document stopSessionRequestDoc = MRXMLUtils.stringToDocument("<StopSession/>");
	MRXMLUtils.setAttributeValue(stopSessionRequestDoc.getRootElement(), SessionServiceConst.SESSION_ID, sessionId);		

	MRRequest stopSessionRequest = new MRRequest(request.getAppServerJID(), request.getUserAgentName(), request.getAppServerJID(), 
		request.getLogLevel(), request.getNID(), ServiceClient.SESSIONSERVICE_SESSION, "StopSessionRequest", new MRMessage(stopSessionRequestDoc), 
		request.getResourceGroup(), request.getClientData());		

	Document stopSessionResponseDoc = null;
	try {
	    stopSessionResponseDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).stopSession(stopSessionRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
	} catch (MRException mrException) {
	    if (mrException.getCode() != SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS) {
		throw mrException;
	    }
	}

	// Remove the sessionId from the objectXML document	
	Element rootEl = objectXMLDoc.getRootElement();
	String NID = MRXMLUtils.getAttributeValue(rootEl, SessionServiceConst.NID, "");
	MRXMLUtils.setAttributeValueXPath(objectXMLDoc, parentElXPath, SessionServiceConst.SESSION_ID, "");		
	MRXMLUtils.setValueXpath(objectXMLDoc, parentElXPath+"/MediaStreamInfo/"+SessionServiceConst.MEDIA_STREAM_URL, "");		
	log.addInfo("AutoStartManager::clean session id and media stream url in db", MRLog.OK, MRLog.NOTIFY);

	UpdateObjectUtils.updateElement(request, xmpp, log, db, rootEl.getName(), NID, objectXMLDoc,  parentElXPath);

	return stopSessionResponseDoc;

    }
}
