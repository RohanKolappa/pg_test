package com.ipvs.sessionservice.impl;

import java.util.ArrayList;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.sessionservice.resource.handler.GetFileRequestHandler;
import com.ipvs.utils.Utils;

public class UpdateSessionUtils {
	public static Document  addSrcResourceToLiveSession(MRRequest request, XMPPI xmpp,
			MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element entity,
			String sessionId, String roomNID, String resourceElementName)  throws MRException,Exception {
		
		Document sessionTemplateDoc = Session.getSessionTemplate(log);
		Element tmpSrcSession = sessionTemplateDoc.getRootElement();
		String tmpSessionId = Utils.getUUID();
		tmpSrcSession.addAttribute(SessionServiceConst.ID, tmpSessionId);
		Element entityListElement = tmpSrcSession.element("EntityList");
		entityListElement.add(entity);		
		
		SessionResourceUtils.updateSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionTemplateDoc, tmpSessionId, "");
	
			// Check that we have a valid source
			Element resourceElement = entity.element(resourceElementName);
			String resourceNID = resourceElement.attributeValue(SessionServiceConst.RESOURCENID);
			if((resourceNID == null) || ("".equals(resourceNID)) )
				throw new MRException(SessionServiceExceptionCodes.SOURCE_RESOURCE_NOT_FOUND, 
						"SrcPort not found portTitle=" + resourceElement.attributeValue(SessionServiceConst.RESOURCETITLE));
			// Update the current session using the data in the tmpSession
			
			// Add the entities
			List<?> entityList = MRXMLUtils.getListXpath(sessionTemplateDoc, "//" + SessionServiceConst.ENTITY);
			for(Object o: entityList) {
				Element entityItem = (Element)o;
				updateEntityNameAndType(request, log, db, entityItem, SessionServiceConst.PORT);
				AppServerAgentUtils.getSessionManager(serviceAgentFactory).addEntity(roomNID, entityItem);
			}
			
			// Add the connections
			List<?> connectionList = MRXMLUtils.getListXpath(sessionTemplateDoc, "//" + SessionServiceConst.CONNECTION);
			for(Object o: connectionList) {
				Element connectionItem = (Element)o;
				AppServerAgentUtils.getSessionManager(serviceAgentFactory).addConnection(roomNID, connectionItem);
			}

			// Create the sources and destinations
			Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);	
			SessionMediaRoomUtils.createSourcesAndDestinations(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, roomNID, entityList, false);
	
			// Start the connections
			SessionMediaRoomUtils.startConnectionList(request, xmpp, log, db, resourceFactory, serviceAgentFactory, connectionList, sessionId);

			return sessionTemplateDoc;
		
	}

	public static void updateEntityNameAndType(MRRequest request, MRLog log, MRDB db,
			Element entity, String resourceElementName) throws Exception {
		String streamType = MRXMLUtils.getAttributeValueXpath(entity, "@"+SessionServiceConst.STREAM_TYPE, "") ;
		String title = MRXMLUtils.getAttributeValueXpath(entity, "@"+SessionServiceConst.NAME, "") ;
		if (!"".equals(title) && !"".equals(streamType)){
			return;
		}
		String resourceNID = MRXMLUtils.getAttributeValueXpath(entity, ".//"+resourceElementName, SessionServiceConst.RESOURCENID, "");
		if("".equals(resourceNID))
			return;
		Document resourceDoc = MRXMLDBUtils.readElement(request, log, db, resourceNID);
		title = MRXMLUtils.getValueXpath(resourceDoc, "//"+SessionServiceConst.SESSION_CONFIG_TITLE, "");
		List<?> li = MRXMLUtils.getValuesXpath(resourceDoc, "//"+MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME);
		if (li.size() > 1){
			streamType = SessionServiceConst.MULTI;
		} else {
			streamType = (!li.isEmpty())?li.get(0).toString(): "";
		}
		entity.addAttribute(SessionServiceConst.NAME,title);
	    entity.addAttribute(SessionServiceConst.STREAM_TYPE,streamType);
	}
	
	//moved this method from view request handler. We will reuse these methods in invite request
	public static String addSrcPortToLiveSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc, String sessionId, String roomNID) throws Exception {
		
		Element entity = createSrcPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, sessionId);
        log.addInfo("ViewRequestHandler:addSrcEntity sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);   

		Document sessionTemplateDoc = null;
		
		try {
			String resourceElementName = SessionServiceConst.PORT;
			sessionTemplateDoc = addSrcResourceToLiveSession( request,  xmpp,  log, db,  resourceFactory,serviceAgentFactory, entity,  sessionId,  roomNID,resourceElementName);
			return MRXMLUtils.getAttributeValue(entity, SessionServiceConst.ID,"");			
		}
		catch(Exception e) {// Problem to be handled is that the PVR file is now created and has to be cleaned up if there is a problem
			if (sessionTemplateDoc != null) {
				List<?> pvrFileList = MRXMLUtils.getListXpath(sessionTemplateDoc, "//" + SessionServiceConst.FILE);
				for(Object o: pvrFileList) {
					Element pvrFileItem = (Element)o;
					String pvrFileNID = pvrFileItem.attributeValue(SessionServiceConst.RESOURCENID);
					SessionMediaRoomUtils.gcPVRFile(request, xmpp, log, db, resourceFactory, serviceAgentFactory, pvrFileNID);
				}
			}
			throw e;
		}
	}
	
	// Create connection element in session
	public static Element createConnection(String connId, String dstEntityId, String srcEntityId,
			String connName, Document sessionDoc, String canvasId, String windowId,String profileId) throws Exception {
		// Add the connection
		Element connection = MRXMLUtils.stringToElement(
                "<Connection dstEntityId='' id='' srcEntityId='' type='' name=''/>");
        connection.attribute(SessionServiceConst.ID).setValue(connId);
        connection.attribute(SessionServiceConst.DSTENTITYID).setValue(dstEntityId);
        connection.attribute(SessionServiceConst.SRCENTITYID).setValue(srcEntityId);
       
        if(connName.isEmpty()){
        	connName ="Connection_"+connId;
        }
          
        connection.attribute(SessionServiceConst.NAME).setValue(connName);	    
    	
    	String connectionType = SessionResourceUtils.getConnectionType(connection, dstEntityId, srcEntityId, sessionDoc);
		MRXMLUtils.setAttributeValue(connection, SessionServiceConst.TYPE_ATTRIBUTE, connectionType);
		MRXMLUtils.setAttributeValue(connection, SessionServiceConst.CANVASID, canvasId);
		MRXMLUtils.setAttributeValue(connection, SessionServiceConst.WINDOWID, windowId);
		MRXMLUtils.setAttributeValue(connection, SessionServiceConst.PROFILE_ID, profileId);
		Element entityEL = MRXMLUtils.getElementXpath(sessionDoc, "//Entity[@id='"+srcEntityId+"']");
		MRXMLUtils.setAttributeValue(connection, SessionServiceConst.STREAM_TYPE, MRXMLUtils.getAttributeValue(entityEL, SessionServiceConst.STREAM_TYPE, ""));
		
		return connection;
	}
	
	// create dir element in session
	public static Element createDirElement(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, String dirNID,String dirID) throws MRException,Exception {
		if(dirID.isEmpty()) {
		   dirID = Utils.getUUID();
		}
		Element dirEL = MRXMLUtils.newElement(SessionServiceConst.DIR);
		MRXMLUtils.addAttribute(dirEL, SessionServiceConst.RESOURCENID,dirNID);
		MRXMLUtils.addAttribute(dirEL, SessionServiceConst.ID, dirID);
		return dirEL;
	}

	// Create source port entity in session
	public static Element createSrcPortEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc, String sessionId) throws Exception {
		String resourceType = SessionServiceConst.SRC_PORT;
		Element entity = createPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				requestDoc, sessionId, resourceType);
		Element portElement = MRXMLUtils.getElementXpath(requestDoc, "//" + resourceType);
		String pvrEnable = portElement.attributeValue(SessionServiceConst.PVRENABLE);
		String trimLength = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.TRIMLENGTH, "");
		String dirId = MRXMLUtils.getAttributeValue(portElement, SessionServiceConst.DIRID, "");
		if(trimLength.isEmpty())
			trimLength = MediaRoomServiceConst.DEFAULT_TRIMLENGTH;
		entity.add(MRXMLUtils.stringToElement("<PVR enable='" + pvrEnable + "' trimLength='"+ trimLength + "' dirId='" + dirId + "'/>"));		
		return entity;
	}

	public static Element createPortEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Element  portElement , String resourceType) throws MRException, Exception {
		String portNID = portElement.attributeValue("portNID");
		String portTitle = portElement.attributeValue("portTitle");
		if(        ( (portNID == null) || ("".equals(portNID)) )
				&& ( (portTitle == null) || ("".equals(portTitle)) ) )
 			throw new MRException(SessionServiceExceptionCodes.OBJECT_NOT_FOUND, 
					"Resource not found resourceType=" + resourceType);
	
		String id = portElement.attributeValue(SessionServiceConst.ID);
		if(id == null || "".equals(id)) // Generate an id if not present
			id = Utils.getUUID();
		
	    Element entity = MRXMLUtils.stringToElement("<" + SessionServiceConst.ENTITY + "/>");
	    // In the mode that is in use the GC is always false, however this can change
        entity.addAttribute(SessionServiceConst.GC,MRConst.FALSE);
        entity.addAttribute(SessionServiceConst.ID,id);
        entity.addAttribute(SessionServiceConst.TYPE_ATTRIBUTE,resourceType);
        entity.addAttribute(SessionServiceConst.NAME,portTitle);
	    String resourceElementName = SessionServiceConst.PORT;
	    Element resourceElement = MRXMLUtils.stringToElement("<" + resourceElementName + "/>");
	    entity.add(resourceElement);
		resourceElement.addAttribute(SessionServiceConst.RESOURCENID,portNID);
		resourceElement.addAttribute(SessionServiceConst.RESOURCETITLE,portTitle);
		
		String portIP = portElement.attributeValue("portIP");
		if(portIP != null) {
			resourceElement.addAttribute(SessionServiceConst.RESOURCEIP,portIP);
	        log.addInfo("UpdatSessionUtils:addPortEntity portIP=" + portIP, MRLog.OK, MRLog.NOTIFY);   
		}
		String portNumber = portElement.attributeValue("portNumber");
		if(portNumber != null) {
			resourceElement.addAttribute(SessionServiceConst.RESOURCEPORTNUMBER,portNumber);
	        log.addInfo("UpdatSessionUtils:addPortEntity portNumber=" + portNumber, MRLog.OK, MRLog.NOTIFY);   
		}
		return entity;
	}
	

	
	public static Element createPortEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document doc, String sessionId, String resourceType) throws Exception {
        log.addInfo("UpdateSessionUtils:createPortEntity sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
		Element portElement = MRXMLUtils.getElementXpath(doc, "//" + resourceType);
		return createPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, portElement, resourceType);
	}
	
	//moved playback api method to upate session. We will reuse these methods in invite request	
	public static String addSrcFileToLiveSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc, String sessionId, String roomNID) throws Exception {
		
		/*
		<SrcFile id="" fileNID="" fileTitle=""  grpFileId="grpId1"/>
		*/
        log.addInfo("PlaybackRequestHandler:addSrcEntity sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);

		Document sessionTemplateDoc = Session.getSessionTemplate(log);
		Element tmpSrcSession = sessionTemplateDoc.getRootElement();
		String tmpSessionId = Utils.getUUID();
		tmpSrcSession.addAttribute(SessionServiceConst.ID, tmpSessionId);

		Element fileElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.SRCFILE);
		Element entity = createPlyFileEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fileElement, sessionId);		
		Element entityListElement = tmpSrcSession.element("EntityList");
		entityListElement.add(entity);
		
		SessionResourceUtils.updateSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				sessionTemplateDoc, tmpSessionId, "");
		
		String logType = AuditEventLogger.NAME_FILE;
		Document loggerDocument = request.getLoggingDocument();
		if(loggerDocument ==null) {
			request.setLoggingDocument(MRXMLUtils.stringToDocument("<Log type='"+logType+"' plyFileNID='"+MRXMLUtils.getAttributeValue(fileElement, SessionServiceConst.FILENID,"")+"'/>"));		
		} else {
			MRXMLUtils.setAttributeValue(loggerDocument.getRootElement(), "type",  AuditEventLogger.NAME_FILE);		
			MRXMLUtils.setAttributeValue(loggerDocument.getRootElement(), "plyFileNID",MRXMLUtils.getAttributeValue(fileElement, SessionServiceConst.FILENID,""));
		}
		
		// Check that we have a valid source
		String resourceElementName = SessionServiceConst.FILE;
		Element resourceElement = entity.element(resourceElementName);
		String resourceNID = resourceElement.attributeValue(SessionServiceConst.RESOURCENID);
		if((resourceNID == null) || ("".equals(resourceNID)) )
			throw new MRException(SessionServiceExceptionCodes.SOURCE_RESOURCE_NOT_FOUND, 
					"SrcFile not found fileTitle=" + resourceElement.attributeValue(SessionServiceConst.RESOURCETITLE));
		// Update the current session using the data in the tmpSession
		
		// Add the entities
		List<?> entityList = MRXMLUtils.getListXpath(sessionTemplateDoc, "//" + SessionServiceConst.ENTITY);
		for(Object o: entityList) {
			Element entityItem = (Element)o;
			updateEntityNameAndType(request, log, db, entityItem, resourceElementName);
			AppServerAgentUtils.getSessionManager(serviceAgentFactory).addEntity(roomNID, entityItem);
		}

		// Add the GrFile
		Element sessionGrpFileElement = MRXMLUtils.getElementXpath(sessionTemplateDoc, "//" + SessionServiceConst.GRPFILE);
		if(sessionGrpFileElement != null) {
			AppServerAgentUtils.getSessionManager(serviceAgentFactory).addGrpFile(roomNID, sessionGrpFileElement);
		}
		
		// Create the sources and destinations
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);	
		SessionMediaRoomUtils.createSourcesAndDestinations(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, roomNID, entityList, false);

		return entity.attributeValue(SessionServiceConst.ID);
	}
	
	
	

	public static Element createPlyFileEntity(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Element fileElement, String sessionId) throws Exception {
		String fileNID = fileElement.attributeValue("fileNID");
		String fileTitle = fileElement.attributeValue("fileTitle");
		List<String> entityStreamTypeList =  null;
		//if((fileTitle == null) || fileTitle.isEmpty()) {
		Document fileDoc = null;
		if(fileNID !=null && !fileNID.isEmpty())
		      fileDoc = MRXMLDBUtils.readElement(request, log, db, fileNID);
		//Element file = this.getFile(request, xmpp, log, db, resourceFactory, serviceAgentFactory, request.getMessage().getDocument(), fileNID);
		if(fileDoc != null) {
			//fileTitle = file.attributeValue("title");
			fileTitle = MRXMLUtils.getValueXpath(fileDoc, "//Info/Title", "");
			entityStreamTypeList = MRXMLUtils.getValuesXpath(fileDoc.getRootElement(), ".//StreamTypeList/StreamType");
		}
			
		//}
	
		String id = fileElement.attributeValue(SessionServiceConst.ID);
		if((id == null) || ("".equals(id)) ) // Generate an id if not present
			id = Utils.getUUID();
		
	    Element entity = MRXMLUtils.stringToElement("<" + SessionServiceConst.ENTITY + "/>");
        entity.addAttribute(SessionServiceConst.GC,MRConst.FALSE);
        entity.addAttribute(SessionServiceConst.ID,id);
        entity.addAttribute(SessionServiceConst.TYPE_ATTRIBUTE, SessionServiceConst.PLYFILE);
        entity.addAttribute(SessionServiceConst.NAME, fileTitle);
	    String resourceElementName = SessionServiceConst.FILE;
	    Element resourceElement = MRXMLUtils.stringToElement("<" + resourceElementName + "/>");
	    entity.add(resourceElement);
		resourceElement.addAttribute(SessionServiceConst.RESOURCENID, fileNID);
		resourceElement.addAttribute(SessionServiceConst.RESOURCETITLE, fileTitle);
		
		MRXMLUtils.addAttribute(entity, SessionServiceConst.STREAM_TYPE, StringUtils.join(entityStreamTypeList, " "));

		return entity;
	}
	
	public static Element createGrpFileElement(Document requestDoc, String fileTitle) throws Exception  {
		Element grpFileElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.GRPFILE);
		String grpFileNID = grpFileElement.attributeValue("fileNID");
		String grpFileTitle = grpFileElement.attributeValue("fileTitle");
		String grpId = grpFileElement.attributeValue(SessionServiceConst.ID);
		if("".equals(grpId)) // Generate an id if not present
			grpId = Utils.getUUID();
	    Element sessionGrpFileElement = MRXMLUtils.stringToElement("<" + SessionServiceConst.GRPFILE + "/>");
	    sessionGrpFileElement.addAttribute(SessionServiceConst.RESOURCENID, grpFileNID);
		if((grpFileTitle == null) || grpFileTitle.isEmpty()) {
			grpFileTitle = fileTitle;
		}
	    sessionGrpFileElement.addAttribute(SessionServiceConst.RESOURCETITLE, grpFileTitle);
	    sessionGrpFileElement.addAttribute(SessionServiceConst.GC,MRConst.FALSE);
	    sessionGrpFileElement.addAttribute(SessionServiceConst.ID,grpId);
	    sessionGrpFileElement.addAttribute(SessionServiceConst.TYPE_ATTRIBUTE, SessionServiceConst.SYNCPLY);
	    return sessionGrpFileElement;

	}
	
	public static Element getFile(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,
			String fileNID) throws Exception {
		
		Document getFileRequestDoc = MRXMLUtils.stringToDocument("<GetFile whereNID='" + fileNID + "' whereState= '" + SessionServiceConst.READY + ","
		+MediaStoreServiceConst.BUSY+"' />");
		MRRequest getFileRequest = MRRequest.createRequest(request, getFileRequestDoc, 
				request.getRequestName(), request.getServiceName());
		GetFileRequestHandler handler = new GetFileRequestHandler();
		Document response = handler.getResponse(getFileRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		//System.out.println("PlayhackRequestHandler:getClipFiles response=" + MRXMLUtils.documentToString(response));
		return MRXMLUtils.getElementXpath(response, "//GetFile/File");
	}
	
	
	public static void addGrpFileToLiveSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,ServiceAgentFactoryI serviceAgentFactory, Document requestDoc,
			 String sessionId, String roomNID) throws Exception {
		Element grpFileElement = MRXMLUtils.getElementXpath(requestDoc, "//" + SessionServiceConst.GRPFILE);
		String grpFileNID = MRXMLUtils.getAttributeValue(grpFileElement, SessionServiceConst.FILENID);
	    Element grpFile = getFile(request, xmpp, log, db, resourceFactory, serviceAgentFactory, request.getMessage().getDocument(), grpFileNID);
	 	AppServerAgentUtils.getSessionManager(serviceAgentFactory).addGrpFile(roomNID, createGrpFileElemet(request, xmpp, log, db, resourceFactory, serviceAgentFactory,  grpFile));
		
	 	List<?> fileList = MRXMLUtils.getListXpath(grpFile, ".//FileList/File");
	 	List<Element> entityList = new ArrayList<Element>();
	    for(Object o : fileList) {
	    	Element fileEntity = createPlyFileEntityElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, requestDoc, (Element)o,sessionId);
	    	AppServerAgentUtils.getSessionManager(serviceAgentFactory).addEntity(roomNID, fileEntity);
	    	entityList.add(fileEntity);
	    }
	    
		// Create the sources and destinations
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);	
		SessionMediaRoomUtils.createSourcesAndDestinations(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionDoc, roomNID, entityList, false);

	}
	
	public static Element createGrpFileElemet(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Element grpFile)  throws MRException, Exception {
		Element sessionGrpFileElement = createGrpFileElement(request.getMessage().getDocument(), grpFile.attributeValue("title"));
		MRXMLUtils.setAttributeValue(sessionGrpFileElement, SessionServiceConst.DATE_CREATED, MRXMLUtils.getAttributeValue(grpFile, SessionServiceConst.DATE_CREATED,""));
	    return sessionGrpFileElement;
	}
	
	public static Element createPlyFileEntityElement(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document requestDoc, Element fileElement,String sessionId) throws Exception {
		Element srcFileElement = MRXMLUtils.stringToElement("<" + SessionServiceConst.SRCFILE + "/>");
			
		String fileNID = fileElement.attributeValue("NID");
    	String fileTitle = fileElement.attributeValue("title");
    	srcFileElement.addAttribute(SessionServiceConst.FILENID, fileNID);
    	srcFileElement.addAttribute(SessionServiceConst.FILETITLE, fileTitle);
    	
    	// Create file entity
		return createPlyFileEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, srcFileElement, sessionId);
	}

	public static String addDstPortToLiveSession(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			Document requestDoc, String sessionId, String roomNID) throws Exception {
        log.addInfo("UpdateSessionUtils:addDstEntity sessionId=" + sessionId, MRLog.OK, MRLog.NOTIFY);
        String resourceType = SessionServiceConst.DST_PORT;
		Element entity = UpdateSessionUtils.createPortEntity(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
				requestDoc, sessionId, resourceType);
	    String resourceElementName = SessionServiceConst.PORT;
		Element resourceElement = entity.element(resourceElementName);
		SessionResourceUtils.updateResourceElement(request, xmpp, log, db, resourceFactory, serviceAgentFactory, 
    			resourceElement, resourceElementName, resourceType);
		String resourceNID = resourceElement.attributeValue(SessionServiceConst.RESOURCENID);
		if((resourceNID == null) || ("".equals(resourceNID)) )
			throw new MRException(SessionServiceExceptionCodes.DEST_RESOURCE_NOT_FOUND, 
					"DstPort not found portTitle=" + resourceElement.attributeValue(SessionServiceConst.RESOURCETITLE));
		UpdateSessionUtils.updateEntityNameAndType(request, log, db, entity, SessionServiceConst.PORT);
		AppServerAgentUtils.getSessionManager(serviceAgentFactory).addEntity(roomNID, entity);
		Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionId);
		String id = entity.attributeValue(SessionServiceConst.ID);
		SessionMediaRoomUtils.createMediaDest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, id, sessionDoc, roomNID);
		return id;		
	}


}
