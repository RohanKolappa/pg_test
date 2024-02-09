package com.ipvs.sessionservice.session.handler;

import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.impl.SessionAuditEventUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.UpdateSessionUtils;
import com.ipvs.utils.Utils;

public class PlaybackRequestHandler extends UpdateSessionRequestHandler{

	
	
	private void addPlyFileSourcesAndConnections(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,
			Document startSessionRequestDoc, String sessionId, String connId, Element viewBaseElement, String dstEntityId) throws Exception {
		Element fileElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.SRCFILE);
		Element entity = UpdateSessionUtils.createPlyFileEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fileElement, sessionId);
		String srcEntityId = entity.attributeValue(SessionServiceConst.ID);
		Element entityList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.ENTITYLIST);
		entityList.add((Element)entity.clone());
		
		String connName = viewBaseElement.attributeValue(SessionServiceConst.NAME, "");
	    String canvasId = viewBaseElement.attributeValue(SessionServiceConst.CANVASID, "");
	    String windowId = viewBaseElement.attributeValue(SessionServiceConst.WINDOWID, "");
	
		Element connection = this.createConnection(connId, dstEntityId, srcEntityId, connName, startSessionRequestDoc, canvasId, windowId);
		Element connectionList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.CONNECTIONLIST);
		connectionList.add((Element)connection.clone());	
	}
	
	

	private void addGrpFileSourcesAndConnections(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,
			Document startSessionRequestDoc, String sessionId, String connId, Element viewBaseElement, String dstEntityId,
			String grpFileNID) throws Exception {

	    Element grpFile = UpdateSessionUtils.getFile(request, xmpp, log, db, resourceFactory, serviceAgentFactory, request.getMessage().getDocument(), grpFileNID);
	    Element sessionGrpFileElement = UpdateSessionUtils.createGrpFileElemet(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  grpFile);
		Element grpFileListElement = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.GRPFILELIST);
	    grpFileListElement.add(sessionGrpFileElement);
	    
	    // Create all the srcEntities
	    String connName = viewBaseElement.attributeValue(SessionServiceConst.NAME, "");
	    String canvasId = viewBaseElement.attributeValue(SessionServiceConst.CANVASID, "");	    
	    String windowId = viewBaseElement.attributeValue(SessionServiceConst.WINDOWID, "");
	    
    	int i = 0;
    	List<?> fileList = MRXMLUtils.getListXpath(grpFile, ".//FileList/File");
        for(Object o : fileList) {	    	
	    	// Create file entity
			Element entity = UpdateSessionUtils.createPlyFileEntityElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, startSessionRequestDoc, (Element)o, sessionId);
			String srcEntityId = entity.attributeValue(SessionServiceConst.ID);
			Element entityList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.ENTITYLIST);
			entityList.add((Element)entity.clone());
					
	
			String fileConnId = StringUtils.isEmpty(connId) ? Utils.getUUID() : String.valueOf(i) + "_" + connId;
			String fileWindowId = StringUtils.isEmpty(windowId) ? "" : String.valueOf(i) + "_" + windowId;
			String fileConnName = StringUtils.isEmpty(connName) ? "" : String.valueOf(i) + "_" + connName;
			
		    // Create connection
		    Element connection = this.createConnection(
		    		fileConnId, 
		    		dstEntityId, srcEntityId, 
		    		fileConnName, startSessionRequestDoc, canvasId, fileWindowId);
			Element connectionList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.CONNECTIONLIST);
			connectionList.add((Element)connection.clone());
			i++;
	    } 	    
	}

	protected void addSourcesAndConnections(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,
			Document startSessionRequestDoc, String sessionId, String connId, Element viewBaseElement, String dstEntityId) throws Exception {
		/*
		<GrpFile id='grpId1' fileNID='' fileTitle='' />
		<SrcFile id="" fileNID="" fileTitle=""  grpFileId="grpId1"/>
		*/
		
		// System.out.println("PlaybackRequestHandler doc=" + MRXMLUtils.documentToString(requestDoc));
		String plyFileNID = null;
		String grpFileNID = null;
		Element plyFileElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.SRCFILE);
		if(plyFileElement != null)
			plyFileNID = plyFileElement.attributeValue(SessionServiceConst.FILENID);
		
		Element grpFileElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.GRPFILE);
		
		String logType = AuditEventLogger.NAME_FILE;
		if(grpFileElement != null) {
			grpFileNID = grpFileElement.attributeValue(SessionServiceConst.FILENID);
			logType = AuditEventLogger.NAME_GROUP;
		}
		
		Document loggerDocument = request.getLoggingDocument();
		if(loggerDocument ==null) {
			request.setLoggingDocument(MRXMLUtils.stringToDocument("<Log type='"+logType+"' connId='"+connId+"' grpFileNID='"+grpFileNID+"' plyFileNID='"+plyFileNID+"'/>"));		
		} else {
			MRXMLUtils.setAttributeValue(loggerDocument.getRootElement(), "type", logType);
			MRXMLUtils.setAttributeValue(loggerDocument.getRootElement(), "grpFileNID", grpFileNID);
			MRXMLUtils.setAttributeValue(loggerDocument.getRootElement(), "plyFileNID", plyFileNID);
		}
		
		if( (plyFileNID != null) && (!"".equals(plyFileNID)) ) {
			this.addPlyFileSourcesAndConnections(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					requestDoc, startSessionRequestDoc, sessionId, connId, viewBaseElement, dstEntityId);
		} else if( (grpFileNID != null) && (!"".equals(grpFileNID)) ) {	
			this.addGrpFileSourcesAndConnections(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					requestDoc, startSessionRequestDoc, sessionId, connId, viewBaseElement, dstEntityId, grpFileNID);
		}
		else
			throw new MRException(SessionServiceExceptionCodes.SOURCE_RESOURCE_NOT_FOUND, "SrcFile not found");
		
		
	}


	protected String addSrcEntityToLiveSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc, String sessionId, String roomNID) throws Exception {
		return UpdateSessionUtils.addSrcFileToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, roomNID);
	}

	protected boolean addingNewSources(Document requestDoc) throws Exception {
		// Check if fileNIDs are specified, if so then this has to be a new session
		String plyFileNID = null;
		String grpFileNID = null;
		Element plyFileElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.SRCFILE);
		if(plyFileElement != null)
			plyFileNID = plyFileElement.attributeValue(SessionServiceConst.FILENID);		
		Element grpFileElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.GRPFILE);
		if(grpFileElement != null)
			grpFileNID = grpFileElement.attributeValue(SessionServiceConst.FILENID);
		
		return ( (plyFileNID != null) && (!"".equals(plyFileNID)) ||
			     (grpFileNID != null) && (!"".equals(grpFileNID)) );
	}
	
	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

		return this.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, "Playback");
	}
	
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, String status, String level, String errorDescription) throws Exception {

		Document loggerDocument = request.getLoggingDocument();
		String type = MRXMLUtils.getAttributeValue(loggerDocument.getRootElement(), SessionServiceConst.TYPE_ATTRIBUTE,"");

		if(request.getException()!=null) {	
			return new AuditEvent.Builder(AuditEventLogger.PLAYBACK_EVENT,type, request.getUserJID(), level).status(status).
			descriptionText(errorDescription).operatorEl(request.getOperatorGroups()).build();
		}
		
		
		String connId = MRXMLUtils.getAttributeValue(loggerDocument.getRootElement(), SessionServiceConst.CONN_ID, "");
		Element connectionElement = MRXMLUtils.getElementXpath(request.getResult(), "//Connection[@id='"+connId+"']");
		
		//log group event if user playing back a group. Otherwise log connection/source based event
		if(type.equals(AuditEventLogger.NAME_GROUP)) {
			String grpFileNID = MRXMLUtils.getAttributeValue(loggerDocument.getRootElement(), "grpFileNID","");
			return SessionAuditEventUtils.getGroupBasedAuditEvent(request.getResult(), connectionElement, grpFileNID, request, log);
		} else {
			String playFileNID = MRXMLUtils.getAttributeValue(loggerDocument.getRootElement(), "plyFileNID","");
			if(playFileNID.isEmpty()) {
				return SessionAuditEventUtils.getSourceBasedAuditEventUsingConnectionElement(request.getResult(), connectionElement,request, log);
			}
			return SessionAuditEventUtils.getSourceBasedAuditEvent(request.getResult(), connectionElement,playFileNID,request, log);

			}
	}
}
