package com.ipvs.assetadminservice.handler;

import java.util.HashMap;
import java.util.List;
import java.util.concurrent.Semaphore;

import org.apache.commons.lang.StringUtils;
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
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.systemservice.impl.FileTransferAgent;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public abstract class ExportOffloadBaseRequestHandler implements MRRequestHandlerI, AgentResponseHandlerI{
	
	
	public static final String EXPORT_MEDIAGROUP_FILES_READ_QUERY ="/com/ipvs/assetadminservice/handler/xquery/ExportMediaGroupFilesQuery.xml";
	public static final String _FILE_XPATH_ = "_FILE_XPATH_";
	
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
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String requestType) throws Exception {
        //will check if delete clean up is needed 
        //FileSMQueue.getInstance().cleanupDeleted(null, request, xmpp, log, db, resourceFactory, serviceAgentFactory, null);

        //get request document
        Document requestDoc = request.getMessage().getDocument();
        //create response document
        Element respRoot = AssetAdminUtils.getResponseDoc(this.getClass());
        //get element from request
        Element reqEL = MRXMLUtils.getElementXpath(requestDoc.getRootElement(), "//" + respRoot.getName());
        if (reqEL == null) {
            throw new Exception("Invalid request " + respRoot.getName());
        }
        
        if(FileSM.TYPE_EXPORT.equals(requestType))
        	PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.EXPORT_ROLE_ACTION,AMConst.MEDIA_STREAM_FILE_RESOURCE, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        
        //get request params 
        String type = FileSM.TYPE_EXPORT.equals(requestType) ? FileSM.TYPE_EXPORT : FileSM.TYPE_OFFLOAD;
        String ownerJID = request.getUserJID();
        int priority = Utils.getIntValue(MRXMLUtils.getAttributeValue(reqEL, "priority", "1").trim(), 1);
        String extJID = MRXMLUtils.getAttributeValue(reqEL, "JID", "").trim();
        String extFolderPath = MRXMLUtils.getAttributeValue(reqEL, "folderPath", "").trim();
        String mediaFormat = MRXMLUtils.getAttributeValue(reqEL, "format", "Proprietary").trim();
        
        List<Element> list = (List<Element>) MRXMLUtils.getListXpath(reqEL, "//File");
        
        List<String> fileIdList = MRXMLUtils.getValuesXpath(reqEL,  "//File/@fileNID");              
		String fileIdXPath = "[.//@NID=('"+ StringUtils.join(fileIdList, "','") +"')]";
		String query = FileUtils.readFileResource(EXPORT_MEDIAGROUP_FILES_READ_QUERY, request.getClass());
		query = query.replaceAll(_FILE_XPATH_, fileIdXPath);		
		String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);		
		if(responseXML!="" && responseXML.length()>0)
			list.addAll((List<Element>) MRXMLUtils.getListXpath(MRXMLUtils.stringToDocument(responseXML),  "//File"));
        
        if(extFolderPath.equals("") && FileSM.TYPE_EXPORT.equals(type)) {
        	throw new MRException(AMExceptionCodes.MEDIA_EXPORT_EMPTY_DEST_FOLDER_PATH_EXCEPTION, "Cannot Export. Media Destination folder path is empty.");
        }
        String date = MRXMLUtils.getAttributeValue(reqEL, "dateCreated", "").trim();
        //String date = System.currentTimeMillis()+"";//(new Date()).toString();//todo: fix date format.. needed for search
        if (date.equals("")) {
            date = System.currentTimeMillis() + "";
        }
        if (extJID.equals("")) {
            extJID = ownerJID;
        }
        
        String nasURL = MRXMLUtils.getAttributeValue(reqEL, "nasURL", "").trim();
        if(nasURL.equals("") && FileSM.TYPE_OFFLOAD.equals(type)) {
        	throw new MRException(AMExceptionCodes.MEDIA_OFFLOAD_EMPTY_NAS_URL_PATH_EXCEPTION, "Cannot Offlaod. Destination nasURL is empty.");
        }
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
        MRXMLUtils.addAttribute(objectEL, "format", mediaFormat);

        //add one record per '//File'
        
        for (Element fileEL : list) {
            Element exportImportFileListItemEL = objectEL.createCopy();
            Element internalEL = MRXMLUtils.getChild(exportImportFileListItemEL, "Internal");
            Element externalEL = MRXMLUtils.getChild(exportImportFileListItemEL, "External");
            MRXMLUtils.addAttribute(exportImportFileListItemEL, "dateCompleted", System.currentTimeMillis() + "");

            String NID = MRXMLUtils.getAttributeValue(fileEL, "fileNID", "");
            String title = MRXMLUtils.getAttributeValue(fileEL, "title", "");
            String dateCreated = MRXMLUtils.getAttributeValue(fileEL, "dateCreated", "");
            String fileName = MRXMLUtils.getAttributeValue(fileEL, "fileName", System.currentTimeMillis() + "");
            
            String startOffset = MRXMLUtils.getAttributeValue(fileEL, "startOffset", "");
            String endOffset = MRXMLUtils.getAttributeValue(fileEL, "endOffset", "");

            MRXMLUtils.addAttribute(internalEL, "fileNID", NID);
            MRXMLUtils.addAttribute(internalEL, "title", title);
            MRXMLUtils.addAttribute(internalEL, "dateCreated", dateCreated);
            MRXMLUtils.addAttribute(internalEL, "startOffset", startOffset);
            MRXMLUtils.addAttribute(internalEL, "endOffset", endOffset);

            MRXMLUtils.addAttribute(externalEL, "JID", extJID);
            MRXMLUtils.addAttribute(externalEL, "folderPath", extFolderPath);
            MRXMLUtils.addAttribute(externalEL, "fileName", fileName);
            MRXMLUtils.addAttribute(externalEL, "nasURL", nasURL);

            // create the new fileNID and UUID for export file
            exportImportFileListItemEL = updateQueueELNIDs(exportImportFileListItemEL, NID, request, db, log, type);
            
            log.addInfo("FileSM:AddExportFileRequestHandler add := " + extJID + "='" + extFolderPath + "/" + fileName + "' " + " title='" + title + "'" + " NID='" + NID + "'", MRLog.INFO, MRLog.NOTIFY);
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
	
	private Element updateQueueELNIDs(Element exportQueueEL, String fileNID, MRRequest request, MRDB db, MRLog log, String requestType) throws Exception{
	    
    	Element internalEL = MRXMLUtils.getChild(exportQueueEL, "Internal");
    	Element externalEL = MRXMLUtils.getChild(exportQueueEL, "External");
    	// read media file
    	Element file_el = FileSMUtils.readMediaStreamFile_db(exportQueueEL, request, db, log);
        if (file_el == null) {
            throw new MRException(AMExceptionCodes.MEDIA_EXPORT_FILE_NOT_FOUND_EXCEPTION, "Can not +" + requestType + ". File not found ('" + fileNID + "') ");
        }
        
        //read directory
        String query = " let $parentDirNID := '_PARENT_DIR_NID_'" +"" +
    			"  	\n	    let $clipType := '_CLIP_TYPE_'" + 
    			"	\n		let $dirDoc :=   doc(concat('/db/ipvs.default.', "+ 
    			"   \n                         substring-before( substring-after( $parentDirNID, 'default.') , '.'  ) , '/', " + 
    			"   \n 							substring-after( substring-after( $parentDirNID, 'default.'),'.')))/*[@NID=$parentDirNID] "+ 
    			"   \n 		let $parentDirDoc := if($clipType = 'PVRMediaClip') then "+
    			"   \n 		let $parentDirNID2 := data($dirDoc//ParentMediaDirNID) "+
    			"	\n		let $dirDoc2 :=   doc(concat('/db/ipvs.default.', "+ 
    			"   \n                         substring-before( substring-after( $parentDirNID2, 'default.') , '.'  ) , '/', " + 
    			"   \n 							substring-after( substring-after( $parentDirNID2, 'default.'),'.')))/*[@NID=$parentDirNID2] "+
    			"   \n      				return $dirDoc2 "+
    			"   \n      else "+
    			"   \n      $dirDoc"+
    			"   \n      return $parentDirDoc";

        query  = query.replace("_PARENT_DIR_NID_", MRXMLUtils.getValueXpath(file_el, "//ParentMediaDirNID", "").trim());
   	 	query  = query.replace("_CLIP_TYPE_", MRXMLUtils.getValueXpath(file_el, "//Info/MediaStoreFileInfo/Type", "").trim());
   	 	String responseXML  = MRXMLDBUtils.readQuery(MRXMLDBUtils.TIME_OUT, request, log, db, query, new HashMap<String, String>(), null);
   	 	if(responseXML ==null || responseXML.isEmpty()) {
   	 		throw new MRException(AMExceptionCodes.MEDIA_EXPORT_PARENT_DIR_NOT_FOUND_EXCEPTION, "Can not " + requestType + ". Directory not found ('" + fileNID + "') ");
   	 	}
   	 	Element elDir = MRXMLUtils.stringToElement(responseXML);
   	 	
        if (elDir == null) {
            throw new MRException(AMExceptionCodes.MEDIA_EXPORT_PARENT_DIR_NOT_FOUND_EXCEPTION, "Can not " + requestType + ". Directory not found ('" + fileNID + "') ");
        }
        
        String cliptType = MRXMLUtils.getValueXpath(file_el, "//MediaStoreFileInfo/Type", "").trim();
        String streamType = MRXMLUtils.getValueXpath(file_el, "//StreamType", "").trim();
        String new_UUID = Utils.getUUID(); 						// Create new UUID for the export/offload file.
        String new_fileNID = FileSMUtils.getFileNID(db, log);   // Create new fileNID for the export/offload file.
        String dirJID = MediaStoreServiceUtils.getMediaStoreOwnerJID(MRXMLUtils.elementToDocument(elDir), request, db, log);
        String dir_title = MRXMLUtils.getAttributeValue(internalEL, "dirTitle", "").trim();
        if (dir_title.equals("")) {
            dir_title = MRXMLUtils.getValueXpath(elDir, ".//Info/Title", "");
            MRXMLUtils.setAttributeValue(internalEL, "dirTitle", dir_title);
        }
        
        MRXMLUtils.setAttributeValue(internalEL, "cliptType", cliptType);
        MRXMLUtils.setAttributeValue(internalEL, "streamType", streamType);
        MRXMLUtils.setAttributeValue(internalEL, "new_fileNID", new_fileNID);
        MRXMLUtils.setAttributeValue(internalEL, "new_UUID", new_UUID);
        MRXMLUtils.setAttributeValue(internalEL, "dirJID", dirJID);
        MRXMLUtils.setAttributeValue(internalEL, "dirNID", MRXMLUtils.getAttributeValue(elDir, "NID", ""));
        
        MRXMLUtils.setAttributeValue(externalEL, "fileName", new_fileNID + FileSMUtils.MEDIA_TAR_EXTENSION);
        
        // Supports only MKDs for V2D media. 
        if(MediaRoomServiceConst.V2D.equals(streamType))
        	MRXMLUtils.setAttributeValue(exportQueueEL, "format", FileSM.MEDIA_FORMAT_PROPRIETARY);
        
        return exportQueueEL;
    }

	
	public void handleAgentResponse(MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String requestType) throws Exception {
        String transition = null;
        Document responseDoc = request.getResult();
        String requestName = MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
        String responseHandlerName = MRXMLUtils.getValueXpath(responseDoc, "//responseHandlerName", "");
        int resp_state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(responseDoc, "//Header","state", ""), -1);
        String error = null;
        int errorCode = -1;
        Element queueEL = null;
        boolean callNext=false;
        Semaphore lock = null;
        String queueID="";
        try {
            if (FileTransferAgent.FILEXFERSETUPREQUEST.equals(requestName) || FileTransferAgent.FILEXFERHTTPUPLOADREQUEST.equals(requestName)) {
        	String queueNID="";
        	String action;
        	if(FileTransferAgent.FILEXFERSETUPREQUEST.equals(requestName)){
                String fileXferID = MRXMLUtils.getValueXpath(responseDoc, "//FileXferID", "").trim()+"__";
                String[] arr = fileXferID.split("__");
                queueNID = arr[0];
                 action= arr[1];
                 queueEL = FileSMUtils.readQueueElement_db(queueNID, request, db, log);
        	}
        	else if(FileTransferAgent.FILEXFERHTTPUPLOADREQUEST.equals(requestName)){
        	    queueID =  MRXMLUtils.getValueXpath(responseDoc, "//QueueID", "");
        	    lock = IPVSMonitor.getMonitor().getLock(queueID, request.getNID());
        	    log.addInfo("ExportOffloadRequestHandler.handleAgentResponse acquire lock on queueID:"+queueID );
        	    String[] arrWhereXpath = { "//Internal/@queueID='" + queueID + "'" };
        	    queueEL = FileSMUtils.readQueueElement_db_whereXpath(request, db, log, arrWhereXpath);
        	}               
                //debug message
                //String fromFilename = MRXMLUtils.getValueXpath(responseDoc, "//fromFileName", "");
                //String fromAgentJID = MRXMLUtils.getValueXpath(responseDoc, "//fromAgentJID", "");
                //String toAgentJID = MRXMLUtils.getValueXpath(responseDoc, "//toAgentJID", "");
                //String toFilename = MRXMLUtils.getValueXpath(responseDoc, "//toFileName", "");
                //log.addInfo("FileSM:handleAgentResponse fileTransfer event: " + " from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + toFilename + " fileXferID=" + fileXferID, MRLog.OK, MRLog.NOTIFY);
                
                
                if (queueEL == null) {
                    FileSMQueue.getInstance().setRunning_nid(queueNID);
                    log.addInfo("FileSM:handleAgentResponse ABORT:requestName :='" + requestName + "' " + " ABORTING queueEL is NULL ", MRLog.INFO, MRLog.NOTIFY);
                    return;
                }
                int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "queue_state", ""), -1);
                //if(queue_state != FileSMQueue.QUEUE_STATUS_RUNNING && queue_state != FileSMQueue.QUEUE_STATUS_ERROR){
                if (queue_state != FileSMQueue.QUEUE_STATUS_RUNNING) {
                    log.addInfo("FileSM:handleAgentResponse ABORT: already done or timed out ?? queue_state " + queue_state + " " + queueEL.asXML(), MRLog.OK, MRLog.NOTIFY);
                    callNext=true;
                    return;
                }
                if (resp_state == MRRequest.STATE_EXCEPTION) {
                    error = MRXMLUtils.getValueXpath(responseDoc, "//Exception/Description", "");
                    if(error.equals("")){
                        action = MRXMLUtils.getAttributeValue(queueEL, "action", "").trim();
                        error="Error during "+requestName +" '"+ FileSMUtils.getActionDescription(action, log)+"'";
                    }
                    errorCode = Utils.getIntValue(MRXMLUtils.getValueXpath(responseDoc, "//Exception/Code", ""), AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION);
                }
            } else {
                queueEL = MRXMLUtils.getElementXpath(responseDoc, "//" + AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
                if (queueEL == null) {
                    log.addInfo("FileSM:handleAgentResponse ABORT:requestName :='" + requestName + "' " + " ABORTING queueEL is NULL ", MRLog.INFO, MRLog.NOTIFY);
                    callNext=true;
                    return;
                }
                String queueNID = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
                Element queueELDB = FileSMUtils.readQueueElement_db(queueNID, request, db, log);
                if (queueELDB == null) {
                    FileSMQueue.getInstance().setRunning_nid(queueNID);
                    log.addInfo("FileSM:handleAgentResponse ABORT:requestName :='" + requestName + "' " + " ABORTING current item is already deleted from db " + queueNID, MRLog.INFO, MRLog.NOTIFY);
                    callNext=true;
                    return;
                }
                int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueELDB, "queue_state", ""), -1);
                //if(queue_state != FileSMQueue.QUEUE_STATUS_RUNNING && queue_state != FileSMQueue.QUEUE_STATUS_ERROR){
                if (queue_state != FileSMQueue.QUEUE_STATUS_RUNNING) {
                    log.addInfo("FileSM:handleAgentResponse ABORT:: already done or timed out ?? queue_state " + queue_state + " " + queueELDB.asXML(), MRLog.OK, MRLog.NOTIFY);
                    callNext=true;
                    return;
                }
                if (resp_state == MRRequest.STATE_EXCEPTION) {
                    error = MRXMLUtils.getValueXpath(responseDoc, "//Exception/Description", "").trim();
                    if(error.equals("")){
                        String action = MRXMLUtils.getAttributeValue(queueEL, "action", "").trim();
                        error="Error during "+requestName +" '"+ FileSMUtils.getActionDescription(action, log)+"'";
                    }
                    errorCode = Utils.getIntValue(MRXMLUtils.getValueXpath(responseDoc, "//Exception/Code", ""), AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION);
                }
            }
            //handle transition
            String state = MRXMLUtils.getAttributeValue(queueEL, "state", "").trim();
            transition = FileSMUtils.getTransitionFromEvent(request, xmpp, log, db, requestType, queueEL);
            log.addInfo("FileSM:handleAgentResponse transition :='" + transition +":State = "+state , MRLog.INFO, MRLog.NOTIFY);
            if (transition != null) {
                if (transition.equals(FileSM.DMSSTATUS_TRANS_OK)) {
                    state = FileSM.STATE_DMSSTATUS;
                }
                FileSM stateMachine = FileSMQueue.getInstance().checkOutFileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler, state, responseHandlerName);
                try {
                    if (error != null) {
                        stateMachine.setErrorCode(errorCode);
                        stateMachine.setError(error);
                    }
                    //call handle transition 
                    stateMachine.handleTransition(transition);
                    String sm_state = stateMachine.getState();
                    callNext = FileSM.STATE_COMPLETED.equals(sm_state) || stateMachine.getError() != null;
                    if(callNext){
                        System.out.println("FileSM:handleAgentResponset: callNext ??" + sm_state + " ERROR='" + ((stateMachine.getError() == null) ? "NONE" : stateMachine.getError()) + "'");
                    }
                } finally {
                    FileSMQueue.getInstance().checkinFileSM(stateMachine, log);
                }
            }
        }finally{
            if (callNext) {
                FileSMQueue.getInstance().processQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler);
            }
            if(lock != null){
        	IPVSMonitor.getMonitor().releaseLock(queueID, lock);
        	log.addInfo("ExportOffloadRequestHandler.handleAgentResponse release lock on queueID:"+queueID );
            }
        }
    }
}
