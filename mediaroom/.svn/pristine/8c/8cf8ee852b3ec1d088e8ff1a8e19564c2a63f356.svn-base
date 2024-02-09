package com.ipvs.mediastoreservice.impl;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Semaphore;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.file.handler.AddFileRequestHandler;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.assetadminservice.impl.MediaManagementUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRDBHelper;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.IPVSMonitor;
import com.ipvs.common.service.MRClientUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.AddMediaGroupFileFromMediaClipRequestHandler;
import com.ipvs.mediaroomservice.handler.UpdateMediaGroupPlayLenghtAndSizeRequestHandler;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediastoreservice.handler.RepairMediaRequestHandler;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;


public class MediaStoreServiceUtils {
	@Deprecated
	//Send media store request to media store admin server agent.
    public static void sendMediaStoreFileRequestToMediaStoreAdmin(MRRequest request, String requestName, boolean isWriteOperation, String fromAgentName, Document mediaStoreRequestDoc, Document mediaStreamFileDoc, Document mediaRoomRequestDoc, XMPPI xmpp, MRLog log, MRDB db)
        throws Exception {
        String mediaStorePortNID = getMediaStorePortResourceNIDFromDir(mediaStreamFileDoc, isWriteOperation, request, db, log);
        sendAgentMessageToMediaStoreAdmin(mediaStorePortNID, request, requestName, fromAgentName, mediaStoreRequestDoc, mediaRoomRequestDoc, xmpp, log, db);
    }    
    
	@Deprecated
    //Send media store request to media store admin server agent.
    public static void sendAgentMessageToMediaStoreAdmin(String mediaStorePortNID, MRRequest request, String requestName, String fromAgentName, Document mediaStoreRequestDoc, Document userRequestDoc, XMPPI xmpp, MRLog log, MRDB db)
        throws Exception {
        if (requestName.isEmpty()) {
            throw new MRException(MediaStoreServiceExceptionCodes.INVALID_MEDIA_STORE_REQUEST, "Invalid Media Store Request");
        }

        String mediaStoreStreamAgentJID = MediaRoomServiceUtils.getContactJIDForPort(request, log, db, mediaStorePortNID);

        if ((mediaStoreStreamAgentJID == null) || (mediaStoreStreamAgentJID.trim().length() == 0)) {
            throw new MRException(MediaRoomServiceExceptionCodes.AGENT_NOT_FOUND, "Media Store Agent JID Not Found");
        }

        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
        ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, mediaStoreStreamAgentJID, MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT, requestName, MRXMLUtils.documentToStringOmitDeclaration(mediaStoreRequestDoc),
            fromAgentName + ": Sending " + requestName + " to " + MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT, request.getNID(), request.getUserJID(), MRConst.MEDIA_STORE_ADMIN_SERVICE, clientData);
    }
	@Deprecated
    //Send media store request to media store admin server agent.
    public static void sendMediaStreamDirRequestToMediaStoreAdmin(MRRequest request, String mediaStreamDirNID, boolean isWriteOperation, Document userRequestDoc, boolean upgradeMediaStreamFiles, XMPPI xmpp, MRLog log, MRDB db)
        throws Exception {
        //Marshal the Request object into XML format        
        Document mrRequestDoc = request.marshalRequestObject(log);
        String mrRequestStr = URLEncoder.encode(MRXMLUtils.documentToString(mrRequestDoc), "UTF-8");
        MRXMLUtils.setValueXpath(userRequestDoc, "//"+MRConst.CLIENT_DATA, mrRequestStr);

        String mediaStoreRequestName = MediaStoreServiceConst.mediaroomRequestToMediaStoreRequestMapping.get(request.getRequestName());

        if (mediaStoreRequestName == null) {
            return;
        }

        Document mediaDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(mediaStreamDirNID, request, db, log);

        Element mediaStorePortEL = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, isWriteOperation, db, log);
        String mediaStorePortNID = null;

        if (mediaStorePortEL != null) {
            mediaStorePortNID = MRXMLUtils.getValue(mediaStorePortEL);
        }

        Document mediaStoreRequestDocument = generateRequestDocumentForMediaStoreDirActions(mediaStoreRequestName, mediaStreamDirNID, upgradeMediaStreamFiles, log);

        sendAgentMessageToMediaStoreAdmin(mediaStorePortNID, request, mediaStoreRequestName, AppServerAgent.class.getName(), mediaStoreRequestDocument, userRequestDoc, xmpp, log, db);
    }
    
	
    public static String getMediaStoreOwnerJID(Document mediaDirDoc,MRRequest request,MRDB db,MRLog log) throws MRException,Exception {
        
        Element mediaStorePortEL = MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, true, db, log);
        String mediaStorePortNID = null;

        if (mediaStorePortEL != null) {
            mediaStorePortNID = MRXMLUtils.getValue(mediaStorePortEL);
        }
        
        String mediaStoreStreamAgentJID = MediaRoomServiceUtils.getContactJIDForPort(request, log, db, mediaStorePortNID);

        if ((mediaStoreStreamAgentJID == null) || (mediaStoreStreamAgentJID.trim().length() == 0)) {
            throw new MRException(MediaRoomServiceExceptionCodes.AGENT_NOT_FOUND, "Media Store Agent JID Not Found");
        }
        
        return mediaStoreStreamAgentJID;
    }
	@Deprecated
    public static MRRequest getRequestObject(Document responseDoc, Document requestDoc, MRLog log)
        throws MRException, Exception {
        //Get the request document MRRequest.xml from client Data of request Document.
        String requestClientDate = MRXMLUtils.getValueXpath(requestDoc, "//"+MRConst.CLIENT_DATA, "");
        Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(requestClientDate, "UTF-8")));
        //set the client data with client data of request received.
        MRXMLUtils.setValueXpath(requestDoc, "//"+MRConst.CLIENT_DATA, MRXMLUtils.getValueXpath(mrRequestDoc, "//"+MRConst.CLIENT_DATA, ""));

        MRRequest mediaStreamFilerequest = MRRequest.unMarshalRequestObject(mrRequestDoc, requestDoc, "", log);

        return mediaStreamFilerequest;
    }

    public static String getMediaStorePortResourceNIDFromDir(String mediaStreamFileNID, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        if (mediaStreamFileNID.isEmpty()) {
            throw new MRException(MRExceptionCodes.NODENOTFOUND, MRXMLUtils.generateErrorXML("nid", mediaStreamFileNID));
        }

        Document mediaStreamFileDoc = MRXMLDBUtils.readElement(request, log, db, mediaStreamFileNID);

        return getMediaStorePortResourceNIDFromDirWrite(mediaStreamFileDoc, request, db, log);
    }

    public static Document getMediaStreamDirDoc(Document mediaStreamFileDoc, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        String parentDirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");

        return getMediaStreamDirDoc(parentDirNID, request, db, log);
    }

    public static Document getMediaStreamDirDoc(String dirNID, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        if ((dirNID == null) || dirNID.isEmpty()) {
            return null;
        }

        Document mediaDirDoc = MRXMLDBUtils.readElement(request, log, db, dirNID);

        if (mediaDirDoc == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NOT_FOUND, "MediaStreamDirResource Not found");
        }

        return mediaDirDoc;
    }

    public static String getMediaStorePortResourceNIDFromDirWrite(Document mediaStreamFileDoc, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        return getMediaStorePortResourceNIDFromDir(mediaStreamFileDoc, true, request, db, log);
    }

    public static String getMediaStorePortResourceNIDFromDirRead(Document mediaStreamFileDoc, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        return getMediaStorePortResourceNIDFromDir(mediaStreamFileDoc, false, request, db, log);
    }

    public static String getMediaStorePortResourceNIDFromDir(Document mediaStreamFileDoc, boolean isWritable, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        Element mediaStorePortResourceEL = getMediaStorePortResourceElementFromFile(mediaStreamFileDoc, isWritable, request, db, log);

        if (mediaStorePortResourceEL == null) {
            return "";
        } else {
            return MRXMLUtils.getValue(mediaStorePortResourceEL);
        }
    }

    public static Element getMediaStorePortResourceElementFromFile(Document mediaStreamFileDoc, boolean isWriteOperation, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        Document mediaDirDoc = getMediaStreamDirDoc(mediaStreamFileDoc, request, db, log);

        if (mediaDirDoc == null) {
            return null;
        }

        String title = MRXMLUtils.getValueXpath(mediaDirDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");

        if (MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME.equals(title) || MediaStoreServiceConst.PVR_DIR_NAME.equals(title)) {
            mediaDirDoc = getMediaStreamDirDoc(mediaDirDoc, request, db, log);
        }

        Element mediaStorePortResourceEL = getMediaStorePortResourceElementFromDir(mediaDirDoc, isWriteOperation, db, log);

        return mediaStorePortResourceEL;
    }

    public static Element getMediaStorePortResourceElementFromDir(Document mediaDirDoc, boolean isWriteOperation, MRDB db, MRLog log)
    throws MRException, Exception {
    	return MediaStoreServiceUtils.getMediaStorePortResourceElementFromDir(mediaDirDoc, isWriteOperation, db, log, true);
    }
    
    public static Element getMediaStorePortResourceElementFromDir(
    		Document mediaDirDoc, boolean isWriteOperation, MRDB db, MRLog log,
    		boolean checkState)  throws MRException, Exception {
        if (mediaDirDoc == null) {
            return null;
        }     
        String dirState = MRXMLUtils.getValueXpath(mediaDirDoc, "//StateFlag", "");

        if (checkState && !AMConst.ASSETSTATE_READY.equals(dirState)) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_STATE_NOT_READY, "MediaStreamDir State is not Ready. State:= " + dirState);
        }

        List<?> mediastorePortNIDList = MRXMLUtils.getListXpath(mediaDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME);

        Element objEL = null;
        String presence = null;
        int priority = 0;
        int highPriority = -1;
        boolean isReadOnly = false;
        Element mediaStorePortResourceEL = null;

        for (int index = 0; (mediastorePortNIDList != null) && (index < mediastorePortNIDList.size()); index++) {
            objEL = (Element) mediastorePortNIDList.get(index);
          
            presence = MRXMLUtils.getAttributeValue(objEL, MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME, "");

            if (!AMConst.AVAILABLE.equals(presence)) {
                continue;
            }

            if (isWriteOperation) {
                isReadOnly = Utils.getBoolean(MRXMLUtils.getAttributeValue(objEL, MediaStoreServiceConst.READ_ONLY_ATTRIBUTE_NAME, ""), false);

                if (isReadOnly) {
                    continue;
                }
            }

            priority = Utils.getIntValue(MRXMLUtils.getAttributeValue(objEL, MediaStoreServiceConst.PRIORITY_ATTRIBUTE_NAME, ""), highPriority);

            if (priority <= highPriority || highPriority == -1) {
                highPriority = priority;
                mediaStorePortResourceEL = objEL;
            }
        }

        if (mediaStorePortResourceEL == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_PORT_NOT_CONFIGURED, "MediaStorePortResource Not Configured in Dir");
        }

        return mediaStorePortResourceEL;
    }
    @Deprecated
    public static MRRequest getRequestObject(Document responseDoc, MRLog log)
        throws MRException, Exception {
        Document requestDoc = getRequestDocFromResponseClientData(responseDoc, log);

        return getRequestObject(responseDoc, requestDoc, log);
    }
    @Deprecated
    public static Document getRequestDocFromResponseClientData(Document responseDoc, MRLog log)
        throws MRException, Exception {
        String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");

        return MRXMLUtils.stringToDocument(URLDecoder.decode(clientData, "UTF-8"));
    }
    @Deprecated
    public static MRRequest getUserRequest(MRRequest request, Document responseDoc, MRLog log)
        throws MRException, Exception {
        Document assetRequestDoc = getRequestDocFromResponseClientData(responseDoc, log);

        //Get the request document MRRequest.xml from client Data of request Document.
        String requestClientData = MRXMLUtils.getValueXpath(assetRequestDoc, "//"+MRConst.CLIENT_DATA, "");
        if(requestClientData.isEmpty()){
        	return null;
        }
        Document mrRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(requestClientData, "UTF-8")));
        //set the client data with client data of request received.
        MRXMLUtils.setValueXpath(assetRequestDoc, "//"+MRConst.CLIENT_DATA, MRXMLUtils.getValueXpath(mrRequestDoc, "//"+MRConst.CLIENT_DATA, ""));

        MRRequest assetAdminrequest = MRRequest.unMarshalRequestObject(mrRequestDoc, assetRequestDoc, "", log);

        //Get the client Data of asset data to check user request is coming from media room or not.
        String assetClientData = MRXMLUtils.getValueXpath(assetRequestDoc, "//"+MRConst.CLIENT_DATA, "");
        Document userRequestDoc = null;

        if (AssetAdminServiceUtils.isMediaRoomRequest(assetClientData)) {
            userRequestDoc = MRXMLUtils.stringToDocument(URLDecoder.decode(assetClientData, "UTF-8"));
        } else {
            userRequestDoc = assetRequestDoc;
        }

        String requestName = MRXMLUtils.getAttributeValueXpath(userRequestDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
        String serviceName =  MRXMLUtils.getAttributeValueXpath(userRequestDoc, MRConst.SERVICE_NAME,MRConst.REQUEST_NAME, "");
        String userClientData = MRXMLUtils.getValueXpath(userRequestDoc, "//"+MRConst.CLIENT_DATA, "");

        return MRRequest.createRequest(assetAdminrequest, userRequestDoc, requestName, serviceName, userClientData);
    }
    @Deprecated
    //Generate media stream file request from media store file response
    public static MRRequest createMediaStreamFileRequest(MRRequest request, Document responseDoc, Element responseXMLEL, String actionResponse, MRLog log)
        throws MRException, Exception {
        MRRequest userRequest = getUserRequest(request, responseDoc, log);
        if(userRequest ==null) {
        	return null;
        }

        Document mediaStreamFileResponseDoc = MRXMLUtils.getDocument();
        MRXMLUtils.addElement(mediaStreamFileResponseDoc, userRequest.getResponseDataName());
        MRDBHelper.getActionResEL(actionResponse, userRequest.getNID(), mediaStreamFileResponseDoc);

        Element responseXML = MRXMLUtils.getElementXpath(mediaStreamFileResponseDoc, "//ResponseXML");

        if (request.getState() != MRRequest.STATE_EXCEPTION) {
            MRXMLUtils.addChildElement(responseXML, responseXMLEL);
        }

        if (actionResponse.equals("GetResponse")) {
            int objectCount = responseXML.elements().size();
            int objectLastWhereCount = 0;
            int countToFetch = 5000;
            Element getObjectListResponseEL = getListObjectResponseeElement(objectCount, objectLastWhereCount, countToFetch);
            Element getResponseEL = MRXMLUtils.getElementXpath(mediaStreamFileResponseDoc, "//GetResponse");
            MRXMLUtils.addElement(getResponseEL, getObjectListResponseEL);
        }

        userRequest.setResult(mediaStreamFileResponseDoc);
        userRequest.setState(request.getState());
        userRequest.setException(request.getException());

        return userRequest;
    }

    private static Element getListObjectResponseeElement(int objectCount, int objectLastWhereCount, int countToFetch)
        throws MRException, Exception {
        String xml = " <GetListObjectResponseData><ObjectCount>" + objectCount + "</ObjectCount>" + "<ObjectLastWhereEndCount>" + +objectLastWhereCount + "</ObjectLastWhereEndCount>" + "<CountToFetch>" + countToFetch + "</CountToFetch>" + "</GetListObjectResponseData>";

        return MRXMLUtils.stringToElement(xml);
    }

    public static Document checkMediaStreamFileState(Document responseDoc, int exceptionCode, MRLog log, MRDB db, MRRequest request)
        throws MRException, Exception {
        MediaStoreServiceUtils.validateMediaStoreResponse(responseDoc, exceptionCode);

        String mediaStreamFileNID = MRXMLUtils.getValueXpath(responseDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");

        return checkMediaStreamFileState(mediaStreamFileNID, log, db, request);
    }

    public static Document checkMediaStreamFileState(String mediaStreamFileNID, MRLog log, MRDB db, MRRequest request)
        throws MRException, Exception {
        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);

        if (mediaStreamFileDoc == null) {
            throw new MRException(MediaRoomServiceExceptionCodes.MEDIANOTFOUND, MRXMLUtils.generateErrorXML("fileNID", mediaStreamFileNID));
        }

        MediaStoreServiceUtils.checkMediaStreamFileState(mediaStreamFileDoc);

        return mediaStreamFileDoc;
    }
    
    
    public static Document getMediaStreamFileDocFromStoreFile(String mediaStoreFileNID,String mediaStreamDirNID, MRLog log, MRDB db, MRRequest request)
    throws MRException, Exception {
    	
    	String whereXpath[] = { "//"+MediaStoreServiceConst.UUID+"='"+mediaStoreFileNID+"'" ,"//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+"='"+mediaStreamDirNID+"'"};
    	List<?> fileList =  getMediaStreamFileResourceList(whereXpath, request.getUserJID(), log, db);
    	if(fileList ==null||fileList.isEmpty()) {
    		throw new MRException(MediaRoomServiceExceptionCodes.MEDIANOTFOUND, MRXMLUtils.generateErrorXML("fileNID", mediaStoreFileNID));
    	}
    	Document mediaStreamFileDoc = MRXMLUtils.elementToDocument((Element)fileList.get(0));
    	MediaStoreServiceUtils.checkMediaStreamFileState(mediaStreamFileDoc);
    	return mediaStreamFileDoc;
    }

    //check Media Stream File State 
    public static void checkMediaStreamFileState(Document mediaStreamFileDoc)
        throws MRException, Exception {
        String state = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, null);

        /* MediaStore not yet contacted for this file, is a valid state
        if ((state == null) || state.equals(MediaStoreServiceConst.NOT_READY)) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_NOT_READY, "Media Store Not Ready");
        }
        */

        if ((state == null) || state.equals(MediaStoreServiceConst.ERROR)) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_STATE_ERROR, "Media Stream File State is Error, Can not proceed");
        }
    }
    @Deprecated
    //Loads the media store response document and generate request xml from that by setting the values
    public static Document generateResponseDocument(String responseName, String mediaStoreFileNID, String mediaStreamFileNID, String responseXML, int state, String logMessage, String requestNID, MRLog log)
        throws MRException, Exception {
    	if(responseName ==null|| responseName.isEmpty()) {
    		return null;
    	}
        Document responseDoc = MediaStoreServiceUtils.loadMediaStoreRequestOrResponseDocument(responseName, log);
        MRXMLUtils.setAttributeValueXPath(responseDoc, MRConst.HEADER_XPATH,MRConst.STATE, String.valueOf(state));
        MRXMLUtils.setValueXpath(responseDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID, mediaStoreFileNID);
        MRXMLUtils.setValueXpath(responseDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, mediaStreamFileNID);
 
        return generateResponseDocument(responseName, responseDoc, responseXML, state, logMessage, requestNID, log);
    }
    @Deprecated
    public static Document generateResponseDocument(String responseName, Element mediaStoreFileListEl, String responseXML, int state, String logMessage, String requestNID, MRLog log)
    throws MRException, Exception {
		if(responseName ==null|| responseName.isEmpty()) {
			return null;
		}
	    Document responseDoc = MediaStoreServiceUtils.loadMediaStoreRequestOrResponseDocument(responseName, log);
	    MRXMLUtils.getElementXpath(responseDoc, "//MediaStoreFileList").detach();
	    MRXMLUtils.addElement(MRXMLUtils.getElementXpath(responseDoc, "//" + responseName + "Data"), mediaStoreFileListEl);
	    return generateResponseDocument(responseName, responseDoc, responseXML, state, logMessage, requestNID, log);
	}
    
    @Deprecated
    //Loads the media store response document and generate request xml from that by setting the values
    public static Document generateResponseDocument(String responseName, Document responseDoc, String responseXML, int state, String logMessage, String requestNID, MRLog log)
        throws MRException, Exception {
    	
    	MRXMLUtils.setAttributeValueXPath(responseDoc, MRConst.HEADER_XPATH,MRConst.STATE, String.valueOf(state));
   	   
        Element rootEL = responseDoc.getRootElement();

        if (!responseXML.isEmpty()) {
            String responseXMLStr = MRXMLUtils.generateXML("ResponseXML", responseXML);
            Element responseXMLEL = MRXMLUtils.stringToElement(responseXMLStr);
            Element responseEL = MRXMLUtils.getElementXpath(responseDoc, "//ResponseXML");
            Element requestDataEL = MRXMLUtils.getElementXpath(responseDoc, "//" + responseName + "Data");
            if(responseEL!=null)
        	responseEL.detach();
            requestDataEL.add(responseXMLEL);
        }

        if (!logMessage.isEmpty()) {
            String logELStr = MRXMLUtils.generateXML(MediaStoreServiceConst.LOG_ELEMENT, logMessage);
            Element logEL = MRXMLUtils.stringToElement(logELStr);
            Element responseLogEL = MRXMLUtils.getElementXpath(responseDoc, "//" + MediaStoreServiceConst.LOG_ELEMENT);
            if(responseLogEL!=null){
        	responseLogEL.detach();
            }
            rootEL.add(logEL);
        }

        return MRXMLUtils.getDocumentXpath(responseDoc, "//" + responseName + "Data");
    }
    
    @Deprecated
  //Loads the media store request document and generate request xml from that by setting the values
    public static Document generateRequestDocument(String requestName, 
    		Hashtable<String,String> fileMapping, String mediaStreamFileDirNID, String actionData, boolean purgeFiles, String requestNID, MRLog log)
        throws MRException, Exception {
        Document requestDoc = MediaStoreServiceUtils.loadMediaStoreRequestOrResponseDocument(requestName, log);
        Element mediaStoreFileListEl = MRXMLUtils.getElementXpath(requestDoc, "//MediaStoreFileList");
        Element mediaStoreFileEl =(Element) MRXMLUtils.getElement(mediaStoreFileListEl,"MediaStoreFile").detach();
        for(String mediaStreamFileNID: fileMapping.keySet()) {
        	MRXMLUtils.setValueXpath(mediaStoreFileEl, "./" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID, fileMapping.get(mediaStreamFileNID));
        	MRXMLUtils.setValueXpath(mediaStoreFileEl, "./" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, mediaStreamFileNID);
        	MRXMLUtils.addElement(mediaStoreFileListEl,(Element)mediaStoreFileEl.clone());
        }
        
        MRXMLUtils.setValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, mediaStreamFileDirNID);
        if(!MediaStoreServiceConst.UPDATE_FILE_AUTOSTOARGE_RULES_REQUEST.equalsIgnoreCase(requestName)){
        	MRXMLUtils.setAttributeValue(mediaStoreFileListEl, MediaStoreServiceConst.PURGE_FILES_ATTRIBUTE, ""+purgeFiles);
        }
        
        if (!actionData.isEmpty()) {
            String actionDataStr = MRXMLUtils.generateXML(MediaStoreServiceConst.ACTION_DATA, actionData);
            Element actionDataEL = MRXMLUtils.stringToElement(actionDataStr);
            Element requestActionEL = MRXMLUtils.getElementXpath(requestDoc, "//" + MediaStoreServiceConst.ACTION_DATA);
            requestActionEL.detach();

            Element requestDataEL = MRXMLUtils.getElementXpath(requestDoc, "//" + requestName + "Data");
            requestDataEL.add(actionDataEL);
        }

        return MRXMLUtils.getDocumentXpath(requestDoc, "//" + requestName + "Data");
    }
    
    @Deprecated
    //Loads the media store request document and generate request xml from that by setting the values
    public static Document generateRequestDocument(String requestName, 
    		String mediaStoreFileNID, String mediaStreamFileNID, String mediaStreamFileDirNID, String actionData, String requestNID, MRLog log)
        throws MRException, Exception {
        Document requestDoc = MediaStoreServiceUtils.loadMediaStoreRequestOrResponseDocument(requestName, log);
        MRXMLUtils.setValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID, mediaStoreFileNID);
        MRXMLUtils.setValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, mediaStreamFileNID);
        MRXMLUtils.setValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, mediaStreamFileDirNID);

        if (!actionData.isEmpty()) {
            String actionDataStr = MRXMLUtils.generateXML(MediaStoreServiceConst.ACTION_DATA, actionData);
            Element actionDataEL = MRXMLUtils.stringToElement(actionDataStr);
            Element requestActionEL = MRXMLUtils.getElementXpath(requestDoc, "//" + MediaStoreServiceConst.ACTION_DATA);
            requestActionEL.detach();

            Element requestDataEL = MRXMLUtils.getElementXpath(requestDoc, "//" + requestName + "Data");
            requestDataEL.add(actionDataEL);
        }

        return MRXMLUtils.getDocumentXpath(requestDoc, "//" + requestName + "Data");
    }
    
    //Loads the media store request document and generate request xml from that by setting the values
    public static Document generateRequestDocumentForMediaStoreDirActions(String requestName, String mediaStreamDirNID, boolean upgradeMediaStreamFiles, MRLog log)
        throws MRException, Exception {
        Document requestDoc = MediaStoreServiceUtils.loadMediaStoreRequestOrResponseDocument(requestName, log);
        MRXMLUtils.setValueXpath(requestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, mediaStreamDirNID);
        MRXMLUtils.setValueXpath(requestDoc, "//UpgradeMediaStreamFiles" , upgradeMediaStreamFiles+"");

        return MRXMLUtils.getDocumentXpath(requestDoc, "//" + requestName + "Data");
    }
    
    //Get the UUID from Media Stream File Document
    public static String getMediaStoreFileNID(String mediaStreamFileNID, String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
        Document mediaStreamFileDoc = getMediaStreamFileDoc(mediaStreamFileNID, MRRequest.createRequest(userJID), log, db);

        return getMediaStoreFileNID(mediaStreamFileDoc, MRRequest.createRequest(userJID), log, db);
    }

    public static String getMediaStoreFileNID(Document mediaStreamFileDoc, MRRequest request, MRLog log, MRDB db)
        throws MRException, Exception {
        String mediaStoreFileNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//UUID", "");

        if (mediaStoreFileNID.isEmpty()) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_NID_NOT_FOUND, "MediaStoreNID Not Found");
        }

        return mediaStoreFileNID;
    }

    public static String getParentDirNID(Document mediaStreamFileDoc, MRRequest request, MRLog log, MRDB db)
        throws MRException, Exception {
        String parentDirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");

        if (parentDirNID.isEmpty()) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NID_NOT_FOUND, "Media Dir NID Not Found");
        }

        return parentDirNID;
    }

    //Send Response to Media Store Client Agent Message
    public static void sendMediaStoreResponseMessage(Document requestdoc, Document responseDoc, String requestName, String toJID, int status, String logdata, XMPPI xmpp, MRLog log, String userJID)
        throws Exception {
    	if(responseDoc == null) {
    		return;
    	}
        String mediaStreamFileNID = MRXMLUtils.getValueXpath(requestdoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
        String clientcallback = MRXMLUtils.getValueXpath(requestdoc, "//"+MRConst.CLIENT_CALLBACK, "");

        String info = "MediaStoreAdminServerAgent:=" + userJID + "/" + mediaStreamFileNID + " > Response request=" + requestName + " to " + clientcallback +" Status:="+status;
        int responseState = MRRequest.STATE_DONE;

        if (status == MRLog.ERROR) {
            responseState = MRRequest.STATE_EXCEPTION;
        }

        String response = MRXMLUtils.documentToStringOmitDeclaration(responseDoc);
         
        ServiceAgentUtils.sendAgentResponseMessage(xmpp, log, null, MediaStoreAdminServerAgent.class.getName(), toJID, clientcallback, ServiceClient.MEDIASTORESERVICE, requestName, response, info, responseState, logdata, MRXMLUtils.getAttributeValueXpath(requestdoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NID, ""),
            MRXMLUtils.getAttributeValueXpath(requestdoc,MRConst.HEADER_XPATH, MRConst.USER_JID, ""), MRXMLUtils.getValueXpath(requestdoc,"//"+ MRConst.CLIENT_DATA, ""));
    }

    //check media store response passed or failed.
    public static void validateMediaStoreResponse(Document responseDoc, int exceptionCode)
        throws MRException, Exception {
        Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH,MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);

        if (state == MediaStoreServiceConst.FAILURE_STATE) {
            throw new MRException(Utils.getIntValue(MRXMLUtils.getValueXpath(responseDoc, "//Code", ""), exceptionCode), MRXMLUtils.getValueXpath(responseDoc, "//Description", ""));
        }
    }
    
    // Delete Media Stream File from DB
    public static Document deleteMediaStreamFileDB(String mediaStreamFileNID, MRRequest request, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
            throws MRException, Exception {
    	
        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);  
        String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
        log.addInfo("deleteMediaStreamFileDB: Delete MediaStreamFileResource NID:=" + mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);
        MRXMLDBUtils.deleteElement(request, log, db, mediaStreamFileNID);
        if(!mediaGroupID.isEmpty()) {
        	Semaphore lock = null;
        	try {

				List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, db);
				if ((listOfFiles == null) || listOfFiles.isEmpty()) {				
					log.addInfo("deleteMediaStreamFileDB: Aquire lock on mediaGroupID:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);
					lock = IPVSMonitor.getMonitor().getLock(mediaGroupID, request.getNID());
					Document mediaGroupDoc = MediaStoreServiceUtils.getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);
					String fileNID = MRXMLUtils.getAttributeValue(mediaGroupDoc.getRootElement(), MediaStoreServiceConst.NID, "");
					MRXMLDBUtils.deleteElement(request, log, db, fileNID);
					log.addInfo("deleteMediaStreamFileDB: Delete MediaGroupFile NID:=" + fileNID +" MediaGroupID:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);
					log.addInfo("deleteMediaStreamFileDB: Release lock on mediaGroupID:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);
					if(lock!=null)
						IPVSMonitor.getMonitor().releaseLock(mediaGroupID, lock);
				}
				else {
					//update Media Group Size and Play length on delete
					MediaStoreServiceUtils.updateMediaGroupOnMediaClipUpdate(mediaStreamFileDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
				}
				
			} catch (Exception e) {
				e.printStackTrace();
			}
		
        }
        
        return mediaStreamFileDoc;
    
    }

    public static Document getMediaStreamFileDoc(String mediaStreamFileNID, MRRequest request, MRLog log, MRDB db)
        throws MRException, Exception {
        //log.addInfo("Read Media Stream File STart NID ="+mediaStreamFileNID ,MRLog.INFO,MRLog.NOTIFY);
        //com.ipvs.common.XMPPTransaction.setDEBUG(true);        
        // com.ipvs.common.XMPPTransaction.startBlock("getMediaStreamFileDoc");
        if (mediaStreamFileNID.isEmpty()) {
            throw new MRException(MediaRoomServiceExceptionCodes.MEDIA_STREAM_FILE_NID_NOT_FOUND, "MediaStreamFile NID Not Found");
        }

        /*
        if (!MRXMLDBUtils.objectExists(request, db, log, mediaStreamFileNID)) {
            throw new MRException(MediaRoomServiceExceptionCodes.MEDIANOTFOUND, MRXMLUtils.generateErrorXML("fileNID", mediaStreamFileNID));
        }
        */

        MRRequest msrequest = MRRequest.createRequest(request.getUserJID(), request.getAppServerJID());
        msrequest.setCache(request.getCache());
        Document mediaStreamFileDoc = MRXMLDBUtils.readElement(msrequest, log, db, mediaStreamFileNID);
        if(mediaStreamFileDoc == null) {
            throw new MRException(MediaRoomServiceExceptionCodes.MEDIANOTFOUND, MRXMLUtils.generateErrorXML("fileNID", mediaStreamFileNID));        	
        }

        // com.ipvs.common.XMPPTransaction.endBlock("getMediaStreamFileDoc");
        // com.ipvs.common.XMPPTransaction.setDEBUG(false);

        //log.addInfo("Read Media Stream File  End NID ="+mediaStreamFileNID ,MRLog.INFO,MRLog.NOTIFY);
        return mediaStreamFileDoc;
    }
    @Deprecated
    public static void sendRequestToMediaStoreAgent(String mediaRoomRequestName, Document mediaStreamFileDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB schema, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Document mediaRoomRequestDoc = (Document) request.getMessage().getDocument().clone();

        try {
            String mediaStoreRequestName = null;
            String mediaStoreFileNID = null;
            String mediaStreamFileNID = null;
            String mediaStreamFileDirNID = null;
            Element actionDataEL = null;
            String actionData = "";
            mediaStreamFileNID = MRXMLUtils.getAttributeValueXpath(mediaStreamFileDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE, "NID", "");
            mediaStreamFileDirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");

            mediaStoreFileNID = getMediaStoreFileNID(mediaStreamFileDoc, request, log, schema);

            //Marshal the Request object into XML format        
            Document mrRequestDoc = request.marshalRequestObject(log);
            String mrRequestStr = URLEncoder.encode(MRXMLUtils.documentToString(mrRequestDoc), "UTF-8");
            MRXMLUtils.setValueXpath(mediaRoomRequestDoc, "//"+MRConst.CLIENT_DATA, mrRequestStr);
            mediaStoreRequestName = MediaStoreServiceConst.mediaroomRequestToMediaStoreRequestMapping.get(mediaRoomRequestName);
         
            if (mediaStoreRequestName == null) {
                return;
            }

            actionDataEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);

            /*if (mediaRoomRequestName.equals(MediaRoomServiceConst.ADD_BOOKMARK_REQUEST) || mediaRoomRequestName.equals(MediaRoomServiceConst.UPDATE_BOOKMARK_REQUEST) || mediaRoomRequestName.equals(MediaRoomServiceConst.DELETE_BOOKMARK_REQUEST)) {
                actionDataEL = MRXMLUtils.getElementXpath(mediaRoomRequestDoc, "//Bookmark");
            } else*/
            boolean isWriteOperation = true;

            if (mediaRoomRequestName.equals(MediaRoomServiceConst.GET_SRC_MEDIA_FILE_TRACK_REQUEST)) {
                actionDataEL = MRXMLUtils.getElementXpath(mediaRoomRequestDoc, "//GetRequest");
                isWriteOperation = false;
            } else if (mediaRoomRequestName.equals(MediaRoomServiceConst.UPDATE_DEST_FILE_METADATA_REQUEST) || mediaRoomRequestName.equals(MediaRoomServiceConst.UPDATE_DEST_FILE_PROFILE_REQUEST)) {
                actionDataEL = MRXMLUtils.getElementXpath(mediaRoomRequestDoc, request.getRequestDataXPath());
            }

            actionData = MRXMLUtils.elementToString(actionDataEL);

            Document mediaStoreRequestDocument = generateRequestDocument(mediaStoreRequestName, mediaStoreFileNID, mediaStreamFileNID, mediaStreamFileDirNID, actionData, request.getNID(), log);            
            sendMediaStoreFileRequestToMediaStoreAdmin(request, mediaStoreRequestName, isWriteOperation, AppServerAgent.class.getName(), mediaStoreRequestDocument, mediaStreamFileDoc, mediaRoomRequestDoc, xmpp, log, schema);
        } catch (MRException exp) {
            exp.printStackTrace();
            throw new MRException(exp.getCode(), exp.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            throw new MRException(MediaStoreServiceExceptionCodes.CREATE_MEDIA_STORE_REQUEST_FAILED, exp.toString());
        }
    }

    public static void sendRequestToMediaStoreAgent(String mediaStoreStreamAgentJID, String mediaStoreFileNID, String mediaRoomRequestName, String mediaStreamFileNID, String mediaStreamFileDirNID, Document actionDataDoc, MRRequest request, XMPPI xmpp, MRLog log)
        throws MRException, Exception {
       
        String mediaStoreRequestName = MediaStoreServiceConst.mediaroomRequestToMediaStoreRequestMapping.get(mediaRoomRequestName);
        String actionData = "";
        if(actionDataDoc != null) {
        	actionData = MRXMLUtils.documentToStringOmitDeclaration(actionDataDoc);	
        }        
        Document mediaStoreRequestDoc = generateRequestDocument(mediaStoreRequestName, mediaStoreFileNID, mediaStreamFileNID, mediaStreamFileDirNID, actionData, request.getNID(), log);
       
        
        Document mrRequestDoc = request.marshalRequestObject(log);
        String mrRequestStr = URLEncoder.encode(MRXMLUtils.documentToString(mrRequestDoc), "UTF-8");
        MRXMLUtils.setValueXpath(request.getMessage().getDocument(), "//"+MRConst.CLIENT_DATA, mrRequestStr);
        String clientData = URLEncoder.encode(MRXMLUtils.documentToString(request.getMessage().getDocument()), "UTF-8");
        String fromAgentName = AppServerAgent.class.getName();
	    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, mediaStoreStreamAgentJID, MRConst.MEDIA_STORE_ADMIN_SERVICE, mediaStoreRequestName, MRXMLUtils.documentToStringOmitDeclaration(mediaStoreRequestDoc),
	        fromAgentName + ": Sending " + mediaStoreRequestName + " to " + 
	        MRConst.MEDIA_STORE_ADMIN_SERVICE, request.getNID(), request.getUserJID(), 
	        MRConst.MEDIA_STORE_ADMIN_SERVICE, clientData);
	    
    }

    //Loads the Media Store Request or Response XML
    public static Document loadMediaStoreRequestOrResponseDocument(String fileName, MRLog log)
        throws MRException, Exception {
        Document requestDocument = null;
        String xmlFilePath = "/com/ipvs/mediastoreservice/xml/" + fileName + ".xml";
        requestDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);

        return requestDocument;
    }

    //Loads the Media Store Request or Response XML
    public static Document loadMediaStoreInfoDocument(MRLog log) throws MRException, Exception {
        Document requestDocument = null;
        String xmlFilePath = "/com/ipvs/xsd/objectXml/MediaStoreInfo.xml";
        requestDocument = MRXMLUtils.loadXMLResource(xmlFilePath, log);
        MRXMLUtils.setAttributeValue(requestDocument.getRootElement(), MediaStoreServiceConst.SERVICE_VERSION, String.valueOf(MediaVersionManager.getServiceVersion(log)));

        return requestDocument;
    }

    public static Document loadMediaStoreObjectXML(MRLog log) throws MRException, Exception {
        Document fileDoc = null;
        String xmlFilePath = "/com/ipvs/xsd/objectXml/MediaStoreFile.xml";
        fileDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);

        return fileDoc;
    }

    public static Document loadMediaSegmentInfoDocument(MRLog log) throws MRException, Exception {
        Document fileDoc = null;
        String xmlFilePath = "/com/ipvs/xsd/objectXml/MediaSegmentInfo.xml";
        fileDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);

        return fileDoc;
    }

    public static Document loadBookmarkDocument(MRLog log) throws MRException, Exception {
        Document fileDoc = null;
        String xmlFilePath = "/com/ipvs/xsd/objectXml/Bookmark.xml";
        fileDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);

        return fileDoc;
    }

    //Create Media Store Event
    public static String createMediaStoreEventMessage(String actiondata, String mediaStreamFileResourceNID, String mediaStoreNID, int eventCode) {
        return "<data> " + "<action>" + MediaStoreServiceConst.MEDIA_STORE_EVENT + "</action>" + "<eventCode>" + eventCode + "</eventCode>" + "<" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID + ">" + mediaStreamFileResourceNID + "</" +
        MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID + ">" + "<" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID + ">" + mediaStoreNID + "</" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID + ">" + "<" + MediaStoreServiceConst.MEDIA_STORE_EVENT + "ActionData>" + actiondata + "</" +
        MediaStoreServiceConst.MEDIA_STORE_EVENT + "ActionData>" + "</data>";
    }

    //Send Message to source Stream Agent
    public static void sendMessageToSourceStreamAgent(String userJID, int eventCode, String mediaStreamFileNID, String mediaStoreFileNID, Document responseDoc, XMPPI xmpp, MRLog log)
        throws MRException, Exception {
        String info = userJID + " > Request action=MediaStoreEvent" + " to " + ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.SOURCESTREAMAGENT);
        String request = createMediaStoreEventMessage(MRXMLUtils.documentToStringOmitDeclaration(responseDoc), mediaStreamFileNID, mediaStoreFileNID, eventCode);
        ServiceAgentUtils.sendAgentEventMessage(xmpp, log, null, userJID,MRConst.MEDIA_SERVICE, MediaStoreServiceConst.MEDIA_STORE_EVENT, MRXMLUtils.stringToElement(request), info);
    }

    //returns the handler name
    public static String getHandlerClassName(String requestName) {
        return "com.ipvs." + ServiceClient.MEDIASTORESERVICE + ".handler." + requestName + "Handler";
    }
    
    public static void updateElement(MRRequestI request, MRLog log, MRDB db, String parentNID, Element element) throws MRException, Exception  {
    	// System.out.println("MediaStoreServiceUtils:updateElement element=" + MRXMLUtils.elementToString(element));
    	MRXMLDBUtils.updateElement(request, log, db, parentNID, element);
    }

    //Update Media Stream File Document in DataBase
    public static void updateMediaStreamFileDocInDB(Document mediaStreamFileDoc, String fileNID, String userJID, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        MRRequest request = MRRequest.createRequest(userJID,  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory));
        updateDateModified(mediaStreamFileDoc, xmpp);
        MediaStoreServiceUtils.updateElement(request, log, db, fileNID, mediaStreamFileDoc.getRootElement());
    }

    // update media stream file with given state and revision
    public static void updateMediaStreamFileDoc(String mediaStreamFileNID, int revision, String state, String mediaStoreFileNID, String userJID, XMPPI xmpp, MRLog log, MRDB db, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Document mediaStreamFileDoc = getMediaStreamFileDoc(mediaStreamFileNID, MRRequest.createRequest(userJID,  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory)), log, db);
        updateRevision(mediaStreamFileDoc, revision);
        updateState(mediaStreamFileDoc, state);

        MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaStreamFileDoc, mediaStreamFileNID, userJID, xmpp, log, db, serviceAgentFactory);
        log.addInfo("Update Media Stream File document With State :=" + state + " and Revision:=" + revision, MRLog.INFO, MRLog.NOTIFY);
    }

    // update media stream file with given state and revision
    public static void updateMediaStreamFileState(String mediaStreamFileNID, String state, String mediaStoreFileNID, String userJID, XMPPI xmpp, MRLog log, MRDB db)
        throws MRException, Exception {
    	String statusXML = MRXMLUtils.generateXML(AMConst.STATEFLAG, state);
    	MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, MRRequest.createRequest(userJID), log, db, "", "", mediaStreamFileNID, "//"+AMConst.STATEFLAG, new String[]{}, MRXMLUtils.stringToElement(statusXML));
        
    	/*Document mediaStreamFileDoc = getMediaStreamFileDoc(mediaStreamFileNID, MRRequest.createRequest(userJID,  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory)), log, db);
        updateState(mediaStreamFileDoc, state);
        MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaStreamFileDoc, mediaStreamFileNID, userJID, xmpp, log, db);*/
        
        
        log.addInfo("Update Media Stream File document With State :=" + state, MRLog.INFO, MRLog.NOTIFY);
    }

    /*// update media stream file with given revision
    public static void updateMediaStreamFileDoc(String mediaStreamFileNID, int revision, String mediaStoreFileNID, String userJID, XMPPI xmpp, MRLog log, MRDB db)
        throws MRException, Exception {
        Document mediaStreamFileDoc = getMediaStreamFileDoc(mediaStreamFileNID, MRRequest.createRequest(userJID,  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory)), log, db);
        updateRevision(mediaStreamFileDoc, revision);
        MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaStreamFileDoc, mediaStreamFileNID, userJID, xmpp, log, db);
        log.addInfo("Update Media Stream File Document With Revision:=" + revision, MRLog.INFO, MRLog.NOTIFY);
    }*/

    // Set the latest revision in media stream file store info element
    public static void updateRevision(Document mediaStreamFileDoc, int revision)
        throws MRException, Exception {
        Element mediaStoreInfoEL = MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT);

        if (mediaStoreInfoEL == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_INFO_NOT_FOUND, "Media Store Info Not Found");
        }

        MRXMLUtils.setAttributeValue(mediaStoreInfoEL, MediaStoreServiceConst.MEDIA_STORE_FILE_REVISION, String.valueOf(revision));
    }

    // Set the latest revision in media stream file store info element
    public static void updateState(Document mediaStreamFileDoc, String state)
        throws MRException, Exception {
        if (state != null) {
            MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//State/StateFlag", state);
        }
    }
    public static void updateStateMsg(Document mediaStreamFileDoc, String stateMsg)
            throws MRException, Exception {
            if (stateMsg != null) {
                MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//State/StateMsg", stateMsg);
            }
        }

    //Add Object in Data Base
    public static final Element addAssetAdminObjectInDB(String objectType, MRRequest request, Element objectEL, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document assetRequestDoc = AssetAdminServiceUtils.createAssetAdminServiceRequest(objectType + "_BASE_ALL_ADD.xml", (Element) objectEL.clone(), log);
        MRDBHelper amUtil = new MRDBHelper();

        Document assetResponseDoc = MRXMLUtils.getDocument();
        MRXMLUtils.addElement(assetResponseDoc, request.getResponseDataName());

        amUtil.doInit(ServiceClient.ASSETADMINSERVICE, objectType, assetResponseDoc, request, xmpp, log, db, resourceFactory, profileFactory);

        Element actionEL = MRXMLUtils.getElementXpath(assetRequestDoc, "//AddRequest");
        amUtil.doAdd(actionEL, "AddResponse", objectType, null);
        objectEL = MRXMLUtils.getElementXpath(assetResponseDoc, "//" + objectType);

        if (objectEL == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.ADD_ASSET_ADMIN_OBJECT_FAILED, "Add Object Failed");
        }

        return objectEL;
    }

    public static List<?> getBusyOrErrorFiles(String parentMediaDirNID, String pvrDirNID,String recycleBinDirNID,String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
        String whereXpath = 
                "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE +"[(//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+
                  "[.='"+parentMediaDirNID+"' or .= '"+pvrDirNID+"'   or .= '"+recycleBinDirNID+"']) and " +
                  "(//"+AMConst.STATEFLAG+"[.='"+MediaStoreServiceConst.ERROR+"' or .='"+MediaStoreServiceConst.BUSY+"'])]";
        return getMediaStreamFileResourceList(whereXpath, userJID, log, db);
    }
    
    
    public static List<?> getWritableFiles(String parentMediaDirNID, String pvrDirNID,String recycleBinDirNID,String userJID, MRLog log, MRDB db)
            throws MRException, Exception {
            String whereXpath = 
                    "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE +"[(//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+
                      "[.='"+parentMediaDirNID+"' or .= '"+pvrDirNID+"'   or .= '"+recycleBinDirNID+"']) and " +
                      "(//"+MediaStoreServiceConst.WRITE+"='"+MediaStoreServiceConst.TRUE+"')]";
            return getMediaStreamFileResourceList(whereXpath, userJID, log, db);
        }

    
    public static List<?> getMediaStreamFileResourceList(String[] whereXpath, String userJID, MRLog log, MRDB db)
    throws MRException, Exception {
    	Element response = MRXMLUtils.newElement("MediaStreamFileList");
    	MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
    	String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

    	MRXMLDBUtils.findChildElements(parentNID, response, null, 0, null, whereXpath, MRRequest.createRequest(userJID), db, log);

    	List<?> mediaStreamFileList = MRXMLUtils.getListXpath(MRXMLUtils.elementToDocument(response), "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);

    	if ((mediaStreamFileList == null) || mediaStreamFileList.isEmpty()) {
    		return null;
    	}

    	return mediaStreamFileList;
    }
    public static Document getMediaStreamFileResourceDoc(String[] whereXpath, String userJID, MRLog log, MRDB db)
    		throws MRException, Exception {    	
    	Element response = MRXMLUtils.newElement("MediaStreamFileList");
    	MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
    	String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

    	MRXMLDBUtils.findChildElements(parentNID, response, null, 0, null, whereXpath, MRRequest.createRequest(userJID), db, log);

    	return MRXMLUtils.elementToDocument(response);
    }
    public static List<?> getMediaStreamFileResourceList(String whereXpath, String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
    	try {
    		long dbtimeout = MRXMLDBUtils.TIME_OUT;
    		MRRequest request = MRRequest.createRequest(userJID);
    		Hashtable<String, String> table = new Hashtable<String, String>();
    		String query = 	FileUtils.readFileResource(MediaStoreServiceConst.GET_FILE_LIST_XQUERY, request.getClass());
    		query = query.replace(MediaStoreServiceConst.XPATH, whereXpath);
    		String mediaStreamFileResourceListStr = MRXMLDBUtils.readQuery(dbtimeout, request, log, db, query, table, null);
    		if(mediaStreamFileResourceListStr!= null) {
    			Document mediaStreamFileResourceListDoc = MRXMLUtils.stringToDocument(mediaStreamFileResourceListStr);
    			//return mediaStreamFileResourceListDoc.getRootElement().getChildren("MediaStreamFileResource");
    			return MRXMLUtils.getChildren(mediaStreamFileResourceListDoc.getRootElement(), "MediaStreamFileResource");
    		}	
    	} catch (Exception e) {
    		e.printStackTrace();
    		
    	}
    	return null;
    }

    public static List<?> getMediaStreamFileResourceNIDList_readonly(String mediaStreamDirNID, String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
        Document docRead = MRXMLUtils.getDocument();
        MRXMLUtils.addElement(docRead, "MediaStreamFileList");
        Element dbRead = MRXMLUtils.addElement(docRead.getRootElement() , "ResponseXML");
        String[] arrWhereXpathRead={"//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME + "='" + mediaStreamDirNID + "'"
                ,MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.PROPERTIES_EL_NAME 
                + "/" + MediaStoreServiceConst.PERMISSION_EL_NAME + "/" + MediaStoreServiceConst.WRITE + "='false'"};
        MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);
        String listName = config.getParentNIDKey(log);
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
        String childXPath = null;
        String selectXML = "<"+AMConst.MEDIA_STREAM_FILE_RESOURCE+" NID='{data($VAR/@NID)}' mediaGroupID='{data($VAR//MediaGroupID)}'  />";
        MRXMLDBUtils.readList(MRXMLDBUtils.TIME_OUT, MRRequest.createRequest(userJID), log, db, listName, 
                AMConst.MEDIA_STREAM_FILE_RESOURCE, parentNID, childXPath, arrWhereXpathRead, null, null, selectXML, dbRead, 0, -1);
        List<?> mediaStreamFileList = MRXMLUtils.getListXpath(dbRead, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
        if ((mediaStreamFileList == null) || mediaStreamFileList.isEmpty()) {
            return Collections.emptyList();
        }
        return mediaStreamFileList;
    }
    
    public static List<?> getListOfMediaStreamFilesInDir(String parentMediaDirNID,String pvrDirNID,String recycleBinDirNID,String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
    	String whereXPath = "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME+"[.='"+parentMediaDirNID+"'  " +
    			"  or .= '"+pvrDirNID+"'   or .= '"+recycleBinDirNID+"']"; 
        return getMediaStreamFileResourceList(whereXPath, userJID, log, db);
    }

    public static List<?> getMediaStreamFileResourceListWithoutGroupsAndPlaylists(String parentMediaDirNID, String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
		String whereXpath =  "[(//" + AMConst.MEDIA_STREAM_FILE_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.PROPERTIES_EL_NAME + "/" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME + "='" + parentMediaDirNID + "') and (//" +
        MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "/" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME + " = '" + MediaRoomServiceConst.MEDIACLIP + "' or //" +
        MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "/" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME + " = '" + MediaRoomServiceConst.MEDIAIMAGE + "') ]";
        return getMediaStreamFileResourceList(whereXpath, userJID, log, db);
    }

    //check media store response passed or failed.
    public static boolean validateMediaStoreSynResponse(Document responseDoc)
        throws MRException, Exception {
        Integer state = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(responseDoc,MRConst.HEADER_XPATH,MRConst.STATE, ""), MediaStoreServiceConst.FAILURE_STATE);

        if (state == MediaStoreServiceConst.FAILURE_STATE) {
            return false;
        }

        return true;
    }

    public static void sendSynchRequestToAppServerAgent(String requestName, Document mediaStreamFileDoc, String mediaStreamFileNID, MRDB db, MRLog log, XMPPI xmpp, MRRequest request)
        throws MRException, Exception {
        Document assetRequestDoc = AssetAdminServiceUtils.getAssetAdminRequest(log, requestName + ".xml");
        Element requestEL = MRXMLUtils.getElementXpath(assetRequestDoc, "//" + requestName + "Data");
        MRXMLUtils.replaceContentWithElement(requestEL, mediaStreamFileDoc.getRootElement());
        if (mediaStreamFileNID != null) {
            Element mediaStreamFileNIDEL = MRXMLUtils.newElement(MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID);
            MRXMLUtils.setValue(mediaStreamFileNIDEL, mediaStreamFileNID);
            MRXMLUtils.addElement(requestEL, (Element) mediaStreamFileNIDEL.clone());
        }

        MRXMLUtils.setValueXpath(assetRequestDoc, "//"+MRConst.CLIENT_DATA, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + AMConst.MEDIA_STORE_PORT_RESOURCE_NID, ""));
        MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, request.getUserJID(), assetRequestDoc, log, requestName, MediaStoreAdminServerAgent.class.getName(), request.getAppServerJID());
    }

    public static void removeChildren(Element profileInfoParent) {
        try {
            List<?> children = profileInfoParent.elements();
            ArrayList<String> childNameList = new ArrayList<String>();

            for (Iterator<?> i = children.iterator(); i.hasNext();) {
                Element child = (Element) i.next();
                String childName = child.getName();
                childNameList.add(childName);
            }

            for (Iterator<String> i = childNameList.iterator(); i.hasNext();) {
                String childName = i.next();
                MRXMLUtils.removeChild(profileInfoParent,childName);
            }
        } catch (Exception e) {
            System.out.println("Problems removing profiles from filedoc");
            e.printStackTrace();
        }
    }

    // Set the media Group ID media stream file 
    public static void updateMediaGroupID(Document mediaStreamFileDoc, String mediaGroupID)
        throws MRException, Exception {
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, mediaGroupID);
    }

    //eliminate the elements other than given group
    public static void updateInfoElementBasedOnType(Element infoEL, String mediaType) throws MRException {
        if (mediaType.equals(MediaRoomServiceConst.MEDIACLIP)) {
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PVRMEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAGROUP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PLAYLIST);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAIMAGE);
        } else if (mediaType.equals(MediaRoomServiceConst.PVRMEDIACLIP)) {
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAGROUP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PLAYLIST);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAIMAGE);
        } else if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PVRMEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PLAYLIST);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAIMAGE);
        } else if (mediaType.equals(MediaRoomServiceConst.PLAYLIST)) {
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PVRMEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAGROUP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAIMAGE);
        }  else if (mediaType.equals(MediaRoomServiceConst.MEDIAIMAGE)) {
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PVRMEDIACLIP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.MEDIAGROUP);
            MRXMLUtils.removeChild(infoEL,MediaRoomServiceConst.PLAYLIST);
        } else {
        	throw new MRException(MediaStoreServiceExceptionCodes.INVALID_MEDIA_TYPE, MRXMLUtils.generateErrorXML("mediatype", mediaType));
        }
    }

    //get media stream file nid from media stream doc
    public static String getMediaStreamFileNID(Document mediaStreamFileDoc)
        throws MRException, Exception {
        return MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), MediaStoreServiceConst.NID, "");
    }

    public static void callRepairMedia(String mediaStreamFileNID, String mediaStoreFileNID, 
    		String mediaStreamFileDirNID, MRRequest request, XMPPI xmpp, MRDB db, MRLog log, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String requestName = MRRequest.getRequestName(RepairMediaRequestHandler.class.getName());
        Document repairMediaRequestDoc = MediaStoreServiceUtils.loadMediaStoreRequestOrResponseDocument(requestName, log);
        MRXMLUtils.setValueXpath(repairMediaRequestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, mediaStreamFileNID);
        MRXMLUtils.setValueXpath(repairMediaRequestDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID, mediaStoreFileNID);
        MRXMLUtils.setValueXpath(repairMediaRequestDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, mediaStreamFileDirNID);

        MRRequest mediaStoreRequest = MRRequest.createRequest(request, repairMediaRequestDoc, requestName, ServiceClient.MEDIASTORESERVICE);
        MRRequestHandlerI handler = (MRRequestHandlerI) Class.forName(MediaStoreServiceUtils.getHandlerClassName(requestName)).newInstance();
        handler.getResponse(mediaStoreRequest, xmpp, log, db, resourceFactory, serviceAgentFactory);        
    }

    //Add media stream file and corresponding book marks    
    public static String synchAddMediaFile(Document mediaFileDoc, String mediaStoreFileNID, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreDirectoryInfo storeInfo, String userJID, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        Document mediaStreamFileDoc = MRXMLUtils.getDocumentXpath(mediaFileDoc, "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);

        if (mediaStreamFileDoc == null) {
            return null;
        }

        String mediaStreamFileNID = synchAddMediaStreamFile(mediaStreamFileDoc, mediaStoreFileNID, mediaStoreAndStreamMapping, storeInfo, userJID, log, db, xmpp, resourceFactory, serviceAgentFactory);
        MRXMLUtils.setAttributeValueXPath(mediaFileDoc, "//"+AMConst.MEDIA_STREAM_FILE_RESOURCE, MediaStoreServiceConst.NID, mediaStreamFileNID);
        Document bookMarkListDoc = MRXMLUtils.getDocumentXpath(mediaFileDoc, "//" + MediaStoreServiceConst.BOOKMARK_LIST);

        if (bookMarkListDoc != null) {
            List<?> bookMarkList = MRXMLUtils.getListXpath(bookMarkListDoc, "//" + AMConst.BOOKMARK);

            if ((bookMarkList != null) && !bookMarkList.isEmpty()) {
                Element bookMarkEL = null;
                MRRequest request = MRRequest.createRequest(userJID);
                log.addInfo("Add Bookmarks. StoreUUID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);

                for (Object bookMarkObject : bookMarkList) {
                    bookMarkEL = (Element) bookMarkObject;
                    MRXMLUtils.setValueXpath(bookMarkEL, MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, mediaStreamFileNID);
                    MediaStoreServiceUtils.addAssetAdminObjectInDB(AMConst.BOOKMARK, request, bookMarkEL, log, db, xmpp, resourceFactory, serviceAgentFactory);
                }
            }
        }

        return mediaStreamFileNID;
    }

    //Synch add media stream file
    public static String synchAddMediaStreamFile(Document mediaStreamFileDoc, String mediaStoreFileNID, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreDirectoryInfo storeInfo, String userJID, MRLog log, MRDB db, XMPPI xmpp, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
        MRRequest request = MRRequest.createRequest(userJID);

        Element mediaStreamFileEL = MediaStoreServiceUtils.addAssetAdminObjectInDB(AMConst.MEDIA_STREAM_FILE_RESOURCE, request, mediaStreamFileDoc.getRootElement(), log, db, xmpp, resourceFactory, serviceAgentFactory);
        mediaStreamFileDoc.clearContent();
        MRXMLUtils.addElement(mediaStreamFileDoc, (Element) mediaStreamFileEL.clone());

        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileEL, MediaStoreServiceConst.NID, "");
        mediaStoreAndStreamMapping.put(mediaStoreFileNID, mediaStreamFileDoc);
        storeInfo.incrementNotSynchCount();
        log.addInfo("Add Media Stream File. StoreUUID :=" + mediaStoreFileNID +" mediaStreamFileNID:="+mediaStreamFileNID , MRLog.INFO, MRLog.NOTIFY);

        return mediaStreamFileNID;
    }

    // synch update media stream file
    public static void synchUpdateMediaStreamFile(Document mediaStreamFileDoc, String mediaStoreFileNID, HashMap<String, Document> mediaStoreAndStreamMapping, MediaStoreDirectoryInfo storeInfo, String userJID, MRLog log, MRDB db, XMPPI xmpp, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), "NID", "");
        MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaStreamFileDoc, mediaStreamFileNID, userJID, xmpp, log, db, serviceAgentFactory);
        mediaStoreAndStreamMapping.put(mediaStoreFileNID, mediaStreamFileDoc);
        storeInfo.incrementNotSynchCount();
        log.addInfo("Update Media Stream File. StoreUUID :=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
    }

    // synch update media stream file
    public static void synchDeleteMediaStreamFile(Document mediaStreamFileDoc, String userJID, MRLog log, MRDB db, XMPPI xmpp)
        throws MRException, Exception {
        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaStreamFileDoc.getRootElement(), "NID", "");
        List<String> mediaStreamFileResourceNIDList = new ArrayList<String>();
        mediaStreamFileResourceNIDList.add("'"+mediaStreamFileNID+"'");
        MRRequest request = MRRequest.createRequest(userJID);
        MediaManagementUtils.deleteBookmarks(mediaStreamFileResourceNIDList, request, db, log);
        MRXMLDBUtils.deleteElement(request, log, db, mediaStreamFileNID);
        log.addInfo("Synch Delete Media Stream File. mediaStreamFileNID :=" + mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);
    }

    public static List<?> getReadOnlyMediaStreamFileResourceList(String mediaGroupID, String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
        StringBuffer xPath = new StringBuffer("//" + AMConst.MEDIA_STREAM_FILE_RESOURCE);
        xPath.append("[");

        if ((mediaGroupID != null) && !mediaGroupID.isEmpty()) {
            xPath.append("(" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "/" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "='" + mediaGroupID + "')  and ");
            xPath.append("(" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "/" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME + "!='" + MediaRoomServiceConst.MEDIAGROUP + "')  and ");
        }
        
        xPath.append("(" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.PROPERTIES_EL_NAME + "/" + MediaStoreServiceConst.PERMISSION_EL_NAME + "/" + MediaStoreServiceConst.WRITE + "='false')");

        xPath.append("]");

        String[] whereXpath = { xPath.toString() };

        return getMediaStreamFileResourceList(whereXpath, userJID, log, db);
    }

    public static void updateMediaStreamFilePermissions(Document mediaStreamFileDoc, String mediaStreamFileNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.WRITE, String.valueOf(true));
        updateMediaStreamFileDocInDB(mediaStreamFileDoc, mediaStreamFileNID, request.getUserJID(), xmpp, log, db, serviceAgentFactory);
        log.addInfo("Update Media Stream File document Permissions. MediastreamFileResourceNID:=" + mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);
    }

    public static void updateDateModified(Document mediaStreamFileDoc, XMPPI xmpp)
        throws MRException, Exception {
        long xmpp_date = xmpp.getCurrentTimeMillis();
        MRXMLUtils.setValueXpath(mediaStreamFileDoc, "//Info/Properties/DateModified", String.valueOf(xmpp_date));
    }

    public static Element getMediaStreamDirFromDataBase(String whereXpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        List<?> objectList = getMediaStreamDirList(whereXpath, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        if ((objectList == null) || objectList.isEmpty()) {
            return null;
        }

        return (Element) objectList.get(0);
    }
    
    
    public static String getRecycleBinNID(String parentDirNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws MRException, Exception {

    	String whereXpath = "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "[(" +
    	MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME + "='" + MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME + "')" + " and (" +
    	MediaStoreServiceConst.PROPERTIES_EL_NAME + "/" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME + "='" + parentDirNID + "')]";

    	String recycleBinNID = null;

    	Element mediaStreamDirEL = MediaStoreServiceUtils.getMediaStreamDirFromDataBase(whereXpath, request, xmpp, log, db, resourceFactory,
    			serviceAgentFactory);

    	if (mediaStreamDirEL != null) {
    		recycleBinNID = MRXMLUtils.getAttributeValue(mediaStreamDirEL, MediaStoreServiceConst.NID, "");
    	}

    	if ((recycleBinNID == null) || recycleBinNID.isEmpty()) {
    		throw new MRException(AMExceptionCodes.RECYCLE_BIN_NOT_CONFIGURED, "Recycle Bin Not configured in Parent Dir");
    	}
    	return recycleBinNID;
    }

    public static List<?> getMediaStreamDirList(String whereXpath, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Element response = MRXMLUtils.newElement(AMConst.MEDIA_STREAM_DIR_RESOURCE + "List");
        MRConfig config = MRConfig.getInstance(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_DIR_RESOURCE, log);
        String parentNID = db.getServiceDomainRootNID(config.getParentNIDKey(log));
        String[] xPath = { whereXpath };
        MRXMLDBUtils.findChildElements(parentNID, response, null, 0, null, xPath, request, db, log);

        List<?> objectList = MRXMLUtils.getListXpath(MRXMLUtils.elementToDocument(response), "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE);

        return objectList;
    }

    public static void updateMediaStreamDirResourceState(String dirNID, String state, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        if ((dirNID == null) || dirNID.isEmpty()) {
            return;
        }

        Document mediaStreamDirDoc = MRXMLDBUtils.readElement(request, log, db, dirNID);
        updateMediaStreamDirResourceState(mediaStreamDirDoc, state, request, xmpp, log, db, resourceFactory, profileFactory);
    }

    public static void updateMediaStreamDirResourceState(Document mediaStreamDirDoc, String state, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI profileFactory) throws MRException, Exception {
    	MediaStoreServiceUtils.updateMediaStreamDirResourceStateAndMsg(mediaStreamDirDoc, state, null, request, xmpp, log, db, resourceFactory, profileFactory);
    }

    public static void updateMediaStreamDirResourceStateAndMsg(Document mediaStreamDirDoc, String state, 
    		String stateMsg, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory,
        ServiceAgentFactoryI profileFactory) throws MRException, Exception {
        request.setSuperUser(true);

        long xmpp_time = xmpp.getCurrentTimeMillis();
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);
        if(stateMsg != null) {
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEMSG, stateMsg);
        }
        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, String.valueOf(xmpp_time));

        String title = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");
        String dirNID = MRXMLUtils.getAttributeValue(mediaStreamDirDoc.getRootElement(), MediaStoreServiceConst.NID, "");
        AssetAdminServiceUtils.updateAssetAdminObjectInDB(AMConst.MEDIA_STREAM_DIR_RESOURCE, dirNID, (Element) mediaStreamDirDoc.getRootElement().clone(), request, log, db, xmpp, resourceFactory, profileFactory);
        request.setSuperUser(false);
        log.addInfo("Update MediaStreamDirResoource Title:=" + title + " State:=" + state, MRLog.INFO, MRLog.NOTIFY);
    }

    //return the list of sub directories for given dir nid.
    public static List<?> getSubDirList(String dirNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        String whereXpath = "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.PROPERTIES_EL_NAME + "[" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME + " ='" + dirNID + "']";

        return MediaStoreServiceUtils.getMediaStreamDirList(whereXpath, request, xmpp, log, db, resourceFactory, profileFactory);
    }

     //return the list of sub directories for given dir nid.
    public static String getPVRDirNID(String dirNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String whereXpath = "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "[" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME + "='" +
            MediaStoreServiceConst.PVR_DIR_NAME + "' and  " + MediaStoreServiceConst.PROPERTIES_EL_NAME + "/" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME + " ='" + dirNID + "']";
        Element pvrDirEL = getMediaStreamDirFromDataBase(whereXpath, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        if (pvrDirEL == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.PVR_MEDIA_DIR_NOT_FOUND, "PVR Media Dir Not found");
        }

        return MRXMLUtils.getAttributeValue(pvrDirEL, MediaStoreServiceConst.NID, "");
    }

    public static void updateMediaStreamDirResourceStateAndPresence(Document mediaStreamDirDoc, String mediaStorePortResourceNID, String state, String presence, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
    throws MRException, Exception {
        String xPath = "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + " [.='" + mediaStorePortResourceNID + "']";
        List<?> listOfMediaStoreEntries = MRXMLUtils.getListXpath(mediaStreamDirDoc, xPath);

        if ((listOfMediaStoreEntries != null) && !listOfMediaStoreEntries.isEmpty()) {
            Element objEL = null;

            for (Object object : listOfMediaStoreEntries) {
                objEL = (Element) object;
                MRXMLUtils.setAttributeValue(objEL, MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME, presence);
            }
        }

        if (state.equals(MRClientUtils.ASSET_STATE_OFFLINE)) {
            String synchMediaStorePortNID = MRXMLUtils.getAttributeValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME, MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, "");
            String dirState = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//" + AMConst.STATEFLAG, "");

            //check the synch interrupted in the process.
            if (synchMediaStorePortNID.equals(mediaStorePortResourceNID) && dirState.equals(MediaStoreServiceConst.BUSY)) {
                log.addInfo("Synch Interrupted. Set the dir state as OFFLINE", MRLog.INFO, MRLog.NOTIFY);
                MRXMLUtils.setAttributeValueXPath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME, MediaStoreServiceConst.SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME, "");
                MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);
            } else {
                updateDirState(mediaStreamDirDoc);
            }
        } else {
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, state);
        }

        String title = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");
        MediaStoreServiceUtils.updateMediaStreamDirDocInDB(mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);
        log.addInfo("Update MediaStreamDirResoource Title:=" + title + " State:=" + state + " And Presence :=" + presence, MRLog.INFO, MRLog.NOTIFY);
    }

    public static void updateDirState(Document mediaStreamDirDoc) throws MRException, Exception {
        String presenceXpath = "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + "[@" + MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME + "='" + AMConst.AVAILABLE + "']";
        Element presenceEL = MRXMLUtils.getElementXpath(mediaStreamDirDoc, presenceXpath);

        if (presenceEL == null) {
            MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + AMConst.STATE + "/" + AMConst.STATEFLAG, MRClientUtils.ASSET_STATE_OFFLINE);
        }
    }

    public static void updateMediaStreamDirResourcePresence(Document mediaStreamDirDoc, String mediaStorePortResourceNID, String presence, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        String xPath = "//" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + " [.='" + mediaStorePortResourceNID + "']";
        List<?> listOfMediaStoreEntries = MRXMLUtils.getListXpath(mediaStreamDirDoc, xPath);

        if ((listOfMediaStoreEntries != null) && !listOfMediaStoreEntries.isEmpty()) {
            Element objEL = null;

            for (Object object : listOfMediaStoreEntries) {
                objEL = (Element) object;
                MRXMLUtils.setAttributeValue(objEL, MediaStoreServiceConst.PRESENCE_ATTRIBUTE_NAME, presence);
            }
        }

        String title = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");

        MediaStoreServiceUtils.updateMediaStreamDirDocInDB(mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);

        log.addInfo("Update MediaStreamDirResoource Title:=" + title + " And storeProt :=" + mediaStorePortResourceNID + " Presence :=" + presence, MRLog.INFO, MRLog.NOTIFY);
    }

    public static void updateStorePresenceInDir(String mediaStorePortNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, String state, String presence)
        throws MRException, Exception {
        String whereXpath = "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.MEDIA_DIR_STORAGE_CONFIG + "/" + MediaStoreServiceConst.MEDIA_DIR_SPANNING + "/" + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME + "[ " + MediaStoreServiceConst.MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME + " = '" + mediaStorePortNID + "']";
        List<?> mediaDirList = MediaStoreServiceUtils.getMediaStreamDirList(whereXpath, request, xmpp, log, db, resourceFactory, serviceAgentFactory);

        if ((mediaDirList == null) || mediaDirList.isEmpty()) {
            return;
        }

        Element dirEL = null;

        for (Object object : mediaDirList) {
            dirEL = (Element) object;
            String dirStateInDB = MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument(dirEL), "//State/StateFlag", "");
            if(MediaStoreServiceConst.DETACHED.equalsIgnoreCase(dirStateInDB))
            	state = dirStateInDB;
            
            if( presence.equals(AMConst.UNAVAILABLE) ) {
            	String NID = MRXMLUtils.getAttributeValue(dirEL, MediaStoreServiceConst.NID, "");
            	log.addInfo("updateStorePresenceInDir::Deleting the existing Dir task if any", MRLog.OK, MRLog.NOTIFY);
            	AppServerAgentUtils.getScheduleManager(serviceAgentFactory).deleteTaskFromScheduler(request, xmpp, log, db, resourceFactory, serviceAgentFactory, NID);
            	String objectType = AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE;
            	MRConfig mrConfig = MRConfig.getInstance(objectType, log);
            	String listName = mrConfig.getParentNIDKey(log);
            	String parentNID = db.getServiceDomainRootNID(listName);

            	//To resume the Queue for the files that are stuck in state other than completed or queued 
            	Element queueElement =  FileSMUtils.getQueueElement(null, request, xmpp, log, db);
            	if(queueElement!=null){               	

            		List<?> exportQueue = MRXMLUtils.getListXpath(queueElement, "//ExportImportFileListItem[(@type='" + FileSM.TYPE_IMPORT + "' and @action='"+FileSM.ACTION_IMPORT_UNPACK+"') or (@type= '"+FileSM.TYPE_EXPORT+"' and @action='"+FileSM.ACTION_EXPORT_PACK+"')][./Internal[@dirNID='"+NID+"']]");
            		if(exportQueue!=null && exportQueue.size()>0){
            			for (Object queueObject : exportQueue) {
            				Element queueEL = (Element)queueObject;
            				String queueState = MRXMLUtils.getAttributeValue(queueEL, "state", "");
            				String action =MRXMLUtils.getAttributeValue(queueEL, "action", "");
            				int queue_state =Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "queue_state", ""),0);

            				if(FileSM.TYPE_IMPORT.equals(MRXMLUtils.getAttributeValue(queueEL, "type", ""))){
            					queueState = FileSM.STATE_READY;                			
            					action = FileSM.ACTION_READY;
            					queue_state = FileSMQueue.QUEUE_STATUS_ERROR;
            				}
            				queue_state = FileSMQueue.QUEUE_STATUS_ERROR;
            				String error = "DMS is unreachable, please suspend and resume when it comes online: '" + FileSMUtils.getActionDescription(MRXMLUtils.getAttributeValue(queueEL, "action", ""),log) + "'";
            				int errorCode = AMExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
            				FileSM.setQueueState(queueEL, FileSMUtils.getActionDescription(FileSM.ACTION_ERROR, log), 
            						queueState, action, queue_state);
            				int errorCount = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "errorCount", ""), 0)+1;
            				MRXMLUtils.addAttribute(queueEL, "errorCount", (errorCount) + "");
            				log.addInfo("FileSM:on DMS reboot : " + errorCode + " " + error , MRLog.INFO, MRLog.NOTIFY);
            				FileSMUtils.addErrorToQueueElement(queueEL, errorCode, error);
            				FileSMUtils.updateQueueElement_db(queueEL, request, db, log);                			
            			}
            		}
            	}
            }
            MediaStoreServiceUtils.updateMediaStreamDirResourceStateAndPresence(MRXMLUtils.elementToDocument(dirEL), mediaStorePortNID, state, presence, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
        }
    }

    public static void updateMediaStreamDirSizeUsed(String dirNID, long sizeUsed, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
    	
        
    	String xml = MRXMLUtils.generateXML(MediaStoreServiceConst.MEDIA_DIR_SIZE_USED, String.valueOf(sizeUsed));
    	MRXMLDBUtils.replace(MRXMLDBUtils.TIME_OUT, request, log, db, "", "", dirNID, "//"+MediaStoreServiceConst.MEDIA_DIR_SIZE_USED,
    			new String[]{}, MRXMLUtils.stringToElement(xml));
    	
        //MediaStoreServiceUtils.updateMediaStreamDirDocInDB(mediaStreamDirDoc, request, xmpp, log, db, resourceFactory, profileFactory);

        log.addInfo("Update Media Dir SizeUsed :=" + sizeUsed + " DirNID:=" + dirNID, MRLog.INFO, MRLog.NOTIFY);
    }

    public static Document getHardDiskStatus(MRLog log) {
        String[] cmdArr = { MediaStoreServiceConst.GET_DISK_SPACE_CLI_CMD };
        Document cmdResultDoc = ServiceAgentUtils.executeCLICommand(cmdArr, log);

        return cmdResultDoc;
    }

    public static Document getNASDriveStatus(MRLog log) {
        String[] cmdArr = { MediaStoreServiceConst.GET_NAS_STATUS_CLI_CMD };
        Document cmdResultDoc = ServiceAgentUtils.executeCLICommand(cmdArr, log);

        return cmdResultDoc;
    }

    public static boolean checkDiskStatus(Document cmdResultDoc) throws MRException, Exception {
        String status = MRXMLUtils.getValueXpath(cmdResultDoc, "//Parameter[@name='Status']", "");

        if (Utils.getBoolean(status, false) || status.equals("Ok")) {
            return true;
        }

        return false;
    }

    public static Element getDiskEntry(String storePortElementID, Document nasStatus)
        throws MRException, Exception {
        if (storePortElementID.isEmpty() || nasStatus==null) {
            return null;
        }
        

        Element statusEL = MRXMLUtils.getElementXpath(nasStatus, "//Parameters[Parameter[(@name='Name' and .='" + storePortElementID + "')]]");

        return statusEL;
    }

    private static long updateDirSpaceUsed(String dirNID, Document statusDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        if (!MediaStoreServiceUtils.checkDiskStatus(statusDoc)) {
            return 0;
        }

        long sizeUsed = Utils.getLongValue(MRXMLUtils.getValueXpath(statusDoc, "//Parameter[@name='Used']", ""), 0);

        MediaStoreServiceUtils.updateMediaStreamDirSizeUsed(dirNID, sizeUsed, request, xmpp, log, db, resourceFactory, profileFactory);

        return sizeUsed;
    }

    public static Document getDiskStatusDoc(boolean isInternal, MRLog log)
        throws MRException, Exception {
        if (isInternal) {
            return MediaStoreServiceUtils.getHardDiskStatus(log);
        } else {
            return MediaStoreServiceUtils.getNASDriveStatus(log);
        }
    }

    public static long updateMediaDirSizeUsed(boolean isInternal, String storePortElementID, String dirNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        Document diskStatusDoc = getDiskStatusDoc(isInternal, log);
        Element stateEl = MediaStoreServiceUtils.getDiskEntry(storePortElementID, diskStatusDoc);

        if (stateEl == null) {
            return 0;
        }

        diskStatusDoc = MRXMLUtils.elementToDocument(stateEl);

        return updateDirSpaceUsed(dirNID, diskStatusDoc, request, xmpp, log, db, resourceFactory, profileFactory);
    }

    public static void updateMediaStreamDirDocInDB(Document mediaStreamDirDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
        throws MRException, Exception {
        request.setSuperUser(true);

        MRXMLUtils.setValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, String.valueOf(xmpp.getCurrentTimeMillis()));

        String dirNID = MRXMLUtils.getAttributeValue(mediaStreamDirDoc.getRootElement(), MediaStoreServiceConst.NID, "");
        AssetAdminServiceUtils.updateAssetAdminObjectInDB(AMConst.MEDIA_STREAM_DIR_RESOURCE, dirNID, (Element) mediaStreamDirDoc.getRootElement().clone(), request, log, db, xmpp, resourceFactory, profileFactory);
        request.setSuperUser(false);
    }

    public static Element getMediaStreamDirGroupsEL(String mediaStreamDirNID, MRRequest request, MRDB db, MRLog log)
        throws MRException, Exception {
        Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(mediaStreamDirNID, request, db, log);

        if (mediaStreamDirDoc == null) {
            return null;
        }

        return MRXMLUtils.getElementXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.GROUPS_EL_NAME);
    }

    public static void updateDirGroupsInFile(Document newMediaStreamFileDoc, Element dirGroupsEL)
        throws MRException, Exception {
    	if(dirGroupsEL == null) {
    		return;
    	}
        //set the groups 
        Element groupsEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.GROUPS_EL_NAME);
        MRXMLUtils.removeContent(groupsEL);
        MRXMLUtils.addChildrenToElement(groupsEL, dirGroupsEL);
    }

    public static List<?> getListOfFilesInMediaGroup(String mediaGroupID, String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
        String[] whereXpath = {
                "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "[(" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "='" + mediaGroupID + "') " + " and (" +
                MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME + "!='" + MediaRoomServiceConst.MEDIAGROUP + "')]"
            };

        return getMediaStreamFileResourceList(whereXpath, userJID, log, db);
    }
    
    
    //add media group from media clip information
    public static Document addMediaGroupFromMediaClip(String mediaStreamFileNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws MRException, Exception {

  		Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, request, log, db);
       	if(mediaStreamFileDoc == null) {
    		return null;
    	}
       	return addOrUpdateMediaGroupFromMediaClip(mediaStreamFileDoc, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }
    
    //add media group from media clip information
    public static Document addOrUpdateMediaGroupFromMediaClip(Document mediaStreamFileDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
    throws MRException, Exception {

  		
    	
    	String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
    	//if the file type is not group then find corresponding group file
    	if(!mediaType.equals(MediaRoomServiceConst.MEDIACLIP)) {
    		return null;  		
    	}    	
    	
        String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");     
	
    	Document mediaGroupFileDoc = getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);    		

    	if(mediaGroupFileDoc != null) {
    		return updateMediaGroupProperties(mediaGroupFileDoc, mediaGroupID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    	}
    	
    	Document newMediaStreamFileDoc = MRConfig.getObjectXMLDocument(ServiceClient.ASSETADMINSERVICE, AMConst.MEDIA_STREAM_FILE_RESOURCE, log);

    	//Remove media clip,pvr clip and play list nodes from media file doc 
    	Element infoEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.INFO_ELEMENT_NAME);
    	MediaStoreServiceUtils.updateInfoElementBasedOnType(infoEL, MediaRoomServiceConst.MEDIAGROUP);
    	
    	//update type 
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, MediaRoomServiceConst.MEDIAGROUP);

    	//update Media Group Id
    	MediaStoreServiceUtils.updateMediaGroupID(newMediaStreamFileDoc, mediaGroupID);
    
    	//Update Clip Id
    	MRXMLUtils.setAttributeValueXPath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, MediaStoreServiceConst.CLIP_ID, MRXMLUtils.getAttributeValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, MediaStoreServiceConst.CLIP_ID, ""));

    	//set the groups 
    	MediaStoreServiceUtils.updateDirGroupsInFile(newMediaStreamFileDoc, MRXMLUtils.getElementXpath(mediaStreamFileDoc, "//"+MediaStoreServiceConst.GROUPS_EL_NAME));

    	//set parent dir as NONE.
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME, "NONE");

    	//set title,description and stream type
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME, ""));
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME,  MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, ""));
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, ""));
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.READY);

    	//set the  file permissions. no write only read, xfer and stream 
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.READ, MediaStoreServiceConst.TRUE);
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.WRITE, MediaStoreServiceConst.TRUE);
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.XFER, MediaStoreServiceConst.TRUE);
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.STREAM, MediaStoreServiceConst.TRUE);

    	//set date created
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED, ""));

		//set playlength and size
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, ""));
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, ""));
    
    	//set bypass flag
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, ".//"+MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, "false"));		
		
    	log.addInfo("Create Media Group. MediaGroupID:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);
		request.setSuperUser(true);
    	Element responseEl = addAssetAdminObjectInDB(AMConst.MEDIA_STREAM_FILE_RESOURCE, request, newMediaStreamFileDoc.getRootElement(), log, db, xmpp, resourceFactory, serviceAgentFactory);
    	request.setSuperUser(false);
    	return MRXMLUtils.elementToDocument(responseEl);    	
    }

    public synchronized static boolean checkBypassFlagValueOfMediaClips(List <?>listOfFiles, MRRequest request, XMPPI xmpp, MRLog log, MRDB db)throws MRException, Exception {
		boolean isMediaGroupBypassed = false;
		if ((listOfFiles != null) && !listOfFiles.isEmpty()) {		   
		    isMediaGroupBypassed = true;
			for (Object object : listOfFiles) {
				Element fileEL = (Element) object;			
				String bypassValue = MRXMLUtils
						.getValueXpath(
								fileEL,
								".//"+ MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT,"false");
				if ("false".equalsIgnoreCase(bypassValue)) {
					isMediaGroupBypassed = false;
					break;
				}

			}
		}
		return isMediaGroupBypassed;
    	
    }
    public synchronized static boolean checkStateOfMediaClips(List <?>listOfFiles, MRRequest request, XMPPI xmpp, MRLog log, MRDB db)throws MRException, Exception {
	boolean isFileInError = true;
	if ((listOfFiles != null) && !listOfFiles.isEmpty()) {
		for (Object object : listOfFiles) {
			Element fileEL = (Element) object;
			String state = MRXMLUtils
					.getValueXpath(
							fileEL,
							".//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, "");
			if (!MediaStoreServiceConst.ERROR.equalsIgnoreCase(state)) {
			    isFileInError = false;
				break;
			}

		}
	}
	return isFileInError;
	
}

    public static Document updateMediaGroupProperties(Document mediaGroupFileDoc, String mediaGroupID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {    	   	
    	String mediaType = MRXMLUtils.getValueXpath(mediaGroupFileDoc, "//"+MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
    	//if the file type is not group then find corresponding group file
    	if(!mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
    		mediaGroupFileDoc = getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);    		
    	}    	
    	if(mediaGroupFileDoc == null) {
    		return null;
    	}
    	//log.addInfo("updateMediaGroupProperties: Aquire lock on mediaGroupID:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY); 
    	//Semaphore lock = IPVSMonitor.getMonitor().getLock(mediaGroupID, request.getNID());	
        List<?> fileList = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, db);
        List<?> fileListNotInRecycleBin = new ArrayList <Object>(fileList);
        long mediaGroupPlayLength = 0;
        long mediaGroupSize = 0;
        long recycleBinGroupPlayLength = 0;
        long recycleBinMediaGroupSize = 0;
	    boolean allfilesInRecyclebin= true;
	    String mediaGroupFileState = MediaStoreServiceConst.BUSY;
	    String mediaGroupWritePermissions = MediaStoreServiceConst.TRUE;
	    
        if ((fileList != null) && !fileList.isEmpty()) {
            Document mediaStreamFileDoc = null;
            long filePlayLength = 0;
            long fileSize = 0;
            for (Object object : fileList) {            	
        	mediaStreamFileDoc = MRXMLUtils.elementToDocument((Element) object);
        	String parentDirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
        	filePlayLength = Utils.getLongValue(MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, ""), 0);
        	fileSize = Utils.getLongValue(MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, ""), 0);

                if (isRecycleBinDir(parentDirNID, request, xmpp, log, db)) {
                	//remove the recycled file from list to send to bypass auto storage rules flag
                	fileListNotInRecycleBin.remove(object);
                    if (recycleBinGroupPlayLength < filePlayLength) {
                    	recycleBinGroupPlayLength = filePlayLength;
                    	
                	}
                	recycleBinMediaGroupSize = recycleBinMediaGroupSize + fileSize; 	                    
                } else {
                	
                    allfilesInRecyclebin = false;
                    if (mediaGroupPlayLength < filePlayLength) {
                    	mediaGroupPlayLength = filePlayLength;
                	}
                	mediaGroupSize = mediaGroupSize + fileSize; 	 
                }
                String mediaFileState = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, "");
                if(mediaFileState.equals(MediaStoreServiceConst.READY)) {
                	mediaGroupFileState = MediaStoreServiceConst.READY;
                }              
                String writePermissions = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.WRITE, "");
                if(writePermissions.equals(MediaStoreServiceConst.FALSE)) {
                	mediaGroupWritePermissions = MediaStoreServiceConst.FALSE;
                }
            }
        }
        String groupDirNID = MRXMLUtils.getValueXpath(mediaGroupFileDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
        boolean groupInDefaultDir= false;
        if(groupDirNID.equals(MediaStoreServiceConst.DEFAULT_DIR) || groupDirNID.equals(MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME)) {
        	groupInDefaultDir = true;
        }
        
   	
        //if all the files in recycle bin then use recycle bin playLength and size
	    if(allfilesInRecyclebin) {
	    	mediaGroupSize = recycleBinMediaGroupSize;
	    	mediaGroupPlayLength = recycleBinGroupPlayLength; 
	        MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.DELETED);
	        if(groupInDefaultDir) {
	        	MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME);
	        } else {
	        	
	        	if(!isRecycleBinDir(groupDirNID, request, xmpp, log, db)) {           
	        		MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, 
	        			MediaStoreServiceUtils.getRecycleBinNID(groupDirNID, request, xmpp, log, db, resourceFactory, serviceAgentFactory));
	        	}
	        }
	    } else {
			//Check whether all the media clips are in error state
			String mediaGroupFileFinalState = checkStateOfMediaClips(fileListNotInRecycleBin, request, xmpp, log, db) ? MediaStoreServiceConst.ERROR: mediaGroupFileState;
			MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, mediaGroupFileFinalState);
		 	
			
			if(groupInDefaultDir) {
				MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, MediaStoreServiceConst.DEFAULT_DIR); 
			} else {
			    	Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(groupDirNID, request, db, log);
				String title = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");
				if (title.equals(MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME)) {
					groupDirNID =  MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//"+MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");
				}
				MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, groupDirNID);
			}
			
	    }
	    MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.WRITE, mediaGroupWritePermissions);
        MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, String.valueOf(mediaGroupPlayLength));
        MRXMLUtils.setValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_SIZE, String.valueOf(mediaGroupSize));
        log.addInfo("Update MediaGroupPlayLengthAndSize. PlayLenght:=" + mediaGroupPlayLength + " Size:=" + mediaGroupSize+" Media group State:="+  MRXMLUtils.getValueXpath(mediaGroupFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, MediaStoreServiceConst.DELETED), MRLog.INFO, MRLog.NOTIFY);
        
        //update Bypass flag       
	    MRXMLUtils.setValueXpath(mediaGroupFileDoc, ".//"+MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, Boolean.toString(checkBypassFlagValueOfMediaClips(fileListNotInRecycleBin, request, xmpp, log, db)));
				
        String mediaStreamFileNID = MRXMLUtils.getAttributeValue(mediaGroupFileDoc.getRootElement(), MediaStoreServiceConst.NID, "");
        MediaStoreServiceUtils.updateMediaStreamFileDocInDB(mediaGroupFileDoc, mediaStreamFileNID, request.getUserJID(), xmpp, log, db, serviceAgentFactory);
      //  IPVSMonitor.getMonitor().releaseLock(mediaGroupID, lock);
       // log.addInfo("updateMediaGroupProperties: Release lock on mediaGroupID:="+mediaGroupID, MRLog.INFO, MRLog.NOTIFY);
        return mediaGroupFileDoc;
    }

    private static boolean isRecycleBinDir(String parentDirNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db)
        throws MRException, Exception {
        Document mediaStreamDirDoc = MediaStoreServiceUtils.getMediaStreamDirDoc(parentDirNID, request, db, log);

        if (mediaStreamDirDoc == null) {
            return false;
        }

        String title = MRXMLUtils.getValueXpath(mediaStreamDirDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, "");

        if (title.equals(MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME)) {
            return true;
        }

        return false;
    }

    public static Document getMediaGroupFileDoc(String mediaGroupID, String userJID, MRLog log, MRDB db)
        throws MRException, Exception {
        String[] whereXpath = {
                "//" + AMConst.MEDIA_STREAM_FILE_RESOURCE + "/" + MediaStoreServiceConst.INFO_ELEMENT_NAME + "/" + MediaStoreServiceConst.MEDIA_STORE_FILE_INFO_ELEMENT + "[(" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME + "='" + mediaGroupID + "') " + " and (" +
                MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME + "='" + MediaRoomServiceConst.MEDIAGROUP + "')]"
            };

        List<?> mediaFileList = getMediaStreamFileResourceList(whereXpath, userJID, log, db);

        if ((mediaFileList == null) || mediaFileList.isEmpty()) {
            return null;
        }

        return MRXMLUtils.elementToDocument((Element) mediaFileList.get(0));
    }
    @Deprecated
    public static void sendUpdateMediaGroupPlayLenghtAndSizeRequesToAppserver(String mediaStreamFileNID, XMPPI xmpp, MRDB db, MRLog log, String userJID, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String requestName = MRRequest.getRequestName(UpdateMediaGroupPlayLenghtAndSizeRequestHandler.class.getName());
        sendRequestToAppserver(requestName, mediaStreamFileNID, xmpp, db, log, userJID, serviceAgentFactory);
    }
    
    public static void sendRequestToAppserver(String requestName,String mediaStreamFileNID, XMPPI xmpp, MRDB db, MRLog log, String userJID, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        Document mediaRoomRequestDoc = MediaRoomServiceUtils.getMediaRoomRequest(log, ServiceClient.MEDIAROOMSERVICE, requestName);
        MRXMLUtils.setValueXpath(mediaRoomRequestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, mediaStreamFileNID);
        log.addInfo("Send request " + requestName +" to AppserverAgent for MediaStreamFileNID:="+mediaStreamFileNID, MRLog.INFO, MRLog.NOTIFY);
        MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, userJID, mediaRoomRequestDoc, log, requestName, MediaStoreAdminServerAgent.class.getName(),  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory));
    }
    
    @Deprecated
    public static void sendAddMediaGroupFileFromMediaClipRequestToAppserver(String mediaStreamFileNID, XMPPI xmpp, MRDB db, MRLog log, String userJID, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {
        String requestName = MRRequest.getRequestName(AddMediaGroupFileFromMediaClipRequestHandler.class.getName());
        sendRequestToAppserver(requestName, mediaStreamFileNID, xmpp, db, log, userJID, serviceAgentFactory);
    }

    public static void updateMediaGroupOnMediaClipUpdate(Document mediaStreamFileDoc, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws MRException, Exception {	
    	if(mediaStreamFileDoc == null) {
    		return;
    	}
        String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");        
        if (mediaGroupID.isEmpty()) {
            return;
        }

        Document mediaGroupFileDoc = getMediaGroupFileDoc(mediaGroupID, request.getUserJID(), log, db);
        
        if (mediaGroupFileDoc == null) {
            return;
        }        
        MediaStoreServiceUtils.updateMediaGroupProperties(mediaGroupFileDoc, mediaGroupID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);
    }
    
    public static boolean checkMediaStreamFileDeleted(String dirNID, MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI profileFactory)
    throws MRException, Exception {
	    if ((dirNID == null) || dirNID.isEmpty()) {
	        return false;
	    }
	    Element el = null;
        String objectType = AMConst.MEDIA_STREAM_DIR_RESOURCE;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] arrWhereXpath = { "@NID='" + dirNID + "'and //Title ='"+MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME+"'" };
        String[] sortByXpath = null;
        String selectXML = "<"+AMConst.MEDIA_STREAM_DIR_RESOURCE+" NID='{data($VAR/@NID)}' Title='{data($VAR//Title)}'  />";
        String sortOrder = null;
        //returns null if given dirNID is not "RECYCLEBIN"
        el = MRXMLDBUtils.readSingle(MRXMLDBUtils.TIME_OUT, request, log, db, listName, objectType, perentNID, childXPath, arrWhereXpath, sortByXpath, sortOrder, selectXML, null);
        if (el != null) {
    	    	return true;
        }
	    return false ;
   
    }
    
    public static void updateFilePermissions(Document newMediaStreamFileDoc,String elName,String value) throws Exception {

    	Element element = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + elName);
    	if(element == null) {    		
    		MRXMLUtils.addElement(MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.PERMISSION_EL_NAME),MRXMLUtils.newElement(elName));
    	}
    	MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + elName, value);
    }
    @Deprecated
    public static void sendInternalRequestToMediaStoreAgent(String mediaStoreStreamAgentJID, String mediaStoreFileNID, String mediaRoomRequestName, String mediaStreamFileNID, String mediaStreamFileDirNID, Document actionDataDoc, String userJID, XMPPI xmpp, MRLog log)
    throws MRException, Exception {
    	MRRequest request = MRRequest.createRequest(userJID);
    	String mediaStoreRequestName = MediaStoreServiceConst.mediaroomRequestToMediaStoreRequestMapping.get(mediaRoomRequestName);
    	String actionData = "";
    	if(actionDataDoc != null) {
    		actionData = MRXMLUtils.documentToStringOmitDeclaration(actionDataDoc);	
    	}        
    	Document mediaStoreRequestDoc = generateRequestDocument(mediaStoreRequestName, mediaStoreFileNID, mediaStreamFileNID, mediaStreamFileDirNID, actionData, request.getNID(), log);

    	Document userRequestDoc = request.getMessage().getDocument();
    	String clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
    	String fromAgentName = DestStreamAgent.class.getName();
    	ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, mediaStoreStreamAgentJID, MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT, mediaStoreRequestName, MRXMLUtils.documentToStringOmitDeclaration(mediaStoreRequestDoc),
    			fromAgentName + ": Sending " + mediaStoreRequestName + " to " + MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT, request.getNID(), request.getUserJID(), request.getServiceName(), clientData);
    }
    
    public static void updateMediaStreamFileDocWithInfo(XMPPI xmpp, Document newMediaStreamFileDoc,String mediaType,String title,String description,
    		String dirNID,Element dirGroupsEL,String mediaStoreFileNID,String streamType,String state,String writable, Element metaData,String dateCreated,String dateModified,String byPassAutoStorageRule, Element downloadEl) throws MRException,Exception {
    	  //set the groups 
    	MediaStoreServiceUtils.updateDirGroupsInFile(newMediaStreamFileDoc, dirGroupsEL);
    	
        //set UUID
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.UUID,mediaStoreFileNID);

        //if it is new file in the data base then set the parent dir NID as NONE.
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME, dirNID);

        //set title,description and stream type
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_DESCRIPTION_ELEMENT_NAME,description);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TITLE_ELEMENT_NAME, title);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_TYPE_ELEMENT_NAME, streamType);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_STATE_ELEMENT, state);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.BYPASS_AUTO_STORAGE_RULES_ELEMENT, byPassAutoStorageRule);
        
        //updateMediaType and info element
        Element infoEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//"+MediaStoreServiceConst.INFO_ELEMENT_NAME);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, mediaType);  
        MediaStoreServiceUtils.updateInfoElementBasedOnType(infoEL, mediaType);
    
        //set the  file permissions. no write only read, xfer and stream 
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.READ, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.WRITE, writable);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.XFER, MediaStoreServiceConst.TRUE);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.STREAM, MediaStoreServiceConst.TRUE);
        
        // set the file download information
        if(downloadEl != null) {
        	updateDownloadInfo(newMediaStreamFileDoc.getRootElement(), downloadEl);
        }
        
        //set date created
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_CREATED, dateCreated);
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.DATE_MODIFIED, dateModified);
        
        //update medatadata        
        Element fileMetaDataEL = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//"+MediaStoreServiceConst.META_DATA_ELEMENT_NAME);
        MRXMLUtils.replaceContent(fileMetaDataEL, metaData);
        
        MRXMLUtils.setValueXpath(newMediaStreamFileDoc, "//" + MediaStoreServiceConst.PLAY_LENGTH, 0 + "");
        
	    // Clear the profile
	    Element profileElement = MRXMLUtils.getElementXpath(newMediaStreamFileDoc, "//" + MediaRoomServiceConst.MEDIA_STREAM_PROFILE_INFO);
	    if(profileElement != null)
	        MRXMLUtils.removeContent(profileElement);


    }
    
    
    public static void sendAddMediaStreamFileRequestToAppserver(String dirNID, String mediaStoreFileNID,String readOnly,long size,int revision,String selectFormat,Document mediaStoreFileDoc,String fromAgentClass,
    		XMPPI xmpp, MRDB db, MRLog log, String userJID, ServiceAgentFactoryI serviceAgentFactory,String clientData)  throws MRException, Exception {
    	
    	String requestName = MRRequest.getRequestName(AddFileRequestHandler.class.getName());			
		Document assetRequestDoc = AssetAdminServiceUtils.getAssetAdminRequest(log,SessionServiceConst.FILE.toLowerCase() ,  requestName + ".xml");
		
		String elementName = MRRequest.getRequestRootName(requestName); 
		Element requestElement = MRXMLUtils.getElementXpath(assetRequestDoc, "//"+elementName);
		MRXMLUtils.setAttributeValue(requestElement, AMConst.DIR_NID_ATTRIBUTE, dirNID);
		MRXMLUtils.setAttributeValue(requestElement, AMConst.REVISION_ATTRIBUTE, String.valueOf(revision));
		MRXMLUtils.setAttributeValue(requestElement, AMConst.SIZE_ATTRIBUTE, String.valueOf(size));			
		MRXMLUtils.setAttributeValue(requestElement, AMConst.READ_ONLY_ATTRIBUTE,readOnly);			
		MRXMLUtils.setAttributeValue(requestElement, AMConst.STORE_FILE_NID_ATTRIBUTE,mediaStoreFileNID);
		MRXMLUtils.setAttributeValue(requestElement, SessionServiceConst.SELECTFORMAT,selectFormat);
		Element mediastorefileEl  = MRXMLUtils.getElementXpath(assetRequestDoc, "//"+AMConst.MEDIA_STORE_FILE);
		MRXMLUtils.replaceContent(mediastorefileEl, mediaStoreFileDoc.getRootElement());
		
		MRXMLUtils.setValueXpath(assetRequestDoc,"//"+MRConst.CLIENT_DATA,clientData);
		
		log.addInfo(fromAgentClass+ ": Send "+ requestName +" to appserver ",MRLog.INFO,MRLog.NOTIFY);
		MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, userJID, assetRequestDoc, log, requestName, fromAgentClass,  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory));
	   
    }
    
    
    public static void updateDownloadInfo(Element mediaStreamFileEl, Element downloadEl) throws Exception {
    	if(downloadEl == null) {
    		return;
    	}
    	Element dwnldEl = MRXMLUtils.getElementXpath(mediaStreamFileEl,	"//Download");
    	List<?> mediaElList = MRXMLUtils.getListXpath(downloadEl,"//Media");
    	for (Object obj : mediaElList) {
    		Element mediaNewEL = (Element) obj;
    		String format = mediaNewEL.attributeValue("format");
    		Element mediaOldEl = MRXMLUtils.getElementXpath(dwnldEl,"//Media[@format='" + format + "']");
    		if (mediaOldEl != null)
    			mediaOldEl.detach();
    		MRXMLUtils.addElement(dwnldEl, (Element) mediaNewEL.clone());
    	}
    	//Remove the empty media node if any (<Media state="NotReady" format="" stale="">)
    	Element mediaEmptyEl = MRXMLUtils.getElementXpath(mediaStreamFileEl,"//Media[@format='' and @state='NotReady']");
    	List<?> mediaList = MRXMLUtils.getListXpath(mediaStreamFileEl,"//Media");
    	if(mediaEmptyEl!=null && mediaList.size()>1){
    		mediaEmptyEl.detach();
    	}
    	
    }
    
    public static void invalidateFileThumbnailCache(String mediaID, MRLog log) {
		
		if (mediaID == null || mediaID.isEmpty()) {
			log.addInfo("No thumbnail dir to invalidate.");
			return;
		}
		String[] systemCmd = new String[3];
		systemCmd[0] = "rm";
		systemCmd[1] = "-rf";
		systemCmd[2] = MediaRoomServiceConst.RECORDING_THUMBNAIL_CACHE + "/" + mediaID;

		log.addInfo("Invalidate File thumbnail cache command: "	+ Arrays.toString(systemCmd));
		int cmdResult;
		try {
			cmdResult = MediaRoomServiceUtils.executeSystemCommand(systemCmd);
			if (cmdResult != 0) {
				log.addInfo("Failed to invalidate thumbnail cache for mediaID: " + mediaID);
				return;
			}
			log.addInfo("Successfully invalidated thumbnail cache for mediaID: " + mediaID);
		} catch (Exception e) {
			log.addInfo("Failed to invalidate thumbnail cache for mediaID: " + mediaID);
			e.printStackTrace();
		}
	}
    
    /*public synchronized static Document setFileBypassAutoStorageRules(Document mediaStreamFileDoc, String mediaStreamFileNID, String action, Document resDoc, String dbObjectName,
            String pemXPath, Boolean moveToRecycleBin, MRRequest request, MRDBHelper dbHelper, MRDB schema, MRLog log, XMPPI xmpp, MRResourceFactoryI resourceFactory,
            ServiceAgentFactoryI serviceAgentFactory) throws MRException, Exception {
          
    	String mediaType = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_TYPE_ELEMENT_NAME, "");
    	Document responseDoc = null;
				if (mediaType.equals(MediaRoomServiceConst.MEDIAGROUP)) {
	                String mediaGroupID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.MEDIA_GROUP_ID_EL_NAME, "");
	                List<?> listOfFiles = MediaStoreServiceUtils.getListOfFilesInMediaGroup(mediaGroupID, request.getUserJID(), log, schema);

	                if ((listOfFiles != null) && !listOfFiles.isEmpty()) {
	                    for (Object object : listOfFiles) {
	                        Element fileEL = (Element) object;
	                        MediaStoreServiceUtils.sendRequestToMediaStoreAgent(MediaStoreServiceConst.UPDATE_FILE_AUTOSTOARGE_RULES_REQUEST, MRXMLUtils.elementToDocument(fileEL),
	                                request, xmpp, log, schema, resourceFactory, serviceAgentFactory);                    
	                    }
	                }
	    		}
	            else {
	              	MediaStoreServiceUtils.sendRequestToMediaStoreAgent(request, xmpp, log, db, resourceFactory, serviceAgentFactory, MediaStoreServiceConst.UPDATE_FILE_AUTOSTOARGE_RULES_REQUEST, 
	        				actionData, mediaStreamFileDoc, this.getClass().getName(), true);
	            }
				return null;
				//String mediaStoreStreamAgentJID, String mediaStoreFileNID, String mediaRoomRequestName, String mediaStreamFileNID, String mediaStreamFileDirNID, Document actionDataDoc, MRRequest request, XMPPI xmpp, MRLog log)
    }
    */
}
