package com.ipvs.sessionservice.session.handler;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.policyadminservice.impl.SessionPolicyUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.sessionservice.impl.Session;
import com.ipvs.sessionservice.impl.SessionMediaRoomUtils;
import com.ipvs.sessionservice.impl.SessionResourceUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceExceptionCodes;
import com.ipvs.sessionservice.impl.SessionTrackActionRequestHandler;
import com.ipvs.sessionservice.impl.UpdateSessionUtils;
import com.ipvs.utils.Utils;

public class RecordRequestHandler extends UpdateSessionRequestHandler {	


	//TBD: remove this method when group pvr implementing
	protected void addSourcesAndConnections(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,
			Document startSessionRequestDoc, String sessionId, String connId,
			Element updateElement, String dstEntityId) throws Exception {
	}	

	public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		
		Document responseDoc = this.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory, SessionServiceConst.RECORD_REQUEST);
		
		Element updateElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(), "//" + SessionServiceConst.RECORD_REQUEST);
		String resetTimeline = MRXMLUtils.getAttributeValue(updateElement, SessionServiceConst.RESET_TIMELINE,SessionServiceConst.FALSE);
	
		if (MediaStoreServiceConst.TRUE.equals(resetTimeline)){
			String sessionId = MRXMLUtils.getAttributeValueXpath(responseDoc, "//Session", SessionServiceConst.ID, "");
			String connId = updateElement.attributeValue(SessionServiceConst.ID, null);
		
			resetTimeline(sessionId, connId, updateElement, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
			responseDoc = request.createResponseData(AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDocWithMediaRoomInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId).getRootElement());
		}
		
		return responseDoc;
	}

	// TBD: Move this code to base class when implementing group pvr
	protected Document handleStartNewSession(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document doc, String sessionId, String connId, Element updateElement) throws Exception {
		log.addInfo("UpdateSessionRequestHandler:handleStartNewSession sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		Document sessionTemplateDoc = Session.getSessionTemplate(log);
		sessionTemplateDoc.getRootElement().addAttribute(SessionServiceConst.GC, MRConst.TRUE);
		Document startSessionRequestDoc = MRXMLUtils.stringToDocument("<StartSession/>");
		startSessionRequestDoc.getRootElement().addAttribute(SessionServiceConst.SESSION_ID, sessionId);
		startSessionRequestDoc.getRootElement().add((Element)sessionTemplateDoc.getRootElement().clone());
		startSessionRequestDoc.getRootElement().addAttribute(SessionServiceConst.GC, MRConst.TRUE);

		MRXMLUtils.addAttribute(sessionTemplateDoc.getRootElement(), SessionServiceConst.NAME, MRXMLUtils.getAttributeValue(updateElement, SessionServiceConst.SESSION_NAME,""));

		String connName = MRXMLUtils.getAttributeValue(updateElement, SessionServiceConst.NAME,"");
		
		this.addNewConnections(request, xmpp,  log,db, resourceFactory, serviceAgentFactory,startSessionRequestDoc,updateElement,connId,connName);
		
		updateProfileInfo(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId, updateElement, startSessionRequestDoc);
		
		// Start the session
		MRRequest startSessionRequest = MRRequest.createRequest(request, startSessionRequestDoc, 
				request.getRequestName(), request.getServiceName());
		StartSessionRequestHandler handler = new StartSessionRequestHandler();
		return handler.startSession(startSessionRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}

	// Add new connections 
	protected void addNewConnections(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document startSessionRequestDoc, Element updateElement, String connId,String connName) throws MRException ,Exception {
		Element srcPort = MRXMLUtils.getElement(updateElement, SessionServiceConst.SRC_PORT);
		Element dstFile = MRXMLUtils.getElement(updateElement, SessionServiceConst.DST_FILE);
		Element srcGroup = MRXMLUtils.getElement(updateElement, SessionServiceConst.SRC_GROUP);
		Element dstGrpFile = MRXMLUtils.getElement(updateElement, SessionServiceConst.DST_GRP_FILE);
		if (srcPort != null && dstFile!=null) {
			handleClipRecording( request,  xmpp,  log, db,  resourceFactory, serviceAgentFactory,srcPort,dstFile,startSessionRequestDoc,connId,connName);
		} else if (srcGroup != null && dstGrpFile != null) {
			handleGroupRecording(request,  xmpp,  log, db,  resourceFactory, serviceAgentFactory, srcGroup,dstGrpFile,startSessionRequestDoc,connId,connName);
		} else {
			throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Invalid Input");
		}
	}


	//handle the clip recording. Add src,dest entity and connection
	private void handleClipRecording(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element srcPort,
			Element dstFile, Document startSessionRequestDoc, String connId,String connName) throws MRException,Exception{

		String srcPortEntityId = addSrcPortEntity(request,  xmpp,  log, db,  resourceFactory, serviceAgentFactory,srcPort,startSessionRequestDoc);
		String dstFileEntityId = addDstFileEntity(request,  xmpp,  log, db,  resourceFactory, serviceAgentFactory,dstFile,startSessionRequestDoc,"","");
		addConnection(request,  xmpp,  log, db,  resourceFactory, serviceAgentFactory,srcPortEntityId,dstFileEntityId,startSessionRequestDoc,connId,connName);
	}


	// add connection to session doc
	private void addConnection(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String srcPortEntityId, String dstFileEntityId, Document startSessionRequestDoc,
			String connId, String connName) throws MRException,Exception {
		Element connection = this.createConnection(connId, dstFileEntityId, srcPortEntityId, connName, startSessionRequestDoc, "", "");
		Element connectionList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.CONNECTIONLIST);
		connectionList.add(connection);		
	}

	// add source entity to session doc
	private String addSrcPortEntity(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element srcPort, Document startSessionRequestDoc) throws MRException,Exception  {
		Element entity = UpdateSessionUtils.createPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				srcPort,  SessionServiceConst.SRC_PORT);
		String srcEntityId = entity.attributeValue(SessionServiceConst.ID);
		Element entityList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.ENTITYLIST);
		entityList.add(entity);
		return srcEntityId;		
	}

	// add dest file entity to session doc
	private String addDstFileEntity(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element dstFileElement, Document startSessionRequestDoc,String grpFileId,String clipId)  throws MRException,Exception {			

		String dirNID = MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.DIR_NID,"");
		String dirID = MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.DIRID,"");

		if(!dirNID.isEmpty()) {
			Element dirEL = UpdateSessionUtils.createDirElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirNID,dirID);
			Element dirListEL = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//"+SessionServiceConst.DIRLIST);
			MRXMLUtils.addElement(dirListEL, dirEL);
			dirID = MRXMLUtils.getAttributeValue(dirEL, SessionServiceConst.ID,"");
		}

		Element entity = createDstFileEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstFileElement, grpFileId, clipId, dirID);

		Element entityList = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//" + SessionServiceConst.ENTITYLIST);
		entityList.add((Element)entity.clone());

		return MRXMLUtils.getAttributeValue(entity, SessionServiceConst.ID,"");
	}

	private Element createDstFileEntity(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element dstFileElement, String grpFileId,String clipId,String dirID)  throws MRException,Exception {	

		String id = MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.ID,"");
		String fileTitle = MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.FILETITLE,"");		
		String keyAttribute = MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.KEY,"");
		String action = MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.ACTION,"");
		String keyMetaDataTagName = MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.KEY_METADATA_TAG_NAME,"");
		Element metadataEL = MRXMLUtils.getElement(dstFileElement, MediaStoreServiceConst.META_DATA_ELEMENT_NAME);

		if(id.isEmpty()) { 
			id = Utils.getUUID();
		}

		Element entity = MRXMLUtils.stringToElement("<" + SessionServiceConst.ENTITY + "/>");
		MRXMLUtils.addAttribute(entity, SessionServiceConst.GC, MRConst.TRUE);
		MRXMLUtils.addAttribute(entity, SessionServiceConst.ID, id);
		MRXMLUtils.addAttribute(entity, SessionServiceConst.TYPE_ATTRIBUTE, SessionServiceConst.RECFILE);
		MRXMLUtils.addAttribute(entity, SessionServiceConst.NAME, fileTitle);

		Element fileElement = MRXMLUtils.stringToElement("<" + SessionServiceConst.FILE + "/>");
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.RESOURCETITLE, fileTitle);
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.GRPFILEID,grpFileId);
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.KEY, keyAttribute);		
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.ACTION, action);
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.KEY_METADATA_TAG_NAME, keyMetaDataTagName);
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.DIRID, dirID);
		MRXMLUtils.addAttribute(fileElement, SessionServiceConst.CLIPID, clipId);
		if(metadataEL != null )
			MRXMLUtils.addElement(fileElement,(Element)metadataEL.clone());

		MRXMLUtils.addElement(entity,fileElement);	

		return entity;
	}
	
	/*
	private String createGroupFile(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element grpFileElement,Element portGroupElement, Document startSessionRequestDoc) throws MRException, Exception {
		String groupFileTitle = MRXMLUtils.getAttributeValue(grpFileElement, SessionServiceConst.FILETITLE,"");
		String groupFileId = MRXMLUtils.getAttributeValue(grpFileElement, SessionServiceConst.ID,"");
		if (groupFileId.isEmpty()) {
			groupFileId = Utils.getUUID();
		}
		Element sessionGrpFileEL = MRXMLUtils.newElement(SessionServiceConst.GRPFILE);
		MRXMLUtils.addAttribute(sessionGrpFileEL, SessionServiceConst.RESOURCETITLE, groupFileTitle);
		MRXMLUtils.addAttribute(sessionGrpFileEL, SessionServiceConst.ID, groupFileId);
		MRXMLUtils.addAttribute(sessionGrpFileEL, SessionServiceConst.TYPE, SessionServiceConst.SYNCREC);
		Element sessionGrpFileListEL = MRXMLUtils.getElementXpath(startSessionRequestDoc, "//"+SessionServiceConst.GRPFILELIST);
		MRXMLUtils.addElement(sessionGrpFileListEL, sessionGrpFileEL);
		return groupFileId;
	}*/

	//TBD: handle group recording
	private void handleGroupRecording(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element srcGroup, Element dstGrpFile, 
			Document startSessionRequestDoc, String connId,String connName) throws MRException,Exception {
		//TBD
	}



	// add source entity to live session.
	protected String addSrcEntityToLiveSession(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,String sessionId, String roomNID) throws Exception {
		Element entity = UpdateSessionUtils.createPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId, SessionServiceConst.SRC_PORT);		
		log.addInfo("RecordRequestHandler:addSrcEntity sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		String resourceElementName = SessionServiceConst.PORT;
		UpdateSessionUtils.addSrcResourceToLiveSession( request,  xmpp,  log, db,  resourceFactory,serviceAgentFactory, entity,  sessionId,  roomNID,resourceElementName);
		return MRXMLUtils.getAttributeValue(entity, SessionServiceConst.ID,"");	
	}

	//add destination entity to live session
	protected String addDstEntityToLiveSession(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc, String sessionId, String roomNID,String srcEntityId) throws Exception {
		log.addInfo("RecordRequestHandler:addDstEntity sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		Element dstFileElement = MRXMLUtils.getElementXpath(requestDoc,"//"+ SessionServiceConst.DST_FILE);
		Element dstGrpFileElement = MRXMLUtils.getElement(requestDoc, "//"+SessionServiceConst.DST_GRP_FILE);
		String dstEntityId ="";
		if(dstFileElement != null) {
			dstEntityId = addClipRecordingToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstFileElement,requestDoc, sessionId, roomNID,srcEntityId);
		} else if(dstGrpFileElement != null) {
			addGroupRecordingToLiveSession(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstFileElement,requestDoc, sessionId, roomNID,srcEntityId);
		} else {
			throw new MRException(SessionServiceExceptionCodes.INVALID_INPUT, "Invalid Input");
		}

		return dstEntityId;		
	}


	private void addGroupRecordingToLiveSession(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element dstFileElement, Document requestDoc,String sessionId,String roomNID,String srcEntityId) throws MRException,Exception {
		// TBD

	}

	// Add clip recording to live session
	private String addClipRecordingToLiveSession(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element dstFileElement, Document requestDoc,String sessionId,String roomNID,String srcEntityId) throws MRException,Exception {	

		//update dir information if present
		String dirID = getDirID(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstFileElement,  sessionId, roomNID, srcEntityId);

		//get session document
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);

		String sessionConfigNID = MRXMLUtils.getAttributeValueXpath(sessionDoc,"//Properties",SessionServiceConst.SESSION_CONFIG_NID_CONST,"");
		String requestedDirNID = "";
		if(!dirID.isEmpty()) {
			requestedDirNID = MRXMLUtils.getAttributeValueXpath(sessionDoc, "//Dir[@id='"+dirID+"']", SessionServiceConst.RESOURCENID, "");
		}
		Element groups = MRXMLUtils.getElementXpath(sessionDoc, "//Groups");

		//create destination file entity
		Element fileEntityEL = createDstFileEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstFileElement, "", Utils.getUUID() , dirID);	
		String dstEntityId = MRXMLUtils.getAttributeValue(fileEntityEL, SessionServiceConst.ID,"");

		//temporary ADD connection to the session document to go through policy check
		addConnection(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcEntityId, dstEntityId, sessionDoc, Utils.getUUID(), "");


		//create file resource
		Element fileElement = MRXMLUtils.getElement(fileEntityEL, SessionServiceConst.FILE);
		
		String recordAccessXpath = SessionPolicyUtils.getDirAccessXPath(request, xmpp, log, db, resourceFactory, serviceAgentFactory, PolicyAdminConstants.RECORD_ROLE_ACTION);			
		
		if(recordAccessXpath.equals(PolicyAdminConstants.NULL)){
			throw new MRException(SessionServiceExceptionCodes.DIR_ACCESS_POLICY_DENY, "Dir Access Policy Deny");
		}
		
		SessionResourceUtils.createStreamFileResource(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, fileElement, requestedDirNID, xmpp.getCurrentTimeMillis(), 
				SessionServiceConst.RECFILE, groups, sessionId, sessionConfigNID, recordAccessXpath);

		//update file resource
		SessionResourceUtils.updateFileElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fileElement);

		//add file entity in session
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).addEntity(roomNID, fileEntityEL);

		//create media destination
		sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		SessionMediaRoomUtils.createMediaDest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dstEntityId, sessionDoc, roomNID);

		return dstEntityId;
	}

	private String getDirID(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element dstFileElement, String sessionId,String roomNID,String srcEntityId) throws MRException,Exception {

		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		String dirNID = MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.DIR_NID,"");
		String dirID= MRXMLUtils.getAttributeValue(dstFileElement, SessionServiceConst.DIRID,"");

		if (dirID.isEmpty() && dirNID.isEmpty()) {
			return "";
		}

		//check dir already there in session or not. if not then add to the session
		Element dirEl = MRXMLUtils.getElementXpath(sessionDoc, "//Dir[@id='"+dirID+"' or @resourceNID='"+dirNID+"']");
		if(dirEl == null && dirNID.isEmpty()) {
			return "";			
		} else if(dirEl ==null && (!dirID.isEmpty() && dirNID.isEmpty())) {	
			throw new MRException(SessionServiceExceptionCodes.DESTDIR_NOT_FOUND, "Directory not found in session");
		} else if(dirEl == null && !dirNID.isEmpty()) {
			dirEl = UpdateSessionUtils.createDirElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, dirNID,dirID);
			AppServerAgentUtils.getSessionManager(serviceAgentFactory).addDir(roomNID, dirEl);
		}
		
		dirID = MRXMLUtils.getAttributeValue(dirEl, SessionServiceConst.ID,"");
		return dirID;
	}
	
	protected void resetTimeline(String sessionId,String connId,Element updateEl,MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws MRException,Exception {
		try {
		Element resumeRecordElement = MRXMLUtils.newElement(SessionServiceConst.RESUMERECORD);
		MRXMLUtils.setAttributeValue(resumeRecordElement, SessionServiceConst.SESSION_ID, sessionId);
		log.addInfo("Reset Record Timeline ",MRLog.INFO,MRLog.NOTIFY);	
		MRRequest recordRequest =MRRequest.createRequest(request, MRXMLUtils.elementToDocument(resumeRecordElement), SessionServiceConst.RESUMERECORD,request.getServiceName());
		new SessionTrackActionRequestHandler().getTrackResponseInternal(recordRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				SessionServiceConst.RESUMERECORD, MediaRoomServiceConst.TRACK_CATCHUP);
		} catch(MRException exp) {			
			//ignore exception
			//exp.printStackTrace();
		}
	}

}
