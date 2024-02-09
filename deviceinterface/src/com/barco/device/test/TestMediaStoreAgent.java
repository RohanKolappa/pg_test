package com.barco.device.test;

import java.util.HashMap;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.MediaStoreAgentI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.utils.XMLUtils;

public class TestMediaStoreAgent implements MediaStoreAgentI{
	DeviceFrameworkI deviceFramework = null;
	@Override
	public void initAgent(DeviceFrameworkI deviceFramework) {
		this.deviceFramework = deviceFramework;
	}

	@Override
	public Document getPublishInfo() throws Exception {
		
		Document storeServicedoc = XMLUtils.stringToDocument("<"+DeviceAdminConst.MEDIASTORE_SERVICE_INFO+"/>");
		
		Element dirElement = DocumentHelper.createElement("Dir");

		XMLUtils.addAttribute(dirElement,"uuid", deviceFramework.getMyJID());	
		XMLUtils.addAttribute(dirElement, "id", MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME);
		XMLUtils.addAttribute(dirElement, "mediaStoreElementId", MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME);
		XMLUtils.addAttribute(dirElement, "mediaStoreRevision",""+MediaStoreServiceConst.DEFAULT_REVISION);
		XMLUtils.addAttribute(dirElement, "mediaStoreServiceVersion", deviceFramework.getServiceVersion());
		XMLUtils.addAttribute(dirElement, "size",String.valueOf(Long.valueOf(MediaStoreServiceConst.LIMIT_ON_AVAILABLE_DISK_SPACE) * 5)  );
		XMLUtils.addAttribute(dirElement, "sizeUsed", "0");
		XMLUtils.addAttribute(dirElement, "readOnly", MediaStoreServiceConst.FALSE);
		XMLUtils.addAttribute(dirElement, "needsUpgrade", MediaStoreServiceConst.FALSE);
		XMLUtils.addAttribute(dirElement, "internal", MediaStoreServiceConst.TRUE);
		XMLUtils.addAttribute(dirElement,"state",MediaStoreServiceConst.READY);
		XMLUtils.addAttribute(dirElement, "mediaStoreName", "DummyDir");	
		storeServicedoc.getRootElement().add(dirElement);
		
		return storeServicedoc;

	}

	@Override
	public void handleDirAvailable(String dirID) {
	}

	@Override
	public void handleDirUnavailable(String dirID) {
		
	}

	@Override
    public Document upgradeDir(String dirID, String dirNID) throws Exception {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public Element upgradeFile(String dirId, String dirNID,String fileNID, String fileUUID, Document mediaStoreFileDoc) throws Exception {
        // TODO Auto-generated method stub
        return null;
    }
	@Override
	public void updateDiskSizeUsed(String dirID, long diskSizeUsed)
			throws Exception {
		
	}

	@Override
	public Document addFile(String dirID, String dirNID, String fileUUID, Document mediaStoreFileDocument) throws Exception {
		return DocumentHelper.createDocument(DocumentHelper.createElement("File"));
	}

	@Override
	public Document setFileRequest(String dirID, String dirNID, String fileNID,
			String fileUUID, Document mediaStoreFileDocument) throws Exception {
		return DocumentHelper.createDocument(DocumentHelper.createElement("File"));
	}

	@Override
	public void deleteFile(String dirID, String dirNID, String fileNID,
			String fileUUID, boolean purgeFile) throws Exception {		
	}

	@Override
	public Document getMediaStoreFileDocument(String dirID, String dirNID, String fileUUID) throws Exception {
		return null;
	}

	@Override
	public void setFilePlaylengthAndSize(String dirID, String dirNID,
			String fileUUID, String state, long size, long playlength) throws Exception {
	}

	@Override
	public Document getStatus(String xpath) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void startSynch(String dirId, String maxRevision, String dirNID,
			String pvrDirNID, String recycleBinNID, Integer index) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void updateDBInformation(String dirId, String dirNID,
			String pvrDirNID, String recycleBinNID, Long dirThreshold, String state)
			throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public String getDirPath(String dirNID, String dirID) throws Exception {
		// TODO Auto-generated method stub
		return "/data/DSS/data/portal/site/default";
	}

	@Override
	public String getDirIDFromDirPath(String dirPath) throws Exception {
		// TODO Auto-generated method stub
		return "";
	}
	
	@Override
	public void deleteBookmark(String dirID, String dirNID, String mediaUUID, String bookmarkUUID) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public Document getMediaStoreBookmarkDocument(String dirID, String dirNID, String fileUUID, String bookkmarUUID) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document updateBookmark(String dirID, String dirNID, String mediaUUID, String bookmarkUUID, Document mediaStoreBookmarkDocument, String timecode, String offSet, String synchClipId) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document addBookmark(String dirID, String dirNID, String mediaUUID, String bookmarkUUID, String clipUUID, Document bookmarkFileDoc) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public void diskSpaceUpdate(String dirID, String dirNID, long sizeDeltaAdded,
			long sizeDeltaDeleted, long sizeUnreserved, boolean publishSizeUpdate) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean isDiskFreeSpaceReachesToQuotaLimit(String dirID,
			String dirNID) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isDiskSizeExceedsThreshold(String dirID, String dirNID)
			throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void updateDirThreshold(String dirId, String dirNID,
			Long dirThreshold) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public long getOffset(String dirID, String dirNID, String mediaUUID, long tc) throws Exception {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public long getTimecode(String dirID, String dirNID, String mediaUUID, long offset) throws Exception {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public long getFileSize(String dirID, String dirNID, String mediaUUID) throws Exception{
		// TODO Auto-generated method stub
		return 0;
	}

    @Override
    public boolean isMediaStoreBookmarkExist(String dirID, String dirNID, String mediaStoreFileID, String bookmarkID)
            throws Exception {
        // TODO Auto-generated method stub
        return false;
    }

	@Override
	public void offloadFileToNAS(String dirNID, String dirID, String format, String nasURL, String fileNID, String fileUUID) throws Exception {
		// TODO Auto-generated method stub
		
	}
	@Override
	public HashMap<String, Element> getMediaStoreBookMarks(String dirID, String dirNID, String mediaStoreFileID) throws Exception {
	    // TODO Auto-generated method stub
	    return null;
	}

	@Override
	public void unpackImportFile(String dirID, String dirNID, String mediaUUID, String dateModified, String tempFolderPath, String queueID, String groupID,String mediaGroupNID, String isOldMediaGroup, String groupTitle) throws Exception {
	    // TODO Auto-generated method stub
	    
	}

	@Override
	public void prepareFileForDownload(String fileNID, String fileUUID, String dirNID, String mediaFormat, String startOffset, String endOffset, String requestTimeStamp,
			String type, String mediaGroupPlaylength, String new_fileUUID) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public Document createMediaFromCutList(String byNID, String dirID, String cutList, String title, String fileUUID, String dirNID, String mediaFormat, String new_fileNID, String new_UUID, Document mediaStoreFileDoc)
			throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void clearDownloadCache(String byNID, String dirID, String fileUUID, String dirNID) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public Document ingestMedia(String filePath, String currentTime, String dirId, String dirNID, String fileUUID, Document mediaStoreFileDocument, String hlsenable) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void trimMediaFromCutList(String byNID, String dirNID, String dirID, String cutlist, String fileUUID) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void startPostingThumbnail(String sourceNID, String portId, String publishURL) throws Exception {
	    // TODO Auto-generated method stub
	    
	}

	@Override
	public void reset() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void abortMediaPackagingProcess(String newUUID, String oldUUID,
			String dirNID, String type, String format) throws Exception {
		// TODO Auto-generated method stub
		
	}
	
	@Override
	public void startPurgeFileProcess(String dirID, String dirNID,
			List<?> fileList) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void stopPurgeFileProcess(String id, String dirID, String dirNID)
			throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean isMediaUpgraded(String dirID, String dirNID,
			String mediaStoreFileID) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}
}
