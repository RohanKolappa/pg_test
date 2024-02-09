package com.ipvs.device.mediastore;

import java.util.HashMap;

import org.dom4j.Document;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.stream.StreamUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.utils.Utils;


public class BookMarkInfo {
    String bookmarkID = null;
    String title = null;
    long timeCode = 0;
    long dateModified = 0;
    String comment = null;
    String ownerUserJID = null;

    //indexes
    int bookmarkID_index = 0;
    int title_index = 1;
    int timeCode_index = 2;
    int dateModified_index = 3;
    int comment_index = 5;
    int ownerUserJID_index = 4;

    @SuppressWarnings("unchecked")
    public BookMarkInfo(String phpSerializedString, MRLog log) {
        Object javaObject = Utils.unserializePHPSerializedObject(phpSerializedString);

        if (!Utils.isHashMapObject(javaObject)) {
            return;
        }

        HashMap<Object, Object> arrayObject = (HashMap<Object, Object>) javaObject;

        setBookmarkID(String.valueOf(arrayObject.get(bookmarkID_index)));

        //set the title
        if (Utils.isStringObject(arrayObject.get(title_index))) {
            setTitle((String) arrayObject.get(title_index));
        } else {
            setTitle("");
        }

        //set TC
        setTimeCode(Utils.getLongValue((String) arrayObject.get(timeCode_index), 0));

        //set date modified
        long datemodified = Utils.getLongValue(String.valueOf(arrayObject.get(dateModified_index)), 0);
        setDateModified(datemodified * 1000);

        //set comment
        if (Utils.isStringObject(arrayObject.get(comment_index))) {
            setComment((String) arrayObject.get(comment_index));
        } else {
            setComment("");
        }

        //set owner JID
        if (Utils.isStringObject(arrayObject.get(ownerUserJID_index))) {
            setOwnerUserJID((String) arrayObject.get(ownerUserJID_index));
        } else {
            setOwnerUserJID("");
        }
    }

    public Document generateBookMarkDoc(Document bookmarkDoc,String mediaStoreFileNID,StreamUtils sourceTable, MediaStoreDirectoryInfo storeInfo) throws MRException, Exception {
        Document newBookmarkDoc = (Document) bookmarkDoc.clone();
        String timecodeStr =String.valueOf(getTimeCode());        
        MRXMLUtils.setValueXpath(newBookmarkDoc, "//AtTC", timecodeStr);
        long offset = sourceTable.getOffset(mediaStoreFileNID, timecodeStr, storeInfo.getMediaStoreRootPath());
        MRXMLUtils.setValueXpath(newBookmarkDoc, "//Offset", String.valueOf(offset));
        MRXMLUtils.setValueXpath(newBookmarkDoc, "//DateCreated", String.valueOf(getDateModified()));
        MRXMLUtils.setValueXpath(newBookmarkDoc, "//DateModified", String.valueOf(getDateModified()));
        MRXMLUtils.setValueXpath(newBookmarkDoc, "//AuthorUserID", getOwnerUserJID());
        MRXMLUtils.setValueXpath(newBookmarkDoc, "//Title", getTitle());
        MRXMLUtils.setValueXpath(newBookmarkDoc, "//Comment", getComment());
        return newBookmarkDoc;
    }

    public String getBookmarkID() {
        return bookmarkID;
    }

    public void setBookmarkID(String bookmarkID) {
        this.bookmarkID = bookmarkID;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public long getTimeCode() {
        return timeCode;
    }

    public void setTimeCode(long timeCode) {
        this.timeCode = timeCode;
    }

    public long getDateModified() {
        return dateModified;
    }

    public void setDateModified(long dateModified) {
        this.dateModified = dateModified;
    }

    public String getComment() {
        return comment;
    }

    public void setComment(String comment) {
        this.comment = comment;
    }

    public String getOwnerUserJID() {
        return ownerUserJID;
    }

    public void setOwnerUserJID(String ownerUserJID) {
        this.ownerUserJID = ownerUserJID;
    }
}
