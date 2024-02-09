package com.barco.devicevng.MediaStore.StoreServer;

import java.util.HashMap;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.devicevng.MediaStore.MediaStoreDirectoryInfo;

public interface MediaStoreFileI {
	
	/**
	 * this method returns media store file document 
	 * @param dirInfo
	 * @param mediaStoreFileID
	 * @return
	 * @throws Exception
	 */
	public Document getMediaStoreFile(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID) throws Exception;	
	
	/**
	 * 
	 * @param dirInfo
	 * @param mediaStoreFileID
	 * @param bookmarkID
	 * @return
	 * @throws Exception
	 */
	public Document getMediaStoreBookmark(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID, String bookmarkID) throws Exception;	
	
	
	/**
	 * This method will update store file document 
	 * @param dirInfo
	 * @param mediaStoreFileID
	 * @param mediaStoreFileDoc
	 * @param revision
	 * @param logger TODO
	 * @throws Exception
	 */
	
	public void updateMediaStoreFile(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID, Document mediaStoreFileDoc,String revision, Logger logger) throws Exception;
	
	/**
	 * 
	 * @param dirInfo
	 * @param mediaStoreFileID
	 * @param bookmarkID
	 * @param mediaStoreBookmarkDoc
	 * @param revision
	 * @param logger
	 * @throws Exception
	 */
	public void updateMediaStoreBookmark(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String bookmarkID,Document mediaStoreBookmarkDoc, Logger logger) throws Exception; 
	
    /**
     * This method returns the size of given store file 
     * @param dirInfo
     * @param mediaStoreFileID
     * @param logger
     * @return
     * @throws Exception
     */
	public long getMediaStoreFileSize(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID,Logger logger) throws Exception;

	  /**
	     * This method returns the playlength of given store file 
	     * @param dirInfo
	     * @param mediaStoreFileID
	     * @param logger
	     * @return
	     * @throws Exception
	     */
		public long getMediaPlayLegth(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID,Logger logger) throws Exception;
		
	
	/**
	 * this method repair media store file  if file state is in error or busy (dms reboots while recording) state
	 * @param dirInfo
	 * @param mediaStoreFileID
	 * @param logger
	 * @return MediaStoeFileDoc
	 * @throws Exception
	 */
	
	public Document repairMediaStoreFile(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID,Document mediaStoreFileDoc, String revision,Logger logger) throws Exception;
	
	
	/**
	 * returns list of bookmark objects in blob directory 
	 * @param dirInfo
	 * @param mediaStoreFileID
	 * @return
	 * @throws Exception
	 */
	public HashMap<String, Element> getListOfBookmarks(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID) throws Exception ;


	/**
	 * Create media store file in blob
	 * @param dirInfo
	 * @param mediaStoreFileID
	 * @param mediaStoreFileDoc TODO
	 * @param logger
	 * @throws Exception
	 */
	public void createMediaStoreFile(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID,Document mediaStoreFileDoc, Logger logger) throws Exception;
	
	/**
	 * 
	 * @param dirInfo TODO
	 * @param fileUUID
	 * @param trashDirName
	 * @return
	 * @throws Exception
	 */
	  public void deleteFile(MediaStoreDirectoryInfo dirInfo, String fileUUID, String trashDirName, Logger logger)
			    throws Exception;
	  
	  
	  public void createSnapshot(String storePath, String currentTime, String filePath, String snapshotFilePath, String extension) 
				throws Exception;
	
	  /**
	   * 
	   * @param rootdir
	   * @param mediaUUID
	   * @param byDirID
	   * @param logger
	   * @throws Exception
	   */
	  public void createBookmarkInMediaStore(MediaStoreDirectoryInfo storeDirInfo, String mediaUUID, String bookmarkUUID, Document bookmarkFileDoc, Logger logger) throws Exception;

	  /**
	   * 
	   * @param mediaUUID
	   * @param offset
	   * @param rootDir
	   * @return
	   * @throws Exception
	   */
	  public long getTCAtOffset(String mediaUUID, long offset,MediaStoreDirectoryInfo storeDirInfo, DeviceDBI deviceDB, Logger log) throws Exception;   

	  /**
	   * 
	   * @param timecode
	   * @param mediaUUID
	   * @param srcDirPath
	   * @return
	   * @throws ServiceException
	   * @throws Exception
	   */
	  public long getOffsetAtTC(long timecode,String mediaUUID,MediaStoreDirectoryInfo storeDirInfo, DeviceDBI deviceDB, Logger log) throws ServiceException,Exception;
	  
	  /**
	   * 
	   * @param dirInfo
	   * @param fileUUID
	   * @param paramBoolean
	   * @param logger
	   * @throws Exception
	   */
	  
	  public void deleteBookmark(MediaStoreDirectoryInfo dirInfo, String bookmarkUUID, String mediaUUID,Logger logger)throws Exception;
	  
	  public void upgradeFile(MediaStoreDirectoryInfo dirInfo, String fileUUID,Document mediaStoreFileDoc, Logger logger) throws Exception;

	  boolean isMediaStoreBookmarkExist(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String bookmarkID)throws Exception;
	
	  boolean isStaleFileExist(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String format) throws Exception;
	
	  
	  public void updateStateFlag(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID) throws Exception;	
	
	  public void addStaleFile(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID, String format) throws Exception;
	
	  public boolean isDownloadFormatStale(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String formatType) throws Exception;
	
	  public void addDownloadTagForFormat(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, Element downloadEl, String state, String format) throws Exception;
	
	  public void deleteStaleFile(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID, String format) throws Exception;
	
}
