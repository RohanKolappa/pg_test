package com.ipvs.sessionservice.resource.handler;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.MediaManagementUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.DBExceptionCodes;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.AuditEventUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.policyadminservice.utils.PolicyAdminConstants;
import com.ipvs.policyadminservice.utils.PolicyAdminServiceUtils;
import com.ipvs.sessionservice.handler.base.FileBaseRequestHandler;
import com.ipvs.sessionservice.impl.RequestProcessor;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.Utils;

public class DeleteFileRequestHandler extends FileBaseRequestHandler
implements MRRequestHandlerI
{
	@Override
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)	throws Exception {

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

	@Override
    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
			throws Exception
			{
		log.addInfo("DeleteFileRequestHandler::handleRequest", MRLog.OK, MRLog.NOTIFY);
		Document responseDoc = null;
		
		String pemXPath = PolicyAdminServiceUtils.checkPolicyPermissions(PolicyAdminConstants.DELETE_ROLE_ACTION,AMConst.MEDIA_STREAM_FILE_RESOURCE, request, xmpp, log, schema, resourceFactory, serviceAgentFactory); 
		Boolean moveToRecycleBin = Boolean.valueOf(MRXMLUtils.getAttributeValueXpath(request.getMessage().getDocument(), "//DeleteFile", "moveToRecycleBin", "true"));
		RequestProcessor requestObject = new RequestProcessor(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, AMConst.MEDIA_STREAM_FILE_RESOURCE);
		String byNID = requestObject.getNID();
		
		
		///if not moving to recycle bin get the response before delete
		if(!moveToRecycleBin) {
            //responseDoc = GetObjectUtils.getResponse(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, requestObject,SessionServiceConst.SELECTFORMAT_EXTENDED, SessionServiceConst.GET_FILE_REQUEST_XML, GetFileRequestHandler.class.getName());
			responseDoc = getFileResponse(request, xmpp, log, schema, resourceFactory, serviceAgentFactory,  byNID,"","");
        }

		handleDeleteMediaStreamFile(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, pemXPath, requestObject.getNID(), moveToRecycleBin,request.getMessage().getDocument(),AMConst.MEDIA_DELETE_REQUEST);

		if(moveToRecycleBin) {
            //responseDoc = GetObjectUtils.getResponse(request, xmpp, log, schema, resourceFactory, serviceAgentFactory, requestObject, SessionServiceConst.SELECTFORMAT_EXTENDED, SessionServiceConst.GET_FILE_REQUEST_XML, GetFileRequestHandler.class.getName());
			responseDoc = getFileResponse(request, xmpp, log, schema, resourceFactory, serviceAgentFactory,  byNID,"","");
        }


		return responseDoc;
	}

	public Document handleDeleteMediaStreamFile(MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			String pemXPath, String mediaStreamFileNID, boolean moveToRecycleBin,Document reqDoc,String action)
					throws MRException, Exception	{	


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

		List<?> list =  MRXMLUtils.getListXpath(ExportImportFileListItems, update_state_xpath);
		if (list != null && list.size() > 0) {
			throw new MRException(AMExceptionCodes.MEDIA_FILE_BUSY, "Can not delete MediaStreamFile. File is in Use ");
		}

		if (mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)) {
			sendDeleteMediaStoreRequestToDMS(mediaStreamFileDoc, request, xmpp, log, schema, resourceFactory, serviceAgentFactory, this.getClass().getName());
			return null;
		}

		handleFileDeleteOrRestore(mediaStreamFileDoc, mediaStreamFileNID, action, pemXPath,  moveToRecycleBin, request, schema, log, xmpp, resourceFactory, serviceAgentFactory);

		return request.createResponseData();
	}


	private Document handleFileDeleteOrRestore(Document mediaStreamFileDoc, String mediaStreamFileNID, String action, String pemXPath, Boolean moveToRecycleBin, MRRequest request, MRDB schema, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
			throws MRException, Exception
			{
		String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
		Document responseDoc = null;
		if (moveToRecycleBin) {
			handleFileDeleteOrRestore(mediaStreamFileDoc, mediaStreamFileNID, action, pemXPath,  request, schema, log, xmpp, resourceFactory, serviceAgentFactory);
		}
		else {
			if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
				String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
				List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, schema);

				if ((listOfFiles != null) && (!listOfFiles.isEmpty())) {
                    for (Object object:listOfFiles ) { 
						Element fileEL = (Element)object;
						sendDeleteMediaStoreRequestToDMS(MRXMLUtils.elementToDocument(fileEL), 
								request, xmpp, log, schema, resourceFactory, serviceAgentFactory, DeleteFileRequestHandler.class.getName());
					}
                }
			}
			else
			{
				sendDeleteMediaStoreRequestToDMS(mediaStreamFileDoc, request, xmpp, log, schema, resourceFactory, serviceAgentFactory,this.getClass().getName());
			}
			request.addAuditEvent(AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(log, request.getUserJID(), mediaStreamFileDoc, mediaStreamFileDoc, request.getOperatorGroups(), 
					AuditEventLogger.STATUS_SUCCESS, AuditEventLogger.LEVEL_INFO, "", AMConst.MEDIA_STREAM_FILE_RESOURCE, AMConst.FILE_DELETE_REQUEST));
			request.setLogEvent(false);
		}
		return responseDoc;
			}

	private Document handleFileDeleteOrRestore(Document mediaStreamFileDoc, String mediaStreamFileNID, String action, String pemXPath,  MRRequest request, MRDB schema, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
			throws MRException, Exception
			{

		String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

		String parentDirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");


		String newParentDirNID = null;
		//if the action is restore then move the files to regular dir. If the action is delete move the files to recycle bin dir.
		if(action.equals(AMConst.MEDIA_RESTORE_REQUEST)){
			if(isFileInDefaultDir(parentDirNID)) {
				newParentDirNID=MediaStoreServiceConst.DEFAULT_DIR;
			} else {
				Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(parentDirNID, request, schema, log);
				newParentDirNID = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
				if ((newParentDirNID == null) || newParentDirNID.isEmpty()) {
					throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NOT_FOUND, "Parent Media Dir Not Found");
				} else if(newParentDirNID.equals(MediaStoreServiceConst.DEFAULT_DIR)){
					newParentDirNID = MRXMLUtils.getAttributeValue(mediaStreamDirDoc.getRootElement(), MediaStoreServiceConst.NID, "");                	
				}
			}
		} else {

			if(isFileInDefaultDir(parentDirNID)) {
				newParentDirNID=MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME;
			} else {
				newParentDirNID = MediaStoreServiceUtils.getRecycleBinNID(parentDirNID, request, xmpp, log, schema, resourceFactory, serviceAgentFactory);
			}
		}   


		if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
			String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
			List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, schema);


			if ((listOfFiles != null) && (!listOfFiles.isEmpty())) {
				Element fileEL = null;
				String fileNID = null;
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
					handleFileDeleteOrRestore(MRXMLUtils.elementToDocument(fileEL), fileNID, action, pemXPath,  request, schema, log, xmpp, resourceFactory, serviceAgentFactory);
				}
			}
		}

		String state = MediaStoreServiceConst.DELETED;
		if(action.equals(AMConst.MEDIA_RESTORE_REQUEST)){
			state = MediaStoreServiceConst.READY;
		} 

		MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//ParentMediaDirNID", newParentDirNID);

		//doing partial updates to eliminate race conditions        
		AssetAdminServiceUtils.updateChild(mediaStreamFileNID, "//ParentMediaDirNID", MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//ParentMediaDirNID"), request, log, schema, xmpp, resourceFactory, serviceAgentFactory);
		
		if(isFileInDefaultDir(newParentDirNID)){
			MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+AMConst.STATEFLAG, state);
			AssetAdminServiceUtils.updateChild(mediaStreamFileNID, "//StateFlag", MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//StateFlag"), request, log, schema, xmpp, resourceFactory, serviceAgentFactory);        	     
		} else {
			//send store request to update media store file state to deleted and update the group length and size
			//send media store request to update media store file status
			MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//"+AMConst.STATEFLAG, state);
			String updatedState = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+AMConst.STATEFLAG, state);
			new SetFileInfoRequestHandler().SetFile(request, mediaStreamFileNID, "", "", null, updatedState, xmpp, log, schema, resourceFactory, serviceAgentFactory);
		}


		//log audit event for media deletion
		if (mediaType.equals(MediaRoomServiceConst.MEDIACLIP) || mediaType.equals(MediaRoomServiceConst.MEDIAIMAGE) || mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
			request.addAuditEvent(AuditEventUtils.getAuditEventObjectForGivenObjectAndAction(log, request.getUserJID(), mediaStreamFileDoc, mediaStreamFileDoc, request.getOperatorGroups(), 
					AuditEventLogger.STATUS_SUCCESS, AuditEventLogger.LEVEL_INFO, "", AMConst.MEDIA_STREAM_FILE_RESOURCE, AMConst.DELETE_REQUEST));
			request.setLogEvent(false);
		}

		return null;
	}

	public void deleteFile(Document mediaStreamFileDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,String responseHandlerClass) throws MRException, Exception{
		sendDeleteMediaStoreRequestToDMS( mediaStreamFileDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory,
				responseHandlerClass);
	}


	@Override
    protected Document getResponseDocument(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			Document userRequestdoc,MRRequest userRequest,Document agentResponseDoc) throws MRException,Exception {

		Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(agentResponseDoc,MRConst.HEADER_XPATH,MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);
		if (state == MediaStoreServiceConst.FAILURE_STATE) {
			return userRequest.createResponseData();
		}

		Element fileEl = MRXMLUtils.getElementXpath(agentResponseDoc, "//File");
		String mediaStreamFileNID = MRXMLUtils.getAttributeValue(fileEl, AMConst.NID, "");
	       
        return deleteFileAfterAgentResponse(xmpp, log, db, resourceFactory, serviceAgentFactory,  userRequest, agentResponseDoc, mediaStreamFileNID);
	}
	
	
	
	
	protected Document deleteFileAfterAgentResponse(XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
			MRRequest userRequest,Document agentResponseDoc,String mediaStreamFileNID) throws MRException,Exception {

		Document responseDoc = getFileResponse(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, mediaStreamFileNID, "","");

		Document mediaStreamFileDoc=deleteMediaStreamFileDB(mediaStreamFileNID, userRequest, log, db, xmpp, resourceFactory, serviceAgentFactory,new DeleteFileRequestHandler());
		String fileType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");

		// invalidate the recording thumbnail cache after delete.
		MediaStoreServiceUtils.invalidateFileThumbnailCache(mediaStreamFileNID, log);
        
		Document notificationDoc = MRXMLUtils.getDocumentXpath(responseDoc, "//File[@NID='"+mediaStreamFileNID+"']");
        if(notificationDoc != null ) {
            Document clonedNotificationDoc = (Document)notificationDoc.clone();
            if(fileType.equals(MediaRoomServiceConst.MEDIAGROUP)) { //remove FileList in case of mediagroup notification data.
                MRXMLUtils.removeNodeXpath(clonedNotificationDoc, "//FileList");
            }
            ((AppServerAgent)(serviceAgentFactory.getServiceAgent(AppServerAgent.class.getName()))).getResourceManager().queueObjectNotification(xmpp, userRequest, log, db, SessionServiceConst.FILE, SessionServiceConst.FILE, mediaStreamFileNID, SessionServiceConst.DELETE, clonedNotificationDoc);
        }
        
       return responseDoc;
	}
	
}