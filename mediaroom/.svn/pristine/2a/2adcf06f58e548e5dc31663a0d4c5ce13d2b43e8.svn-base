package com.ipvs.device.mediastore;

import java.util.HashMap;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.stream.StreamUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerImpl;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;


public class Device3Dot2MediaStoreServerImpl extends MediaStoreServerImpl {
    public Device3Dot2MediaStoreServerImpl(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
    		String userJID) {
        super(xf, log, db, resourceFactory, userJID);
    }

    public long getMediaStoreFileLastSegmentPlayLength(String mediaStoreFileNID, long endTC, String rootDir)
        throws MRException, Exception {
        return 0;
    }

    public void addBookMark(String mediaStoreFileNID, Document bookmarkDoc, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public void createMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public void createMediaStoreInfo(String rootDir, int revision, MRLog log)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public void deleteBookMark(String mediaStoreFileNID, Document bookmarkDoc, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public void deleteMediaStoreFile(String mediaStoreFileNID, String rootDir, boolean purgeFiles)
        throws MRException, Exception {
        operationNotAllowed();
    }

    /* public HashMap<String, Document> getMediaStoreFileListWhichOrNotInSynch(int revision, String rootDir, HashMap<String, Document> mediaStoreAndStreamMapping)
         throws MRException, Exception {
         String parentDirPath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/";
    
         //Get list of 3.2 media files in /blob/UUID/db.xml
         return getListOfMediaStoreFile(parentDirPath, rootDir, mediaStoreAndStreamMapping);
     }*/
    public Integer getMediaStoreRevisionNumber(String rootDir,boolean isWritable) throws MRException, Exception {
        return null;
    }

   /* public Element getMediaStoreTrack(String mediaStoreFileNID, Document requestDocument, String rootDir, String mediaType)
        throws MRException, Exception {
        return null;
    }*/

    public Integer getNewRevisionNumber(String rootDir) throws MRException, Exception {
        return null;
    }

    public void updateBookMark(String mediaStoreFileNID, Document bookmarkDoc, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public void updateMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public void updateMediaStoreFileStatus(String mediaStoreFileNID, int revision, Document fileDoc, String state, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public void updateMediaStoreInfo(String rootDir, int revision) throws MRException, Exception {
        operationNotAllowed();
    }

    public void updateSynMediaStoreFile(String mediaStoreFileNID, int revision, Document storeDocument, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public Document generateMediaStreamFileDocFromStoreFile(Document mediaStoreFileDoc, Document mediaStreamFileDoc, String mediaStoreFileNID, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, int storeRevision, boolean isNewFile, HashMap<String, Document> mediaStoreAndStreamMapping)
        throws MRException, Exception {
        Document newMediaStreamFileDoc = (Document) mediaStreamFileDoc.clone();

        Element infoEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.INFO_ELEMENT_NAME);
        
        //set the groups 
    	MediaStoreServiceUtils.updateDirGroupsInFile(newMediaStreamFileDoc, dirGroupsEL);
    

        //set UUID
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.UUID, mediaStoreFileNID);        
        

        MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIACLIP);
        MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PVRMEDIACLIP);
        MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAGROUP);
        MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PLAYLIST);

        //set title,description and stream type
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, ""));
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, ""));
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, MediaRoomServiceConst.V2D);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.READY);

         //added code as per Shankar to fix the DATE_CREATED problem during migration
        /*
          <MediaTrackIndex>
             <MediaSegmentInfo>
              <MediaOffset>0</MediaOffset>
              <StartTC>1251394338795</StartTC>
              <EndTC>1251394635708</EndTC>
             </MediaSegmentInfo>
            </MediaTrackIndex>
            <BookmarkTrack /> 
         */
        String dateCreatedValue= MRXMLUtils.getValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED, "");
        if(dateCreatedValue.equals("")){
            String startTCValue= MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//MediaSegmentInfo[MediaOffset='0']/StartTC", "9");
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED,startTCValue);
        }

        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, ""));
        
        //set the permission
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.READ, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.FALSE);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.XFER, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.STREAM, MediaStoreServiceConst.TRUE);

        // Media Group ID
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, ""));

        long size = getMediaSize(mediaStoreFileNID, storeInfo.getMediaStoreRootPath());
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//Size", String.valueOf(size));

        //set type        
        String mediaType = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
        if(MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType)) {
        	 MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME, storeInfo.getPvrDirNID());             
        } else {
        	 MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME, storeInfo.getMediaStoreDirNID());
        }
        
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, mediaType);

        //update media store file info
        Element mediaStoreFileEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT);
        MRXMLUtils.removeChild(mediaStoreFileEL,MediaStoreServiceConst.META_DATA_ELEMENT_NAME);

        Element metaDataEL = (Element) MRXMLUtils.getElementXpath(mediaStoreFileDoc, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME).clone();
        MRXMLUtils.addElement(mediaStoreFileEL, metaDataEL);

        Element mediaStoreMediaTypeEL = (Element) MRXMLUtils.getElementXpath(mediaStoreFileDoc, "//" + mediaType).clone();

        if (mediaStoreMediaTypeEL == null) {
            return null;
        }

        Element bookmarkTrackEL = MRXMLUtils.getElement(mediaStoreMediaTypeEL, "BookmarkTrack");
        Element mediaTrackIndexEL = MRXMLUtils.getElement(mediaStoreMediaTypeEL, "MediaTrackIndex");
        Element bookMarkListEL = null;

        if (bookmarkTrackEL != null) {
            bookmarkTrackEL.detach();
            bookMarkListEL = getBookmarkList(bookmarkTrackEL, mediaStoreFileNID,storeInfo.getMediaStoreRootPath());
        }

        if (mediaTrackIndexEL != null) {
            mediaTrackIndexEL.detach();
        }

        //add the media type element
        MRXMLUtils.addElement(infoEL, (Element) mediaStoreMediaTypeEL.clone());

        MediaStoreServiceUtils.updateRevision(newMediaStreamFileDoc, storeRevision);

        Document mediaFileDoc = MRXMLUtils.getDocument();
        Element rootEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_FILE_DOC);

        MRXMLUtils.addElement(rootEL, (Element)newMediaStreamFileDoc.getRootElement().detach());

        if (bookMarkListEL != null) {
            MRXMLUtils.addElement(rootEL, (Element) bookMarkListEL.detach());
        }

        MRXMLUtils.addElement(mediaFileDoc, (Element) rootEL.clone());

        return mediaFileDoc;
    }

    private Element getBookmarkList(Element bookmarkTrackEL, String mediaStoreFileNID,String rootDirPath)
        throws MRException, Exception {
        List<?> bookmarkList = MRXMLUtils.getList(bookmarkTrackEL);

        if ((bookmarkList == null) || bookmarkList.isEmpty()) {
            return null;
        }

        Element bookmarkEL = null;
        Document bookmarkDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.BOOKMARK, this.log);
        String timeCode = null;
        StreamUtils streamUtils = StreamUtils.getStreamUtils(this.userJID);
        Element bookMarkListEL = MRXMLUtils.newElement(MediaStoreServiceConst.BOOKMARK_LIST);

        long offset = 0;

        for (Object object : bookmarkList) {
            bookmarkEL = (Element) object;
            timeCode = MRXMLUtils.getValueXpath(bookmarkEL, "AtTC", "");
            offset = streamUtils.getOffset(mediaStoreFileNID, timeCode,rootDirPath);
            MRXMLUtils.setValueXpath(bookmarkDoc, "//AtTC", timeCode);
            MRXMLUtils.setValueXpath(bookmarkDoc, "//Offset", String.valueOf(offset));
            MRXMLUtils.setValueXpath(bookmarkDoc, "//" + MediaStoreServiceConst.DATE_CREATED, MRXMLUtils.getValueXpath(bookmarkEL, MediaStoreServiceConst.DATE_CREATED, ""));
            MRXMLUtils.setValueXpath(bookmarkDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, MRXMLUtils.getValueXpath(bookmarkEL, MediaStoreServiceConst.DATE_MODIFIED, ""));
            MRXMLUtils.setValueXpath(bookmarkDoc, "//AuthorUserID", MRXMLUtils.getValueXpath(bookmarkEL, "AuthorUserID", ""));
            MRXMLUtils.setValueXpath(bookmarkDoc, "//Title", MRXMLUtils.getValueXpath(bookmarkEL, "Title", ""));
            MRXMLUtils.setValueXpath(bookmarkDoc, "//Comment", MRXMLUtils.getValueXpath(bookmarkEL, "Comment", ""));
            MRXMLUtils.addElement(bookMarkListEL, (Element) bookmarkDoc.getRootElement().clone());
        }

        return bookMarkListEL;
    }

    public void repairMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    @Override
    public String getMediaStoreDirUUID(String mediaStoreDirRootPath) throws MRException, Exception {
        operationNotAllowed();

        return null;
    }

    @Override
    public void setupMediaStoreDir(String mediaStoreDirRootPath, String UUID)
        throws MRException, Exception {
        operationNotAllowed();
    }

	@Override
	public String upgradeMediaStoreDir(String rootDir) throws MRException,
			Exception {
		operationNotAllowed();
		return null;
	}

	@Override
	public boolean isMediaStoreExist(String rootDir, String mediaStoreFileNID)
			throws MRException, Exception {
		operationNotAllowed();
		return false;
	}
}
