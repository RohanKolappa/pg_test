package com.ipvs.sessionservice.handler.base;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Hashtable;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.file.handler.BasePublishFileRequestHandler;
import com.ipvs.assetadminservice.file.handler.PublishFileRequestHandler;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.assetadminservice.impl.DirSM;
import com.ipvs.assetadminservice.impl.MediaManagementUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.impl.DeleteObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.DeleteFileRequestHandler;
import com.ipvs.sessionservice.resource.handler.GetFileRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.Utils;

public abstract class FileBaseRequestHandler extends AsyncBaseRequestHandler {



	protected Hashtable<String,String> getFileMapping(List<?> fileList) throws MRException,Exception{
		Hashtable<String,String> fileMapping = new Hashtable<String,String>();
		for(Object object:fileList){
			Element element = (Element) object;
			String fileNID = MRXMLUtils.getAttributeValue(element, SessionServiceConst.NID,"");
			String storeNID = MRXMLUtils.getValueXpath(element, ".//"+MediaStoreServiceConst.UUID,"");
			fileMapping.put(fileNID, storeNID);
		}
		return fileMapping;
	}

	@Deprecated
	//TODO: Update this when fixing the ILM code to NEHA From Padmaja
	protected void sendRequestToMediaStoreAgent(MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String mediaStoreRequestName,String mediaStreamFileDirNID,List<?> fileList, String actionData, String responseHandlerClass,boolean purgeFiles, boolean isWriteOperation)  throws MRException,Exception {
		Document mediaStoreRequestDocument = MediaStoreServiceUtils.generateRequestDocument(mediaStoreRequestName,getFileMapping(fileList), mediaStreamFileDirNID, actionData,purgeFiles, userRequest.getNID(), log);
		String mediaStoreStreamAgentJID = DirSM.getDirAgentJID(userRequest, xmpp, log, schema, mediaStreamFileDirNID);

		if ((mediaStoreStreamAgentJID == null) || (mediaStoreStreamAgentJID.trim().length() == 0)) {
			throw new MRException(MediaRoomServiceExceptionCodes.AGENT_NOT_FOUND, "Media Store Agent JID Not Found");
		}
		sendRequestToAgent(userRequest, xmpp, log, schema, resourceFactory, serviceAgentFactory, mediaStoreRequestName, mediaStoreRequestDocument, MRConst.MEDIA_STORE_ADMIN_SERVICE,
				mediaStoreStreamAgentJID, MRConst.MEDIA_STORE_ADMIN_SERVICE, responseHandlerClass);
	}


	protected void sendSetFileRequestToMediaStore(MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document mediaStreamFileDoc, String title, String description, String state, String bypassAutoStorageRules, Element metaDataEl, String responseHandlerClass)  throws MRException,Exception {

		String mediaStreamFileDirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
		Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(mediaStreamFileDoc, true, userRequest, db, log);

		Document mediaStoreRequestDocument = generateRequestDocument(MediaStoreServiceConst.SET_FILE_REQUEST, getFileMapping(Arrays.asList((Element)mediaStreamFileDoc.getRootElement().clone())), 
				mediaStreamFileDirNID, mediaStorePortElement, title, description, state, bypassAutoStorageRules, metaDataEl, userRequest.getNID(), log);

		sendMediaStoreRequestToDMS(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory,mediaStoreRequestDocument, mediaStorePortElement, responseHandlerClass);	
	}



	public Document generateRequestDocument(String mediaStoreRequestName, Hashtable<String,String> fileMapping, String mediaStreamFileDirNID,Element mediaStorePortResourceEL, 
			String title, String description, String state, String bypassAutoStorageRules, Element metaDataEl, String requestNID, MRLog log) throws MRException, Exception {

		String mediaStreamFileDirID = MRXMLUtils.getAttributeValue(mediaStorePortResourceEL, MediaStoreServiceConst.DEVICE_ELEMENT_ID_ATTRIBUTE_NAME, "");
		Document requestDoc = createRequestDocument(mediaStoreRequestName, mediaStreamFileDirNID, mediaStreamFileDirID, log);
		Element requestEl = MRXMLUtils.getElementXpath(requestDoc, "//" + mediaStoreRequestName);
		Element element = MRXMLUtils.newElement(SessionServiceConst.FILE);
		for(String mediaStreamFileNID: fileMapping.keySet()) {
			MRXMLUtils.addAttribute(element, MediaStoreServiceConst.BY_ID_ATTRIBUTE_NAME, fileMapping.get(mediaStreamFileNID));
			MRXMLUtils.addAttribute(element, SessionServiceConst.FILENID, mediaStreamFileNID);
			if(!bypassAutoStorageRules.isEmpty())
				MRXMLUtils.addAttribute(element, MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES, bypassAutoStorageRules);
			if(!title.isEmpty())
				MRXMLUtils.addAttribute(element, SessionServiceConst.TITLE, title);
			if(!state.isEmpty())
				MRXMLUtils.addAttribute(element, SessionServiceConst.STATE, state);
			if(!description.isEmpty()) 
				MRXMLUtils.addElement(element, MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, description);
			if(metaDataEl != null)
				MRXMLUtils.addElement(element, metaDataEl);

			MRXMLUtils.addElement(requestEl, (Element)element.clone());
		}

		return requestDoc;

	}


	public Document createRequestDocument(String mediaStoreRequestName, String mediaStreamFileDirNID, 
			String dirID, MRLog log) throws MRException, Exception {


		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document newRequestDoc = (Document)requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(newRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, mediaStoreRequestName);
		MRXMLUtils.setAttributeValueXPath(newRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		MRXMLUtils.addAttribute(MRXMLUtils.getElementXpath(newRequestDoc, MRConst.HEADER_XPATH), MRConst.TYPE,  MRConst.REQUEST_TYPE_RPC);
		Element DataEL = MRXMLUtils.getElementXpath(newRequestDoc, "//Data");

		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(mediaStoreRequestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(mediaStoreRequestName);
		MRXMLUtils.addAttribute(requestEl, "byDirNID", mediaStreamFileDirNID);
		MRXMLUtils.addAttribute(requestEl, "byDirID", dirID);		
		MRXMLUtils.addElement(requestDataEl, requestEl);

		return newRequestDoc;

	}



	protected Document getFileResponse(MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,Element requestElement) throws MRException,Exception {

		// get the file information

		String byNID = MRXMLUtils.getAttributeValueWithAlternate(requestElement, SessionServiceResourceConst.BY_NID_ATTRIBUTE, SessionServiceConst.WHERE_NID, "");
		String byTitle = MRXMLUtils.getAttributeValueWithAlternate(requestElement, SessionServiceResourceConst.BY_TITLE_ATTRIBUTE, SessionServiceConst.WHERE_TITLE, "");

		return getFileResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, byNID, byTitle,"");
	}


	protected Document getFileResponse(MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String byNID,String byTitle, String whereType) throws MRException,Exception {

		// get the file information

		Document getFileSampleRequestDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.GET_FILE_REQUEST_XML, log);
		String requestName = MRRequest.getRequestName(GetFileRequestHandler.class.getName());
		log.addInfo("File base request handler Start Creating getfile request");
		Element getFileEL = MRXMLUtils.getElementXpath(getFileSampleRequestDoc,"//GetFile");
		MRXMLUtils.setAttributeValue(getFileEL, SessionServiceConst.WHERE_NID, byNID);
		MRXMLUtils.setAttributeValue(getFileEL, SessionServiceConst.WHERE_TITLE, byTitle);	
		MRXMLUtils.setAttributeValue(getFileEL, SessionServiceConst.SELECTFORMAT, SessionServiceConst.SELECTFORMAT_EXTENDED);
		if (whereType!=null && !whereType.isEmpty())
			MRXMLUtils.setAttributeValue(getFileEL,SessionServiceConst.WHERE_TYPE, whereType);
		Document getFileRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, getFileEL);
		
		MRRequest getFileRequst = MRRequest.createRequest(userRequest, getFileRequestDoc, requestName, ServiceClient.SESSIONSERVICE_RESOURCE);
		getFileRequst.setSuperUser(true);
		GetFileRequestHandler fileRequestHandler = new GetFileRequestHandler();
		log.addInfo("File base request handler End Creating getfile request: Send request: "+getFileRequestDoc.asXML());
		Document responseDoc = null;
		if(byTitle!=null && !byTitle.isEmpty())
		 responseDoc = fileRequestHandler.getResponse(getFileRequst, xmpp, log, db, resourceFactory, serviceAgentFactory);
		else 
		 responseDoc = fileRequestHandler.getFileAlternativeForWhereNID(getFileRequst, xmpp, log, db, resourceFactory, serviceAgentFactory, byNID,SessionServiceConst.SELECTFORMAT_EXTENDED);
		//log.addInfo("File base request handler End Creating getfile request: Send request: "+getFileRequestDoc.asXML());
		Element respFileEl = MRXMLUtils.getElementXpath(responseDoc, "//File");
		respFileEl=respFileEl !=null?(Element)respFileEl.clone():respFileEl;
		return userRequest.createResponseUsingRequestRootName(respFileEl);
	}
	@Override
	public AuditEvent getAuditEvent(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String status, String level, String errorDescription)
			throws Exception {
		String title = "";
		Element targetGroupsEl = null;

		Element requestElement = MRXMLUtils.getElementXpath(request.getMessage().getDocument(),"//"+ MRRequest.getRequestRootName(request.getRequestName()));		
		String whereNID = MRXMLUtils.getAttributeValue(requestElement, SessionServiceConst.WHERE_NID,"");
		if(request.getException()==null) {
			Element responseEl = MRXMLUtils.getElementXpath(request.getResult(), "//File[@NID='"+whereNID+"']");
			targetGroupsEl = MRXMLUtils.getElementXpath(responseEl, ".//"+MediaStoreServiceConst.GROUPS_EL_NAME);
			if(responseEl == null) {
				return null;
			}
			title    = MRXMLUtils.getAttributeValue(responseEl, SessionServiceConst.TITLE, "");
		}
		return  new  AuditEvent.Builder(AuditEventLogger.MEDIA_EVENT, AuditEventLogger.NAME_MODIFY, request.getUserJID(), level)
		.target(title).status(status).descriptionText(""+errorDescription).operatorEl(request.getOperatorGroups()).targetEl(targetGroupsEl).build();
	}


	protected void sendMediaStoreRequestToDMS(MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory, Document mediaStoreRequestDoc,Element mediaStorePortResourceEL,String responseHandlerClass) throws Exception,MRException {

		String mediaStorePortNID = MRXMLUtils.getValue(mediaStorePortResourceEL);
		String mediaStoreStreamAgentJID = MediaRoomServiceUtils.getContactJIDForPort(userRequest, log, db, mediaStorePortNID);		
		if ((mediaStoreStreamAgentJID == null) || (mediaStoreStreamAgentJID.trim().length() == 0)) {
			throw new MRException(MediaRoomServiceExceptionCodes.AGENT_NOT_FOUND, "Media Store Agent JID Not Found");
		}
		sendRequestToAgent(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, MRXMLUtils.getAttributeValueXpath(mediaStoreRequestDoc, MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, ""),
				mediaStoreRequestDoc,MRConst.MEDIA_STORE_ADMIN_SERVICE,	mediaStoreStreamAgentJID, MRConst.MEDIA_STORE_ADMIN_SERVICE, responseHandlerClass);

	}

	public Document createOrUpdateMediaStreamFileInDB(MRRequest userRequest, Document agentResponseDoc, Element groupsEl, boolean addFile, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc,MRConst.HEADER_XPATH, MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);
		if (state == MediaStoreServiceConst.FAILURE_STATE) {
			return userRequest.createResponseData();
		}

		Element fileEl = MRXMLUtils.getElementXpath(agentResponseDoc, "//File");		
		Element mediaStreamFileEl = createOrUpdateMediaStreamFileInDB(fileEl, userRequest, agentResponseDoc, groupsEl, addFile, xmpp, log, db, resourceFactory, serviceAgentFactory);

		return getFileResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, MRXMLUtils.getAttributeValue(mediaStreamFileEl, SessionServiceConst.NID,""), "","");
	}
	
	
	
	public Element createOrUpdateMediaStreamFileInDB(Element fileEl, MRRequest userRequest, Document agentResponseDoc, Element groupsEl, boolean addFile, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
	
		Element mediaStoreFileEl  = MRXMLUtils.getElementXpath(agentResponseDoc, "//MediaStoreFile");		
		String mediaStoreFileNID = MRXMLUtils.getAttributeValue(fileEl, SessionServiceConst.ID,"");
		String dirNID = MRXMLUtils.getAttributeValue(fileEl, AMConst.DIR_NID_ATTRIBUTE,"");
		String fileNID = MRXMLUtils.getAttributeValue(fileEl, "fileNID", "");
		Element mediaStreamFileEl = null;
		Element downloadElement = null;
		if(!fileNID.isEmpty()) {
			Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(fileNID, userRequest, log, db);
			List<?> mediaElList = MRXMLUtils.getListXpath(mediaStreamFileDoc,"//Media");
			for (Object obj : mediaElList) {
				Element mediaEL = (Element) obj;
				String state = mediaEL.attributeValue(SessionServiceConst.FILE_STATE_FLAG);
				if(SessionServiceConst.FILE_STATE_PREPARED.equalsIgnoreCase(state))
					mediaEL.addAttribute(SessionServiceConst.FILE_STALE_FLAG, SessionServiceConst.TRUE);
			}
			
			mediaStreamFileEl = mediaStreamFileDoc.getRootElement();
			downloadElement= MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//Download");
		}
		
		
		Element responseEl =   new BasePublishFileRequestHandler().CreateOrUpdateMediaStreamFileInDatabase(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, fileEl, mediaStoreFileNID,mediaStoreFileEl, groupsEl, addFile, "", dirNID, mediaStreamFileEl, downloadElement);
		fileNID = MRXMLUtils.getAttributeValue(mediaStreamFileEl, SessionServiceConst.NID,"");
		String fileType = MRXMLUtils.getValueXpath(responseEl, ".//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, ""); 
		if(!addFile){ 
			MRRequest newrequest = MRRequest.createRequest(userRequest.getAppServerJID());
			newrequest.setSuperUser(true);
			new PublishFileRequestHandler().sendFileNotification(MRRequest.createRequest(userRequest.getAppServerJID()), xmpp, log, db, resourceFactory, serviceAgentFactory, fileNID, SessionServiceConst.UPDATE, fileType);
		}
		
		return responseEl;
		
	}
	
	
	

	protected void sendDeleteMediaStoreRequestToDMS( Document mediaStreamFileDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String responseHandlerClass)	throws MRException, Exception	{
		String requestName =  "DeleteFileRequest";
		String dirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
		Element mediaStorePortElement = MediaStoreServiceUtils.getMediaStorePortResourceElementFromFile(mediaStreamFileDoc, true, request, db, log);
		Document mediaStoreRequestDocument= generateRequestDocument(requestName, getFileMapping(Arrays.asList((Element)mediaStreamFileDoc.getRootElement().clone())),
				dirNID, mediaStorePortElement, "", "", "", "", null, request.getNID(), log);
		sendMediaStoreRequestToDMS(request, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStoreRequestDocument, mediaStorePortElement, responseHandlerClass);
	}

	public void deleteFileFromDatabase(String mediaStreamFileNID, MRRequest request, MRLog log, MRDB db, XMPPI xmpp) throws Exception {
	
		//Delete Bookmarks
		String[] arrWhereXpath = {"//Bookmark[MediaStreamFileResourceNID='"+mediaStreamFileNID+"']"};	
		MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.BOOKMARK, "", "", arrWhereXpath);
		log.addInfo("deleteFileFromDatabase: Delete Bookmarks fileNID:=" + mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);
		//Delete Media stream file from DB
		DeleteObjectUtils.deleteObjectByNID(request, xmpp, log, db, "MediaStreamFileResource", mediaStreamFileNID);

		log.addInfo("deleteFileFromDatabase: Delete MediaStreamFileResource NID:=" + mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);

	}



	public Document deleteMediaStreamFileDB(String mediaStreamFileNID, MRRequest request, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,AgentResponseHandlerI handlerClass)
			throws MRException, Exception
			{
		Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);
		String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
		deleteFileFromDatabase(mediaStreamFileNID, request, log, db, xmpp);
		String type = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

		if (!mediaGroupID.isEmpty() && !type.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaGroup.toString())) {
			try
			{
				List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, db);
				if ((listOfFiles == null) || (listOfFiles.isEmpty())) {
					deleteMediaGroupFile(mediaGroupID, request, log, db, xmpp, resourceFactory, serviceAgentFactory,handlerClass);
				}
				else
				{
					MediaStoreServiceUtils.updateMediaGroupOnMediaClipUpdate(mediaStreamFileDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				}
			}
			catch (Exception e) {
				e.printStackTrace();
			}

		}

		return mediaStreamFileDoc;
	}
	
	//this method can be removed in future releases.
	public void deleteMediaGroupFile(String mediaGroupID, MRRequest request, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,AgentResponseHandlerI handlerClass)
			throws MRException, Exception
	{		
		log.addInfo("deleteMediaStreamFileDB: Aquire lock on mediaGroupID:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);
	
		Document mediaGroupDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);
		if(mediaGroupDoc == null){
			return;
		}
		log.addInfo("deleteMediaStreamFileDB: Delete Media Group mediaGroupID:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);
		String fileNID = MRXMLUtils.getAttributeValue(mediaGroupDoc.getRootElement(), MediaStoreServiceConst.NID, "");
		String dirNID = MRXMLUtils.getValueXpath(mediaGroupDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
		if(isFileInDefaultDir(dirNID)){
			deleteFileFromDatabase(fileNID, request, log, db, xmpp);
		} else {
			//send delete media group request to store.
			MRRequest newRequest = MRRequest.createRequest(request.getAppServerJID());
			new DeleteFileRequestHandler().deleteFile(mediaGroupDoc, newRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, 
					handlerClass.getClass().getName());
		}	
	}


	public void handleEmptyMediaGroupForDeletedFiles(String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory) throws MRException, Exception {
		handleEmptyMediaGroupForDeletedFiles(MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME, userJID, xmpp, log, db, resourceFactory, profileFactory);
		//Make sure media groups parent dir nid as empty. If they are changing the logic to move NONE then this logic should modify.
		handleEmptyMediaGroupForDeletedFiles("", userJID, xmpp, log, db, resourceFactory, profileFactory);
	}


	public  void handleEmptyMediaGroupForDeletedFiles(String dirNID, String userJID, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) throws MRException, Exception {
		List<?> mediaGroupList = MediaManagementUtils.getListOfFilesInDir(userJID, dirNID, db, log, xmpp);

		if ((mediaGroupList == null) || mediaGroupList.isEmpty()) {
			return;
		}

		Document mediaStreamFileDoc = null;
		String mediaGroupID = null;
		MRRequest request = MRRequest.createRequest(userJID);

		List<String> mediaStreamFileNIDList = new ArrayList<String>();


		for (Object object : mediaGroupList) {
			mediaStreamFileDoc = MRXMLUtils.elementToDocument((Element) object);

			mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");

			if (!mediaGroupID.isEmpty()) {
				String whereXpath = 
						"//" + AMConst.MEDIA_STREAM_FILE_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + 
								MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "[(" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "='" + 
								mediaGroupID + "') " + " and (" +
								MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME + "!='" + MediaRoomServiceConst.MEDIAGROUP + "')]";
				List<?> mediaStreamFileList = getMediaStreamFileNIDList(whereXpath, request, db, log, xmpp);
				if (!mediaStreamFileList.isEmpty()) {
					continue;
				}
			}

			mediaStreamFileNIDList.add(
					"'"+MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), MediaStoreServiceConst.NID, "")+"'");            

		}
		if(mediaStreamFileNIDList.isEmpty()) {
			return;
		}

		deleteFilesAndBookmarks(mediaStreamFileNIDList, request, db, log);       
	}

	private  void deleteFiles(List<String> mediaStreamFileNIDList , MRRequest request,MRDB db,MRLog log ) throws MRException,Exception {
		String[] arrWhereXpath = {"//MediaStreamFileResource[@NID=(" + StringUtils.join(mediaStreamFileNIDList, ",")+ ")]"};	
		MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.MediaStreamFileResource, "", "", arrWhereXpath);
		log.addInfo("Delete Media Files");
	}  

	private  List<?> getMediaStreamFileNIDList(String whereXpath,MRRequest request,MRDB db,MRLog log,XMPPI xmpp) throws MRException,Exception {

		String selectXML = "<MediaStreamFileResource NID='{$VAR/@NID/data(.) }'/>";
		Element responseEL = AssetAdminServiceUtils.getChildBasedOnSelectXML(AMConst.MEDIA_STREAM_FILE_RESOURCE, selectXML,
				whereXpath, request, db, log, xmpp);

		return MRXMLUtils.getList(responseEL);
	}

	private void deleteFilesAndBookmarks(List<String> mediaStreamFileNIDList, MRRequest request, MRDB db, MRLog log) throws MRException,Exception {
		int countToFetch = 25;
		int startIndex = 0;
		int listSize = mediaStreamFileNIDList.size();
		LinkedList<String> list = new LinkedList<String>(mediaStreamFileNIDList);
		while(startIndex < listSize) {
			int endIndex = ((startIndex + countToFetch) < listSize ? (startIndex + countToFetch) :  listSize);
			List<String> subList= list.subList(startIndex, endIndex);
			if(subList.isEmpty()){
				break;
			}
			deleteBookmarks(subList, request, db, log);  
			deleteFiles(subList, request, db, log);
			log.addInfo("deleteFilesAndBookmarks. startIndex:="+startIndex+" endIndex:="+endIndex+" countToFetch:="+countToFetch+ " NoOfFiles:="+listSize, MRLog.INFO,MRLog.NOTIFY);
			startIndex = startIndex + countToFetch;
		}
	}


	public void deleteBookmarks(List<String> mediaStreamFileNIDList , MRRequest request,MRDB db,MRLog log ) throws MRException,Exception {
		String[] arrWhereXpath = {"//Bookmark[MediaStreamFileResourceNID=(" + StringUtils.join(mediaStreamFileNIDList, ",")+ ")]"};	
		MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.BOOKMARK, "", "", arrWhereXpath);
		log.addInfo("Delete Boookmarks",MRLog.INFO,MRLog.NOTIFY);

	}
	
	public boolean isFileInDefaultDir(String dirNID) {
		return (dirNID.equals(MediaStoreServiceConst.DEFAULT_DIR) || dirNID.equals(MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME));
	}
	
	public void checkPEMPermissions(String mediaStreamFileNID, Document mediaStreamFileDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
			throws MRException, Exception {
		Document assetRequestDoc = AssetAdminServiceUtils.createAssetAdminServiceRequest(AMConst.MEDIA_STREAM_FILE_RESOURCE + "_BASE_ALL_UPDATE.xml", mediaStreamFileNID, (Element) mediaStreamFileDoc.getRootElement().clone(), log);
		Element actionEL = MRXMLUtils.getElementXpath(assetRequestDoc, "//" + AMConst.REPLACE_REQUEST);

		//This one is checking to make sure user have permission to do update operation or not.        
		PolicyAdminServiceUtils.getAssetAdminUpdatePEMXPath(actionEL, AMConst.MEDIA_STREAM_FILE_RESOURCE,request, xmpp, log, db, resourceFactory, serviceAgentFactory);
	}
	
	

}
