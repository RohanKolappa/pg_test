package com.barco.devicevng.MediaStore;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.SortedSet;
import java.util.UUID;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.common.DeviceDBI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.device.handler.MediaStore.MediaStoreServiceExceptionCodes;
import com.barco.device.handler.MediaStore.MediaStoreUtils;
import com.barco.device.vng.utils.CommandLineUtils;
import com.barco.devicevng.MediaStore.StoreServer.MediaStoreFileI;
import com.barco.devicevng.MediaStore.StoreServer.MediaStoreServer;
import com.barco.devicevng.MediaStore.StoreServer.MediaVersionManager;
import com.barco.devicevng.MediaStore.responsehandlers.PublishDirPurgeFileStatusResponseHandler;
import com.barco.devicevng.MediaStore.responsehandlers.PublishDirResponseHandler;
import com.barco.devicevng.MediaStore.responsehandlers.PublishDirSizeResponseHandler;
import com.barco.devicevng.MediaStore.responsehandlers.PublishFileResponseHandler;
import com.barco.utils.FileUtils;
import com.barco.utils.MediaRoomServiceExceptionCodes;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

public class DMSMediaStoreAgentImpl implements MediaStoreAgentI {

	DeviceFrameworkI deviceFramework = null;
	Hashtable<String,MediaStoreDirectoryInfo> mediaStoreCache = null;
	MediaStoreServer storeServer = null;
	DeviceDBI deviceDB = null;
	protected long deleterPortRunningNumber = 1000;
	protected Logger log = null;
	protected static final String TRASH_DIR_NAME = "trash";
	protected static final String MEDIA_TAR_EXTENSION = ".mkd";
	private static final String DB_TAR_EXTENSION = ".tar";
	private static final String CR_LF = "\r\n";
	ExecutorService thumbnailUploadService;// = Executors.newCachedThreadPool();
	@Override
	public void initAgent(DeviceFrameworkI deviceFramework) {
		this.deviceFramework = deviceFramework;
		deviceDB = deviceFramework.getDeviceDB();
		Document deviceXML = deviceFramework.getDeviceDB().getDeviceConfig();
		List<?> nasDriveList = null;
		List<?> internalDriveList = null;
		log=deviceFramework.getLogger();
		storeServer = new MediaStoreServer();		
		thumbnailUploadService = Executors.newCachedThreadPool();
		try {
			this.mediaStoreCache = new Hashtable<String,MediaStoreDirectoryInfo>();
			internalDriveList = XMLUtils.getListOfNodesXpath(deviceXML, "//HardDiskEntry");
			nasDriveList = XMLUtils.getListOfNodesXpath(deviceXML, "//NASTable/NASEntry");

			initializeMediaStores(internalDriveList, true);
			initializeMediaStores(nasDriveList, false);

			storeServer.addDeleterPort(String.valueOf(getDeleterPortRunningNumber()), "/data/mediadir/trash", deviceDB, log);
			
			//on each start sycn make sure we clean import/export Temporary area 
			reset();
			
			

		} catch (Exception e) {
			e.printStackTrace();
		}
	}


	private void initializeMediaStores(List<?> storeConfigList, boolean isInternal)
			throws  Exception {
		if ((storeConfigList == null) || storeConfigList.isEmpty()) {
			return;
		}

		Document diskStatusDoc = MediaStoreServiceUtils.getDiskStatusDoc(isInternal, deviceFramework.getLogger());

		for (Object object : storeConfigList) {
			initializeMediaStore((Element) object, diskStatusDoc, isInternal);
		}
	}



	private void initializeMediaStore(Element configEntryEl, Document diskStatusDoc, boolean isInternal)
			throws  Exception {
		String listIndex = XMLUtils.getAttributeValueWithDefault(configEntryEl, "ListIndex", "");
		String dirName = XMLUtils.getValueXpath(configEntryEl, ".//MediaStore/Name", "");

		Element statusEL = MediaStoreServiceUtils.getDiskEntry(dirName, diskStatusDoc);

		if (statusEL == null) {
			return;
		}

		if (!MediaStoreServiceUtils.checkDiskStatus(XMLUtils.elementToDocument(statusEL))) {
			return;
		}

		String dirPath = XMLUtils.getValueXpath(configEntryEl, "MediaStore/RootDirPath", "");

		if (!isInternal) {
			dirPath = dirPath + "/" + listIndex  +"/";
		} else {
			dirPath = dirPath  +"/"  +MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME + "/";
		}

		String readOnly = XMLUtils.getValueXpath(configEntryEl, "./ReadOnly", "");

		long size = Utils.getLongValue(XMLUtils.getValueXpath(statusEL, "./Parameter[@name='Total']", ""), 0);
		long sizeUsed = Utils.getLongValue(XMLUtils.getValueXpath(statusEL, "./Parameter[@name='Used']", ""), 0);

		long system_space_reservation = Utils.getLongValue(XMLUtils.getValueXpath(configEntryEl, ".//SystemReservation/@Size", ""), MediaStoreServiceConst.DISK_QUOTA_EXCEED_LIMIT);
		long thumbnail_space_reservation = 0;

		String xmppServerEnable = XMLUtils.getValueXpath(configEntryEl, "//XMPPServerEnable", "false");
		if("true".equals(xmppServerEnable))
			thumbnail_space_reservation = Utils.getLongValue(XMLUtils.getValueXpath(configEntryEl, ".//ThumbnailReservation/@Size", ""), MediaStoreServiceConst.DISK_QUOTA_EXCEED_LIMIT);

		long disk_quota_exceed_limit = system_space_reservation + thumbnail_space_reservation;

		deviceFramework.getLogger().info("Disk space Quota set as : " + disk_quota_exceed_limit);

		//create deleterport
		MediaStoreDirectoryInfo storeInfo = addMediaStoreToCache(dirPath, dirName, dirName, Utils.getBooleanValue(readOnly, false), size, sizeUsed, isInternal, disk_quota_exceed_limit);
		storeInfo.setDeleterPort(getDeleterPortRunningNumber());
		storeServer.addDeleterPort(String.valueOf(storeInfo.getDeleterPort()), storeInfo.getMediaStoreRootPath()+ "/" + TRASH_DIR_NAME, deviceDB, log);
		
		//Move purge files to trash directory 
		try {
			String cmd[] = {"/bin/mv", "-f",storeInfo.getMediaStoreRootPath()+MediaStoreDirectoryInfo.DirPurgeFileProcess.PURGEFILE_DIR_PATH , storeInfo.getMediaStoreRootPath()+ "/" + TRASH_DIR_NAME+"/"};
			CommandLineUtils.executeSystemCommand(cmd);
			deviceFramework.getLogger().debug("Move purge files to trash directory");
		} catch (Exception e) {
			e.printStackTrace();
		}
	}


	public MediaStoreDirectoryInfo addMediaStoreToCache(String rootPath, String storeName, String storeDeviceElementID, boolean readOnly, long availableDiskSpace, long usedDiskSpace, boolean isInternal, long disk_quota_exceed_limit) {

		MediaStoreDirectoryInfo storeDirInfo = new MediaStoreDirectoryInfo(storeName, rootPath, storeDeviceElementID, readOnly, availableDiskSpace, usedDiskSpace, isInternal, disk_quota_exceed_limit);
		try {
			storeDirInfo.setDeviceServiceVersion(deviceFramework.getServiceVersion());		
			storeServer.fillStoreInfo(storeDirInfo);
		} catch (Exception e) {
		}
		this.mediaStoreCache.put(storeDirInfo.getMediaStoreDeviceElementID(), storeDirInfo);		
		return storeDirInfo;

	}

	@Override
	public Document getPublishInfo() throws Exception {

		Document storeServicedoc = XMLUtils.stringToDocument("<"+DeviceAdminConst.MEDIASTORE_SERVICE_INFO+"/>");
		for(String store:this.mediaStoreCache.keySet()) {
			MediaStoreDirectoryInfo dirInfo = this.mediaStoreCache.get(store);
			Element dirElement = generateDirElement(dirInfo);		
			storeServicedoc.getRootElement().add(dirElement);
		}
		return storeServicedoc;
	}


	public Element generateDirElement(MediaStoreDirectoryInfo dirInfo) throws Exception {
		Element dirElement = DocumentHelper.createElement("Dir");

		XMLUtils.addAttribute(dirElement,"uuid", dirInfo.getMediaStoreUUID());	
		XMLUtils.addAttribute(dirElement, "id", dirInfo.getMediaStoreDeviceElementID());
		XMLUtils.addAttribute(dirElement, "mediaStoreElementId", dirInfo.getMediaStoreDeviceElementID());
		XMLUtils.addAttribute(dirElement, "mediaStoreRevision",String.valueOf(dirInfo.getDirRevision()));
		XMLUtils.addAttribute(dirElement, "mediaStoreServiceVersion", dirInfo.getMediaStoreServiceVer());
		XMLUtils.addAttribute(dirElement, "size", String.valueOf(dirInfo.getSize()));
		XMLUtils.addAttribute(dirElement, "sizeUsed", String.valueOf(dirInfo.getSizeUsed()));
		XMLUtils.addAttribute(dirElement, "readOnly", String.valueOf(dirInfo.isReadOnly()));
		XMLUtils.addAttribute(dirElement, "needsUpgrade", String.valueOf(!dirInfo.isWritable()));
		XMLUtils.addAttribute(dirElement, "internal", String.valueOf(dirInfo.isInternal()));
		XMLUtils.addAttribute(dirElement,"state",dirInfo.getMediaStoreState());

		String storeName = dirInfo.getMediaStoreName();
		if(dirInfo.isInternal()) {
			storeName = getDirTitle(storeName, deviceFramework.getLoginInfo().getUserName(), deviceFramework.getDeviceDB().getDeviceType());
		} 
		XMLUtils.addAttribute(dirElement, "mediaStoreName", storeName);	

		return dirElement;
	}

	private String getDirTitle(String storeName, String resourceTitle,String deviceType) throws Exception {
		if(deviceType.equals("DMS1500Device")){
			String index = storeName.substring(storeName.indexOf("/data"),"/data".length());
			if(index != null && !index.isEmpty()) {
				return resourceTitle+="_"+index;
			}
		}
		return resourceTitle;
	}

	@Override
	public Document getStatus(String xpath) throws Exception {
		return null;
	}

	@Override
	public void handleDirAvailable(String dirID) {

		try {

			//keep the mediastoreDirInfo in cache 

			Document deviceXML = deviceFramework.getDeviceDB().getDeviceConfig();
			Element  nasDriveEl = XMLUtils.getElementXpath(deviceXML, "//NASTable/NASEntry[.//Name='"+dirID+"']");
			Document diskStatusDoc = MediaStoreServiceUtils.getDiskStatusDoc(false, deviceFramework.getLogger());
			initializeMediaStore(nasDriveEl,diskStatusDoc,false); 

			//publish DirInfo to SC
			Element publishDirInfoEl = generateDirElement(getMediaStoredirectoryInfo(null, dirID));
			MediaStoreUtils.sendPublishDirRequest(publishDirInfoEl,deviceFramework, new PublishDirResponseHandler());
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

	}

	@Override
	public void handleDirUnavailable(String dirID) {

		try {
			MediaStoreDirectoryInfo dirInfo = getMediaStoredirectoryInfo("",dirID);
			Long deleterPortInstanceID =dirInfo.getDeleterPort();
			this.storeServer.deleteDeleterPort(String.valueOf(deleterPortInstanceID), deviceDB, log);
			//publish DirInfo to SC with state OFFLINE
			dirInfo.setMediaStoreState(MediaStoreServiceConst.OFFLINE);
			Element publishDirInfoEl = generateDirElement(getMediaStoredirectoryInfo(null, dirID));
			MediaStoreUtils.sendPublishDirRequest(publishDirInfoEl,deviceFramework,new PublishDirResponseHandler());
			//remove dirinfo form cache
			this.mediaStoreCache.remove(dirID);
		} catch (Exception e) {
			if(e.toString().contains("code='9307'")){
				deviceFramework.getLogger().info("Drop this mediaStoreEvent as no Dir Found for dirID "+dirID);   
			}else{
				e.printStackTrace();
			} 
		}

	}

	@Override
	public Document upgradeDir(String dirID, String dirNID) throws Exception {
		deviceFramework.getLogger().info("DMSMediaStoreAgent : upgradeDir : dirID = "+dirID +" dirNID = "+dirNID);
		MediaStoreDirectoryInfo mediaStoredirectoryInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		int storeRevision = mediaStoredirectoryInfo.getDirRevision();
		String serviceVersion = mediaStoredirectoryInfo.getDeviceServiceVersion();
		this.storeServer.updateServiceVersionAndRevesion(serviceVersion, storeRevision, mediaStoredirectoryInfo);
		mediaStoredirectoryInfo.setWritable(true);
		return XMLUtils.stringToDocument("<UpgradeDirResponse rev='"+storeRevision+"'/>");
	}

	@Override

	public Element upgradeFile(String dirId, String dirNID,String fileNID, String fileUUID, Document mediaStoreFileDoc) throws Exception {

		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirId);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(deviceFramework.getServiceVersion());;
		mediaStoreFile.upgradeFile(storeDirInfo, fileUUID, mediaStoreFileDoc, deviceFramework.getLogger());
		return MediaStoreUtils.generateFileElement(fileUUID, fileNID, dirNID, mediaStoreFileDoc);
	}

	@Override
	public void updateDiskSizeUsed(String dirID, long diskSizeUsed)
			throws Exception {
	}

	private MediaStoreDirectoryInfo getMediaStoredirectoryInfo(String dirNID,String dirID) throws Exception {

		MediaStoreDirectoryInfo storeDirInfo = null;
		if((dirID==null || dirID.isEmpty()) && dirNID!=null && !dirNID.isEmpty()) {
			for(String storeDirId:mediaStoreCache.keySet()) {
				MediaStoreDirectoryInfo dirInfo = mediaStoreCache.get(storeDirId);

				if(dirInfo.getMediaStoreDirNID().equals(dirNID) || dirInfo.getPvrDirNID().equals(dirNID) || dirInfo.getRecycleBinNID().equals(dirNID)) {
					storeDirInfo = dirInfo;
					break;
				}				
			}
		} else {
			storeDirInfo = mediaStoreCache.get(dirID);
		}

		if(storeDirInfo == null) {
			throw new ServiceException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NID_NOT_FOUND,"Dir Not Found");
		}

		return storeDirInfo;
	}

	@Override
	public Document addFile(String dirID, String dirNID, String fileUUID, Document mediaStoreFileDocument) throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);

		//check disk space
		storeServer.checkAvailableDiskSpace(storeDirInfo, deviceFramework.getLogger());

		Integer storeRevision = storeServer.getNewRevision(storeDirInfo);
		MediaStoreFileI storefile = MediaVersionManager.getMediaStoreFileI(deviceFramework.getServiceVersion());
		XMLUtils.addAttribute(mediaStoreFileDocument.getRootElement(), MediaStoreServiceConst.REVISION, String.valueOf(storeRevision));
		storefile.createMediaStoreFile(storeDirInfo, fileUUID, mediaStoreFileDocument, deviceFramework.getLogger());


		Document resultDoc = DocumentHelper.createDocument();

		Element fileEl =  DocumentHelper.createElement("File");
		resultDoc.add(fileEl);
		XMLUtils.addAttribute(fileEl, "id", fileUUID);
		XMLUtils.addAttribute(fileEl, "dirNID", storeDirInfo.getMediaStoreDirNID());
		String fileType = XMLUtils.getValueXpath(mediaStoreFileDocument, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
		if(fileType.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.PVRMediaClip.toString())) {
			XMLUtils.addAttribute(fileEl, "dirNID", storeDirInfo.getPvrDirNID());
		} 
		fileEl.add(mediaStoreFileDocument.getRootElement().detach());
		//return mediaStoreFileDocument;
		return XMLUtils.elementToDocument(fileEl);

	}

	@Override
	public Document setFileRequest(String dirID, String dirNID, String fileNID, String fileUUID, Document mediaStoreFileDocument) throws Exception {

		MediaStoreDirectoryInfo dirInfo = getMediaStoredirectoryInfo(dirNID, dirID);

		int revision = storeServer.getNewRevision(dirInfo);
		String storeVersion = storeServer.getVersionOfMediaStoreFile(dirInfo, fileUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		mediaStoreFile.updateMediaStoreFile(dirInfo, fileUUID, mediaStoreFileDocument, String.valueOf(revision), deviceFramework.getLogger());

		return mediaStoreFileDocument;
	}


	private Document ingestSnapshot(String filePath, String currentTime, String dirID, String dirNID, String mediaStoreFileNID, Document mediaStoreFileDoc) throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storePath = storeDirInfo.getMediaStoreRootPath() +MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR +"/" + mediaStoreFileNID +"/";
		Document resultDoc = addFile(dirID, dirNID, mediaStoreFileNID, mediaStoreFileDoc);
		//find file path
		int pos = filePath.lastIndexOf('.');
		String extension = filePath.substring(pos);
		if(extension.equals(".jpeg")) {
			extension = ".jpg";
		} else if(extension.equals(".tiff")) {
			extension = ".tif";
		}
		String snapshotFilePath = storePath+MediaStoreServiceConst.SNAPSHOT_FILE_NAME+extension;

		MediaStoreFileI storefile = MediaVersionManager.getMediaStoreFileI(deviceFramework.getServiceVersion());
		storefile.createSnapshot(storePath, currentTime, filePath, snapshotFilePath, extension);
		Element mediaStoreFileEl = XMLUtils.getElementXpath(resultDoc, "//MediaStoreFile");
		XMLUtils.setValueXpathElement(mediaStoreFileEl, ".//State", MediaStoreServiceConst.READY);
		setFileRequest(dirID, dirNID, "", mediaStoreFileNID, XMLUtils.elementToDocument(mediaStoreFileEl));
		long fileSize = getFileSize(dirID, dirNID, mediaStoreFileNID);
		XMLUtils.addAttribute(resultDoc.getRootElement(), "size", String.valueOf(fileSize));
		return resultDoc;
	}

	@Override
	public void setFilePlaylengthAndSize(String dirID, String dirNID,
			String fileUUID, String state, long size, long playlength) throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = this.storeServer.getVersionOfMediaStoreFile(storeDirInfo, fileUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		Integer newRevision = storeServer.getNewRevision(storeDirInfo);
		Document mediaStoreFileDoc = mediaStoreFile.getMediaStoreFile(storeDirInfo, fileUUID);		
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.PLAY_LENGTH, String.valueOf(playlength));		
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.STATE_EL_NAME, state);
		deviceFramework.getLogger().info("setFilePlaylengthAndSize: Updated playlegth and state: "+playlength+":: "+state+":: fileUUID: "+fileUUID);
		mediaStoreFile.updateMediaStoreFile(storeDirInfo, fileUUID, mediaStoreFileDoc, String.valueOf(newRevision), deviceFramework.getLogger());

		//publish update to sc
		String fileType = XMLUtils.getValueXpath(mediaStoreFileDoc, "//Type", "");

		sendPublishFileRequest(storeDirInfo, new ArrayList<Element>(), 1, new ArrayList<String> (), fileUUID, mediaStoreFile, 
				storeDirInfo.getMediaStoreDirNID(), fileType, storeDirInfo.getPvrDirNID(), state, storeDirInfo.getRecycleBinNID(), mediaStoreFileDoc,true, size, null,false,null);

	}	

	@Override
	public void deleteFile(String dirID, String dirNID, String fileNID, String fileUUID, boolean purgeFile)
			throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = this.storeServer.getVersionOfMediaStoreFile(storeDirInfo, fileUUID);		
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		mediaStoreFile.deleteFile(storeDirInfo, fileUUID, TRASH_DIR_NAME, deviceFramework.getLogger());
	}

	@Override
	public Document getMediaStoreFileDocument(String dirID, String dirNID, String fileUUID) throws Exception {
		deviceFramework.getLogger().info("DirID="+ dirID + "and fileUUID: "+fileUUID);
		MediaStoreDirectoryInfo dirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		if(dirInfo == null) {
			return null;
		}
		return getMediaStoreFileDocument(dirInfo, fileUUID);
	}

	public Document getMediaStoreFileDocument(MediaStoreDirectoryInfo dirInfo,String fileUUID) throws Exception {
		String storeVersion = storeServer.getVersionOfMediaStoreFile(dirInfo, fileUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		return mediaStoreFile.getMediaStoreFile(dirInfo, fileUUID);
	}


	@Override
	public Document getMediaStoreBookmarkDocument(String dirID, String dirNID, String fileUUID, String bookkmarUUID) throws Exception {
		deviceFramework.getLogger().info("DirID="+ dirID + "and fileUUID: "+fileUUID + "and bookmarkUUID: "+bookkmarUUID);
		MediaStoreDirectoryInfo dirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		if(dirInfo == null) {
			return null;
		}
		return getMediaStoreBookmarkDocument(dirInfo, fileUUID, bookkmarUUID);
	}

	public Document getMediaStoreBookmarkDocument(MediaStoreDirectoryInfo dirInfo,String fileUUID, String bookkmarUUID) throws Exception {
		String storeVersion = storeServer.getVersionOfMediaStoreFile(dirInfo, fileUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		return mediaStoreFile.getMediaStoreBookmark(dirInfo, fileUUID, bookkmarUUID);
	}

	@Override
	public boolean isMediaStoreBookmarkExist(String dirID, String dirNID,String mediaStoreFileID, String bookmarkID) throws Exception{
		MediaStoreDirectoryInfo dirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		if(dirInfo == null) {
			return false;
		}
		String storeVersion = storeServer.getVersionOfMediaStoreFile(dirInfo, mediaStoreFileID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		return mediaStoreFile.isMediaStoreBookmarkExist(dirInfo, mediaStoreFileID, bookmarkID);

	}

	@Override
	public void startSynch(String dirId, String mediaStreamFileMaxRevision, String dirNID,
			String pvrDirNID, String recyclebinDirNID, Integer index ) throws Exception {
		
		MediaStoreDirectoryInfo dirInfo = getMediaStoredirectoryInfo(dirNID, dirId);		
		dirInfo.setMediaStoreDirNID(dirNID);
		dirInfo.setPvrDirNID(pvrDirNID);
		dirInfo.setRecycleBinNID(recyclebinDirNID);		
		//Clean Temporary area 
		reset();
		List<String> listOfDirUUIDs = dirInfo.getDirUUIDs();
		
		if(listOfDirUUIDs==null){
			String[] listOfMedia = storeServer.getListOfFiles(dirInfo);
			if(listOfMedia == null || listOfMedia.length == 0) {		    	
				dirInfo.setMediaStoreState(MediaStoreServiceConst.READY);
			}			
			listOfDirUUIDs =Arrays.asList(listOfMedia);
			Collections.sort(listOfDirUUIDs);
			dirInfo.setDirUUIDs(listOfDirUUIDs); 
		}
		//deviceFramework.getLogger().info("listOfDirUUIDs: "+listOfDirUUIDs);
		int startIndex = index*10;
		int endIndex = startIndex+9;
		int publishSize = 10;
		if(endIndex >=listOfDirUUIDs.size()){
			endIndex =listOfDirUUIDs.size()-1;
			publishSize = (endIndex - startIndex)+1;
			index=-1;
		}
		int noofFilesSynched = 0;
		deviceFramework.getLogger().info("startIndex is: "+startIndex+" :: endIndex is :"+endIndex+" :: publishSize: "+publishSize+":: noofFilesSynched: "+noofFilesSynched);
		int maxRevision = Utils.getIntValue(mediaStreamFileMaxRevision, 0);		
		List<Element> publishList = new ArrayList<Element>();
		
		Element callbackInfo = XMLUtils.stringToElement("<SyncCallBack />");
		XMLUtils.addAttribute(callbackInfo, "index", String.valueOf(index+1));
		XMLUtils.addAttribute(callbackInfo, "dirID",dirId);
		XMLUtils.addAttribute(callbackInfo, "dirNID",dirNID);
		XMLUtils.addAttribute(callbackInfo, "pvrDirNID",pvrDirNID);
		XMLUtils.addAttribute(callbackInfo, "recyclebinDirNID",recyclebinDirNID);		
		XMLUtils.addAttribute(callbackInfo, "mediaStreamFileMaxRevision",mediaStreamFileMaxRevision);
		
		String clientData = URLEncoder.encode(callbackInfo.asXML(), "UTF-8");
		for(int i = startIndex;i <= endIndex || i==listOfDirUUIDs.size()-1;i++ ) {
			String fileUUID = listOfDirUUIDs.get(i);
			try {
				String storeVersion = storeServer.getVersionOfMediaStoreFile(dirInfo, fileUUID);
				MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
				Document mediaStoreFileDoc =   mediaStoreFile.getMediaStoreFile(dirInfo, fileUUID);
				String fileType = XMLUtils.getValueXpath(mediaStoreFileDoc, "//Type", "");
				String state = XMLUtils.getValueXpath(mediaStoreFileDoc, "//State", "");			
				int storeRevision =  Utils.getIntValue(XMLUtils.getAttributeValueWithDefault(mediaStoreFileDoc.getRootElement(), MediaStoreServiceConst.REVISION,""),
						MediaStoreServiceConst.DEFAULT_REVISION);

				//deviceFramework.getLogger().info("fileUUID: "+fileUUID+" :: fileType : "+fileType+" :: state: "+state);
				if(state.equals(MediaStoreServiceConst.BUSY) && !fileType.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaGroup.toString())) {
					mediaStoreFileDoc=mediaStoreFile.repairMediaStoreFile(dirInfo, fileUUID, mediaStoreFileDoc, String.valueOf(dirInfo.getDirRevision()),  deviceFramework.getLogger());
					noofFilesSynched++;					
					
					sendPublishFileRequest(dirInfo, publishList,publishSize, listOfDirUUIDs,fileUUID, mediaStoreFile, dirNID, fileType, pvrDirNID, state, recyclebinDirNID, mediaStoreFileDoc,false,
							mediaStoreFile.getMediaStoreFileSize(dirInfo, fileUUID, deviceFramework.getLogger()), null, true,clientData);					

				}
				else if(!dirInfo.isWritable() || storeRevision > maxRevision) { //publish store file information to SC
					HashMap<String, Element> listOfBookmark =  null;
					if(dirInfo.isWritable()) {	    
						mediaStoreFile.updateMediaStoreFile(dirInfo, fileUUID, mediaStoreFileDoc, String.valueOf(dirInfo.getDirRevision()), deviceFramework.getLogger());
						try{
						listOfBookmark = mediaStoreFile.getListOfBookmarks(dirInfo, fileUUID);
						}catch(Exception e){
							deviceFramework.getLogger().error("DMSMediaStoreAgent:StartSynch: Problem getting bookmarks :"+fileUUID, e);	
						}
					}
					noofFilesSynched++;
				
					sendPublishFileRequest(dirInfo, publishList, publishSize, listOfDirUUIDs,fileUUID, mediaStoreFile, dirNID, fileType, pvrDirNID, state, recyclebinDirNID, mediaStoreFileDoc,false,
							mediaStoreFile.getMediaStoreFileSize(dirInfo, fileUUID, deviceFramework.getLogger()), listOfBookmark, true, clientData);
					
				}
			} catch (Exception e) {
				deviceFramework.getLogger().error("DMSMediaStoreAgent:StartSynch: Problem to Synch File :"+fileUUID, e);				
				noofFilesSynched++;
			}
			

		}
	


		if(!publishList.isEmpty()) {
			deviceFramework.getLogger().info("DMSMediaStoreAgent: publishList is not empty: noofFilesSynched: "+noofFilesSynched);			
			publishFileRequest(publishList,clientData);
		}
		//no files are there to sycn update stream file revision in store blob
		if(noofFilesSynched == 0 && endIndex == listOfDirUUIDs.size()-1) {
			storeServer.updateRevision(maxRevision, dirInfo);
			deviceFramework.getLogger().info("DMSMediaStoreAgent: No files found to sycn. Update stream revision in store file");
		}
		else if(noofFilesSynched == 0 && publishList.isEmpty() && index!=-1)//no files files published in this iteration, check next files
		{
			deviceFramework.getLogger().info("DMSMediaStoreAgent: No files found to sycn. calling startSync for next lot");
			startSynch(dirId, mediaStreamFileMaxRevision, dirNID, pvrDirNID, recyclebinDirNID, index+1);
		}
		dirInfo.setMediaStoreState(MediaStoreServiceConst.READY);
	}

	@Override
	public void updateDBInformation(String dirId, String dirNID, String pvrDirNID, String recycleBinNID, Long dirThreshold, String state) 	throws Exception {
		MediaStoreDirectoryInfo dirInfo = getMediaStoredirectoryInfo(dirNID, dirId);

		if(dirNID.isEmpty() || pvrDirNID.isEmpty() || recycleBinNID.isEmpty()) {
			new Exception().printStackTrace();
			throw new ServiceException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NID_NOT_FOUND, "Dir/PVR/Recycle Bin Dir NID is Missing. "+ dirNID +" "+pvrDirNID+" "+recycleBinNID);
		}

		dirInfo.setMediaStoreDirNID(dirNID);
		dirInfo.setPvrDirNID(pvrDirNID);
		dirInfo.setRecycleBinNID(recycleBinNID);
		if(dirThreshold !=0) {
			dirInfo.setDiskFullThreshold(dirThreshold);
		}
		dirInfo.setMediaStoreState(state);
	}

	@Override
	public String getDirPath(String dirNID, String dirID) throws Exception {
		return getMediaStoredirectoryInfo(dirNID, dirID).getMediaStoreRootPath();
	}

	public long getDeleterPortRunningNumber() {
		return deleterPortRunningNumber++;
	}

    private Element createFileElementForPublish( String mediaStoreFileID, String dirNID, String fileType, String pvrDirNID, String state, 
    		String recyclebinDirNID, Document mediaStoreFileDoc, boolean appendSize, long size, HashMap<String, Element> listOfBookmark,boolean dirSync) throws Exception {
    	Element fileEl = DocumentHelper.createElement("File");

    	XMLUtils.addAttribute(fileEl, "id", mediaStoreFileID);
		XMLUtils.addAttribute(fileEl, "size", String.valueOf(size));
		XMLUtils.addAttribute(fileEl, "dirNID", dirNID);
		XMLUtils.addAttribute(fileEl, "appendSize", String.valueOf(appendSize));
		XMLUtils.addAttribute(fileEl, "dirSync", String.valueOf(dirSync));
		if (fileType.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.PVRMediaClip.toString())) {
			XMLUtils.addAttribute(fileEl, "dirNID", pvrDirNID);
		}

		if (state.equals(MediaStoreServiceConst.DELETED)) {
			XMLUtils.addAttribute(fileEl, "dirNID", recyclebinDirNID);
		}

		fileEl.add(mediaStoreFileDoc.getRootElement().detach());

		// Add Bookmarks
		if (listOfBookmark != null && listOfBookmark.size() > 0) {
			Element bookmarksEl = DocumentHelper.createElement("Bookmarks");
			for (Map.Entry<String, Element> entry : listOfBookmark.entrySet()) {
				Element bookmark = entry.getValue();
				bookmarksEl.add(bookmark);
			}
			fileEl.add(bookmarksEl);
		}
    	
		return fileEl;
    }
    
	// publish store file information to SC
	private void sendPublishFileRequest(MediaStoreDirectoryInfo dirInfo,
			List<Element> publishList, int publishCount, List<String> listOfMediaStoreFileIDs,
			String mediaStoreFileID, MediaStoreFileI mediaStoreFile,
			String dirNID, String fileType, String pvrDirNID, String state,
			String recyclebinDirNID, Document mediaStoreFileDoc,
			boolean appendSize, long size,

			HashMap<String, Element> listOfBookmark,boolean dirSync, String clientData) throws Exception {

		Element fileEl = createFileElementForPublish(mediaStoreFileID, dirNID,
				fileType, pvrDirNID, state, recyclebinDirNID,
				mediaStoreFileDoc, appendSize, size, listOfBookmark,dirSync);

		createDownloadTagForFormat(dirInfo, mediaStoreFileID, fileEl);
		publishList.add(fileEl);
		deviceFramework.getLogger().debug(" sendPublishFileRequest: Count is: "+publishCount+ ":: Publis list size is" +publishList.size() );
		if (publishList.size() == publishCount || publishList.size() == listOfMediaStoreFileIDs.size()) { // publish 10 files at a time
			deviceFramework.getLogger().debug("Calling publishFileRequest: Count is: "+publishCount+ ":: Publis list size is" +publishList.size() );
			publishFileRequest(publishList, clientData);
		}
	}

	private void publishFileRequest(List<Element> publishList, String clientData) throws Exception {		
		Document publishFileRequestDoc = XMLUtils.loadXMLResource("/com/ipvs/assetadminservice/file/xml/PublishFileRequest.xml");
		if(clientData!=null && !clientData.isEmpty())
			XMLUtils.setValueXpath(publishFileRequestDoc, "//ClientData", clientData);
		Element publishFileRequestDataEl = XMLUtils.getElementXpath(publishFileRequestDoc, "//PublishFileRequestData");
		for(Element publishEl:publishList) {
			XMLUtils.addAttribute(publishEl, "eventClockTime", String.valueOf(System.currentTimeMillis()));
			publishFileRequestDataEl.add(publishEl);
		}
		Message publishRequestMessage =  Message.createRequest(publishFileRequestDoc, deviceFramework.getAppserverJID() , deviceFramework.getMyJID());		
		deviceFramework.sendRequest(publishRequestMessage, publishFileRequestDoc, new PublishFileResponseHandler());
		publishList.clear();
	}

	@Override
	public String getDirIDFromDirPath(String dirPath) throws Exception {
		for(String storeDirId:mediaStoreCache.keySet()) {
			MediaStoreDirectoryInfo dirInfo = mediaStoreCache.get(storeDirId);
			deviceFramework.getLogger().debug("dirPath:="+dirPath +" cachepath:="+dirInfo.getMediaStoreRootPath());
			if(dirInfo.getMediaStoreRootPath().equals(dirPath)) {
				return dirInfo.getMediaStoreDeviceElementID();
			}				
		}
		throw new ServiceException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NID_NOT_FOUND,"Dir Not Found");
	}

    @Override
    public void prepareFileForDownload(String fileNID, String fileUUID,
	    String dirNID, String mediaFormat, String startOffset,
	    String endOffset, String requestTimeStamp, String type, String mediaGroupPlaylength, String new_fileUUID) throws Exception {

	String rootFolderPath = getDirPath(dirNID, null);
	String tempFolderPath = "";
	String new_UUID = new_fileUUID;

	MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, "");
	String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, fileUUID);
	MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
	// get blob size and check for available disk space
	long blobsize = mediaStoreFile.getMediaStoreFileSize(storeDirInfo, fileUUID, deviceFramework.getLogger());
	storeServer.checkAvailableDiskSpaceForPrepareDownload(blobsize, storeDirInfo, deviceFramework.getLogger());

	// Pack the media file
	if(new_UUID.isEmpty())
		new_UUID = Utils.getUUID(); // generate a new UUID for the file
	
	tempFolderPath = getDMSWorkAreaPath(new_UUID, rootFolderPath);

	Document mediaStoreFileDoc = getMediaStoreFileDocument(storeDirInfo, fileUUID);
	String mediaType = XMLUtils.getValueXpath(mediaStoreFileDoc, "//Type", "");
	Element fileEl =  DocumentHelper.createElement("File");
	XMLUtils.addAttribute(fileEl, "size", String.valueOf(blobsize));		
	XMLUtils.addAttribute(fileEl, "dirNID", dirNID);
	XMLUtils.addAttribute(fileEl, "appendSize", String.valueOf(false));
	fileEl.add(mediaStoreFileDoc.getRootElement().detach());

	if(mediaType.equalsIgnoreCase("MediaGroup")){
		// Media Group Handling
		packMediaGroup(storeDirInfo,mediaStoreFile,fileNID, fileUUID, new_UUID, dirNID, rootFolderPath, tempFolderPath, mediaFormat, startOffset, endOffset, requestTimeStamp, fileEl, type, mediaGroupPlaylength);
	}else{
		packMediaFile(storeDirInfo,mediaStoreFile,fileNID, fileUUID, new_UUID, dirNID, rootFolderPath, tempFolderPath, mediaFormat, startOffset, endOffset,
				requestTimeStamp, fileEl, type);
	}

	}

	public void packMediaFile(MediaStoreDirectoryInfo dirInfo,MediaStoreFileI mediaStoreFile, String fileNID, String fileUUID, String new_UUID,
			String dirNID, String rootFolderPath, String tempFolderPath,
			String mediaFormat, String startOffset, String endOffset,
			String requestTimeStamp, Element fileEl, String requestType) throws Exception {
		log.info("INSIDE MEDIA FILE :::::::::::::::::");
		String mediaFolderPath = rootFolderPath + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + fileUUID; 
		Document dbXMLdoc = XMLUtils.fileToDocument(mediaFolderPath+"/db.xml");
		String title = XMLUtils.getValueXpath(dbXMLdoc, "//MediaStoreFile/Title", "");
		String mediatype = XMLUtils.getValueXpath(dbXMLdoc, "//MediaStoreFile/Type", "");
		String streamType = mediatype.equals(MediaStoreServiceConst.MEDIA_IMAGE_TYPE) ? MediaStoreServiceConst.SNAPSHOT : XMLUtils.getValueXpath(dbXMLdoc, "//StreamType", "");
		String new_fileNID = "msfrl_" + Utils.getUUID(); // generate new file NID for export
		if(mediaFormat == null || mediaFormat.isEmpty()) {
			mediaFormat = "Proprietary";
		}

		String tempFilePath = getDMSFilePath_prepare_proprietary(tempFolderPath, new_UUID);
		String prepare_file_folder = tempFolderPath+"/"+ new_UUID;

		MediaStoreServiceUtils.writeIndexFile(prepare_file_folder,dirNID, new_fileNID, new_UUID, streamType, title, requestTimeStamp);
		MediaStoreServiceUtils.writeMediaFile(prepare_file_folder+"/db/IPVSc_default/", new_fileNID, fileEl.asXML());

		// Create SDP template
		String sdpfilePath = tempFolderPath+"/"+ new_UUID+"/"+ new_UUID;
		createSDPTemplate(mediaFolderPath, sdpfilePath);

		// Mediafile name for native format prepare
		String standardMediaFileName = title + "_" + new_UUID;
		standardMediaFileName = standardMediaFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_"); // replace any special characters with '_'
		sdpfilePath = sdpfilePath + ".sdp";
		String size = XMLUtils.getAttributeValue(fileEl, "size");
		log.info("File is calling scripts params in order "+ "mediafile_prepare" + "2- " + tempFilePath + "3-" + mediaFolderPath + "4- " + prepare_file_folder
				+ "5- " + new_fileNID + "6- " + startOffset + "7- " +endOffset + "8- " + requestTimeStamp + "9- " + mediaFormat
				+ "10- " + streamType + "11- " +sdpfilePath+ "12- " +standardMediaFileName+ "13- " +dirNID+ "14- " +fileUUID + "15- " +requestType+ "16- "+size+ "17- "+new_UUID);

		String[] cmd = { "/etc/scripts/mediafile_conversion_utils.sh", "mediafile_prepare",
				tempFilePath, mediaFolderPath,
				prepare_file_folder, new_fileNID,
				startOffset, endOffset, requestTimeStamp, mediaFormat,
				streamType, sdpfilePath, standardMediaFileName, dirNID, fileUUID, requestType, size, new_UUID, "&" };


		int retVal = 0;
		StringBuffer buf = new StringBuffer();
		retVal = CommandLineUtils.executeProcessInThread(cmd, buf, log);
		if(retVal != 0) {
			String error = "Error in preparing MediaFile: " + tempFilePath + " " + buf.toString();
			deviceFramework.getLogger().error(error);
			throw new Exception(error);
		}else{			
			mediaStoreFile.deleteStaleFile(dirInfo, fileUUID, mediaFormat.toLowerCase());
		}

	}	

	public void packMediaGroup(MediaStoreDirectoryInfo dirInfo,MediaStoreFileI mediaStoreFile, String fileNID, String fileUUID, String new_UUID,
			String dirNID, String rootFolderPath, String tempFolderPath,
			String mediaFormat, String startOffset, String endOffset,
			String requestTimeStamp, Element fileEl, String requestType, String mediaGroupPlaylength) throws Exception {
		log.info("INSIDE MEDIA GROUP :::::::::::::::::");
		String mediaFolderPath = rootFolderPath + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + fileUUID; 
		Document dbXMLdoc = XMLUtils.fileToDocument(mediaFolderPath+"/db.xml");

		String title = XMLUtils.getValueXpath(dbXMLdoc, "//MediaStoreFile/Title", "");
		String streamType = XMLUtils.getValueXpath(dbXMLdoc, "//StreamType", "");
		String new_fileNID = "msfrl_" + Utils.getUUID(); // generate new file NID for export
		if(mediaFormat == null || mediaFormat.isEmpty())
			mediaFormat = "Proprietary";

		String tempFilePath = getDMSFilePath_prepareMediaGroup_proprietary(tempFolderPath, new_UUID);
		String prepare_file_folder = tempFolderPath+"/"+ new_UUID;

		String standardMediaFileName = title + "_" + new_UUID;
		standardMediaFileName = standardMediaFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_"); // replace any special characters with '_'

		String size = XMLUtils.getAttributeValue(fileEl, "size");

		log.info("Group function is calling scripts params in order "+ "mediaGroup_prepare" + "2- " + tempFilePath + "3-" + mediaFolderPath + "4- " + prepare_file_folder
				+ "5- " + new_fileNID + "6- " + startOffset + "7- " +endOffset + "8- " + requestTimeStamp + "9- " + mediaFormat
				+ "10- " + streamType + "11- " +"blankparam"+ "12- " +standardMediaFileName+ "13- " +dirNID+ "14- " +fileUUID + "15- " +"EXPORT" + "16- "+size + "17- "+mediaGroupPlaylength);	

		MediaStoreServiceUtils.writeIndexFile(prepare_file_folder,dirNID, new_fileNID, new_UUID, streamType, title, requestTimeStamp);
		MediaStoreServiceUtils.writeMediaFile(prepare_file_folder+"/db/IPVSc_default/", new_fileNID, fileEl.asXML());

		String[] cmd = { "/etc/scripts/mediafile_conversion_utils.sh", "mediaGroup_prepare",
				tempFilePath, mediaFolderPath,
				prepare_file_folder, new_fileNID,
				startOffset, endOffset, requestTimeStamp, mediaFormat,
				streamType, "", standardMediaFileName, dirNID, fileUUID, requestType, size, mediaGroupPlaylength, "&" };

		int retVal = 0;
		StringBuffer sb = new StringBuffer();
		retVal = CommandLineUtils.executeProcessInThread(cmd, sb, log);
		if(retVal != 0) {
			String error = "Error in preparing MediaGroup: " + tempFilePath + " " + sb.toString();
			deviceFramework.getLogger().error(error);
			throw new Exception(error);
		}else{			
			mediaStoreFile.deleteStaleFile(dirInfo, fileUUID, mediaFormat.toLowerCase());			
		}

	}

	/*
	 *  ----- Export/Import/Offload Utility Functions start------------------
	 */

	public String getDMSFilePath_prepare_proprietary(String tempFolderPath, String UUID) {
		return tempFolderPath + "/" + UUID + "/" + UUID + ".mkd";
	}

	public String getDMSFilePath_prepare_standard(String tempFolderPath, String UUID) {
		return tempFolderPath + "/" + UUID + "/" + "standard" + ".zip";
	}

	public String getDMSFilePath_prepareMediaGroup_proprietary(String tempFolderPath, String UUID) {
		return tempFolderPath + "/" + UUID + "/" + UUID + ".grp";
	}

	public String getDMSWorkAreaPath(String fileUUID, String rootFolderPath) throws Exception {
		File workDir = null;
		workDir = new File(getDMSWorkAreaPath_Prepare(rootFolderPath));
		File workDir_uuid = new File(workDir, fileUUID);
		workDir_uuid.mkdirs();
		return workDir.getAbsolutePath();
	}

	public String getDMSWorkAreaPath_Prepare(String rootFolderPath)
			throws Exception {
		File workDir = new File(MediaStoreUtils.PREPARE_TEMP_FOLDER_PATH);
		workDir.mkdirs();
		return workDir.getAbsolutePath();
	}

	/*
	 *  ----- Export/Import/Offload Utility Functions end ------------------
	 */

	@Override
	public void deleteBookmark(String dirID, String dirNID, String mediaUUID, String bookmarkUUID)throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = this.storeServer.getVersionOfMediaStoreFile(storeDirInfo, mediaUUID);		
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		mediaStoreFile.deleteBookmark(storeDirInfo, bookmarkUUID, mediaUUID, deviceFramework.getLogger());
	}

	@Override
	public Document addBookmark(String dirID, String dirNID, String mediaUUID, String bookmarkUUID, String clipUUID,Document bookmarkFileDoc) throws Exception {
		// TODO Auto-generated method stub
		long timecode = Utils.getLongValue(XMLUtils.getValueXpath(bookmarkFileDoc,"//AtTC",""),0);  
		long offSet = Utils.getLongValue(XMLUtils.getValueXpath(bookmarkFileDoc,"//Offset",""),0);
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, mediaUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);

		//clipUUID is empty in case when there is no media group, bookmark need to be added on a clip file 
		if(clipUUID.isEmpty()) {
			clipUUID=mediaUUID;
		}

		if(timecode==0 && offSet==0){
			throw new ServiceException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, "Both offset and TC can't be 0");
		}else if(timecode==0){	
			timecode=	mediaStoreFile.getTCAtOffset(clipUUID, offSet, storeDirInfo, deviceDB,deviceFramework.getLogger());
		}else if(offSet==0){
			offSet=mediaStoreFile.getOffsetAtTC(timecode, clipUUID, storeDirInfo, deviceDB,deviceFramework.getLogger());
		} 


		XMLUtils.setValueXpath(bookmarkFileDoc,"//"+ MediaStoreServiceConst.TC,String.valueOf(timecode));
		XMLUtils.setValueXpath(bookmarkFileDoc,"//"+ MediaStoreServiceConst.OFFSET,String.valueOf(offSet));

		mediaStoreFile.createBookmarkInMediaStore(storeDirInfo, mediaUUID,bookmarkUUID,bookmarkFileDoc,deviceFramework.getLogger());


		return bookmarkFileDoc;
	}


	@Override
	public Document updateBookmark(String dirID, String dirNID, String mediaUUID, String bookmarkUUID, Document mediaStoreBookmarkDocument, String strTimecode, String strOffSet, String synchClipId) throws Exception {
		// TODO Auto-generated method stub
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, mediaUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		long timecode = Utils.getLongValue(strTimecode,0);  
		long offSet = Utils.getLongValue(strOffSet,0);

		if(synchClipId.isEmpty()) {
			synchClipId=mediaUUID;
		}

		if(timecode==0 && offSet==0){
			throw new ServiceException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, "Both offset and TC can't be 0");
		}else if(timecode==0){	
			timecode=	mediaStoreFile.getTCAtOffset(synchClipId, offSet, storeDirInfo, deviceDB,deviceFramework.getLogger());
		}else if(offSet==0){
			offSet=mediaStoreFile.getOffsetAtTC(timecode, synchClipId, storeDirInfo, deviceDB,deviceFramework.getLogger());
		} 

		XMLUtils.setValueXpath(mediaStoreBookmarkDocument,"//"+ MediaStoreServiceConst.TC,String.valueOf(timecode));
		XMLUtils.setValueXpath(mediaStoreBookmarkDocument,"//"+ MediaStoreServiceConst.OFFSET,String.valueOf(offSet));
		mediaStoreFile.updateMediaStoreBookmark(storeDirInfo, mediaUUID,bookmarkUUID,mediaStoreBookmarkDocument,deviceFramework.getLogger());			

		return mediaStoreBookmarkDocument;
	}


	@Override
	public void unpackImportFile(String dirID, String dirNID, String mediaUUID, String dateModified,String tempFolderPath, String queueID, String groupID, String groupNID, String isOldMediaWithGroup, String mediaGroupTitle) throws Exception {
		try{
			String rootFolderPath = getDirPath(dirNID, dirID);
			MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
			String groupFileUUID="";
			boolean isGroupCreated = true;
			if(("true").equals(isOldMediaWithGroup)){ //4.0 or older media associated with group			
				//check whether the group is already created based on groupID
				if(!MediaStoreUtils.isMediaGroupFilePresent(groupID, rootFolderPath+ MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR, mediaUUID)){ // media group not already present			
					// need to create mediagroup and get bookmarks belonging to mediagroup				    
					groupFileUUID = UUID.randomUUID().toString(); 
					createMediaGroupStoreFile(dirID, dirNID, groupFileUUID, mediaGroupTitle, groupID, "MediaGroup");
					isGroupCreated = false;
				}
			}		   
			String tarPath = tempFolderPath ;	    
			File extract_folder = new File(MediaStoreUtils.UNPACK_TEMP_FILE_PATH, mediaUUID);
			String tarPath_db = MediaStoreUtils.UNPACK_TEMP_FILE_PATH + "/" + mediaUUID + "/" + mediaUUID +   DB_TAR_EXTENSION;
			File db_tar = new File(tarPath_db);
			extract_folder.mkdirs();
			String action = "mediafile_unpack";	 

			String[] cmd = {MediaStoreUtils.SCRIPT_MEDIA_CONVERTOR_EXEC, "-i", "all", "-f", tarPath};
			deviceFramework.getLogger().info("MediaConvertor info: " + MediaStoreUtils.joinArray(cmd, " "));
			int retVal = 0;

			Document mediaInfoDoc = MediaStoreServiceUtils.executeCLICommands(cmd);
			String streamType = XMLUtils.getValueXpath(mediaInfoDoc, "//StreamType", "");

			if(streamType.equals("RTP")){
				// Create SDP File
				String sdpfilePath = extract_folder.getParentFile().getAbsolutePath()+"/"+ mediaUUID+"/"+ mediaUUID;
				String sdpTemplateDoc = URLDecoder.decode(XMLUtils.getValueXpath(mediaInfoDoc, "//SDP-Template-Src", ""), "UTF-8");
				String sdpContent = MediaStoreServiceUtils.generateSDPFromSDPTemplate(sdpTemplateDoc, "0.0.0.0", 0);
				String sdpFix = MediaStoreServiceUtils.fixSDPProblemWithOlderMedia(sdpContent);
				MediaStoreServiceUtils.writeSDPFile(sdpfilePath, sdpFix);
			}

			String [] cmdsh = { MediaStoreUtils.SCRIPT_MEDIA_FILE_CONVERSION, action, tarPath, extract_folder.getParentFile().getAbsolutePath(), 
					rootFolderPath, db_tar.getName(), mediaUUID, dateModified, storeServer.getCurrentRevision(storeDirInfo)+"", tarPath,dirID,dirNID,deviceFramework.getServiceVersion(),queueID,groupID,groupNID,groupFileUUID,new Boolean(isGroupCreated).toString(),"&" };
			deviceFramework.getLogger().debug("mediafile_unpack :=" + MediaStoreUtils.joinArray(cmdsh, " "));
			StringBuffer buf = new StringBuffer();
			retVal = CommandLineUtils.executeProcessInThread(cmdsh, buf,log);
			if (retVal != 0) {
				// send back error..
				String error = "Error during unpack the media file-'"+ tempFolderPath + "'";
				deviceFramework.getLogger().debug(error);
				throw new Exception(error);
			}
		}catch(Exception e){
			deleteTempFileOnException(tempFolderPath);
			throw e;
		}

	}

	@Override
	public void diskSpaceUpdate(String dirID, String dirNID, long sizeDeltaAdded, long sizeDeltaDeleted, long sizeUnreserved, boolean publishSizeUpdate) throws Exception {

		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		// Unreserving size of fileUUID after prepare/cutlist of media is completed
		if(sizeUnreserved != 0) {
			long currentReservedSize = storeDirInfo.getReservedDiskSize();
			long newReserveSize = ((currentReservedSize - sizeUnreserved) >= 0) ? (currentReservedSize - sizeUnreserved) : 0;
			storeDirInfo.setReservedDiskSize(newReserveSize);
			
			// Unreserve size from sizeUsed 
			long currentSizeUsed = storeDirInfo.getSizeUsed();
			storeDirInfo.setSizeUsed(currentSizeUsed - sizeUnreserved);
			deviceFramework.getLogger().info("DMSMediaStoreAgent: DiskSpaceUpdate: Unreserving size: " + sizeUnreserved + ", SizeUsed: " + storeDirInfo.getSizeUsed());
		}

		if(publishSizeUpdate) {
			long sizeUsed =storeServer.getDirSizeUsed(storeDirInfo, log);
			MediaStoreUtils.publishDirSizeUpdate(storeDirInfo.getMediaStoreDirNID(), sizeUsed, deviceFramework, new PublishDirSizeResponseHandler());
			return;
		}		

		long sizeUsed= storeDirInfo.getSizeUsed();
		storeDirInfo.setSizeUsed(sizeUsed+(sizeDeltaAdded-sizeDeltaDeleted));
		deviceFramework.getLogger().debug("DMSMediaStoreAgent:diskSpaceUpdate for "+dirID +" sizeDeltaAdded "+sizeDeltaAdded +" sizeDeltaRemoved " + sizeDeltaDeleted +" updated sizeUsed"+sizeUsed);

	}

	@Override
	public boolean isDiskFreeSpaceReachesToQuotaLimit(String dirID, String dirNID) throws Exception {

		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		return (storeDirInfo.getSize()-storeDirInfo.getSizeUsed()) <= storeDirInfo.getDiskQuotaExceedLimit();
	}

	@Override
	public boolean isDiskSizeExceedsThreshold(String dirID, String dirNID)
			throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void updateDirThreshold(String dirId, String dirNID, Long dirThreshold) throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirId);
		storeDirInfo.setDiskFullThreshold(dirThreshold);
	}


	@Override
	public long getOffset(String dirID, String dirNID, String mediaUUID, long timecode) throws Exception{
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, mediaUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		long offSet=mediaStoreFile.getOffsetAtTC(timecode, mediaUUID, storeDirInfo, deviceDB,deviceFramework.getLogger());
		return offSet;
	}

	@Override
	public long getTimecode(String dirID, String dirNID, String mediaUUID, long offSet) throws Exception{
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, mediaUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		long timecode=	mediaStoreFile.getTCAtOffset(mediaUUID, offSet, storeDirInfo, deviceDB,deviceFramework.getLogger());
		return timecode;
	}


	@Override
	public long getFileSize(String dirID, String dirNID, String mediaUUID) throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, mediaUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		return mediaStoreFile.getMediaStoreFileSize(storeDirInfo, mediaUUID, deviceFramework.getLogger());

	}

	@Override
	public void offloadFileToNAS(String dirNID, String dirID, String format, String nasURL, String fileNID, String fileUUID) throws Exception {
		int retVal = -1;
		StringBuffer buf = new StringBuffer();
		String nfsuploadcommand = "/etc/scripts/nfs_upload.sh";
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String rootFolderPath = storeDirInfo.getMediaStoreRootPath();
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, fileUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		Document mediaStoreFileDoc = mediaStoreFile.getMediaStoreFile(storeDirInfo, fileUUID);
		String title = XMLUtils.getValueXpath(mediaStoreFileDoc, "//Title", "");
		String streamType  = XMLUtils.getValueXpath(mediaStoreFileDoc, "//StreamType", "");

		if(format == null || format.isEmpty()) {
			format = "Proprietary";
		}

		String tempFolderPath =rootFolderPath+"blob/"+fileUUID ;
		String mkdPath = ""; 
		String zipPath = "";

		if(!format.equalsIgnoreCase("Standard") || streamType.equals("V2D")|| streamType.equals("UDP"))
		{
			mkdPath = tempFolderPath+"/proprietary/*.*";
		}
		if(!format.equalsIgnoreCase("Proprietary") &&  !streamType.equals("V2D") && !streamType.equals("UDP"))
		{
			zipPath = tempFolderPath+"/standard/*.*";
		}

		String clientData = fileNID;
		String standardMediaFileName = title;
		standardMediaFileName = standardMediaFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_"); // replace any special characters with '_'
		String[] cmd = { nfsuploadcommand, "nfs_upload", mkdPath, nasURL, clientData, format, zipPath, standardMediaFileName, streamType, dirNID, fileUUID , "&" };
		retVal = CommandLineUtils.executeProcessInThread(cmd, buf, log);
		if (retVal != 0) {
			String error = "Error offloading the media file-'" + mkdPath + "'";
			deviceFramework.getLogger().info(error.toString());
			throw new Exception(error);
		}
	}

	@Override
	public HashMap<String, Element> getMediaStoreBookMarks(String dirID, String dirNID, String mediaStoreFileID) throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, mediaStoreFileID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		// TODO Auto-generated method stub
		return mediaStoreFile.getListOfBookmarks(storeDirInfo, mediaStoreFileID);
	}



	private void createDownloadTagForFormat(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, Element fileEl) throws Exception {
		String storeVersion = storeServer.getVersionOfMediaStoreFile(dirInfo, mediaStoreFileID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);		

		Element downloadEl = DocumentHelper.createElement("Download");
		String format = "";

		String state = "Prepared";	
		if(mediaStoreFile.isDownloadFormatStale(dirInfo,mediaStoreFileID, "proprietary")){
			format ="Proprietary";
			mediaStoreFile.addDownloadTagForFormat(dirInfo, mediaStoreFileID, downloadEl, state, format);
		}

		if(mediaStoreFile.isDownloadFormatStale(dirInfo,mediaStoreFileID, "standard")){
			format="Standard";
			mediaStoreFile.addDownloadTagForFormat(dirInfo, mediaStoreFileID, downloadEl, state, format);
		}

		if(format.isEmpty()){
			mediaStoreFile.addDownloadTagForFormat(dirInfo, mediaStoreFileID, downloadEl, "NotReady", format);
		}	

		fileEl.add(downloadEl.detach());
	}

	private void createMediaGroupStoreFile(String dirId, String dirNID, String fileUUID, String title, String mediaGroupID,String type) throws Exception{
		Document mediaStoreFileDoc = XMLUtils.loadXMLResource("/com/ipvs/xsd/objectXml/MediaStoreFile.xml");	
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, type);

		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.STATE_EL_NAME, MediaStoreServiceConst.NOT_READY);
		String currentTime = String.valueOf(System.currentTimeMillis());
		XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.DATE_CREATED,currentTime);
		XMLUtils.setValueXpath(mediaStoreFileDoc,"//"+ MediaStoreServiceConst.DATE_MODIFIED,currentTime);
		XMLUtils.addAttribute(mediaStoreFileDoc.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION, deviceFramework.getServiceVersion());


		//update media group information 
		Element mediaGroupELement= XMLUtils.getElementXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME);		

		mediaGroupELement.setText(mediaGroupID);		


		for( MediaStoreServiceConst.MEDIA_TYPE_ENUMS typeEnum : MediaStoreServiceConst.MEDIA_TYPE_ENUMS.values()) {
			if(!typeEnum.toString().equals(type)) {
				XMLUtils.getElementXpath(mediaStoreFileDoc, "//" + typeEnum.toString()).detach();
			}
		}


		addFile(dirId, dirNID, fileUUID, mediaStoreFileDoc);
	}

	@Override
	public Document createMediaFromCutList(String byNID, String dirID, String cutList, String title, String fileUUID, String dirNID, String mediaFormat, String new_fileNID, String new_UUID, Document mediaStoreFileRespDoc) throws Exception{
		Element respfileEl =  DocumentHelper.createElement("File");
		String rootFolderPath = getDirPath(dirNID, null);
		String tempFolderPath = "";
		String action = "mediafile_cutlist";
		String requestTimeStamp = System.currentTimeMillis() + "";

		tempFolderPath = getDMSWorkAreaPath(new_UUID, rootFolderPath);
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, "");
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, fileUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);

		// get blob size and check for available disk space
		long blobsize = mediaStoreFile.getMediaStoreFileSize(storeDirInfo, fileUUID, deviceFramework.getLogger());
		storeServer.checkAvailableDiskSpaceForPrepareDownload(blobsize, storeDirInfo, deviceFramework.getLogger());

		Document mediaStoreFileDoc = getMediaStoreFileDocument(storeDirInfo, fileUUID);
		String streamType  = XMLUtils.getValueXpath(mediaStoreFileDoc, "//StreamType", "");
		String mediaType = XMLUtils.getValueXpath(mediaStoreFileDoc, "//Type", "");

		Element fileEl =  DocumentHelper.createElement("File");
		XMLUtils.addAttribute(fileEl, "size", String.valueOf(blobsize));		
		XMLUtils.addAttribute(fileEl, "dirNID", dirNID);
		XMLUtils.addAttribute(fileEl, "appendSize", String.valueOf(false));
		fileEl.add(mediaStoreFileDoc.getRootElement().detach());

		String mediaFolderPath = rootFolderPath + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + fileUUID; 
		String tempFilePath = getDMSFilePath_prepare_proprietary(tempFolderPath, new_UUID);
		String prepare_file_folder = tempFolderPath+"/"+ new_UUID;
		MediaStoreServiceUtils.writeIndexFile(prepare_file_folder,dirNID, new_fileNID, new_UUID, streamType, title, requestTimeStamp);
		MediaStoreServiceUtils.writeMediaFile(prepare_file_folder+"/db/IPVSc_default/", new_fileNID, fileEl.asXML());

		//Create cutlist Metadata File
		Element createMediaFromCutListEL =  DocumentHelper.createElement("CreateMediaFromCutList");
		XMLUtils.addAttribute(createMediaFromCutListEL, "title", title);
		createMediaFromCutListEL.add((Element)XMLUtils.getElementXpath(mediaStoreFileRespDoc, "//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME).clone());
		createMediaFromCutListEL.add((Element)XMLUtils.getElementXpath(mediaStoreFileRespDoc, "//"+MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME).clone());
		MediaStoreServiceUtils.writeMediaFile(prepare_file_folder, "cutlist_metadata.xml", createMediaFromCutListEL.asXML());

		// Create SDP template
		String sdpfilePath = tempFolderPath+"/"+ new_UUID;
		createSDPTemplate(mediaFolderPath, sdpfilePath);


		// Mediafile name for native format prepare
		String standardMediaFileName = title + "_" + new_UUID;
		standardMediaFileName = standardMediaFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_"); // replace any special characters with '_'
		sdpfilePath = sdpfilePath + ".sdp";

		File extract_folder = new File(MediaStoreUtils.UNPACK_TEMP_FILE_PATH, new_UUID);
		String tarPath_db = MediaStoreUtils.UNPACK_TEMP_FILE_PATH + "/" + new_UUID + "/" + new_UUID +   DB_TAR_EXTENSION;
		File db_tar = new File(tarPath_db);
		extract_folder.mkdirs();

		log.info("File is calling scripts params in order "+ action + "2- " + tempFilePath + "3-" + mediaFolderPath + "4- " + prepare_file_folder
				+ "5- " + new_fileNID + "6- " + cutList + "7- " + requestTimeStamp + "8- " + mediaFormat
				+ "9- " + streamType + "10- " +sdpfilePath+ "11- " +standardMediaFileName+ "12- " +dirNID+ "13- " +fileUUID + "14- " +streamType 
				+ "15- "+extract_folder.getParentFile().getAbsolutePath()+" 16- "+rootFolderPath+" 17-"+db_tar.getName()+" 18- "+new_UUID 
				+" 19- "+storeServer.getCurrentRevision(storeDirInfo)+ " 20- "+deviceFramework.getServiceVersion()+ "21- "+blobsize);

		String[] cmd = { MediaStoreUtils.SCRIPT_MEDIA_FILE_CONVERSION, action,
				tempFilePath, mediaFolderPath,
				prepare_file_folder, new_fileNID,
				cutList, requestTimeStamp, mediaFormat,
				streamType, sdpfilePath, standardMediaFileName, dirNID, fileUUID, streamType, extract_folder.getParentFile().getAbsolutePath(), 
				rootFolderPath, db_tar.getName(), new_UUID, storeServer.getCurrentRevision(storeDirInfo)+"", 
				deviceFramework.getServiceVersion(), String.valueOf(blobsize), requestTimeStamp,  "&" };
		deviceFramework.getLogger().debug("mediafile_cutlist :=" + MediaStoreUtils.joinArray(cmd, " "));
		int retVal = 0;
		StringBuffer buf = new StringBuffer();
		retVal = CommandLineUtils.executeProcessInThread(cmd, buf,log);
		if (retVal != 0) {
			//send back error..
			String error = "Error during unpack the media file-'"+ tempFolderPath+"'";
			deviceFramework.getLogger().debug(error);
			throw new Exception(error);
		}

		//Creating Response Doc for New cut-list media
		XMLUtils.setValueXpath(mediaStoreFileRespDoc, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, mediaType);
		XMLUtils.setValueXpath(mediaStoreFileRespDoc, "//"+MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, streamType);
		XMLUtils.setValueXpath(mediaStoreFileRespDoc, "//"+MediaStoreServiceConst.DATE_CREATED,requestTimeStamp);
		XMLUtils.setValueXpath(mediaStoreFileRespDoc,"//"+ MediaStoreServiceConst.DATE_MODIFIED,requestTimeStamp);
		XMLUtils.addAttribute(mediaStoreFileRespDoc.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION,  deviceFramework.getServiceVersion());
		//		for( MediaStoreServiceConst.MEDIA_TYPE_ENUMS typeEnum : MediaStoreServiceConst.MEDIA_TYPE_ENUMS.values()) {
		//			if(!typeEnum.toString().equals(mediaType)) {
		//				XMLUtils.getElementXpath(mediaStoreFileRespDoc, "//" + typeEnum.toString()).detach();
		//			}
		//		}
		Document resultDoc = DocumentHelper.createDocument();
		resultDoc.add(respfileEl);
		XMLUtils.addAttribute(respfileEl, "id", new_UUID);
		XMLUtils.addAttribute(respfileEl, "dirNID", dirNID);

		if(mediaType.equals(MediaStoreServiceConst.MEDIA_TYPE_ENUMS.PVRMediaClip.toString())) {
			XMLUtils.addAttribute(respfileEl, "dirNID", storeDirInfo.getPvrDirNID());
		} 
		respfileEl.add(mediaStoreFileRespDoc.getRootElement().detach());
		return XMLUtils.elementToDocument(respfileEl);
	}

	@Override
	public void clearDownloadCache(String fileNID, String dirID, String fileUUID, String dirNID) throws Exception {
		String action = "cleardownloadcache";
		String rootFolderPath = getDirPath(dirNID, null);
		String mediaFolderPath = rootFolderPath + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + fileUUID; 
		String[] cmd = { MediaStoreUtils.SCRIPT_MEDIA_FILE_CONVERSION, action,
				mediaFolderPath, fileNID, dirNID, fileUUID,  "&" };
		deviceFramework.getLogger().debug("clear_download_cache :=" + MediaStoreUtils.joinArray(cmd, " "));
		int retVal = 0;
		StringBuffer buf = new StringBuffer();
		retVal = CommandLineUtils.executeProcessInThread(cmd, buf,log);
		if (retVal != 0) {
			//send back error..
			String error = "Error during Clear Download Cache '"+ mediaFolderPath+"'";
			deviceFramework.getLogger().debug(error);
			throw new Exception(error);
		}
	}


	private void validateMediaIngestion(String filePath) throws Exception {
		String action = "validate_media_ingestion";
		String[] cmd = { MediaStoreUtils.SCRIPT_MEDIA_FILE_CONVERSION, action,
				filePath};
		deviceFramework.getLogger().debug("filePath :=" + MediaStoreUtils.joinArray(cmd, " "));
		int retVal = CommandLineUtils.executeSystemCommand(cmd);
		if (retVal != 0) {
			//send back error..
			String error = "Error during validate Ingesting Media '"+ filePath+"'";
			deviceFramework.getLogger().debug(error);
			throw new ServiceException(MediaRoomServiceExceptionCodes.EXECUTION_OF_COMMANDS_FIALED, error);
		}
	}

	@Override
	public Document ingestMedia(String filePath, String currentTime, String dirID, String dirNID, String mediaStoreFileNID, Document mediaStoreFileDocument, String hlsenable) throws Exception {
		try{
			int pos = filePath.lastIndexOf('.');
			String extension = filePath.substring(pos);
			if (extension.equalsIgnoreCase(".mp4")) {
				return ingestMP4Media(filePath, currentTime, dirID, dirNID, mediaStoreFileNID, mediaStoreFileDocument, hlsenable);
			}else {
				return ingestSnapshot(filePath, currentTime, dirID, dirNID, mediaStoreFileNID, mediaStoreFileDocument);
			}
		}
		catch(Exception e){
			deleteTempFileOnException(filePath);
			throw e;
		}
	}

	private Document ingestMP4Media(String filePath, String currentTime, String dirID, String dirNID, String mediaStoreFileNID, Document mediaStoreFileDocument, String hlsenable) throws Exception {
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String storePath = storeDirInfo.getMediaStoreRootPath() + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID + "/";
		String requestTimeStamp = System.currentTimeMillis() + "";

		// validate the MP4 is importable
		validateMediaIngestion(filePath);

		String action = "media_ingestion";
		String title = XMLUtils.getValueXpath(mediaStoreFileDocument, "//"+MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");
		Document resultDoc = addFile(dirID, dirNID, mediaStoreFileNID, mediaStoreFileDocument);

		File ingest_folder = new File(MediaStoreUtils.UNPACK_TEMP_FILE_PATH, mediaStoreFileNID);
		String tempFolderPath = ingest_folder.getParentFile().getAbsolutePath();
		String sdpfilePath = tempFolderPath+"/"+mediaStoreFileNID+".sdp";

		if (ingest_folder.exists()) {
			ingest_folder.delete();
		}
		ingest_folder.mkdirs();
		//copy db.xml from blob to temporary area
		CommandLineUtils.executeCLICommand(new String[]{"cp",storePath+MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME,tempFolderPath+"/"+mediaStoreFileNID+"/"+MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME});

		if (dirNID.isEmpty()) {
			dirNID = storeDirInfo.getMediaStoreDirNID();
		}


		String new_UUID = Utils.getUUID(); // generate a new UUID for prepare file
		String standardMediaFileName = title + "_" + new_UUID;
		standardMediaFileName = standardMediaFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_"); // replace any special characters with '_'
		MediaStoreServiceUtils.writeIndexFile(tempFolderPath+"/"+mediaStoreFileNID,dirNID, standardMediaFileName, mediaStoreFileNID, "", title, requestTimeStamp);
		String[] cmd = { MediaStoreUtils.SCRIPT_MEDIA_FILE_CONVERSION, action, filePath, storePath, tempFolderPath, sdpfilePath, dirNID,
				mediaStoreFileNID, hlsenable, standardMediaFileName, "&" };
		deviceFramework.getLogger().info("injectMedia :=" + MediaStoreUtils.joinArray(cmd, " "));
		int retVal = 0;
		StringBuffer buf = new StringBuffer();
		retVal = CommandLineUtils.executeProcessInThread(cmd, buf, log);
		if (retVal != 0) {
			// send back error..
			String error = "Error during ingest media file-'" + tempFolderPath + "'";
			deviceFramework.getLogger().debug(error);
			throw new Exception(error);
		}

		return resultDoc;
	}

	@Override
	public void trimMediaFromCutList(String byNID, String dirNID, String dirID, String cutlist, String fileUUID) throws Exception {

		String tempFolderPath = "";
		String action = "mediafile_trim";
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		String rootFolderPath = storeDirInfo.getMediaStoreRootPath();
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, fileUUID);
		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
		Document mediaStoreFileDoc = mediaStoreFile.getMediaStoreFile(storeDirInfo, fileUUID);

		tempFolderPath = getDMSWorkAreaPath(fileUUID, rootFolderPath);

		// get blob size and check for available disk space
		long blobsize = mediaStoreFile.getMediaStoreFileSize(storeDirInfo, fileUUID, deviceFramework.getLogger());
		storeServer.checkAvailableDiskSpaceForPrepareDownload(blobsize, storeDirInfo, deviceFramework.getLogger());

		//Mark File BUSY
		Element mediaStoreFileEl = XMLUtils.getElementXpath(mediaStoreFileDoc, "//MediaStoreFile");
		XMLUtils.setValueXpathElement(mediaStoreFileEl, ".//State", MediaStoreServiceConst.BUSY);
		setFileRequest(dirID, dirNID, "", fileUUID, XMLUtils.elementToDocument(mediaStoreFileEl));

		String streamType  = XMLUtils.getValueXpath(mediaStoreFileDoc, "//StreamType", "");
		String title = XMLUtils.getValueXpath(mediaStoreFileDoc, "//Title", "");
		String requestTimeStamp = System.currentTimeMillis() + "";

		Element fileEl =  DocumentHelper.createElement("File");
		XMLUtils.addAttribute(fileEl, "size", String.valueOf(blobsize));		
		XMLUtils.addAttribute(fileEl, "dirNID", dirNID);
		XMLUtils.addAttribute(fileEl, "appendSize", String.valueOf(false));
		fileEl.add(mediaStoreFileDoc.getRootElement().detach());

		String mediaFolderPath = rootFolderPath + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + fileUUID; 
		String tempFilePath = getDMSFilePath_prepare_proprietary(tempFolderPath, fileUUID);
		String prepare_file_folder = tempFolderPath+"/"+ fileUUID;
		MediaStoreServiceUtils.writeIndexFile(prepare_file_folder,dirNID, byNID, fileUUID, streamType, title, requestTimeStamp);
		MediaStoreServiceUtils.writeMediaFile(prepare_file_folder+"/db/IPVSc_default/", byNID, fileEl.asXML());

		// Create SDP template
		String sdpfilePath = tempFolderPath+"/"+ fileUUID;
		createSDPTemplate(mediaFolderPath, sdpfilePath);

		// Mediafile name for native format prepare
		String standardMediaFileName = title + "_" + fileUUID;
		standardMediaFileName = standardMediaFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_"); // replace any special characters with '_'
		sdpfilePath = sdpfilePath + ".sdp";

		log.info("File is calling scripts params in order 1-"+ action + " 2- " + tempFilePath + " 3-" + mediaFolderPath + " 4- " + prepare_file_folder
				+ "5- " + byNID + " 6- " + cutlist + " 7- " + streamType + " 8- " + sdpfilePath
				+ " 9- " + standardMediaFileName + " 10 - "+dirNID+" 11- " +rootFolderPath+ " 12- " +fileUUID+ " 13- " +storeServer.getCurrentRevision(storeDirInfo)+ " 14- " +deviceFramework.getServiceVersion() 
				+ "15- "+blobsize+" 16- "+requestTimeStamp);

		String[] cmd = { MediaStoreUtils.SCRIPT_MEDIA_FILE_CONVERSION, action, tempFilePath, mediaFolderPath, prepare_file_folder, byNID, cutlist, streamType, sdpfilePath,
				standardMediaFileName, dirNID, rootFolderPath, fileUUID, storeServer.getCurrentRevision(storeDirInfo) + "", deviceFramework.getServiceVersion(), String.valueOf(blobsize),
				requestTimeStamp, "&" };
		deviceFramework.getLogger().debug("mediafile_trim :=" + MediaStoreUtils.joinArray(cmd, " "));
		int retVal = 0;
		StringBuffer buf = new StringBuffer();
		retVal = CommandLineUtils.executeProcessInThread(cmd, buf,log);
		if (retVal != 0) {
			//send back error..
			String error = "Error during trim media file-'"+ tempFolderPath+"'";
			deviceFramework.getLogger().debug(error);
			throw new Exception(error);
		}

	}

	private void createSDPTemplate(String mediaFolderPath, String sdpfilePath) throws Exception{
		Document dbXMLdoc = XMLUtils.fileToDocument(mediaFolderPath+"/db.xml");
		String sdpTemplateDoc = URLDecoder.decode(XMLUtils.getValueXpath(dbXMLdoc, "//SDP-Template-Src", ""), "UTF-8");
		String sdpContent = MediaStoreServiceUtils.generateSDPFromSDPTemplate(sdpTemplateDoc, "0.0.0.0", 0);
		String sdpFix = MediaStoreServiceUtils.fixSDPProblemWithOlderMedia(sdpContent);
		MediaStoreServiceUtils.writeSDPFile(sdpfilePath, sdpFix);
	}

	@Override
	/*
	 * (non-Javadoc)
	 * @see com.barco.device.agent.MediaAgentI#startPostingThumbnail(java.lang.String, java.lang.String, java.lang.String)
	 * get the thumbnail from mediaID (fileNID) of the URL and make httpPost request to SC with the thumbnail
	 */
	public void startPostingThumbnail(String sourceNID, String portId, String publishURL) throws Exception {

		String dirPath = getDirPath("", portId)+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR +"/"+sourceNID+"/tn/tn.jpg";
		deviceFramework.getLogger().debug("StartPostingThumbnail: Posting image dir path: "+dirPath+":: URL: "+publishURL);

		File thumbnailFile = new File(dirPath);
		if(!thumbnailFile.exists()){
			thumbnailFile = new File(MediaStoreServiceConst.DEFAULT_RECORDING_THUMBNAIL_PATH);
		}
		if(thumbnailFile.exists() && !thumbnailFile.isDirectory()) {
			URL url = new URL(publishURL);

			thumbnailUploadService.execute(new Thread(new UploadThumbnailToSCThread(url, thumbnailFile)));
		}
	}




	class UploadThumbnailToSCThread implements Runnable{
		URL url;
		File file;
		public UploadThumbnailToSCThread(URL url, File file) {
			this.url = url;
			this.file= file;
		}
		@Override
		public void run() {
			deviceFramework.getLogger().info(Thread.currentThread().getName()+" Upload "+file.getAbsolutePath() +" to "+url.getPath());
			try {
				uploadThumbnailToSC(url, file);
			} catch (Exception e) {
				deviceFramework.getLogger().error(e);
				e.printStackTrace();
			}
		}
		private void uploadThumbnailToSC(URL url, File file) throws Exception{
			deviceFramework.getLogger().debug("StartPostingThumbnail: Uploading the image!");
			HttpURLConnection urlConnection = null;
			FileInputStream fStream = null;
			OutputStream out = null;
			InputStream is = null;
			try{

				urlConnection = (HttpURLConnection) url.openConnection();
				urlConnection.setRequestMethod("POST");

				urlConnection.setDoOutput(true);		
				urlConnection.setRequestProperty("Content-Type", "multipart/form-data; boundary=---------------------------4664151417711"); 

				String message1 = ""; 
				message1 += "-----------------------------4664151417711" + CR_LF; 
				message1 += "Content-Disposition: form-data; name=\"file\"; filename=\"" + file.getName()+UUID.randomUUID().toString() +"\"" + CR_LF; 
				message1 += "Content-Type: image/jpeg" + CR_LF; 
				message1 += CR_LF; 

				String message2 = ""; 
				message2 += CR_LF + "-----------------------------4664151417711--" + CR_LF; 

				urlConnection.setRequestProperty("Content-Length", String.valueOf((message1.length() + message2.length() + file.length()))); 


				out = new BufferedOutputStream(urlConnection.getOutputStream()); // request

				out.write(message1.getBytes());
				try{
					fStream = new FileInputStream(file);
					byte[] buffer = new byte[1024 * 1024];
					int length = -1;
					while ((length = fStream.read(buffer)) != -1) {
						out.write(buffer, 0, length);
						out.flush();
					}
					deviceFramework.getLogger().debug("written:" + file.getAbsolutePath()); 
				}
				catch (Exception e){
					deviceFramework.getLogger().error("Thumbnail does not exist");
				}
				out.write(message2.getBytes()); 
				out.flush();

				if(urlConnection.getResponseCode() == HttpURLConnection.HTTP_OK){
					is = urlConnection.getInputStream();
					StringBuilder response = new StringBuilder();
					byte[] respBuffer = new byte[4096];
					while (is.read(respBuffer) >= 0) {
						response.append(new String(respBuffer).trim());
					}
					deviceFramework.getLogger().debug("DMSMediaStoreAgent.UploadThumbnailToSC: The response from server is: " + response.toString());
				}
				else{
					//deviceFramework.getLogger().debug("DMSMediaStoreAgent.UploadThumbnailToSC: The response from server is: " + urlConnection.getResponseCode());
				}
			}
			finally{
				if(fStream != null)
					fStream.close();
				if(out != null)
					out.close();
				if(is != null)
					is.close();
				if(urlConnection != null)
					urlConnection.disconnect();
			}
		}
	}

	@Override
	public void reset() {  
		try {
			String[] cmd = { "/etc/scripts/mediafile_conversion_utils.sh", "stop_mediaconverter_service",  "&" };
			deviceFramework.getLogger().debug("Cleaning Media :=" + MediaStoreUtils.joinArray(cmd, " "));
			int retVal = 0;
			StringBuffer buf = new StringBuffer();
			retVal = CommandLineUtils.executeProcessInThread(cmd, buf,log);
			if (retVal != 0) {
				String error = "Error during cleaning media file.";
				deviceFramework.getLogger().debug(error);
			}
		} catch (Exception e) {
		}
		
	}

	public void deleteTempFileOnException (String filePath){
		File tarPath_file = new File(filePath); 
		if(tarPath_file.exists())
			tarPath_file.delete();
	}

    @Override
    public synchronized void startPurgeFileProcess(String dirID, String dirNID,
    		List<?> fileList) throws Exception {
    	MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
    	if(storeDirInfo.getDirPurgePorcess() == null ) 
    	{   	
    		storeDirInfo.createDirPurgeFileProcess(getDeleterPortRunningNumber());
    	} 
    	
    	//move all the files to trash and start deleter port 
    	for(Object object: fileList) {
    		Element fileEl = (Element) object;
    		String fileUUID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.BY_ID_ATTRIBUTE_NAME, "");
    		String fileNID = XMLUtils.getAttributeValueWithDefault(fileEl, MediaStoreServiceConst.FILE_NID_ATTRIBUTE_NAME, "");			
    		String storeVersion = this.storeServer.getVersionOfMediaStoreFile(storeDirInfo, fileUUID);	
    		MediaStoreFileI mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
    		mediaStoreFile.deleteFile(storeDirInfo, fileUUID, MediaStoreDirectoryInfo.DirPurgeFileProcess.PURGEFILE_DIR_PATH, deviceFramework.getLogger());
    		storeDirInfo.getDirPurgePorcess().addFile(fileUUID, fileNID);	  
    	}

    	//start deleter port
    	if(!storeDirInfo.isDirPurgeFileProcessRunning()) {
      		storeDirInfo.startDirPurgeFileProcess();    	
      		storeServer.addDeleterPort(storeDirInfo.getDirPurgePorcess().getDeleterPortID(), storeDirInfo.getMediaStoreRootPath()+MediaStoreDirectoryInfo.DirPurgeFileProcess.PURGEFILE_DIR_PATH, deviceDB, log);
    	}
    }

	@Override
	public void abortMediaPackagingProcess(String newUUID, String oldUUID, String dirNID, String type, String format) throws Exception {
		if(oldUUID.isEmpty() || dirNID.isEmpty() || type.isEmpty()) {
			String error = "Error in aborting export/import files, missing parameters";
		    deviceFramework.getLogger().debug(error);
		    throw new Exception(error);
		}
		
		String tempDirPath = getTempDirPathForExportImport(type);
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, "");
		String fileType = "";
		Document mediaStoreFileDoc = null;
		MediaStoreFileI mediaStoreFile = null;
		try {
			String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, oldUUID);
			mediaStoreFile = MediaVersionManager.getMediaStoreFileI(storeVersion);
			mediaStoreFileDoc = mediaStoreFile.getMediaStoreFile(storeDirInfo, oldUUID);
			fileType = XMLUtils.getValueXpath(mediaStoreFileDoc, "//Type", "");
			
		} catch (Exception e) {
			deviceFramework.getLogger().info("Error in reading MediaStoreFile. Entry not found!");
		}
		
		Element filePublishEl = null;
		if(("Export".equals(type) || "Offload".equals(type))) {
			// Cleanup temp dir after abort
			cleanupAbortedTempDir(storeDirInfo, tempDirPath, newUUID);
			cleanupAbortedTempDir(storeDirInfo, tempDirPath+"/../cutlist", newUUID);
		
			if(mediaStoreFile != null && mediaStoreFileDoc != null) {
				// Publish update to sc
				filePublishEl = createFileElementForPublish(oldUUID, storeDirInfo.getMediaStoreDirNID(), fileType, storeDirInfo.getPvrDirNID(), MediaStoreServiceConst.READY, storeDirInfo.getRecycleBinNID(), mediaStoreFileDoc, false, mediaStoreFile.getMediaStoreFileSize(storeDirInfo, oldUUID, deviceFramework.getLogger()), null,false);
				
				// Set download status to not ready
				Element downloadEl = MediaStoreUtils.createDownloadElement(format, "NotReady", false);
				filePublishEl.add(downloadEl.detach());
						
				// Un-reserve the disk space allocated
				long blobsize = mediaStoreFile.getMediaStoreFileSize(storeDirInfo, oldUUID, deviceFramework.getLogger());
				diskSpaceUpdate("", dirNID, 0, 0, blobsize, true);
			}
	
		} else if ("Import".equals(type)) {

			// Cleanup temp dir after abort
			cleanupAbortedTempDir(storeDirInfo, tempDirPath, oldUUID);

			if(mediaStoreFile != null && mediaStoreFileDoc != null) {
				// Mark store file error
				XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+ MediaStoreServiceConst.STATE_EL_NAME, "Error");
				setFileRequest("", dirNID, "", oldUUID, mediaStoreFileDoc);
				
				// Mark db file error
				filePublishEl = createFileElementForPublish(oldUUID, storeDirInfo.getMediaStoreDirNID(), fileType, storeDirInfo.getPvrDirNID(), MediaStoreServiceConst.ERROR, storeDirInfo.getRecycleBinNID(), mediaStoreFileDoc, false, mediaStoreFile.getMediaStoreFileSize(storeDirInfo, oldUUID, deviceFramework.getLogger()), null,false);
			}
			
		}
	
		if (filePublishEl != null) {
			ArrayList<Element> publishList = new ArrayList<Element>();
			publishList.add(filePublishEl);

			// Publish file to SC.
			publishFileRequest(publishList, null);
		}

	}
	
	public String getTempDirPathForExportImport(String type) throws Exception {
		String path = "";
		if("Export".equals(type) || "Offload".equals(type))
			path = MediaStoreUtils.PREPARE_TEMP_FOLDER_PATH;
		else
			path = MediaStoreUtils.UNPACK_TEMP_FILE_PATH;
		return path;
	}
	
	public void cleanupAbortedTempDir(MediaStoreDirectoryInfo storeDirInfo, String tempDirPath, String uuid) throws Exception {
		if (uuid.isEmpty())
			return;

		String pidSuffix = "process.pid";
		String childpidSuffix = "childprocess.pid";
		String pidFileName = "";

		// Abort process on DMS
		pidFileName = tempDirPath + "/" + uuid + "/" + pidSuffix;
		abortProcess(pidFileName);
		// Abort child processes if any.
		pidFileName = tempDirPath + "/" + uuid + "/" + childpidSuffix;
		abortProcess(pidFileName);
		
		String trashPath = "/data/mediadir/trash";
		File trash = new File(trashPath);
		trash.mkdirs();

		File moveFilePath = new File(tempDirPath+"/"+uuid);
		if(!moveFilePath.exists()) {
			deviceFramework.getLogger().debug("No File found to move to trash =" + moveFilePath.getAbsolutePath());
			return;
		}
		
		String[] systemCmd = {"mv", moveFilePath.getAbsolutePath(), trashPath + "/" + uuid + System.currentTimeMillis()};
		deviceFramework.getLogger().info("cleanupAbortedTempDir: Move to trash systemcmd:=" +  MediaStoreUtils.joinArray(systemCmd, " "));
		CommandLineUtils.executeSystemCommand(systemCmd);
	}
	
	public void abortProcess(String pidFileName) throws Exception {
		File pidFile = new File(pidFileName);
		if(pidFile.exists()) {
			deviceFramework.getLogger().info("Read pid from file: " + pidFileName);
			String pid = FileUtils.readFile(pidFileName);
			if(pid != null && !pid.isEmpty()) {
				String[] systemCmd = {"sh", "/etc/scripts/mediafile_conversion_utils.sh", "stop_process", pid};
				deviceFramework.getLogger().debug("Kill process pid command:=" + MediaStoreUtils.joinArray(systemCmd, " "));
				int retVal = CommandLineUtils.executeSystemCommand(systemCmd);
				if (retVal != 0) {
				    //send back error..
				    String error = "Error killing process pid: " + pid;
				    deviceFramework.getLogger().debug(error);
				    throw new ServiceException(MediaRoomServiceExceptionCodes.EXECUTION_OF_COMMANDS_FIALED, error);
				}
				deviceFramework.getLogger().info("Aborted process pid: " + pid);
			}
		}
	
	}


	@Override
	public synchronized void stopPurgeFileProcess(String id, String dirID, String dirNID) throws Exception {
		
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		if( !storeDirInfo.isDirPurgeFileProcessRunning() || !id.equals(storeDirInfo.getDirPurgePorcess().getDeleterPortID())) {
			log.debug("Either Purge process is not running or the deleter port id is not matching");
			return;
		}
		
		//check there is no files exists in purge folder before deleting.
		if(!FileUtils.isDirEmpty((storeDirInfo.getMediaStoreRootPath()+"/"+MediaStoreDirectoryInfo.DirPurgeFileProcess.PURGEFILE_DIR_PATH))) {
			//Wait for all the files to be cleaned in purge directory
			log.debug("Found files in purge directory wait for all the files to be cleaned.");			
			return; 
		}
		
		//stop deleter port process
		storeServer.deleteDeleterPort(storeDirInfo.getDirPurgePorcess().getDeleterPortID(), deviceDB, log);
		
		//reset purge process
		storeDirInfo.stopDirPurgeFileProcess();
		
		//send purge file process status with size update to MS
		long sizeUsed =storeServer.getDirSizeUsed(storeDirInfo, log);
		MediaStoreUtils.publishPurgeFileStatus(storeDirInfo.getMediaStoreDirNID(), sizeUsed, deviceFramework,  new PublishDirPurgeFileStatusResponseHandler());
		
	}


	@Override
	public boolean isMediaUpgraded(String dirID, String dirNID,
			String mediaStoreFileID) throws Exception {
		// TODO Auto-generated method stub
		//!storeDirInfo.getDeviceServiceVersion().equals(storeDirInfo.getMediaStoreServiceVer())
		
		MediaStoreDirectoryInfo storeDirInfo = getMediaStoredirectoryInfo(dirNID, dirID);
		if(storeDirInfo == null) {
			return false;
		}		
		String storeVersion = storeServer.getVersionOfMediaStoreFile(storeDirInfo, mediaStoreFileID);
		return storeDirInfo.getDeviceServiceVersion().equals(storeVersion);
	}

}
