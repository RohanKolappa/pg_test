package com.ipvs.assetadminservice.impl;

import java.io.File;
import java.net.URLEncoder;
import java.util.Hashtable;
import java.util.List;
import java.util.Vector;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.handler.AddExportFileRequestHandler;
import com.ipvs.assetadminservice.handler.AddOffloadRequestHandler;
import com.ipvs.assetadminservice.handler.DeleteExportImportFileRequestHandler;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.IpvsDBUtils;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRServer;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.UserManager;
import com.ipvs.config.IPVSConst;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.sessionservice.resource.handler.PrepareFileRequestHandler;
import com.ipvs.systemservice.handler.SystemServiceUtils;
import com.ipvs.systemservice.impl.FileTransferAgent;
import com.ipvs.systemservice.impl.SystemServiceConst;
import com.ipvs.utils.Utils;

public class FileSMUtils {
    public static final String responseHandlerName = "com.ipvs.assetadminservice.handler.AddExportFileRequestHandler";
//    public static final String offloadRespHandler = "com.ipvs.assetadminservice.handler.AddOffloadRequestHandler";
    private static String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
    private static String object_listName = null;
    private static final String DB_TAR_EXTENSION = ".tar";
    public static final String MEDIA_TAR_EXTENSION = ".mkd";
    public static final String MEDIA_TAR_EXTENSION_2DOT4 = ".zip";
    public static final String MEDIA_STANDARD_ZIP_EXTENSION = ".zip";

    //////////////////////////////////////////////
    public static String getTransitionFromEvent(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, String type, Element queueEL) throws Exception {
        Document responseDoc = request.getResult();
        String requestName = MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");        
        if(requestName!=null && requestName.isEmpty()){
        	requestName = MRXMLUtils.getAttributeValueXpath(request.getMessage().getDocument(),MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
        	responseDoc = request.getMessage().getDocument();
        }
        log.addInfo("FileSMUtils requestName ::"+requestName);
        String eventTypeXPath = "";
        if(requestName.equals(FileSM.MEDIA_FILE_CONVERTER_EVENT_REQUEST)) {
            eventTypeXPath = "//mediaFileConverterEventType";
        }
        String mediaFileConverterEventType = "";
        if(!eventTypeXPath.isEmpty()) {
            mediaFileConverterEventType = MRXMLUtils.getValueXpath(responseDoc, eventTypeXPath, "");
        }
        int resp_state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(responseDoc, "//Header","state", ""), -1);
        String refID = MRXMLUtils.getValueXpath(responseDoc, "//FileXferID", "");
        String transition = null;
        if (FileTransferAgent.FILEXFERSETUPREQUEST.equals(requestName)) {
        	String fileName = MRXMLUtils.getValueXpath(responseDoc, "//toFileName", "");
        	String[] arr = (refID + "__" + " __").split("__");
            String queueNID = arr[1];
            if (resp_state == MRRequest.STATE_EXCEPTION) {
                transition = getTransitionError(queueNID, log);
            } else {
            	String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", FileSM.MEDIA_FORMAT_PROPRIETARY).trim();
            	if(!mediaFormat.equals(FileSM.MEDIA_FORMAT_PROPRIETARY) && fileName.endsWith(FileSMUtils.MEDIA_TAR_EXTENSION)) {
            		Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
            		String streamType = MRXMLUtils.getAttributeValue(internalEL, "streamType", "");
            		if(!streamType.equals(MediaRoomServiceConst.V2D) && !streamType.equals(MediaRoomServiceConst.UDP)) {
            			FileSMUtils.handleStandardMediaFileTransfer(queueNID, queueEL, request, xmpp, log, db);
                	    return null; // to ensure the state machine does not move to next transition.
            		} else {
                		transition = getTransitionOK(queueNID, log);
            		}
            	} else {
            		transition = getTransitionOK(queueNID, log);
            	}
            }
        } else if (FileTransferAgent.FILEXFERERROR.equals(requestName)) {
            String[] arr = (refID + "__" + " __").split("__");
            String queueNID = arr[1];
            transition = getTransitionError(queueNID, log);
        } else if (FileSM.MEDIA_FILE_CONVERTER_EVENT_REQUEST.equals(requestName)) {
            if (FileSM.MEDIA_FILE_PACK_COMPLETE_EVENT.equals(mediaFileConverterEventType)) {
                transition = FileSM.TYPE_EXPORT.equals(type) ? FileSMUtils.getTransitionOK(FileSM.ACTION_EXPORT_PACK, log) : null;
            } else if (FileSM.MEDIA_FILE_PACK_ERROR_EVENT.equals(mediaFileConverterEventType)) {
                transition = FileSM.TYPE_EXPORT.equals(type) ? FileSMUtils.getTransitionError(FileSM.ACTION_EXPORT_PACK, log) : FileSMUtils.getTransitionError(FileSM.ACTION_OFFLOAD_PACK, log);
            } else if (FileSM.MEDIA_FILE_UNPACK_COMPLETE_EVENT.equals(mediaFileConverterEventType)) {
                transition = FileSMUtils.getTransitionOK(FileSM.ACTION_IMPORT_UNPACK, log);
            } else if (FileSM.MEDIA_FILE_UNPACK_ERROR_EVENT.equals(mediaFileConverterEventType)) {
                transition = FileSMUtils.getTransitionError(FileSM.ACTION_IMPORT_UNPACK, log);
            } else if (FileSM.MEDIA_2DOT4_FILE_UNPACK_COMPLETE_EVENT.equals(mediaFileConverterEventType)) {
                transition = FileSMUtils.getTransition(FileSM.ACTION_IMPORT_UNPACK, "ok_2dot4", log);
            } else if (FileSM.MEDIA_2DOT4_FILE_UNPACK_ERROR_EVENT.equals(mediaFileConverterEventType)) {
                transition = FileSMUtils.getTransitionError(FileSM.ACTION_IMPORT_UNPACK, log);
            } 
        } else {
            //response 
        	if ("PublishFileRequest".equals(requestName) && type.equalsIgnoreCase(FileSM.TYPE_EXPORT)) {
				if (resp_state == MRRequest.STATE_EXCEPTION) {
					transition = FileSMUtils.getTransitionError(FileSM.ACTION_EXPORT_PACK, log);
				} else {
					transition = FileSMUtils.getTransitionOK(FileSM.ACTION_EXPORT_PACK, log);
				}
			} else if (FileSM.PUBLISH_FILE_REQUEST.equals(requestName)) {
				if (resp_state == MRRequest.STATE_EXCEPTION) {
					transition = FileSMUtils.getTransitionError(FileSM.ACTION_EXPORT_FILETRANSFER, log);
				} else {
					transition = FileSMUtils.getTransitionOK(FileSM.ACTION_EXPORT_FILETRANSFER, log);
                }
            } else if (FileTransferAgent.FILEXFERHTTPUPLOADREQUEST.equals(requestName)) {
                if (resp_state == MRRequest.STATE_EXCEPTION) {
                    transition = FileSMUtils.getTransitionError(FileSM.ACTION_IMPORT_FILETRANSFER, log);
                } else {
                    transition = FileSMUtils.getTransitionOK(FileSM.ACTION_IMPORT_FILETRANSFER, log);
                }
           } else if (FileSM.PUBLISH_IMPORT_FILE_STATUS.equals(requestName)) {
                    if (resp_state == MRRequest.STATE_EXCEPTION) {
                        transition = FileSMUtils.getTransitionError(FileSM.ACTION_IMPORT_UNPACK, log);
                    } else {
                        transition = FileSMUtils.getTransitionOK(FileSM.ACTION_IMPORT_UNPACK, log);
               }
              
            }else if ("PublishFileRequest".equals(requestName) && FileSM.TYPE_OFFLOAD.equals(type)) {
            	if (resp_state == MRRequest.STATE_EXCEPTION) {
            		transition = FileSMUtils.getTransitionError(FileSM.ACTION_OFFLOAD_PACK, log);
                } else {
                	 transition = FileSMUtils.getTransitionOK(FileSM.ACTION_OFFLOAD_PACK, log);
                }
            } else if ("OffloadStateRequest".equals(requestName) && FileSM.TYPE_OFFLOAD.equals(type)) {
            	if (resp_state == MRRequest.STATE_EXCEPTION) {
            		transition = FileSMUtils.getTransitionError(FileSM.ACTION_OFFLOAD_FILETRANSFER, log);
                } else {
                	 transition = FileSMUtils.getTransitionOK(FileSM.ACTION_OFFLOAD_FILETRANSFER, log);
                }
            } else {
                if (FileSM.EXPORT_IMPORT_FILE_STATUS_REQUEST.equals(requestName)) {
                    if (resp_state == MRRequest.STATE_EXCEPTION) {
                        return null;
                    } else {
                        transition = FileSMUtils.getTransitionOK(FileSM.ACTION_DMSSTATUS, log);
                    }
                }
            }
        }
        if (transition == null) {
            log.addInfo("FileSM:getTransitionFromEvent: (transition==null) ?? requestName :='" + requestName + "'", MRLog.INFO, MRLog.NOTIFY);
        }else{
        	log.addInfo("FileSM:getTransitionFromEvent: transition :="+ transition+" requestName :='" + requestName + "'", MRLog.INFO, MRLog.NOTIFY);
        }
        return transition;
    }

    public static String getActionDescription(String action, MRLog log) {
        try {
            String xpath = "FileSMAction[@action='" + action + "']/@desc";
            String val = MRXMLUtils.getAttributeValueXpath(FileSMQueue.getInstance().getFileStateMachineList(log).getRootElement(), xpath, "").trim();
            if (!val.equals("")) {
                return val;
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return "FileSM action not found for action=" + action;
    }

    public static String getTransition(String action, String actionResult, MRLog log) throws Exception {
        try {
            //actionResult=ok, error or something else
            String xpath = "FileSMAction[@action='" + action + "']/@" + actionResult;
            String val = MRXMLUtils.getAttributeValueXpath(FileSMQueue.getInstance().getFileStateMachineList(log).getRootElement(), xpath, "").trim();
            if (!val.equals("")) {
                return val;
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return "FileSM action not found for action=" + action;
    }

    public static String getTransitionOK(String action, MRLog log) throws Exception {
        return getTransition(action, "ok", log);
    }

    public static String getTransitionError(String action, MRLog log) throws Exception {
        return getTransition(action, "error", log);
    }

    public static String getTransitionResume(String action, MRLog log) throws Exception {
        return getTransition(action, "resume", log);
    }

    public static boolean isDMSStatusCheckNeeded(String type, String action) {
        if (FileSM.ACTION_READY.equals(action)) {
            return false;       
        } else if (FileSM.ACTION_EXPORT_PACK.equals(action)) {
            return true;
        } else if (FileSM.ACTION_EXPORT_FILETRANSFER.equals(action)) {
            return false;
        } else if (FileSM.ACTION_COMPLETED.equals(action)) {
            return false;
        } else if (FileSM.ACTION_IMPORT_GETDMSBLOBPATH.equals(action)) {
            return true;
        } else if (FileSM.ACTION_IMPORT_FILETRANSFER.equals(action)) {
            return false;
        } else if (FileSM.ACTION_IMPORT_UNPACK.equals(action)) {
            return true;
        } else if (FileSM.ACTION_IMPORT_DBFILETRANSFER.equals(action)) {
            return true;
        } else if (FileSM.ACTION_IMPORT_DB.equals(action)) {
            return false;
        } else if (FileSM.ACTION_IMPORT_CLEANUPTEMPFILES.equals(action)) {
            return true;
        } else if (FileSM.STATE_COMPLETED.equals(action)) {
            return false;
        }
        return false;
    }
    
    public static Element readQueueElement_db_whereXpath(MRRequest request, MRDB db, MRLog log, String[] arrWhereXpath) throws Exception {
        Element el = null;
        //get Object MRConfig
        String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] sortByXpath = null;
        String selectXML = null;
        String sortOrder = null;
        //do db read
        el = MRXMLDBUtils.readSingle(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, null);
        return el;
    }
    
    public static boolean isDMSStatusCheckNeeded_ft(String type, String action) {
        if (FileSM.ACTION_EXPORT_FILETRANSFER.equals(action)) {
            return true;
        } else if (FileSM.ACTION_IMPORT_FILETRANSFER.equals(action)) {
            return true;
        }
        return false;
    }
    public static boolean isDMSStatusCheckNeeded_pack_unpack(String type, String action) {
        if (FileSM.ACTION_EXPORT_PACK.equals(action)) {
            return true;
        } else if (FileSM.ACTION_IMPORT_UNPACK.equals(action)) {
            return true;
        }
        return false;
    }
    public static String getResponseHandler(String type) {
        if (type.equals(FileSM.TYPE_EXPORT)) {
            return AddExportFileRequestHandler.class.getName();
        } else if (type.equals(FileSM.TYPE_IMPORT)) {
            return AddExportFileRequestHandler.class.getName();
        } else if (type.equals(FileSM.TYPE_DELETE)) {
            return DeleteExportImportFileRequestHandler.class.getName();
        } else if (type.equals(FileSM.TYPE_OFFLOAD)) {
            return AddOffloadRequestHandler.class.getName();
        }  
        return AddExportFileRequestHandler.class.getName();
    }

    ///////// send agent message //////////
    public static void sendAgentRequestToDMS(String requestName, Element queueEL, MRResponseHandlerI responseHandler, MRRequest request2, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    	String type = MRXMLUtils.getAttributeValue(queueEL, "type", "").trim();
    	sendAgentRequestToDMS(requestName, queueEL, responseHandler, request2, xmpp, log, db, resourceFactory, serviceAgentFactory, getResponseHandler(type));
    }

    public static void sendAgentRequestToDMS(String requestName, Element queueEL, MRResponseHandlerI responseHandler, MRRequest request2, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String responseHandlerName) throws Exception {
        // Send the getDirFTPPath request to the MediaStoreAdminServerAgent
        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
        String toAgentJID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@JID", "").trim();//dirAgentJID;
        String toAgentName = MRConst.MEDIA_STORE_ADMIN_SERVICE;
        MRRequest request = request2.clone();
        request.setRequestType(requestName);
        if (!FileSMUtils.isUserPresenceAvailable(xmpp, toAgentJID, log, db)) {
            log.addInfo("FileSM:sendAgentRequestToDMS ERROR: User " + toAgentJID + " is not available request:" + requestName + " ", MRLog.OK, MRLog.NOTIFY);
            throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + toAgentJID + "</User></error>");
        } 
        String dataStr = "<data>" + queueEL.asXML() + "</data>";
        AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fromAgentName, toAgentJID, toAgentName, dataStr, responseHandlerName);
    }
    
    public static void sendAgentRequestToOwner(String requestName, Element queueEL, MRResponseHandlerI responseHandler, MRRequest request2, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        // Send the getDirFTPPath request to the MediaStoreAdminServerAgent
	
        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
        String toAgentJID = MRXMLUtils.getAttributeValue(queueEL, "ownerJID", "").trim();
        String toAgentName = FileSM.OWNER_AGENT_CLASS_NAME;
        MRRequest request = request2.clone();
        request.setRequestType(requestName);
        log.addInfo("FileSM:sendAgentRequestToOwner User " + toAgentJID + " is not available request:" + requestName + " Responsehandle name: "+responseHandlerName, MRLog.OK, MRLog.NOTIFY);
        if (!FileSMUtils.isUserPresenceAvailable(xmpp, toAgentJID, log, db)) {
            log.addInfo("FileSM:sendAgentRequestToOwner ERROR: User " + toAgentJID + " is not available request:" + requestName + " ", MRLog.OK, MRLog.NOTIFY);
            throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + toAgentJID + "</User></error>");
        } 
        String dataStr = "<data>" + queueEL.asXML() + "</data>";
        AppServerAgent.sendAgentRequestForUserRequest(request, xmpp, log, db, resourceFactory, serviceAgentFactory, fromAgentName, toAgentJID, toAgentName, dataStr, responseHandlerName);
    }
    
    public static void sendAgentEventToOwner(boolean done, Element queueEL, MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        try {
            String userJID = MRXMLUtils.getAttributeValue(queueEL, "ownerJID", "").trim();
            String type = MRXMLUtils.getAttributeValue(queueEL, "type", "").trim();
            String action = FileSM.EXPORT_IMPORT_FILE_EVENT;
            if (!FileSMUtils.isUserPresenceAvailable(xmpp, userJID, log, db)) {
                log.addInfo("FileSM:sendAgentEventToOwner ERROR: User is not available=" + userJID, MRLog.OK, MRLog.NOTIFY);
                //throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + userJID + "</User></error>");
                return;
            }

            int eventType = 0;
            if (FileSM.TYPE_EXPORT.equals(type)) {
                if (done) {
                    eventType = FileSM.EXPORT_FILE_COMPLETE_EVENT_CODE;
                } else {
                    eventType = FileSM.EXPORT_FILE_ERROR_EVENT_CODE;
                }
            } else if (FileSM.TYPE_IMPORT.equals(type)) {
                if (done) {
                    eventType = FileSM.IMPORT_FILE_COMPLETE_EVENT_CODE;
                } else {
                    eventType = FileSM.IMPORT_FILE_ERROR_EVENT_CODE;
                }
            }else if (FileSM.TYPE_OFFLOAD.equals(type)) {
                if (done) {
                    eventType = FileSM.OFFLOAD_FILE_COMPLETE_EVENT_CODE;
                } else {
                    eventType = FileSM.OFFLOAD_FILE_ERROR_EVENT_CODE;
                }
            }
            StringBuffer data = new StringBuffer();
            data.append("<data>");
            data.append("<action>" + action + "</action>");
            data.append("<eventCode>" + action + "</eventCode>");
            data.append(queueEL.asXML());
            data.append("</data>");
            /*
            <data>
                <MediaStreamDirNID>default.mediastreamdirresourcelist.msdrl_fff6173c-8ad1-4bed-89d6-bec9aa7f249a</MediaStreamDirNID>
                <action>EmptyDirProcess</action>
                <eventCode>1001</eventCode>
            </data>
            */
            String info = action + " > AgentEvent Message FileSM:action=" + action + " eventType=" + eventType + "  to " + FileSM.OWNER_AGENT_CLASS_NAME;
            ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, userJID, MRConst.OPERATOR_SERVICE, action, MRXMLUtils.stringToElement(data.toString()), info);            
            
        } catch (Exception ex) {

        }
    }

    public static void handleStartTransfer(String file_transfer_request, String fileXferID, String fromAgentJID, String fromFilename, String toAgentJID, String toFilename, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
        fileXferID = fileXferID + "__" + file_transfer_request + "__";
        //setup file transfer 
        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
        String toAgentName = SystemServiceConst.FILETRANSFERAGENT;
        String responseHandlerName = FileSMUtils.responseHandlerName;//this.responseHandlerName;
        if (!FileSMUtils.isUserPresenceAvailable(xmpp, toAgentJID, log, db)) {
            log.addInfo("FileSM:handleStartTransfer ERROR: User not available " + toAgentJID + " " + " from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + toFilename + " fileXferID=" + fileXferID, MRLog.OK, MRLog.NOTIFY);
            throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + toAgentJID + "</User></error>");
        }
        if (!FileSMUtils.isUserPresenceAvailable(xmpp, fromAgentJID, log, db)) {
            log.addInfo("FileSM:handleStartTransfer ERROR: User " + fromAgentJID + " is not available. " + " from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + toFilename + " fileXferID=" + fileXferID, MRLog.OK, MRLog.NOTIFY);
            throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + fromAgentJID + "</User></error>");
        }
        MRLog.println("FileSM:handleStartTransfer start: from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + toFilename + " fileXferID=" + fileXferID);
        Document requestDoc = MRXMLUtils.loadXML(com.ipvs.systemservice.impl.SystemServiceConst.FILEXFERSETUPREQUESTFILE, log);
        MRXMLUtils.setValueXpath(requestDoc, "//fromAgentJID", fromAgentJID);
        MRXMLUtils.setValueXpath(requestDoc, "//fromFileName", fromFilename);
        MRXMLUtils.setValueXpath(requestDoc, "//FileXferID", fileXferID);
        MRXMLUtils.setValueXpath(requestDoc, "//toAgentJID", toAgentJID);
        MRXMLUtils.setValueXpath(requestDoc, "//toFileName", toFilename);
        String requestName = MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
        MRRequest ftpRequest = MRRequest.createRequest(request, requestDoc, requestName, request.getServiceName());
        Element data = MRXMLUtils.getElementXpath(requestDoc, ftpRequest.getRequestDataXPath());
        String dataStr = MRXMLUtils.elementToString(data);
        String info = "FileSM::sendStartTransfer from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + toFilename + " fileXferID=" + fileXferID;
        String serviceName = MRConst.FTP_SERVICE;
        Document userRequestDoc = request.marshalRequestObject(log, responseHandlerName);
        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
        ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, toAgentJID, toAgentName, requestName, dataStr, info, request.getNID(), request.getUserJID(), serviceName, clientData);
    }
    public static void handleStartHttpTransfer(String file_transfer_request, String queueID, String fromAgentJID, String fromFilename, String toAgentJID, String url, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
       
        //setup file http transfer 
        String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
        String toAgentName = SystemServiceConst.FILETRANSFERAGENT;
        String responseHandlerName = FileSMUtils.responseHandlerName;
        if (!FileSMUtils.isUserPresenceAvailable(xmpp, toAgentJID, log, db)) {
            log.addInfo("FileSM:handleStartHttpTransfer ERROR: User not available " + toAgentJID + " " + " from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + url + " queueID=" + queueID, MRLog.OK, MRLog.NOTIFY);
            throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + toAgentJID + "</User></error>");
        }
        if (!FileSMUtils.isUserPresenceAvailable(xmpp, fromAgentJID, log, db)) {
            log.addInfo("FileSM:handleStartHttpTransfer ERROR: User " + fromAgentJID + " is not available. " + " from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + url + " queueID=" + queueID, MRLog.OK, MRLog.NOTIFY);
            throw new MRException(AMExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + fromAgentJID + "</User></error>");
        }
        MRLog.println("FileSM:handleStartHttpTransfer start: from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + url + " queueID=" + queueID);
        Document requestDoc = MRXMLUtils.loadXML(com.ipvs.systemservice.impl.SystemServiceConst.FILEXFERHTTPUPLOADREQUESTFILE, log);
        
        Element filexHttpUploadrequestEl= MRXMLUtils.getElementXpath(requestDoc, "//"+FileTransferAgent.FILEXFERHTTPUPLOADREQUEST+"Data");        
        MRXMLUtils.addAttribute(filexHttpUploadrequestEl, "byNID", queueID);             
        MRXMLUtils.setValueXpath(requestDoc, "//filePath", fromFilename);     
        MRXMLUtils.setValueXpath(requestDoc, "//url", url);
        
        String requestName = MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
        MRRequest ftpRequest = MRRequest.createRequest(request, requestDoc, requestName, request.getServiceName());
        Element data = MRXMLUtils.getElementXpath(requestDoc, ftpRequest.getRequestDataXPath());
        String dataStr = MRXMLUtils.elementToString(data);
        String info = "FileSM::sendHttpStartTransfer from=" + fromAgentJID + ":" + fromFilename + " to=" + toAgentJID + ":" + url + " queueID=" + queueID;
        String serviceName = MRConst.FTP_SERVICE;
        Document userRequestDoc = request.marshalRequestObject(log, responseHandlerName);
        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
        String deprecated = MRXMLUtils.getAttributeValue(MRXMLUtils.getElement(requestDoc, "Header"), MRConst.DEPRECATED, "false");       
        ServiceAgentUtils.sendDeprecatedAgentRequestMessage(xmpp, log, "", fromAgentName, toAgentJID, toAgentName, requestName, dataStr, info, request.getNID(), request.getUserJID(), serviceName, clientData, deprecated);        
    }
    public static String handleStandardMediaFileTransfer(String action, Element queueEL, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
    	Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
		// Queue up the standard format file for Transfer..
    	String fileXferIDSuffix = MRXMLUtils.getAttributeValue(internalEL, "title", "");
    	String fileXferID = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
    	String fromFilename = FileSMUtils.getDMSFilePath_export_standard(queueEL);
    	String fromAgentJID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@JID", "");
    	String toFilename = FileSMUtils.getClientFilePath_export_standard(queueEL);
    	String toAgentJID = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@JID", "");
    	fileXferIDSuffix = fileXferIDSuffix.replaceAll("[\\-\\+\\.\\^\\ :,]", "_");
    	//setup file transfer 
    	FileSMUtils.handleStartTransfer(action + "__" + fileXferIDSuffix, fileXferID, fromAgentJID, fromFilename, toAgentJID, toFilename, request, xmpp, log, db);
    	return null; 
    }

    @SuppressWarnings("unchecked")
    public static void cleanupDMSTempFolder(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler, String responseHandlerName) throws Exception {
        //send message to each DMS to delete all temp files
        String objectName = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectName, log);
        Document docObject = mrConfig.getXmlRepresentation(log).getDoc();
        Element queueEL = docObject.getRootElement().createCopy();

        Document doc = MRXMLUtils.getDocument();
        MRXMLUtils.addElement(doc, "DBResponse");
        Element dbReadRootEL = MRXMLUtils.addElement(doc.getRootElement(), "DBRead");
        FileSMUtils.getLoggedInDMSJIDS(dbReadRootEL, request, xmpp, log, db);
        Hashtable<String, String> ownerJIDDone = new Hashtable<String, String>();
        List<Element> list = (List<Element>) MRXMLUtils.getList(dbReadRootEL, "Owner");
        for (Element element : list) {
            try {
        	log.addInfo("DMS list owner Element is: "+element.asXML(), MRLog.OK, MRLog.NOTIFY);
                String ownerJID = MRXMLUtils.getAttributeValue(element, "userID", "");
                String dirNID = MRXMLUtils.getAttributeValue(element, "NID", "");
                if (!ownerJID.equals("") && FileSMUtils.isUserPresenceAvailable(xmpp, ownerJID, log, db) && !ownerJIDDone.contains(ownerJID)) {
                    //if (!ownerJIDDone.contains(ownerJID)) {
                    ownerJIDDone.put(ownerJID, ownerJID);
                    MRXMLUtils.setAttributeValueXPath(queueEL, "Internal", "JID", ownerJID);
                    MRXMLUtils.setAttributeValueXPath(queueEL, "Internal", "dirNID", dirNID);
                    //FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_IMPORT_CLEANUP_TEMP_FILES, queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandlerName);
                    FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_IMPORT_ABORT_TEMP_FILE, queueEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandlerName);
                    
                    //}
                }
            } catch (Exception ex) {
                log.addInfo("FileSMQueue:cleanupDeleted Error " + element.asXML(), MRLog.INFO, MRLog.NOTIFY);
                ex.printStackTrace();
            }
        }
    }

    //////////////////// async events ////////////////////
    public static boolean isExportImportFileTransferID(String fileXferID) {
        String[] arr = (fileXferID + "__" + " " + "__").split("__");
        String queueNID = arr[0].trim();
        String requestName = arr[1].trim();
        if (!queueNID.equals("") && !requestName.equals("")) {
            return true;
        }
        return false;
    }

    public static void cleanupTempFiles(Element queueEL) {
        try {
            String type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
            String nid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");

            String tarFile = "";
            if (type.equals(FileSM.TYPE_EXPORT) || type.equals(FileSM.TYPE_OFFLOAD)) {
                tarFile = getMSDBFilePath_export(queueEL);
            } else {
                tarFile = getMSDBFilePath_import(queueEL);
            }
            File file = new File(tarFile);
            file.delete();
            if (!nid.equals("")) {
                //DB_TEMP_FOLDER
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    ////////////////////// next transition ////////////////////////

    ////////////////////// set state ////////////////////////

    //////////// file path for DMS work area ////////////////////
    public static String getDMSFileMediaFolderPath(Element queueEL, String rootFolderPath) throws Exception {
        String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "");
        File dir = new File(rootFolderPath, "blob/" + uuid);
        return dir.getAbsolutePath();
    }

    public static String getDMSWorkAreaPath(Element queueEL, String rootFolderPath) throws Exception {
        String type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
        String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "");
        File workDir = null;
        if (type.equalsIgnoreCase(FileSM.TYPE_EXPORT) || type.equalsIgnoreCase(FileSM.TYPE_OFFLOAD)) {
        	uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_UUID", "");
            workDir = new File(getDMSWorkAreaPath_ExportRoot(rootFolderPath));
        } else if (type.equalsIgnoreCase(FileSM.TYPE_IMPORT)) {
            workDir = new File(getDMSWorkAreaPath_ImportRoot(rootFolderPath));
        }
        File workDir_uuid = new File(workDir, uuid);
        workDir_uuid.mkdirs();
        return workDir.getAbsolutePath();
    }

    public static String getDMSWorkAreaPath_ExportRoot(String rootFolderPath) throws Exception {
        File workDir = new File(IPVSConst.getInstance().getIpvsDMSExportFile());
        workDir.mkdirs();
        return workDir.getAbsolutePath();
    }

    public static String getDMSWorkAreaPath_ImportRoot(String rootFolderPath) throws Exception {
        File workDir = new File(IPVSConst.getInstance().getIpvsDMSImportFile());
        workDir.mkdirs();
        return workDir.getAbsolutePath();
    }

    //////////// file path for DB file on ms////////////////////
    public static String getMSDBFilePath_export(Element queueEL) throws Exception {
        String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_fileNID", "");
        return IPVSConst.getInstance().getIpvsDbExportFile() + "/" + fileNID + DB_TAR_EXTENSION;
    }

    public static String getMSDBFilePath_import(Element queueEL) throws Exception {
        String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
        return IPVSConst.getInstance().getIpvsDbImportFile() + "/" + fileNID + DB_TAR_EXTENSION;
    }

    //////////// file path for DB file on dms////////////////////
    public static String getDMSDBFilePath_export(Element queueEL) throws Exception {
        String tempFolderPath = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@tempFolderPath", "");
        String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_UUID", "");
        String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_fileNID", "");
        return tempFolderPath + "/" + uuid + "/" + fileNID +  DB_TAR_EXTENSION;
    }

    public static String getDMSDBFilePath_import(Element queueEL) throws Exception {
        String tempFolderPath = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@tempFolderPath", "");
        String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "");
        String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
        return tempFolderPath + "/" + uuid + "/" + fileNID +   DB_TAR_EXTENSION;
    }

    //////////// file path for media tar file on client////////////////////
    public static String getClientFilePath_export(Element queueEL) throws Exception {
        Element externalEL = MRXMLUtils.getElement(queueEL, "External");
        Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
        //String filename = MRXMLUtils.getAttributeValue(externalEL, "fileName", "");
        String newFileName = MRXMLUtils.getAttributeValue(internalEL, "title", "") + "_" + MRXMLUtils.getAttributeValue(internalEL, "new_UUID", "");
        newFileName = newFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_");
        String folderPath = MRXMLUtils.getAttributeValue(externalEL, "folderPath", "");
        //return folderPath + "/" + filename;
        return folderPath + "/" + newFileName + FileSMUtils.MEDIA_TAR_EXTENSION;
    }
    
	// //////////file path for standard media zip file on client////////////////////
	public static String getClientFilePath_export_standard(Element queueEL) throws Exception {
		Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
		Element externalEL = MRXMLUtils.getElement(queueEL, "External");
		String newFileName = MRXMLUtils.getAttributeValue(internalEL, "title", "") + "_" + MRXMLUtils.getAttributeValue(internalEL, "new_UUID", "");
		newFileName = newFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_");
		String folderPath = MRXMLUtils.getAttributeValue(externalEL, "folderPath", "");
		return folderPath + "/" + newFileName + FileSMUtils.MEDIA_STANDARD_ZIP_EXTENSION;
	}

    public static String getClientFilePath_import(Element queueEL) throws Exception {
        Element externalEL = MRXMLUtils.getElement(queueEL, "External");
        String filename = MRXMLUtils.getAttributeValue(externalEL, "fileName", "");
        String folderPath = MRXMLUtils.getAttributeValue(externalEL, "folderPath", "");
        return folderPath + "/" + filename;
    }

    //////////// file path for media tar file on dms////////////////////
    public static String getDMSFilePath_export(Element queueEL) throws Exception {
        Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
        String tempFolderPath = MRXMLUtils.getAttributeValue(internalEL, "tempFolderPath", "");
        String uuid = MRXMLUtils.getAttributeValue(internalEL, "new_UUID", "");
        return tempFolderPath + "/" + uuid + MEDIA_TAR_EXTENSION;
    }
    
	// //////////file path for standard media zip file on dms////////////////////
	public static String getDMSFilePath_export_standard(Element queueEL) throws Exception {
		Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
		String tempFolderPath = MRXMLUtils.getAttributeValue(internalEL,
				"tempFolderPath", "");
		return tempFolderPath + "/standard" + MEDIA_STANDARD_ZIP_EXTENSION;
	}

    public static String getDMSFilePath_import(Element queueEL) throws Exception {
        Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
        String tempFolderPath = MRXMLUtils.getAttributeValue(internalEL, "tempFolderPath", "");
        String uuid = MRXMLUtils.getAttributeValue(internalEL, "UUID", "");
        String externalFileName = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@fileName", "");
        if(externalFileName.endsWith(MEDIA_TAR_EXTENSION_2DOT4)){
            return tempFolderPath + "/" + uuid + MEDIA_TAR_EXTENSION_2DOT4;
        }
        return tempFolderPath + "/" + uuid + MEDIA_TAR_EXTENSION;
    }

    public static String getFileExtension(String srcPath) throws Exception {
        if (srcPath.indexOf('.') != -1) {
            return "." + srcPath.substring(srcPath.lastIndexOf('.'));
        }
        return "";
    }

    public static void cleanupLocalAllTemp(MRLog log) throws Exception {
        try {
            //delete export temp folder
            String tempFolderPath = IPVSConst.getInstance().getIpvsDbExportFile();
            File temp_folder = new File(tempFolderPath);
            del_folder(log, temp_folder);
            //delete export temp folder
            tempFolderPath = IPVSConst.getInstance().getIpvsDbImportFile();
            temp_folder = new File(tempFolderPath);
            del_folder(log, temp_folder);
        } catch (Exception ex) {
            log.addInfo("FileSMQueue:cleanupLocalAllTemp Error "+ex.toString(), MRLog.INFO, MRLog.NOTIFY);
            ex.printStackTrace();
        }
    }

    public static void del_folder(MRLog log, File temp_folder) throws Exception {
        try {
            if (temp_folder.exists()) {
                if (IPVSConst.getInstance().getOS().equalsIgnoreCase("windows")) {
                    deleteDirectory(temp_folder);
                } else {
                    StringBuffer buf = new StringBuffer();
                    String[] cmd = { "rm", "-rf", temp_folder.getAbsolutePath() };
                    try {
                        SystemServiceUtils.executeProcess(cmd, buf);
                    } catch (Exception ex) {
                        log.addInfo("FileSMQueue:cleanupLocalAllTemp Error " + joinArray(cmd, " "), MRLog.INFO, MRLog.NOTIFY);
                        ex.printStackTrace();
                    }
                }
            }

            temp_folder.mkdirs();
        } catch (Exception ex) {
            log.addInfo("FileSMQueue:cleanupLocalAllTemp Error "+ex.toString(), MRLog.INFO, MRLog.NOTIFY);
            ex.printStackTrace();
        }
    }

    //////////// file path for import/export end////////////////////
    ///////////////// export import db /////////////////////////////////////////

    public static Document exportDBToTarFile(Element queueEL, MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
    //    Document respdoc = null;
        String parentMediaDirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
        String new_fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_fileNID", "");
        String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
        String new_UUID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_UUID", "");
        String path = IPVSConst.getInstance().getIpvsDbExportFile();
        StringBuffer buf = new StringBuffer();
        buf.append("<" + AMConst.EXPORT_MEDIA_FILE + ">");
        buf.append("<params>");
        buf.append("<mediaStreamFileResourceNID>" + fileNID + "</mediaStreamFileResourceNID>");
        buf.append("<exportMediaStreamFileResourceNID>" + new_fileNID + "</exportMediaStreamFileResourceNID>");
        buf.append("<UUID>" + new_UUID + "</UUID>");
        buf.append("<parentMediaDirNID>" + parentMediaDirNID + "</parentMediaDirNID>");
        buf.append("</params>");
        buf.append("<zipFolderPath>" + path + "</zipFolderPath>");
        buf.append(queueEL.asXML());
        buf.append("</" + AMConst.EXPORT_MEDIA_FILE + ">");
        long timeout = 900 * 1000;
        return MRXMLDBUtils.exportDB(timeout, request, db, log, AMConst.EXPORT_MEDIA_FILE, buf.toString());
    }

    public static String getFileNID(MRDB db, MRLog log) throws Exception {
        MRConfig mrconfig = MRConfig.getInstance(AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
        String parentNID = db.getServiceDomainRootNID(mrconfig.getParentNIDKey(log));
        return IpvsDBUtils.getAutoNID(parentNID);
    }

    public static Document importTarFileInToDB(Element queueEL, MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
       // Document respdoc = null;
        String path = IPVSConst.getInstance().getIpvsDbImportFile();
        long timeout = 900 * 1000;
        String parentMediaDirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
        String UUID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "");
        String readquery_id = "readquery_groupid_mapping";
        String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
        StringBuffer buf = new StringBuffer();
        buf.append("<" + AMConst.IMPORT_MEDIA_FILE + ">");
        buf.append("<params>");
        buf.append("<mediaStreamFileResourceNID>" + fileNID + "</mediaStreamFileResourceNID>");
        buf.append("<parentMediaDirNID>" + parentMediaDirNID + "</parentMediaDirNID>");
        buf.append("<UUID>" + UUID + "</UUID>");
        buf.append("<readquery_id>" + readquery_id + "</readquery_id>");
        buf.append("</params>");
        buf.append("<zipFolderPath>" + path + "</zipFolderPath>");
        buf.append(queueEL.asXML());
        buf.append("</" + AMConst.IMPORT_MEDIA_FILE + ">");
        return MRXMLDBUtils.importDB(timeout, request, db, log, AMConst.IMPORT_MEDIA_FILE, buf.toString());
    }

    //////////// file path for import/export start////////////////////

    //////////// db read/updates ////////////////////
    public static void getLoggedInDMSJIDS(Element appendToParentElement, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
        String objectType = AMConst.MEDIA_STREAM_DIR_RESOURCE;
        MRConfig mrcConfig = MRConfig.getInstance(objectType, log);
        String listName = mrcConfig.getParentNIDKey(log);

        String[] arrWhereXpath = { "State/StateFlag !='Offline' and Info/Title!=('PVR','RECYCLEBIN')" };
        String[] sortByXpath = null;
        String sortOrder = null;
        String selectXML = "<Owner NID='{data($VAR/@NID)}' userID='{data($VAR/Info/Properties/Owner)}'  > </Owner>";
        String childXPath = null;
        String parentNID = null;
        int countToFetch = -1;
        MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, parentNID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, appendToParentElement, 0, countToFetch);
    }

    public static void setFileStateReady(Element queueEL, MRResponseHandlerI responseHandler, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        
    	Element el = FileSMUtils.readMediaStreamFile_db(queueEL, request, db, log);
        String cliptType = MRXMLUtils.getValueXpath(el, "//MediaStoreFileInfo/Type", "").trim();
        if(cliptType.equals(MediaRoomServiceConst.PVRMEDIACLIP)) {
        	FileSMUtils.resetFileStateMsg_db(el, "", request, db, log);
        } else { 
        	FileSMUtils.setFileStateFlag_db(el, "Ready", "", request, db, log);
        }
    }

    public static void readNextInQueue(Element appendToParentElement, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
        String[] arrWhereXpath = { "@queue_state > -1 and @state != 'completed' " };
        String[] sortByXpath = { "$VAR/@priority", "$VAR/@dateCreated", "$VAR/@queue_state" };
        String sortOrder = "ascending";
        String selectXML = "";
        String childXPath = "";
        String NID = null;
        int countToFetch = 1;
        MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, getListName(log), objectType, NID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, appendToParentElement, 0, countToFetch);
    }

    public static void getCurrentInQueue(Element appendToParentElement, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
        String[] arrWhereXpath = { "@queue_state=" + FileSMQueue.QUEUE_STATUS_RUNNING + " " };
        String selectXML = "";
        String childXPath = "";
        String NID = null;
        int countToFetch = -1;
        MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, getListName(log), objectType, NID, childXPath, arrWhereXpath, null, "", selectXML, appendToParentElement, 0, countToFetch);
    }

    public static void getClearQueueCount(Element appendToParentElement, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
        String[] arrWhereXpath = { "@queue_state=('" + FileSMQueue.QUEUE_STATUS_RUNNING + "','" + FileSMQueue.QUEUE_STATUS_ERROR + "' ,'" + FileSMQueue.QUEUE_STATUS_ERROR_RESUME + "')" };
        String[] sortByXpath = { "$VAR/@dateCreated" };
        String sortOrder = "descending";
        String selectXML = "";
        String childXPath = "";
        String NID = null;
        int countToFetch = 1;
        MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, getListName(log), objectType, NID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, appendToParentElement, 0, countToFetch);
    }
    
    public static Element getQueueElement(Element appendToParentElement, MRRequest request, XMPPI xmpp, MRLog log, MRDB db) throws Exception {
       	String[] arrWhereXpath = { "@queue_state > -1 and  (@state != 'completed' and @state != 'queued') " };
    	String[] sortByXpath = { "$VAR/@dateCreated" };
        String sortOrder = "";
        String selectXML = "";
        String childXPath = "";
        String NID = null;
        int countToFetch = -1;
       return MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, getListName(log), objectType, NID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, appendToParentElement, 0, countToFetch);
    }

    public static Element readMediaStreamFile_db(Element queueEL, MRRequest request, MRDB db, MRLog log) throws Exception {
        Element el = null;
        //get Object MRConfig
        String objectType = AMConst.MEDIA_STREAM_FILE_RESOURCE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] arrWhereXpath = { getFileWhereClause(queueEL) };
        String[] sortByXpath = null;
        String selectXML = null;
        String sortOrder = null;
        //do db read
        el = MRXMLDBUtils.readSingle(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, null);
        if (el != null) {
            Document doc = MRXMLUtils.getDocument();
            MRXMLUtils.addElement(doc, el);
        }
        return el;
    }

    public static String getMediaStreamDirJID_db(String dirNID, MRRequest request, MRDB db, MRLog log) throws Exception {
        Element el = readMediaStreamDir_db(dirNID, request, db, log);
        return MRXMLUtils.getValueXpath(el, "//Owner", "");
    }

    public static String getMediaStreamDirJID_db(Element fileEL, MRRequest request, MRDB db, MRLog log) throws Exception {
        Element el = readMediaStreamDir_db(fileEL, request, db, log);
        return MRXMLUtils.getValueXpath(el, "//Owner", "");
    }

    public static Element readMediaStreamDir_db(Element fileEL, MRRequest request, MRDB db, MRLog log) throws Exception {
        String dirNID = MRXMLUtils.getValueXpath(fileEL, "//ParentMediaDirNID", "");
        return readMediaStreamDir_db(dirNID, request, db, log);
    }

    public static Element readMediaStreamDir_db(String dirNID, MRRequest request, MRDB db, MRLog log) throws Exception {
        Element el = null;
        String objectType = AMConst.MEDIA_STREAM_DIR_RESOURCE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] arrWhereXpath = { "@NID='" + dirNID + "'" };
        String[] sortByXpath = null;
        String selectXML = null;
        String sortOrder = null;
        //do db read
        el = MRXMLDBUtils.readSingle(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, null);
        if (el != null) {
            Document doc = MRXMLUtils.getDocument();
            MRXMLUtils.addElement(doc, el);
        }
        return el;
    }

    public static void setFileStateFlag_db(Element el, String stateFlag, String stateMsg, MRRequest request, MRDB db, MRLog log) throws Exception {
        MRXMLUtils.replaceValueXpath(el, "//State/StateFlag", stateFlag);
        MRXMLUtils.replaceValueXpath(el, "//State/StateMsg", stateMsg);
        String NID = MRXMLUtils.getAttributeValue(el, "NID", "").trim();
        //get Object MRConfig
        String objectType = AMConst.MEDIA_STREAM_FILE_RESOURCE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        //update only state
        String childXpath = "//State";
        Element el2 = MRXMLUtils.getElementXpath(el, "//State");
        if(el2==null){
        	MRLog.println( "DB setFileStateFlag_db " + el + " "+ el.asXML() );        
        }
        String[] arrWhereXpath = null;
        //do db replace
        MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, NID, childXpath, arrWhereXpath, el2);
    }
    
    public static void resetFileStateMsg_db(Element el, String stateMsg, MRRequest request, MRDB db, MRLog log) throws Exception {
        MRXMLUtils.replaceValueXpath(el, "//State/StateMsg", stateMsg);
        String NID = MRXMLUtils.getAttributeValue(el, "NID", "").trim();
        //get Object MRConfig
        String objectType = AMConst.MEDIA_STREAM_FILE_RESOURCE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        //update only stateMsg
        String childXpath = "//State/StateMsg";
        Element el2 = MRXMLUtils.getElementXpath(el, "//State/StateMsg");
        if(el2==null){
        	MRLog.println( "DB setFileStateMsg_db " + el + " "+ el.asXML() );        
        }
        String[] arrWhereXpath = null;
        //do db replace
        MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, NID, childXpath, arrWhereXpath, el2);
    }
    
    @SuppressWarnings("unchecked")
    public static void resetFileStateFlag_db(Document dbRead, String stateFlag, String stateMsg,String stateFlag_new, String stateMsg_new,  MRRequest request, MRDB db, MRLog log) throws Exception {
        String nids="";
        if(dbRead !=null){
            String update_state_xpath="//"+AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE+"[@queue_state='" + FileSMQueue.QUEUE_STATUS_ERROR 
            + "' or @queue_state='" + FileSMQueue.QUEUE_STATUS_ERROR_RESUME + "' or @queue_state='" + FileSMQueue.QUEUE_STATUS_RUNNING + "']";
            List<Element> list = (List<Element>) MRXMLUtils.getListXpath(dbRead, update_state_xpath);
            if(list !=null && list.size()>0){
                Vector<String> vec = new Vector<String>();
                for (Element element : list) {
                    String nid = MRXMLUtils.getAttributeValueXpath(element, "Internal/@fileNID", "").trim(); 
                    if(!nid.equals("")){
                        vec.add("'"+nid+"'");
                    }
                }
                if(vec.size()>0){
                    String[] array = new String[vec.size()];
                    vec.copyInto(array);
                    nids="@NID=("+FileSMUtils.joinArray(array, ",")+")";
                }
             }
        }
        //get Object MRConfig
        String objectType = AMConst.MEDIA_STREAM_FILE_RESOURCE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXpath = "//State";
        String[] arrWhereXpath = {"//State[StateFlag='"+stateFlag+"' and StateMsg='"+stateMsg+"']",nids};
        Element el =MRXMLUtils.getElementXpath(mrConfig.getXmlRepresentation(log).getDoc().getRootElement(),"//State").createCopy();
        MRXMLUtils.replaceValue(el, "StateFlag", stateFlag_new);
        MRXMLUtils.replaceValue(el, "StateMsg", stateMsg_new);
        //do db replace
        MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXpath, arrWhereXpath, el);
    }
    
    public static  void delete_queue(Element respRoot, String[] arrWhereXpath , String[] arrWhereXpathRead, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory) throws Exception {
        //get Object MRConfig
        String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;

        Document docRead = MRXMLUtils.getDocument();
        MRXMLUtils.addElement(docRead, "Response");
        Element dbRead = MRXMLUtils.addElement(docRead.getRootElement() , "ResponseXML");
        MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpathRead, null, null, null, dbRead, 0, -1);

        //do db delete
        Document doc = MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpath);
        if (doc != null && doc.getRootElement() != null) {
            MRXMLUtils.addChildElement(respRoot, doc.getRootElement().createCopy());
        }

        //will check if delete clean up is needed
        try {
            int counter=0;
            //could be busy
            while(!FileSMQueue.getInstance().cleanupDeleted(docRead, request, xmpp, log, db, resourceFactory, serviceAgentFactory, null)){
                counter++;
                if(counter>20){
                    break;
                }
                Thread.sleep(1000);
            }
            
        } catch (Exception ex) {
        }
        //try next to process
        FileSMQueue.getInstance().processQueue(request, xmpp, log, db, resourceFactory, serviceAgentFactory, null);


    }    
    @SuppressWarnings("unchecked")
    public static Document cleanupFile_importDB(Document dbRead, MRRequest request, MRDB db, MRLog log) throws Exception {
        Document respdoc = null;
        try {
            StringBuffer bufNIDS = new StringBuffer();
            if(dbRead !=null){
                String update_state_xpath="//"+AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE+"[@queue_state='" + FileSMQueue.QUEUE_STATUS_ERROR 
                + "' or @queue_state='" + FileSMQueue.QUEUE_STATUS_ERROR_RESUME + "' or @queue_state='" + FileSMQueue.QUEUE_STATUS_RUNNING + "']";
                List<Element> list = (List<Element>) MRXMLUtils.getListXpath(dbRead, update_state_xpath);
                if(list !=null && list.size()>0){
                    for (Element element : list) {
                        String type = MRXMLUtils.getAttributeValue(element, "type", "").trim(); 
                        if(type.equals(FileSM.TYPE_IMPORT)){
                            String fileNID = MRXMLUtils.getAttributeValueXpath(element, "Internal/@fileNID", "").trim(); 
                            if(!fileNID.equals("")){
                                bufNIDS.append("<mediaStreamFileResourceNID>" + fileNID + "</mediaStreamFileResourceNID>");
                            }
                        }
                    }
                 }
            }
            if(bufNIDS.length()==0){
                return null;
            }
            StringBuffer buf = new StringBuffer();
            buf.append("<" + AMConst.DELETE_MEDIA_FILE + ">");
            buf.append("<params>");
            buf.append(bufNIDS);
            buf.append("</params>");
            buf.append("</" + AMConst.DELETE_MEDIA_FILE + ">");
            long timeout = 900 * 1000;
            Document resp = MRXMLDBUtils.deleteDB(timeout, request, db, log, AMConst.DELETE_MEDIA_FILE, buf.toString());
            log.addInfo("FileSM:cleanupFile_db files=" + bufNIDS.toString()+" " + resp  , MRLog.OK, MRLog.NOTIFY);
        } catch (Exception ex) {
        }
        return respdoc;
         
    }  
    
    @SuppressWarnings("unchecked")
	public static Document cleanupDMSTempFiles(Document queueDoc, MRRequest request, XMPPI xmpp, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, MRResponseHandlerI responseHandler, String  responseHandlerName, MRLog log) throws Exception {
        Document respdoc = null;
        try {
        	
        	Hashtable<String, StringBuffer> dmsList = new Hashtable<String, StringBuffer>();
            if(queueDoc !=null){
                List<Element> list = (List<Element>) MRXMLUtils.getListXpath(queueDoc, "//"+AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
                if(list !=null && list.size()>0){
                    for (Element element : list) {
                    	String type = MRXMLUtils.getAttributeValue(element, "type", "");
                    	String format = MRXMLUtils.getAttributeValue(element, "format", "");
                        String new_fileUUID = MRXMLUtils.getAttributeValueXpath(element, "Internal/@new_UUID", "").trim();
                        String prev_fileUUID = MRXMLUtils.getAttributeValueXpath(element, "Internal/@UUID", "").trim();
                        String ownerJID = MRXMLUtils.getAttributeValueXpath(element, "Internal/@JID", "").trim();
                        String dirNID = MRXMLUtils.getAttributeValueXpath(element, "Internal/@dirNID", "").trim();
                        if(!ownerJID.isEmpty()) {
                        	if(dmsList.containsKey(ownerJID)) {
                        		StringBuffer buf = dmsList.get(ownerJID);
                        		if(buf == null)
                        			buf = new StringBuffer();
                        	
                        		buf.append("<FileInfo dirNID='"+ dirNID +"' type='" + type + "' format='"+ format +"'>");
                                buf.append("<NewUUID>" + new_fileUUID + "</NewUUID>");
                                buf.append("<OldUUID>" + prev_fileUUID + "</OldUUID>");
                                buf.append("</FileInfo>");
                        		
                        	} else {
                        		dmsList.put(ownerJID, new StringBuffer());
                        		
                        		dmsList.get(ownerJID).append("<FileInfo dirNID='"+ dirNID +"' type='" + type + "' format='"+ format +"'>");
                        		dmsList.get(ownerJID).append("<NewUUID>" + new_fileUUID + "</NewUUID>");
                        		dmsList.get(ownerJID).append("<OldUUID>" + prev_fileUUID + "</OldUUID>");
                        		dmsList.get(ownerJID).append("</FileInfo>");
                        		
                        	}
                        }
                        
                        
                    }
                 }
            }
            
            StringBuffer bufNIDs = null;
            for (String jid : dmsList.keySet()) {
				
            	if(!jid.isEmpty() && FileSMUtils.isUserPresenceAvailable(xmpp, jid, log, db)) {
				
					bufNIDs = dmsList.get(jid);
					if(bufNIDs != null) {
						StringBuffer buf = new StringBuffer();
						buf.append("<AbortFileUUIDList>");
			            buf.append("<Internal JID='" + jid + "'>");
			            buf.append(bufNIDs);
			            buf.append("</Internal>");
			            buf.append("</AbortFileUUIDList>");
			            Element dataEL = MRXMLUtils.stringToElement(buf.toString());
			            FileSMUtils.sendAgentRequestToDMS(FileSM.EXPORT_IMPORT_ABORT_TEMP_FILE, dataEL, responseHandler, request, xmpp, log, db, resourceFactory, serviceAgentFactory, responseHandlerName);
			            log.addInfo("FileSMUtils:cleanupDMSTempFiles for jid=" + jid + "FileUUIDs= " + bufNIDs.toString() , MRLog.OK, MRLog.NOTIFY);
					}
				
					
				}
			}
            
        } catch (Exception ex) {
        	ex.printStackTrace();
        }
        return respdoc;
         
    }
    
    public static void updateQueueElement_db(Element queueEL, MRRequest request, MRDB db, MRLog log) throws Exception {
        String NID = MRXMLUtils.getAttributeValue(queueEL, "NID", "").trim();
        //get Object MRConfig
        String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String childXpath = null;
        String[] arrWhereXpath = null;
        //do db replace
        MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, NID, childXpath, arrWhereXpath, queueEL);
    }

    public static Document dleteQueueElement_db(Element queueEL, String[] arrWhereXpath, MRRequest request, MRDB db, MRLog log) throws Exception {
        String NID = MRXMLUtils.getAttributeValue(queueEL, "NID", "").trim();
        String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String childXpath = null;
        return MRXMLDBUtils.delete(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, NID, childXpath, arrWhereXpath);
    }

    public static void resumeQueue_db(MRRequest request, MRDB db, MRLog log, XMPPI xmpp) throws Exception {
    	String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
    	MRConfig mrConfig = MRConfig.getInstance(objectType, log);
    	String listName = mrConfig.getParentNIDKey(log);
    	String parentNID = db.getServiceDomainRootNID(listName);  
    	//for file which is packing or unpacking, resume is ignored if DMS is not rebooted 
    	String childXpath = "//ExportImportFileListItem/@queue_state";
    	String[] arrWhereXpath = { "@queue_state='" + FileSMQueue.QUEUE_STATUS_ERROR + "'" };
    	//do db replace atrribute
    	MRXMLDBUtils.replaceAttribute(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, parentNID, childXpath, arrWhereXpath, String.valueOf(FileSMQueue.QUEUE_STATUS_ERROR_RESUME));

    	childXpath = "//ExportImportFileListItem/@state_description";
    	String[] arrWhereXpath2 = { "@state_description='" + getActionDescription(FileSM.ACTION_ERROR, log) + "'" };
    	//do db replace atrribute
    	MRXMLDBUtils.replaceAttribute(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, parentNID, childXpath, arrWhereXpath2,getActionDescription(FileSM.ACTION_RESUME, log));

    }

    public static void updateResumeQueueState_db(MRRequest request, MRDB db, MRLog log) throws Exception {
        String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String childXpath = "//ExportImportFileListItem/@queue_state";
        String[] arrWhereXpath = { "@queue_state='" + FileSMQueue.QUEUE_STATUS_ERROR + "'" };
        String parentNID = db.getServiceDomainRootNID(listName);
        //do db replace atrribute
        MRXMLDBUtils.replaceAttribute(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, parentNID, childXpath, arrWhereXpath, String.valueOf(FileSMQueue.QUEUE_STATUS_ERROR_RESUME));
    }

    public static Element readQueueElement_db(String nid, MRRequest request, MRDB db, MRLog log) throws Exception {
        Element el = null;
        //get Object MRConfig
        String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] arrWhereXpath = { "@NID='" + nid + "'" };
        String[] sortByXpath = null;
        String selectXML = null;
        String sortOrder = null;
        //do db read
        el = MRXMLDBUtils.readSingle(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, null);
        if (el != null) {
            Document doc = MRXMLUtils.getDocument();
            MRXMLUtils.addElement(doc, el);
        }
        return el;
    }

    public static String getWhereRequestParam(String field, String requestParam) {
        StringBuffer buf = new StringBuffer();
        if (requestParam.length() == 0) {
            return buf.toString();
        }
        String[] ret = requestParam.split(",");
        for (String element : ret) {
            String param = element.trim();
            if (!param.equals("")) {
                if (buf.length() > 0) {
                    buf.append(',');
                }
                buf.append("'" + param + "'");
            }
        }
        if (buf.length() > 0) {
            return field + "=(" + buf.toString() + ")";
        }
        return buf.toString();
    }

    public static String getFileWhereClause(Element queueEL) throws Exception {
        Element internalEL = MRXMLUtils.getChild(queueEL, "Internal");
        String fileNID = MRXMLUtils.getAttributeValue(internalEL, "fileNID", "");
        String title = MRXMLUtils.getAttributeValue(internalEL, "title", "");
        String dateCreated = MRXMLUtils.getAttributeValue(internalEL, "dateCreated", "");
        if (!fileNID.equals("")) {
            return " @NID='" + fileNID + "' ";
        }
        if (!title.equals("") && !dateCreated.equals("")) {
            return " //Title='" + title + "' and  //DateCreated='" + dateCreated + "' ";
        }
        throw new Exception("Unknown file");
    }

    public static String getListName(MRLog log) throws Exception {
        if (object_listName == null) {
            MRConfig mrcConfig = MRConfig.getInstance(objectType, log);
            object_listName = mrcConfig.getParentNIDKey(log);
        }
        return object_listName;
    }

    @SuppressWarnings("unchecked")
    public static void addErrorToQueueElement(Element queueEL, int errorCode, String error) throws Exception {
        List<Element> elList = (List<Element>) MRXMLUtils.getListXpath(queueEL, "//Exception");
        for (Element element : elList) {
            if (element != null) {
                element.detach();
            }
        }
        Element el = MRXMLUtils.addElement(queueEL, "Exception");
        MRXMLUtils.addElement(el, "Code", errorCode + "");
        MRXMLUtils.addElement(el, "Description", error + "");
    }

    public static void addErrorToResponse(Element errorParentEL, int errorCode, String error) throws Exception {
        MRException ex = new MRException(errorCode, error);
        if (errorParentEL != null) {
            Element EL = errorParentEL;
            if (EL.getName().equals(AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE)) {
                FileSMUtils.addErrorToQueueElement(EL, errorCode, ex.getData());
            } else {
                EL = MRXMLUtils.getElementXpath(errorParentEL, "//Data");
                if (EL != null) {
                    Element errorEL = MRXMLUtils.addElement(EL, "Exception");
                    MRXMLUtils.addElement(errorEL, "Description", ex.getData());
                    MRXMLUtils.addElement(errorEL, "Code", errorCode+"");
                }else{ 
                    MRLog.println("data element not found");
                }
            }
        }
    }

    public static String getMRException(Element queueEL, String error) {
        StringBuffer data = new StringBuffer();
        String nid = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
        String type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
        data.append("<error>");
        data.append("<type>");
        data.append(type);
        data.append("</type>");
        data.append("<queue_nid>");
        data.append(nid);
        data.append("</queue_nid>");
        data.append("<error>");
        data.append(error);
        data.append("</error>");
        data.append("</error>");
        return data.toString();
    }

    //////////// db read/updates ////////////////////
    public static String joinArray(String[] array, String saparator) {
        if (array != null) {
            StringBuffer buf = new StringBuffer();
            for (int i = 0; i < array.length; i++) {
                if (i != 0) {
                    buf.append(saparator);
                }
                buf.append(array[i]);
            }
            return buf.toString();
        }
        return saparator;
    }

    public static void deleteDirectory(File delFile) {
        try {
            if (delFile.exists()) {
                if (delFile.isDirectory()) {
                    File files[] = delFile.listFiles();
                    for (File file : files) {
                        deleteDirectory(file);
                    }
                } else {
                    boolean deleted = delFile.delete();
                    if (!deleted) {
                        System.out.println("FileSM:deleteDirectory failed to delete " + delFile.getAbsolutePath());
                    }
                }
            }
            delFile.delete();
        } catch (Exception ex) {
            System.out.println("FileSM:Error deleteDirectory " + delFile.getAbsolutePath());
        }
    }

    public static void deleteChildrenDirectory(File delFile) {
        try {
            if (delFile.exists()) {
                if (delFile.isDirectory()) {
                    File files[] = delFile.listFiles();
                    for (File file : files) {
                        deleteDirectory(file);
                    }
                } else {
                    boolean deleted = delFile.delete();
                    if (!deleted) {
                        System.out.println("FileSM:deleteChildrenDirectory failed to delete " + delFile.getAbsolutePath());
                    }
                }
            }

        } catch (Exception ex) {
            System.out.println("FileSM:error deleteDirectory " + delFile.getAbsolutePath());
        }
    }

    public static boolean isUserPresenceAvailable(XMPPI xmpp, String userJID, MRLog log, MRDB db) {
        boolean available = false;
        try{
        	String user = org.jivesoftware.smack.util.StringUtils.parseName(userJID);        	
         	// bypassing the presence in case of ADMIN user and bareJID (like managementserver.localhost)
        	if(user!=null && (user.equals(MRServer.ADMIN) || user.trim().isEmpty())) {
         		return true;
        	}else{
        	  UserManager userManager = UserManager.getInstance();	
        	  String resource = org.jivesoftware.smack.util.StringUtils.parseResource(userJID);
        	  available=  userManager.checkJID(userJID, log, db, resource);
        	}
           }
        catch(Exception exc){
            exc.printStackTrace();
            return false;
         }
        if(!available){
            //todo:  reduce the retry attempts 
            try {
                xmpp.subscribePresence(userJID);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
        return available;
    }
    
    public static boolean is2Dot4Media(String fileName) throws Exception {
        if(fileName.endsWith(MEDIA_TAR_EXTENSION_2DOT4)){
            return true;
        }
        return false;
    }    
    
    
    
}
