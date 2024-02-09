package com.ipvs.assetadminservice.impl;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.common.db.DBExceptionCodes;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;


/**
 * Handle the Media Management Operations.
 * Like deleting the media stream file and emptying the recycle bin.
 * @author padmaja
 *
 */
public class MediaManagementUtils {
    /**
     * Check the media stream file.
     * If the file type is Media Group move the list of clips under media Group to Recycle Bin first then move group.
     * If the Media Clip is PVR Clip then send request to media store delete from media store.
     * If the file type is media clip then move the media clip to recycle bin and update the media group play length and size.
     * @param mediaStreamFileDoc
     * @param mediaStreamFileNID
     * @param resDoc
     * @param dbObjectName
     * @param pemXPath
     * @param request
     * @param dbHelper
     * @param schema
     * @param log
     * @param xmpp
     * @param resourceFactory
     * @param serviceAgentFactory
     * @return
     * @throws MRException
     * @throws Exception
     */
    private static Document handleFileDeleteOrRestore(Document mediaStreamFileDoc, String mediaStreamFileNID, String action, Document resDoc, String dbObjectName,
        String pemXPath, MRRequest request, MRDBHelper dbHelper, MRDB schema, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
       
    	String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

        String parentDirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
        
        String newParentDirNID = null;
        if (!mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {

        	//if the action is restore then move the files to regular dir. If the action is delete move the files to recycle bin dir.
        	if(action.equals(AMConst.MEDIA_RESTORE_REQUEST)){
        		Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(parentDirNID, request, schema, log);
        		newParentDirNID = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
        		if ((newParentDirNID == null) || newParentDirNID.isEmpty()) {
            		throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NOT_FOUND, "Parent Media Dir Not Found");
            	} else if(newParentDirNID.equals(MediaStoreServiceConst.DEFAULT_DIR)){
            		newParentDirNID = MRXMLUtils.getAttributeValue(mediaStreamDirDoc.getRootElement(), MediaStoreServiceConst.NID, "");                	
            	}
        	} else {
        		newParentDirNID = MediaStoreServiceUtils.getRecycleBinNID(parentDirNID, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
        	}         	

        }

        if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
            String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
            List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, schema);

            if ((listOfFiles != null) && !listOfFiles.isEmpty()) {
                Element fileEL = null;
                String fileNID = null;
                Document response = null;
                Document responseDoc_recursive = null;
                MRDBHelper dbHelper_recursive = null;
                String mediaClipParentDirNID = null;

                for (Object object : listOfFiles) {
                    fileEL = (Element) object;
                    fileNID = MRXMLUtils.getAttributeValue(fileEL, MediaStoreServiceConst.NID, "");
                    mediaClipParentDirNID = MRXMLUtils.getValueXpath(fileEL, ".//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
                    if(action.equals(AMConst.MEDIA_DELETE_REQUEST)) {
	                    boolean isClipDeleted = MediaStoreServiceUtils.checkMediaStreamFileDeleted(mediaClipParentDirNID, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
	                    if (isClipDeleted){
	                    	continue;
	                    }
                    }
                    dbHelper_recursive = new MRDBHelper();
                    responseDoc_recursive = (Document) resDoc.clone();
                    dbHelper_recursive.doInit(request.getServiceName(), dbObjectName, responseDoc_recursive, request, xmpp, log, schema,
                        resourceFactory, serviceAgentFactory);

                    response = handleFileDeleteOrRestore(MRXMLUtils.elementToDocument(fileEL), fileNID, action,responseDoc_recursive, dbObjectName, pemXPath,
                            request, dbHelper_recursive, schema, log, xmpp, resourceFactory, serviceAgentFactory);

                //for PVR clips send delete request to media store.
                    if (response != null) {
                        MediaStoreServiceUtils.sendRequestToMediaStoreAgent(AMConst.ASSET_DELETE_REQUEST, MRXMLUtils.elementToDocument(fileEL),
                            request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
                    }
                }
            }
            newParentDirNID = MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME;
            if(action.equals(AMConst.MEDIA_RESTORE_REQUEST)){
            	newParentDirNID = MediaStoreServiceConst.DEFAULT_DIR;
            }
        }

        String state = MediaStoreServiceConst.DELETED;
        if(action.equals(AMConst.MEDIA_RESTORE_REQUEST)){
        	state = MediaStoreServiceConst.READY;
        } 
     
    	MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//ParentMediaDirNID", newParentDirNID);
 
    	//doing partial updates to eliminate race conditions        
    	AssetAdminServiceUtils.updateChild(mediaStreamFileNID, "//ParentMediaDirNID", MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//ParentMediaDirNID"), request, log, schema, xmpp, resourceFactory, serviceAgentFactory);
   
        if(mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)){
           MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+AMConst.STATEFLAG, state);
    	   AssetAdminServiceUtils.updateChild(mediaStreamFileNID, "//StateFlag", MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//StateFlag"), request, log, schema, xmpp, resourceFactory, serviceAgentFactory);        	     
    	}
         
     
        //send store request to update media store file state to deleted and update the group length and size
        if (mediaType.equals(MediaRoomServiceConst.MEDIACLIP) || mediaType.equals(MediaRoomServiceConst.MEDIAIMAGE)) {
        	//send media store request to update media store file status
        	MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+AMConst.STATEFLAG, state);
        	 Document updateRequestDoc = AssetAdminServiceUtils.createAssetAdminServiceRequest("MediaStreamFileResource_BASE_ALL_UPDATE_NID.xml",
                     mediaStreamFileNID, (Element) mediaStreamFileDoc.getRootElement().clone(), log);
            MRRequest userRequest = MRRequest.createRequest(request.getAppServerJID(),updateRequestDoc);        
        	MediaStoreServiceUtils.sendRequestToMediaStoreAgent(AMConst.ASSET_REPLACE_REQUEST, mediaStreamFileDoc, userRequest, xmpp, log, schema, resourceFactory, serviceAgentFactory);        	
        	/*MediaStoreServiceUtils.updateMediaGroupPlayLenghtAndSizeOnDelete(mediaStreamFileDoc, request, xmpp, log, schema, resourceFactory,
        			serviceAgentFactory);*/
        } 
        
        //log audit event for media deletion
        if (mediaType.equals(MediaRoomServiceConst.MEDIACLIP) || mediaType.equals(MediaRoomServiceConst.MEDIAIMAGE) || mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
        	request.addAuditEvent(AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(log, request.getUserJID(), mediaStreamFileDoc, mediaStreamFileDoc, request.getOperatorGroups(), 
        			AuditEventLogger.STATUS_SUCCESS, AuditEventLogger.LEVEL_INFO, "", dbObjectName, AMConst.DELETE_REQUEST));
        	request.setLogEvent(false);
        }

        //Change the update response name to delete response 
        Element replaceResponse = MRXMLUtils.getElementXpath(resDoc, "//ReplaceResponse");
        
        if (replaceResponse != null && action.equals(AMConst.MEDIA_DELETE_REQUEST)) {
            replaceResponse.setName("DeleteResponse");
        } else  if (replaceResponse != null && action.equals(AMConst.MEDIA_RESTORE_REQUEST)) {
            replaceResponse.setName("RestoreResponse");
        }

        return null;
    }
    
    public static Document handleDeleteMediaStreamFile(MRRequest request, XMPPI xmpp,
    		MRLog log, MRDB schema, MRResourceFactoryI resourceFactory,
    		ServiceAgentFactoryI serviceAgentFactory, String serviceName,
    		MRDBHelper amUtil, Element actionEL, Element resourceGroups,
    		Element userGroups, String pemXPath, String dbObjectName,
    		String action, Document resDoc, Document reqDoc) throws MRException, Exception {

    	    String mediaStreamFileNID = "";
    	    boolean moveToRecycleBin = true;
    	    
    	    if(action.equals("DeleteFileRequest")) {
    	    	Element requestEL = MRXMLUtils.getElementXpath(reqDoc, "//" + MRRequest.getRequestRootName(action));
        	    mediaStreamFileNID = MRXMLUtils.getAttributeValue(requestEL, SessionServiceResourceConst.BY_NID_ATTRIBUTE, "");
        	    moveToRecycleBin = Boolean.valueOf(MRXMLUtils.getAttributeValue(requestEL, "moveToRecycleBin", "true"));
    	    }
    	    else {
    	    	mediaStreamFileNID = MRXMLUtils.getValueXpath(reqDoc, "//NID", "");
    	    }
    	    
    	    
    	    if (mediaStreamFileNID.isEmpty()) {
    	    	throw new MRException(DBExceptionCodes.NODE_ID_NOT_FOUND, "NID not Found");
    	    }
    	    
    	    Document mediaStreamFileDoc = MRXMLDBUtils.readElement(MRRequest.createRequest(request.getUserJID()), log, schema, mediaStreamFileNID);

    	    if (mediaStreamFileDoc == null) {
    	    	throw new MRException(MRExceptionCodes.NODENOTFOUND, MRXMLUtils.generateErrorXML("nid", mediaStreamFileNID));
    	    }

    	    String mediaState = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + AMConst.STATEFLAG, "");

    	    if (mediaState.equals(MediaStoreServiceConst.BUSY) && MediaManagementUtils.isFileInRoom(mediaStreamFileNID, request, xmpp, log, schema)) {
    	    	throw new MRException(AMExceptionCodes.MEDIA_FILE_BUSY, "Can not delete MediaStreamFile. File is in Use ");
    	    }
    	    String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
    	  
		String mediaStreamFileGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "").trim();
		String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
		MRConfig mrConfig = MRConfig.getInstance(objectType, log);
		String listName = mrConfig.getParentNIDKey(log);
		String perentNID = schema.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
		String childXPath = null;
		String arrWhereXpathStr = "";
		Document docRead = MRXMLUtils.getDocument();
		MRXMLUtils.addElement(docRead, "Response");
		if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
			arrWhereXpathStr = "//Internal/@prev_mediaGroupID='" + mediaStreamFileGroupID + "'";
		} else {
			arrWhereXpathStr = "//Internal/@fileNID='" + mediaStreamFileNID + "'";
		}
		String[] arrWhereXpathRead = { "@type='" + FileSM.TYPE_EXPORT + "'", "@queue_state='" + FileSMQueue.QUEUE_STATUS_RUNNING + "'", arrWhereXpathStr };
		Element dbRead = MRXMLUtils.addElement(docRead.getRootElement(), "ResponseXML");
		Element ExportImportFileListItems = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, schema, listName, objectType, perentNID, childXPath, arrWhereXpathRead,
				null, null, null, dbRead, 0, -1);
		String update_state_xpath = "//" + AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
		@SuppressWarnings("unchecked")
		List<?> list = (List<Element>) MRXMLUtils.getListXpath(ExportImportFileListItems, update_state_xpath);
		if (list != null && list.size() > 0) {
			throw new MRException(AMExceptionCodes.MEDIA_FILE_BUSY, "Can not delete MediaStreamFile. File is in Use ");
		}

    	    
           if (mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)){
    	    	String requestName = getMediaStreamFileRequestName(request, AMConst.ASSET_DELETE_REQUEST);
    	    	MediaStoreServiceUtils.sendRequestToMediaStoreAgent(requestName, mediaStreamFileDoc, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
    	    	return null;	
    	    } else {
    	    	MediaManagementUtils.handleFileDeleteOrRestore(mediaStreamFileDoc, mediaStreamFileNID, AMConst.MEDIA_DELETE_REQUEST, resDoc, dbObjectName, pemXPath, moveToRecycleBin, request, amUtil, schema, log, xmpp,
    	    			resourceFactory, serviceAgentFactory);
    	    	resDoc.getRootElement().setName(request.getResponseDataName());
    	    	return resDoc;
    	    } 
    	}
    
    public static Document handleFileDeleteOrRestore(Document mediaStreamFileDoc, String mediaStreamFileNID, String action, Document resDoc, String dbObjectName,
            String pemXPath, Boolean moveToRecycleBin, MRRequest request, MRDBHelper dbHelper, MRDB schema, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
    	String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
		Document responseDoc = null;
    	if(moveToRecycleBin) {
    		handleFileDeleteOrRestore(mediaStreamFileDoc, mediaStreamFileNID, action, resDoc, dbObjectName, pemXPath, request, dbHelper, schema, log, xmpp,
    			resourceFactory, serviceAgentFactory);
    	}
    	else {
    		if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
                String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
                List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, schema);

                if ((listOfFiles != null) && !listOfFiles.isEmpty()) {
                    for (Object object : listOfFiles) {
                        Element fileEL = (Element) object;
                        MediaStoreServiceUtils.sendRequestToMediaStoreAgent(AMConst.ASSET_DELETE_REQUEST, MRXMLUtils.elementToDocument(fileEL),
                                request, xmpp, log, schema, resourceFactory, serviceAgentFactory);                    
                    }
                }
    		}
            else {
              	MediaStoreServiceUtils.sendRequestToMediaStoreAgent(AMConst.ASSET_DELETE_REQUEST, mediaStreamFileDoc,
                           request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
            }
    		request.addAuditEvent(AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(log, request.getUserJID(), mediaStreamFileDoc, mediaStreamFileDoc, request.getOperatorGroups(), 
        			AuditEventLogger.STATUS_SUCCESS, AuditEventLogger.LEVEL_INFO, "", dbObjectName, AMConst.DELETE_REQUEST));
        	request.setLogEvent(false);
    	}
    	return responseDoc;
    }
    
    public static String getMediaStreamFileRequestName(MRRequest request, String requestType) {
        String requestName = request.getRequestName();

        if (requestName.equals(AMConst.ASSET_REQUEST_NAME)) {
            return requestType;
        } else {
            return requestName;
        }
    }
    	
    

    public static boolean isFileInRoom(String mediaStreamFileNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws MRException, Exception {
        String recordXpath = "data/destNID='" + mediaStreamFileNID + "'";
        String streamXpath = "//Stream[ " + recordXpath + " ]";
        String roomNID = null;

        try {
            roomNID = MediaRoomServiceUtils.getRoomNIDWithXPath(request, xmpp, log, db, streamXpath);
        } catch (Exception exp) {
            return false;
        }

        if ((roomNID != null) && !roomNID.isEmpty()) {
            return true;
        }

        return false;
    }

    /**
    * Get the list of recycle bin files and send message to media store agent to delete that first file in the list.
    * before sending check the file state is ok or not and DMS user is up or not.
    * @param request
    * @param db
    * @param log
    * @param xmpp
    * @param resourceFactory
    * @param serviceAgentFactory
    * @return
    * @throws MRException
    * @throws Exception
    */
    public static boolean handleEmptyDirRequest(MRRequest request, MRDB db, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        String dirNID = MRXMLUtils.getValueXpath(request.getMessage().getDocument(), "//MediaStreamDirNID", "");

        if (dirNID.isEmpty()) {
            throw new MRException(AMExceptionCodes.MEDIA_STREAM_DIR_NID_MISSING, "MediaStreamDirNID Missing");
        }
        //XMPPTransaction.setDEBUG(true,Thread.currentThread().getId());
        
        String ownerJID = getMediaStoreContactJID(dirNID, request, db, log, xmpp);
        
        String []whereXpath = {"//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+"[.='"+dirNID+"']", "//StateFlag[.!='"+MediaStoreServiceConst.BUSY+"']"};
        String selectXML = "<MediaStreamFileResource NID='{$VAR/@NID/data(.)}'> " +
        		"{$VAR//MediaStoreFileInfo/Type}" +
        		"{$VAR//UUID}</MediaStreamFileResource>";
        Element responseEL = MRXMLUtils.newElement("ResponseEL");
        responseEL = MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.MEDIA_STREAM_FILE_RESOURCE,
        	null, null, whereXpath, null, 
			null, selectXML, responseEL, 0, 1);
        List<?> mediaStreamFileList = MRXMLUtils.getList(responseEL);        

        if (mediaStreamFileList == null) {
            log.addInfo("No files found in dir.", MRLog.INFO, MRLog.NOTIFY);

            return false;
        }

        Iterator<?> mediaStreamfileListIterator = mediaStreamFileList.iterator();

        Element mediaStreamFileEL = null;
        Document mediaStreamFileDoc = null;
        String mediaStreamFileNID = null;

        while (mediaStreamfileListIterator.hasNext()) {
            try {
                mediaStreamFileEL = (Element) mediaStreamfileListIterator.next();
                mediaStreamFileDoc = MRXMLUtils.elementToDocument(mediaStreamFileEL);
                mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileEL, MediaStoreServiceConst.NID, "");
                String[] arrWhereXpath = {"//Bookmark[MediaStreamFileResourceNID='"+mediaStreamFileNID+"']"};	
        	    MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.BOOKMARK, "", "", arrWhereXpath);

                String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
                if (mediaType.equals(MediaRoomServiceConst.MEDIACLIP) || mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP) || mediaType.equals(MediaRoomServiceConst.MEDIAIMAGE)) {
                    MediaStoreServiceUtils.sendRequestToMediaStoreAgent(ownerJID, 
                	    MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.UUID, ""), 
                	    AMConst.ASSET_DELETE_REQUEST, mediaStreamFileNID, dirNID,null, request, xmpp, log);                   
                    return true;
                } else {
                    MRXMLDBUtils.deleteElement(request, log, db, mediaStreamFileNID);
                    return handleEmptyDirRequest(request, db, log, xmpp, resourceFactory, serviceAgentFactory);
                }
            } catch (MRException exp) {
                exp.printStackTrace();
            } catch (Exception exp) {
                exp.printStackTrace();
            }
        }
        //XMPPTransaction.setDEBUG(false,Thread.currentThread().getId());
        log.addInfo("Can not proceed Further. Either File state is Error or Store User is Offline", MRLog.INFO, MRLog.NOTIFY);

        return false;
    }

    public static List<?> getListOfFilesInDir(String userJID, String dirNID, MRDB db, MRLog log, XMPPI xmpp) throws MRException, Exception {
        String[] whereXpath = { "//MediaStreamFileResource[Info/Properties/ParentMediaDirNID='" + dirNID + "']" };

        MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
        Element responseEL = MRXMLUtils.newElement(AMConst.MEDIA_STREAM_FILE_RESOURCE + "List");
        Element whereAssistXML = MRXMLUtils.newElement("whereAssistXML");
        MRXMLDBUtils.findChildElements(parentNID, responseEL, null, 1, whereAssistXML, whereXpath, MRRequest.createRequest(userJID), db, log);

        List<?> mediaStreamFileList = MRXMLUtils.getList(responseEL, AMConst.MEDIA_STREAM_FILE_RESOURCE);

        if (mediaStreamFileList.isEmpty()) {
            return null;
        }

        return mediaStreamFileList;
    }

/*    private static Document getBookmarkList(String mediaStreamFileNID, MRRequest request, MRDB db, MRLog log, XMPPI xmpp,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        String requestName = MRRequest.getRequestName(GetBookmarkListRequestHandler.class.getName());
        Document requestDoc = MediaRoomServiceUtils.getMediaRoomRequest(log, ServiceClient.MEDIAROOMSERVICE, requestName);
        MRXMLUtils.setValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, mediaStreamFileNID);

        return MediaRoomServiceUtils.getMediaRoomResponse(request, request.getUserJID(), xmpp, log, db, resourceFactory, serviceAgentFactory,
            ServiceClient.MEDIAROOMSERVICE, requestName, requestDoc);
    }*/
    
    public static void deleteBookmarks(List<String> mediaStreamFileNIDList , MRRequest request,MRDB db,MRLog log ) throws MRException,Exception {
    	String[] arrWhereXpath = {"//Bookmark[MediaStreamFileResourceNID=(" + StringUtils.join(mediaStreamFileNIDList, ",")+ ")]"};	
    	MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.BOOKMARK, "", "", arrWhereXpath);
    	log.addInfo("Delete Boookmarks",MRLog.INFO,MRLog.NOTIFY);

    }

    /*private static void handleBookMark(String mediaStreamFileNID, MRRequest request, MRDB db, MRLog log, XMPPI xmpp,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        Document bookMarkListDoc = getBookmarkList(mediaStreamFileNID, request, db, log, xmpp, resourceFactory, serviceAgentFactory);
        List<?> bookMarkList = MRXMLUtils.getListXpath(bookMarkListDoc, "//" + AMConst.BOOKMARK);

        if ((bookMarkList == null) || bookMarkList.isEmpty()) {
            return;
        }

        log.addInfo("Handle Delete BookMark Of MediaStreamFileResource. FileNID:=" + mediaStreamFileNID + "BookMark Count:=" + bookMarkList.size(),
            MRLog.INFO, MRLog.NOTIFY);

        Element bookMarkEL = null;
        String NID = null;
        List<?> mediaStreamFileList = null;
        Element mediaStreamFileResourceNIDEL = null;

        for (Object bookMarkObject : bookMarkList) {
            bookMarkEL = (Element) bookMarkObject;
            NID = MRXMLUtils.getAttributeValue(bookMarkEL, MediaStoreServiceConst.NID, "");

            //check book mark is associate to multiple files or not. If yes then do update else do delete 
            mediaStreamFileList = MRXMLUtils.getList(bookMarkEL, MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID);

            //TBD: Test this code when multiple files are associate to same book mark.
            if (mediaStreamFileList.size() > 1) {
                for (Object fileNIDObject : mediaStreamFileList) {
                    mediaStreamFileResourceNIDEL = (Element) fileNIDObject;

                    if (MRXMLUtils.getValue(mediaStreamFileResourceNIDEL).equals(mediaStreamFileNID)) {
                        mediaStreamFileResourceNIDEL.detach();

                        break;
                    }
                }

                MRXMLDBUtils.updateElement(request, log, db, NID, bookMarkEL);
            } else {
                MRXMLDBUtils.deleteElement(request, log, db, NID);
            }
        }
    }*/

    public static boolean handleCleanOrphanDirRequest(String dirNID, MRRequest request, MRDB db, MRLog log, XMPPI xmpp,
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
    	//XMPPTransaction.setDEBUG(true, Thread.currentThread().getId());
    	XMPPTransaction.startBlock("HandleCleanOrphanDirRequest");
    	String whereXpath = 
    		"//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME +"[.='"+dirNID+"']";
        List<?> mediaStreamFileList = getMediaStreamFileNIDList(whereXpath, request, db, log, xmpp);

    	log.addInfo("List Of Files In dir :=" + dirNID + " Size:=" + mediaStreamFileList.size(), MRLog.INFO, MRLog.NOTIFY);
  	  
        if ((mediaStreamFileList != null) && !mediaStreamFileList.isEmpty()) {
    
            Iterator<?> mediaStreamfileListIterator = mediaStreamFileList.iterator();

            Element mediaStreamFileEL = null;
            
            List<String> mediaStreamFileNIDList  = new ArrayList<String>();
            while (mediaStreamfileListIterator.hasNext()) {
                mediaStreamFileEL = (Element) mediaStreamfileListIterator.next();
                mediaStreamFileNIDList.add("'"+MRXMLUtils.getAttributeValue(mediaStreamFileEL, MediaStoreServiceConst.NID, "")+"'");               
                //handleBookMark(mediaStreamFileNID, request, db, log, xmpp, resourceFactory, serviceAgentFactory);
            }            
            deleteFilesAndBookmarks(mediaStreamFileNIDList, request, db, log);           
        }
    	MRXMLDBUtils.deleteElement(request, log, db, dirNID);
    	log.addInfo("Delete MediaStreamDirResource. DirNID :=" + dirNID, MRLog.INFO, MRLog.NOTIFY);
    	XMPPTransaction.endBlock("HandleCleanOrphanDirRequest");        
    	// XMPPTransaction.setDEBUG(false, Thread.currentThread().getId());
        MediaManagementUtils.handleEmptyMediaGroupForDeletedFiles(request.getUserJID(),xmpp, log, db, resourceFactory, serviceAgentFactory);
        
    	return false;
    }

    private static void deleteFilesAndBookmarks(List<String> mediaStreamFileNIDList, MRRequest request, MRDB db, MRLog log) throws MRException,Exception {
    	int countToFetch = 25;
    	int startIndex = 0;
    	int listSize = mediaStreamFileNIDList.size();
    	LinkedList<String> list = new LinkedList<String>(mediaStreamFileNIDList);
    	while(startIndex < listSize) {
    		int endIndex = ((startIndex + countToFetch) < listSize ? (startIndex + countToFetch) :  listSize);
    		List<String> subList= list.subList(startIndex, endIndex);
    		if(subList.isEmpty()){
    			break;
    		}
    		deleteBookmarks(subList, request, db, log);  
    		deleteFiles(subList, request, db, log);
    		log.addInfo("deleteFilesAndBookmarks. startIndex:="+startIndex+" endIndex:="+endIndex+" countToFetch:="+countToFetch+ " NoOfFiles:="+listSize, MRLog.INFO,MRLog.NOTIFY);
    		startIndex = startIndex + countToFetch;
    	}
//    	//sublist excludes end index element this will delete the last index one
//    	List<String> lastElement = new ArrayList<String>();
//    	lastElement.add(mediaStreamFileNIDList.get(listSize));
//  		deleteBookmarks(lastElement, request, db, log);  
//  		deleteFiles(lastElement, request, db, log);
//  		log.addInfo("Delete last index element lastIndex:="+listSize, MRLog.INFO,MRLog.NOTIFY);
    }
    public static void handleEmptyMediaGroupForDeletedFiles(String userJID, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI profileFactory) throws MRException, Exception {
        handleEmptyMediaGroupForDeletedFiles(MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME, userJID, xmpp, log, db, resourceFactory, profileFactory);
        //Make sure media groups parent dir nid as empty. If they are changing the logic to move NONE then this logic should modify.
        handleEmptyMediaGroupForDeletedFiles("", userJID, xmpp, log, db, resourceFactory, profileFactory);
    }

    public static void handleEmptyMediaGroupForDeletedFiles(String dirNID, String userJID, XMPPI xmpp, MRLog log, MRDB db,
        MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory) throws MRException, Exception {
        List<?> mediaGroupList = getListOfFilesInDir(userJID, dirNID, db, log, xmpp);

        if ((mediaGroupList == null) || mediaGroupList.isEmpty()) {
            return;
        }

        Document mediaStreamFileDoc = null;
        String mediaGroupID = null;
        MRRequest request = MRRequest.createRequest(userJID);
        
        List<String> mediaStreamFileNIDList = new ArrayList<String>();
        
        
        for (Object object : mediaGroupList) {
            mediaStreamFileDoc = MRXMLUtils.elementToDocument((Element) object);
            
            mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");

            if (!mediaGroupID.isEmpty()) {
        	String whereXpath = 
                        "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + 
                        MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "[(" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "='" + 
                        mediaGroupID + "') " + " and (" +
                        MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME + "!='" + MediaRoomServiceConst.MEDIAGROUP + "')]";
        	List<?> mediaStreamFileList = getMediaStreamFileNIDList(whereXpath, request, db, log, xmpp);
                if (!mediaStreamFileList.isEmpty()) {
                    continue;
                }
            }

            mediaStreamFileNIDList.add(
        	    "'"+MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), MediaStoreServiceConst.NID, "")+"'");            

        }
        if(mediaStreamFileNIDList.isEmpty()) {
            return;
        }
        
        deleteFilesAndBookmarks(mediaStreamFileNIDList, request, db, log);       
    }
    
    private static void deleteFiles(List<String> mediaStreamFileNIDList , MRRequest request,MRDB db,MRLog log ) throws MRException,Exception {
        String[] arrWhereXpath = {"//MediaStreamFileResource[@NID=(" + StringUtils.join(mediaStreamFileNIDList, ",")+ ")]"};	
	    MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, "", AMConst.MediaStreamFileResource, "", "", arrWhereXpath);
	    log.addInfo("Delete Media Files");
    }  
    
    private static List<?> getMediaStreamFileNIDList(String whereXpath,MRRequest request,MRDB db,MRLog log,XMPPI xmpp) throws MRException,Exception {
	
	String selectXML = "<MediaStreamFileResource NID='{$VAR/@NID/data(.) }'/>";
	Element responseEL = AssetAdminServiceUtils.getChildBasedOnSelectXML(AMConst.MEDIA_STREAM_FILE_RESOURCE, selectXML,
		whereXpath, request, db, log, xmpp);
	
        return MRXMLUtils.getList(responseEL);
    }
    
    public static String getMediaStoreContactJID(String dirNID, MRRequest request, MRDB db, MRLog log,XMPPI xmpp)
    throws MRException, Exception {
	String selectXML = "<MediaStreamDirResource>{$VAR//State}{$VAR//Title}{$VAR//MediaStorePortResourceNIDList}{$VAR//ParentMediaDirNID}</MediaStreamDirResource>";
	Element  mediaStreamDirEL = AssetAdminServiceUtils.getChildBasedOnSelectXMLNID(dirNID, selectXML, "", request, db, log, xmpp);
	if (mediaStreamDirEL.nodeCount()<0) {
	    throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NOT_FOUND, "MediaDirNotFound");
	}

	String title = MRXMLUtils.getValueXpath(mediaStreamDirEL, ".//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");

	if (MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME.equals(title) || MediaStoreServiceConst.PVR_DIR_NAME.equals(title)) {
	    String parentDirNID = MRXMLUtils.getValueXpath(mediaStreamDirEL, ".//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
	    if(parentDirNID.isEmpty() || parentDirNID.equals("NONE"))
	       throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NOT_FOUND, "MediaDirNotFound");	    
	    mediaStreamDirEL = AssetAdminServiceUtils.getChildBasedOnSelectXMLNID(parentDirNID, selectXML, "", request, db, log, xmpp);
	}

	return MediaStoreServiceUtils.getMediaStoreOwnerJID(MRXMLUtils.elementToDocument(mediaStreamDirEL), request, db, log);
    }
}
