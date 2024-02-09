package com.ipvs.mediastoreservice.impl;

import java.util.HashMap;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.AsynchDBRequestResponseListenerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;


public interface MediaStoreServerI {
    public void createMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir) throws MRException, Exception;

    public void updateMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir) throws MRException, Exception;

    public void deleteMediaStoreFile(String mediaStoreFileNID, String rootDir, boolean purgeFiles) throws MRException, Exception;

    public void updateMediaPlayLength(String mediaStoreFileNID, String mediaStreamDirNID, int revision, Document actionDocument, String rootDir,AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject) throws MRException, Exception;

    public void updateMediaStatus(String mediaStoreFileNID, String mediaStreamFileNID, int revision, Document actionDocument, String rootDir) throws MRException, Exception;

    public void initRecordOrPVRFile(String mediaStoreFileNID, String mediaStreamFileNID, Document actionDocument, String rootDir, AsynchDBRequestResponseListenerI callBackObject,Object callBackDataObject) throws MRException, Exception;

    public void updateMediaMetaData(String mediaStoreFileNID, String mediaStreamFileNID, int revision, Document actionDocument, String rootDir) throws MRException, Exception;
    
    public void updateMediaFile(String mediaStoreFileNID,String mediaStreamFileNID,int revision, Document mediaStreamFileDoc, String rootDir) throws MRException, Exception;

    public Element getMediaStoreTrack(String mediaStoreFileNID, Document requestDocument, String rootDir,String mediaType) throws MRException, Exception;

    public void updateMediaStoreFileStatus(String mediaStoreFileNID, int revision, Document fileDoc, String state, String rootDir) throws MRException, Exception;

    public Integer getNewRevisionNumber(String rootDir) throws MRException, Exception;

    public Integer getMediaStoreRevisionNumber(String rootDir,boolean isWritable) throws MRException, Exception;

    public void updateMediaStoreInfo(String rootDir, int revision) throws MRException, Exception;

    public void createMediaStoreInfo(String rootDir, int revision, MRLog log) throws MRException, Exception;

    public String getVersionOfMediaStoreFile(String rootDir, String mediaStoreUUID) throws MRException, Exception;

    public String generateMediaStoreFileDoc(Document fileDoc, int revision, String state, MRLog log) throws MRException, Exception;
   
    public void updateSynMediaStoreFile(String mediaStoreFileNID, int revision, Document storeDocument, String rootDir) throws MRException, Exception;
    
    public Document getMediaStoreFileDoc(String mediaStoreFileNID, String rootDir) throws MRException, Exception;
    
    public long getMediaPlayLegth(String mediaStoreFileNID,String rootDir) throws MRException,Exception;
    
    public long getMediaSize(String mediaStoreFileNID,String rootDir) throws MRException,Exception;
    
    public void repairMediaStoreIndexFile(String mediaStoreFileNID,String rootDir) throws MRException,Exception;
    
    public Document generateMediaStreamFileDocFromStoreFile(Document mediaStoreFileDoc, Document mediaStreamFileDoc, String mediaStoreFileNID,MediaStoreDirectoryInfo storeInfo,Element dirGroups, int storeRevision, boolean isNewFile,HashMap<String, Document> mediaStoreAndStreamMapping) throws MRException, Exception;
    
    public void repairMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir) throws MRException, Exception;
  
    public String[] getListOfMediaStoreFiles(String rootDir) throws MRException, Exception;
   
    public void upgradeMediaStore(String mediaStoreFileNID,Document mediaStreamFileDoc,String rootDir,int revision) throws MRException,Exception;
    
    public String getMediaStoreDirUUID(String mediaStoreDirRootPath) throws MRException,Exception;
    
    public void setupMediaStoreDir(String mediaStoreDirRootPath,String UUID) throws MRException,Exception;

    public String getMediaStoreDirServiceVersion(String currentServiceVersion,String mediaStoreDirRootPath) throws MRException,Exception;
    
    public String upgradeMediaStoreDir(String rootDir) throws MRException,Exception;
    
    public String getExportFile(String parentDirPath) throws MRException, Exception;
    
    public void deleteFile(String filename) throws  Exception;
    
    public abstract boolean isMediaStoreExist(String rootDir , String mediaStoreFileNID) throws MRException, Exception;
    
    public void updateFileBypassAutoRules(String mediaStoreFileNID, String mediaStreamFileNID, int revision, Document actionDocument, String rootDir) throws MRException, Exception;
    
}
