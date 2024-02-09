package com.barco.devicevng.MediaStore.StoreServer;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.HashMap;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.device.handler.MediaStore.MediaStoreServiceExceptionCodes;
import com.barco.device.handler.MediaStore.MediaStoreUtils;
import com.barco.device.vng.utils.CommandLineUtils;
import com.barco.devicevng.MediaStore.MediaStoreDirectoryInfo;
import com.barco.devicevng.MediaStore.MediaStoreServiceUtils;
import com.barco.utils.FileUtils;
import com.barco.utils.MediaRoomServiceExceptionCodes;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;


public class MediaStoreFileImpl implements MediaStoreFileI{


	@Override
	public Document getMediaStoreFile(MediaStoreDirectoryInfo dirInfo,
			String mediaStoreFileID) throws Exception {
		String mediaStoreFile = FileUtils.readFileWithPath(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/", MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);
		return XMLUtils.stringToDocument(mediaStoreFile);
	}

	@Override
	public Document getMediaStoreBookmark(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID, String bookmarkID) throws Exception{
		String mediaStoreFile = FileUtils.readFileWithPath(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/bookmarks/", bookmarkID+".xml");
		return XMLUtils.stringToDocument(mediaStoreFile);

	}
	@Override
	public boolean isMediaStoreBookmarkExist(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID, String bookmarkID) throws Exception{
		String bookmarkPath = dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/bookmarks/";
		//in case bookmark directory does not exist create new (in case of upgrade)
		File fileObject = new File(bookmarkPath); 
		if(!fileObject.exists()) {
			fileObject.mkdirs();
		}
		return FileUtils.isFileExist(bookmarkPath, bookmarkID+".xml");

	}

	@Override
	public void updateMediaStoreFile(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, Document mediaStoreFileDoc, String revision, Logger logger) throws Exception {
		XMLUtils.addAttribute(mediaStoreFileDoc.getRootElement(), MediaStoreServiceConst.REVISION, String.valueOf(revision));		
		FileUtils.writeFileWithPath(dirInfo.getMediaStoreRootPath()+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/", MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME, mediaStoreFileDoc.asXML());
		updateStateFlag(dirInfo, mediaStoreFileID);
		logger.debug("MediaStoreFileImpl: Update Media store file "+mediaStoreFileID +" revision:="+revision);
	}

	@Override
	public void updateMediaStoreBookmark(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String bookmarkID,Document mediaStoreBookmarkDoc, Logger logger) throws Exception {
		FileUtils.writeFileWithPath(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/bookmarks/", bookmarkID+".xml", mediaStoreBookmarkDoc.asXML());
		updateStateFlag(dirInfo, mediaStoreFileID);
		logger.debug("MediaStoreFileImpl: Update Media store Bookmark "+bookmarkID);
	}

	@Override
	public long getMediaStoreFileSize(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, Logger logger) throws Exception {

		String mediaStoreFilePath = dirInfo.getMediaStoreRootPath() + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileID;
		File storeFileObject = new File(mediaStoreFilePath);
		return FileUtils.getDirectorySize(storeFileObject);
	}

	@Override
	public Document repairMediaStoreFile(MediaStoreDirectoryInfo dirInfo,
			String mediaStoreFileID, Document mediaStoreFileDoc,String revision,Logger logger) throws Exception {
		//repair the index of the busy file	    
		String parentDir = dirInfo.getMediaStoreRootPath() + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
		logger.debug("MediaStoreFileImple: repairMediaStoreFile: MediaStoreFileID: "+mediaStoreFileID);		
		int repairStatus = 0;
		if((MediaStoreServiceConst.BUSY).equals(XMLUtils.getValueXpath(mediaStoreFileDoc, "//State", ""))) {
			String filePath = parentDir + "/" + mediaStoreFileID + "/" + MediaStoreServiceConst.MEDIA_STORE_INDEX_FILE_NAME;
			String[] cmdarr = { "/bin/sed", "-i", "-e", "1 s/-1/-0/", filePath };
			repairStatus = CommandLineUtils.executeSystemCommand(cmdarr);
		}
		if(repairStatus !=0 ){			
			XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.STATE_EL_NAME, MediaStoreServiceConst.ERROR);
		}else{			
			XMLUtils.setValueXpath(mediaStoreFileDoc, "//"+MediaStoreServiceConst.STATE_EL_NAME, MediaStoreServiceConst.READY);
		}
		
		Long playlength = getMediaPlayLegth(dirInfo, mediaStoreFileID, logger);
		XMLUtils.setValueXpath(mediaStoreFileDoc,"//"+MediaStoreServiceConst.PLAY_LENGTH, String.valueOf(playlength));
		updateMediaStoreFile(dirInfo, mediaStoreFileID, mediaStoreFileDoc,revision, logger);		
		return mediaStoreFileDoc;
	}

	@Override
	public void createMediaStoreFile(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, Document mediaStoreFileDoc, Logger logger) throws Exception {
		String mediaStoreFilePath = dirInfo.getMediaStoreRootPath() + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileID+"/";
		File fileObject = new File(mediaStoreFilePath);
		if(fileObject.exists()) {
			throw  new ServiceException(MediaStoreServiceExceptionCodes.DUPLICATE_MEDIA_STORE_NID, "Duplicate Media Store");			   
		}

		fileObject.mkdirs();	
		fileObject = new File(mediaStoreFilePath +"/bookmarks");	    
		fileObject.mkdirs();
		FileUtils.writeFileWithPath(mediaStoreFilePath, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME, mediaStoreFileDoc.asXML());

		logger.debug("MediaStoreFileImpl:Crate MediaStore "+mediaStoreFileID);
	}
	//get the play length of given media store file
	@Override
	public long getMediaPlayLegth(MediaStoreDirectoryInfo dirInfo,String mediaStoreFileID,Logger logger)
			throws Exception {

		long playLegth;
		try {
			String mediaStoreFilePath = dirInfo.getMediaStoreRootPath() + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileID+"/";
			Element GapInfoEL = getMediaStoreGapInfo(mediaStoreFilePath);
			Document gapInfoDoc = XMLUtils.elementToDocument(GapInfoEL);
			List<?> segmentInfoList = XMLUtils.getListOfNodesXpath(gapInfoDoc, "//MediaSegmentInfo");
			long maxOffset = 0;
			long mediaOffset = 0;

			for (Object segmentInfoObject : segmentInfoList) {
				Element segmentInfoEL = (Element) segmentInfoObject;
				mediaOffset = Utils.getLongValue(XMLUtils.getValueXpath(segmentInfoEL, "MediaOffset", ""), 0);

				if (mediaOffset >= maxOffset) {
					maxOffset = mediaOffset;
				}
			}

			Element lastSegmentEL = XMLUtils.getElementXpath(gapInfoDoc, "//MediaSegmentInfo[MediaOffset='" + maxOffset + "']");
			long startTC = Utils.getLongValue(XMLUtils.getValueXpath(lastSegmentEL, "StartTC", ""), 0);
			long endTC = Utils.getLongValue(XMLUtils.getValueXpath(lastSegmentEL, "EndTC", ""), 0);
			playLegth = maxOffset;

			if ((endTC != -1) && (endTC != 0)) {
				playLegth = maxOffset + (endTC - startTC);
			}
			return playLegth;
		} catch (Exception e) {
			return 0;
		}

	}
	//returns the gap info of given media store file.
	protected Element getMediaStoreGapInfo(String mediaStoreFilePath) throws Exception {
		Element mediaSegmentTrackEL = XMLUtils.stringToElement("<" + MediaStoreServiceConst.MEDIA_TRACK+ "/>"); 

		File file = new File(mediaStoreFilePath, MediaStoreServiceConst.MEDIA_STORE_INDEX_FILE_NAME);

		if (!file.exists()) {
			return mediaSegmentTrackEL;
		}

		BufferedReader input = new BufferedReader(new FileReader(file));
		String line = null;
		int noOfLinesRead = 1;
		long recordStartTC = 0;

		long recordEndTC = 0;
		int index = 0;

		long startTC = -1;
		long endTC = -1;
		long mediaOffset = -1;
		Element mediaSegmentTrackItemEL = null;
		Document mediaSegmentInfoDoc = MediaStoreServiceUtils.loadMediaSegmentInfoDocument();

		/*
		 * INDEX file Structure
		 * StartTC - 0/1 (0 means stopped 1 means started)
		 * StartTC - EndTC
		 * Gap1_StartTC - Gap1_EndTC
		 * Gap2_StartTC - Gap2_EndTC
		 */
		String seperator = "-";
		int recordingStarted = 0;

		while ((line = input.readLine()) != null) {
			if (line.isEmpty()) {
				continue;
			}

			mediaSegmentTrackItemEL = XMLUtils.stringToElement("<" + MediaStoreServiceConst.MEDIA_TRACK_ITEM + "/>");
			index = line.indexOf(seperator);

			if (noOfLinesRead == 1) {
				recordingStarted = Utils.getIntValue(line.substring(index + 1), 0);
			}

			if (noOfLinesRead == 2) {
				recordStartTC = Utils.getLongValue(line.substring(0, index), -1);
				recordEndTC = Utils.getLongValue(line.substring(index + 1), -1);
				startTC = recordStartTC;
				endTC = recordEndTC;
				mediaOffset = 0;
			}

			if (noOfLinesRead > 2) {
				//Gap start tc is stopped time stamp of recording file
				endTC = Utils.getLongValue(line.substring(0, index), -1);

				//set the start tc and end tc in media segment info              
				//XMLUtils.addElement(mediaSegmentTrackItemEL, getMediaSegmentInfo(mediaSegmentInfoDoc, startTC, endTC, mediaOffset));
				mediaSegmentTrackItemEL.add(getMediaSegmentInfo(mediaSegmentInfoDoc, startTC, endTC, mediaOffset));
				//XMLUtils.addElement(mediaSegmentTrackEL, (Element) mediaSegmentTrackItemEL.clone());
				mediaSegmentTrackEL.add((Element) mediaSegmentTrackItemEL.clone());

				//calculate next segment info start tc, end tc and media offset.
				if (endTC != 0) {
					mediaOffset = mediaOffset + (endTC - startTC);
				}

				startTC = Utils.getLongValue(line.substring(index + 1), -1);
				endTC = recordEndTC;
			}

			noOfLinesRead++;
		}

		if (recordingStarted == 1) {
			endTC = -1;
		}

		mediaSegmentTrackItemEL = XMLUtils.stringToElement("<" + MediaStoreServiceConst.MEDIA_TRACK_ITEM + "/>");
		//add final index.
		//MRXMLUtils.addElement(mediaSegmentTrackItemEL, getMediaSegmentInfo(mediaSegmentInfoDoc, startTC, endTC, mediaOffset));
		mediaSegmentTrackItemEL.add(getMediaSegmentInfo(mediaSegmentInfoDoc, startTC, endTC, mediaOffset));
		// MRXMLUtils.addElement(mediaSegmentTrackEL, (Element) mediaSegmentTrackItemEL.clone());
		mediaSegmentTrackEL.add((Element) mediaSegmentTrackItemEL.clone());

		try {
			if (input != null) {
				input.close();
			}
		} catch (Exception exp) {
			//ignore exception
		}

		return mediaSegmentTrackEL;
	}

	//Set the  start tc , end tc and media offset in media segment info and returns that element.
	private Element getMediaSegmentInfo(Document mediaSegmentInfoDoc, long startTC, long endTC, long mediaOffset)
			throws Exception {
		XMLUtils.setValueXpath(mediaSegmentInfoDoc, "//StartTC", String.valueOf(startTC));
		XMLUtils.setValueXpath(mediaSegmentInfoDoc, "//EndTC", String.valueOf(endTC));
		XMLUtils.setValueXpath(mediaSegmentInfoDoc, "//MediaOffset", String.valueOf(mediaOffset));

		return ((Element) mediaSegmentInfoDoc.getRootElement().clone());
	}

	@Override
	public void deleteFile(MediaStoreDirectoryInfo dirInfo, String fileUUID, String trashDirName, Logger logger)
			throws Exception	{
		String[] systemCmd = { "/etc/scripts/trash_media.sh", fileUUID, "full", dirInfo.getMediaStoreRootPath(), trashDirName};

		//	if (purgeFile) {
		//	    systemCmd = new String[3];
		//	    systemCmd[0] = "rm";
		//	    systemCmd[1] = "-rf";
		//	    systemCmd[2] = (dirInfo.getMediaStoreRootPath() + "blob" + "/" + fileUUID);
		//	}
		System.out.println("systemcmd:=" + systemCmd[0]);
		CommandLineUtils.executeSystemCommand(systemCmd);
		logger.debug("Delete Media Store File. MediaStoreFileNID:="+fileUUID);

	}

	@Override
	public void createSnapshot(String storePath, String currentTime, String filePath, String snapshotFilePath, String extension) 
			throws Exception {
		//write INDEX file
		String indexFilePath = storePath + MediaStoreServiceConst.SS_INDEX_FILE_NAME;
		FileUtils.writeFile(indexFilePath, currentTime +" "+MediaStoreServiceConst.SNAPSHOT_FILE_NAME+extension);

		//move file from temporary area to blob area
		CommandLineUtils.executeCLICommand(new String[]{"mv",filePath,snapshotFilePath});

		//create Thumbnail
		String [] thumbnailCmd = new String [] {MediaRoomServiceConst.FFCREATOR_TOOL, "-t", "-s", storePath, "-F", MediaRoomServiceConst.SNAPSHOT};
		CommandLineUtils.executeCLICommand(thumbnailCmd);

	}

	@Override
	public void createBookmarkInMediaStore(MediaStoreDirectoryInfo dirInfo, String mediaUUID,String bookmarkUUID, Document bookmarkFileDoc,Logger logger) throws Exception {
		// TODO Auto-generated method stub
		String rootDir = dirInfo.getMediaStoreRootPath();
		String mediaStoreFilePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaUUID+"/bookmarks";
		File fileObject = new File(mediaStoreFilePath);
		if(!fileObject.exists()) {
			throw  new ServiceException(MediaStoreServiceExceptionCodes.NO_BOOKMARK_FOLDER_FOUND, "No bookmark folder");			   
		}

		fileObject.mkdirs();	
		FileUtils.writeFileWithPath(mediaStoreFilePath,bookmarkUUID+".xml", bookmarkFileDoc.asXML());    


		logger.debug("MediaStoreFileImpl:Crate MediaStore ");
	}

	@Override
	public long getTCAtOffset(String mediaUUID, long offset, MediaStoreDirectoryInfo dirInfo, DeviceDBI deviceDB, Logger log) throws Exception {
		String rootDir = dirInfo.getMediaStoreRootPath();
		StringBuffer statusCmd = new StringBuffer();
		getTCAtOffsetCmd(mediaUUID,rootDir, offset, statusCmd);
		String statusResult = CommandLineUtils.executeStaticSSCommand(statusCmd.toString(),deviceDB, log);
		if ((statusResult == null) || statusResult.contains("201 OK")) {
			throw new ServiceException(MediaRoomServiceExceptionCodes.GETINSTANCESTATUSFAILED, "VNGDeviceStreamingServerImpl:executeStatus Get STREAMING SERVER INSTANCE STATUS FAILED cmd=" + statusCmd + " result=" + statusResult);
		}
		// System.out.println("StreamingServerImpl:getTCAtOffset cmd=" + statusCmd.toString() + " result=" + statusResult);
		return this.parseStatusResultForValue(statusResult, "t");
	}

	@Override
	public long getOffsetAtTC(long timecode, String mediaUUID, MediaStoreDirectoryInfo dirInfo, DeviceDBI deviceDB, Logger log) throws ServiceException, Exception {
		String rootDir = dirInfo.getMediaStoreRootPath();  
		StringBuffer statusCmd = new StringBuffer();
		getOffsetAtTCCmd(mediaUUID, timecode, rootDir, statusCmd);
		String result =  CommandLineUtils.executeStaticSSCommand(statusCmd.toString(),deviceDB, log);

		if ((result == null) || result.contains("201 OK")) {
			throw new ServiceException(MediaRoomServiceExceptionCodes.GETINSTANCESTATUSFAILED, "VNGDeviceStreamingServerImpl:executeStatus Get STREAMING SERVER INSTANCE STATUS FAILED cmd=" + statusCmd + " result=" + result);
		}
		// System.out.println("StreamingServerImpl:getOffsetAtTC cmd=" + statusCmd.toString() + " result=" + result);
		return this.parseStatusResultForValue(result, "t");
	}


	public long parseStatusResultForValue(String result, String name) throws Exception {
		String valueStr = null;
		long value = 0;

		StringTokenizer resultST = new StringTokenizer(result, " ");
		String token = null;

		while (resultST.hasMoreTokens()) {
			token = resultST.nextToken();

			if (token.contains(name + ":")) {
				int index = token.indexOf(":");
				valueStr = token.substring(index + 1, token.length());

				if ((valueStr != null) && (valueStr.trim().length() != 0)) {
					value = Long.parseLong(valueStr);
				}

				break;
			}
		}

		return value;    	
	}


	/**
	 * returns hashmap of bookmark elements in blob directory 
	 * @param dirInfo
	 * @param mediaStoreFileID
	 * @return hashmap: key: mookmarkID and value : bookmark element
	 * @throws Exception
	 */
	@Override
	public HashMap<String,Element> getListOfBookmarks(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID) throws Exception {
		File bookmark= new File(dirInfo.getMediaStoreRootPath()+ "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/"+ mediaStoreFileID + "/bookmarks");
		HashMap <String, Element> bookmarksMapping = new HashMap<String, Element>();
		if (bookmark.exists()){
			String[] mediaStoreBookmarkArray = bookmark.list();	
			for (String bookmarkUUID : mediaStoreBookmarkArray) {
				String bookmarkID = bookmarkUUID.replace(".xml", "");				
				Element bookmarkEl = XMLUtils.getElementXpath(getMediaStoreBookmark(dirInfo, mediaStoreFileID, bookmarkID), "//Bookmark");
				XMLUtils.addAttribute(bookmarkEl, "id", bookmarkID);				
				bookmarksMapping.put(bookmarkID, bookmarkEl);
			}
		} else System.out.println("No bookmarks found for : "+mediaStoreFileID);
		return bookmarksMapping;
	}


	@Override
	public void deleteBookmark(MediaStoreDirectoryInfo dirInfo, String bookmarkUUID, String mediaUUID, Logger logger) throws Exception {
		String rootDir = dirInfo.getMediaStoreRootPath();  
		String mediaStoreFilePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaUUID+"/bookmarks/"+bookmarkUUID+".xml";
		File fileObject = new File(mediaStoreFilePath);
		if(!fileObject.exists()) {
			throw  new ServiceException(MediaStoreServiceExceptionCodes.NO_DIR_UUID_FOUND, "No bookmark with UUID found");			   
		}
		String[] systemCmd = new String[3];
		systemCmd[0] = "rm";
		systemCmd[1] = "-rf";
		systemCmd[2] = (dirInfo.getMediaStoreRootPath() + "blob" + "/" + mediaUUID +"/bookmarks/"+bookmarkUUID+".xml");

		System.out.println("systemcmd:=" + systemCmd);
		CommandLineUtils.executeSystemCommand(systemCmd);
		logger.debug("Delete Bookmark Store File. MediaStoreFileNID:="+bookmarkUUID);

	}

	public void getTCAtOffsetCmd(String mediaUUID,String srcDirPath, long offset, StringBuffer statusCmd) throws Exception {
		statusCmd.append("action=query ");
		statusCmd.append("command=GetTimeCodeAtOffset ");
		statusCmd.append("srctype=DMSMediaPort ");
		statusCmd.append("srcname=" + mediaUUID + " ");
		statusCmd.append("srcdir=\"" + srcDirPath + "\" ");  
		statusCmd.append("offset=" + offset);
	}

	public void getOffsetAtTCCmd(String mediaUUID, long tc,String srcDirPath, StringBuffer statusCmd) throws Exception {
		statusCmd.append("action=query ");
		statusCmd.append("command=GetOffsetAtTimecode ");
		statusCmd.append("srctype=DMSMediaPort ");
		statusCmd.append("srcname=" + mediaUUID + " ");
		statusCmd.append("srcdir=\"" + srcDirPath + "\" ");  
		statusCmd.append("tc=" + tc);
	}
	@Override
	public void upgradeFile(MediaStoreDirectoryInfo dirInfo, String fileUUID, Document mediaStoreFileDoc, Logger logger) throws Exception {
		String serviceVersion = dirInfo.getDeviceServiceVersion(); 
		int rev = dirInfo.getDirRevision();
		Element mediaStoreFileEl =mediaStoreFileDoc.getRootElement();
		XMLUtils.addAttribute(mediaStoreFileEl, MediaStoreServiceConst.SERVICE_VERSION, serviceVersion);
		XMLUtils.addAttribute(mediaStoreFileEl, MediaStoreServiceConst.REVISION, String.valueOf(rev));
		FileUtils.writeFileWithPath(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+fileUUID+"/", MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME, mediaStoreFileEl.asXML());
		logger.debug("MediaStoreFileImpl: Upgrade Media store file " +"serviceVersion:= "+serviceVersion + "rev:= "+ rev);

	}


	@Override
	public void updateStateFlag(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID) throws Exception {
		File proprietaryFileObject = new File(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/proprietary/"); 		    
		File standardFileObject = new File(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/standard/"); 

		if(proprietaryFileObject.exists() && proprietaryFileObject.list().length>0)
			addStaleFile(dirInfo, mediaStoreFileID, "proprietary");

		if(standardFileObject.exists() && standardFileObject.list().length>0)
			addStaleFile(dirInfo, mediaStoreFileID, "standard");			    

	}

	@Override
	public boolean isStaleFileExist(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String format) throws Exception {
		if(format.isEmpty()){
			return false;
		}
		String filePath = dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/"+format+"/";			   
		return FileUtils.isFileExist(filePath, "EmptyStaleFile");
	}

	@Override
	public void addStaleFile(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String format) throws Exception {
		File ifAlreadyExist = new File(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/"+format+"/EmptyStaleFile"); 		    
		if(!ifAlreadyExist.exists()){
			FileUtils.writeFileWithPath(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/"+format+"/","EmptyStaleFile", "");
		}

	}

	@Override
	public boolean isDownloadFormatStale(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String formatType) throws Exception {			
		File fileObject = new File(dirInfo.getMediaStoreRootPath()+"/"+MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR+"/"+mediaStoreFileID+"/"+formatType+"/"); 	    
		if(fileObject.exists() && fileObject.list().length>0)
			return true;
		else
			return false;				
	}

	@Override
	public void addDownloadTagForFormat(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, Element downloadEl, String state, String format) throws Exception {
		boolean stale=isStaleFileExist(dirInfo, mediaStoreFileID, format.toLowerCase());
		Element mediaEl =  MediaStoreUtils.createMediaDownloadElement(format, state, stale);
		downloadEl.add((Element)mediaEl.detach());
	}

	@Override
	public void deleteStaleFile(MediaStoreDirectoryInfo dirInfo, String mediaStoreFileID, String format) throws Exception {
		String[] systemCmd = new String[3];
		systemCmd[0] = "rm";
		systemCmd[1] = "-rf";
		systemCmd[2] = (dirInfo.getMediaStoreRootPath() + "blob" + "/" + mediaStoreFileID +"/"+format+"/EmptyStaleFile");			
		System.out.println("systemcmd:=" + systemCmd);
		CommandLineUtils.executeSystemCommand(systemCmd);			
	}





}
