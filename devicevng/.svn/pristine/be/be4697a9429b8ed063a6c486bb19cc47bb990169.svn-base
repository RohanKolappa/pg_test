package com.barco.devicevng.MediaStore.StoreServer;

import java.io.File;
import java.util.UUID;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.device.handler.MediaStore.MediaStoreServiceExceptionCodes;
import com.barco.device.vng.utils.CommandLineUtils;
import com.barco.devicevng.MediaStore.MediaStoreDirectoryInfo;
import com.barco.devicevng.MediaStore.MediaStoreServiceUtils;
import com.barco.utils.FileUtils;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class MediaStoreServer{

	/**
	 * This method fills the store information in cache.
	 * UUID,  store revision and store service version
	 * @param storeDirInfo
	 * @throws Exception
	 */
	public void fillStoreInfo(MediaStoreDirectoryInfo storeDirInfo) throws Exception {

		//fill dir id information
		Document uuidDoc = getStoreUUIDDocument(storeDirInfo);
		storeDirInfo.setMediaStoreUUID(XMLUtils.getValueXpath(uuidDoc, "//" + MediaStoreServiceConst.UUID, ""));
		
		//fill revision, service version information
		Document mediaStoreDocument = getStoreRevisionDocument(storeDirInfo);
		storeDirInfo.setDirRevision(Utils.getIntValue(XMLUtils.getValueXpath(mediaStoreDocument, "//" + MediaStoreServiceConst.REVISION_ELEMENT_NAME, ""), 0));
		storeDirInfo.setMediaStoreServiceVer(XMLUtils.getAttributeValueWithDefault(mediaStoreDocument.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION, ""));
		if(storeDirInfo.isReadOnly() || !storeDirInfo.getDeviceServiceVersion().equals(storeDirInfo.getMediaStoreServiceVer())) {
			storeDirInfo.setWritable(false);			
		}

		Utils.getIntValue(XMLUtils.getValueXpath(mediaStoreDocument.getRootElement(), "//" + MediaStoreServiceConst.REVISION_ELEMENT_NAME, ""), 0);
		String[] listOfFilesInBlob = this.getListOfFiles(storeDirInfo);
		String state = MediaStoreServiceConst.READY;
		if(listOfFilesInBlob != null && listOfFilesInBlob.length != 0) {
			state = MediaStoreServiceConst.START_SYNCH;
		}
		storeDirInfo.setMediaStoreState(state);
	}

	private Document getStoreRevisionDocument(MediaStoreDirectoryInfo storeDirInfo) throws  Exception {

		if (!isStoreRevisionFileExists(storeDirInfo.getMediaStoreRootPath(), MediaStoreServiceConst.MEDIA_STORE_REVISION_FILE_NAME)) {
			Document storeRevisionFileDoc = XMLUtils.loadXMLResource("/com/ipvs/xsd/objectXml/MediaStoreInfo.xml");
			XMLUtils.addAttribute(storeRevisionFileDoc.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION, storeDirInfo.getDeviceServiceVersion());
			updateMediaStoreRevision(MediaStoreServiceConst.DEFAULT_REVISION, storeDirInfo.getMediaStoreRootPath(), storeRevisionFileDoc);
		} 

		String strMediaStoreDocument = FileUtils.readFileWithPath(storeDirInfo.getMediaStoreRootPath(), MediaStoreServiceConst.MEDIA_STORE_REVISION_FILE_NAME);

		try {
			return XMLUtils.stringToDocument(strMediaStoreDocument);
		} catch (Exception exp) {
			return null;
		}

	}


	private Document getStoreUUIDDocument(MediaStoreDirectoryInfo storeDirInfo) throws  Exception {
		int i =0;
		while(!isStoreRevisionFileExists(storeDirInfo.getMediaStoreRootPath(), MediaStoreServiceConst.DIR_UUID_FILE_NAME)){
			//wait for a min and check file exists or not
			Thread.sleep(6000);
			if(i++ >10)
				break;
			if (!isStoreRevisionFileExists(storeDirInfo.getMediaStoreRootPath(), MediaStoreServiceConst.DIR_UUID_FILE_NAME)) {
				FileUtils.writeFileWithPath(storeDirInfo.getMediaStoreRootPath(), MediaStoreServiceConst.DIR_UUID_FILE_NAME, "<UUID>"+UUID.randomUUID().toString()+"</UUID>");
			} 
		}

		String strMediaStoreDocument = FileUtils.readFileWithPath(storeDirInfo.getMediaStoreRootPath(), MediaStoreServiceConst.DIR_UUID_FILE_NAME);

		try {
			return XMLUtils.stringToDocument(strMediaStoreDocument);
		} catch (Exception exp) {
			return null;
		}

	}

	public void updateMediaStoreRevision(int revision, String rootDir,Document storeRevisionFileDoc) throws Exception {
		XMLUtils.setValueXpath(storeRevisionFileDoc, "//"+MediaStoreServiceConst.REVISION_ELEMENT_NAME, String.valueOf(revision));
		FileUtils.writeFileWithPath(rootDir, MediaStoreServiceConst.MEDIA_STORE_REVISION_FILE_NAME,storeRevisionFileDoc.asXML());
	}
	
	

	public void updateRevision(Integer storeRevision, MediaStoreDirectoryInfo storeDirInfo) throws Exception {
		Document storeRevisionFileDoc = getStoreRevisionDocument(storeDirInfo);
		updateMediaStoreRevision(storeRevision, storeDirInfo.getMediaStoreRootPath(), storeRevisionFileDoc);
	}


	/**
	 * this method will increase the revision number and update the cache in dir info object
	 * @param storeDirInfo
	 * @return TODO
	 * @throws Exception
	 */
	public Integer getNewRevision(MediaStoreDirectoryInfo storeDirInfo) throws Exception {
		Document storeRevisionFileDoc = getStoreRevisionDocument(storeDirInfo);
		Integer storeRevision = Utils.getIntValue(XMLUtils.getValueXpath(storeRevisionFileDoc, "//" + MediaStoreServiceConst.REVISION_ELEMENT_NAME, ""), 0);	    	
		storeRevision++;
		updateMediaStoreRevision(storeRevision, storeDirInfo.getMediaStoreRootPath(), storeRevisionFileDoc);
		return storeRevision;
	}
	/**
	 * this method returns the revision number 
	 * @param storeDirInfo
	 * @return TODO
	 * @throws Exception
	 */
	public Integer getCurrentRevision(MediaStoreDirectoryInfo storeDirInfo) throws Exception {
		Document storeRevisionFileDoc = getStoreRevisionDocument(storeDirInfo);
		Integer storeRevision = Utils.getIntValue(XMLUtils.getValueXpath(storeRevisionFileDoc, "//" + MediaStoreServiceConst.REVISION_ELEMENT_NAME, ""), 0);		
		return storeRevision;
	}

	// Checked the media store exists on the disk or not
	private boolean isStoreRevisionFileExists(String dirPath, String fileName) throws Exception {
		return FileUtils.isFileExist(dirPath, MediaStoreServiceConst.MEDIA_STORE_REVISION_FILE_NAME);
	}

	/**
	 * returns list of file objects in blob directory 
	 * @param dirInfo
	 * @return
	 * @throws Exception
	 */
	public String[] getListOfFiles(MediaStoreDirectoryInfo dirInfo) throws Exception {
		File file = new File(dirInfo.getMediaStoreRootPath()+ "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR);
		String[] mediaStoreFileArray = file.list();
		return mediaStoreFileArray;
	}

	/**
	 * check the version of media store file for given media store uuid and parent path
	 * @param dirInfo
	 * @param mediaStoreUUID
	 * @return
	 * @throws Exception
	 */
	public String getVersionOfMediaStoreFile(MediaStoreDirectoryInfo dirInfo, String mediaStoreUUID)
			throws  Exception {
		String parentDir = dirInfo.getMediaStoreRootPath() + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreUUID+"/";
		Document storeFileDoc = null;
		try {
			String fileContent = FileUtils.readFileWithPath(parentDir,  MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);
			storeFileDoc = XMLUtils.stringToDocument(fileContent);
		} catch (Exception exp) {
			//ignore exception
			exp.printStackTrace();
			storeFileDoc = null;
		}

		if (storeFileDoc == null) {
			return MediaVersionManager.MEDIA_STORE_VERSION_2_DOT_4;
		}

		String revisionValue = XMLUtils.getAttributeValueWithDefault(storeFileDoc.getRootElement(),MediaStoreServiceConst.REVISION,"");

		if ((revisionValue == null) || revisionValue.isEmpty()) {
			return MediaVersionManager.MEDIA_STORE_VERSION_3_DOT_2;
		}

		return  XMLUtils.getAttributeValueWithDefault(storeFileDoc.getRootElement(),MediaStoreServiceConst.SERVICE_VERSION,"0");
	} 
	  //This port will start the streaming server process to clean the deleted media
	    public void addDeleterPort(String deleterPortInstanceID,String trashDirPath, DeviceDBI deviceDB, Logger log) throws ServiceException,Exception {
	    	StringBuffer addDeleterPortCmd = new StringBuffer();
	    	addDeleterPortCmd.append("action=add ");
	    	addDeleterPortCmd.append("id="+deleterPortInstanceID +" ");
	    	addDeleterPortCmd.append("dsttype="+MediaRoomServiceConst.DELETER_PORT + " ");
	    	addDeleterPortCmd.append("deletedir=\"" + trashDirPath+ "\" ");    	
	        log.info("Add Deleter Port :="+deleterPortInstanceID);
	    
	        String commandResult = CommandLineUtils.executeStaticSSCommand(addDeleterPortCmd.toString(), deviceDB, log);
	        if ((commandResult == null) || commandResult.startsWith("201 OK")) {
	            throw new ServiceException(MediaRoomServiceExceptionCodes.EXECUTESTREAMINGSERVERCMDFAILED, "EXECUTE STREAMING SERVER COMMAND FAILED: RESULT = " + commandResult);
	        }       
	    }
	    
	    
	    //This port will Delete deleter port from streaming server
	    public void deleteDeleterPort(String deleterPortInstanceID,DeviceDBI deviceDB, Logger log) throws ServiceException,Exception {
	    	StringBuffer addDeleterPortCmd = new StringBuffer();
	    	addDeleterPortCmd.append("action=delete ");
	    	addDeleterPortCmd.append("id="+deleterPortInstanceID +" ");
	    	addDeleterPortCmd.append("dsttype="+MediaRoomServiceConst.DELETER_PORT + " ");    	    	
	        log.info("Delete Deleter Port:="+deleterPortInstanceID);
	    
	        String commandResult = CommandLineUtils.executeStaticSSCommand(addDeleterPortCmd.toString(), deviceDB, log);
	        if ((commandResult == null) || commandResult.startsWith("201 OK")) {
	            throw new ServiceException(MediaRoomServiceExceptionCodes.EXECUTESTREAMINGSERVERCMDFAILED, "EXECUTE STREAMING SERVER COMMAND FAILED: RESULT = " + commandResult);
	        }       
	    }
	    
	    public long getDirSizeUsed(MediaStoreDirectoryInfo storedirInfo, Logger log) throws Exception {
	    	Element statusEntryEL = null;

	    	Document diskStatusDoc = MediaStoreServiceUtils.getDiskStatusDoc(storedirInfo.isInternal(), log);

	    	statusEntryEL = MediaStoreServiceUtils.getDiskEntry(storedirInfo.getMediaStoreDeviceElementID(), diskStatusDoc);

	    	if (statusEntryEL != null) {
	    		diskStatusDoc = XMLUtils.elementToDocument(statusEntryEL);
	    	} else {
	    		diskStatusDoc = null;
	    	}

	    	if (diskStatusDoc == null) {
	    		throw new ServiceException(MediaStoreServiceExceptionCodes.GET_AVAILABLE_DISK_SPACE_FAILED, "CLI Command Result is Empty");
	    	}

	    
	    	String availableBytes = XMLUtils.getValueXpath(diskStatusDoc, "//Parameter[@name='Used']", "");
	    	
	    	Long dirSizeUsed = Utils.getLongValue(availableBytes, 0);
	    	storedirInfo.setSizeUsed(dirSizeUsed);
	    	return dirSizeUsed;

	    }

	    
	    public void checkAvailableDiskSpace(MediaStoreDirectoryInfo storedirInfo, Logger log) throws Exception {
	     	long availableDiskSpace = getAvailableDiskSpace(storedirInfo, log);
	        long diskFullThreshold = storedirInfo.getDiskFullThreshold();
	        if ((availableDiskSpace <= diskFullThreshold) || (availableDiskSpace <= storedirInfo.getDiskQuotaExceedLimit())) {
	            throw new ServiceException(MediaStoreServiceExceptionCodes.NO_AVAILABLE_DISK_SPACE, "No Disk space Available on Device To Record Media");
	        }
	    }
	    
	    public void checkAvailableDiskSpaceForPrepareDownload(long blobsize, MediaStoreDirectoryInfo storedirInfo, Logger log) throws Exception {
	    	// requires extra space to be available due to mkd file/ standard media file creation
	    	long availableDiskSpace = getAvailableDiskSpace(storedirInfo, log);
	        long diskFullThreshold = storedirInfo.getDiskFullThreshold();
	        long reservedDiskSize = storedirInfo.getReservedDiskSize();
	        if ((availableDiskSpace <= diskFullThreshold + (blobsize * 3)) || (availableDiskSpace <= storedirInfo.getDiskQuotaExceedLimit())) {
	            throw new ServiceException(MediaStoreServiceExceptionCodes.NO_AVAILABLE_DISK_SPACE, "No Disk space Available on Device To Record Media");
	        }
	        // Reserving size of fileUUID for preparing/cutlist of media to avoid race condition in handling multiple requests
	        storedirInfo.setReservedDiskSize(reservedDiskSize + blobsize);
	        
	        long sizeUsed = storedirInfo.getSizeUsed();
	        // Add the reserved size to the sizeUsed 
	        storedirInfo.setSizeUsed(sizeUsed + blobsize);
	        log.info("MediaStoreServer: CheckAvailableDiskSpaceforPrepareDownload: Reserve disk space : " + blobsize + ", SizeUsed is: " + storedirInfo.getSizeUsed());
	    }
	  
	    public long getAvailableDiskSpace(MediaStoreDirectoryInfo storedirInfo, Logger log) throws Exception {
	    	Element statusEntryEL = null;

	        Document diskStatusDoc = MediaStoreServiceUtils.getDiskStatusDoc(storedirInfo.isInternal(), log);

	        statusEntryEL = MediaStoreServiceUtils.getDiskEntry(storedirInfo.getMediaStoreDeviceElementID(), diskStatusDoc);

	        if (statusEntryEL != null) {
	            diskStatusDoc = XMLUtils.elementToDocument(statusEntryEL);
	        } else {
	            diskStatusDoc = null;
	        }

	        if (diskStatusDoc == null) {
	            throw new ServiceException(MediaStoreServiceExceptionCodes.GET_AVAILABLE_DISK_SPACE_FAILED, "CLI Command Result is Empty");
	        }

	        String status = XMLUtils.getValueXpath(diskStatusDoc, "//Parameter[@name='Status']", "");

	        String availableBytes = XMLUtils.getValueXpath(diskStatusDoc, "//Parameter[@name='Available']", "");

	        if (((status == null) || status.equals("False") || !status.equals("Ok")) && ((availableBytes == null) || (availableBytes.length() == 0))) {
	            throw new ServiceException(MediaStoreServiceExceptionCodes.GET_AVAILABLE_DISK_SPACE_FAILED, "Command Status Falied ");
	        }

	        long availableDiskSpace = Utils.getLongValue(availableBytes, 0);
	        long reservedDiskSize = storedirInfo.getReservedDiskSize();
	        availableDiskSpace = availableDiskSpace - reservedDiskSize;
	        log.info("Available Disk Space on Device : " + availableDiskSpace + " Bytes");
	       
	        return availableDiskSpace;
	    }
	    
	    public void updateMediaStoreServiceVersionAndRevesion(String serviceVersion, int revision, String rootDir,Document storeRevisionFileDoc) throws Exception {
	        XMLUtils.setValueXpath(storeRevisionFileDoc, "//"+MediaStoreServiceConst.REVISION_ELEMENT_NAME, String.valueOf(revision));
            XMLUtils.addAttribute(storeRevisionFileDoc.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION, serviceVersion);
            FileUtils.writeFileWithPath(rootDir, MediaStoreServiceConst.MEDIA_STORE_REVISION_FILE_NAME,storeRevisionFileDoc.asXML());
        }
        
        

        public void updateServiceVersionAndRevesion(String serviceVersion, int revision, MediaStoreDirectoryInfo storeDirInfo) throws Exception {
            Document storeRevisionFileDoc = getStoreRevisionDocument(storeDirInfo);
            updateMediaStoreServiceVersionAndRevesion(serviceVersion, revision, storeDirInfo.getMediaStoreRootPath(), storeRevisionFileDoc);
        }
	    

}
