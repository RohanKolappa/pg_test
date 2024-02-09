package com.ipvs.mediastoreservice.impl;

import java.util.HashMap;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;


public class DummyMediaStoreServerImpl extends MediaStoreServerImpl {
    public DummyMediaStoreServerImpl(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID) {
        super(xf, log, db, resourceFactory, userJID);
    }

    public void createMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        String parentDir = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
        log.addInfo("Create Media Store File With Revision :=" + revision + " MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }

    public void updateMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        String parentDir = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
        log.addInfo("Update Media Store File. MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }

    public void deleteMediaStoreFile(String mediaStoreFileNID, String rootDir, boolean purgeFiles)
        throws MRException, Exception {
        String parentDir = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
        log.addInfo("Delete Media Store File. MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }

    public void addBookMark(String mediaStoreFileNID, Document bookmarkDoc, String rootDir)
        throws MRException, Exception {
        String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
        log.addInfo("Add BookMark for MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }

    public void updateBookMark(String mediaStoreFileNID, Document bookmarkDoc, String rootDir)
        throws MRException, Exception {
        String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
        log.addInfo("Update BookMark for MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }

    public void deleteBookMark(String mediaStoreFileNID, Document bookmarkDoc, String rootDir)
        throws MRException, Exception {
        String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
        log.addInfo("Delete BookMark from MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);
    }

    public Element getMediaStoreTrack(String mediaStoreFileNID, Document requestDocument, String rootDir, String mediaType)
        throws MRException, Exception {
        String parentDir = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;
        log.addInfo("Get MediaStoreTack for MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + parentDir, MRLog.INFO, MRLog.NOTIFY);

        return null;
    }

    //Once update play length dummy one is implemented then we will implement this method
    public long getMediaStoreFileLastSegmentPlayLength(String mediaStoreFileNID, long endTC, String rootDir)
        throws MRException, Exception {
        return 0;
    }

    //Update Media Store File Status.
    public void updateMediaStoreFileStatus(String mediaStoreFileNID, int revision, Document fileDoc, String state, String rootDir)
        throws MRException, Exception {
        log.addInfo("Update Media Store File Status:=" + state, MRLog.INFO, MRLog.NOTIFY);
    }

    //Return the dummy revision
    public Integer getNewRevisionNumber(String rootDir) throws MRException, Exception {
        return MediaStoreServiceConst.DEFAULT_REVISION;
    }

    //returns the default revision
    public Integer getMediaStoreRevisionNumber(String rootDir,boolean isWritable) throws MRException, Exception {
        return MediaStoreServiceConst.DEFAULT_REVISION;
    }

    /*public HashMap<String, Document> getMediaStoreFileListWhichOrNotInSynch(int revision, String rootDir, HashMap<String, Document> mediaStoreAndStreamMapping)
        throws MRException, Exception {
        return null;
    }*/
    public void createMediaStoreInfo(String rootDir, int revision, MRLog log)
        throws MRException, Exception {
    }

    public void updateMediaStoreInfo(String rootDir, int revision) throws MRException, Exception {
    }

    public void updateSynMediaStoreFile(String mediaStoreFileNID, int revision, Document storeDocument, String rootDir)
        throws MRException, Exception {
    }

    public Document getMediaStoreFileDoc(String mediaStoreFileNID, String rootDir)
        throws MRException, Exception {
        return null;
    }

    public Document generateMediaStreamFileDocFromStoreFile(Document mediaStoreFileDoc, Document mediaStreamFileDoc, String mediaStoreFileNID, MediaStoreDirectoryInfo storeInfo, Element diorGroupsEL, int storeRevision, boolean isNewFile, HashMap<String, Document> mediaStoreAndStreamMapping)
        throws MRException, Exception {
        return null;
    }

    //check the version of media store file for given media store uuid and parent path
    public String getVersionOfMediaStoreFile(String rootDir, String mediaStoreUUID)
        throws MRException, Exception {
        return MediaVersionManager.getServiceVersion(log);
    }

    public void repairMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
    }

    public String getMediaStoreDirUUID(String mediaStoreDirRootPath) throws MRException, Exception {
        return this.userJID;
    }

    public void setupMediaStoreDir(String mediaStoreDirRootPath, String UUID)
        throws MRException, Exception {
    }

    //return the current service version
    public double getMediaStoreDirServiceVersion(double currentServiceVersion, String mediaStoreDirRootPath)
        throws MRException, Exception {
        return currentServiceVersion;
    }

	
	public String upgradeMediaStoreDir(String rootDir) throws MRException,
			Exception {		
		return this.userJID;
	}
	
	//Get the size of the /blob/UUID directory
    public long getMediaSize(String mediaStoreFileNID, String rootDir) throws MRException, Exception {        
        return 0;
    }
    
   //get the play length of given media store file
    public long getMediaPlayLegth(String mediaStoreFileNID, String rootDir)
        throws MRException, Exception {
        return 0;
    }
    
    
    public void deleteFile(String filename) throws  Exception {
    	log.addInfo("Deleting file " + filename, MRLog.INFO, MRLog.NOTIFY);
    }
    
    public String getExportFile(String parentDirPath) throws MRException, Exception {
    	log.addInfo("Get tgzfile " + parentDirPath, MRLog.INFO, MRLog.NOTIFY);
    	return null;
    }

	@Override
	public boolean isMediaStoreExist(String rootDir, String mediaStoreFileNID)
			throws MRException, Exception {
		// TODO Auto-generated method stub
		return true;
	}

}
