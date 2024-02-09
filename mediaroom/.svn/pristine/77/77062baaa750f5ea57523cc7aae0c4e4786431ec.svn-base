package com.ipvs.device.mediastore;

import com.ipvs.utils.Utils;

import java.util.HashMap;


public class PlayListEntry {
    String playListID = null;
    String PlayListEntryID = null;
    String mediaID = null;
    String mediaStreamFileNID = null;
    long mediaStartOffset = 0;
    long mediaPlayLength = 0;
    int loopCount = 0;
    long playlistEntryStartOffset = 0;
    long playlistEntryPlayLength = 0;
    String fillerMediaStreamFileNID = null;
    String fillerMediaID = null;
    int playOrder = 0;

    //index
    int playListID_index = 0;
    int playListEntryID_index = 1;
    int mediaID_index = 3;
    int mediaStartOffset_index = 5;
    int mediaLength_index = 6;
    int playLength_index = 7;
    int fillerMediaID_index = 8;
    int loopCount_index = 9;
    int playOrder_index = 10;

    @SuppressWarnings("unchecked")
    public PlayListEntry(String phpSerializedString) {
        Object javaObject = Utils.unserializePHPSerializedObject(phpSerializedString);

        if (!Utils.isHashMapObject(javaObject)) {
            return;
        }

        HashMap<Object, Object> arrayObject = (HashMap<Object, Object>) javaObject;

        //set the play list id
        if (arrayObject.get(playListID_index) != null) {
            setPlayListID(String.valueOf(arrayObject.get(playListID_index)));
        } else {
            setPlayListID("");
        }

        //set the play list entry id
        if (arrayObject.get(playListEntryID_index) != null) {
            setPlayListEntryID(String.valueOf(arrayObject.get(playListEntryID_index)));
        } else {
            setPlayListEntryID(Utils.getUUID());
        }

        //set the media ID
        if (arrayObject.get(mediaID_index) != null) {
            setMediaID(String.valueOf(arrayObject.get(mediaID_index)));
        } else {
            setMediaID("");
        }

        // if no media id then return  invalid play list entry
        if (getMediaID().isEmpty()) {
            return;
        }

        //set the media start offset
        setMediaStartOffset(Utils.getLongValue(String.valueOf(arrayObject.get(mediaStartOffset_index)), 0));
        //set media length
        setMediaPlayLength(Utils.getLongValue(String.valueOf(arrayObject.get(mediaLength_index)), 0) * 1000);
        //set play length 
        setPlaylistEntryPlayLength(Utils.getLongValue(String.valueOf(arrayObject.get(playLength_index)), 0) * 1000);

        //set filler media id 
        if (arrayObject.get(fillerMediaID_index) != null) {
            setFillerMediaID(String.valueOf(arrayObject.get(fillerMediaID_index)));
        } else {
            setFillerMediaID("");
        }

        //set the loop count
        setLoopCount(Utils.getIntValue(String.valueOf(arrayObject.get(loopCount_index)), 0));
        //set the play oder
        setPlayOrder(Utils.getIntValue(String.valueOf(arrayObject.get(playOrder_index)), 0));
    }

    public String getMediaStreamFileNID() {
        return mediaStreamFileNID;
    }

    public void setMediaStreamFileNID(String mediaStreamFileNID) {
        this.mediaStreamFileNID = mediaStreamFileNID;
    }

    public String getPlayListID() {
        return playListID;
    }

    public void setPlayListID(String playListID) {
        this.playListID = playListID;
    }

    public String getPlayListEntryID() {
        return PlayListEntryID;
    }

    public void setPlayListEntryID(String playListEntryID) {
        PlayListEntryID = playListEntryID;
    }

    public String getMediaID() {
        return mediaID;
    }

    public void setMediaID(String mediaID) {
        this.mediaID = mediaID;
    }

    public long getMediaStartOffset() {
        return mediaStartOffset;
    }

    public void setMediaStartOffset(long mediaStartOffset) {
        this.mediaStartOffset = mediaStartOffset;
    }

    public String getFillerMediaStreamFileNID() {
        return fillerMediaStreamFileNID;
    }

    public void setFillerMediaStreamFileNID(String fillerMediaStreamFileNID) {
        this.fillerMediaStreamFileNID = fillerMediaStreamFileNID;
    }

    public String getFillerMediaID() {
        return fillerMediaID;
    }

    public void setFillerMediaID(String fillerMediaID) {
        this.fillerMediaID = fillerMediaID;
    }

    public int getLoopCount() {
        return loopCount;
    }

    public void setLoopCount(int loopCount) {
        this.loopCount = loopCount;
    }

    public int getPlayOrder() {
        return playOrder;
    }

    public void setPlayOrder(int playOrder) {
        this.playOrder = playOrder;
    }

    public long getMediaPlayLength() {
        return mediaPlayLength;
    }

    public void setMediaPlayLength(long mediaPlayLength) {
        this.mediaPlayLength = mediaPlayLength;
    }

    public long getPlaylistEntryStartOffset() {
        return playlistEntryStartOffset;
    }

    public void setPlaylistEntryStartOffset(long playlistEntryStartOffset) {
        this.playlistEntryStartOffset = playlistEntryStartOffset;
    }

    public long getPlaylistEntryPlayLength() {
        return playlistEntryPlayLength;
    }

    public void setPlaylistEntryPlayLength(long playlistEntryPlayLength) {
        this.playlistEntryPlayLength = playlistEntryPlayLength;
    }
}
