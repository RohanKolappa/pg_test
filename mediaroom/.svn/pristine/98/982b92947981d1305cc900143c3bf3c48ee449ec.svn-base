package com.ipvs.mediastoreservice.impl;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.AsynchDBRequestResponseListenerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.db.MRXMLAsynchDBUtils;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.systemservice.impl.ServiceVersionManager;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public abstract class MediaStoreServerImpl implements MediaStoreServerI {
    protected XMPPF xf;
    protected MRDB db;
    protected MRLog log;
    protected MRResourceFactoryI resourceFactory;
    protected String userJID;

    public MediaStoreServerImpl(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID) {
        this.xf = xf;
        this.log = log;
        this.db = db;
        this.resourceFactory = resourceFactory;
        this.userJID = userJID;
    }

    protected void operationNotAllowed() throws MRException, Exception {
        throw new MRException(MediaStoreServiceExceptionCodes.OPERATION_NOT_ALLOWED, "Operation is Not allowed In Version +=" + MediaVersionManager.MEDIA_STORE_VERSION_2_DOT_4);
    }

    public abstract long getMediaStoreFileLastSegmentPlayLength(String mediaStoreFileNID, long endTC, String rootDir)
        throws MRException, Exception;

    
    //returns the gap info of given media store file.
    protected Element getMediaStoreGapInfo(String mediaStoreFilePath) throws MRException, Exception {
        Element mediaSegmentTrackEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_TRACK);

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
        Document mediaSegmentInfoDoc = MediaStoreServiceUtils.loadMediaSegmentInfoDocument(log);

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

            mediaSegmentTrackItemEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_TRACK_ITEM);
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
                MRXMLUtils.addElement(mediaSegmentTrackItemEL, getMediaSegmentInfo(mediaSegmentInfoDoc, startTC, endTC, mediaOffset));
                MRXMLUtils.addElement(mediaSegmentTrackEL, (Element) mediaSegmentTrackItemEL.clone());

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

        mediaSegmentTrackItemEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_TRACK_ITEM);
        //add final index.
        MRXMLUtils.addElement(mediaSegmentTrackItemEL, getMediaSegmentInfo(mediaSegmentInfoDoc, startTC, endTC, mediaOffset));
        MRXMLUtils.addElement(mediaSegmentTrackEL, (Element) mediaSegmentTrackItemEL.clone());

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
        throws MRException, Exception {
        MRXMLUtils.setValueXpath(mediaSegmentInfoDoc, "//StartTC", String.valueOf(startTC));
        MRXMLUtils.setValueXpath(mediaSegmentInfoDoc, "//EndTC", String.valueOf(endTC));
        MRXMLUtils.setValueXpath(mediaSegmentInfoDoc, "//MediaOffset", String.valueOf(mediaOffset));

        return ((Element) mediaSegmentInfoDoc.getRootElement().clone());
    }

    //Update media file play length and size in data base. And updates the play length in  blob db.xml
    public void updateMediaPlayLength(String mediaStoreFileNID, String mediaStreamDirNID, int revision, 
    		Document actionDocument, String rootDir,AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject) throws MRException, Exception {    	
    	Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDocFromStoreFile(mediaStoreFileNID, mediaStreamDirNID, log, db, MRRequest.createRequest(userJID));
    	String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), MediaStoreServiceConst.NID,"");
              
    	//if store file does not exist return
    	if(!this.isMediaStoreExist(rootDir, mediaStoreFileNID)) {
    		log.addInfo("Media Store File Does not exist. mediaStoreFileNID:="+mediaStoreFileNID,MRLog.INFO,MRLog.NOTIFY);
    		MediaStoreServiceUtils.updateState(mediaStreamFileDoc, MediaStoreServiceConst.ERROR);
    		MediaStoreServiceUtils.updateElement(MRRequest.createRequest(this.userJID), log, db, mediaStreamFileNID, mediaStreamFileDoc.getRootElement());
    		return;
    	}
    	
        long sizeInFileDoc = Utils.getLongValue(MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE,""),0);
        long size = Utils.getLongValue(MRXMLUtils.getValueXpath(actionDocument, "//BytesWritten",""),0) + sizeInFileDoc;
        long playLength = Utils.getLongValue(MRXMLUtils.getValueXpath(actionDocument, "//PlayLength",""),0);
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//Size", String.valueOf(size));
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//PlayLength", String.valueOf(playLength));
        MediaStoreServiceUtils.updateState(mediaStreamFileDoc, MediaStoreServiceConst.READY);
        updateMediaFile(mediaStoreFileNID, mediaStreamFileNID, revision, mediaStreamFileDoc, rootDir,callBackObject,callBackDataObject);
        log.addInfo("MediaStoreServerImple:updateMediaPlayLength PlayLength:=" + playLength + " PreviousSize:="+sizeInFileDoc +" Size:=" + size + " MediaStoreFileNID:=" + mediaStoreFileNID + " Revision :=" + revision, MRLog.INFO, MRLog.NOTIFY);        
    }

    //update file status in media stream file and store file
    public void updateMediaStatus(String mediaStoreFileNID, String mediaStreamFileNID, int revision, Document actionDocument, String rootDir)
        throws MRException, Exception {
        Document mediaStreamFileDoc = MediaStoreServiceUtils.checkMediaStreamFileState(mediaStreamFileNID, log, db, MRRequest.createRequest(userJID));
        String recordStatus = MRXMLUtils.getValueXpath(actionDocument, "//Status", "");
        MediaStoreServiceUtils.updateState(mediaStreamFileDoc, recordStatus);

        MediaStoreServiceUtils.updateDateModified(mediaStreamFileDoc, xf.xmpp(userJID));
        updateMediaStoreFileStatus(mediaStoreFileNID, revision, (Document) mediaStreamFileDoc.clone(), recordStatus, rootDir);
        MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, revision);
        MediaStoreServiceUtils.updateElement(MRRequest.createRequest(this.userJID), log, db, mediaStreamFileNID, mediaStreamFileDoc.getRootElement());
        log.addInfo("Update Media Status. MediaStoreFileNID:=" + mediaStoreFileNID + ", Status:=" + recordStatus + " and Revision:=" + revision, MRLog.INFO, MRLog.NOTIFY);
    }

    //Generate media store file Document from media stream file document
    public String generateMediaStoreFileDoc(Document fileDoc, int revision, String state, MRLog log)
        throws MRException, Exception {
        Document mediaStoreFileDoc = MediaStoreServiceUtils.loadMediaStoreObjectXML(log);
        Element stateEL = (Element) MRXMLUtils.getElementXpath(mediaStoreFileDoc, "//State").detach();
        MRXMLUtils.setValue(stateEL, state);

        Element rootEL = mediaStoreFileDoc.getRootElement();
        MRXMLUtils.removeContent(rootEL);
        MRXMLUtils.setAttributeValue(rootEL, MediaStoreServiceConst.REVISION, String.valueOf(revision));
        MRXMLUtils.setAttributeValue(rootEL, MediaStoreServiceConst.SERVICE_VERSION, String.valueOf(ServiceVersionManager.getServiceVersion(log)));

        Element mediaStoreFileInfoEL = MRXMLUtils.getElementXpath(fileDoc, "//MediaStoreFileInfo");

        if (mediaStoreFileInfoEL != null) {
            MRXMLUtils.copyContent(rootEL, mediaStoreFileInfoEL);
        }

        //get the clip document from media stream file.
        String mediaType = MRXMLUtils.getValueXpath(fileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
        Element clipEL = MRXMLUtils.getElementXpath(fileDoc, "//" + mediaType);

        rootEL.add((Element) MRXMLUtils.getElementXpath(fileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME).clone());
        rootEL.add((Element) MRXMLUtils.getElementXpath(fileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME).clone());
        rootEL.add((Element) MRXMLUtils.getElementXpath(fileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME).clone());
        rootEL.add((Element) MRXMLUtils.getElementXpath(fileDoc, "//" + MediaStoreServiceConst.DATE_CREATED).clone());
        rootEL.add((Element) MRXMLUtils.getElementXpath(fileDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED).clone());
        rootEL.add(stateEL);
        rootEL.add((Element) clipEL.clone());

        /*
        System.out.println("MediaStoreServerImpl:generateMediaStoreFileDoc stream=" +
        		MRXMLUtils.documentToString(fileDoc) + "store=" + 
        		MRXMLUtils.documentToString(mediaStoreFileDoc));
		*/
        
        return MRXMLUtils.elementToString(rootEL);
    }

    /*//Generate the book mark File Name from bookmark Document
    protected String getBookMarkFileName(Document bookmarkDoc) throws MRException, Exception {
        if (bookmarkDoc == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_BOOKMARK_FOUND, "No Bookmark Found");
        }
    
        String bookmarkID = MRXMLUtils.getValueXpath(bookmarkDoc, "//ID", "");
        String bookmarkTC = MRXMLUtils.getValueXpath(bookmarkDoc, "//AtTC", "");
    
        if (bookmarkID.isEmpty() || bookmarkTC.isEmpty()) {
            throw new MRException(MediaStoreServiceExceptionCodes.MISSING_BOOKMARK_ID_OR_TC, "Missing Either Bookmark ID or TC");
        }
    
        String fileName = bookmarkTC + "." + bookmarkID;
    
        return fileName;
    }*/

    // Get the document of a file from disk 
    protected Document getFileDocumentFromDisk(String rootDir, String fileName)
        throws MRException, Exception {
        String filePath = rootDir + "/" + fileName;
        String strMediaStoreDocument = getFileDocumentFromDisk(filePath);

        try {
            return MRXMLUtils.stringToDocument(strMediaStoreDocument);
        } catch (Exception exp) {
            return null;
        }
    }

    // Get the document of a file from disk 
    protected String getFileDocumentFromDisk(String filePath) throws MRException, Exception {
        File mediaStoreFile = new File(filePath);

        if (!mediaStoreFile.exists()) {
            return null;
        }

        String strMediaStoreDocument = FileUtils.readFile(mediaStoreFile);

        if ((strMediaStoreDocument == null) || strMediaStoreDocument.isEmpty()) {
            return null;
        }

        return strMediaStoreDocument;
    }

    //Write the given content to specified file
    protected void writeFileContent(String rootDir, String strMediaFileDoc, String fileName)
        throws MRException, Exception {
        String filePath = rootDir + "/" + fileName;
        FileUtils.writeFile(filePath, strMediaFileDoc);
    }

    //check the version of media store file for given media store uuid and parent path
    public String getVersionOfMediaStoreFile(String rootDir, String mediaStoreUUID)
        throws MRException, Exception {
        String parentDir = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreUUID;
        Document mediaStoreFileDoc = null;

        try {
            mediaStoreFileDoc = getFileDocumentFromDisk(parentDir, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);
        } catch (Exception exp) {
            //ignore exception
            mediaStoreFileDoc = null;
        }

        return getVersionOfMediaStoreFile(mediaStoreFileDoc);
    }

    //check the version of media store file for given media store file doc
    protected String getVersionOfMediaStoreFile(Document mediaStoreFileDoc) throws MRException, Exception {
        if (mediaStoreFileDoc == null) {
            return MediaVersionManager.MEDIA_STORE_VERSION_2_DOT_4;
        }

        String revisionValue = MRXMLUtils.getAttributeValue(mediaStoreFileDoc.getRootElement(),MediaStoreServiceConst.REVISION);

        if ((revisionValue == null) || revisionValue.isEmpty()) {
            return MediaVersionManager.MEDIA_STORE_VERSION_3_DOR_2;
        }
        String mediaStoreVersion = MRXMLUtils.getAttributeValue(mediaStoreFileDoc.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION, "0");
        return mediaStoreVersion;
    }

    //update the profile xml in media store and stream file
    public void initRecordOrPVRFile(String mediaStoreFileNID, String mediaStreamFileNID, Document actionDocument, String rootDir, 
    		AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject)
        throws MRException, Exception {
    	
      	
        String streamType = MRXMLUtils.getValueXpath(actionDocument, "//streamType", "");     
        if (MediaRoomServiceConst.RTP.equals(streamType)) { // Work around, have to decode the template and put it back in

            String sdptemplate = MRXMLUtils.getValueXpath(actionDocument, "//" + MediaRoomServiceConst.SDPTEMPLATE, "");
            sdptemplate = MediaRoomServiceUtils.encodeToFileSDPTemplate(sdptemplate);
            MRXMLUtils.getElementXpath(actionDocument, "//" + MediaRoomServiceConst.SDPTEMPLATE).setText(sdptemplate);
        }

        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, MRRequest.createRequest(this.userJID), log, db);

        MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//StreamType").setText(streamType);

        // Update the profileInfo in the mediaStreamFileDoc
        Element profileInfoParent = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaRoomServiceConst.MEDIA_STREAM_PROFILE_INFO);
        MediaStoreServiceUtils.removeChildren(profileInfoParent);

        Element profileElement = MRXMLUtils.getElementXpath(actionDocument, "//" + streamType + MediaRoomServiceConst.MEDIA_STREAM_PROFILE);
        Element profileInfoElement = (Element) profileElement.clone();
        profileInfoElement.setName(streamType + MediaRoomServiceConst.MEDIA_STREAM_PROFILE_INFO);
        profileInfoParent.add(profileInfoElement);
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//State/StateFlag", MediaStoreServiceConst.BUSY);
        int revision = this.getNewRevisionNumber(rootDir); 
        updateMediaFile(mediaStoreFileNID, mediaStreamFileNID, revision, mediaStreamFileDoc, rootDir,callBackObject,callBackDataObject);
    }

    //Update the metadata of media store and stream file
    public void updateMediaMetaData(String mediaStoreFileNID, String mediaStreamFileNID, int revision, Document actionDocument, String rootDir)
        throws MRException, Exception {
        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, MRRequest.createRequest(this.userJID), log, db);
        Element metaDataEL = null;

        if (actionDocument != null) {
            metaDataEL = MRXMLUtils.getElementXpath(actionDocument, "//MetaData");
        }

        Element fileMetaDataEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//MetaData");

        if (fileMetaDataEL != null) {
            if (metaDataEL != null) {
                MRXMLUtils.replaceContent(fileMetaDataEL, metaDataEL);
            }
        }

        updateMediaFile(mediaStoreFileNID, mediaStreamFileNID, revision, mediaStreamFileDoc, rootDir);
    }
    
    //update the bypassAutoStorageRules flag of media store and stream file
    public void updateFileBypassAutoRules(String mediaStoreFileNID, String mediaStreamFileNID, int revision, Document actionDocument, String rootDir)
            throws MRException, Exception {
            Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, MRRequest.createRequest(this.userJID), log, db);           
            Element bypassAutoStorageRule = null;
            String bypassValue = null;
           if (actionDocument != null) {
           
            	bypassAutoStorageRule = MRXMLUtils.getElementXpath(actionDocument, ".//"+MediaStoreServiceConst.SET_BYPASS_AUTO_STORAGE_RULES_ELEMENT);
            	if(bypassAutoStorageRule!=null){
            		bypassValue= MRXMLUtils.getAttributeValue(bypassAutoStorageRule, MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES);
            	}
            }
           
            Element fileBypassAutoStorageRule = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT);
            
            if (fileBypassAutoStorageRule != null) {
                if (bypassValue != null) {
                    MRXMLUtils.setValue(fileBypassAutoStorageRule, bypassValue);            
               }
            }
            
            updateMediaFile(mediaStoreFileNID, mediaStreamFileNID, revision, mediaStreamFileDoc, rootDir);
        }
    
   //Update the media store file in /blob/UUID/db.xml and updates the media stream file data base.
    public void updateMediaFile(String mediaStoreFileNID, String mediaStreamFileNID, int revision, Document mediaStreamFileDoc, String rootDir) throws MRException ,Exception {
    	updateMediaFile( mediaStoreFileNID,  mediaStreamFileNID,  revision,  mediaStreamFileDoc,  rootDir, null,null);
    }
    

    // Update the media store file in /blob/UUID/db.xml and updates the media stream file data base.
    public void updateMediaFile(String mediaStoreFileNID, String mediaStreamFileNID, int revision, Document mediaStreamFileDoc, String rootDir,AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject)
        throws MRException, Exception {
        MediaStoreServiceUtils.updateDateModified(mediaStreamFileDoc, this.xf.xmpp(userJID));
        String mediaStoreFilePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        if (FileUtils.isFileExist(mediaStoreFilePath, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME)) {
        	updateMediaStoreFile(mediaStoreFileNID, revision, mediaStreamFileDoc, rootDir);
        } else {
        	log.addInfo("MediaStoreServerImpl:updateMediaFile creating file=" + mediaStoreFilePath, MRLog.OK, MRLog.NOTIFY);
        	this.createMediaStoreFile(mediaStoreFileNID, revision, mediaStreamFileDoc, rootDir);
        }
        MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, revision);
        if(callBackObject != null) {
            MRXMLAsynchDBUtils.getInstance().replaceAsynch(MRXMLDBUtils.TIME_OUT, MRRequest.createRequest(this.userJID), log, db, "", "", mediaStreamFileNID, "", new String[]{}, mediaStreamFileDoc.getRootElement(),callBackDataObject,callBackObject, xf.sf(userJID));
        } else {
            MediaStoreServiceUtils.updateElement(MRRequest.createRequest(this.userJID), log, db, mediaStreamFileNID, mediaStreamFileDoc.getRootElement());
        }
        log.addInfo("Update Media File. MediaStoreFileNID:=" + mediaStoreFileNID + " MediaStreamFileNID:=" + mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);
    }
    
    //get the play length of given media store file
    public long getMediaPlayLegth(String mediaStoreFileNID, String rootDir)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        Element GapInfoEL = getMediaStoreGapInfo(mediaStoreFilePath);
        Document gapInfoDoc = MRXMLUtils.elementToDocument(GapInfoEL);
        List<?> segmentInfoList = MRXMLUtils.getListXpath(gapInfoDoc, "//MediaSegmentInfo");
        long maxOffset = 0;
        long mediaOffset = 0;

        for (Object segmentInfoObject : segmentInfoList) {
            Element segmentInfoEL = (Element) segmentInfoObject;
            mediaOffset = Utils.getLongValue(MRXMLUtils.getValueXpath(segmentInfoEL, "MediaOffset", ""), 0);

            if (mediaOffset >= maxOffset) {
                maxOffset = mediaOffset;
            }
        }

        Element lastSegmentEL = MRXMLUtils.getElementXpath(gapInfoDoc, "//MediaSegmentInfo[MediaOffset='" + maxOffset + "']");
        long startTC = Utils.getLongValue(MRXMLUtils.getValueXpath(lastSegmentEL, "StartTC", ""), 0);
        long endTC = Utils.getLongValue(MRXMLUtils.getValueXpath(lastSegmentEL, "EndTC", ""), 0);
        long playLegth = maxOffset;

        if ((endTC != -1) && (endTC != 0)) {
            playLegth = maxOffset + (endTC - startTC);
        }

        return playLegth;
    }

    //Get the size of the /blob/UUID directory
    public long getMediaSize(String mediaStoreFileNID, String rootDir) throws MRException, Exception {
        String mediaStoreFilePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        File storeFileObject = new File(mediaStoreFilePath);
        long size = FileUtils.getDirectorySize(storeFileObject);

        return size;
    }

    //This is a temporary fix. repair the media store INDEX file.
    public void repairMediaStoreIndexFile(String mediastoreFileNID, String rootDir)
        throws MRException, Exception {
        String filePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediastoreFileNID + "/" + MediaStoreServiceConst.MEDIA_STORE_INDEX_FILE_NAME;
        String[] cmdarr = { "/bin/sed", "-i", "-e", "1 s/-1/-0/", filePath };
        MediaRoomServiceUtils.executeSystemCommand(cmdarr);
    }

    /* //older media upgrade. Not used for current version synch
     public void makeMediaStreamFileAndMediaStoreFileSynch(HashMap<String, Document> mediaStoreAndStreamMapping, HashMap<String, Document> mediaStoreFileList, int storeRevision, MediaStoreInfo storeInfo)
         throws MRException, Exception {
         Set<String> mediaStoreKeySet = mediaStoreFileList.keySet();
         Document mediaStreamFileDoc = null;
         Document mediaStoreFileDoc = null;
         Document mediaFileDoc = null;
    
         for (String mediaStoreFileNID : mediaStoreKeySet) {
             mediaStreamFileDoc = mediaStoreAndStreamMapping.get(mediaStoreFileNID);
    
             if (mediaStreamFileDoc != null) {
                 //for older media if write permission is
                 boolean writePermission = Utils.getBooleanValue(MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PERMISSION_EL_NAME + "/" + MediaStoreServiceConst.WRITE, ""), true);
    
                 if (writePermission) {
                     MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, storeRevision);
                     MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PERMISSION_EL_NAME + "/" + MediaStoreServiceConst.WRITE, String.valueOf(false));
    
                     String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), MediaStoreServiceConst.NID, "");
                     MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaStreamFileDoc, mediaStreamFileNID, storeInfo.getMediaStoreRootPath(), xmpp, log, db);
                 }
    
                 continue;
             }
    
             mediaStoreFileDoc = mediaStoreFileList.get(mediaStoreFileNID);
    
             mediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
    
             //Which contains both media stream file and corresponding bookmarks.
             mediaFileDoc = generateMediaStreamFileDocFromStoreFile(mediaStoreFileDoc, mediaStreamFileDoc, mediaStoreFileNID, storeInfo, storeRevision, true, mediaStoreAndStreamMapping);
    
             if (mediaFileDoc == null) {
                 continue;
             }
    
             addMediaFile(mediaFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo);
         }
     }*/

    /*//Add media stream file and corresponding book marks
    protected String addMediaFile(Document mediaFileDoc, String mediaStoreFileNID, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreInfo storeInfo)
        throws MRException, Exception {
        Document mediaStreamFileDoc = MRXMLUtils.getDocumentXpath(mediaFileDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
    
        if (mediaStreamFileDoc == null) {
            return null;
        }
    
        String mediaStreamFileNID = addMediaStreamFile(mediaStreamFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo);
        Document bookMarkListDoc = MRXMLUtils.getDocumentXpath(mediaFileDoc, "//" + MediaStoreServiceConst.BOOKMARK_LIST);
    
        if (bookMarkListDoc != null) {
            List<?> bookMarkList = MRXMLUtils.getListXpath(bookMarkListDoc, "//" + AMConst.BOOKMARK);
    
            if ((bookMarkList != null) && !bookMarkList.isEmpty()) {
                Element bookMarkEL = null;
                MRRequest request = MRRequest.createRequest(this.userJID);
                log.addInfo("Add Bookmarks. StoreUUID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
    
                for (Object bookMarkObject : bookMarkList) {
                    bookMarkEL = (Element) bookMarkObject;
                    MRXMLUtils.setValueXpath(bookMarkEL, MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, mediaStreamFileNID);
                    MediaStoreServiceUtils.addAssetAdminObjectInDB(AMConst.BOOKMARK, request, bookMarkEL, log, db, xmpp, resourceFactory, serviceAgentFactory);
                }
            }
        }
    
        return mediaStreamFileNID;
    }
    
    protected String addMediaStreamFile(Document mediaStreamFileDoc, String mediaStoreFileNID, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreInfo storeInfo)
        throws MRException, Exception {
        MRRequest request = MRRequest.createRequest(this.userJID);
    
        Element mediaStreamFileEL = MediaStoreServiceUtils.addAssetAdminObjectInDB(AMConst.MEDIA_STREAM_FILE_RESOURCE, request, mediaStreamFileDoc.getRootElement(), log, db, xmpp, resourceFactory, serviceAgentFactory);
        mediaStreamFileDoc.removeContent();
        MRXMLUtils.addElement(mediaStreamFileDoc, (Element) mediaStreamFileEL.clone());
    
        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileEL, MediaStoreServiceConst.NID, "");
        mediaStoreAndStreamMapping.put(mediaStoreFileNID, mediaStreamFileDoc);
        storeInfo.incrementNotSynchCount();
        log.addInfo("Add Media Stream File. StoreUUID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
    
        return mediaStreamFileNID;
    }
    
    protected void updateMediaStreamFile(Document mediaStreamFileDoc, String mediaStoreFileNID, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreInfo storeInfo)
        throws MRException, Exception {
        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), "NID", "");
        MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaStreamFileDoc, mediaStreamFileNID, userJID, xmpp, log, db);
        mediaStoreAndStreamMapping.put(mediaStoreFileNID, mediaStreamFileDoc);
        storeInfo.incrementNotSynchCount();
        log.addInfo("Update Media Stream File. StoreUUID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
    }*/
    public Document getMediaStoreFileDoc(String mediaStoreFileNID, String rootDir)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID + "/";
        Document mediaStoredoc = getFileDocumentFromDisk(mediaStoreFilePath, MediaStoreServiceConst.MEDIA_STORE_DB_XML_FILE_NAME);

        return mediaStoredoc;
    }

    /*protected HashMap<String, Document> getListOfMediaStoreFile(String parentDirPath, String rootDir, HashMap<String, Document> mediaStoreAndStreamMapping)
        throws MRException, Exception {
        File file = new File(parentDirPath);
    
        String[] mediaStoreFileArray = file.list();
    
        if ((mediaStoreFileArray == null) || (mediaStoreFileArray.length == 0)) {
            return null;
        }
    
        Document mediaStoreFileDoc = null;
    
        HashMap<String, Document> mediaStoreMapping = new HashMap<String, Document>();
    
        for (String storeFileUUID : mediaStoreFileArray) {
            mediaStoreFileDoc = getMediaStoreFileDoc(storeFileUUID, rootDir);
    
            if (mediaStoreFileDoc == null) {
                continue;
            }
    
            mediaStoreMapping.put(storeFileUUID, mediaStoreFileDoc);
        }
    
        return mediaStoreMapping;
    }*/
    public String[] getListOfMediaStoreFiles(String parentDirPath) throws MRException, Exception {
        File file = new File(parentDirPath);
        String[] mediaStoreFileArray = file.list();

        return mediaStoreFileArray;
    }

    
    public void deleteFile(String filename) throws  Exception {
    	try {
    		File file = new File(filename);
    		if(file.exists()) {
    			file.delete();
    		}
    	}catch(Exception e) {
    		e.printStackTrace();
    	}
    }
    
    public String getExportFile(String parentDirPath) throws MRException, Exception {
        File file = new File(parentDirPath);
        String[] tgzFileArray = file.list();
        for(int i=0;i<tgzFileArray.length;i++) {
        	if(tgzFileArray[i].contains(".tar")) {
        		return tgzFileArray[i];
        	}
        }
        return null;
    }

    public void upgradeMediaStore(String mediaStoreFileNID, Document mediaStreamFileDoc, String rootDir, int revision)
        throws MRException, Exception {
    }

    public Element getMediaStoreTrack(String mediaStoreFileNID, Document requestDocument, String rootDir, String mediaType)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        Element mediaStoreTrackEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_STORE_FILE_TRACK);
        Element gapInfoElement = getMediaStoreGapInfo(mediaStoreFilePath);
        /*Element bookmarkTrackEL = getBookmarksList(mediaStoreFilePath);
        MRXMLUtils.addChildElement(mediaStoreTrackEL, bookmarkTrackEL);*/
        MRXMLUtils.addChildElement(mediaStoreTrackEL, gapInfoElement);

        log.addInfo("Get MediaStoreTack for MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + mediaStoreFilePath, MRLog.INFO, MRLog.NOTIFY);

        return mediaStoreTrackEL;
    }

    //return the current service version of the media store dir 
    public String getMediaStoreDirServiceVersion(String currentServiceVersion, String mediaStoreDirRootPath)
        throws MRException, Exception {    	
        String parentDirPath = mediaStoreDirRootPath + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
        String[] listOfMediaStorefiles = getListOfMediaStoreFiles(parentDirPath);
        if ((listOfMediaStorefiles == null) || (listOfMediaStorefiles.length == 0)) {
            return currentServiceVersion;
        }

        Document mediaStoreDocument = getFileDocumentFromDisk(mediaStoreDirRootPath, MediaStoreServiceConst.MEDIA_STORE_INFO_FILE_NAME);
        if (mediaStoreDocument == null) {
            return "0";
        }

        String mediaStoreDirServiceVersion = MRXMLUtils.getAttributeValue(mediaStoreDocument.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION, "0");
        return mediaStoreDirServiceVersion;
    }
    
    public void handleUpdateMediaPlayLengthResponse(String mediaStreamFileNiD,String mediaGroupID) throws Exception{
    	log.addInfo("handle UpdateMediaPlayLengthResponse. mediaStreamFileNiD:="+mediaStreamFileNiD);
    	if(mediaGroupID.isEmpty()) {
    		return;
    	}
        MediaStoreServiceUtils.sendUpdateMediaGroupPlayLenghtAndSizeRequesToAppserver(mediaGroupID,this.xf.xmpp(userJID),this.db,this.log,this.userJID, this.xf.sf(userJID));
    }
    
    
    
    
	
}
