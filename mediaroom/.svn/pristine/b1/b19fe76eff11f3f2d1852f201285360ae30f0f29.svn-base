package com.ipvs.device.mediastore;

import com.ipvs.utils.Utils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Set;


public class PlayList {
    String fillerMediaStreamFileNID = null;

    //filler and play list used same data structure in 2.4    
    PlayListEntry fillerMediaInfo = null;
    List<PlayListEntry> listOfPlayList = new ArrayList<PlayListEntry>();
    String playListGroupID = null;

    //Indexes
    int fillerMedia_index = 1;
    int listOfPlayList_index = 2;
    int playListGroupID_index = 3;

    @SuppressWarnings("unchecked")
    public PlayList(String phpSerializedString) {
        Object javaObject = Utils.unserializePHPSerializedObject(phpSerializedString);

        if (!Utils.isHashMapObject(javaObject)) {
            return;
        }

        HashMap<Object, Object> arrayObject = (HashMap<Object, Object>) javaObject;

        //set the filler media info
        if (arrayObject.get(fillerMedia_index) != null) {
            setFillerMediaInfo(new PlayListEntry((String) arrayObject.get(fillerMedia_index)));
        } else {
            setFillerMediaInfo(null);
        }

        String playListInfo = null;

        //set the list of play list array
        if (arrayObject.get(listOfPlayList_index) != null) {
            playListInfo = (String) arrayObject.get(listOfPlayList_index);

            if (!playListInfo.isEmpty()) {
                Object playListObject = Utils.unserializePHPSerializedObject(playListInfo);

                if (Utils.isHashMapObject(javaObject)) {
                    HashMap<Object, Object> playListArrayObject = (HashMap<Object, Object>) playListObject;
                    Set<Object> keySet = playListArrayObject.keySet();
                    String playListStr = null;
                    PlayListEntry playListEntry = null;

                    for (Object key : keySet) {
                        playListStr = (String) playListArrayObject.get((Integer) key);
                        playListEntry = new PlayListEntry(playListStr);

                        if (!playListEntry.getMediaID().isEmpty()) {
                            addPlayListEntry(playListEntry);
                        }
                    }
                }
            }
        }

        //set the play list group id
        if (arrayObject.get(playListGroupID_index) != null) {
            setPlayListGroupID((String) arrayObject.get(playListGroupID_index));
        } else {
            setPlayListGroupID("");
        }
    }

    public String getPlayListGroupID() {
        return playListGroupID;
    }

    public void setPlayListGroupID(String playListGroupID) {
        this.playListGroupID = playListGroupID;
    }

    public PlayListEntry getFillerMediaInfo() {
        return fillerMediaInfo;
    }

    public void setFillerMediaInfo(PlayListEntry fillerMediaInfo) {
        this.fillerMediaInfo = fillerMediaInfo;
    }

    public String getFillerMediaStreamFileNID() {
        return fillerMediaStreamFileNID;
    }

    public void setFillerMediaStreamFileNID(String fillerMediaStreamFileNID) {
        this.fillerMediaStreamFileNID = fillerMediaStreamFileNID;
    }

    public List<PlayListEntry> getListOfPlayList() {
        return listOfPlayList;
    }

    public void addPlayListEntry(PlayListEntry playListEntry) {
        listOfPlayList.add(playListEntry);
    }

    public void removePlayListEntry(PlayListEntry playListEntry) {
        listOfPlayList.remove(playListEntry);
    }

    public PlayListEntry getPlayListEntryBasedOnOrder(int playOrder) {
        for (PlayListEntry playListEntry : listOfPlayList) {
            if (playListEntry.getPlayOrder() == playOrder) {
                return playListEntry;
            }
        }

        return null;
    }
}
