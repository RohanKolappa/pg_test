package com.ipvs.device.mediastore;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.stream.StreamUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.utils.Utils;


public class MediaStreamFileEntry {
    private String mediaStreamFileNID = null;
    private String mediaType = null;
    private String title = null;
    private long playLength = 0;
    private String mediaIDList = null;
    private String roomNID = null;
    private long dateCreated = 0;
    private double roomConfigVersion = 0;
    private String streamType = null;
    private long size = 0;
    private String SDP = null;
    private long trimLength = 0;
    private String streamNID = null;
   
    //In case of  group
    private List<String> mediaStreamFileNIDList = new ArrayList<String>();

    //set the play list array string
    private String playListInfo = null;
    private String mediaGroupID = null;
    private boolean isPlayListAdded = false;

    //List<?> playList = null;
    private String description = null;
    private String bookMarkInfo = null;
    private boolean syn = false;

    //PlayList 
    PlayList playList = null;

    //2.4 media indexes mapping to 3.4
    private int mediaStreamFileNID_index = 0;
    private int mediaType_index = 1;
    private int title_index = 2;
    private int playLength_index = 4;
    private int mediaIDList_index = 6;
    private int roomNID_index = 7;
    private int dateCreated_index = 8;
    private int version_index = 9;
    private int streamType_index = 11;
    private int size_index = 12;
    private int SDP_index = 13;
    private int trimLength_index = 14;
    private int streamNID_index = 15;
    private int playListInfo_index = 16;
    private int bookMarkInfo_index = 17;
    private int description_index = 18;

    /* //2.4 media indexes not mapped to 3.4
     private int groups_index = 3;
     private int metaData_index = 5;
     private int portType_index = 10;
     private int dmsVersion_index = 19;
    */
    @SuppressWarnings("unchecked")
    public MediaStreamFileEntry(String phpSerializedString) {
        Object javaObject = Utils.unserializePHPSerializedObject(phpSerializedString);

        if (!Utils.isHashMapObject(javaObject)) {
            return;
        }

        HashMap<Object, Object> arrayObject = (HashMap<Object, Object>) javaObject;
        Integer typeOfMedia = (Integer) arrayObject.get(mediaType_index);
        long trimlength = Utils.getLongValue(String.valueOf(arrayObject.get(trimLength_index)), 0);
        setTrimLength(trimlength * 1000);
        
        if(typeOfMedia == null) {
        	return;
        }
        
        //set the media type
        if ((typeOfMedia == 4) && (getTrimLength() == 0)) {
            setMediaType(MediaRoomServiceConst.MEDIACLIP);
        } else if ((typeOfMedia == 4) && (getTrimLength() != 0)) {
            setMediaType(MediaRoomServiceConst.PVRMEDIACLIP);
        } else if ((typeOfMedia == 7) || (typeOfMedia == 107)) {
            setMediaType(MediaRoomServiceConst.MEDIAGROUP);
        } else if ((typeOfMedia == 100)) {
            setMediaType(MediaRoomServiceConst.PLAYLIST);
        }

        //set media NID 
        if (arrayObject.get(mediaStreamFileNID_index) != null) {
            setMediaStreamFileNID(String.valueOf(arrayObject.get(mediaStreamFileNID_index)));
        } else {
            setMediaStreamFileNID("");
        }

        //set title
        if (Utils.isStringObject(arrayObject.get(title_index))) {
        	String title = (String)arrayObject.get(title_index);
        	title = fixProblemWithPHPSerializationString(title);
            setTitle(title);
        } else {
            setTitle("");
        }

        //set play length
        long playLength = Utils.getLongValue(String.valueOf(arrayObject.get(playLength_index)), 0);
        setPlayLength(playLength * 1000);

        //set the room NID
        if (arrayObject.get(roomNID_index) != null) {
            setRoomNID(String.valueOf(arrayObject.get(roomNID_index)));
        } else {
            setRoomNID("");
        }

        //set date created
        long dateCreated = Utils.getLongValue(String.valueOf(arrayObject.get(dateCreated_index)), 0);
        setDateCreated(dateCreated * 1000);

        //set  room config version
        if (Utils.isIntegerObject(arrayObject.get(version_index))) {
            setRoomConfigVersion((Integer) arrayObject.get(version_index));
        } else {
            setRoomConfigVersion(0);
        }

        //set the media id list
        if (Utils.isStringObject(arrayObject.get(mediaIDList_index))) {
            setMediaIDList((String) arrayObject.get(mediaIDList_index));
        } else {
            setMediaIDList("");
        }

        //set the stream type
        int streamType = Utils.getIntValue(String.valueOf(arrayObject.get(streamType_index)), 0);

        if (streamType == 1) {
            setStreamType(MediaRoomServiceConst.V2D);
        } else if (streamType == 2) {
            setStreamType(MediaRoomServiceConst.MPEGTS);
        } else if (streamType == 3) {
            setStreamType(MediaRoomServiceConst.RTP);
        } else if (streamType == 4) {
            setStreamType(MediaRoomServiceConst.UDP);
        } else {
        	if(getMediaType().equals(MediaRoomServiceConst.MEDIACLIP)) {
                setStreamType(MediaRoomServiceConst.V2D);
            } else {
            	setStreamType("");
            }
        }

        //set the size
        String strSize = String.valueOf(arrayObject.get(size_index));
        StringTokenizer stkSize = new StringTokenizer(strSize);
        long size = Utils.getLongValue(stkSize.nextToken(), 0);        
        setSize(size * 1000);

        //set SDP
        if (Utils.isStringObject(arrayObject.get(SDP_index))) {
        	String sdp = (String) arrayObject.get(SDP_index);
        	sdp =fixProblemWithPHPSerializationString(sdp);     	
            setSDP(sdp);
        } else {
            setSDP("");
        }

        //set stream NID
        if (arrayObject.get(streamNID_index) != null) {
            setStreamNID(String.valueOf(arrayObject.get(streamNID_index)));
        } else {
            setStreamNID("");
        }

        //set the book mark
        if (Utils.isStringObject(arrayObject.get(bookMarkInfo_index))) {
            setBookMarkInfo((String) arrayObject.get(bookMarkInfo_index));
        } else {
            setBookMarkInfo("");
        }

        //set the play list array
        if (Utils.isStringObject(arrayObject.get(playListInfo_index))) {
            setPlayListInfo((String) arrayObject.get(playListInfo_index));
        } else {
            setPlayListInfo("");
        }

        //set the description
        if (Utils.isStringObject(arrayObject.get(description_index))) {
            setDescription((String) arrayObject.get(description_index));
        } else {
            setDescription("");
        }
    }
    //Fix the problem with ;: parsing related to PHP Serialization problem	
    private String fixProblemWithPHPSerializationString(String string){
    	return string.replace(";:", ":");
    }

    public boolean isPlayListAdded() {
        return isPlayListAdded;
    }

    public void setPlayListAdded(boolean isPlayListAdded) {
        this.isPlayListAdded = isPlayListAdded;
    }

    public String getMediaGroupID() {
        return mediaGroupID;
    }

    public void setMediaGroupID(String mediaGroupID) {
        this.mediaGroupID = mediaGroupID;
    }

    public PlayList getPlayList() {
        return playList;
    }

    public void setPlayList(PlayList playList) {
        this.playList = playList;
    }

    public String getPlayListInfo() {
        return playListInfo;
    }

    public void setPlayListInfo(String playListInfo) {
        this.playListInfo = playListInfo;
    }

    public boolean isSyn() {
        return syn;
    }

    public void setSyn(boolean syn) {
        this.syn = syn;
    }

    public String getMediaIDList() {
        return mediaIDList;
    }

    public void setMediaIDList(String mediaIDList) {
        this.mediaIDList = mediaIDList;
    }

    public void addMediaStreamFileNIDToList(String mediaStreamFileNID) {
        this.mediaStreamFileNIDList.add(mediaStreamFileNID);
    }

    public List<String> getMediaStreamFileNIDList() {
        return this.mediaStreamFileNIDList;
    }

    public String getBookMarkInfo() {
        return bookMarkInfo;
    }

    public void setBookMarkInfo(String bookMarkInfo) {
        this.bookMarkInfo = bookMarkInfo;
    }

    public String getMediaStreamFileNID() {
        return mediaStreamFileNID;
    }

    public void setMediaStreamFileNID(String mediaStreamFileNID) {
        this.mediaStreamFileNID = mediaStreamFileNID;
    }

    public String getMediaType() {
        return mediaType;
    }

    public void setMediaType(String mediaType) {
        this.mediaType = mediaType;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public long getPlayLength() {
        return playLength;
    }

    public void setPlayLength(long playLength) {
        this.playLength = playLength;
    }

    public String getRoomNID() {
        return roomNID;
    }

    public void setRoomNID(String roomNID) {
        this.roomNID = roomNID;
    }

    public long getDateCreated() {
        return dateCreated;
    }

    public void setDateCreated(long dateCreated) {
        this.dateCreated = dateCreated;
    }

    public double getRoomConfigVersion() {
        return roomConfigVersion;
    }

    public void setRoomConfigVersion(double roomConfigVersion) {
        this.roomConfigVersion = roomConfigVersion;
    }

    public String getStreamType() {
        return streamType;
    }

    public void setStreamType(String streamType) {
        this.streamType = streamType;
    }

    public long getSize() {
        return size;
    }

    public void setSize(long size) {
        this.size = size;
    }

    public String getSDP() {
        return SDP;
    }

    public void setSDP(String sdp) {
        SDP = sdp;
    }

    public long getTrimLength() {
        return trimLength;
    }

    public void setTrimLength(long trimLength) {
        this.trimLength = trimLength;
    }

    public String getStreamNID() {
        return streamNID;
    }

    public void setStreamNID(String streamNID) {
        this.streamNID = streamNID;
    }

    /* public List<?> getPlayListInfo() {
         return playListInfo;
     }
    
     public void setPlayListInfo(List<?> playListInfo) {
         this.playListInfo = playListInfo;
     }*/
    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public Document getClipMediaStreamFileDoc(Document mediaStreamFileDoc, String mediaStorePortNID,Element dirGroupsEL)
        throws MRException, Exception {
        Document newMediaStreamFileDoc = (Document) mediaStreamFileDoc.clone();

        Element infoEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.INFO_ELEMENT_NAME);

        //set the Media Info Like title description
        setMediaFileInfo(newMediaStreamFileDoc, mediaStorePortNID,dirGroupsEL);

        String mediaType = getMediaType();
        MediaStoreServiceUtils.updateInfoElementBasedOnType(infoEL, mediaType);

        if (mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)) {
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.PVR_TRIM_LENGTH_EL_NAME, String.valueOf(getTrimLength()));
        }

        //set the profile
        Element profileEL = (Element) MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + getStreamType() + MediaRoomServiceConst.MEDIA_STREAM_PROFILE_INFO).clone();
        Element profileInfoParent = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaRoomServiceConst.MEDIA_STREAM_PROFILE_INFO);
        MediaStoreServiceUtils.removeChildren(profileInfoParent);
        MRXMLUtils.addElement(profileInfoParent, profileEL);

        if (getStreamType().equals(MediaRoomServiceConst.RTP)) {
            MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaRoomServiceConst.SDPTEMPLATE, getSDP());
        }

        //update media type
        setMediaTypeOfFile(newMediaStreamFileDoc);

        //set meta data
        setMetaData(newMediaStreamFileDoc);
        MediaStoreServiceUtils.updateMediaGroupID(newMediaStreamFileDoc, "");

        //set size and playLength 
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, String.valueOf(getPlayLength()));
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(getSize()));

        return newMediaStreamFileDoc;
    }

    public Document getClipGroupMediaStreamFileDoc(Document mediaStreamFileDoc, String parentDirNID, String mediaGroupID,Element dirGroupsEL)
        throws MRException, Exception {
        Document newMediaStreamFileDoc = (Document) mediaStreamFileDoc.clone();

        Element infoEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.INFO_ELEMENT_NAME);

        //set the Media Info Like title description
        setMediaFileInfo(newMediaStreamFileDoc, parentDirNID, dirGroupsEL);

        MediaStoreServiceUtils.updateInfoElementBasedOnType(infoEL, getMediaType());

        //update media type
        setMediaTypeOfFile(newMediaStreamFileDoc);

        //set meta data
        setMetaData(newMediaStreamFileDoc);
        MediaStoreServiceUtils.updateMediaGroupID(newMediaStreamFileDoc, mediaGroupID);

        Element mediaGroupTypeEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + getMediaType());
        Element mediaStreamFileNIDEL = (Element) MRXMLUtils.getElement(mediaGroupTypeEL, MediaStoreServiceConst.MEDIA_STREAM_FILE_NID).detach();

        for (String mediaStreamFileNID : mediaStreamFileNIDList) {
            MRXMLUtils.setValue(mediaStreamFileNIDEL, mediaStreamFileNID);
            MRXMLUtils.addElement(mediaGroupTypeEL, (Element) mediaStreamFileNIDEL.clone());
        }

        //set the sync flag 
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_SYNC_EL_NAME, String.valueOf(isSyn()));

        //set size and playLength 
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, String.valueOf(getPlayLength()));
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(getSize()));

        return newMediaStreamFileDoc;
    }

    public Document getPlayListMediaStreamFileDoc(Document mediaStreamFileDoc, String mediaStorePortResourceNID,Element dirGroupsEL)
        throws MRException, Exception {
        Document newMediaStreamFileDoc = (Document) mediaStreamFileDoc.clone();

        Element infoEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.INFO_ELEMENT_NAME);

        //set the Media Info Like title description
        setMediaFileInfo(newMediaStreamFileDoc, mediaStorePortResourceNID,dirGroupsEL);

        MediaStoreServiceUtils.updateInfoElementBasedOnType(infoEL, getMediaType());

        //update media type
        setMediaTypeOfFile(newMediaStreamFileDoc);

        //set meta data
        setMetaData(newMediaStreamFileDoc);
        MediaStoreServiceUtils.updateMediaGroupID(newMediaStreamFileDoc, mediaGroupID);

        PlayList playList = getPlayList();
        Element playListEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + getMediaType());
        MRXMLUtils.setValueXpath(playListEL, MediaStoreServiceConst.FILLER_INFO + "/" + MediaStoreServiceConst.FILLER_MEDIA_STREAM_FILE_NID, playList.getFillerMediaStreamFileNID());
        MRXMLUtils.setValueXpath(playListEL, MediaStoreServiceConst.FILLER_INFO + "/" + MediaStoreServiceConst.FILLER_MEDIA_START_OFFSET, String.valueOf(playList.getFillerMediaInfo().getMediaStartOffset()));
        MRXMLUtils.setValueXpath(playListEL, MediaStoreServiceConst.FILLER_INFO + "/" + MediaStoreServiceConst.FILLER_MEDIA_PLAYLENGTH, String.valueOf(playList.getFillerMediaInfo().getMediaPlayLength()));

        //set play list entries id
        Element playListEntriesEL = MRXMLUtils.getElement(playListEL, MediaStoreServiceConst.PLAYLIST_ENTIRES);
        MRXMLUtils.setAttributeValue(playListEntriesEL, MediaStoreServiceConst.ID, getMediaStreamFileNID());

        Element playListEntryEL = (Element) MRXMLUtils.getElement(playListEntriesEL, MediaStoreServiceConst.PLAYLIST_ENTRY).detach();

        // add the play list entry to play list entries
        for (PlayListEntry playListEntry : playList.getListOfPlayList()) {
            if ((playListEntry.getMediaStreamFileNID() == null) || playListEntry.getMediaStreamFileNID().trim().isEmpty()) {
                continue;
            }

            MRXMLUtils.setAttributeValue(playListEntryEL, MediaStoreServiceConst.ID, playListEntry.getPlayListEntryID());
            MRXMLUtils.setValueXpath(playListEntryEL, MediaStoreServiceConst.PLAY_LIST_ENTRY_START_OFFSET, String.valueOf(playListEntry.getPlaylistEntryStartOffset()));
            MRXMLUtils.setValueXpath(playListEntryEL, MediaStoreServiceConst.PLAY_LIST_ENTRY_PLAY_LENGTH, String.valueOf(playListEntry.getPlaylistEntryPlayLength()));
            MRXMLUtils.setValueXpath(playListEntryEL, MediaStoreServiceConst.MEDIA_STREAM_FILE_NID, playListEntry.getMediaStreamFileNID());
            MRXMLUtils.setValueXpath(playListEntryEL, MediaStoreServiceConst.PLAY_ORDER, String.valueOf(playListEntry.getPlayOrder()));
            MRXMLUtils.setValueXpath(playListEntryEL, MediaStoreServiceConst.LOOP_COUNT, String.valueOf(playListEntry.getLoopCount()));
            MRXMLUtils.setValueXpath(playListEntryEL, MediaStoreServiceConst.MEDIA_START_OFFSET, String.valueOf(playListEntry.getMediaStartOffset()));
            MRXMLUtils.setValueXpath(playListEntryEL, MediaStoreServiceConst.MEDIA_PLAYLENGTH, String.valueOf(playListEntry.getMediaPlayLength()));
            MRXMLUtils.setValueXpath(playListEntryEL, MediaStoreServiceConst.FILLER_INFO + "/" + MediaStoreServiceConst.FILLER_MEDIA_STREAM_FILE_NID, playListEntry.getFillerMediaStreamFileNID());
            MRXMLUtils.addElement(playListEntriesEL, (Element) playListEntryEL.clone());
        }

        //set size and playLength 
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, String.valueOf(getPlayLength()));
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(getSize()));

        return newMediaStreamFileDoc;
    }

    private void setMetaData(Document newMediaStreamFileDoc) throws MRException, Exception {
        //update media data info        
        Element metaDataEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
        Element metaDataTagEL = (Element) MRXMLUtils.getElementXpath(metaDataEL, MediaStoreServiceConst.META_DATA_TAG_ELEMENT_NAME).clone();
        MRXMLUtils.removeContent(metaDataEL);
        //set room NID in meta data 
        addMetaDataInfo(MediaStoreServiceConst.ROOM_NID, getRoomNID(), metaDataEL, metaDataTagEL);

        //set stream NID in meta data 
        addMetaDataInfo(MediaStoreServiceConst.STREAM_NID, getStreamNID(), metaDataEL, metaDataTagEL);

        //set Version in meta data 
        addMetaDataInfo(MediaStoreServiceConst.ROOM_CONFIG_VERSION, String.valueOf(getRoomConfigVersion()), metaDataEL, metaDataTagEL);
    }

    private void addMetaDataInfo(String metaDataTagNameAttValue, String metaDataTagValue, Element metaDataEL, Element metaDataTagEL)
        throws MRException, Exception {
        MRXMLUtils.setAttributeValue(metaDataTagEL, MediaStoreServiceConst.META_DATA_TAG_ATT_NAME, metaDataTagNameAttValue);
        MRXMLUtils.setValue(metaDataTagEL, metaDataTagValue);

        MRXMLUtils.addElement(metaDataEL, (Element) metaDataTagEL.clone());
    }

    private void setMediaTypeOfFile(Document newMediaStreamFileDoc) throws MRException, Exception {
        //set type
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, getMediaType());
    }

    private void setMediaFileInfo(Document newMediaStreamFileDoc, String parentMediaDirNID,Element dirGroupsEL)
        throws MRException, Exception {
    	
        //set the groups 
    	MediaStoreServiceUtils.updateDirGroupsInFile(newMediaStreamFileDoc, dirGroupsEL);
    	
        //set UUID
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.UUID, getMediaStreamFileNID());

        //if it is new file in the data base then set the parent dir NID as NONE.
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME, parentMediaDirNID);

        //set title,description and stream type
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, getDescription());
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, getTitle());
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, getStreamType());
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.READY);

        //set the  file permissions. no write only read, xfer and stream 
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.READ, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.FALSE);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.XFER, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.STREAM, MediaStoreServiceConst.TRUE);

        //set date created
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED, String.valueOf(getDateCreated()));
    }

    @SuppressWarnings("unchecked")
    public Element generateBookmarksDoc(MRLog log,StreamUtils sourceTable, MediaStoreDirectoryInfo storeInfo, String mediaStoreClipFileNID, String mediaType) throws MRException, Exception {
        String phpSerializedString = getBookMarkInfo();
        Element bookMarkListEL = MRXMLUtils.newElement(MediaStoreServiceConst.BOOKMARK_LIST);

        if ((phpSerializedString == null) || phpSerializedString.isEmpty()) {
            return null;
        }

        Object javaObject = Utils.unserializePHPSerializedObject(phpSerializedString);

        if (!javaObject.getClass().equals(HashMap.class)) {
            return null;
        }

        HashMap<Object, Object> arrayObject = (HashMap<Object, Object>) javaObject;

        Set<Object> objectKeySet = arrayObject.keySet();
        String bookmarkInfoSerializedString = null;
        BookMarkInfo bookmarkInfo = null;

        Document bookmarkObjectXMLDoc = MediaStoreServiceUtils.loadBookmarkDocument(log);
        Document bookmarkDoc = null;
        
        for (Object key : objectKeySet) {
            bookmarkInfoSerializedString = (String) arrayObject.get((Integer) key);
            bookmarkInfo = new BookMarkInfo(bookmarkInfoSerializedString, log);
           	bookmarkDoc = bookmarkInfo.generateBookMarkDoc(bookmarkObjectXMLDoc,mediaStoreClipFileNID,sourceTable, storeInfo);
            MRXMLUtils.addElement(bookMarkListEL, (Element) bookmarkDoc.getRootElement().clone());
        }

        return bookMarkListEL;
    }
}
