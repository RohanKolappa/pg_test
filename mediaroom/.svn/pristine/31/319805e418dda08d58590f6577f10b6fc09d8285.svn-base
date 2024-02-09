package com.ipvs.device.mediastore;

import java.io.File;
import java.util.HashMap;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerImpl;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaVersionManager;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public class DeviceMediaStoreServerImpl extends MediaStoreServerImpl {
    public DeviceMediaStoreServerImpl(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
    		String userJID) {
        super(xf, log, db, resourceFactory, userJID);
    }

    // Create db.xml in ..../blob/UUID/ directory
    public void createMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;

        if (FileUtils.isFileExist(parentDir, mediaStoreFileNID)) {
            throw new MRException(MediaStoreServiceExceptionCodes.DUPLICATE_MEDIA_STORE_NID, MRXMLUtils.generateErrorXML("storeID", mediaStoreFileNID));
        }

        parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;

        //Create if the directory blob directory.
        File mediaStoreFileFolder = new File(parentDir);

        if (!mediaStoreFileFolder.mkdirs()) {
            throw new MRException(MediaStoreServiceExceptionCodes.CREATION_OF_MEDIA_STORE_FILE_FAILED, MRXMLUtils.generateErrorXML("storeID", mediaStoreFileNID));
        }

        createMediaStoreFileInBlob(mediaStoreFileNID, rootDir, revision, fileDoc);
        log.addInfo("Create Media Store File. mediaStoreFileNID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);

        // createBookmarkDir(rootDir, mediaStoreFileNID, log);
    }

    /* // Create BookMarkList.xml in ..../blob/UUID/ directory
     public void createBookmarkDir(String rootDir, String mediaStoreFileNID, MRLog log)
         throws MRException, Exception {
         String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID + "/" + MediaStoreServiceConst.MEDIA_STORE_BOOKMARKS_FOLDER_NAME;
    
         //Create if the directory blob directory.
         File mediaStoreFileFolder = new File(parentDir);
    
         if (!mediaStoreFileFolder.mkdir()) {
             throw new MRException(MediaStoreServiceExceptionCodes.CREATION_OF_BOOKMARK_FOLDER_FAILED, MRXMLUtils.generateErrorXML("storeID", mediaStoreFileNID));
         }
    
         log.addInfo("Create Bookmark folder in MediaStoreNID:=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
     }*/

    //Update db.xml in ..../blob/UUID/ directory
    public void updateMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID + "/";
        if (FileUtils.isFileExist(mediaStoreFilePath, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME)) {
        	getMediaStoreDocument(mediaStoreFilePath, fileDoc); // Reads and  checks the revision
        }
        String state = MRXMLUtils.getValueXpath(fileDoc, "//State/StateFlag", "");
        updateMediaStoreFile(mediaStoreFilePath, fileDoc, state, revision);
    }

    //Update Media Store File Status.
    public void updateMediaStoreFileStatus(String mediaStoreFileNID, int revision, Document fileDoc, String state, String rootDir)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID + "/";
        getMediaStoreDocument(mediaStoreFilePath, fileDoc);
        updateMediaStoreFile(mediaStoreFilePath, fileDoc, state, revision);
    }

    //Get the media store document. Check the media store is available or not and check the revision numbers are equal are or not.
    private Document getMediaStoreDocument(String mediaStoreFilePath, Document fileDoc)
        throws MRException, Exception {
        isMediaStoreFileExists(mediaStoreFilePath);

        Document mediaStoredoc = getFileDocumentFromDisk(mediaStoreFilePath, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);

        isMediaStreamAndMediaStoreIsSynchOrNot(fileDoc, mediaStoredoc);

        return mediaStoredoc;
    }

    //check the revision of media store file and stream file are in synch or not, if not through the exception
    private void isMediaStreamAndMediaStoreIsSynchOrNot(Document mediaStreamFileDoc, Document mediaStoreDocument)
        throws MRException, Exception {
        Element mediaStoreInfoEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT);

        int mediaStreamFileRevision = Utils.getIntValue(MRXMLUtils.getAttributeValue(mediaStoreInfoEL, MediaStoreServiceConst.MEDIA_STORE_FILE_REVISION, ""), 0);
        int mediaStoreFileRevision = Utils.getIntValue(MRXMLUtils.getAttributeValue(mediaStoreDocument.getRootElement(), MediaStoreServiceConst.REVISION, ""), 0);

        if (mediaStoreFileRevision != mediaStreamFileRevision) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_AND_STREAM_FILE_ARE_OUT_OF_SYNCH, MRXMLUtils.generateErrorXMLTags("mediaStoreFileRevision", String.valueOf(mediaStoreFileRevision), "mediaStreamFileRevision", String.valueOf(mediaStreamFileRevision)));
        }
    }

    //update media store file on disk with latest revision and state
    private void updateMediaStoreFile(String mediaStoreFilePath, Document fileDoc, String state, int revision)
        throws MRException, Exception {
        String strMediaFileDoc = generateMediaStoreFileDoc(fileDoc, revision, state, log);
        writeFileContent(mediaStoreFilePath, strMediaFileDoc, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);
        log.addInfo("Update Media Store File. State :=" + state + " Revision :=" + revision, MRLog.INFO, MRLog.NOTIFY);
    }

    private void isMediaStoreFileExists(String mediaStoreFilePath) throws MRException, Exception {
        if (!FileUtils.isFileExist(mediaStoreFilePath, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME)) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_FILE_NOT_FOUND, "Media Store File Not Found");
        }
    }

    //Delete uuid dir from /blob/ folder.
    public void deleteMediaStoreFile(String mediaStoreFileNID, String rootDir, boolean purgeFiles)
        throws MRException, Exception {
    	
        String[] systemCmd = { "/etc/scripts/trash_media.sh", mediaStoreFileNID, "full", rootDir };
        
        if(purgeFiles) {
        	systemCmd = new String[3];
        	systemCmd[0]= "rm";
        	systemCmd[1]= "-rf";
        	systemCmd[2]= rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID ;
        }
        System.out.println("systemcmd:="+systemCmd[0]);
        MediaRoomServiceUtils.executeSystemCommand(systemCmd);
        log.addInfo("Delete Media Store File. MediaStoreFileNID:=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
    }

    /*//Add book mark in /blob/uuid/bookmarks/ folder.
    public void addBookMark(String mediaStoreFileNID, Document bookmarkDoc, String rootDir)
        throws MRException, Exception {
        String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
    
        if (!FileUtils.isFileExist(parentDir, MediaStoreServiceConst.MEDIA_STORE_BOOKMARKS_FOLDER_NAME)) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_BOOKMARK_FOLDER_FOUND, "No BookMarks Folder Found in Dir :=" + parentDir);
        }
    
        String bookmarkFileName = getBookMarkFileName(bookmarkDoc);
        parentDir = parentDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BOOKMARKS_FOLDER_NAME;
    
        if (FileUtils.isFileExist(parentDir, bookmarkFileName)) {
            throw new MRException(MediaStoreServiceExceptionCodes.DUPLICATE_BOOKMARK_FILE_NAME, "Duplicate Bookmark FileName.  FileName:=" + bookmarkFileName);
        }
    
        writeFileContent(parentDir, MRXMLUtils.documentToString(bookmarkDoc), bookmarkFileName);
        log.addInfo("Add BookMark-> BookMarkFileName:=" + bookmarkFileName + " MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }
    
    //Update Book mark in /blob/uuid/bookmarks/bookmarkTc.bookmarkID file
    public void updateBookMark(String mediaStoreFileNID, Document bookMarkDoc, String rootDir)
        throws MRException, Exception {
        String bookmarkFileName = getBookMarkFileName(bookMarkDoc);
        String parentDir = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID + "/" + MediaStoreServiceConst.MEDIA_STORE_BOOKMARKS_FOLDER_NAME;
    
        Document storedBookMarkDoc = getBookmarkDocument(parentDir, mediaStoreFileNID, bookmarkFileName);
    
        int storedRevision = Utils.getIntValue(MRXMLUtils.getAttributeValue(storedBookMarkDoc.getRootElement(), MediaStoreServiceConst.REVISION, ""), 0);
        int requestedRevision = Utils.getIntValue(MRXMLUtils.getAttributeValue(bookMarkDoc.getRootElement(), MediaStoreServiceConst.REVISION, ""), MediaStoreServiceConst.DEFAULT_REVISION);
    
        //Check the requested book mark and stored book mark are in same revision. If not then  throw the exception
        if (storedRevision != requestedRevision) {
            throw new MRException(MediaStoreServiceExceptionCodes.INVALID_BOOKMARK_REVISION, "Invalid Bookmark revision. BookmarkFileName=" + bookmarkFileName);
        }
    
        storedRevision++;
        //update  new revision in book mark xml's
        MRXMLUtils.setAttributeValue(bookMarkDoc.getRootElement(), MediaStoreServiceConst.REVISION, String.valueOf(storedRevision));
    
        writeFileContent(parentDir, MRXMLUtils.documentToString(bookMarkDoc), bookmarkFileName);
        log.addInfo("Update BookMark-> BookmarkFileName:= " + bookmarkFileName + " MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }
    
    //check book mark file exists or not. if exists return the document.
    private Document getBookmarkDocument(String parentDir, String mediaStoreFileNID, String bookmarkFileName)
        throws MRException, Exception {
        checkBookMarkFileExists(parentDir, bookmarkFileName);
    
        Document storedBookMarkDoc = getFileDocumentFromDisk(parentDir, bookmarkFileName);
    
        return storedBookMarkDoc;
    }
    
    //Check book mark file exists in the blob/uuid/bookmarks/ folder or not
    private void checkBookMarkFileExists(String parentDir, String bookmarkFileName)
        throws MRException, Exception {
        if (!FileUtils.isFileExist(parentDir, bookmarkFileName)) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_BOOKMARK_FOUND, "No Bookmark found to Update.  FileName:=" + bookmarkFileName);
        }
    }
    
    //Delete book mark file from /blob/uuid/bookmarks/ folder.
    public void deleteBookMark(String mediaStoreFileNID, Document bookMarkDoc, String rootDir)
        throws MRException, Exception {
        String bookmarkFileName = getBookMarkFileName(bookMarkDoc);
        String parentDir = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID + "/" + MediaStoreServiceConst.MEDIA_STORE_BOOKMARKS_FOLDER_NAME;
    
        checkBookMarkFileExists(parentDir, bookmarkFileName);
        FileUtils.deleteFile(parentDir, bookmarkFileName);
        log.addInfo("Delete BookMark from MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }*/

    //Get Media Store Track.
    /*public Element getMediaStoreTrack(String mediaStoreFileNID, Document requestDocument, String rootDir, String mediaType)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        Element mediaStoreTrackEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_STORE_FILE_TRACK);
        Element gapInfoElement = getMediaStoreGapInfo(mediaStoreFilePath);
        Element bookmarkTrackEL = getBookmarksList(mediaStoreFilePath);
        MRXMLUtils.addChildElement(mediaStoreTrackEL, bookmarkTrackEL);
        MRXMLUtils.addChildElement(mediaStoreTrackEL, gapInfoElement);
    
        log.addInfo("Get MediaStoreTack for MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + mediaStoreFilePath, MRLog.INFO, MRLog.NOTIFY);
    
        return mediaStoreTrackEL;
    }*/

    /* //Returns the book mark list for given media store file.
     private Element getBookmarksList(String mediaStoreFilePath) throws MRException, Exception {
         String bookmarkFileRootPath = mediaStoreFilePath + "/" + MediaStoreServiceConst.MEDIA_STORE_BOOKMARKS_FOLDER_NAME;
         File file = new File(bookmarkFileRootPath);
    
         //Get list of files in /blob/uuid/bookmarks
         String[] bookmarkFileArray = file.list();
         List<String> bookmarkFileList = Arrays.asList(bookmarkFileArray);
    
         //Sort the list in ascending order.
         //TBD: At Present file.list method is retrieving in sorted order only. we have to test this.
         Collections.sort(bookmarkFileList);
    
         Element mediaBookmarkTrackEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_TRACK);
         Element mediaBookmarkTrackItemEL = null;
    
         Iterator<String> bookMarkListIterator = bookmarkFileList.iterator();
         Document bookmarkDoc = null;
    
         //Read each file and add to MediaBookMarkTrack Element.
         while (bookMarkListIterator.hasNext()) {
             mediaBookmarkTrackItemEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_TRACK_ITEM);
             bookmarkDoc = getFileDocumentFromDisk(bookmarkFileRootPath, bookMarkListIterator.next());
             MRXMLUtils.addElement(mediaBookmarkTrackItemEL, (Element) bookmarkDoc.getRootElement().detach());
             MRXMLUtils.addElement(mediaBookmarkTrackEL, (Element) mediaBookmarkTrackItemEL.clone());
         }
    
         return mediaBookmarkTrackEL;
     }*/

    //Get the last media segment info from index file and calculate the last segment play length.
    public long getMediaStoreFileLastSegmentPlayLength(String mediaStoreFileNID, long endTC, String rootDir)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        Element mediaSegmentTrackEL = getMediaStoreGapInfo(mediaStoreFilePath);

        if (mediaSegmentTrackEL == null) {
            return 0;
        }

        Document mediaSegmentTrackDoc = MRXMLUtils.elementToDocument(mediaSegmentTrackEL);
        Element mediaSegementInfoEL = MRXMLUtils.getElementXpath(mediaSegmentTrackDoc, "//MediaSegmentInfo[EndTC='" + endTC + "']");

        if (mediaSegementInfoEL == null) {
            return 0;
        }

        long startTC = Utils.getLongValue(MRXMLUtils.getValue(mediaSegementInfoEL, "StartTC", ""), -1);

        if (startTC == -1) {
            return 0;
        }

        long playLength = endTC - startTC;

        return playLength;
    }

    // get the latest revision number from mediastoreinfo.xml and
    // increment by one and update in the mediastoreinfo.xml.
    public Integer getNewRevisionNumber(String rootDir) throws MRException, Exception {
        int revision = getMediaStoreRevisionNumber(rootDir, true);

        int newRevision = revision + 1;

        updateMediaStoreInfo(rootDir, newRevision);

        return newRevision;
    }

    // Checked if the MediaStoreInfo.xml is there or not
    // If not create MediaStoreInfo.xml with default revision +1.
    // If already exists read that file and return the revision from MediaStoreInfo.xml
    public Integer getMediaStoreRevisionNumber(String rootDir, boolean isWritable)
        throws MRException, Exception {
        if (!isWritable) {
            return MediaStoreServiceConst.DEFAULT_REVISION;
        }

        if (!isStoreExists(rootDir, MediaStoreServiceConst.MEDIA_STORE_INFO_FILE_NAME)) {
            createMediaStoreInfo(rootDir, MediaStoreServiceConst.DEFAULT_REVISION, log);

            return MediaStoreServiceConst.DEFAULT_REVISION;
        }

        Document mediaStoreDocument = getFileDocumentFromDisk(rootDir, MediaStoreServiceConst.MEDIA_STORE_INFO_FILE_NAME);

        if (mediaStoreDocument == null) {
            return 0;
        }

        return Utils.getIntValue(MRXMLUtils.getValueXpath(mediaStoreDocument.getRootElement(), "//" + MediaStoreServiceConst.REVISION_ELEMENT_NAME, ""), 0);
    }

    // Checked the media store exists on the disk or not
    public static boolean isStoreExists(String dirPath, String fileName) throws MRException, Exception {
        return FileUtils.isFileExist(dirPath, MediaStoreServiceConst.MEDIA_STORE_INFO_FILE_NAME);
    }

    // Creates the MediaStore.xml in the /default/ dir    
    public void createMediaStoreInfo(String rootDir, int revision, MRLog log)
        throws MRException, Exception {
        Document mediaStoreDocument = MediaStoreServiceUtils.loadMediaStoreInfoDocument(log);
        updateMediaStoreInfo(mediaStoreDocument, rootDir, revision);
        log.addInfo("Create Media Store XML file. FileName :=" + MediaStoreServiceConst.MEDIA_STORE_INFO_FILE_NAME, MRLog.INFO, MRLog.NOTIFY);
    }

    //Updates the MediaStoreInfo.xml with Next revision
    private void updateMediaStoreInfo(Document mediaStoreDocument, String rootDir, int revision)
        throws MRException, Exception {
        MRXMLUtils.setValueXpath(mediaStoreDocument.getRootElement(), "//" + MediaStoreServiceConst.REVISION_ELEMENT_NAME, String.valueOf(revision));

        String mediaStoreDocumentStr = MRXMLUtils.documentToString(mediaStoreDocument);
        writeFileContent(rootDir, mediaStoreDocumentStr, MediaStoreServiceConst.MEDIA_STORE_INFO_FILE_NAME);
    }

    //Updates the MediaStoreInfo.xml with Next revision 
    public void updateMediaStoreInfo(String rootDir, int revision) throws MRException, Exception {
        Document mediaStoreDocument = getFileDocumentFromDisk(rootDir, MediaStoreServiceConst.MEDIA_STORE_INFO_FILE_NAME);

        if (mediaStoreDocument == null) {
            return;
        }

        updateMediaStoreInfo(mediaStoreDocument, rootDir, revision);
    }

    /* public HashMap<String, Document> getMediaStoreFileListWhichOrNotInSynch(int mediaStreamFileRevision, String rootDir, HashMap<String, Document> mediaStoreAndStreamMapping)
         throws MRException, Exception {
         String parentDirPath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
         File file = new File(parentDirPath);
    
         //Get list of files in /blob/uuid
         String[] mediaStoreFileArray = file.list();
    
         if ((mediaStoreFileArray == null) || (mediaStoreFileArray.length == 0)) {
             return null;
         }
    
         String parentDir = null;
         Document mediaStoreFileDoc = null;
         int mediaStoreFileRevision = 0;
         String revisionValue = null;
         HashMap<String, Document> mediaStoreMapping = new HashMap<String, Document>();
         int version = 0;
    
         for (String storeFileUUID : mediaStoreFileArray) {
             parentDir = parentDirPath + "/" + storeFileUUID;
    
             version = getVersionOfMediaStoreFile(rootDir, storeFileUUID);
    
             if ((version == MediaStoreServiceConst.MEDIA_STORE_VERSION_2_DOT_4) || (version == MediaStoreServiceConst.MEDIA_STORE_VERSION_3_DOR_2)) {
                 continue;
             } else if (version == MediaStoreServiceConst.MEDIA_STORE_VERSION_CURRENT) {
                 mediaStoreFileDoc = getFileDocumentFromDisk(parentDir, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);
                 revisionValue = mediaStoreFileDoc.getRootElement().getAttributeValue(MediaStoreServiceConst.REVISION);
                 mediaStoreFileRevision = Utils.getIntValue(revisionValue, 0);
    
                 if (mediaStoreFileRevision > mediaStreamFileRevision) {
                     mediaStoreMapping.put(storeFileUUID, mediaStoreFileDoc);
                 }
             } else {
                 throw new MRException(MediaStoreServiceExceptionCodes.INVALID_MEDIA_STORE_VERSION, "Invalid Media Store File Version");
             }
         }
    
         return mediaStoreMapping;
     }*/
    public void updateSynMediaStoreFile(String mediaStoreFileNID, int revision, Document storeDocument, String rootDir)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        MRXMLUtils.setAttributeValue(storeDocument.getRootElement(), MediaStoreServiceConst.REVISION, String.valueOf(revision));
        writeFileContent(mediaStoreFilePath, MRXMLUtils.documentToString(storeDocument), MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);
    }

    public Document generateMediaStreamFileDocFromStoreFile(Document mediaStoreFileDoc, Document mediaStreamFileDoc, String mediaStoreFileNID, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, int storeRevision, boolean isNewFile, HashMap<String, Document> mediaStoreAndStreamMapping)
        throws MRException, Exception {
        Document newMediaStreamFileDoc = (Document) mediaStreamFileDoc.clone();

        Element infoEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.INFO_ELEMENT_NAME);
        String state =MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_STATE_ELEMENT, ""); 
        
        if (isNewFile) {
            //set UUID
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.UUID, mediaStoreFileNID);

            //set the groups 
            MediaStoreServiceUtils.updateDirGroupsInFile(newMediaStreamFileDoc, dirGroupsEL);

            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PVRMEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAGROUP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PLAYLIST);

            //set title,description and stream type
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, ""));
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, ""));
            
            String streamType = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, "");
            if(streamType.isEmpty()){
            	streamType = MediaRoomServiceConst.V2D;
            }
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, streamType);
            
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, state);
         	MediaStoreServiceUtils.updateFilePermissions(newMediaStreamFileDoc, MediaStoreServiceConst.XFER, MediaStoreServiceConst.TRUE);
         	MediaStoreServiceUtils.updateFilePermissions(newMediaStreamFileDoc, MediaStoreServiceConst.STREAM, MediaStoreServiceConst.TRUE);
           
            // Media Group ID
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, ""));
            
            //BypassAutoStorageRules
            //MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, "false"));

            long size = getMediaSize(mediaStoreFileNID, storeInfo.getMediaStoreRootPath());
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//Size", String.valueOf(size));

            //update date created and date modified.
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED, ""));
        }

        //set the file permission based on service version.
        String serviceVersion = getVersionOfMediaStoreFile(mediaStoreFileDoc);

        MediaStoreServiceUtils.updateFilePermissions(newMediaStreamFileDoc, MediaStoreServiceConst.READ, MediaStoreServiceConst.TRUE);
        

        if (MediaVersionManager.equals(serviceVersion,log)) {
            //if the file is current version 
        	MediaStoreServiceUtils.updateFilePermissions(newMediaStreamFileDoc, MediaStoreServiceConst.WRITE, MediaStoreServiceConst.TRUE);
            //MediaStoreServiceUtils.updateRevision(newMediaStreamFileDoc, storeRevision);
        } else {
            //set the file permissions read only
        	MediaStoreServiceUtils.updateFilePermissions(newMediaStreamFileDoc, MediaStoreServiceConst.WRITE, MediaStoreServiceConst.FALSE);          
        }
        
        int mediaStoreFileRevision = Utils.getIntValue(MRXMLUtils.getAttributeValue(mediaStoreFileDoc.getRootElement(), MediaStoreServiceConst.REVISION, ""), storeRevision);
        MediaStoreServiceUtils.updateRevision(newMediaStreamFileDoc, mediaStoreFileRevision);
        

        //override common information from media store file

        //set type
        String mediaType = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

        String dirNID = storeInfo.getMediaStoreDirNID();
        if (MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType)) {
            dirNID =  storeInfo.getPvrDirNID();
        } else if(state.equals(MediaStoreServiceConst.DELETED)){
        	dirNID =  storeInfo.getRecycleBinNID();
        } 
        
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME, dirNID);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, mediaType);

        //update media store file info
        Element mediaStoreFileEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT);
        MRXMLUtils.removeChild(mediaStoreFileEL,MediaStoreServiceConst.META_DATA_ELEMENT_NAME);

        Element metaDataEL = (Element) MRXMLUtils.getElementXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME).clone();
        MRXMLUtils.addElement(mediaStoreFileEL, metaDataEL);

        //add the media type element
        MRXMLUtils.removeChild(infoEL,mediaType);

        Element mediaTypeEL = (Element) MRXMLUtils.getElementXpath(mediaStoreFileDoc, "//" + mediaType).clone();
        //remove system attributes 
        mediaTypeEL = MRXMLUtils.removeSystemAttributesFromXML(MRXMLUtils.elementToString(mediaTypeEL));
        MRXMLUtils.addElement(infoEL, mediaTypeEL);

        //set Media Group ID
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, ""));
        
        //set BypassAutoStorageRules
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, "false"));

        //update date modified 
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, ""));

        return newMediaStreamFileDoc;
    }    
   
    /*public void makeMediaStreamFileAndMediaStoreFileSynch(HashMap<String, Document> mediaStoreAndStreamMapping, HashMap<String, Document> mediaStoreFileList, int storeRevision, MediaStoreInfo storeInfo)
        throws MRException, Exception {
        Set<String> mediaStoreKeySet = mediaStoreFileList.keySet();
        Document mediaStreamFileDoc = null;
        Document mediaStoreFileDoc = null;
        Document newMediaStreamFileDoc = null;
        boolean isNewFile = false;
    
        for (String mediaStoreFileNID : mediaStoreKeySet) {
            isNewFile = false;
            mediaStreamFileDoc = mediaStoreAndStreamMapping.get(mediaStoreFileNID);
            mediaStoreFileDoc = mediaStoreFileList.get(mediaStoreFileNID);
    
            if (mediaStreamFileDoc == null) {
                mediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
                isNewFile = true;
            }
    
            newMediaStreamFileDoc = generateMediaStreamFileDocFromStoreFile(mediaStoreFileDoc, mediaStreamFileDoc, mediaStoreFileNID, storeInfo, storeRevision, isNewFile, mediaStoreAndStreamMapping);
            updateSynMediaStoreFile(mediaStoreFileNID, storeRevision, mediaStoreFileDoc, storeInfo.getMediaStoreRootPath());
    
            if (isNewFile) {
                addMediaStreamFile(newMediaStreamFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo);
            } else {
                updateMediaStreamFile(newMediaStreamFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo);
            }
        }
    }*/
    public void repairMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;

        if (!FileUtils.isFileExist(parentDir, mediaStoreFileNID)) {
            createMediaStoreFile(mediaStoreFileNID, revision, fileDoc, rootDir);
        } else {
            createMediaStoreFileInBlob(mediaStoreFileNID, rootDir, revision, fileDoc);
            log.addInfo("Repair Media Store File in. mediaStoreFileNID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
        }
    }

    private void createMediaStoreFileInBlob(String mediaStoreFileNID, String rootDir, int revision, Document fileDoc)
        throws MRException, Exception {
        String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        String state = MRXMLUtils.getValueXpath(fileDoc, "//State/StateFlag", MediaStoreServiceConst.READY);
        
        // Create Media Store db.xml if not present in that dir.        
        String strMediaFileDoc = generateMediaStoreFileDoc(fileDoc, revision, state, log);
        writeFileContent(parentDir, strMediaFileDoc, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);
    }

    public void upgradeMediaStore(String mediaStoreFileNID, Document mediaStreamFileDoc, String rootDir, int revision)
        throws MRException, Exception {
        createMediaStoreFileInBlob(mediaStoreFileNID, rootDir, revision, mediaStreamFileDoc);
        log.addInfo("Create Media Store db.xml. mediaStoreFileNID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
    }

    public String getMediaStoreDirUUID(String mediaStoreDirRootPath) throws MRException, Exception {
        Document uuidDoc = getFileDocumentFromDisk(mediaStoreDirRootPath, MediaStoreServiceConst.DIR_UUID_FILE_NAME);

        if (uuidDoc == null) {
            return null;
        }

        return MRXMLUtils.getValueXpath(uuidDoc, "//" + MediaStoreServiceConst.UUID, null);
    }

    private void setupMediaStoreDirUUID(String mediaStoreDirRootPath, String uuid)
        throws MRException, Exception {
        String uuidFileContent = MRXMLUtils.generateXML(MediaStoreServiceConst.UUID, uuid);
        writeFileContent(mediaStoreDirRootPath, uuidFileContent, MediaStoreServiceConst.DIR_UUID_FILE_NAME);
    }

    private void setupMediaStoreDirBlob(String mediaStoreDirRootPath) throws MRException, Exception {
        FileUtils.createFileDir(mediaStoreDirRootPath, MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR);
    }

    public void setupMediaStoreDir(String mediaStoreDirRootPath, String uuid)
        throws MRException, Exception {
        setupMediaStoreDirUUID(mediaStoreDirRootPath, uuid);
        getMediaStoreRevisionNumber(mediaStoreDirRootPath, true);
        setupMediaStoreDirBlob(mediaStoreDirRootPath);
        log.addInfo("Setup Media Store Dir", MRLog.INFO, MRLog.NOTIFY);
    }

    public String upgradeMediaStoreDir(String rootDir) throws MRException, Exception {
        String dirUUID = getMediaStoreDirUUID(rootDir);

        // create dir UUID if  not there. 
        if (dirUUID == null) {
            dirUUID = Utils.getUUID();
            setupMediaStoreDir(rootDir, dirUUID);
        }

        int revision = MediaStoreServiceConst.DEFAULT_REVISION;

        if (isStoreExists(rootDir, MediaStoreServiceConst.MEDIA_STORE_INFO_FILE_NAME)) {
            revision = getMediaStoreRevisionNumber(rootDir, true);
        }

        createMediaStoreInfo(rootDir, revision, log);
        log.addInfo("Upgrade Media Store Dir ", MRLog.INFO, MRLog.NOTIFY);

        return dirUUID;
    }

	@Override
	public boolean isMediaStoreExist(String rootDir, String mediaStoreFileNID)
			throws MRException, Exception {
		
	   String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
       return FileUtils.isFileExist(parentDir, mediaStoreFileNID);
	}
}
