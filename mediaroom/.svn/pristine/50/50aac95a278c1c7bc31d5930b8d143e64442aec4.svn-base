package com.ipvs.assetadminservice.impl;

import java.text.SimpleDateFormat;
import java.util.Date;

import javax.servlet.http.HttpServletResponse;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AuditEvent;
import com.ipvs.common.service.AuditEventLogger;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.http.common.HttpAuthentication;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.StreamPolicyUtils;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.impl.GetObjectUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.sessionservice.impl.SessionServiceUtils;
import com.ipvs.sessionservice.resource.handler.OffloadFileRequestHandler;
import com.ipvs.sessionservice.resource.handler.PrepareFileRequestHandler;
import com.ipvs.sessionservice.resource.handler.PublishFileDownloadStateRequestHandler;
import com.ipvs.sessionservice.resource.handler.SessionServiceResourceConst;
import com.ipvs.sessionservice.resource.handler.StartDirCapacityManagementRulesRequestHandler;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;

public class FileSM {
    public static final String SCRIPT_MEDIA_FILE_CONVERSION = "/etc/scripts/mediafile_conversion_utils.sh";
    public static final String SCRIPT_MEDIA_FILE_OFFLOAD = "/etc/scripts/nfs_upload.sh";
    //const 
    public static final String TYPE_EXPORT = "Export";
    public static final String TYPE_IMPORT = "Import";
    public static final String TYPE_DELETE = "Delete";
    public static final String TYPE_OFFLOAD = "Offload";

    public static final String MEDIA_FORMAT_STANDARD= "Standard";
    public static final String MEDIA_FORMAT_PROPRIETARY = "Proprietary";
    public static final String MEDIA_FORMAT_BOTH = "Both";
    
    public static final String OWNER_AGENT_CLASS_NAME = MediaRoomServiceConst.OPERATOR;
    public static final String FILE_STATEMACHINE_XML = "/com/ipvs/assetadminservice/impl/FileStateMachineTransitions.xml";
    public static final String FILE_STATELIST_XML = "/com/ipvs/assetadminservice/impl/FileSMActionList.xml";
    ///////////// file transfer types /////////////////
    //user events 
    public static final String EXPORT_IMPORT_FILE_EVENT = "ExportImportFileEvent";
    public static final int EXPORT_FILE_COMPLETE_EVENT_CODE = 1;
    public static final int EXPORT_FILE_ERROR_EVENT_CODE = 2;
    public static final int IMPORT_FILE_COMPLETE_EVENT_CODE = 3;
    public static final int IMPORT_FILE_ERROR_EVENT_CODE = 4;
    public static final int OFFLOAD_FILE_COMPLETE_EVENT_CODE = 5;
    public static final int OFFLOAD_FILE_ERROR_EVENT_CODE = 6;

    ////////////////////////////////all actions /////////////////////////////////////
    public static final String ACTION_READY = "handle_ready";
    public static final String ACTION_QUEUE = "handle_queue";
    public static final String ACTION_COMPLETED = "handle_completed";
    public static final String ACTION_ERROR = "handle_error";
    public static final String ACTION_RESUME = "error_resume";

    //check timeouts
    public static final String ACTION_CHECK_STATUS = "handle_checkStatus";
    public static final String ACTION_DMSSTATUS = "handle_dmsStatus";
    ///////////////////////////////////export actions///////////////////////////////////////////
    public static final String ACTION_EXPORT_PACK = "handle_export_pack";
    public static final String ACTION_EXPORT_FILETRANSFER = "handle_export_fileTransfer";
    ///////////////////////////////////import actions///////////////////////////////////////////
    public static final String ACTION_IMPORT_PACK = "handle_import_pack";
    public static final String ACTION_IMPORT_GETDMSBLOBPATH = "handle_import_getDMSBlobPath";
    public static final String ACTION_IMPORT_FILETRANSFER = "handle_import_fileTransfer";    
    public static final String ACTION_IMPORT_UNPACK = "handle_import_unpack";
    public static final String ACTION_IMPORT_DBFILETRANSFER = "handle_import_dbFileTransfer";
    public static final String ACTION_IMPORT_DB = "handle_import_db";
    public static final String ACTION_IMPORT_CLEANUPMEDIAFILE = "handle_import_cleanupMediaFile";
    public static final String ACTION_IMPORT_CLEANUPTEMPFILES = "handle_import_cleanupTempFiles";
    ///////////////////////////////////export actions///////////////////////////////////////////
    public static final String ACTION_OFFLOAD_GETDMSBLOBPATH = "handle_offload_getDMSBlobPath";
    public static final String ACTION_OFFLOAD_DB = "handle_offload_db";
    public static final String ACTION_OFFLOAD_DBFILETRANSFER = "handle_offload_dbFileTransfer";
    public static final String ACTION_OFFLOAD_PACK = "handle_offload_pack";
    public static final String ACTION_OFFLOAD_FILETRANSFER = "handle_offload_fileTransfer";
    public static final String ACTION_OFFLOAD_FILEDELETE = "handle_offload_fileDelete";
    public static final String ACTION_OFFLOAD_CLEANUPTEMPFILES = "handle_offload_cleanupTempFiles";

    //////////////////state all //////////////////////////
    public static final String STATE_ERROR = "error";
    public static final String STATE_ERROR_RESUME = "error_resume";
    public static final String STATE_READY = "ready";
    public static final String STATE_COMPLETED = "completed";
    public static final String STATE_QUEUED = "queued";
    ////////////////////todo//////////////////
    public static final String STATE_CHECK_STATUS = "checkStatus";
    public static final String STATE_DMSSTATUS = "dmsStatus";

    public static final String DMSSTATUS_TRANS_OK = "dmsStatus_trans_ok";

    /////////////////////events from tar utility///////////////////////
    public static final String MEDIA_FILE_CONVERTER_EVENT = "MediaFileConverterEvent";
    public static final String MEDIA_FILE_CONVERTER_EVENT_REQUEST = "MediaFileConverterRequest";
    
    ////////////////////////from dms to ms events//////////////////////////
    public static final String MEDIA_FILE_PACK_COMPLETE_EVENT = "MediaFilePackComplete";
    public static final String MEDIA_FILE_PACK_ERROR_EVENT = "MediaFilePackError";
    public static final String MEDIA_FILE_UNPACK_COMPLETE_EVENT = "MediaFileUnPackComplete";
    public static final String MEDIA_FILE_UNPACK_ERROR_EVENT = "MediaFileUnPackError";
    public static final String MEDIA_2DOT4_FILE_UNPACK_COMPLETE_EVENT = "Media2dot4FileUnPackComplete";
    public static final String MEDIA_2DOT4_FILE_UNPACK_ERROR_EVENT = "Media2dot4FileUnPackError";
    public static final String MEDIA_FILE_NFS_COMPLETE_EVENT = "NfsUploadComplete";
    public static final String MEDIA_FILE_NFS_ERROR_EVENT = "NfsUploadError";
    
    ///////////////////////////request to export from ms DMS///////////////////////////
    public static final String PREPARE_FILE_REQUEST = "PrepareFileRequest"; 
    public static final String PREPARE_FILE_DOWNLOAD_REQUEST = "PrepareFileForDownloadRequest";
    public static final String PUBLISH_FILE_REQUEST = "PublishFileDownloadStateRequest"; 
    public static final String EXPORT_FILE_REQUEST = "ExportFileRequest";
    public static final String EXPORT_FILE_PACK_REQUEST = "ExportFilePackRequest";
    public static final String EXPORT_CLEANUP_TEMP_FILES = "ExportCleanupTempFilesRequest";
    /////////////////////////////request to import from ms DMS///////////////////////////
    public static final String IMPORT_FILE_REQUEST = "ImportFileRequest";
    public static final String IMPORT_FILE_PACK_REQUEST = "ImportFilePackRequest";
    public static final String IMPORT_FILE_UNPACK_REQUEST = "ImportFileUnPackRequest";
    public static final String IMPORT_CLEANUP_TEMP_FILES = "ImportCleanupTempFilesRequest";
    public static final String IMPORT_CLEANUP_MEDIA_FILE = "ImportCleanupMediaFileRequest";
    public static final String EXPORT_IMPORT_FILE_STATUS_REQUEST = "ExportImportFileStatusRequest";
    /////////////////////////////request to abort / delete from ms DMS///////////////////////////
    public static final String EXPORT_IMPORT_CLEANUP_TEMP_FILES = "ExportImportCleanupTempFilesRequest";
    public static final String EXPORT_IMPORT_ABORT_TEMP_FILE = "ExportImportAbortFileRequest";
    public static final String PUBLISH_IMPORT_FILE_STATUS = "PublishImportFileRequest";
    
    //////////////////events  from ms to UI////////////////////
     
    /// instance variables 
    private MRRequest request;
    private XMPPI xmpp;
    private static MRLog log;
    private MRDB db;
    private MRResourceFactoryI resourceFactory;
    private ServiceAgentFactoryI serviceAgentFactory;
    private MRResponseHandlerI responseHandler;
    private Element queueEL = null;
    private String action;
    private String transition_last;
    private String state;
    private String type;
    private String fileName;
    private int errorCode = AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
    private String error = null;

    static FileSM getFileSM(Element queueEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler, String state, String responseHandlerName) throws Exception {
        FileSM fileSM = new FileSM(queueEL, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandler, state, responseHandlerName);
        return fileSM;
    }

    ///////////////////////////////////////
    public FileSM(Element queueEL, MRRequest userRequest, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler, String state, String responseHandlerName) {
        this.queueEL = queueEL;
        this.request = userRequest;
        this.xmpp = xmpp;
        if (FileSM.log == null) {
            FileSM.log = log;
        }
        this.db = db;
        this.resourceFactory = resourceFactory;
        this.serviceAgentFactory = serviceAgentFactory;
        this.responseHandler = responseHandler;
        this.state = state;
    }

    public Element getQueueEL() {
        return queueEL;
    }

    public void setQueueEL(Element queueEL) {
        this.queueEL = queueEL;
    }

    public void handleTransition(String ptransition) throws Exception {
        String transition = ptransition;
        while (transition != null) {
            transition = this.handleTransitionInternal(transition);
        }
    }

    private String handleTransitionInternal(String transition) throws Exception {
        long t0 = System.currentTimeMillis();
        String nextTransition = null;
        this.action = null;
        this.transition_last = transition;
        type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
        fileName = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@fileName", "");
        String queueELtype = MRXMLUtils.getAttributeValue(queueEL, "type", "");
        try {
            Document fileStateMachineDoc = FileSMQueue.getInstance().getFileStateMachine(log);
            Element transitionElement = MRXMLUtils.getElementXpath(fileStateMachineDoc, "//Transition[@beginstate='" + state + "' and @transition='" + transition + "']");

            if (transitionElement == null) {
                action = MRXMLUtils.getAttributeValue(queueEL, "action", "");
                String error = "Invalid transition '" + transition + "' state='" + this.state + "' prev_action='" + action + "'";
                throw new Exception(error);
            }
            action = transitionElement.attributeValue("action");
            state = transitionElement.attributeValue("endstate");
            if (action.equals(ACTION_DMSSTATUS)) {
                return handleDMSStatus();
            } else if (action.equals(ACTION_CHECK_STATUS)) {
                return handleCheckStatus();
            } else if (action.equals(ACTION_ERROR)) {
                return handleError();
            } else if (action.equals(ACTION_COMPLETED)) {
                return handleComplete();
            }

            //save before calling action....
            setQueueStateRunning(queueEL, state, action);
            FileSMQueue.getInstance().updateQueueElement_db(false, queueEL, request, db, log);

            ////////////////// ready or next //////////   
            if (action.equals(ACTION_READY)) {
                nextTransition = handle_ready(queueELtype);
                /////////////////////// export /////////////////////////////
            } else if (action.equals(ACTION_EXPORT_PACK)) {
				String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
				String startOffset = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@startOffset", "");
				String endOffset = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@endOffset", "");
				String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", FileSM.MEDIA_FORMAT_PROPRIETARY).trim();
				
				if ((!startOffset.isEmpty() && !endOffset.isEmpty()) || !isFilePrepared(fileNID, mediaFormat)) {
					nextTransition = handlePackMediaFile(fileNID);
				} else {
					nextTransition = FileSMUtils.getTransitionOK(FileSM.ACTION_EXPORT_PACK, log);
				}
			} else if (action.equals(ACTION_EXPORT_FILETRANSFER)) {
				nextTransition = handleFileTransfer();
                /////////////////////// import /////////////////////////////   
                          
            } else if (action.equals(ACTION_IMPORT_FILETRANSFER)) {
        	nextTransition = handleHttpFileTransferDMSToClient();
            }

            /////////////////////// Offload /////////////////////////////
           
			else if (action.equals(ACTION_OFFLOAD_PACK)) {
				String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
				String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", FileSM.MEDIA_FORMAT_PROPRIETARY).trim();
				
				if (!isFilePrepared(fileNID, mediaFormat)) {
					nextTransition = handlePackMediaFile(fileNID);
				} else {

					nextTransition = FileSMUtils.getTransitionOK(FileSM.ACTION_OFFLOAD_PACK, log);
				}

			} else if (action.equals(ACTION_OFFLOAD_FILETRANSFER)) {
				nextTransition = handlePackOffloadMediaFile(queueELtype);
			}

            } catch (MRException ex) {
            ex.printStackTrace();
            //set error 
            setErrorCode(ex.getCode());
            setError(ex.getDescription());
            //save
            saveError();
            nextTransition = null;
        } catch (Exception ex) {
            ex.printStackTrace();
            //set error 
            String error = FileSMUtils.getMRException(queueEL, ex.getMessage());
            MRException ex2 = new MRException(AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION, error);
            setErrorCode(ex2.getCode());
            setError(ex2.getDescription());
            //save
            saveError();
            nextTransition = null;
        } finally {
            log.addInfo("FileSM:handleTransition:"+action+" DONE:" + (System.currentTimeMillis() - t0) + "  " + toString() + "" + "  nextTransition(" + nextTransition + ")", MRLog.INFO, MRLog.NOTIFY);
            MRLog.println("FileSM:handleTransition:"+action+" DONE:" + (System.currentTimeMillis() - t0) + "  " + toString() + "" + "  nextTransition(" + nextTransition + ")");
        }
        return nextTransition;
    }

    private String handleError() throws Exception {
        state = MRXMLUtils.getAttributeValue(queueEL, "state", "");
        action = MRXMLUtils.getAttributeValue(queueEL, "action", "");
        
        String err_code = MRXMLUtils.getValueXpath(queueEL, "//Exception/Code", "");
        String err_desc = MRXMLUtils.getValueXpath(queueEL, "//Exception/Description", "");
        
        if(!err_code.isEmpty()) {
            setErrorCode(Utils.getIntValue(err_code, -1));
        }
        if(!err_desc.isEmpty()) {
            setError(err_desc);
        }
        
        saveError();

        if (type.equals(FileSM.TYPE_OFFLOAD)){
        	// callback request to Dir capacity management handler.
        	callBackStartDirCapacityManagementRules();
        }
        return null;
    }

    private String handleComplete() throws Exception {
        try {
            FileSMUtils.cleanupTempFiles(queueEL);
            //save
            setQueueStateCompleted(queueEL);
            MRXMLUtils.setAttributeValue(queueEL, "dateCompleted", String.valueOf(System.currentTimeMillis()));
            FileSMUtils.sendAgentEventToOwner(true, queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            String type=MRXMLUtils.getAttributeValue(queueEL, "type","").trim();
            try {
                //skip state update for import/offload
                if(type.equals(FileSM.TYPE_EXPORT)){
                    FileSMUtils.setFileStateReady(queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                }  
                if(type.equals(FileSM.TYPE_IMPORT)){
                	//update the media group bypass flag value
                	String mediaGroupID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_mediaGroupID", "");
                	if(!mediaGroupID.equals("")){	            		
                		Document mediaGroupFileDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);
                		MediaStoreServiceUtils.updateMediaGroupProperties(mediaGroupFileDoc, mediaGroupID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                	}
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
            FileSMQueue.getInstance().updateQueueElement_db(true, queueEL, request, db, log);
            logAuditEvent(queueEL,request,db,log);
            
            if (type.equals(FileSM.TYPE_OFFLOAD)){
            	// callback request to Dir capacity management handler.
            	//callBackStartDirCapacityManagementRules();
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return null;
    }
    
    private void callBackStartDirCapacityManagementRules() throws Exception{    
    	
    	Element internlaEL = MRXMLUtils.getElementXpath(queueEL, ".//Internal");
    	String dirNID = MRXMLUtils.getAttributeValue(internlaEL, "dirNID", "");    
    	
    	Document requestDoc = MRXMLUtils.loadXMLResource(SessionServiceConst.START_DIR_CAPACITY_MANAGEMENT_RULES_REQUEST_XML, log);
		String requestName = MRRequest.getRequestName(StartDirCapacityManagementRulesRequestHandler.class.getName());
		
		Element getFileEL = MRXMLUtils.getElementXpath(requestDoc,"//"+MRRequest.getRequestRootName(requestName));
		MRXMLUtils.setAttributeValue(getFileEL, SessionServiceResourceConst.BY_NID_ATTRIBUTE, dirNID);
		
		Document callBackRequestDoc = SessionServiceUtils.getRequestXML(requestName, log,getFileEL);
		MRRequest callbackRequest = MRRequest.createRequest(request, callBackRequestDoc, requestName, ServiceClient.SESSIONSERVICE_RESOURCE);
		callbackRequest.setSuperUser(true);
		StartDirCapacityManagementRulesRequestHandler handler = new StartDirCapacityManagementRulesRequestHandler();
	    log.addInfo("CallBack StartDirCapacityManagementRules: byNID=" + dirNID, MRLog.INFO, MRLog.NOTIFY);
        handler.getResponse(callbackRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);

    }

    private void logAuditEvent(Element queueEL, MRRequest request, MRDB db,MRLog log) throws MRException,Exception {
		String name = MRXMLUtils.getAttributeValue(queueEL, "type","");
		Element internlaEL = MRXMLUtils.getElementXpath(queueEL, ".//Internal");
		Element externalEL = MRXMLUtils.getElementXpath(queueEL, ".//External");
		
		String state = MRXMLUtils.getAttributeValue(queueEL, "state_description","");
		if(!state.equals("Completed") && !state.endsWith("Error")) {
			return;
		}
		
		String fileTitle = MRXMLUtils.getAttributeValue(internlaEL, "title","");
		String dirTitle = MRXMLUtils.getAttributeValue(internlaEL, "dirTitle","");
		String fileNID = MRXMLUtils.getAttributeValue(internlaEL, "prev_fileNID", "");
		String userJID = MRXMLUtils.getAttributeValue(externalEL, "JID","");
		String dateCreated = MRXMLUtils.getAttributeValue(internlaEL, "dateCreated", "");
		if(!dateCreated.isEmpty()) {
			long timeStamp = Utils.getLongValue(dateCreated, -1);
			if(timeStamp != -1) {
				SimpleDateFormat simpleDateFormat = new SimpleDateFormat("MM/dd/yy HH:mm:ss");		
				dateCreated = simpleDateFormat.format(new Date(timeStamp));
			}
		}  
		
		String status = AuditEventLogger.STATUS_SUCCESS;
		String level = AuditEventLogger.LEVEL_INFO;
		String errorDescription = "";
		if(state.equals("Error")){
			status = AuditEventLogger.STATUS_FAILURE;
			level = AuditEventLogger.LEVEL_ERROR;
			errorDescription = "Error - ";
			if(name.equals(TYPE_EXPORT)) {
				errorDescription += "Export Failed";
			} else if(name.equals(TYPE_IMPORT)) {
				errorDescription += "Import Failed";
			}else if(name.equals(TYPE_OFFLOAD)) {
				errorDescription += "Offload Failed";
			}
		}
		AuditEvent auditEvent = new AuditEvent.Builder(AuditEventLogger.MEDIA_EVENT, name, userJID, level).status(status)
		.descriptionText("Directory - "+dirTitle + " dateCreated - " +dateCreated +" fileNID - " +fileNID+ " " +errorDescription).target(fileTitle).build();
		AuditEventLogger.getInstance().addAuditEventToQueue(auditEvent);
		
	}

	private String handleDMSStatus() {
        try {
            //if DMS is not busy close the current one with error
            String mediafile_converter_status = MRXMLUtils.getValueXpath(queueEL, "//mediafile_converter_status", "");
            if (mediafile_converter_status.equals("FREE")) {
                String queueNID = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
                Element queueEL2 = FileSMUtils.readQueueElement_db(queueNID, request, db, log);
                int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL2, "queue_state", ""), -1);
                if (queue_state == FileSMQueue.QUEUE_STATUS_RUNNING) {
                    state = MRXMLUtils.getAttributeValue(queueEL2, "state", "");
                    action = MRXMLUtils.getAttributeValue(queueEL2, "action", "");
                    errorCode = AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                    error = "Time out during '" + FileSMUtils.getActionDescription(action, log) + "'.";
                    saveError();
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return null;
    }

    ///////////////////////// todo ////////////////////////////////////////////
    public void saveError() {
        try {
            setQueueState(queueEL, FileSMUtils.getActionDescription(FileSM.ACTION_ERROR, log), state, action, FileSMQueue.QUEUE_STATUS_ERROR);
            int errorCount = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "errorCount", ""), 0)+1;
            MRXMLUtils.addAttribute(queueEL, "errorCount", (errorCount) + "");
            if(error==null ||error.equals("")){
                String action = MRXMLUtils.getAttributeValue(queueEL, "action", "");
                error=FileSMUtils.getActionDescription(action, log) ;
            }   
            log.addInfo("FileSM:SaveError: " + errorCode + " " + error + "  " + toString(), MRLog.INFO, MRLog.NOTIFY);
            MRLog.println("FileSM:SaveError: " + errorCode + " " + error + "  " + toString());
            FileSMUtils.addErrorToQueueElement(queueEL, errorCode, error);
            FileSMQueue.getInstance().updateQueueElement_db(true, queueEL, request, db, log);
            FileSMUtils.sendAgentEventToOwner(false, queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
            logAuditEvent(queueEL, request, db, log);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public void setQueueStateRunning(Element queueEL, String state, String action) {
        setQueueState(queueEL, FileSMUtils.getActionDescription(action, log), state, action, FileSMQueue.QUEUE_STATUS_RUNNING);
    }

    public void setQueueStateCompleted(Element queueEL) {
        MRXMLUtils.addAttribute(queueEL, "dateCompleted", System.currentTimeMillis() + "");
        setQueueState(queueEL, FileSMUtils.getActionDescription(ACTION_COMPLETED, log), STATE_COMPLETED, ACTION_COMPLETED, FileSMQueue.QUEUE_STATUS_COMPLETED);
    }

    public static void setQueueState(Element queueEL, String state_description, String state, String action, int queueStatus) {
        MRXMLUtils.addAttribute(queueEL, "state_description", state_description);
        MRXMLUtils.addAttribute(queueEL, "state", state);
        MRXMLUtils.addAttribute(queueEL, "action", action);
        MRXMLUtils.addAttribute(queueEL, "queue_state", queueStatus + "");
        MRXMLUtils.addAttribute(queueEL, "dateUpdated", System.currentTimeMillis() + "");
    }

    ////////////////////// set state ////////////////////////

    //////////////////////// dms request ///////////////////////
    private String handleCheckStatus() throws Exception {
        FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_IMPORT_FILE_STATUS_REQUEST, queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        return null;
    }

 private String handlePackMediaFile(String fileNID) throws Exception {
	 
	 	String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", FileSM.MEDIA_FORMAT_PROPRIETARY).trim();
	 	Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
		String streamType = MRXMLUtils.getAttributeValue(internalEL, "streamType", "");
		String startOffset = MRXMLUtils.getAttributeValue(internalEL, "startOffset", "");
		String endOffset = MRXMLUtils.getAttributeValue(internalEL, "endOffset", "");
		String new_fileUUID = MRXMLUtils.getAttributeValue(internalEL, "new_UUID", "");
		Document prepareRequestDoc = getRequestDocument(fileNID, mediaFormat,streamType, PREPARE_FILE_REQUEST);
		Element prepareFileEl = MRXMLUtils.getElementXpath(prepareRequestDoc, "//PrepareFile");
		MRXMLUtils.addAttribute(prepareFileEl, "startOffset", startOffset);
		MRXMLUtils.addAttribute(prepareFileEl, "endOffset", endOffset);
		MRXMLUtils.addAttribute(prepareFileEl, "new_fileUUID", new_fileUUID);
		
		PrepareFileRequestHandler handler = new PrepareFileRequestHandler();
		MRRequest prepareRequest = MRRequest.createRequest(request, prepareRequestDoc, PREPARE_FILE_REQUEST, request.getServiceName());
		prepareRequest.setSuperUser(true); //to give permission to internal calls where userjid is manamentserver.localhost
		handler.getResponse(prepareRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		return null;
	}
 
 private String handleFileTransfer() throws Exception {

		String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", FileSM.MEDIA_FORMAT_PROPRIETARY).trim();
		String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
		Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
		String streamType = MRXMLUtils.getAttributeValue(internalEL, "streamType", "");
		Document downloadRequestDoc = getRequestDocument(fileNID, mediaFormat,streamType, PUBLISH_FILE_REQUEST);
		PublishFileDownloadStateRequestHandler handler = new PublishFileDownloadStateRequestHandler();
		MRRequest downloadRequest = MRRequest.createRequest(request, downloadRequestDoc, PUBLISH_FILE_REQUEST, request.getServiceName());
		handler.getResponse(downloadRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		return null;
	}

 
	private Document getRequestDocument(String fileNID, String mediaFormat,String streamType, String requestName) throws Exception {

		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document requestDoc = (Document) requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.SESSION_RESOURCE_SERVICE);
		Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(requestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(requestName.replace("Request", ""));
		MRXMLUtils.addAttribute(requestEl, "byNID", fileNID);
		MRXMLUtils.addAttribute(requestEl, "format", mediaFormat);
		MRXMLUtils.addAttribute(requestEl, "streamType", streamType);
		MRXMLUtils.addAttribute(requestEl, "type", type);
		MRXMLUtils.addElement(requestDataEl, requestEl);
		return requestDoc;
	}
    
    private String handlePackOffloadMediaFile(String queueELType) throws Exception {
    	String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
    	String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", FileSM.MEDIA_FORMAT_PROPRIETARY).trim();
    	String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
    	String nasURL = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@nasURL", "");
    	String UUID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "");
    	String requestName="OffloadFileRequest";
		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document offloadRequestDoc = (Document)requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(offloadRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, requestName);
		MRXMLUtils.setAttributeValueXPath(offloadRequestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.SESSION_RESOURCE_SERVICE);
		Element DataEL = MRXMLUtils.getElementXpath(offloadRequestDoc, "//Data");
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(requestName + "Data"));
		Element requestEl = MRXMLUtils.newElement("OffloadFile");
		MRXMLUtils.addAttribute(requestEl,SessionServiceResourceConst.BY_DIR_NID, dirNID);
		MRXMLUtils.addAttribute(requestEl,"nasURL", nasURL);
		MRXMLUtils.addAttribute(requestEl, "format", mediaFormat);
		MRXMLUtils.addAttribute(requestEl, "NID", fileNID);
		MRXMLUtils.addAttribute(requestEl, "UUID", UUID);
		MRXMLUtils.addElement(requestDataEl, requestEl);
		MRRequest offloadRequest = MRRequest.createRequest(request, offloadRequestDoc, requestName, request.getServiceName());
		OffloadFileRequestHandler handler = new OffloadFileRequestHandler();
		handler.getResponse(offloadRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);
		return null;
    }
/*    private String handleImportPack() throws Exception {
        FileSMUtils.sendAgentRequestToOwner(FileSM.IMPORT_FILE_PACK_REQUEST, queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        return null;
    }   
    private String handleExportGetDMSBlobPath() throws Exception {
        FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_FILE_REQUEST, queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        return null;
    }
    private String handleExportCleanupTempFiles() throws Exception {
        FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_CLEANUP_TEMP_FILES, queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        return null;
    }*/

    private boolean isFilePrepared(String fileNID, String format) throws Exception{    	
    	Document mediaFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(fileNID, request, log, db);
    	log.addInfo("FileSM:isFilePrepared: format::" + format);
    	if (format.equalsIgnoreCase(MEDIA_FORMAT_STANDARD)) {
    		return fileFormatCheck(mediaFileDoc, MEDIA_FORMAT_STANDARD);  
    	}else if (format.equalsIgnoreCase(MEDIA_FORMAT_PROPRIETARY)) {
    		return fileFormatCheck(mediaFileDoc, MEDIA_FORMAT_PROPRIETARY);  
    	}else {
    		if(!fileFormatCheck(mediaFileDoc, MEDIA_FORMAT_STANDARD) || !fileFormatCheck(mediaFileDoc, MEDIA_FORMAT_PROPRIETARY)){
    			return false;
    		}
    		return true;
    	}
    	   	  	
    }

	private boolean fileFormatCheck(Document mediaFileDoc, String mediaFormat) throws Exception {
		Element mediaElement =MRXMLUtils.getElementXpath(mediaFileDoc, "//Media[@format='"+mediaFormat+"']");		
		String stateFlag = MRXMLUtils.getAttributeValue(mediaElement, "state", "");
		String isStale = MRXMLUtils.getAttributeValue(mediaElement, "stale", "");
		log.addInfo("FileSM:isFilePrepared: " + stateFlag + " " + stateFlag + "  , isStale" + " "+isStale, MRLog.INFO, MRLog.NOTIFY);
		if ((!stateFlag.equalsIgnoreCase("Prepared")&& (!stateFlag.equalsIgnoreCase("Ready"))) || stateFlag.isEmpty() || (!isStale.isEmpty() && isStale.equalsIgnoreCase("true"))) {
            return false;
        } else {
            return true;
        }
	}
    
    
    //////////////////////// dms request ///////////////////////

    //////////////////// file transfer start//////////////////////////////////////

 /* private String handleFileTransferDMSToClient() throws Exception {
    	
    	String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", FileSM.MEDIA_FORMAT_PROPRIETARY).trim();
    	Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
		String streamType = MRXMLUtils.getAttributeValue(internalEL, "streamType", "");
		if(mediaFormat.equals(FileSM.MEDIA_FORMAT_STANDARD) && !streamType.equals(MediaRoomServiceConst.V2D) && !streamType.equals(MediaRoomServiceConst.UDP)) {
    		FileSMUtils.handleStandardMediaFileTransfer(action, queueEL, request, xmpp, log, db);
    		return null;
    	}
    	
        String fileXferID = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
        String fromFilename = FileSMUtils.getDMSFilePath_export(queueEL);
        String fromAgentJID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@JID", "");
        String toFilename = FileSMUtils.getClientFilePath_export(queueEL);
        String toAgentJID = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@JID", "");
        //setup file transfer 
        FileSMUtils.handleStartTransfer(action, fileXferID, fromAgentJID, fromFilename, toAgentJID, toFilename, request, xmpp, log, db);
        return null;
    }*/
	private String handleHttpFileTransferDMSToClient() throws Exception {

		String toAgentJID = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@JID", "");
		
		String dstPortTitle = "";
		
		try {
			dstPortTitle = ServiceAgentUtils.getAgentName(toAgentJID);
		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} 
		
		String xqueryFileName = SessionServiceConst.XQUERYPACKAGE + SessionServiceResourceConst.IMPORTURL + SessionServiceConst.XQUERYXML;	    	   

		String query = FileUtils.readFileResource(xqueryFileName, log.getClass());

		String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
		String queueID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@queueID", "");
		String groupID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@groupID", "");
		String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "").trim();
		query = query.replace(SessionServiceResourceConst._DIRNID_,dirNID);
		query = query.replace(SessionServiceResourceConst._QUEUEID_,queueID);
		query = query.replace(SessionServiceResourceConst._GROUPNID_,groupID);
		query = query.replace("_DSTTITLE_",dstPortTitle);
		query = query.replace("_UUID_",uuid);

		MRRequest newRequest = MRRequest.createRequest(request.getUserJID());
		newRequest.setSuperUser(true);
		Document queryResultDoc = GetObjectUtils.executeQuery(query,newRequest, xmpp, log, db, serviceAgentFactory, resourceFactory);
		
		String importURL = MRXMLUtils.getAttributeValue(queryResultDoc.getRootElement(), SessionServiceResourceConst.URL_ATTRIBUTE, "");	
		if (importURL.isEmpty()) {
			throw new MRException(HttpServletResponse.SC_INTERNAL_SERVER_ERROR, "Import URL Not Found");
		}
		
		//find ipaddress based on destination where he is coming from.
		Element sourceElement = MRXMLUtils.getElementXpath(queryResultDoc, "//Source");
		Element destinationElement = MRXMLUtils.getElementXpath(queryResultDoc, "//Destination");
		Document deviceDoc = MRXMLUtils.getDocumentXpath(queryResultDoc, "//Device");

		String ipaddress =  StreamPolicyUtils.getResolvedIPAddressForURL(request, xmpp, log, db, destinationElement, sourceElement, deviceDoc);
		importURL = importURL.replace("_IPADDRESS_",ipaddress);	    
		
		long now = System.currentTimeMillis();
		String timestamp = String.valueOf(now + (SessionServiceResourceConst.REDIRECT_EXPIRE_TIME * 1000));
		String signature = HttpAuthentication.generateExpiryBasedSignature(MRXMLUtils.getElementXpath(queryResultDoc, "//" + SessionServiceResourceConst.API_KEY_TABLE), timestamp);
		importURL = importURL.replaceAll(SessionServiceResourceConst.SIGNATURE_CONST, signature);
		importURL = importURL.replaceAll(SessionServiceResourceConst.TIMESTAMP_CONST, timestamp);
		log.addInfo("FileSM.handleHttpFileTransferDMSToClient:: Import Url::" + importURL,MRLog.INFO,MRLog.NOTIFY);
		String fromFilename = FileSMUtils.getClientFilePath_import(queueEL);
		String fromAgentJID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@JID", "");
	
		//call to setup file transfer 
		FileSMUtils.handleStartHttpTransfer(action, queueID, fromAgentJID, fromFilename, toAgentJID, importURL, request, xmpp, log, db);	
		return null;
	}
    
/*    private String handleFileTransferMSToDMS(String queueELType) throws Exception {
        String fileXferID = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
        String fromFilename = FileSMUtils.getMSDBFilePath_export(queueEL);
        String fromAgentJID = request.getUserJID();
        String toFilename = FileSMUtils.getDMSDBFilePath_export(queueEL);
        String toAgentJID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@JID", "").trim();
        if (toAgentJID.equals(fromAgentJID)) {
            File fileFrom = new File(fromFilename);
            if (!fileFrom.exists()) {
                errorCode = AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                error =  queueELType + "ed DB file not found";
                return FileSMUtils.getTransitionError(action, log);//EXPORT/OFFLOAD_DB_TRANS_ERROR;
            }
            File fileTo = new File(toFilename);
            fileTo.delete();
            fileFrom.renameTo(fileTo);
            return FileSMUtils.getTransitionOK(action, log);//EXPORT/OFFLOAD_DB_TRANS_OK;
        }
        //setup file transfer 
        FileSMUtils.handleStartTransfer(action, fileXferID, fromAgentJID, fromFilename, toAgentJID, toFilename, request, xmpp, log, db);
        return null;
    }

 
    private String handleExportMediaFileDB(String queueELType) throws Exception {
        Document messageDoc = null;
        
        if (IPVSConst.getInstance().getOS().equals("windows")) {
            while (messageDoc == null) { //for(int i=0;i<5;i++){
                try {
                    messageDoc = FileSMUtils.exportDBToTarFile(queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        } else {
            messageDoc = FileSMUtils.exportDBToTarFile(queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        }
        Element queueEL2 = null;
        if (messageDoc != null) {
            queueEL2 = MRXMLUtils.getElementXpath(messageDoc, "//" + AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
        }
        if (queueEL2 != null) {
            this.queueEL = queueEL2;
        } else {
            MRLog.println("handleExportMediaFileDB messageDoc is null");
        }
        String fromFilename = FileSMUtils.getMSDBFilePath_export(queueEL);
        File fileFrom = new File(fromFilename);
        if (!fileFrom.exists()) {
            errorCode = AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
            error = queueELType + "ed DB file not found.";
            return FileSMUtils.getTransitionError(action, log);
        }
        return FileSMUtils.getTransitionOK(action, log);
    }*/

    //////////////////// db import export end//////////////////////////////////////

    //////////////////// initReady start//////////////////////////////////////
    public String handle_ready(String queueELType) throws Exception {
        return initReadyQueueEntry(queueELType);
    }

    
    private String initReadyQueueEntry(String type) throws Exception {
        Element internalEL = MRXMLUtils.getChild(queueEL, "Internal");
        
        if(TYPE_IMPORT.equals(type)) {
        	String dirNID = MRXMLUtils.getAttributeValue(internalEL, "dirNID", "").trim();
            Element elDir = FileSMUtils.readMediaStreamDir_db(dirNID, request, db, log);
            String dirTitle = MRXMLUtils.getValueXpath(elDir, "//Info/Title", "");
            String dirJID = MediaStoreServiceUtils.getMediaStoreOwnerJID(elDir.getDocument(), request, db, log);
            //check users presence before starting import
            if (!FileSMUtils.isUserPresenceAvailable(xmpp, dirJID,log, db)) {
                log.addInfo("Can not import. dirJID:" + dirJID + " is not available.", MRLog.OK, MRLog.NOTIFY);
                throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + dirJID + "</User></error>");
            }             
            String client_userJID = MRXMLUtils.getAttributeValue(queueEL, "ownerJID", "").trim();
            if (!FileSMUtils.isUserPresenceAvailable(xmpp, client_userJID, log, db)) {
                log.addInfo("Can not import. client_userJID: " + client_userJID + " is not available.", MRLog.OK, MRLog.NOTIFY);
                throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + client_userJID + "</User></error>");
            }             
            //start import
            MRXMLUtils.setAttributeValue(internalEL, "dirTitle", dirTitle);
            MRXMLUtils.setAttributeValue(internalEL, "JID", dirJID);
            MRXMLUtils.addAttribute(internalEL, "cliptType", MediaRoomServiceConst.MEDIACLIP);
            
            return FileSMUtils.getTransition(FileSM.ACTION_READY, "import_ok", log);
        } 
        else if (TYPE_EXPORT.equals(type) || TYPE_OFFLOAD.equals(type)){
        	String fileNID = MRXMLUtils.getAttributeValue(internalEL, "fileNID", "").trim();
            String uuid = MRXMLUtils.getAttributeValue(internalEL, "UUID", "").trim();
            String dirJID = MRXMLUtils.getAttributeValue(internalEL, "dirJID", "").trim();
           
            //read file and update queue with more info about file
            Element file_el = FileSMUtils.readMediaStreamFile_db(queueEL, request, db, log);
            if (file_el == null) {
                throw new MRException(AMExceptionCodes.MEDIA_EXPORT_FILE_NOT_FOUND_EXCEPTION, "Can not " + type + ". File not found ('" + fileNID + "') ");
            }
            String title = MRXMLUtils.getValueXpath(file_el, "//Info/Title", "").trim();
            String readOnly = MRXMLUtils.getValueXpath(file_el, "Info/Properties/Permissions/Write", "").trim();
            String mediatype = MRXMLUtils.getValueXpath(file_el, "//Info/MediaStoreFileInfo/Type", "").trim();
            String streamType = mediatype.equals(MediaRoomServiceConst.MEDIAIMAGE) ? MediaRoomServiceConst.SNAPSHOT : MRXMLUtils.getValueXpath(file_el, "//StreamType", "").trim();
            if (readOnly.equals("false")) {
                throw new MRException(AMExceptionCodes.MEDIA_EXPORT_FILE_NEEDS_UPGRADE_EXCEPTION, "Can not " + type + "'" + title + "' file; Please try upgrading the file.");
            }
            
            //read key fields
            String dateCreated = MRXMLUtils.getValueXpath(file_el, "//DateCreated", "").trim();
            fileNID = MRXMLUtils.getAttributeValue(file_el, "NID", "").trim();
            uuid = MRXMLUtils.getValueXpath(file_el, "//UUID", "").trim();
            
            //set 
            String cliptType = MRXMLUtils.getAttributeValue(internalEL, "cliptType", "").trim();
            String title_orig = MRXMLUtils.getAttributeValue(internalEL, "title", "").trim();
            if (!MediaStoreServiceConst.MEDIA_TYPE_ENUMS.contains(cliptType)) {
                throw new MRException(AMExceptionCodes.MEDIA_EXPORT_INVALID_TYPE_EXCEPTION, "Can not " + type + ". Unsupported file type='" + cliptType + "'");
            }
            //check users presence before starting packaging
            if (!FileSMUtils.isUserPresenceAvailable(xmpp, dirJID, log, db)) {
                log.addInfo("Can not " + type + ". dirJID:" + dirJID + " is not available.", MRLog.OK, MRLog.NOTIFY);
                throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + dirJID + "</User></error>");
            }
            String client_userJID = MRXMLUtils.getAttributeValue(queueEL, "ownerJID", "").trim();
            if (!FileSMUtils.isUserPresenceAvailable(xmpp, client_userJID, log, db)) {
                log.addInfo("Can not " + type + ". client_userJID: " + client_userJID + " is not available.", MRLog.OK, MRLog.NOTIFY);
                throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + client_userJID + "</User></error>");
            }
            //start export
            String stateFlag = MRXMLUtils.getValueXpath(file_el, "//State/StateFlag", "").trim();
            if (!stateFlag.equalsIgnoreCase("Offline") && !stateFlag.equalsIgnoreCase("Error")) {
                FileSMUtils.setFileStateFlag_db(file_el, stateFlag, type + "ing", request, db, log);
            }else {
                throw new MRException(AMExceptionCodes.MEDIA_EXPORT_INVALID_STATE_EXCEPTION, "Can not " + type + ". Media file is not in ready state. '" + stateFlag + "'.");
            }
            //set values on queue element
            if (title_orig.equals("")) {
                MRXMLUtils.setAttributeValue(internalEL, "title", title);
            }

            MRXMLUtils.setAttributeValue(internalEL, "cliptType", cliptType);
            MRXMLUtils.setAttributeValue(internalEL, "dateCreated", dateCreated);
            MRXMLUtils.setAttributeValue(internalEL, "fileNID", fileNID);
            MRXMLUtils.setAttributeValue(internalEL, "UUID", uuid);
            MRXMLUtils.setAttributeValue(internalEL, "JID", dirJID);
            MRXMLUtils.setAttributeValue(internalEL, "streamType", streamType);
            
            if(TYPE_EXPORT.equals(type)) {
                return FileSMUtils.getTransition(FileSM.ACTION_READY, "export_ok", log);
            } else {
                return FileSMUtils.getTransition(FileSM.ACTION_READY, "offload_ok", log);
            }
        } 
  
        return null;
    }

    //////////////////// initReady end////////////////////////////////////// 

    public String getState() {
        return state;
    }

    public void setState(String state) {
        this.state = state;
    }

    public int getErrorCode() {
        return errorCode;
    }

    public void setErrorCode(int errorCode) {
        this.errorCode = errorCode;
    }

    public String getError() {
        return error;
    }

    public void setError(String error) {
        this.error = error;
    }

    @Override
    public String toString() {
        StringBuffer buf = new StringBuffer();
        if (action != null) {
            buf.append(" action='" + action + "'");
        }
        buf.append(" type='" + type + "'");
        if (state != null) {
            buf.append(" state='" + state + "'");
        }
        if (transition_last != null) {
            buf.append(" prev_transition='" + transition_last + "'");
        }
        if (fileName != null) {
            buf.append(" fileName='" + fileName + "'");
        }
        return buf.toString();
    }

}
