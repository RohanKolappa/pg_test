package com.ipvs.device.mediastore;

import java.util.HashMap;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.SDPUtils;
import com.ipvs.mediaroomservice.stream.StreamUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServerImpl;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public class Device2Dot4MediaStoreServerImpl extends MediaStoreServerImpl {
    public Device2Dot4MediaStoreServerImpl(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
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

    public Document getMediaStoreFileDoc(String mediaStoreFileNID, String rootDir)
        throws MRException, Exception {
        String filePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_DB_DIR + "/" + mediaStoreFileNID;
        String strMediaStoreFileDoc = getFileDocumentFromDisk(filePath);

        if (strMediaStoreFileDoc == null) {
            return null;
        }

        strMediaStoreFileDoc = strMediaStoreFileDoc.replaceFirst("ID=", "ID=\"\"");

        try {
            return MRXMLUtils.stringToDocument(strMediaStoreFileDoc);
        } catch (Exception exp) {
            log.addInfo("File Not a Proper XML. MediaStoreFileNID:=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
            log.addInfo("Exception:=" + exp.toString(), MRLog.INFO, MRLog.NOTIFY);

            return null;
        }
    }

    /*  public HashMap<String, Document> getMediaStoreFileListWhichOrNotInSynch(int revision, String rootDir, HashMap<String, Document> mediaStoreAndStreamMapping)
          throws MRException, Exception {
          String parentDirPath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_DB_DIR + "/";
          //Get list of 3.2 media files in /db/UUID
          return getListOfMediaStoreFile(parentDirPath, rootDir, mediaStoreAndStreamMapping);
      }*/
    public Integer getMediaStoreRevisionNumber(String rootDir, boolean isWritable)
        throws MRException, Exception {
        operationNotAllowed();

        return null;
    }

    /*public Element getMediaStoreTrack(String mediaStoreFileNID, Document requestDocument, String rootDir, String mediaType)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR + "/" + mediaStoreFileNID;
        Element mediaStoreTrackEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_STORE_FILE_TRACK);
        Element gapInfoElement = null;
    
        if (mediaType.equals(MediaRoomServiceConst.MEDIACLIP) || mediaType.equals(MediaRoomServiceConst.MEDIACLIP)) {
            gapInfoElement = getMediaStoreGapInfo(mediaStoreFilePath);
        }
    
        Element bookmarkTrackEL = null;
    
        if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP) || mediaType.equals(MediaRoomServiceConst.PLAYLIST)) {
            //read store file and create media stream file entry from PHP serialized object
            Document mediaStoreFileDoc = getMediaStoreFileDoc(mediaStoreFileNID, rootDir);
            String phpSerializedString = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//METADATA", "");
            MediaStreamFileEntry streamFileEntry = new MediaStreamFileEntry(phpSerializedString);
            String bookMarkPhpSerializedString = streamFileEntry.getBookMarkInfo();
           // bookmarkTrackEL = getBookmaksList(bookMarkPhpSerializedString);
        }
        if (gapInfoElement != null) {
            MRXMLUtils.addChildElement(mediaStoreTrackEL, gapInfoElement);
        }
    
        if (bookmarkTrackEL != null) {
            MRXMLUtils.addChildElement(mediaStoreTrackEL, bookmarkTrackEL);
        }
        log.addInfo("Get MediaStoreTack for MediaStoreFileNID:=" + mediaStoreFileNID + " Dir:=" + mediaStoreFilePath, MRLog.INFO, MRLog.NOTIFY);
    
        return mediaStoreTrackEL;
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

    private boolean isMediaStoreFileExistsInblob(String rootDir, String mediaStoreFileNID)
        throws MRException, Exception {
        String mediaStoreFilePath = rootDir + "/" + MediaStoreServiceConst.MEDIA_STORE_BLOB_DIR;

        return FileUtils.isFileExist(mediaStoreFilePath, mediaStoreFileNID);
    }


    public Document generateMediaStreamFileDocFromStoreFile(Document mediaStoreFileDoc, Document mediaStreamFileDoc, String mediaStoreFileNID, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, int storeRevision, boolean isNewFile, HashMap<String, Document> mediaStoreAndStreamMapping)
        throws MRException, Exception {
        int typeOfMedia = Utils.getIntValue(MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//TYPE", ""), 0);

        if ((typeOfMedia != 4) && (typeOfMedia != 5)) {
            return null;
        }

        String mediaFlag = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//FLAG", "");

        if (mediaFlag.isEmpty() || !mediaFlag.equals("V2R")) {
            return null;
        }

        String phpSerializedString = MRXMLUtils.getValueXpath(mediaStoreFileDoc, "//METADATA", "");

        if (phpSerializedString.isEmpty()) {
            return null;
        }

        MediaStreamFileEntry streamFileEntryObject = new MediaStreamFileEntry(phpSerializedString);
        if ((streamFileEntryObject.getMediaStreamFileNID() == null) || streamFileEntryObject.getMediaStreamFileNID().isEmpty() || !streamFileEntryObject.getMediaStreamFileNID().equalsIgnoreCase(mediaStoreFileNID)) {
            return null;
        }
        String mediaType = streamFileEntryObject.getMediaType();
        Document newMediaStreamFileDoc = null;
        StreamUtils sourceTable = StreamUtils.getStreamUtils(this.userJID);
        Element rootEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_FILE_DOC);
        if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
            String mediaGroupID = Utils.getUUID();
            handleMediaGroup(streamFileEntryObject, mediaStoreAndStreamMapping, storeInfo, dirGroupsEL, mediaGroupID, storeRevision, mediaStreamFileDoc);

            if (streamFileEntryObject.getMediaStreamFileNIDList().size() > 0) {
                streamFileEntryObject.setPlayLength(getMediaGroupPlayLength(mediaStoreAndStreamMapping, streamFileEntryObject.getMediaIDList()));
                newMediaStreamFileDoc = streamFileEntryObject.getClipGroupMediaStreamFileDoc(mediaStreamFileDoc, storeInfo.getMediaStoreDirNID(), mediaGroupID, dirGroupsEL);
            }
            if(newMediaStreamFileDoc == null) {
            	return null;
            }
            String mediaStoreClipFileNID = "";
	       	String mediaIDList = streamFileEntryObject.getMediaIDList();
    	   	StringTokenizer groupListSTK = new StringTokenizer(mediaIDList, ",");
        	while (groupListSTK.hasMoreTokens()) {
        		mediaStoreClipFileNID = groupListSTK.nextToken();
        		Document  syncFileDocRoot = mediaStoreAndStreamMapping.get(mediaStoreClipFileNID);
        		Document syncFileDoc = null;
        		if(syncFileDocRoot!= null) {
        			String mediaClipStreamFileNID = MediaStoreServiceUtils.getMediaStreamFileNID(syncFileDocRoot);
        			syncFileDoc =  MRXMLDBUtils.readElement(MRRequest.createRequest(userJID), log, db, mediaClipStreamFileNID);
        		}	
        		if(syncFileDoc != null) {
        			String syncFileNID = MRXMLUtils.getAttributeValueXpath(syncFileDoc, "//MediaGroupID", "ClipID", "");
        			MRXMLUtils.setAttributeValueXPath(newMediaStreamFileDoc, "//MediaGroupID", "ClipID", syncFileNID);
        			break;
        		}	
	        }	
    	    Element bookMarkListEL = streamFileEntryObject.generateBookmarksDoc(log, sourceTable, storeInfo, mediaStoreClipFileNID, mediaType);
    	    if (bookMarkListEL != null) {
        	    MRXMLUtils.addElement(rootEL, (Element) bookMarkListEL.detach());
            }

        } else if (mediaType.equals(MediaRoomServiceConst.MEDIACLIP) || mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)) {
            if (isMediaStoreFileExistsInblob(storeInfo.getMediaStoreRootPath(), mediaStoreFileNID)) {
                String parentDirNID = storeInfo.getMediaStoreDirNID();

                if (MediaRoomServiceConst.PVRMEDIACLIP.equals(mediaType)) {
                    parentDirNID = storeInfo.getPvrDirNID();
                }

                streamFileEntryObject.setPlayLength(getMediaPlayLegth(mediaStoreFileNID, storeInfo.getMediaStoreRootPath()));
                streamFileEntryObject.setSize(getMediaSize(mediaStoreFileNID, storeInfo.getMediaStoreRootPath()));
                newMediaStreamFileDoc = streamFileEntryObject.getClipMediaStreamFileDoc(mediaStreamFileDoc, parentDirNID, dirGroupsEL);
                if (streamFileEntryObject.getStreamType().equals(MediaRoomServiceConst.RTP)) {
                	String sdp = SDPUtils.fixSDPProblemWithOlderMedia(streamFileEntryObject.getSDP());
                	sdp = SDPUtils.getSDPTemplateFromSDP(sdp,MRLog.getInstance(MRLog.CONSOLE_DAEMON));                 
                	String encodedSDP = MediaRoomServiceUtils.encodeToFileSDPTemplate(sdp);
                	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaRoomServiceConst.SDPTEMPLATE, encodedSDP);
                }
                MRXMLUtils.setAttributeValueXPath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, MediaStoreServiceConst.CLIP_ID, Utils.getUUID());
            }
        } else if (mediaType.equals(MediaRoomServiceConst.PLAYLIST)) {
            handlePlayList(streamFileEntryObject, mediaStoreAndStreamMapping, storeInfo, dirGroupsEL, storeRevision, mediaStreamFileDoc);

            //check play list added or not. If not then only generate new document
            if (!streamFileEntryObject.isPlayListAdded() && (streamFileEntryObject.getPlayList().getListOfPlayList().size() > 0)) {
                newMediaStreamFileDoc = streamFileEntryObject.getPlayListMediaStreamFileDoc(mediaStreamFileDoc, storeInfo.getMediaStoreDirNID(), dirGroupsEL);                
                MRXMLUtils.setAttributeValueXPath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, MediaStoreServiceConst.CLIP_ID, Utils.getUUID());
            }
        }

        if (newMediaStreamFileDoc == null) {
            return null;
        }

        Document mediaFileDoc = MRXMLUtils.getDocument();
        if (mediaType.equals(MediaRoomServiceConst.MEDIACLIP) || mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)) {
            MediaStoreServiceUtils.updateRevision(newMediaStreamFileDoc, storeRevision);
        }
        
        MRXMLUtils.setAttributeValueXPath(newMediaStreamFileDoc, "//RecInfo", "SessionConfigNID", streamFileEntryObject.getRoomNID());         
        MRXMLUtils.addElement(rootEL, (Element)newMediaStreamFileDoc.getRootElement().detach());
        MRXMLUtils.addElement(mediaFileDoc, (Element) rootEL.clone());

        return mediaFileDoc;
    }

    private long getMediaGroupPlayLength(HashMap<String, Document> mediaStoreAndStreamMapping, String mediaIDList)
        throws MRException, Exception {
        if ((mediaIDList == null) || mediaIDList.isEmpty()) {
            return 0;
        }

        StringTokenizer groupListSTK = new StringTokenizer(mediaIDList, ",");
        String mediaStoreFileNID = null;
        Document mediaStreamFileDoc = null;
        long maxPlayLength = 0;
        long playLength = 0;

        while (groupListSTK.hasMoreTokens()) {
            mediaStoreFileNID = groupListSTK.nextToken();
            mediaStreamFileDoc = mediaStoreAndStreamMapping.get(mediaStoreFileNID);

            if (mediaStreamFileDoc == null) {
                continue;
            }

            playLength = Utils.getLongValue(MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, ""), 0);

            if (maxPlayLength < playLength) {
                maxPlayLength = playLength;
            }
        }

        return maxPlayLength;
    }

    private void handlePlayList(MediaStreamFileEntry streamFileEntryObject, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, int storeRevision, Document mediaStreamFileObjectXMLDoc)
        throws MRException, Exception {
        String playListPHPSerializedString = streamFileEntryObject.getPlayListInfo();
        PlayList playList = new PlayList(playListPHPSerializedString);

        //check play list group is there in the db or not
        String playListGroupMediaStoreFileNID = playList.getPlayListGroupID();
        Document playListGroupMediaStreamFileDocRoot = mediaStoreAndStreamMapping.get(playListGroupMediaStoreFileNID);
        String mediaGroupID = "";

        //get the group id from media group file document. If not then group id will update when added media group
        if (playListGroupMediaStreamFileDocRoot != null) {
        	String NID = MediaStoreServiceUtils.getMediaStreamFileNID(playListGroupMediaStreamFileDocRoot);
        	Document playListGroupMediaStreamFileDoc = MRXMLDBUtils.readElement(MRRequest.createRequest(userJID), log, db, NID);
            mediaGroupID = MRXMLUtils.getValueXpath(playListGroupMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
        }

        //set play list group id
        streamFileEntryObject.setMediaGroupID(mediaGroupID);

        //Check file already added in in the process of adding play list group or not.
        Document fileDoc = mediaStoreAndStreamMapping.get(streamFileEntryObject.getMediaStreamFileNID());

        if (fileDoc != null) {
            streamFileEntryObject.setPlayListAdded(true);

            return;
        }

        //check play list filler media stream file exists in the data base or not. If not then add the file
        PlayListEntry fillerEntryInfo = playList.getFillerMediaInfo();
        String fillerMediaStreamFileNID = null;

        if ((fillerEntryInfo.getMediaID() != null) && !fillerEntryInfo.getMediaID().isEmpty()) {
            fillerMediaStreamFileNID = getMediaStreamFileNID(fillerEntryInfo.getMediaID(), mediaStoreAndStreamMapping, "", mediaStreamFileObjectXMLDoc, storeInfo, dirGroupsEL, storeRevision);
        }

        playList.setFillerMediaStreamFileNID(fillerMediaStreamFileNID);

        //Calculate the play list start and offset. 
        //And check play list entry clip exists in the data base or not. If not add to the data base 
        PlayListEntry playListEntry = null;
        long playListStartOffset = 0;
        String mediaStoreFileNID = null;
        String mediaStreamFileNID = null;
        long playLength = 0;

        for (int playOrder = 1; playOrder <= playList.getListOfPlayList().size(); playOrder++) {
            playListEntry = playList.getPlayListEntryBasedOnOrder(playOrder);

            if (playListEntry == null) {
                continue;
            }

            mediaStoreFileNID = playListEntry.getMediaID();

            if ((mediaStoreFileNID == null) || mediaStoreFileNID.isEmpty()) {
                playList.removePlayListEntry(playListEntry);

                continue;
            }

            //set play list start offset
            playListEntry.setPlaylistEntryStartOffset(playListStartOffset);

            //check play list entry filler media id there in data base or not. if not add
            if ((playListEntry.getFillerMediaID() != null) || !playListEntry.getFillerMediaID().isEmpty()) {
                fillerMediaStreamFileNID = getMediaStreamFileNID(playListEntry.getFillerMediaID(), mediaStoreAndStreamMapping, mediaGroupID, mediaStreamFileObjectXMLDoc, storeInfo, dirGroupsEL, storeRevision);
                playListEntry.setFillerMediaStreamFileNID(fillerMediaStreamFileNID);
            }

            //check play list media there in the data base or not if not then add that file            
            mediaStreamFileNID = getMediaStreamFileNID(mediaStoreFileNID, mediaStoreAndStreamMapping, "", mediaStreamFileObjectXMLDoc, storeInfo, dirGroupsEL, storeRevision);

            if ((mediaStreamFileNID == null) || mediaStreamFileNID.isEmpty()) {
                playList.removePlayListEntry(playListEntry);

                continue;
            }

            //set next play list entry start offset 
            playListStartOffset = playListStartOffset + playListEntry.getPlaylistEntryPlayLength();

            playLength = playListStartOffset;

            playListEntry.setMediaStreamFileNID(mediaStreamFileNID);
        }

        //set playlength 
        streamFileEntryObject.setPlayLength(playLength);
        //set play list in media stream file 
        streamFileEntryObject.setPlayList(playList);
    }

    private String getMediaStreamFileNID(String mediaStoreFileNID, HashMap<String, Document> mediaStoreAndStreamMapping, String mediaGroupID, Document mediaStreamFileObjectXMLDoc, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, int storeRevision)
        throws MRException, Exception {
        Document mediaStreamFileDoc = mediaStoreAndStreamMapping.get(mediaStoreFileNID);
        String mediaStreamFileNID = null;

        if (mediaStreamFileDoc == null) {
            mediaStreamFileNID = addMediaStreamFile(mediaStoreFileNID, storeInfo, dirGroupsEL, mediaStreamFileObjectXMLDoc, mediaGroupID, mediaStoreAndStreamMapping, storeRevision);
        } else {
            mediaStreamFileNID = MediaStoreServiceUtils.getMediaStreamFileNID(mediaStreamFileDoc);
        }

        return mediaStreamFileNID;
    }

    private void handleMediaGroup(MediaStreamFileEntry groupFileEntryObject, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, String mediaGroupID, int storeRevision, Document mediaStreamFileObjectXMLDoc)
        throws MRException, Exception {
        if ((groupFileEntryObject.getMediaIDList() == null) || groupFileEntryObject.getMediaIDList().isEmpty()) {
            groupFileEntryObject.setSyn(false);

            return;
        } else {
            groupFileEntryObject.setSyn(true);
        }

        StringTokenizer groupListSTK = new StringTokenizer(groupFileEntryObject.getMediaIDList(), ",");
        String mediaStoreFileNID = null;
        Document mediaStreamFileDocRoot = null;
        String mediaStreamFileNID = null;

        while (groupListSTK.hasMoreTokens()) {
            mediaStoreFileNID = groupListSTK.nextToken();
            if(mediaStoreFileNID.equalsIgnoreCase(mediaGroupID)) {
				continue;
            }
            mediaStreamFileDocRoot = mediaStoreAndStreamMapping.get(mediaStoreFileNID);

            if (mediaStreamFileDocRoot == null) {
                mediaStreamFileNID = addMediaStreamFile(mediaStoreFileNID, storeInfo, dirGroupsEL, mediaStreamFileObjectXMLDoc, mediaGroupID, mediaStoreAndStreamMapping, storeRevision);
            } else {
            	String NID = MediaStoreServiceUtils.getMediaStreamFileNID(mediaStreamFileDocRoot);
            	Document mediaStreamFileDoc = MRXMLDBUtils.readElement(MRRequest.createRequest(userJID), log, db, NID);  
                mediaStreamFileNID = updateMediaStreamFile(mediaStreamFileDoc, storeRevision, mediaGroupID, storeInfo, mediaStoreFileNID, mediaStoreAndStreamMapping);
            }

            if (mediaStreamFileNID == null) {
                continue;
            }

            groupFileEntryObject.addMediaStreamFileNIDToList(mediaStreamFileNID);
        }
    }

    private String updateMediaStreamFile(Document mediaStreamFileDoc, int storeRevision, String mediaGroupID, MediaStoreDirectoryInfo storeInfo, String mediaStoreFileNID, HashMap<String, Document> mediaStoreAndStreamMapping)
        throws MRException, Exception {
        MediaStoreServiceUtils.updateRevision(mediaStreamFileDoc, storeRevision);
        MediaStoreServiceUtils.updateMediaGroupID(mediaStreamFileDoc, mediaGroupID);

        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), MediaStoreServiceConst.NID, "");
        MediaStoreServiceUtils.synchUpdateMediaStreamFile(mediaStreamFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, this.userJID, log, db, xf.xmpp(userJID), xf.sf(userJID));

        return mediaStreamFileNID;
    }

    private String addMediaStreamFile(String mediaStoreFileNID, MediaStoreDirectoryInfo storeInfo, Element dirGroupsEL, Document mediaStreamFileObjectXMLDoc, String mediaGroupID, HashMap<String, Document> mediaStoreAndStreamMapping, int storeRevision)
        throws MRException, Exception {
        Document mediaStoreFileDoc = getMediaStoreFileDoc(mediaStoreFileNID, storeInfo.getMediaStoreRootPath());

        if (mediaStoreFileDoc == null) {
            return null;
        }

        Document mediaFileDoc = generateMediaStreamFileDocFromStoreFile(mediaStoreFileDoc, mediaStreamFileObjectXMLDoc, mediaStoreFileNID, storeInfo, dirGroupsEL, storeRevision, true, mediaStoreAndStreamMapping);

        if (mediaFileDoc == null) {
            return null;
        }

        MediaStoreServiceUtils.updateMediaGroupID(mediaFileDoc, mediaGroupID);

        String mediaStreamFileNID = MediaStoreServiceUtils.synchAddMediaFile(mediaFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, this.userJID, log, db, xf.xmpp(userJID), resourceFactory, xf.sf(userJID));

        return mediaStreamFileNID;
    }

    public void repairMediaStoreFile(String mediaStoreFileNID, int revision, Document fileDoc, String rootDir)
        throws MRException, Exception {
        operationNotAllowed();
    }

    public String getMediaStoreDirUUID(String mediaStoreDirRootPath) throws MRException, Exception {
        operationNotAllowed();

        return null;
    }

    public void setupMediaStoreDir(String mediaStoreDirRootPath, String UUID)
        throws MRException, Exception {
        operationNotAllowed();
    }

    @Override
    public String upgradeMediaStoreDir(String rootDir) throws MRException, Exception {
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
