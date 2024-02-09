package com.ipvs.assetadminservice.handler;

import java.util.HashMap;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class AddImportFileRequestHandler implements MRRequestHandlerI {
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) throws Exception {
        log.addTraceBegin(request);

        int authType = MediaRoomObject.ROOM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;
        MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, authType, role, roomPolicyNID, streamPolicyNID, token, authority);
        log.addTraceEnd(request);

        return auth;
    }

    @SuppressWarnings("unchecked")
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        //will check if delete clean up is needed 
       // FileSMQueue.getInstance().cleanupDeleted(null, request, xmpp, log, db, resourceFactory, serviceAgentFactory, null);

        //get request document
        Document requestDoc = request.getMessage().getDocument();
        //create response document
        Element respRoot = AssetAdminUtils.getResponseDoc(this.getClass());
        //get element from request
        Element reqEL = MRXMLUtils.getElementXpath(requestDoc.getRootElement(), "//" + respRoot.getName());
        if (reqEL == null) {
            //todo:MRException
            throw new Exception("Invalid request " + respRoot.getName());
        }
        
        
        PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.IMPORT_ROLE_ACTION,AMConst.MEDIA_STREAM_DIR_RESOURCE, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        
        //get request params 
        String type = FileSM.TYPE_IMPORT;
        String ownerJID = request.getUserJID();
        int priority = Utils.getIntValue(MRXMLUtils.getAttributeValue(reqEL, "priority", "1").trim(), 1);
        String extJID = MRXMLUtils.getAttributeValue(reqEL, "JID", "").trim();
        String extFolderPath = MRXMLUtils.getAttributeValue(reqEL, "folderPath", "").trim();
        if(extFolderPath.equals("")) {
        	throw new MRException(AMExceptionCodes.MEDIA_IMPORT_EMPTY_SRC_FOLDER_PATH_EXCEPTION, "Cannot Import. Media Source folder path is empty.");
        }
        String date = MRXMLUtils.getAttributeValue(reqEL, "dateCreated", "").trim();
        String dirNID = MRXMLUtils.getAttributeValue(reqEL, "dirNID", "").trim();
        boolean isMedia2dot4 = Utils.booleanValue(MRXMLUtils.getAttributeValue(reqEL, "isMedia2dot4", "").trim(), false);
        String blobFolder = MRXMLUtils.getAttributeValue(reqEL, "blobFolder", "").trim();
        //String date = System.currentTimeMillis()+"";//(new Date()).toString();//todo: fix date format.. needed for search
        if (date.equals("")) {
            date = System.currentTimeMillis() + "";
        }
        if (extJID.equals("")) {
            extJID = ownerJID;
        }
        
        Element elDir = FileSMUtils.readMediaStreamDir_db(dirNID, request, db, log);
        String dirTitle = MRXMLUtils.getValueXpath(elDir, "//Info/Title", "");
        String dirJID = MediaStoreServiceUtils.getMediaStoreOwnerJID(elDir.getDocument(), request, db, log);
        
        /*
        String whereXpath = "[//" + AMConst.MEDIA_STREAM_FILE_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/"
        +MediaStoreServiceConst.PROPERTIES_EL_NAME +
        "/" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME + "='" + dirNID + "']";
        
        String mediaStoreFile_Rev = GetMediaStreamFileMaxRevisionRequestHandler.getMaxRevisionNumber(whereXpath, request.getUserJID(), log, db);
        if(mediaStoreFile_Rev==null ||mediaStoreFile_Rev.equals("")){
            mediaStoreFile_Rev="1";
        }
         */
        String mediaStoreFile_Rev ="1";
        //get Object MRConfig
        String objectName = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectName, log);
        String perentListName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] arrWhereXpath = null;

        //get Object xml
        Document doc = mrConfig.getXmlRepresentation(log).getDoc();
        Element objectEL = doc.getRootElement().createCopy();
        MRXMLUtils.addAttribute(objectEL, "type", type);
        MRXMLUtils.addAttribute(objectEL, "ownerJID", ownerJID);
        MRXMLUtils.addAttribute(objectEL, "priority", priority + "");
        MRXMLUtils.addAttribute(objectEL, "dateCreated", date);
        MRXMLUtils.addAttribute(objectEL, "state_description", FileSMUtils.getActionDescription(FileSM.ACTION_QUEUE, log));
        MRXMLUtils.addAttribute(objectEL, "state", FileSM.STATE_QUEUED);
        MRXMLUtils.addAttribute(objectEL, "action", FileSM.ACTION_QUEUE);
        MRXMLUtils.addAttribute(objectEL, "queue_state", FileSMQueue.QUEUE_STATUS_NOT_STARTED + "");
        //hashmap to maintain mapping of old groupId to newly created groupIDs
        HashMap <String, String> groupsIDs = new HashMap<String, String>();
        //add one record per '//File'
        List<Element> list = (List<Element>) MRXMLUtils.getListXpath(reqEL, "//File");
        for (Element fileEL : list) {
            Element exportImportFileListItemEL = objectEL.createCopy();
            Element internalEL = MRXMLUtils.getChild(exportImportFileListItemEL, "Internal");
            Element externalEL = MRXMLUtils.getChild(exportImportFileListItemEL, "External");
            MRXMLUtils.addAttribute(exportImportFileListItemEL, "dateCompleted", "");
            long blobSize = Utils.getLongValue( MRXMLUtils.getAttributeValue(fileEL, "blobSize", "0"), 0);
            //debug: force huze blob size 
            //blobSize = Utils.getLongValue( MRXMLUtils.getAttributeValue(fileEL, "blobSize", "69910482944"), 0);
            MRXMLUtils.setAttributeValue(exportImportFileListItemEL, "blobSize", blobSize+"");
            
            String NID = MRXMLUtils.getAttributeValue(fileEL, "fileNID", "");
            String oldGroupID = MRXMLUtils.getAttributeValue(fileEL, "groupID", "");
            String title = MRXMLUtils.getAttributeValue(fileEL, "title", "");
            String dateCreated = MRXMLUtils.getAttributeValue(fileEL, "dateCreated", "");
            String fileName = MRXMLUtils.getAttributeValue(fileEL, "fileName", "");
            String groupID = "";
            if(oldGroupID != ""){
        	if(groupsIDs.containsKey(oldGroupID)){        	    
        	    groupID= groupsIDs.get(oldGroupID);        	    
        	}
        	else{
        	    groupID= Utils.getUUID(); // create a new groupid and add to mapping
        	    groupsIDs.put(oldGroupID, groupID);        	    
        	}
        	    
            }
            
            //boolean isMedia2dot4 = FileSMUtils.isMedia2dot4(fileName);
            //create new NID 
            MRXMLUtils.addAttribute(internalEL, "dateCreated", dateCreated);

            //dir nid as argument
            MRXMLUtils.addAttribute(internalEL, "dirNID", dirNID);

            //only for import 
            String queueID = FileSMUtils.getFileNID(db, log);
            log.addInfo("AddImportFileRequestHandler: queueID= "+queueID);
            MRXMLUtils.addAttribute(internalEL, "queueID", queueID);
            MRXMLUtils.addAttribute(internalEL, "fileNID", queueID);
            MRXMLUtils.addAttribute(internalEL, "UUID", Utils.getUUID());

            //internal
            //only in case of import, after reading index file 
            MRXMLUtils.addAttribute(internalEL, "title", title);            
            MRXMLUtils.addAttribute(internalEL, "groupID", groupID); 
            MRXMLUtils.addAttribute(internalEL, "clipType", "");
            MRXMLUtils.addAttribute(internalEL, "prev_fileNID", NID);
            MRXMLUtils.addAttribute(internalEL, "prev_UUID", "");
            MRXMLUtils.addAttribute(internalEL, "prev_mediaGroupID", ""); // need to send it to applet
            MRXMLUtils.addAttribute(internalEL, "dirTitle", dirTitle);
            MRXMLUtils.addAttribute(internalEL, "JID", dirJID);
            MRXMLUtils.addAttribute(internalEL, "dateModified", String.valueOf(xmpp.getCurrentTimeMillis()));
            MRXMLUtils.addAttribute(internalEL, "mediaStoreFile_Rev", mediaStoreFile_Rev); //not needed
          
            //external
            MRXMLUtils.addAttribute(externalEL, "JID", extJID);
            MRXMLUtils.addAttribute(externalEL, "folderPath", extFolderPath);
            MRXMLUtils.addAttribute(externalEL, "fileName", fileName);
            MRXMLUtils.addAttribute(externalEL, "blobFolder", blobFolder);
           // MRXMLUtils.addAttribute(externalEL, "blobName", blobName);
            MRXMLUtils.setAttributeValue(internalEL, "isMedia2dot4", isMedia2dot4+"");
                        
            //add one record to db
            MRXMLDBUtils.add(MRXMLDBUtils.TIME_OUT, request, log, db, perentListName, objectName, perentNID, childXPath, arrWhereXpath, exportImportFileListItemEL);
            //append to the response
            MRXMLUtils.addElement(respRoot, exportImportFileListItemEL);            
        }

        //try executing next in queue---if nothing is running new one will be started
        FileSMQueue.getInstance().processQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null);
        MRXMLUtils.addElement(respRoot, "Suspended", FileSMQueue.getInstance().isSuspendGlobal() + "");
        MRXMLUtils.addElement(respRoot, "Running", FileSMQueue.getInstance().isRunning() + "");
        return respRoot.getDocument();
    }

}
