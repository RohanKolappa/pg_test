/*
 * 
 */
package com.barco.device.agent;

import java.util.HashMap;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;


// TODO: Auto-generated Javadoc
/**
 * The Interface MediaStoreAgentI.
 */
public interface MediaStoreAgentI  extends AgentI{
	
	/**
	 * DIRECTORY API.
	 * 
	 */
	
	/**
	 * handles the directory available notification, add media stream dir and update port presence 
	 * @param dirID
	 */
	public void handleDirAvailable(String dirID);
	
	
	/**
	 * handles the directory unavailable notification , delete media store port
	 * information and mark dir unavailable.
	 * 
	 * @param dirID
	 *            the dir id
	 */
	public void handleDirUnavailable(String dirID);
	
	
	/**
	 * this method will start the sync process
	 * @param dirId
	 * @param maxRevision
	 * @param dirNID
	 * @param pvrDirNID
	 * @param recycleBinNID
	 * @throws Exception
	 */
	public void startSynch(String dirId,String maxRevision,String dirNID,String pvrDirNID,String recycleBinNID, Integer index) throws Exception;
	
	
	
	/**
	 * This method will update directory information in the media store cache
	 * @param dirId
	 * @param dirNID
	 * @param pvrDirNID
	 * @param recycleBinNID
	 * @param dirThreshold TODO
	 * @param state
	 * @throws Exception
	 */
	public void updateDBInformation(String dirId,String dirNID,String pvrDirNID,String recycleBinNID, Long dirThreshold, String state) throws Exception;
	
	
	
	
	/**
     * Upgrade directory to latest server version. If the upgradeFiles true then
     * will upgrade all the files in the media store service
     * 
     * @param dirID
     *            the dir id
     * @param dirNID
     *            the dir nid
     * @param upgradeFiles
     *            the upgrade files
     * @throws Exception
     *             the exception
     */
    public Document upgradeDir(String dirID,String dirNID) throws Exception;
    
    /**
     * This method upgrades file in the blob
     * @param dirId
     * @param dirNID
     * @param fileUUID
     * @param fileNID
     * @param mediaStoreFileDoc
     * @return
     * @throws Exception
     */
    public Element upgradeFile(String dirId, String dirNID,String fileNID, String fileUUID, Document mediaStoreFileDoc) throws Exception; 
    
	
	
	/**
	 * Update disk size used in media store cache.
	 * 
	 * @param dirID
	 *            the dir id
	 * @param diskSizeUsed
	 *            the disk size used
	 * @throws Exception
	 *             the exception
	 */
	public void updateDiskSizeUsed(String dirID, long diskSizeUsed) throws Exception;
	
	/**
	 * FILE API.
	 */
	
	
	/**
	 * Add media store file in blob
	 * @param dirID
	 * @param dirNID TODO
	 * @param mediaStoreDocument
	 * @return store revision to be update in database stream file
	 */
	public Document addFile(String dirID,String dirNID,String fileUUID, Document mediaStoreFileDocument) throws Exception;
	
	
	/**
	 * update given information in media store.
	 * 
	 * @param dirID
	 *            the dir id
	 * @param dirNID
	 *            the dir nid
	 * @param fileNID
	 *            the file nid
	 * @param fileUUID
	 *            the file uuid
	 * @param mediaStoreFileDocument
	 *            the media store file document
	 * @return store revision tobe update in database stream file
	 * @throws Exception
	 *             the exception
	 */
	public Document setFileRequest(String dirID, String dirNID, String fileNID, String fileUUID, Document mediaStoreFileDocument) throws Exception;
	
	
	/**
	 * update the file playlength and size in store.
	 * 
	 * @param dirID
	 *            the dir id
	 * @param dirNID
	 *            the dir nid
	 * @param fileUUID
	 *            the file uuid
	 * @param state TODO
	 * @param size
	 *            the size
	 * @param playlength
	 *            the playlength
	 * @throws Exception
	 *             the exception
	 */
	public void setFilePlaylengthAndSize(String dirID,String dirNID,String fileUUID,String state,long size, long playlength) throws Exception;
	
	
	/**
	 * Delete permanently from disk or move to trash directory. From trash
	 * directory streaming server deleter port will delete
	 * 
	 * @param dirID
	 *            the dir id
	 * @param dirNID
	 *            the dir nid
	 * @param fileNID
	 *            the file nid
	 * @param fileUUID
	 *            the file uuid
	 * @param purgeFile
	 *            : true deletes files from disk immediately else move to trash
	 *            dir
	 * @throws Exception
	 *             the exception
	 */
	public void deleteFile(String dirID,String dirNID,String fileNID,String fileUUID,boolean purgeFile) throws Exception;
	
	
	/**
	 * Returns Media Store File Document.
	 * 
	 * @param dirID
	 *            the dir id
	 * @param dirNID TODO
	 * @param fileUUID
	 *            the file uuid
	 * @return MediaStoreFileDocument
	 * @throws Exception
	 *             the exception
	 */
	public Document getMediaStoreFileDocument(String dirID,String dirNID, String fileUUID) throws Exception;
	
	/**
	 * 
	 * @param dirID
	 * @param dirNID
	 * @param fileUUID
	 * @return
	 * @throws Exception
	 */
	public Document getMediaStoreBookmarkDocument(String dirID, String dirNID, String fileUUID, String bookkmarUUID) throws Exception;
	
	/**
	 * This method return the directory path.  Example /data/DSS/data/portal/site/default
	 * @param dirNID
	 * @param dirID
	 * @return
	 */
	public String getDirPath(String dirNID,String dirID) throws Exception;
	
	/**
	 * 
	 * @param dirPath
	 * @return
	 * @throws Exception
	 */
	public String getDirIDFromDirPath(String dirPath) throws Exception;
	
			
	/**
	 * prepareFileForDownload
	 * 
	 * This method prepares the file for download in the requested format
	 * native or standard
	 * @param fileNID
	 *            the NID of the file
	 * @param fileUUID
	 *            the UUID of the file
	 * @param dirNID
	 * 			  the directory NID of the file           
	 * @param mediaFormat
	 * 			  Proprietary/Standard/Both
	 * @param startOffset
	 * 			  time offset from start for PVR
	 * @param endOffset
	 * 			  time offset from end for PVR
	 * @param requestTimeStamp
	 * 			  time when the request was created
	 * @param new_fileUUID
	 * 			  new UUID for the file.
	 * @throws Exception
	 *             the exception
	 */
	public void prepareFileForDownload(String fileNID, String fileUUID, String dirNID, String mediaFormat, String startOffset, String endOffset,  String requestTimeStamp, String type, String mediaGroupPlaylength, String new_fileUUID) throws Exception;
	
	/**
	 * 
	 * @param dirID
	 * @param dirNID
	 * @param tc
	 * @param offset
	 * @param mediaUUID
	 * @param uuid
	 * @param mediaStoreFileDoc
	 * @return
	 * @throws Exception
	 */
	
	public Document addBookmark(String dirID, String dirNID,String mediaUUID,String bookmarkUUID, String clipUUID, Document bookmarkFileDoc) throws Exception;

/**
 * 
 * @param dirID
 * @param dirNID
 * @param mediaUUID
 * @param fileUUID
 * @throws Exception
 */
	public void deleteBookmark(String dirID, String dirNID, String mediaUUID, String bookmarkUUID)	throws Exception;

	/**
	 * 
	 * @param dirID
	 * @param dirNID
	 * @param mediaUUID
	 * @param bookmarkUUID
	 * @param mediaStoreBookmarkDocument
	 * @return
	 * @throws Exception
	 */
	public Document updateBookmark(String dirID, String dirNID, String mediaUUID, String bookmarkUUID, Document mediaStoreBookmarkDocument, String timecode, String offSet, String synchClipId) throws Exception;

		
	/**
	 * this method will take care of updating disk size and sending update to SC
	 * @param dirID
	 * @param dirNID
	 * @param sizeDeltaAdded
	 * @param sizeDeltaDeleted
	 * @param sizeUnreserved
	 * @param publishSizeUpdate
	 * @throws Exception
	 */
	public void diskSpaceUpdate(String dirID, String dirNID, long sizeDeltaAdded,long sizeDeltaDeleted, long sizeUnreserved, boolean publishSizeUpdate) throws Exception;
	
	
	/**
	 * Checks disk free space reaches to the disk quota. Default disk quota limit is 5GB.
	 * @param dirID
	 * @param dirNID
	 * @return true/false
	 * @throws Exception
	 */
	public boolean isDiskFreeSpaceReachesToQuotaLimit(String dirID,String dirNID) throws Exception;
	
	
	/**
	 * checks disk size exceeds the threshold. 
	 * @param dirID
	 * @param dirNID
	 * @return true if it exceeds threshold limit otherwise false
	 * @throws Exception
	 */
	
	public boolean isDiskSizeExceedsThreshold(String dirID,String dirNID) throws Exception;
	
	
	
	/**
	 * This method updates directory threshold
	 * @param dirId
	 * @param dirNID
	 * @param dirThreshold TODO
	 * @throws Exception
	 */
	public void updateDirThreshold(String dirId,String dirNID, Long dirThreshold) throws Exception;
	
	/**
	 * 
	 * @param dirID
	 * @param dirNID
	 * @param mediaUUID
	 * @param tc
	 * @return
	 * @throws Exception
	 */
	public long getOffset(String dirID, String dirNID, String mediaUUID, long tc) throws Exception;
	
	/**
	 * 
	 * @param dirID
	 * @param dirNID
	 * @param mediaUUID
	 * @param offset
	 * @return
	 * @throws Exception
	 */
	public long getTimecode(String dirID, String dirNID, String mediaUUID, long offset) throws Exception;

	/**
	 * 
	 * @param dirID
	 * @param dirNID
	 * @param fileUUID
	 * @return
	 * @throws Exception
	 */
	public long getFileSize(String dirID, String dirNID, String mediaUUID) throws Exception;


    boolean isMediaStoreBookmarkExist(String dirID, String dirNID,String mediaStoreFileID, String bookmarkID)
            throws Exception;
  
	public void offloadFileToNAS(String dirNID, String dirID, String format, String nasURL, String fileNID, String fileUUID) throws Exception;

	
	public HashMap<String, Element> getMediaStoreBookMarks (String dirID, String dirNID,String mediaStoreFileID) throws Exception;


	void unpackImportFile(String dirID, String dirNID, String mediaUUID, String dateModified, String tempFolderPath, String queueID, String groupID,String mediaGroupNID, String isOldMediaGroup, String groupTitle) throws Exception;
	public Document createMediaFromCutList(String byNID, String dirID, String cutList, String title, String fileUUID, String dirNID, String mediaFormat, String new_fileNID, String new_UUID, Document mediaStoreFileDoc) throws Exception;

	public void clearDownloadCache(String byNID, String dirID, String fileUUID, String dirNID) throws Exception;
	public Document ingestMedia(String filePath, String currentTime, String dirId, String dirNID, String fileUUID, Document mediaStoreFileDocument, String hlsenable) throws Exception;
	public void trimMediaFromCutList(String byNID, String dirNID, String dirID, String cutlist, String fileUUID) throws Exception;

	/**
	 * Start Posting Thumbnail.
	 *
	 * @param sourceNID
	 *            the source NID
	 * @param portId TODO
	 * @param publishURL
	 * 			  the publish URL
	 * @throws Exception
	 *             the exception
	 */
	public void startPostingThumbnail(String sourceNID, String portId, String publishURL) throws Exception;
	

	/**
	 * Abort an ongoing import/export
	 *
	 * @param newUUID
	 *            new uuid for media
	 * @param oldUUID
	 * 			  old uuid for media
	 * @param dirNID
	 *            NID for directory containing media
	 * @param type
	 *            export/import/offload
	 * @param format
	 *            format of packed media
	 *            
	 * @throws Exception
	 *             the exception
	 */
	public void abortMediaPackagingProcess(String newUUID, String oldUUID, String dirNID, String type, String format) throws Exception;
	
	/**
	 * This method will initiate the delete file process which will create deleter port and ask the deleter port to do the job. Mainly used for ILM delete rule
	 * @param dirID
	 * @param dirNID
	 * @param fileList
	 * @throws Exception
	 */
	public void startPurgeFileProcess(String dirID, String dirNID,List<?> fileList) throws Exception;
	
	
	/**
	 * After files got deleted streaming server sends notification after cleaning the disk space. Store agent will stop the deleter port and send update to SC.
	 * @param dirID
	 * @param dirNID
	 * @throws Exception
	 */	
	public void stopPurgeFileProcess(String  id, String dirID, String dirNID) throws Exception;
	
	boolean isMediaUpgraded(String dirID, String dirNID, String mediaStoreFileID) throws Exception;
}
