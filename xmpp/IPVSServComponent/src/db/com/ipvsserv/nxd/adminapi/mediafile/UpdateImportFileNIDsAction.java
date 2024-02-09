package com.ipvsserv.nxd.adminapi.mediafile;

import java.io.File;
import java.util.Hashtable;
import java.util.List;
import java.util.Map;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.IpvsDBUtils;
import com.ipvsserv.nxd.adminapi.db.DBQueryAction;
import com.ipvsserv.nxd.xslt.XsltTransformUtils;

public class UpdateImportFileNIDsAction extends DBQueryAction {
    String ipvscomphome = "";
    private static final String MEDIASTREAMFILERESOURCE_OBJECT = "MediaStreamFileResource";

    //private static final String BOOKMARK_OBJECT = "Bookmark";

    @SuppressWarnings("unchecked")
    @Override
    //public static final String EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE = "ExportImportFileListItem";
    public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        boolean is_group_clip_already_in_db = false;
        String mediaGroupID_new = "";
        String nid_mediaGroupfile_new = "";
        String nid_mediaGroupfile_old = "";
        String bookmark_mediaStreamFileResourceNID = "";
        Element queryEL = Dom4jUtils.element(exportDB.getRequestEL(), "ExportImportFileListItem");
        Element internalEL = Dom4jUtils.element(queryEL, "Internal");
        Element externalEL = Dom4jUtils.element(queryEL, "External");

        String parentMediaDirNID = Dom4jUtils.value(Dom4jUtils.element(exportDB.getRequestEL(), "params"), "parentMediaDirNID", "new_temp_dir_nid");
        String UUID = Dom4jUtils.value(Dom4jUtils.element(exportDB.getRequestEL(), "params"), "UUID", "");
        String readquery_id = Dom4jUtils.value(Dom4jUtils.element(exportDB.getRequestEL(), "params"), "readquery_id", "readquery_groupid_mapping");
        String mediaStreamFileResourceNID = Dom4jUtils.value(Dom4jUtils.element(exportDB.getRequestEL(), "params"), "mediaStreamFileResourceNID", "");
        String dateModified = Dom4jUtils.getAttributeValue(internalEL, "@dateModified", System.currentTimeMillis() + "");
        long mediaStoreFile_Rev = CompUtils.longValue(Dom4jUtils.getAttributeValue(internalEL, "@mediaStoreFile_Rev", "1"), 1);
        //mediaStoreFile_Rev++;
        String owner = Dom4jUtils.getAttributeValue(externalEL, "@JID", "");
        //////////// get collection name //////////////
        File fileIndex = new File(exportDB.getDBIndexFilePath());
        File directory = exportDB.getDBImportFolder();

        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: Update Import File NIDs  from " + directory.getAbsolutePath());
        }
        Document docIndex = Dom4jUtils.getDocument(fileIndex);
        Element elClip = getMediaStreamFileResourceElement(docIndex, "MediaClip");
        Element elGroup = getMediaStreamFileResourceElement(docIndex, "MediaGroup");
        if (elClip == null) {
            throw new Exception("DB_ADMIN:Invalid index file in import db tar.");
        }

        //read prev clipfile nid    
        String prev_mediaStreamFileResourceNID = Dom4jUtils.getAttributeValue(elClip, "nid", "");
        //check if this is groupfile 
        Dom4jUtils.addAttribute(internalEL, "prev_fileNID", prev_mediaStreamFileResourceNID);
        Dom4jUtils.addAttribute(internalEL, "prev_UUID", Dom4jUtils.getValueXpath(elClip, "//UUID", ""));
        Dom4jUtils.addAttribute(internalEL, "title", Dom4jUtils.getValueXpath(elClip, "//Title", ""));
        Dom4jUtils.addAttribute(internalEL, "dateCreated", Dom4jUtils.getValueXpath(elClip, "//DateCreated", ""));
        String prev_mediaGroupID = Dom4jUtils.value(elClip, "MediaGroupID", "");
        String dateCreated_queue = Dom4jUtils.getAttributeValue(queryEL, "dateCreated", "");
        //if its group clip 
        if (!prev_mediaGroupID.equals("")) {
            //read prev groupfile nid
            if (elGroup != null) {
                //if already not created than create new - MediaGrroupID and group file MediaStreamFileResourceNID 
                nid_mediaGroupfile_old = Dom4jUtils.getAttributeValue(elGroup, "nid", "");
            }
            //check if groupfile already added 
            //if the groupdID <DBResp><MediaStreamFileResource>{$b/@NID}<Info><MediaStoreFileInfo>{$b//MediaGroupID}</MediaStoreFileInfo></Info></MediaStreamFileResource></DBResp>
            //String readquery_groupid_mapping = readQueryList.get(readquery_id);
            params.put("mediaGroupID", prev_mediaGroupID);
            params.put("dateCreated", dateCreated_queue);
            params.put("dateModified", dateModified);
            params.put("mediaStoreFile_Rev", mediaStoreFile_Rev + "");
            params.put("owner", owner);

            String readquery_groupid_mapping = getReadQuery(readquery_id, params);
            if (readquery_groupid_mapping != null) {
                //Document dbGroupFileDoc = Dom4jUtils.stringToDocument("<a>"+readquery_groupid_mapping+"</a>");
                Document dbGroupFileDoc = Dom4jUtils.stringToDocument(readquery_groupid_mapping);
                if (dbGroupFileDoc != null) {
                    Element mediaStreamFileResourceEL = Dom4jUtils.getElementXpath(dbGroupFileDoc.getRootElement(), "//MediaStreamFileResource");
                    if (mediaStreamFileResourceEL != null) {
                        nid_mediaGroupfile_new = Dom4jUtils.getAttributeValue(mediaStreamFileResourceEL, "NID", "");
                        mediaGroupID_new = Dom4jUtils.getValueXpath(mediaStreamFileResourceEL, "//MediaGroupID", "");
                        MRLog.debug("DB_ADMIN:**************************** Already exists  in DB nid_mediaGroupfile_new='" + mediaGroupID_new + "'" + " prev_mediaGroupID='" + prev_mediaGroupID + "' nid_mediaGroupfile_new:" + nid_mediaGroupfile_new);
                    }
                }
            }
            //create a new one if not found in db
            if (mediaGroupID_new.equals("")) {
                mediaGroupID_new = IpvsDBUtils.getUUID();
            } else {
                //if already created than use from db - MediaGrroupID and group file MediaStreamFileResourceNID 
                is_group_clip_already_in_db = true;
            }
            if (nid_mediaGroupfile_new.equals("")) {
                NIDSchemeMDC node = new NIDSchemeMDC();
                node.parse(mediaStreamFileResourceNID);
                nid_mediaGroupfile_new = IpvsDBUtils.getAutoNID(node.getCollectionNodeID());
            }
            Dom4jUtils.addAttribute(internalEL, "prev_mediaGroupID", prev_mediaGroupID);
            Dom4jUtils.addAttribute(internalEL, "clipType", "group_clip");
            //book parent is goupfile
            bookmark_mediaStreamFileResourceNID = nid_mediaGroupfile_new;
        } else {
            Dom4jUtils.addAttribute(internalEL, "prev_mediaGroupID", "");
            Dom4jUtils.addAttribute(internalEL, "clipType", "");
            //book parent is clipfile
            bookmark_mediaStreamFileResourceNID = mediaStreamFileResourceNID;
        }

        Dom4jUtils.addAttribute(internalEL, "new_mediaGroupID", mediaGroupID_new);

        //apply xsl: create paramters list
        Hashtable<String, String> parameters = new Hashtable<String, String>();
        //add to xsl paramaters list    
        parameters.put("parentMediaDirNID", parentMediaDirNID);
        parameters.put("UUID", UUID);
        parameters.put("mediaStreamFileResourceNID", bookmark_mediaStreamFileResourceNID);
        parameters.put("mediaGroupID", mediaGroupID_new);
        parameters.put("dateModified", dateModified);
        parameters.put("mediaStoreFile_Rev", mediaStoreFile_Rev + "");
        parameters.put("owner", owner);

        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN: mediaGroupID_old after " + mediaGroupID_new);
            MRLog.debug("DB_ADMIN: mediaGroupID_new " + mediaGroupID_new);
            MRLog.debug("DB_ADMIN: group_mediaStreamFileResourceNID_old " + nid_mediaGroupfile_old);
            MRLog.debug("DB_ADMIN: group_mediaStreamFileResourceNID_cur " + bookmark_mediaStreamFileResourceNID);
            MRLog.debug("DB_ADMIN: prev_mediaStreamFileResourceNID : " + prev_mediaStreamFileResourceNID);
            MRLog.debug("DB_ADMIN: mediaStreamFileResourceNID : " + mediaStreamFileResourceNID);
        }
        File[] list = directory.listFiles();
        for (File file : list) {
            String nid_new = "";

            Document doc = null;
            Element el = null;
            try {
                doc = Dom4jUtils.getDocument(file);
                el = doc.getRootElement();
            } catch (Exception ex) {
            }
            if (el == null) {
                continue;
            }
            String nid_old = Dom4jUtils.getAttributeValue(el, "NID", "").trim();
            String object_type = el.getName();
            NIDSchemeMDC node_old = new NIDSchemeMDC();
            node_old.parse(nid_old);
            //File file = new File(directory, node_old.getDocUri());
            String parentNID = node_old.getCollectionNodeID();
            if (is_group_clip_already_in_db) {
                if (nid_old.equals(prev_mediaStreamFileResourceNID)) {//argument clipfile nid
                    nid_new = mediaStreamFileResourceNID;
                } else {
                    file.delete();
                    continue;
                }
            } else {
                if (nid_old.equals(prev_mediaStreamFileResourceNID)) {//argument clipfile nid
                    nid_new = mediaStreamFileResourceNID;
                } else if (nid_old.equals(nid_mediaGroupfile_old)) { //return groupfile nid
                    nid_new = nid_mediaGroupfile_new;
                } else if (!object_type.equals(MEDIASTREAMFILERESOURCE_OBJECT)) {//BOOKMARK_OBJECT)){ //safety code if database is corrupted              
                    nid_new = IpvsDBUtils.getAutoNID(parentNID);
                } else {
                    MRLog.debug("DB_ADMIN: UNKOWN object nid_old " + nid_old);
                }
            }
            if (MRLog.isDbadminLogEnable()) {
                MRLog.debug("DB_ADMIN: IMPORT XSL transformation nid_old " + nid_old + " nid_new " + nid_new);
            }
            //get new doc uri
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.parse(nid_new);

            //apply xsl and save new file  
            File resultFile = new File(directory, node.getDocUri());
            ipvscomphome = IpvsServGlobals.getHomeDirectory();
            String xsl = ipvscomphome + "/conf/xmls/mediafile/apply_import_modifications.xsl";
            //apply xsl
            parameters.put("NID", nid_new);
            XsltTransformUtils.transform_Xalan(xsl, file, resultFile, parameters);

            //delete old file 
            file.delete();
        }
        return null;
    }

    @SuppressWarnings("unchecked")
    private Element getMediaStreamFileResourceElement(Document docIndex, String type) {
        List<Element> list = (List<Element>) Dom4jUtils.listXpath(docIndex, "//record");
        for (Element el : list) {
            String objectType = Dom4jUtils.getAttributeValue(el, "name", "");
            String clipType = Dom4jUtils.value(el, "Type", ""); //MediaGroup
            if (objectType.equals(MEDIASTREAMFILERESOURCE_OBJECT) && type.equals(clipType)) {
                return el;
            }
        }
        return null;
    }

}
