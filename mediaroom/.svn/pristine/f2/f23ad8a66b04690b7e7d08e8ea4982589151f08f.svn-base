package com.ipvs.assetadminservice.file.handler;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;
import java.util.concurrent.Semaphore;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.AddObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.impl.UpdateObjectUtils;
import com.ipvs.sessionservice.resource.handler.GetFileRequestHandler;
import com.ipvs.sessionservice.resource.handler.GetPVRFileRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.systemservice.impl.ServiceVersionManager;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class BasePublishFileRequestHandler {
	public static final String PUBLISH_FILE_READ_QUERY ="/com/ipvs/assetadminservice/handler/xquery/PublishFileReadQuery.xml";
	public static final String _FILE_XPATH_ = "_FILE_XPATH_";   
	public static final String _BOOKMARK_XPATH_ = "_BOOKMARK_XPATH_";       
	public static final String _MEDIAGROUP_XPATH_ = "_MEDIAGROUPID_XPATH_";
	public static final String _DIR_XPATH_ = "_DIR_XPATH_";

	public String addOrUpdateFile(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory, Document responseDoc, Document resourceDoc, Element fileEl, Hashtable<String,Document> groupsFileCache) throws Exception {

		//read all the list from database
		//Document resourceDoc = getData(request, xmpp, log, db, resourceFactory, profileFactory, requestDocument);
		Semaphore lock = null;
		String queueID="";
		String mediaStoreFileID = MRXMLUtils.getAttributeValue(fileEl,SessionServiceConst.ID,"");
		try{

			lock = IPVSMonitor.getMonitor().getLock(mediaStoreFileID, mediaStoreFileID);
			log.addInfo("PublishFileRequestHandler: aquired lock on media file UUID:="+mediaStoreFileID, MRLog.INFO, MRLog.NOTIFY);
			String mediaType = MRXMLUtils.getValueXpath(fileEl, ".//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
			if(resourceDoc == null){
				log.addInfo("PublishFileRequestHandler: resourceDoc is null, readiong from db");
				resourceDoc = getData(request, xmpp, log, db, resourceFactory, profileFactory, request.getMessage().getDocument());
			}
			Element mediaStreamFileEl = getMediaStreamFileFromFileElement(fileEl, resourceDoc, groupsFileCache, mediaType);

			String fileNID = MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID,"");       
			Element mediaStoreFileEl = MRXMLUtils.getElement(fileEl, AMConst.MEDIA_STORE_FILE);
			queueID = MRXMLUtils.getAttributeValue(fileEl,SessionServiceConst.QUEUEID,"");       
			Element downloadElement= MRXMLUtils.getElement(fileEl, "Download");         
			String dirNID = MRXMLUtils.getAttributeValue(fileEl, AMConst.DIR_NID_ATTRIBUTE,"");
			String dirSyncStr = MRXMLUtils.getAttributeValue(fileEl, "dirSync","");
			boolean dirSync = Utils.getBoolean(dirSyncStr, false);

			boolean addFile = false;
			String action = SessionServiceConst.UPDATE;
			Element dirGroupEl = null;
			if(mediaStreamFileEl == null) {     //add file in the database
				dirGroupEl = MRXMLUtils.getElementXpath(resourceDoc, "//MediaStreamDirResource[@NID='"+dirNID+"']/Info/Groups");
				addFile = true;
				action = SessionServiceConst.ADD;

			} 

			mediaStreamFileEl = CreateOrUpdateMediaStreamFileInDatabase( request,  xmpp,  log,  db,  resourceFactory, profileFactory,fileEl,mediaStoreFileID,mediaStoreFileEl,dirGroupEl,addFile,fileNID,dirNID, mediaStreamFileEl,groupsFileCache, downloadElement);

			//Update/Add Bookmarks
			String mediaStreamFileNID=  MRXMLUtils.getAttributeValue(mediaStreamFileEl,SessionServiceConst.NID,"");
			List<?> listOfBookmarks  =  MRXMLUtils.getListXpath(MRXMLUtils.elementToDocument(fileEl), "//Bookmark");

			if(listOfBookmarks!=null && listOfBookmarks.size()>0){
				for(Object bookmarkobj:listOfBookmarks) {
					Element storeBookmarkEl = (Element) bookmarkobj;
					String mediaStoreBookmarkUUID = MRXMLUtils.getAttributeValue(storeBookmarkEl,SessionServiceConst.ID,"");
					Element mediaBookmarkEl = MRXMLUtils.getElementXpath(resourceDoc, "//Bookmark[.//UUID='"+mediaStoreBookmarkUUID+"']");
					Document stoteBookmarkDoc = MRXMLUtils.elementToDocument(storeBookmarkEl);          

					if(mediaBookmarkEl== null){
						// Add bookmark
						Document bookmarkObjectDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.BOOKMARK, log);
						bookmarkObjectDoc =  createBookmarkObjectDocument(bookmarkObjectDoc, stoteBookmarkDoc,request.getUserJID(), mediaStreamFileNID, xmpp, log); 
						AddObjectUtils.addObjectInDatabaseWithSuperuserPrivileges(request, xmpp, log, db, resourceFactory, profileFactory, bookmarkObjectDoc, AMConst.BOOKMARK);
						log.addInfo("PublishFileRequestHandler: Added Bookmark :="+mediaStoreBookmarkUUID,MRLog.INFO,MRLog.NOTIFY);
					}else{
						// update bookmark  
						Document  bookmarkObjectDoc =  createBookmarkObjectDocument(MRXMLUtils.elementToDocument(mediaBookmarkEl), stoteBookmarkDoc,request.getUserJID(), mediaStreamFileNID, xmpp, log);                   
						MRXMLDBUtils.updateElement(request, log, db, MRXMLUtils.getAttributeValue(mediaBookmarkEl, "NID"), bookmarkObjectDoc.getRootElement());
						log.addInfo("PublishFileRequestHandler: Updated Bookmark :="+mediaStoreBookmarkUUID,MRLog.INFO,MRLog.NOTIFY);
					}
				}
			}
			//Delete Bookmarks 
			List<?> delBookmarkUUIDList= MRXMLUtils.getValuesXpath(fileEl, "//DeleteBookmarkList/Bookmark/@id");
			if(delBookmarkUUIDList!=null && delBookmarkUUIDList.size()>0){
				String[] bookmarkIdXPath = {"//UUID=('"+ StringUtils.join(delBookmarkUUIDList, "','") +"')"}; 
				MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.BOOKMARK, "", "", bookmarkIdXPath);
				log.addInfo("PublishFileRequestHandler: Deleted Bookmark :='"+StringUtils.join(delBookmarkUUIDList, "','")+"'",MRLog.INFO,MRLog.NOTIFY);
			}   

			Element responseFileEl = MRXMLUtils.newElement("File");
			//responseFileList.add(mediaStreamFileEl);
			MRXMLUtils.addAttribute(responseFileEl, SessionServiceConst.NID, MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID, ""));
			MRXMLUtils.addAttribute(responseFileEl, SessionServiceConst.ID, MRXMLUtils.getValueXpath(mediaStreamFileEl, ".//UUID", ""));
			if(mediaType.equals(MediaStoreServiceConst.MEDIA_IMAGE_TYPE)) {
				MRXMLUtils.addAttribute(responseFileEl, SessionServiceConst.MEDIAURL, getMediaURL(xmpp, profileFactory, responseFileEl, mediaStreamFileEl, request));  
			}
			MRXMLUtils.addElement(responseDoc.getRootElement(), (Element)responseFileEl.clone());

			String notificationFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID,"");
			if(!dirSync) //avoid sending file updates on directory sync
				sendFileNotification(request, xmpp, log, db, resourceFactory, profileFactory, notificationFileNID, action, mediaType);
			else log.addInfo("PublishFileRequestHandler:  skip file notification on dir sycn");

		} finally{
			if(lock!=null) {				
				IPVSMonitor.getMonitor().releaseLock(mediaStoreFileID, lock);
				log.addInfo("PublishFileRequestHandler: released lock on media file UUID:="+mediaStoreFileID, MRLog.INFO, MRLog.NOTIFY);

			}
		}
		return queueID;
	}

	public Element  CreateOrUpdateMediaStreamFileInDatabase(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory, Element fileEl, String mediaStoreFileNID, Element mediaStoreFileEl,Element dirGroupEL,boolean addFile,String fileNID,String dirNID, Element mediaStreamFileEl, Element downloadElement) throws Exception {
		return CreateOrUpdateMediaStreamFileInDatabase(request, xmpp, log, db, resourceFactory, profileFactory, fileEl, mediaStoreFileNID, mediaStoreFileEl, dirGroupEL, addFile, fileNID, dirNID, mediaStreamFileEl, null, downloadElement);
	}
	public Element  CreateOrUpdateMediaStreamFileInDatabase(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory, Element fileEl, String mediaStoreFileNID, Element mediaStoreFileEl,Element dirGroupEL,boolean addFile, String fileNID,String dirNID, Element mediaStreamFileEl, Hashtable<String, Document> groupFileCache, Element downloadEl) throws Exception {
			
		String title = MRXMLUtils.getValueXpath(mediaStoreFileEl,".//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME,"");
		String mediaType =MRXMLUtils.getValueXpath(mediaStoreFileEl,".//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME,"");
		String description = MRXMLUtils.getValueXpath(mediaStoreFileEl,".//"+MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME,"");
		String streamType = MRXMLUtils.getValueXpath(mediaStoreFileEl,".//"+MediaStoreServiceConst.STREAM_TYPE,"");
		String dateCreated = MRXMLUtils.getValueXpath(mediaStoreFileEl,".//"+MediaStoreServiceConst.DATE_CREATED,"");
		String dateModified = MRXMLUtils.getValueXpath(mediaStoreFileEl,".//"+MediaStoreServiceConst.DATE_MODIFIED,"");
		String state = MRXMLUtils.getValueXpath(mediaStoreFileEl,".//"+MediaStoreServiceConst.STATE_EL_NAME,"");
		String byPassAutoStorage= MRXMLUtils.getValueXpath(mediaStoreFileEl, ".//"+MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, "");
		Element metaDataEl = MRXMLUtils.getElementXpath(mediaStoreFileEl, ".//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
		String typeOfRequest = MRXMLUtils.getAttributeValue(fileEl, "typeOfRequest","");

		String playLength = "";

		if(!addFile && mediaType.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaGroup.toString())){
			state = MRXMLUtils.getValueXpath(mediaStreamFileEl,".//"+AMConst.STATEFLAG,"");
			byPassAutoStorage= MRXMLUtils.getValueXpath(mediaStreamFileEl, ".//"+MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, "");
			playLength = MRXMLUtils.getValueXpath(mediaStreamFileEl, ".//"+MediaStoreServiceConst.PLAY_LENGTH, ""); 
		}


		Document mediaStreamFileDoc = null;
		String writable = MediaStoreServiceConst.TRUE;
		if(addFile) {
			mediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
		} else {
			mediaStreamFileDoc = MRXMLUtils.elementToDocument(mediaStreamFileEl);
		}
		
		String currentEventClockTime = MRXMLUtils.getAttributeValue(fileEl, "eventClockTime",null);
		String lastDBUpdateEventTime = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), "eventClockTime",null);
		log.addInfo("PublishFileRequestHandler: Added MediaStreamfile lastDBUpdateEventTime:="+lastDBUpdateEventTime + " currentEventClockTime:= "+currentEventClockTime+" UUID:= "+mediaStoreFileNID +" to dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
		if(currentEventClockTime!=null && lastDBUpdateEventTime!= null && Utils.getLongValue(currentEventClockTime,System.currentTimeMillis()) < Utils.getLongValue(lastDBUpdateEventTime,System.currentTimeMillis())){
			log.addInfo("PublishFileRequestHandler: Stale file update received ignoring: UUID:="+mediaStoreFileNID);
			return (Element) mediaStreamFileDoc.getRootElement().detach();
		}
		
		if(currentEventClockTime!=null)
			MRXMLUtils.setAttributeValue(mediaStreamFileDoc.getRootElement(), "eventClockTime", currentEventClockTime);
		Element storeFileMediaGroupEL = MRXMLUtils.getElementXpath(mediaStoreFileEl, ".//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME);
		String mediaGroupID = MRXMLUtils.getValue(storeFileMediaGroupEL);
		Semaphore lock = null;

		//lock media group id to avoid race conditions
		if(mediaGroupID != null && !mediaGroupID.isEmpty()){			
			lock = IPVSMonitor.getMonitor().getLock(mediaGroupID, mediaGroupID);
			log.addInfo("PublishFileRequestHandler: aquired lock on media group Id:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);

		}

		try {
			if(!ServiceVersionManager.getServiceVersion(log).equals(MRXMLUtils.getAttributeValue(mediaStoreFileEl, MediaStoreServiceConst.SERVICE_VERSION,""))) {
				writable = MediaStoreServiceConst.FALSE;
			}

			//Clear the Download Node
			if(typeOfRequest.equalsIgnoreCase("TrimFile")){
				Element downloadInfoEl = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//Download");
				if(downloadInfoEl != null)
					MRXMLUtils.removeContent(downloadInfoEl);

				// invalidate the recording thumbnail cache after trim.
				MediaStoreServiceUtils.invalidateFileThumbnailCache(fileNID, log);
			}

			MediaStoreServiceUtils.updateMediaStreamFileDocWithInfo(xmpp, mediaStreamFileDoc,mediaType, 
					title, description, dirNID, dirGroupEL , mediaStoreFileNID,streamType, state, writable,metaDataEl,dateCreated,dateModified,byPassAutoStorage, downloadEl);

			MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, Utils.getIntValue(MRXMLUtils.getAttributeValue(mediaStoreFileEl, MediaStoreServiceConst.REVISION, ""), 0));

			//updateMediaGroup info
			Element mediaGroupEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, ".//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME); 
			MRXMLUtils.setValue(mediaGroupEL, mediaGroupID);
			String storeClipID = MRXMLUtils.getAttributeValue(storeFileMediaGroupEL, MediaStoreServiceConst.CLIP_ID,"");
			MRXMLUtils.setAttributeValue(mediaGroupEL, MediaStoreServiceConst.CLIP_ID, storeClipID);

			//replace type element 
			Element mediaTypeEl = MRXMLUtils.getElementXpath(mediaStreamFileDoc, ".//"+mediaType);
			Element storefileMediaTypeEL = MRXMLUtils.getElementXpath(mediaStoreFileEl, ".//"+mediaType);
			MRXMLUtils.replaceContent(mediaTypeEl, storefileMediaTypeEL);

			//update size
			if(!mediaType.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaGroup.toString())) {
				if(!addFile) {      
					//after stopping recording, dms sends append size attribute as true for all other case it will send false
					String appendSize = MRXMLUtils.getAttributeValue(fileEl, "appendSize","");
					long sizeSent = Utils.getLongValue(MRXMLUtils.getAttributeValue(fileEl, SessionServiceConst.SIZE,"0"),0);

					if(sizeSent!=0) {
						long sizeInDB = Utils.getLongValue(MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE, "0"),0);

						if(appendSize.equals(MediaStoreServiceConst.TRUE) || sizeInDB == 0) {
							sizeInDB+=sizeSent;
						}

						MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(sizeInDB));
					}
				} else {
					if(MRXMLUtils.getAttribute(fileEl, SessionServiceConst.SIZE) != null) {
						MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE, MRXMLUtils.getAttributeValue(fileEl, SessionServiceConst.SIZE, "0"));
					}
				}
			} else if(!addFile){
				MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.PLAY_LENGTH, playLength);
			}

			request.setSuperUser(true);

			if(addFile) {       
				AddObjectUtils.addObjectInDatabaseNoValidationCheck(request, xmpp, log, db, resourceFactory, profileFactory, mediaStreamFileDoc, AMConst.MediaStreamFileResource);
				log.addInfo("PublishFileRequestHandler: Added MediaStreamfile :="+title + " state:= "+state+" UUID:= "+mediaStoreFileNID +" to dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
			} else {
				UpdateObjectUtils.updateElementNoValidationCheck(request, xmpp, log, db, AMConst.MediaStreamFileResource, fileNID, mediaStreamFileDoc, "");
				log.addInfo("PublishFileRequestHandler: Update MediaStreamfile :="+title + " state:= "+state+" UUID:= "+mediaStoreFileNID +" to dirNID:="+dirNID,MRLog.INFO,MRLog.NOTIFY);
			}

			//update Media Group Information
			if( !mediaType.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaGroup.toString()) && mediaGroupID!=null && !mediaGroupID.isEmpty()) {
				Document mediaGroupDoc = MediaStoreServiceUtils.addOrUpdateMediaGroupFromMediaClip(mediaStreamFileDoc, request, xmpp, log, db, resourceFactory, profileFactory);

				if(groupFileCache!=null) {
					groupFileCache.put(mediaGroupID,mediaGroupDoc);
				}
			}

		} catch (Exception e) {
			throw e;
		} finally{
			if(lock!=null) {				
				IPVSMonitor.getMonitor().releaseLock(mediaGroupID, lock);
				log.addInfo("PublishFileRequestHandler: released lock on media group Id:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);

			}
		}

		return (Element) mediaStreamFileDoc.getRootElement().detach();

	}

	protected Document getData(MRRequest request, XMPPI xmpp,MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI profileFactory,Document requestDocument) throws Exception {

		List<String> fileIdList = MRXMLUtils.getValuesXpath(requestDocument, "//File/@id");
		List<String> bookmarkIdList = MRXMLUtils.getValuesXpath(requestDocument, "//Bookmark/@id");
		List<String> dirNIDList  = MRXMLUtils.getValuesXpath(requestDocument, "//File/@dirNID");
		Set<String> dirNIDAfterRemovingDuplicates = new HashSet<String>();
		dirNIDAfterRemovingDuplicates.addAll(dirNIDList);


		List<String> groupIDList = MRXMLUtils.getValuesXpath(requestDocument, "//MediaGroupID");
		String groupXPath = "";
		if(groupIDList != null && !groupIDList.isEmpty()) {
			Set<String> groupIDAfterRemovingDuplicates = new HashSet<String>();
			groupIDAfterRemovingDuplicates.addAll(groupIDList);         
			groupXPath=" and .//MediaGroupID=('"+ StringUtils.join(groupIDAfterRemovingDuplicates, "','") +"')";
		}


		String fileIdXPath = "[.//UUID=('"+ StringUtils.join(fileIdList, "','") +"')]"; 
		String bookmarkIdXPath = "[.//UUID=('"+ StringUtils.join(bookmarkIdList, "','") +"')]"; 
		String dirNIDXPath = "[@NID=('"+ StringUtils.join(dirNIDAfterRemovingDuplicates, "','") +"')]";

		String query = FileUtils.readFileResource(PUBLISH_FILE_READ_QUERY, request.getClass());
		query = query.replaceAll(_FILE_XPATH_, fileIdXPath);
		query = query.replaceAll(_BOOKMARK_XPATH_, bookmarkIdXPath);        
		query = query.replaceAll(_DIR_XPATH_, dirNIDXPath);
		query = query.replaceAll(_MEDIAGROUP_XPATH_, groupXPath);

		String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);      

		return MRXMLUtils.stringToDocument(responseXML);
	}

	/**
	 * Set the given values in request xml.
	 * @param bookmarkObjectDoc
	 * @param requestDoc
	 * @param TC
	 * @param offset
	 * @param ownerUserJID
	 * @param bookmarkFileNID
	 * @param xmpp
	 * @param log
	 * @return
	 * @throws MRException
	 * @throws Exception
	 */
	protected Document createBookmarkObjectDocument(Document bookmarkObjectDoc, Document requestDoc, String ownerUserJID,
			String bookmarkFileNID, XMPPI xmpp, MRLog log) throws MRException, Exception {
		Element bookmarkEL = MRXMLUtils.getElementXpath(requestDoc, "//" + AMConst.BOOKMARK);
		String title = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.SESSION_CONFIG_TITLE, "");
		String TC = MRXMLUtils.getValueXpath(bookmarkEL,  "./" + SessionServiceConst.AT_TC, "");
		String offset = MRXMLUtils.getValueXpath(bookmarkEL, "./" +  SessionServiceConst.BOOKMARK_OFFSET, "");
		String dateCreated = MRXMLUtils.getValueXpath(bookmarkEL,  "./" +  MediaStoreServiceConst.DATE_CREATED, "");
		String dateModified = MRXMLUtils.getValueXpath(bookmarkEL,  "./" +  MediaStoreServiceConst.DATE_MODIFIED, "");
		String description = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.DESCRIPTION, "");
		String comment = MRXMLUtils.getValueXpath(bookmarkEL, "./" + SessionServiceConst.COMMENT, "");
		String uuid = MRXMLUtils.getAttributeValue(bookmarkEL,SessionServiceConst.ID,"");
		Element requstedMetaDataEL = MRXMLUtils.getElementXpath(bookmarkEL, "./" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME);


		String now = String.valueOf(xmpp.getCurrentTimeMillis());

		MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.AT_TC, TC);

		MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, bookmarkFileNID);
		MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.UUID, uuid);

		MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.BOOKMARK_OFFSET, offset);

		if (!dateCreated.isEmpty()) {
			MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.DATE_CREATED, dateCreated);
		}else{
			MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.DATE_CREATED, now);
		}


		if (!dateModified.isEmpty()) {
			MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, dateModified);
		}else{
			MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, now);
		}


		if (!title.isEmpty()) {
			MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
		}

		if (!description.isEmpty()) {
			MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.COMMENT, description);
		}else if (!comment.isEmpty()) {
			MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.COMMENT, comment);
		}

		String ownerJID = MRXMLUtils.getValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.AUTHOR_USER_ID, "");

		if (ownerJID.isEmpty()) {
			MRXMLUtils.setValueXpath(bookmarkObjectDoc, "//" + SessionServiceConst.AUTHOR_USER_ID, ownerUserJID);
		}

		Element metaDataEL = MRXMLUtils.getElementXpath(bookmarkObjectDoc, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME);

		if ((requstedMetaDataEL != null) && !requstedMetaDataEL.elements().isEmpty()) {
			MRXMLUtils.removeContent(metaDataEL);
			MRXMLUtils.addChildrenToElement(metaDataEL, requstedMetaDataEL);
		}

		return bookmarkObjectDoc;
	}



	public void sendFileNotification(MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory, String fileNID,
			String action, String mediaType) {
		//new Thread(new FileUpdateNotifier(request, xmpp, log, db, resourceFactory, profileFactory, fileNID, action, mediaType)).start();
		AppServerAgentUtils.getAppServerAgent(profileFactory).getThreadPoolManager().getNotificationFixedPool().execute(new FileUpdateNotifier(request, xmpp, log, db, resourceFactory, profileFactory, fileNID, action, mediaType));            
	}

	public class FileUpdateNotifier implements Runnable {

		MRRequest request;
		XMPPI xmpp;
		MRLog log;
		MRDB db;
		MRResourceFactoryI resourceFactory;
		ServiceAgentFactoryI serviceAgentFactory;
		String fileNID;
		String action;
		String mediatype;


		public FileUpdateNotifier(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
				ServiceAgentFactoryI serviceAgentFactory, String fileNID, String action, String mediatype ) {
			this.request = request;
			this.xmpp = xmpp;
			this.log = log;
			this.db = db;
			this.resourceFactory = resourceFactory;
			this.serviceAgentFactory = serviceAgentFactory;
			this.fileNID = fileNID;
			this.action = action; 
			this.mediatype = mediatype;
		}

		@Override
		public void run() {
			try {
				if(fileNID != null && !fileNID.isEmpty()) {
					addFileObjectNotificationsToQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fileNID, action, mediatype);
				}
			} catch (Exception e) {
				e.printStackTrace();
				return;
			}

		}

		private void addFileObjectNotificationsToQueue(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
				ServiceAgentFactoryI serviceAgentFactory, String whereNID, String action, String mediaType) throws Exception {

			Class<?> classObject;
			String className,requestName;
			Element requestEl;
			Document responseDoc;
			log.addInfo("FileUpdateNotifier: Starting the notifier thread for File updates");
					if(mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)){

						classObject = Class.forName(GetPVRFileRequestHandler.class.getName());
						className = classObject.getName();
						requestName = MRRequest.getRequestName(className);
						requestEl = MRXMLUtils.stringToElement("<GetPVRFile/>"); 

					}else{
						classObject = Class.forName(GetFileRequestHandler.class.getName());
						className = classObject.getName();
						requestName = MRRequest.getRequestName(className);
						requestEl = MRXMLUtils.stringToElement("<GetFile/>");
					}

					MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.WHERE_NID, whereNID);
					MRXMLUtils.setAttributeValue(requestEl, SessionServiceConst.SELECTFORMAT, SessionServiceConst.SELECTFORMAT_EXTENDEDURL);
					Document getRequestDoc = SessionServiceUtils.getRequestXML(requestName, log, requestEl);    
					MRRequest getRequest = MRRequest.createRequest(AppServerAgentUtils.getAppServerAgentJID(request.getUserJID(), serviceAgentFactory), getRequestDoc);
					getRequest.setSuperUser(true);
					if(mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)){
						responseDoc = new GetPVRFileRequestHandler().getResponse(getRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
					}else{
						responseDoc = new GetFileRequestHandler().getResponse(getRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
					}
					Document notificationDoc = MRXMLUtils.getDocumentXpath(responseDoc, "//File[@NID='"+whereNID+"']");

					if(notificationDoc != null) {
						String fileType = MRXMLUtils.getAttributeValueXpath(notificationDoc, "//File", "fileType", "");
						String title = MRXMLUtils.getAttributeValueXpath(notificationDoc, "//File", "title", "");
						if(fileType.equals(MediaRoomServiceConst.MEDIAGROUP)) { //remove FileList in case of mediagroup notification data.
							MRXMLUtils.removeNodeXpath(notificationDoc, "//FileList");
						}
						log.addInfo("FileUpdateNotifier: addFileObjectNotificationsToQueue: Attempting to add Notifications to Queue for "+fileType+" File title :" +title , MRLog.INFO, MRLog.NOTIFY);
						if(action.equals(SessionServiceConst.ADD)) {
							MRXMLUtils.setAttributeValueXPath(notificationDoc, "//File", "dirty", "true");
						}

						((AppServerAgent)(serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourceManager().queueObjectNotification(xmpp, getRequest, log, db, SessionServiceConst.FILE, SessionServiceConst.FILE, whereNID, action, notificationDoc);
					}
					log.addInfo("FileUpdateNotifier: End the notifier thread for File updates");
		}

	}
	protected Element getMediaStreamFileFromFileElement(Element fileEl, Document resourceDoc,Hashtable<String,Document> groupsFileCache,String mediaType) throws Exception{
		
		String mediaStoreFileID = MRXMLUtils.getAttributeValue(fileEl,SessionServiceConst.ID,"");

		Element mediaStreamFileEl = MRXMLUtils.getElementXpath(resourceDoc, "//MediaStreamFileResource[.//UUID='"+mediaStoreFileID+"']");

		if(mediaType.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaGroup.toString())) {
			String mediaGroupID = MRXMLUtils.getValueXpath(fileEl, ".//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");

			//check group exists in database                
			if(mediaStreamFileEl==null){
				mediaStreamFileEl = MRXMLUtils.getElementXpath(resourceDoc, "//GroupList/MediaStreamFileResource[.//MediaGroupID='"+mediaGroupID+"']");
			}

			//if db does not contain media group verify it exits in group cache. This is only true if group is added in the current run of sync process.
			if(groupsFileCache.containsKey(mediaGroupID)) {
				mediaStreamFileEl = groupsFileCache.get(mediaGroupID).getRootElement();
			}   
		}
		return mediaStreamFileEl;
	}
	private String getMediaURL(XMPPI xmpp, ServiceAgentFactoryI profileFactory, Element fileEl, Element mediaStreamFileEl, MRRequest request) throws Exception{
		AppServerAgent appServerAgent = AppServerAgentUtils.getAppServerAgent(profileFactory);
		Element ApiKeyTableEl = appServerAgent.getAPIKey();

		long now = xmpp.getCurrentTimeMillis();
		String expireTime = MRXMLUtils.getAttributeValue(fileEl, SessionServiceResourceConst.EXPIRE_TIME, "");
		String newExpireTime = expireTime;
		if(newExpireTime.isEmpty()) {
			newExpireTime = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.VALID_INTERVAL_ATTRIBUTE, String.valueOf(SessionServiceResourceConst.API_KEY_VALID_INTERVAL));
		}
		String timeStamp =String.valueOf(now + (Long.parseLong(newExpireTime) * 1000));
		String signature = HttpAuthentication.generateExpiryBasedSignature(ApiKeyTableEl, timeStamp);
		String apiKey = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.KEY_ATTRIBUTE, "");
		String msIPAddress = MRXMLUtils.getAttributeValueXpath(ApiKeyTableEl, "./" + SessionServiceResourceConst.API_KEY_ELEMENT_NAME, SessionServiceResourceConst.MS_IPADDRESS, "");

		String protocol = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//Header/Protocol", "http");
		String mediaURL = protocol+"://" + msIPAddress + "/controller/snapshot?mediaID=" + MRXMLUtils.getAttributeValue(mediaStreamFileEl, AMConst.NID, "") + "&timestamp=" + timeStamp + "&apiKey=" + apiKey + "&signature=" + signature;
		return mediaURL;
	}
}
