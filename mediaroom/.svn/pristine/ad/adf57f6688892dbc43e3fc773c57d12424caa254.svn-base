package com.ipvs.mediastoreservice.impl;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.assetadminservice.handler.AMExceptionCodes;
import com.ipvs.assetadminservice.handler.GetMediaStreamFileMaxRevisionRequestHandler;
import com.ipvs.assetadminservice.handler.UpgradeMediaStreamFileResourceRequestHandler;
import com.ipvs.assetadminservice.impl.AssetAdminServiceUtils;
import com.ipvs.assetadminservice.impl.FileSM;
import com.ipvs.assetadminservice.impl.FileSMQueue;
import com.ipvs.assetadminservice.impl.FileSMUtils;
import com.ipvs.client.MRClientAgent;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRConfig;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.device.impl.DeviceConst;
import com.ipvs.device.mediastore.Device2Dot4MediaStoreServerImpl;
import com.ipvs.device.mediastore.Device3Dot2MediaStoreServerImpl;
import com.ipvs.device.mediastore.DeviceMediaStoreServerImpl;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.SDPUtils;
import com.ipvs.mediaroomservice.impl.SourceStreamAgent;
import com.ipvs.mediaroomservice.streamingserver.StreamingServerI;
import com.ipvs.mediastoreservice.handler.DeleteMediaStoreFileRequestHandler;
import com.ipvs.mediastoreservice.handler.GetMediaStreamFileMaxRevisionResponseHandler;
import com.ipvs.mediastoreservice.handler.SynchMediaStoreDirRequestHandler;
import com.ipvs.systemservice.handler.SystemServiceUtils;
import com.ipvs.utils.FileUtils;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.DBUpdateEvent;


public abstract class MediaStoreAdminServerAgent implements ServiceAgentI, MRResponseHandlerI {

    protected static final HashMap<String, MediaStoreAdminServerAgent> storeAgentList = new HashMap<String, MediaStoreAdminServerAgent>();
    protected static final String TRASH_DIR_NAME = "trash";
    protected HashMap<String, MediaStoreServerI> storeServerList = new HashMap<String, MediaStoreServerI>();
    protected MRLog log;
    protected HashMap<String, MediaStoreDirectoryInfo> mediaStoreList = null;
    protected MRDB db;
    protected String userJID;
    protected XMPPF xf;
    protected MRResourceFactoryI resourceFactory;
    protected long deleterPortRunningNumber = 1000;
    protected StreamingServerI streamingServer = null;
    protected String mediaStorePortResourceNID = null;
    protected String deviceNID = null;
  
    private static final String DISK_QUOTA_EXCEEDED_SSEVENT_XML = "/com/ipvs/mediaroomservice/streamingserver/DiskQuotaExceededSSEvent.xml";
    
    public abstract void checkAvailableDiskSpace(String parentDirNID) throws MRException, Exception;
    public abstract void checkAvailableDiskSpaceForExportImportFile(long blobSize, String type, String title,  String destFileParentDirNID) throws MRException, Exception;
    public abstract MediaStoreDirectoryInfo getMediaStoreInfoBasedOnDirNID(String mediaStreamDirNID) throws MRException, Exception;
    protected abstract void handleNASAvailable(String storeName) throws Exception;
    
    public String getMediaStorePortResourceNID() {
        return mediaStorePortResourceNID;
    }

    public String getDeviceNID() {
        return deviceNID;
    }

    public void setMediaStorePortResourceNID(String mediaStorePortResourceNID) {
        this.mediaStorePortResourceNID = mediaStorePortResourceNID;
    }

    public long getDeleterPortRunningNumber() {
        return deleterPortRunningNumber++;
    }

    public static MediaStoreAdminServerAgent getMediaStoreAgent(String userJID) {
        return storeAgentList.get(userJID);
    }

    public MediaStoreDirectoryInfo createMediaStoreDirectory(String rootPath, String storeName, String storeDeviceElementID, String mediaStorePortResourceNID, boolean readOnly, long availableDiskSpace, long usedDiskSpace, boolean isInternal) {
        MediaStoreDirectoryInfo storeInfo = new MediaStoreDirectoryInfo(storeName, rootPath, storeDeviceElementID, mediaStorePortResourceNID, readOnly, availableDiskSpace, usedDiskSpace, isInternal);
        //storeInfo.setMediaStoreState(MediaStoreServiceConst.BUSY);
        mediaStoreList.put(storeName, storeInfo);

        return storeInfo;
    }

    public MediaStoreDirectoryInfo getMediaStoreDirectoryInfo(String storeName) {
        return mediaStoreList.get(storeName);
    }

    // get the media store Info for given media Stream dir NID  -- for device simulation
    public MediaStoreDirectoryInfo getMediaStoreInfoBasedOnDeleterPort(long deleterPort)
        throws MRException, Exception {
        Set<String> storeSet = mediaStoreList.keySet();
        MediaStoreDirectoryInfo storeDirInfo = null;

        for (String storeName : storeSet) {
            storeDirInfo = mediaStoreList.get(storeName);

            if (storeDirInfo.getDeleterPort() == deleterPort) {
                return storeDirInfo;
            }
        }

        return null;
    }
    
 // get the media store Info for given media Stream dir NID  -- for device simulation
    public String getMediaStoreDirNIDBasedOnRootPath(String rootPath)
        throws MRException, Exception {
        Set<String> storeSet = mediaStoreList.keySet();
        MediaStoreDirectoryInfo storeDirInfo = null;

        for (String storeName : storeSet) {
            storeDirInfo = mediaStoreList.get(storeName);

            if (storeDirInfo.getMediaStoreRootPath().equals(rootPath)) {
                return storeDirInfo.getMediaStoreDirNID();
            }
        }

        return null;
    }
    
    // get the media store Info based on root path  -- for device simulation
    public MediaStoreDirectoryInfo getMediaStoreInfoBasedOnRootPath(String rootPath)
        throws MRException, Exception {
        Set<String> storeSet = mediaStoreList.keySet();
        
        for (String storeName : storeSet) {
        	MediaStoreDirectoryInfo mediaStoreDirInfo = mediaStoreList.get(storeName);
            if (mediaStoreDirInfo.getMediaStoreRootPath().equals(rootPath)) {
            	return mediaStoreDirInfo;
            }
        }

        return null;
    }

    public List<String> getDirNIDList() {
        Collection<MediaStoreDirectoryInfo> dirInfoList = mediaStoreList.values();

        if (dirInfoList == null) {
            return null;
        }

        List<String> dirNIDList = new ArrayList<String>();

        for (MediaStoreDirectoryInfo dirInfo : dirInfoList) {
            dirNIDList.add(dirInfo.getMediaStoreDirNID());
        }

        return dirNIDList;
    }

    // get the media store state for given media stream file NID
    protected String getMediaStoreDirectoryState(String mediaStreamFileNID)
        throws MRException, Exception {
        MediaStoreDirectoryInfo storeInfo = getMediaStoreInfoBasedOnStreamFileNID(mediaStreamFileNID);

        if (storeInfo == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_MEDIA_STORE_FOUND, "No Media Store Found");
        }

        return storeInfo.getMediaStoreState();
    }

    // get the media store root Path for given media stream file NID    
    /*
    public String getMediaStoreRootPath(String mediaStreamFileNID) throws MRException, Exception {
        MediaStoreDirectoryInfo storeInfo = getMediaStoreInfoBasedOnStreamFileNID(mediaStreamFileNID);

        if (storeInfo == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_MEDIA_STORE_FOUND, "No Media Store Found");
        }

        return storeInfo.getMediaStoreRootPath();
    }
    */

    // get the media store root path for given media stream file document
    public String getMediaStoreRootPath(Document mediaStreamFileDoc) throws MRException, Exception {
        MediaStoreDirectoryInfo storeInfo = getMediaStoreInfoBasedOnStreamFileDoc(mediaStreamFileDoc);

        if (storeInfo == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_MEDIA_STORE_FOUND, "No Media Store Found");
        }

        return storeInfo.getMediaStoreRootPath();
    }

    // get the media store root path for given media store port resource nid   
    public String getMediaStoreRootPathBasedOnDirNID(String mediaStreamDirNID)
        throws MRException, Exception {
        MediaStoreDirectoryInfo storeInfo = getMediaStoreInfoBasedOnDirNID(mediaStreamDirNID);

        if (storeInfo == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_MEDIA_STORE_FOUND, "No Media Store Found");
        }

        return storeInfo.getMediaStoreRootPath();
    }

    // get the media store Info for given media Stream file resource NID
    public MediaStoreDirectoryInfo getMediaStoreInfoBasedOnStreamFileNID(String mediaStreamFileNID)
        throws MRException, Exception {
        Document mediaStreamFileDoc = MediaStoreServiceUtils.getMediaStreamFileDoc(mediaStreamFileNID, MRRequest.createRequest(userJID), log, db);

        return getMediaStoreInfoBasedOnStreamFileDoc(mediaStreamFileDoc);
    }

    // get the media store Info for given media Stream file Document
    protected MediaStoreDirectoryInfo getMediaStoreInfoBasedOnStreamFileDoc(Document mediaStreamFileDoc)
        throws MRException, Exception {
        String mediaStreamDirNID = MRXMLUtils.getValueXpath(mediaStreamFileDoc, "//" + MediaStoreServiceConst.PARENT_DIR_NID_EL_NAME, "");

        return getMediaStoreInfoBasedOnDirNID(mediaStreamDirNID);
    }


    public boolean handleEvent(MRClient client, Document messageDoc, Element event)
        throws Exception {
        MediaStoreDirectoryInfo storeDirInfo = isMediaStreamDirEvent(messageDoc, client.getDb(), client.getLog());

        if (storeDirInfo == null) {
            return false;
        }

        log.addInfo("Handle Dir Update Event. Title:=" + storeDirInfo.getStoreName(), MRLog.INFO, MRLog.NOTIFY);
        
        if(MediaStoreServiceConst.ERROR.equalsIgnoreCase(storeDirInfo.getMediaStoreState())) {
        	return false;
        }
        callSynchDirRequest(storeDirInfo.getStoreName(), MediaStoreServiceConst.STATE_UPDATE);

        String prevStoreDirState = storeDirInfo.getMediaStoreState();

        if (storeDirInfo.getMediaStoreState().equals(MediaStoreServiceConst.START_SYNCH)) {
            sendGetMaxRevisionRequestToAppserverAgent(storeDirInfo, log, xf.xmpp(userJID));
            storeDirInfo.setMediaStoreState(MediaStoreServiceConst.BUSY);
        } else if (MediaStoreServiceConst.BUSY.equals(prevStoreDirState) && storeDirInfo.getMediaStoreState().equals(MediaStoreServiceConst.READY)) {
            //send the get media room invites only when the prev state is busy and the current state is ready. 
            //It means some one finishes the synch send get media room invites to update the ports states.
            sendAgentStateEvent();
        }

        return false;
    }

    public MediaStoreDirectoryInfo isMediaStreamDirEvent(Document messageDoc, MRDB db, MRLog log)
        throws MRException, Exception {
        DBUpdateEvent dbUpdateEvent = new DBUpdateEvent(messageDoc);

        if (dbUpdateEvent.getUpdateEventElement() == null) {
            return null;
        }

        String parentID = dbUpdateEvent.getSubscribedNID();
        MRConfig config = MRConfig.getInstance(AMConst.MEDIA_STREAM_DIR_RESOURCE, log);
        String mediaStreamDirParentID = db.getServiceDomainRootNID(config.getParentNIDKey(log));

        if (!parentID.equals(mediaStreamDirParentID)) {
            return null;
        }

        if (dbUpdateEvent.isAddorUpdate()) {
            String dirNID = dbUpdateEvent.getUpdateNID();
            MediaStoreDirectoryInfo storeDirInfo = getMediaStoreInfoBasedOnDirNID(dirNID);

            if (storeDirInfo == null) {
                return null;
            }

            if (storeDirInfo.getMediaStoreDirNID().equals(dirNID)) {
                return storeDirInfo;
            }
        }

        return null;
    }

    public String handleMessage(MRClient client, String fromJID, String message)
        throws Exception {
        Document messageDoc = null;
        messageDoc = MRXMLUtils.stringToDocument(message);

        String messagetype =MRXMLUtils.getAttributeValueXpath(messageDoc, "//AgentMessage","type", "");

        if (ServiceClient.REQUEST.equals(messagetype)) {
            handleAgentRequest(client, messageDoc, fromJID);
        } else if (ServiceClient.RESPONSE.equals(messagetype)) {
            handleAgentResponse(client, messageDoc, fromJID);
        } else if (ServiceClient.EVENT.equals(messagetype)) {
            handleAgentEvent(client, messageDoc, fromJID);
        }

        return message;
    }

    private void handleAgentEvent(MRClient client, Document messageDoc, String fromJID) {
        try {
            String action = MRXMLUtils.getValueXpath(messageDoc, "//" + DeviceConst.ACTION, "");

            if (action.equals(MediaRoomServiceConst.SSEVENT)) {
                handleSSEvent(client, messageDoc, fromJID);
            } else if (action.equals(MediaRoomServiceConst.SYSTEMEVENT)) {
                handleSystemEvent(client, messageDoc, fromJID);
            }else  if (action.equals(FileSM.MEDIA_FILE_CONVERTER_EVENT)) {
                handleMediaFileConverterEvent(client, messageDoc, fromJID);          
            } 
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    private void handleSystemEvent(MRClient client, Document systemDoc, String fromJID)
        throws MRException, Exception {
        String mediaSource = MRXMLUtils.getValueXpath(systemDoc, "//EventSource", "");
        String eventType = MRXMLUtils.getValueXpath(systemDoc, "//EventType", "");

        if (mediaSource.isEmpty() || eventType.isEmpty()) {
            return;
        }

        if (mediaSource.equals("NAS")) {
            String storeName = MRXMLUtils.getValueXpath(systemDoc, "//ID", "");

            String presence = null;

            if (eventType.equals(MediaStoreServiceConst.MOUNT) || eventType.equals(MediaStoreServiceConst.ONLINE)) {
            	this.handleNASAvailable(storeName);
                presence = AMConst.AVAILABLE;
            } else if (eventType.equals(MediaStoreServiceConst.UNMOUNT) || eventType.equals(MediaStoreServiceConst.OFFLINE) || eventType.equals(MediaStoreServiceConst.ERROR)) {
                presence = AMConst.UNAVAILABLE;
            } else if (eventType.equals(MediaStoreServiceConst.DELETE)) {
                presence = MediaStoreServiceConst.DELETE;
            } else {
                return;
            }

            MediaStoreDirectoryInfo storeDirInfo = getMediaStoreDirectoryInfo(storeName);

            if ((storeDirInfo == null) && eventType.equals(MediaStoreServiceConst.DELETE)) {
                callSynchDirRequest(storeName, presence);
            } else if (storeDirInfo != null) {
                callSynchDirRequest(storeDirInfo, presence);
            } else {
                log.addInfo("No Store found. for StoreName:" + storeName, MRLog.INFO, MRLog.NOTIFY);

                return;
            }

            if (presence.equals(AMConst.AVAILABLE)) {
                if (storeDirInfo.getMediaStoreState().equals(MediaStoreServiceConst.START_SYNCH)) {
                    // send the get media stream file max revision request to app server.
                    sendGetMaxRevisionRequestToAppserverAgent(storeDirInfo, log, xf.xmpp(userJID));
                    storeDirInfo.setMediaStoreState(MediaStoreServiceConst.BUSY);
                } else {
                    sendAgentStateEvent();
                }
            } else if (presence.equals(AMConst.UNAVAILABLE) || (eventType.equals(MediaStoreServiceConst.DELETE))) {
                mediaStoreList.remove(storeName);
            }
        }
    }

    private void handleSSEvent(MRClient client, Document ssEventDoc, String fromJID)
        throws MRException, Exception {  	
        String event = MRXMLUtils.getValueXpath(ssEventDoc, "//EventType", "");
        String ssInstancePort = MRXMLUtils.getValueXpath(ssEventDoc, "//Type", "");

        if (event.isEmpty() || ssInstancePort.isEmpty()) {
            return;
        }

        if (event.equals(MediaRoomServiceConst.SSEVENT_PROGRESS)) {
            if (ssInstancePort.equals(MediaRoomServiceConst.DELETER_PORT)) {
                handleDeleterPortProgressNotification(ssEventDoc);
            } else if (ssInstancePort.equals(MediaRoomServiceConst.DMS_RECORDER_PORT)) {
            	handleDMSRecorderPortProgressNotification(ssEventDoc);
            }
        }
    }

    private void handleDeleterPortProgressNotification(Document ssEventDoc)
        throws MRException, Exception {
        long ssInstanceID = Utils.getLongValue(MRXMLUtils.getValueXpath(ssEventDoc, "//Id", ""), 0);

        if (ssInstanceID == 0) {
            return;
        }

        MediaStoreDirectoryInfo storeDirInfo = getMediaStoreInfoBasedOnDeleterPort(ssInstanceID);
        
        long sizeUsed = MediaStoreServiceUtils.updateMediaDirSizeUsed(storeDirInfo.isInternal(), storeDirInfo.getStoreDeviceElementID(), storeDirInfo.getMediaStoreDirNID(), MRRequest.createRequest(this.userJID), xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
        
        if (sizeUsed != 0) {
            storeDirInfo.setSizeUsed(sizeUsed);
  
            long availableDiskSpace = storeDirInfo.getSize() - sizeUsed;       	
        
            if (availableDiskSpace  > Utils.getLongValue(MediaStoreServiceConst.DISK_QUOTA_EXCEED_LIMIT, 0)) {
        		storeDirInfo.setDiskQuotaExceeded(false);
            }  
        }
    }
    
    private void handleDMSRecorderPortProgressNotification (Document ssEventDoc) 
    	throws MRException, Exception {
		log.addInfo("MediaStoreAdminServerAgent: Received request to handle DMSRecorderPort progress notification", MRLog.INFO, MRLog.NOTIFY);
    	String rootDirPath = MRXMLUtils.getValueXpath(ssEventDoc, "//" + MediaStoreServiceConst.MEDIA_DIR_PATH, "");
    	MediaStoreDirectoryInfo storeDirInfo = getMediaStoreInfoBasedOnRootPath(rootDirPath);
    	
    	if (storeDirInfo == null || storeDirInfo.isDiskQuotaExceeded()) { 
    		return;
    	}
    	
        String ssInstanceId = MRXMLUtils.getValueXpath(ssEventDoc, "//Id", "");

    	long bytesWrittenDelta = Utils.getLongValue(MRXMLUtils.getValueXpath(ssEventDoc, "//" + MediaStoreServiceConst.BYTES_WRITTEN_DELTA, ""), 0);
    	long bytesDeletedDelta = Utils.getLongValue(MRXMLUtils.getValueXpath(ssEventDoc, "//" + MediaStoreServiceConst.BYTES_DELETED_DELTA, ""), 0);
    	
    
    	long usedDiskSpace = storeDirInfo.getSizeUsed() + (bytesWrittenDelta - bytesDeletedDelta);    	
    	long availableDiskSpace = storeDirInfo.getSize() - usedDiskSpace;       	
    	storeDirInfo.setSizeUsed(usedDiskSpace);
    	
    	log.addInfo("MediaStoreAdminServerAgent: SSInstanceId:="+ssInstanceId+ " bytesWrittenDelta:="+bytesWrittenDelta
    			+ " bytesDeletedDelta:= "+bytesDeletedDelta+"  dir-->usedDiskSpace = " + usedDiskSpace + " dir-->availableDiskSpace = " + availableDiskSpace, MRLog.INFO, MRLog.NOTIFY);  
    	if (availableDiskSpace <= Utils.getLongValue(MediaStoreServiceConst.DISK_QUOTA_EXCEED_LIMIT, 0)) {
    		storeDirInfo.setDiskQuotaExceeded(true); 
        	simulateDiskQuotaExceededEvent(ssEventDoc, rootDirPath);
        }    	
    }       	   
    
    private void simulateDiskQuotaExceededEvent(Document ssEventDoc, String mediaDirPath) throws MRException, Exception {
    	log.addInfo("MediaStoreAdminServerAgent: Simulate disk quota exceeded SSEvent and send request to SourceStreamAgent", MRLog.INFO, MRLog.NOTIFY);    	   
    	Document simulatedSSEventDoc = MRXMLUtils.loadXMLResource(DISK_QUOTA_EXCEEDED_SSEVENT_XML, log);
		MRXMLUtils.setValueXpath(simulatedSSEventDoc, "//EventType", MediaRoomServiceConst.DISK_QUOTA_EXCEEDED_EVENT);		
		MRXMLUtils.setValueXpath(simulatedSSEventDoc, "//MediaDirectory", mediaDirPath);		
	    String info = "MediaStoreAdminServerAgent :=" + userJID + " > Disk quota exceeded SSEvent to " + ServiceAgentUtils.getAgentShortName(SourceStreamAgent.class.getName());
        ServiceAgentUtils.sendAgentEvent(xf.xmpp(userJID), log, null, "", userJID, MRConst.MEDIA_SERVICE, MRLog.OK, userJID, xf.xmpp(userJID).getCurrentTimeMillis(), MediaRoomServiceConst.SSEVENT, simulatedSSEventDoc.getRootElement(), "Disk quota exceeded", info);
    }

    private void handleAgentResponse(MRClient client, Document messageDoc, String fromJID)
        throws MRException, Exception {
        String requestName = MRXMLUtils.getAttributeValueXpath(messageDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
        Document responseDoc = null;
        String serviceName = null;

        try {
            if (requestName.isEmpty()) {
                return;
            }

            serviceName = MRXMLUtils.getAttributeValueXpath(messageDoc, MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");

            if (serviceName.isEmpty()) {
                return;
            }

            responseDoc = MRXMLUtils.getDocumentXpath(messageDoc, "//" + ServiceClient.RESPONSE);
            log.addInfo(MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT + " : Received Response. RequestName:=" + requestName + " From UserJID:=" + fromJID, MRLog.INFO, MRLog.NOTIFY);

            String responseName = MRRequest.getResponseName(requestName);
            String requestNID = MRXMLUtils.getAttributeValueXpath(messageDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NID, "");
            Document requestDoc = MRXMLUtils.getDocument();
            MRRequest request = MRRequest.createRequest(this.userJID, MRLog.INFO, requestNID, serviceName, requestName, new MRMessage(requestDoc), "",  AppServerAgentUtils.getAppServerAgentJID(this.userJID,xf.sf(userJID)));
            request.setResult(responseDoc);

            try {
                AgentResponseHandlerI handler = (AgentResponseHandlerI) Class.forName(MediaStoreServiceUtils.getHandlerClassName(responseName)).newInstance();
                handler.handleAgentResponse(this, request, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));

                if (requestName.equals(MRRequest.getRequestName(GetMediaStreamFileMaxRevisionRequestHandler.class.getName()))) {
                    sendAgentStateEvent();
                }
            } catch (Exception exp) {
                //ignore exception            	
            }
        } catch (MRException exp) {
            exp.printStackTrace();
        } catch (Exception exp) {
            exp.printStackTrace();
        }
    }
    
    
    
    public void updatePVRDirNID(String mediaStreamDirNID, String pvrDirNID)
    throws MRException, Exception {
    	MediaStoreDirectoryInfo storeInfo = this.getMediaStoreInfoBasedOnDirNID(mediaStreamDirNID);
    	if(storeInfo!=null) {
    		storeInfo.setPvrDirNID(pvrDirNID);
    	}
    }

    public void updateRecycleBinNID(String mediaStreamDirNID, String recycleBinNID)
    throws MRException, Exception {
    	MediaStoreDirectoryInfo storeInfo = this.getMediaStoreInfoBasedOnDirNID(mediaStreamDirNID);
    	if(storeInfo!=null) {
    		storeInfo.setRecycleBinNID(recycleBinNID);
    	}
    }

    
    void handleDetachAttachRequest(MRClient client, Document messageDoc, String fromJID,
    		String requestName) throws Exception {
	    String dirNID = MRXMLUtils.getValueXpath(messageDoc, "//MediaStreamDirNID", "");
	    String dirState = MRXMLUtils.getValueXpath(messageDoc, "//dirState", "");
    	this.log.addInfo("MediaStoreAdminServerAgent:handleDetachAttachRequest dirState=" + dirState +
    			" requestName=" + requestName, MRLog.OK, MRLog.NOTIFY);
	    String exportDirPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID) + "/" + dirNID + ".tar";
	    if(!"".equals(dirState)) {
	    	// Just a notification
	    	if(MediaStoreServiceConst.READY.equals(dirState)) {
	    		File file = new File(exportDirPath);
	    		if(file.exists()) {
	    	    	this.log.addInfo("MediaStoreAdminServerAgent:handleDetachAttachRequest deleting file=" 
	    	    			+ exportDirPath, MRLog.OK, MRLog.NOTIFY);
	    			file.delete();
	    		}
	    		else {
	    	    	this.log.addInfo("MediaStoreAdminServerAgent:handleDetachAttachRequest deleting file, not found=" 
	    	    			+ exportDirPath, MRLog.OK, MRLog.NOTIFY);	    			
	    		}
	    		
	    		String whereXpath = "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE + "/Info//Properties[  ParentMediaDirNID  = '"  + dirNID + "']";
	    		List<?> objectList = MediaStoreServiceUtils.getMediaStreamDirList( whereXpath,  MRRequest.createRequest(userJID),  xf.xmpp(userJID),  log,  db,  resourceFactory,  xf.sf(userJID));
	    		String pvrDirNID = null;
	    		String recycleBinNID = null;
	    		if(objectList == null) {
	    			return;
	    		}
	    		
	    		for(int i=0;i<objectList.size();i++) {
	    			if(MediaStoreServiceConst.PVR_DIR_NAME.equalsIgnoreCase(MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument((Element)objectList.get(0)), "//Title",""))) {
		    			pvrDirNID = MRXMLUtils.getAttributeValue((Element)objectList.get(0), "NID", "");
		    			updatePVRDirNID(dirNID,pvrDirNID);
	    			} else if (MediaStoreServiceConst.RECYCLE_BIN_DIR_NAME.equalsIgnoreCase(MRXMLUtils.getValueXpath(MRXMLUtils.elementToDocument((Element)objectList.get(0)), "//Title",""))) {
		    			recycleBinNID = MRXMLUtils.getAttributeValue((Element)objectList.get(0), "NID", "");
		    			updateRecycleBinNID(dirNID,recycleBinNID);
	    			}	
		    		
	    		}
	    		
	    	}
	    	return;
	    }
	    String response = 
	    	"<data>" + 
	    		"<MediaStreamDirNID>" + dirNID + "</MediaStreamDirNID>" + 
	    		"<dirAgentJID>" + this.userJID + "</dirAgentJID>" + 
	    		"<exportDirPath>" + exportDirPath + "</exportDirPath>" + 
	    	"</data>";
	    String fromAgentClassName = this.getClass().getName();
	    int responseState = MRRequest.STATE_DONE;
	    if(MediaRoomServiceUtils.ATTACH_DIR_REQUEST.equals(requestName)) { // Check if file exists
	    	try {
	        	File file = new File(exportDirPath);
	        	if(!file.exists())
	        		throw new MRException(AMExceptionCodes.DIR_ATTACH_FILE_NOT_FOUND, "File not found :" + exportDirPath);
	    	}
	    	catch(Exception e) {
	    		// response = e.toString();
	    		responseState = MRRequest.STATE_EXCEPTION;
	    	}
	    }
		ServiceAgentUtils.sendAgentResponse(this.xf.xmpp(userJID), this.log, client, messageDoc, fromJID, response, 
				this.userJID, fromAgentClassName, responseState);
	}
    
    private void handleAgentRequest(MRClient client, Document messageDoc, String fromJID)
        throws MRException, Exception {
        int status = MRLog.OK;
        String logData = null;
        String requestName = null;
        Document responseDoc = null;
        String requestID = null;
        String mediaStoreFileNID = "";
        String mediaStreamFileNID = "";
        String mediaStreamDirNID = "";
        String mediaStoreState = null;

        try {
            requestName = MRXMLUtils.getAttributeValueXpath(messageDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");
            requestID = MRXMLUtils.getAttributeValueXpath(messageDoc, MRConst.HEADER_XPATH,MRConst.REQUEST_NID, "");
            log.addInfo(MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT + " : Received Request. RequestName:=" + requestName + " From UserJID:=" + fromJID, MRLog.INFO, MRLog.NOTIFY);
            // System.out.println("MediaStoreAdminServerAgent:handleAgentRequest message=" + MRXMLUtils.documentToString(messageDoc));
            if (requestName.isEmpty()) {
                throw new MRException(MediaStoreServiceExceptionCodes.INVALID_MEDIA_STORE_REQUEST, "Invalid Media Store Request");
            }

            if (MediaRoomServiceUtils.DETACH_DIR_REQUEST.equals(requestName) ||
            		(MediaRoomServiceUtils.ATTACH_DIR_REQUEST.equals(requestName))) {
            	this.handleDetachAttachRequest(client, messageDoc, fromJID, requestName);
            	return;
            }
            ///handle import export request////////////////
            Element queueEL = MRXMLUtils.getElementXpath(messageDoc, "//" + AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
            if (queueEL != null) {
                try {
                    handleExportImportRequest(queueEL, client, messageDoc, fromJID, requestName);
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
                return;
            }
            ///handle import export request////////////////
            
             
            mediaStoreFileNID = MRXMLUtils.getValueXpath(messageDoc, "//" + MediaStoreServiceConst.MEDIA_STORE_FILE_NID, "");
            mediaStreamFileNID = MRXMLUtils.getValueXpath(messageDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, "");
            mediaStreamDirNID = MRXMLUtils.getValueXpath(messageDoc, "//" + AMConst.MEDIA_STREAM_DIR_RESOURCE_NID, "");

            if (mediaStreamDirNID.isEmpty()) {
                    throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_DIR_NOT_FOUND, "Media Store Dir NID not found");
            }
            MediaStoreDirectoryInfo storeDirInfo = getMediaStoreInfoBasedOnDirNID(mediaStreamDirNID);

            if (storeDirInfo == null) {
                throw new MRException(MediaStoreServiceExceptionCodes.NO_MEDIA_STORE_FOUND, "No Media Store Found");
            }

            mediaStoreState = storeDirInfo.getMediaStoreState();
            
            if (mediaStoreState.equals(MediaStoreServiceConst.BUSY)) {
                throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_IS_NOT_READY, "Media Store is Not Ready");
            }

            if (mediaStoreState.equals(MediaStoreServiceConst.ERROR)) {
                throw new MRException(MediaStoreServiceExceptionCodes.MEDIA_STORE_STATE_IS_ERROR, "Media Store State is Error. Can Not Proceed");
            }

            responseDoc = handleRequest(requestName, requestID, messageDoc, mediaStoreFileNID, mediaStreamFileNID);
        } catch (MRException exp) {
            exp.printStackTrace();
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(exp.getCode()), "Description", exp.getData());
            status = MediaStoreServiceConst.FAILURE_STATE;

            if (!requestName.equals(MediaStoreServiceConst.GET_MEDIA_STORE_FILE_TRACK_REQUEST)) {
                if ((mediaStreamFileNID != null) && !mediaStreamFileNID.isEmpty()) {
                    MediaStoreServiceUtils.updateMediaStreamFileState(mediaStreamFileNID, MediaStoreServiceConst.ERROR, mediaStoreFileNID, mediaStreamFileNID, xf.xmpp(userJID), log, db);
                }
            }

            responseDoc = MediaStoreServiceUtils.generateResponseDocument(MRRequest.getResponseName(requestName), mediaStoreFileNID, mediaStreamFileNID, "", MediaStoreServiceConst.FAILURE_STATE, logData, requestID, log);
        } catch (Exception exp) {
            exp.printStackTrace();
            logData = MRXMLUtils.generateErrorXMLTags("Code", String.valueOf(MediaStoreServiceExceptionCodes.MEDIA_STORE_ADMIN_RESPONSE_FAILED), "Description", exp.toString());
            status = MediaStoreServiceConst.FAILURE_STATE;

            if (!requestName.equals(MediaStoreServiceConst.GET_MEDIA_STORE_FILE_TRACK_REQUEST)) {
                if ((mediaStreamFileNID != null) && !mediaStreamFileNID.isEmpty()) {
                    MediaStoreServiceUtils.updateMediaStreamFileState(mediaStreamFileNID, MediaStoreServiceConst.ERROR, mediaStoreFileNID, mediaStreamFileNID, xf.xmpp(userJID), log, db);
                }
            }

            responseDoc = MediaStoreServiceUtils.generateResponseDocument(MRRequest.getResponseName(requestName), mediaStoreFileNID, mediaStreamFileNID, "", MediaStoreServiceConst.FAILURE_STATE, logData, requestID, log);
        }

        if(responseDoc != null) {
	        log.addInfo(MediaStoreServiceConst.MEDIA_STORE_ADMIN_SERVER_AGENT + " : Send Response to Request:=" + requestName + " To UserJID:=" + fromJID, MRLog.INFO, MRLog.NOTIFY);
	        MediaStoreServiceUtils.sendMediaStoreResponseMessage( messageDoc, responseDoc, requestName, fromJID, status, logData, xf.xmpp(userJID), log, userJID);
	    }
    }

    public Document handleRequest(String requestName, String requestID, Document requestDoc, String mediaStoreFileNID, String mediaStreamFileNID)
        throws MRException, Exception {
        MRRequestHandlerI handler = null;
        Document responseDoc = null;

        handler = (MRRequestHandlerI) Class.forName(MediaStoreServiceUtils.getHandlerClassName(requestName)).newInstance();

        MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO, requestID, ServiceClient.MEDIASTORESERVICE, requestName, new MRMessage(requestDoc), "");

        responseDoc = handler.getResponse(request, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
        
        return responseDoc;
    }

    protected void createMediaStoreSever(String serverVersion, XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
    		String userJID)
        throws MRException, Exception {
        MediaStoreServerI mediaStoreServer = null;
        if (MediaVersionManager.equals(serverVersion,log)) {
            mediaStoreServer = new DeviceMediaStoreServerImpl(xf, log, db, resourceFactory, userJID);
        } else if (MediaVersionManager.is2dot4Media(serverVersion)) {
            mediaStoreServer = new Device2Dot4MediaStoreServerImpl(xf, log, db, resourceFactory, userJID);
        } else if (MediaVersionManager.is3dot2Media(serverVersion)) {
            mediaStoreServer = new Device3Dot2MediaStoreServerImpl(xf, log, db, resourceFactory, userJID);
        } else {
            throw new MRException(MediaStoreServiceExceptionCodes.INVALID_MEDIA_STORE_VERSION, "Invalid Media Store Version");
        }

        storeServerList.put(serverVersion, mediaStoreServer);
    }
    
    public void initInternalReliability() {
        
    	try {
			for(String storeDir: mediaStoreList.keySet()) {
				MediaStoreDirectoryInfo dirInfo  = mediaStoreList.get(storeDir);
				MediaStoreServiceUtils.updateMediaStreamDirResourceState(dirInfo.getMediaStoreDirNID(), dirInfo.getMediaStoreState(), MRRequest.createRequest(userJID),
						this.xf.xmpp(userJID), log, db, resourceFactory, this.xf.sf(this.userJID));
				
			}
		} catch (MRException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
        
    }


    protected void initInternal(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, 
    		String userJID, Document setServiceDomainResponseDoc, StreamingServerI streamingServer)
        throws Exception {
        this.xf = xf;
        this.log = log;
        this.db = db;
        this.resourceFactory = resourceFactory;
        this.userJID = userJID;
        this.streamingServer = streamingServer;

        mediaStoreList = new HashMap<String, MediaStoreDirectoryInfo>();

        String xpath = "//PortResourceNIDList/resourceNID[@type='" + AMConst.MEDIA_STORE_PORT_RESOURCE + "']";
        this.mediaStorePortResourceNID = MRXMLUtils.getValueXpath(setServiceDomainResponseDoc, xpath, "");
        log.addInfo("MediaStorePortResource:=" + mediaStorePortResourceNID, MRLog.INFO, MRLog.NOTIFY);

        xpath = "//deviceNID";
        this.deviceNID = MRXMLUtils.getValueXpath(setServiceDomainResponseDoc, xpath, "");

        MediaStoreServiceConst.init();
        MediaVersionManager.init(log);
        storeAgentList.put(userJID, this);
    }

    protected void sendAgentStateEvent() {
        try {
            Set<String> keySet = this.mediaStoreList.keySet();
            MediaStoreDirectoryInfo storedirInfo = null;

            for (String storeName : keySet) {
                //if any store state is busy then just return
                storedirInfo = this.mediaStoreList.get(storeName);

                if (MediaStoreServiceConst.BUSY.equals(storedirInfo.getMediaStoreState())) {
                    return;
                }
            }
            
            if(!this.xf.sf(userJID).getAgentState(MediaStoreAdminServerAgent.class.getName()))
            	ServiceAgentUtils.sendAgentStateEvent(this.userJID, MRConst.AGENT_STATE, MRClientAgent.class.getName(), MediaStoreAdminServerAgent.class.getName(), MediaStoreServiceConst.READY, this.xf.xmpp(userJID), this.log);
            else 
            	ServiceAgentUtils.sendAgentResourceStateEvent(this.userJID, MRConst.AGENT_RESOURCE_STATE, MRClientAgent.class.getName(), MediaStoreAdminServerAgent.class.getName(), mediaStorePortResourceNID,AMConst.MEDIA_STORE_PORT_RESOURCE, MediaStoreServiceConst.READY, this.xf.xmpp(userJID), this.log);
            
        } catch (Exception e) {
            log.addInfo("Sending Store Ready Event Failed", MRLog.INFO, MRLog.NOTIFY);
            log.addInfo("**** Device Not Ready To Accept Events ******", MRLog.INFO, MRLog.NOTIFY);
            e.printStackTrace();
        }
    }

    public MediaStoreServerI getMediaStoreServer() {
	
        return getMediaStoreServer(MediaVersionManager.getMediaCurrentVersion(log));
    }

    public MediaStoreServerI getMediaStoreServer(String serverVersion) {
	  
        if ((serverVersion.equals(MediaVersionManager.MEDIA_STORE_VERSION_2_DOT_4)) || (serverVersion.equals(MediaVersionManager.MEDIA_STORE_VERSION_3_DOR_2))) {
            return storeServerList.get(serverVersion);
        } else {
            return storeServerList.get(MediaVersionManager.getMediaCurrentVersion(log));
        }
    }

    public void handleResponse(MRRequest request, String logData) throws Exception {
    }


    protected void callSynchDirRequest(MediaStoreDirectoryInfo storeDirInfo, String presence)
        throws MRException, Exception {
        try {
            callSynchDirRequest(storeDirInfo.getStoreName(), presence);

            if (presence.equals(AMConst.AVAILABLE)) {
                String trashDirPath = storeDirInfo.getMediaStoreRootPath() + "/" + TRASH_DIR_NAME;
                long deleterPort = getDeleterPortRunningNumber();
                this.streamingServer.addDeleterPort(String.valueOf(deleterPort), trashDirPath);
                storeDirInfo.setDeleterPort(deleterPort);
            } else if (presence.equals(AMConst.UNAVAILABLE)) {
                if (storeDirInfo.getDeleterPort() != 0) {
                    this.streamingServer.deleteDeleterPort(String.valueOf(storeDirInfo.getDeleterPort()));
                }
            }
        } catch (Exception exp) {
            exp.printStackTrace();
            storeDirInfo.setMediaStoreState(MediaStoreServiceConst.ERROR);
            MediaStoreServiceUtils.updateMediaStreamDirResourceState(storeDirInfo.getMediaStoreDirNID(), MediaStoreServiceConst.ERROR, MRRequest.createRequest(this.userJID), xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
        }
    }

    protected void callSynchDirRequest(String storeName, String presence) throws MRException, Exception {
        String requestName = MRRequest.getRequestName(SynchMediaStoreDirRequestHandler.class.getName());
        Document synchDirRequestDoc = MediaStoreServiceUtils.loadMediaStoreRequestOrResponseDocument(requestName, this.log);
        MRXMLUtils.setValueXpath(synchDirRequestDoc, "//" + MediaStoreServiceConst.DIR_NAME_EL_NAME, storeName);
        MRXMLUtils.setValueXpath(synchDirRequestDoc, "//" + MediaStoreServiceConst.STATE_EL_NAME, presence);

        MRRequest request = MRRequest.createRequest(MRRequest.createRequest(this.userJID), synchDirRequestDoc, requestName, ServiceClient.MEDIASTORESERVICE);
        SynchMediaStoreDirRequestHandler synchDirRequestHandler = new SynchMediaStoreDirRequestHandler();
        log.addInfo("Synch the directory.  DirName :=" + storeName + " Presence:=" + presence, MRLog.INFO, MRLog.NOTIFY);
        synchDirRequestHandler.getResponse(request, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
    }

    //sends the get max revision request to appserver.
    protected void sendGetMaxRevisionRequestToAppserverAgent(MediaStoreDirectoryInfo storeDirInfo, MRLog log, XMPPI xmpp)
        throws MRException, Exception {
        String requestName = MRRequest.getRequestName(GetMediaStreamFileMaxRevisionRequestHandler.class.getName());
        Document assetRequestDoc = AssetAdminServiceUtils.getAssetAdminRequest(log, requestName + ".xml");
        MediaStoreServerI mediaStoreServer = this.getMediaStoreServer();
        int mediaStoreRevision = mediaStoreServer.getMediaStoreRevisionNumber(storeDirInfo.getMediaStoreRootPath(), storeDirInfo.isWritable());
        log.addInfo("Media Store Revision :=" + mediaStoreRevision, MRLog.INFO, MRLog.NOTIFY);
        MRXMLUtils.setValueXpath(assetRequestDoc, "//" + GetMediaStreamFileMaxRevisionRequestHandler.MEDIA_STORE_REVISION_ELEMENT, String.valueOf(mediaStoreRevision));
        MRXMLUtils.setValueXpath(assetRequestDoc, "//" + GetMediaStreamFileMaxRevisionRequestHandler.MEDIA_STREAM_DIR_NID, storeDirInfo.getMediaStoreDirNID());

        String fromAgentClassName = MediaStoreAdminServerAgent.class.getName();
        String xmppAgentJID =  AppServerAgentUtils.getAppServerAgentJID(this.userJID,xf.sf(userJID));
        MediaRoomServiceUtils.sendMessageToAppServerAgent(xmpp, this.userJID, assetRequestDoc, log, requestName, fromAgentClassName, xmppAgentJID);
    }
   

    /////////// file import /export /////////////

    /////////////////////// handle response ///////////////
    private void handleExportImportRequest(Element queueEL, MRClient client, Document messageDoc, String fromJID, String requestName) {
        long t0 = System.currentTimeMillis();
        boolean messageSent = false;
        StringBuffer data = new StringBuffer();
        int responseState = 0;
        StringBuffer response = new StringBuffer();
        String fromAgentClassName = this.getClass().getName();
        //debug
        String state = MRXMLUtils.getAttributeValue(queueEL, "state", "");
        String nid = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
        try {
            try {
                boolean busy = false;
                if (!FileSM.EXPORT_IMPORT_CLEANUP_TEMP_FILES.equals(requestName) && !FileSM.EXPORT_IMPORT_ABORT_TEMP_FILE.equals(requestName)) {
                    busy = isBusy();
                }
                if (busy) {
                    data.append("<mediafile_converter_status>").append("BUSY").append("</mediafile_converter_status>");
                    throw new Exception("MediaStore server is busy. Please try later.");
                }
                if (FileSM.EXPORT_IMPORT_FILE_STATUS_REQUEST.equals(requestName)) {
                    if (!busy) {
                        data.append("<mediafile_converter_status>").append("FREE").append("</mediafile_converter_status>");
                    }
                } else if (FileSM.EXPORT_FILE_REQUEST.equals(requestName) || FileSM.IMPORT_FILE_REQUEST.equals(requestName)) {
                    this.handleGetBlobPathRequest(data, queueEL, client, messageDoc, fromJID, requestName);
                } else if (FileSM.EXPORT_FILE_PACK_REQUEST.equals(requestName)) {
                    this.handleExportFilePackRequest(data, queueEL, client, messageDoc, fromJID, requestName);
                } else if (FileSM.IMPORT_FILE_UNPACK_REQUEST.equals(requestName)) {
                    this.handleImportFileUnPackRequest(data, queueEL, client, messageDoc, fromJID, requestName);
                } else if (FileSM.EXPORT_CLEANUP_TEMP_FILES.equals(requestName)) {
                    this.handleExportImportCleanupTempFileRequest(data, queueEL, client, messageDoc, fromJID, requestName);
                } else if (FileSM.IMPORT_CLEANUP_TEMP_FILES.equals(requestName)) {
                    this.handleExportImportCleanupTempFileRequest(data, queueEL, client, messageDoc, fromJID, requestName);
                }else if (FileSM.EXPORT_IMPORT_CLEANUP_TEMP_FILES.equals(requestName)) {
                    this.handleExportImportCleanupTempAllFileRequest(data, queueEL, client, messageDoc, fromJID, requestName);
                } else if (FileSM.EXPORT_IMPORT_ABORT_TEMP_FILE.equals(requestName)) {
                    this.handleExportImportAbortFileRequest(data, queueEL, client, messageDoc, fromJID, requestName);
                }
                responseState = MRRequest.STATE_DONE;

            } catch (MRException ex) {
                ex.printStackTrace();
                int errorCode = MediaStoreServiceExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                String error = "handleExportImportRequest: Error during '" + requestName + "' " ;
                if (ex != null) {
                    errorCode = ex.getCode();
                    error += ex.getData();
                    ex.printStackTrace();
                }
                responseState = MRRequest.STATE_EXCEPTION;
                FileSMUtils.addErrorToResponse(queueEL, errorCode, error);
                log.addInfo("ERROR:"+errorCode+":"+error, MRLog.INFO, MRLog.NOTIFY);
            } catch (Exception ex) {
                ex.printStackTrace();
                int errorCode = MediaStoreServiceExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                String error = "handleExportImportRequest: Error during '" + requestName + "' " ;
                if (ex != null) {
                    error += ex.getMessage();
                    ex.printStackTrace();
                }
                responseState = MRRequest.STATE_EXCEPTION;
                FileSMUtils.addErrorToResponse(queueEL, errorCode, error);
                log.addInfo("ERROR:"+errorCode+":"+error, MRLog.INFO, MRLog.NOTIFY);
            }
            try {
                response.append("<data>");
                response.append(data);
                response.append(queueEL.asXML());
                response.append("</data>");
                ServiceAgentUtils.sendAgentResponse(this.xf.xmpp(userJID), this.log, client, messageDoc, fromJID, response.toString(), this.userJID, fromAgentClassName, responseState);
                messageSent = true;
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        } catch (Exception ex) {
            if (ex != null) {
                log.addInfo("MediaStoreAdminServerAgent:handleAgentRequest:ERROR:"+ex.toString(), MRLog.INFO, MRLog.NOTIFY);
                ex.printStackTrace();
            }
        } finally {
            log.addInfo("MediaStoreAdminServerAgent:handleAgentRequest:'" + requestName + "'  DONE (" + (System.currentTimeMillis() - t0) + ") messageSent:" + messageSent + " " + requestName + " state:=" + state + " NID:=" + nid, MRLog.INFO, MRLog.NOTIFY);
        }

    }

    /////////////////////// handle events from tar / untar  ///////////////
    private void handleMediaFileConverterEvent(MRClient client, Document messageDoc, String fromJID) throws Exception {
        long t0 = System.currentTimeMillis();
        String requestName = "";
        String eventType = "";
        requestName = MRXMLUtils.getValueXpath(messageDoc, "//" + DeviceConst.ACTION, "");
        eventType = MRXMLUtils.getValueXpath(messageDoc, "//EventType", "");
        Element queueEL = null;
        Document orig_request_doc = null;
        int responseState = 0;
        StringBuffer response = new StringBuffer();
        String fromAgentClassName = this.getClass().getName();
        StringBuffer data = new StringBuffer();
        String state = "";
        String nid = "";
        boolean messageSent = false;
        try {
            try {
                String clientID = MRXMLUtils.getValueXpath(messageDoc, "//clientID", "");
                orig_request_doc = MRXMLUtils.loadXMLFile(clientID, log);
                queueEL = MRXMLUtils.getElementXpath(orig_request_doc, "//" + AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
                fromJID = MRXMLUtils.getAttributeValue(queueEL, "msJID", "");
                //debug
                state = MRXMLUtils.getAttributeValue(queueEL, "state", "");
                nid = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
                //add to response    
                data.append("<mediaFileConverterEventType>").append(eventType).append("</mediaFileConverterEventType>");
                MRXMLUtils.setAttributeValueXPath(orig_request_doc, MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, FileSM.MEDIA_FILE_CONVERTER_EVENT_REQUEST);
            //   MRXMLUtils.replaceValueXpath(orig_request_doc.getRootElement(), "//requestname", FileSM.MEDIA_FILE_CONVERTER_EVENT_REQUEST);
                try {
                	responseState = MRRequest.STATE_DONE;
                    if (FileSM.MEDIA_FILE_PACK_ERROR_EVENT.equals(eventType) || FileSM.MEDIA_FILE_UNPACK_ERROR_EVENT.equals(eventType)
                            || FileSM.MEDIA_2DOT4_FILE_UNPACK_ERROR_EVENT.equals(eventType)) {
                        String encoder_error = MRXMLUtils.getValueXpath(messageDoc, "//error", "");
                        int errorCode = MediaStoreServiceExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                        String error = encoder_error;
                        throw new MRException(errorCode, error);
                    } else if (FileSM.MEDIA_2DOT4_FILE_UNPACK_COMPLETE_EVENT.equals(eventType)) {
                        handle2dot4Register(queueEL);
                    }
                    
                } catch (MRException ex) {
                    ex.printStackTrace();
                    int errorCode = MediaStoreServiceExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                    String error = "handleMediaFileConverterEvent: Error during '" + requestName + "' ";
                    if (ex != null) {
                        errorCode = ex.getCode();
                        error += ex.getData();
                        ex.printStackTrace();
                    }
                    responseState = MRRequest.STATE_EXCEPTION;
                    FileSMUtils.addErrorToResponse(queueEL, errorCode, error);
                    log.addInfo("ERROR:"+errorCode+":"+error, MRLog.INFO, MRLog.NOTIFY);
                } catch (Exception ex) {
                    ex.printStackTrace();
                    int errorCode = MediaStoreServiceExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                    String error = "handleMediaFileConverterEvent: Error during '" + requestName + "'  ";
                    if (ex != null) {
                        error += ex.getMessage();
                        ex.printStackTrace();
                    }
                    responseState = MRRequest.STATE_EXCEPTION;
                    FileSMUtils.addErrorToResponse(orig_request_doc.getRootElement(), errorCode, error);
                    log.addInfo("ERROR:"+errorCode+":"+error, MRLog.INFO, MRLog.NOTIFY);
                }
                
                try {
                    queueEL.detach();
                    response.append("<data>");
                    response.append(data);
                    response.append(queueEL.asXML());
                    response.append("</data>");
                    ServiceAgentUtils.sendAgentResponse(this.xf.xmpp(userJID), this.log, client, orig_request_doc, fromJID, response.toString(), this.userJID, fromAgentClassName, responseState);
                    messageSent = true;
                } catch (Exception ex) {
                    log.addInfo("MediaStoreAdminServerAgent:handleMediaFileConverterEvent:'" + requestName + "' ERROR:"+ex.toString(), MRLog.INFO, MRLog.NOTIFY);
                    ex.printStackTrace();
                }
                
                try { 
                	String queueELtype = MRXMLUtils.getAttributeValue(queueEL, "type", "");
                    if(FileSM.TYPE_OFFLOAD.equals(queueELtype) && FileSM.MEDIA_FILE_PACK_COMPLETE_EVENT.equals(eventType)) {
                    	String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
                    	String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_UUID", "");
                    	
                    	String rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID);
                        String tempFolderPath = FileSMUtils.getDMSWorkAreaPath(queueEL, rootFolderPath);
                        String tarPath = FileSMUtils.getDMSFilePath_export(queueEL);
                        
                        File db_extract_folder = new File(tempFolderPath, uuid + "/db");
                        db_extract_folder.mkdirs();
                        File requetsXMLPath = new File(db_extract_folder.getParentFile(), "request.xml");
                    	
                        handleFileCopyDMSToNAS(queueEL, tarPath, requetsXMLPath.getAbsolutePath());
                    }
				} catch (Exception e) {
					 log.addInfo("MediaStoreAdminServerAgent:handleMediaFileConverterEvent:'" + requestName + "' ERROR:"+e.toString(), MRLog.INFO, MRLog.NOTIFY);
	                 e.printStackTrace();
				}
            } catch (Exception ex) {
                if (ex != null) {
                    ex.printStackTrace();
                    log.addInfo("MediaStoreAdminServerAgent:handleMediaFileConverterEvent:'" + requestName + "' ERROR:"+ex.toString(), MRLog.INFO, MRLog.NOTIFY);
                }
            }
        } finally {
            log.addInfo("MediaStoreAdminServerAgent:handleMediaFileConverterEvent: DONE (" + (System.currentTimeMillis() - t0) + ") " + " messageSent:" + messageSent + " " + requestName + " eventType:=" + eventType + " state:=" + state + " NID:=" + nid, MRLog.INFO, MRLog.NOTIFY);
        }

    }
    
    private void handleNFSUploadEvent(MRClient client, Document messageDoc, String fromJID) throws Exception {
        long t0 = System.currentTimeMillis();
        String requestName = "";
        String eventType = "";
        requestName = MRXMLUtils.getValueXpath(messageDoc, "//" + DeviceConst.ACTION, "");
        eventType = MRXMLUtils.getValueXpath(messageDoc, "//EventType", "");
        Element queueEL = null;
        Document orig_request_doc = null;
        int responseState = 0;
        StringBuffer response = new StringBuffer();
        String fromAgentClassName = this.getClass().getName();
        StringBuffer data = new StringBuffer();
        String state = "";
        String nid = "";
        boolean messageSent = false;
        
        try {
            try {
                String clientID = MRXMLUtils.getValueXpath(messageDoc, "//clientID", "");
                orig_request_doc = MRXMLUtils.loadXMLFile(clientID, log);
                queueEL = MRXMLUtils.getElementXpath(orig_request_doc, "//" + AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
                fromJID = MRXMLUtils.getAttributeValue(queueEL, "msJID", "");
                //debug
                state = MRXMLUtils.getAttributeValue(queueEL, "state", "");
                nid = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
                
                // Delete blob entry successfully before sending nfs upload notification
                try {
                	if (!FileSM.MEDIA_FILE_NFS_ERROR_EVENT.equals(eventType)) {
                			handleDeleteOffloadFile(queueEL);
                	}
                } catch (Exception e) {
            		eventType = FileSM.MEDIA_FILE_NFS_ERROR_EVENT;
        			MRXMLUtils.getValueXpath(messageDoc, "//error", "");
        			MRXMLUtils.setValueXpath(messageDoc, "//error", "Failed to Delete the blob Store Entry..");
                }
                
                //add to response    
                data.append("<nfsUploadEventType>").append(eventType).append("</nfsUploadEventType>");
                try {
                    if (FileSM.MEDIA_FILE_NFS_ERROR_EVENT.equals(eventType)) {
                        String encoder_error = MRXMLUtils.getValueXpath(messageDoc, "//error", "");
                        int errorCode = MediaStoreServiceExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                        String error = encoder_error;
                        FileSMUtils.addErrorToResponse(queueEL, errorCode, error);
                        log.addInfo("ERROR:"+errorCode+":"+error, MRLog.INFO, MRLog.NOTIFY);
                    }
                    responseState = MRRequest.STATE_DONE;
                } catch (MRException ex) {
                    ex.printStackTrace();
                    int errorCode = MediaStoreServiceExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                    String error = "handleNFSUploadEvent: Error during '" + requestName + "' ";
                    if (ex != null) {
                        errorCode = ex.getCode();
                        error += ex.getData();
                        ex.printStackTrace();
                    }
                    responseState = MRRequest.STATE_EXCEPTION;
                    FileSMUtils.addErrorToResponse(queueEL, errorCode, error);
                    log.addInfo("ERROR:"+errorCode+":"+error, MRLog.INFO, MRLog.NOTIFY);
                } catch (Exception ex) {
                    ex.printStackTrace();
                    int errorCode = MediaStoreServiceExceptionCodes.MEDIA_IMPORT_EXPORT_EXCEPTION;
                    String error = "handleNFSUploadEvent: Error during '" + requestName + "'  ";
                    if (ex != null) {
                        error += ex.getMessage();
                        ex.printStackTrace();
                    }
                    responseState = MRRequest.STATE_EXCEPTION;
                    FileSMUtils.addErrorToResponse(orig_request_doc.getRootElement(), errorCode, error);
                    log.addInfo("ERROR:"+errorCode+":"+error, MRLog.INFO, MRLog.NOTIFY);
                }
                try {
                    queueEL.detach();
                    response.append("<data>");
                    response.append(data);
                    response.append(queueEL.asXML());
                    response.append("</data>");
                    ServiceAgentUtils.sendAgentResponse(this.xf.xmpp(userJID), this.log, client, orig_request_doc, fromJID, response.toString(), this.userJID, fromAgentClassName, responseState);
                    messageSent = true;
                } catch (Exception ex) {
                    log.addInfo("MediaStoreAdminServerAgent:handleNFSUploadEvent:'" + requestName + "' ERROR:"+ex.toString(), MRLog.INFO, MRLog.NOTIFY);
                    ex.printStackTrace();
                }
            } catch (Exception ex) {
                if (ex != null) {
                    ex.printStackTrace();
                    log.addInfo("MediaStoreAdminServerAgent:handleNFSUploadEvent:'" + requestName + "' ERROR:"+ex.toString(), MRLog.INFO, MRLog.NOTIFY);
                }
            }
        } finally {
            log.addInfo("MediaStoreAdminServerAgent:handleNFSUploadEvent: DONE (" + (System.currentTimeMillis() - t0) + ") " + " messageSent:" + messageSent + " " + requestName + " eventType:=" + eventType + " state:=" + state + " NID:=" + nid, MRLog.INFO, MRLog.NOTIFY);
        }

    }
 
       
    /////////////////////// handle events from tar / untar  ///////////////
    /////////////////////// handle response ///////////////
    private String  handle2dot4Register(Element queueEL) throws Exception {
        String mediaStreamFileNID = null;
        boolean registerComplete = false;
        Element file_root = null;
        String mediaStreamDirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
        String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "");
        String title = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@title", "");
        String nid = MRXMLUtils.getAttributeValue(queueEL, "NID", "");
        try {
            //read File NID from UUID, if exist done ...else continue
            file_root = readFrom_db(AMConst.MEDIA_STREAM_FILE_RESOURCE, "//UUID='" + uuid + "'", MRRequest.createRequest(userJID), db, log);
            //it will never happen...since we are deleting the blob in case of registration failure...
            if (file_root == null) {
                
                //read dir and get groups from dir
                Element dir_root = readFrom_db(AMConst.MEDIA_STREAM_DIR_RESOURCE, "@NID='" + mediaStreamDirNID + "'", MRRequest.createRequest(userJID), db, log);
                Element dirGroups = MRXMLUtils.getElementXpath(dir_root, "//Info/Groups");

                //storeRevision: store info from dir nid
                MediaStoreDirectoryInfo storeInfo = getMediaStoreInfoBasedOnDirNID(mediaStreamDirNID);
                int storeRevision = getMediaStoreServer().getMediaStoreRevisionNumber(storeInfo.getMediaStoreRootPath(), storeInfo.isWritable());

                //MediaStoreServer for 2dot4
                MediaStoreServerI versionStoreServer = getMediaStoreServer(MediaVersionManager.MEDIA_STORE_VERSION_2_DOT_4);

                //create MEDIA_STREAM_FILE_RESOURCE using MEDIA_STORE_VERSION_2_DOT_4 MediaStoreServerI
                mediaStreamFileNID = GetMediaStreamFileMaxRevisionResponseHandler.register2dot4MediaFile(uuid, title, mediaStreamDirNID, 
                        nid, dirGroups, storeRevision, storeInfo, versionStoreServer, 
                        xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
                if (mediaStreamFileNID == null) {
                    log.addInfo("MediaStoreServiceUtils.synchAddMediaFile mediaStreamFileNID: isNULL " + (mediaStreamFileNID == null), MRLog.INFO, MRLog.NOTIFY);
                    throw new Exception("Failed to synch and add media file='" + title + "' uuid='" + uuid + "'");
                }
            }

            //Confirm: registration complete: the readDB File NID using UUID.
            file_root = readFrom_db(AMConst.MEDIA_STREAM_FILE_RESOURCE, "//UUID='" + uuid + "'", MRRequest.createRequest(userJID), db, log);
            if (file_root == null) {
                throw new Exception("Import 2.4 media failed.");
            }
            registerComplete = true;
            mediaStreamFileNID = MRXMLUtils.getAttributeValue(file_root, "NID");
            String dateCreated = MRXMLUtils.getValueXpath(file_root, "//DateCreated", "").trim();
            String cliptType = MRXMLUtils.getValueXpath(file_root, "//MediaStoreFileInfo/Type", "").trim();
            title = MRXMLUtils.getValueXpath(file_root, "//Info/Title", "");
            Element internalEL = MRXMLUtils.getChild(queueEL, "Internal");
            MRXMLUtils.setAttributeValue(internalEL, "cliptType", cliptType);
            MRXMLUtils.setAttributeValue(internalEL, "dateCreated", dateCreated);
            MRXMLUtils.setAttributeValue(internalEL, "fileNID", mediaStreamFileNID);
            MRXMLUtils.setAttributeValue(internalEL, "title", title);        
            log.addInfo("Import 2.4 media register : DONE  nid : "  + nid +" mediaStreamDirNID "+ mediaStreamDirNID +" uuid " + uuid +" title: " + title  , MRLog.INFO, MRLog.NOTIFY);
        }catch(Exception ex){
            log.addInfo("Import 2.4 media register : failed  nid : "  + nid  , MRLog.INFO, MRLog.NOTIFY);
            throw ex;
        } finally {
            if (!registerComplete) {
                log.addInfo("Import 2.4 media register : failed  nid : "  + nid  , MRLog.INFO, MRLog.NOTIFY);
                //delete the blob and db file 
                String rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(mediaStreamDirNID);
                try {
                    String[] cmd = { FileSM.SCRIPT_MEDIA_FILE_CONVERSION, "remove_2dot4_media", rootFolderPath, uuid };
                    log.addInfo("remove_2dot4_media :=" + FileSMUtils.joinArray(cmd, " "), MRLog.INFO, MRLog.NOTIFY);
                    StringBuffer buf = new StringBuffer();
                    int retVal = SystemServiceUtils.executeProcessInThread(cmd, buf);
                    if (retVal != 0) {
                        log.addInfo("Error during remove_2dot4_media nid-'"+ nid+"'", MRLog.INFO, MRLog.NOTIFY);
                    }
                }catch(Exception ex){
                    ex.printStackTrace();
                }
            }
        }
         
        
        return mediaStreamFileNID;
    }
    private static Element readFrom_db(String objectType, String whereXpath, MRRequest request, MRDB db, MRLog log) throws Exception {
        Element el = null;
        MRConfig mrConfig = MRConfig.getInstance(objectType, log);
        String listName = mrConfig.getParentNIDKey(log);
        String perentNID = db.getServiceDomainRootNID(mrConfig.getParentNIDKey(log));
        String childXPath = null;
        String[] arrWhereXpath = { whereXpath };
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
    
    private void handleImportFileUnPackRequest(StringBuffer data, Element queueEL, MRClient client, Document messageDoc, String fromJID, String requestName) throws Exception {
        //String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
        String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
        String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "");
        String dateModified = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dateModified", "").trim();
        int mediaStoreFile_Rev = Utils.getIntValue(MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@mediaStoreFile_Rev", "1").trim(),1);
        if(dateModified.equals("")){
            dateModified=xf.xmpp(userJID).getCurrentTimeMillis()+"";
        }
        if(mediaStoreFile_Rev<0){
            mediaStoreFile_Rev=1;
        }
        //will throw error if disk space is not available... call from every time getBlob/unpack/pack happens
        checkAvailableDiskSpaceForExportImportFile(queueEL);
        
        //get paths
        String rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID);
        String tempFolderPath = FileSMUtils.getDMSWorkAreaPath(queueEL, rootFolderPath);
        String tarPath = FileSMUtils.getDMSFilePath_import(queueEL);
        File extract_folder = new File(tempFolderPath, uuid);
        String tarPath_db = FileSMUtils.getDMSDBFilePath_import(queueEL);
        File db_tar = new File(tarPath_db);
        extract_folder.mkdirs();
        String action = "mediafile_unpack";
        boolean isMedia2dot4 = Utils.booleanValue(MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@isMedia2dot4", "false"),false);
        if(isMedia2dot4){
            action = "mediafile_unpack_2dot4";
        }
        File requetsXMLPath = new File(extract_folder, "request.xml");
        saveRequestDocForEvent(messageDoc, fromJID, requetsXMLPath.getAbsolutePath());
        String[] cmd = { FileSM.SCRIPT_MEDIA_FILE_CONVERSION, action, tarPath, extract_folder.getParentFile().getAbsolutePath(), 
                rootFolderPath, db_tar.getName(), uuid, dateModified, mediaStoreFile_Rev+"", requetsXMLPath.getAbsolutePath(), "&" };
        log.addInfo("mediafile_unpack :=" + FileSMUtils.joinArray(cmd, " "), MRLog.INFO, MRLog.NOTIFY);
        StringBuffer buf = new StringBuffer();
        int retVal = SystemServiceUtils.executeProcessInThread(cmd, buf);
        if (retVal != 0) {
            //send back error..
            String error = "Error during unpack the media file-'"+ tarPath+"'";//+ " " + FileSMUtils.joinArray(cmd, " ") + " " + buf.toString();
            log.addInfo(error, MRLog.INFO, MRLog.NOTIFY);
            throw new Exception(error);
        }
    }

    private void handleExportFilePackRequest(StringBuffer data, Element queueEL, MRClient client, Document messageDoc, String fromJID, String requestName) throws Exception {
        StringBuffer buf = new StringBuffer();
        int retVal = 0;
        //String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
        String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
        String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_UUID", "");
        String new_fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_fileNID", "");
        String fileNID = "msfrl_" + StringUtils.right(new_fileNID, StringUtils.indexOf(new_fileNID, "msfrl_"));
       
        String startOffset = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@startOffset", "");
        String endOffset = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@endOffset", "");
        String requestTimeStamp = MRXMLUtils.getAttributeValue(queueEL, "dateCreated", "");
        String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", "Proprietary");
        String streamType = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@streamType", "");
        
        //will throw error if disk space is not available... call from every time getBlob/unpack/pack happens
        checkAvailableDiskSpaceForExportImportFile(queueEL);
        
        //get paths
        String rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID);
        String tempFolderPath = FileSMUtils.getDMSWorkAreaPath(queueEL, rootFolderPath);
        String mediaFolder = FileSMUtils.getDMSFileMediaFolderPath(queueEL, rootFolderPath);
        String tarPath = FileSMUtils.getDMSFilePath_export(queueEL);
        String tarPath_db = FileSMUtils.getDMSDBFilePath_export(queueEL);
        File db_extract_folder = new File(tempFolderPath, uuid + "/db");
        db_extract_folder.mkdirs();
        File requetsXMLPath = new File(db_extract_folder.getParentFile(), "request.xml");
        saveRequestDocForEvent(messageDoc, fromJID, requetsXMLPath.getAbsolutePath());
        
        Document dbXMLdoc = MRXMLUtils.getDocument(new File(mediaFolder, "db.xml"));
        String sdpTemplateDec = URLDecoder.decode(MRXMLUtils.getValueXpath(dbXMLdoc, "//SDP-Template-Src", ""), "UTF-8");
        String sdpContent = SDPUtils.generateSDPFromSDPTemplate(sdpTemplateDec, "0.0.0.0", 0);
        String sdpFix = SDPUtils.fixSDPProblemWithOlderMedia(sdpContent);
        
        Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
        String sdpPath= MRXMLUtils.getAttributeValue(internalEL, "tempFolderPath", "") + "/" + MRXMLUtils.getAttributeValue(internalEL, "new_UUID", "");
        writeSDPFile(sdpPath, sdpFix);
    	
        String standardMediaFileName = MRXMLUtils.getAttributeValue(internalEL, "title", "") + "_" + uuid;
        standardMediaFileName = standardMediaFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_"); // replace any special characters with '_'
        sdpPath = sdpPath + ".sdp";
        
        String[] cmd = { FileSM.SCRIPT_MEDIA_FILE_CONVERSION, "mediafile_pack", tarPath, mediaFolder, db_extract_folder.getParentFile().getAbsolutePath(), tarPath_db, requetsXMLPath.getAbsolutePath(), fileNID, startOffset, endOffset, requestTimeStamp, mediaFormat, streamType, sdpPath, standardMediaFileName, "&" };
        log.addInfo("mediafile_pack :=" + FileSMUtils.joinArray(cmd, " "), MRLog.INFO, MRLog.NOTIFY);
        retVal = SystemServiceUtils.executeProcessInThread(cmd, buf);
        if (retVal != 0) {
            //String error = "Error during pack the media file. fileNID :=" + fileNID + "   retVal=" + retVal + " " + FileSMUtils.joinArray(cmd, " ") + " " + buf.toString();
            String error = "Error during pack the media file-'"+ tarPath+"'";//+ " " + FileSMUtils.joinArray(cmd, " ") + " " + buf.toString();
            log.addInfo(error.toString(), MRLog.INFO, MRLog.NOTIFY);
            throw new Exception(error);
        }
        
    }
    
    private void writeSDPFile( String sdpPath, String sdpContent) throws Exception {
    	File sdpFile = new File(sdpPath + ".sdp");
        if(!sdpFile.exists())
        	sdpFile.createNewFile();
        
        FileWriter fstream = new FileWriter(sdpPath + ".sdp");
    	BufferedWriter out = new BufferedWriter(fstream);
    	out.write(sdpContent);
    	out.close();
    }
    
    private void handleFileCopyDMSToNAS(Element queueEL, String tarPath, String clientID) throws Exception {
    	int retVal = 0;
    	StringBuffer buf = new StringBuffer();
    	
    	String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_UUID", "");
    	String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
    	String rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID);
    	String tempFolderPath = FileSMUtils.getDMSWorkAreaPath(queueEL, rootFolderPath);
    	File db_extract_folder = new File(tempFolderPath, uuid + "/db");
        db_extract_folder.mkdirs();
        String standardMediaFileName = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@title", "") + "_" + uuid;
        standardMediaFileName = standardMediaFileName.replaceAll("[\\-\\+\\.\\^\\ :,]", "_"); // replace any special characters with '_'
        String streamType = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@streamType", "");
        
    	Element externalEL = MRXMLUtils.getElement(queueEL, "External");
    	String nasURL = MRXMLUtils.getAttributeValue(externalEL, "nasURL");
    	String mediaFormat = MRXMLUtils.getAttributeValue(queueEL, "format", "Proprietary");
        
    	String[] cmd = { FileSM.SCRIPT_MEDIA_FILE_OFFLOAD, "nfs_upload", tarPath, nasURL, clientID, mediaFormat, db_extract_folder.getParentFile().getAbsolutePath(), standardMediaFileName, streamType, "&" };
        log.addInfo("nfs_upload :=" + FileSMUtils.joinArray(cmd, " "), MRLog.INFO, MRLog.NOTIFY);
        retVal = SystemServiceUtils.executeProcessInThread(cmd, buf);
        if (retVal != 0) {
            String error = "Error offloading the media file-'"+ tarPath+"'";
            log.addInfo(error.toString(), MRLog.INFO, MRLog.NOTIFY);
            throw new Exception(error);
        }
    }
    
    private void handleDeleteOffloadFile(Element queueEL) throws Exception {

    	// Delete Media Store entry for Offloaded file
    	String mediaStoreFileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@prev_UUID", "");
    	String mediaStreamFileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@prev_fileNID", "");
    	String mediaStreamFileDirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
    	String requestNID = "_delete_" + mediaStoreFileNID;
    	Hashtable<String,String> fileMapping  = new Hashtable<String,String>();
    	fileMapping.put(mediaStreamFileNID, mediaStoreFileNID);
        Document requestDoc = MediaStoreServiceUtils.generateRequestDocument(MediaStoreServiceConst.DELETE_MEDIA_STORE_FILE_REQUEST, fileMapping, mediaStreamFileDirNID, "", true,requestNID, log);
        DeleteMediaStoreFileRequestHandler handler = new DeleteMediaStoreFileRequestHandler();
        MRRequest request = MRRequest.createRequest(MRRequest.createRequest(this.userJID), requestDoc, MediaStoreServiceConst.DELETE_MEDIA_STORE_FILE_REQUEST, ServiceClient.ASSETADMINSERVICE);
        log.addInfo("Delete Store File UUID:=" + mediaStoreFileNID, MRLog.INFO, MRLog.NOTIFY);
        handler.getResponse(request, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
    }

    private void handleGetBlobPathRequest(StringBuffer data, Element queueEL, MRClient client, Document messageDoc, String fromJID, String requestName) throws Exception {
        String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
        String rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID);
        String tempFolderPath = FileSMUtils.getDMSWorkAreaPath(queueEL, rootFolderPath);
        String type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
        Element internalEL = MRXMLUtils.getElement(queueEL, "Internal");
        MRXMLUtils.setAttributeValue(internalEL, "rootFolderPath", rootFolderPath);
        MRXMLUtils.setAttributeValue(internalEL, "tempFolderPath", tempFolderPath);
        if (type.equals(FileSM.TYPE_IMPORT)) {
            int mediaStoreFile_Rev = 1;
            try {
                MediaStoreServerI mediaStoreServer = this.getMediaStoreServer();
                mediaStoreFile_Rev = mediaStoreServer.getMediaStoreRevisionNumber(rootFolderPath, true);
            } catch (Exception ex) {
                mediaStoreFile_Rev = 1;
                ex.printStackTrace();
            }
            MRLog.println("handleGetBlobPathRequest  type " + type +" mediaStoreFile_Rev " +mediaStoreFile_Rev);
            MRXMLUtils.setAttributeValue(internalEL, "mediaStoreFile_Rev", mediaStoreFile_Rev + "");
        }
        if (type.equals(FileSM.TYPE_EXPORT) || type.equals(FileSM.TYPE_OFFLOAD)) {
            long blobSize = 0;
            try {
                String mediaFolder = FileSMUtils.getDMSFileMediaFolderPath(queueEL, rootFolderPath);
                File uuid_folder = new File(mediaFolder);
                if (uuid_folder.exists()) {
                    String[] cmd = { "du", uuid_folder.getAbsolutePath() };
                    StringBuffer buf = new StringBuffer();
                    int retVal = SystemServiceUtils.executeProcess(cmd, buf);
                    if (retVal != 0) {
                        String error = "Error during getting folder size folder=" + uuid_folder.getAbsolutePath() + " " + FileSMUtils.joinArray(cmd, " ") + " " + buf.toString();
                        log.addInfo(error, MRLog.INFO, MRLog.NOTIFY);
                    } else {
                        String[] arr = buf.toString().trim().split("\\s+");
                        blobSize = Utils.getLongValue(arr[0].trim(), 0);
                        //debug: force huze blob size 
                        //blobSize = Utils.getLongValue("69910482944",0);
                        MRXMLUtils.setAttributeValue(queueEL, "blobSize", blobSize + "");
                    }
                } else {
                    throw new Exception("MediaFile folder does not exists " + uuid_folder.getAbsolutePath());
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        //throw error if disk space is not available... call from every time getBlob/unpack/pack happens
        checkAvailableDiskSpaceForExportImportFile(queueEL);
    }    

    private void checkAvailableDiskSpaceForExportImportFile(Element queueEL) throws MRException, Exception {
        //throw error check disk space -- if current file can not be imported or exported 
        String desc  = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@title", "");
        String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
        String type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
        long blobSize = Utils.getLongValue(MRXMLUtils.getAttributeValue(queueEL, "blobSize", "0"), 0);
        if(desc .equals("")){
            desc = MRXMLUtils.getAttributeValueXpath(queueEL, "External/@fileName", "");
        }
        checkAvailableDiskSpaceForExportImportFile(blobSize, type, desc ,  dirNID);
    }
    
    private void handleExportImportCleanupTempFileRequest(StringBuffer data, Element queueEL, MRClient client, Document messageDoc, String fromJID, String requestName) throws Exception {
        String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
        //get paths
        String rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID);
        String tempFolderPath = FileSMUtils.getDMSWorkAreaPath(queueEL, rootFolderPath);
        String tarPath = FileSMUtils.getDMSFilePath_export(queueEL);
        String tarPathStandard = FileSMUtils.getDMSFilePath_export_standard(queueEL);
        String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_UUID", "");
        String type = MRXMLUtils.getAttributeValue(queueEL, "type", "");
        String mediaGroupID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@new_mediaGroupID", "");
        //String isMedia2dot4 = MRXMLUtils.getAttributeValueXpath(queueEL, "isMedia2dot4", "true");
        if (type.equals(FileSM.TYPE_IMPORT)) {
            tarPath = FileSMUtils.getDMSFilePath_import(queueEL);
            String mediaFolder = FileSMUtils.getDMSFileMediaFolderPath(queueEL, rootFolderPath);
            File uuid_folder = new File(mediaFolder);
            File db_xml = new File(uuid_folder, "db.xml");
            boolean isMedia2dot4 = Utils.booleanValue(MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@isMedia2dot4", "false"),false);
            if(isMedia2dot4) {
                //for now nothing to be done
            } else {
                if (!db_xml.exists()) {
                    MRLog.println("handleExportImportCleanupTempFileRequest db.xml does not exist? " + db_xml.getAbsolutePath());
                } else {
                    //if db.xml exits than the import has moved to a stage where it can be upgraded
                    try {
                        Document doc = MRXMLUtils.loadXMLFile(db_xml.getAbsolutePath(), log);
                        MRXMLUtils.setValueXpath(doc.getRootElement(), "//MediaGroupID", mediaGroupID);
                        MRXMLUtils.setValueXpath(doc.getRootElement(), "//State", "Ready");
                    } catch (Exception ex) {
                        MRLog.println("handleExportImportCleanupTempFileRequest db.xml  error " + ex.getMessage());
                        ex.printStackTrace();
                    }
                    
                    try {
                        String fileNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@fileNID", "");
                        callUpdgradeFileRequest(queueEL, fileNID);
                    } catch (Exception ex) {
                        MRLog.println("handleExportImportCleanupTempFileRequest callUpdgradeFileRequest error " + ex.getMessage());
                        ex.printStackTrace();
                    }                
                }
            }
            
        }
        //delete the tar
        File tarPath_file = new File(tarPath);
        tarPath_file.delete();
        
        //delete the standard zip
        File tarPathStd = new File(tarPathStandard);
        tarPathStd.delete();
        
        //delete the temp folder
        File temp_folder = new File(tempFolderPath, uuid);
        if (temp_folder.exists()) {
            String[] cmdUntar = { "rm", "-rf", temp_folder.getAbsolutePath() };
            StringBuffer buf = new StringBuffer();
            int retVal = SystemServiceUtils.executeProcess(cmdUntar, buf);
            if (retVal != 0) {
                String error = "Error during handleExportImportCleanupTempFileRequest :=" + tempFolderPath + "  retVal=" + retVal + " " + FileSMUtils.joinArray(cmdUntar, " ") + " " + buf.toString();
                log.addInfo(error, MRLog.INFO, MRLog.NOTIFY);
                throw new Exception(error);
            }
        }
        
        // FIX: This is to remove the uuid folders accumulated before the cleanup.
        if (type.equals(FileSM.TYPE_EXPORT)) { 
        	File tempDir = new File(tempFolderPath);
            if(tempDir.exists() && tempDir.listFiles().length > 0) {
            	for(File file : tempDir.listFiles())
                	file.delete();
            }
        }
       
        //update dir size after cleaning temporary files
       updateDirSize(dirNID);
    	
    }
    
    
    
    public void updateDirSize(String dirNID){    	
    	 try {
			MediaStoreDirectoryInfo storeDirInfo = getMediaStoreInfoBasedOnDirNID(dirNID);
			 long sizeUsed = MediaStoreServiceUtils.updateMediaDirSizeUsed(storeDirInfo.isInternal(), storeDirInfo.getStoreDeviceElementID(), storeDirInfo.getMediaStoreDirNID(), 
					MRRequest.createRequest(userJID), xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
			if (sizeUsed != 0) {
				storeDirInfo.setSizeUsed(sizeUsed);
			}
		} catch (MRException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}
    }

    private void handleExportImportAbortFileRequest(StringBuffer data, Element queueEL, MRClient client, Document messageDoc, String fromJID, String requestName) throws Exception {
        try {
            String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
            String uuid = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@UUID", "");
            //get paths
            String rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID);
            String tempFolderPath = FileSMUtils.getDMSWorkAreaPath(queueEL, rootFolderPath);

            //since this is abort delete the blob folder
            File uuid_folder = new File(rootFolderPath, uuid);
            FileSMUtils.deleteDirectory(uuid_folder);

            //check if still unpack is running... 
            boolean busy = false;
            try {
                busy = isMediaFileConverterBusy();
            } catch (Exception ex) {
            }
            if (busy) {
                int queue_state = Utils.getIntValue(MRXMLUtils.getAttributeValue(queueEL, "queue_state", ""), -1);
                if (queue_state == FileSMQueue.QUEUE_STATUS_RUNNING) {
                    //todo: use queueNID to find which element is currently running unpack/pack
                    //for now assume this is the current one...since done sequentialy..  and current element is running..
                    File extract_folder = new File(tempFolderPath, uuid);
                    File requetsXMLPath = new File(extract_folder, "request.xml");
                    if (requetsXMLPath.exists()) {
                        StringBuffer buf = new StringBuffer();
                        String[] cmd = { FileSM.SCRIPT_MEDIA_FILE_CONVERSION, "stop" };
                        log.addInfo("stopMediaFileConverter stop :=" + FileSMUtils.joinArray(cmd, " "), MRLog.INFO, MRLog.NOTIFY);
                        int retVal = SystemServiceUtils.executeProcess(cmd, buf);
                        if (retVal != 0) {
                            log.addInfo("stopMediaFileConverter:Script execution failed '" + FileSM.SCRIPT_MEDIA_FILE_CONVERSION + "' return value non zero retVal=" + retVal + " " + buf.toString(), MRLog.INFO, MRLog.NOTIFY);
                        }
                    }
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        //finally delete all other temp folders
        handleExportImportCleanupTempFileRequest(data, queueEL, client, messageDoc, fromJID, requestName);
        return;
    }

    private void handleExportImportCleanupTempAllFileRequest(StringBuffer data, Element queueEL, MRClient client, Document messageDoc, String fromJID, String requestName) throws Exception {
        try {
            String dirNID = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@dirNID", "");
            //get paths
            String rootFolderPath = null;
            try {
                rootFolderPath = this.getMediaStoreRootPathBasedOnDirNID(dirNID);
            } catch (Exception ex) {
                log.addInfo("Error handleExportImportCleanupTempAllFileRequest:=" + dirNID + " rootFolderPath " + rootFolderPath + " " + ex.toString(), MRLog.INFO, MRLog.NOTIFY);
                return;
            }
            String tempFolderPath = FileSMUtils.getDMSWorkAreaPath_ExportRoot(rootFolderPath);
            log.addInfo("Error handleExportImportCleanupTempAllFileRequest:=" + dirNID + " rootFolderPath " + rootFolderPath, MRLog.INFO, MRLog.NOTIFY);
            //delete the temp folder
            StringBuffer buf = new StringBuffer();
            File temp_folder = new File(tempFolderPath);
            if (temp_folder.exists()) {
                String[] cmd = { "rm", "-rf", temp_folder.getAbsolutePath() };
                try {
                    if (IPVSConst.getInstance().getOS().equals("windows")) {
                    } else {
                        SystemServiceUtils.executeProcess(cmd, buf);
                    }
                } catch (Exception ex) {
                    log.addInfo("Error handleExportImportCleanupTempAllFileRequest:=" + FileSMUtils.joinArray(cmd, " "), MRLog.INFO, MRLog.NOTIFY);
                    ex.printStackTrace();
                }
            }
            temp_folder.mkdirs();
            tempFolderPath = FileSMUtils.getDMSWorkAreaPath_ImportRoot(rootFolderPath);
            //delete the temp folder
            temp_folder = new File(tempFolderPath);
            if (temp_folder.exists()) {
                String[] cmd = { "rm", "-rf", temp_folder.getAbsolutePath() };
                try {
                    if (IPVSConst.getInstance().getOS().equals("windows")) {
                    } else {
                        SystemServiceUtils.executeProcess(cmd, buf);
                    }
                } catch (Exception ex) {
                    log.addInfo("Error handleExportImportCleanupTempAllFileRequest:=" + FileSMUtils.joinArray(cmd, " "), MRLog.INFO, MRLog.NOTIFY);
                    ex.printStackTrace();
                }
            }
            temp_folder.mkdirs();          
            
            //update dir size after cleaning temporary files
            updateDirSize(dirNID);
            
        } catch (Exception ex) {
            log.addInfo("Error handleExportImportCleanupTempAllFileRequest", MRLog.INFO, MRLog.NOTIFY);
            ex.printStackTrace();
        }

    }

    //////////// todo

    ////////////////// utils 
    private boolean isBusy() throws Exception {
        log.addInfo("isBusy:CHECK_BUSY cmd"  , MRLog.INFO, MRLog.NOTIFY);
        if (isMediaFileConverterBusy()) {
            return true;
        }
        if (checkFileTransferBusy()) {
            return true;
        }
        return false;
    }

    private boolean isMediaFileConverterBusy() throws Exception {
        StringBuffer buf = new StringBuffer();
        String[] cmd = { FileSM.SCRIPT_MEDIA_FILE_CONVERSION, "status" };
        int retVal = SystemServiceUtils.executeProcess(cmd, buf);
        if (retVal != 0) {
            log.addInfo("isMediaFileConverterBusy:CHECK_BUSY Script execution failed '" + FileSM.SCRIPT_MEDIA_FILE_CONVERSION + "' return value non zero retVal=" + retVal + " " + buf.toString(), MRLog.INFO, MRLog.NOTIFY);
            throw new Exception("isMediaFileConverterBusy:Script execution failed '" + FileSM.SCRIPT_MEDIA_FILE_CONVERSION + "' return value non zero retVal=" + retVal + " " + buf.toString());
        }
        if (buf.indexOf("BUSY") != -1) {
            log.addInfo("isMediaFileConverterBusy:CHECK_BUSY ExportImort:=busy. Script execution '" + FileSM.SCRIPT_MEDIA_FILE_CONVERSION + "' " + buf.toString(), MRLog.INFO, MRLog.NOTIFY);
            return true;
        } else if (buf.indexOf("FREE") != -1) {
            return false;
        } else {
            log.addInfo("isMediaFileConverterBusy:CHECK_BUSY Script execution returned unknown response '" + FileSM.SCRIPT_MEDIA_FILE_CONVERSION + "' return value non zero retVal=" + retVal + " " + buf.toString(), MRLog.INFO, MRLog.NOTIFY);
            throw new Exception("isMediaFileConverterBusy: Script execution returned unknown response '" + FileSM.SCRIPT_MEDIA_FILE_CONVERSION + "' return value non zero retVal=" + retVal + " " + buf.toString());
        }
    }

    private boolean checkFileTransferBusy() throws Exception {
        List<String> list = xf.xmpp(userJID).getRequestIDs_fileTransfer();
        if (list != null && list.size() > 0) {
            for (String id : list) {
                Object object = xf.xmpp(userJID).getRequest(id);
                if(object !=null){
                    if (FileSMUtils.isExportImportFileTransferID(object.toString())) {
                        log.addInfo("checkFileTransferBusy:CHECK_BUSY File Transfer Busy='" + object.toString() + "' ", MRLog.INFO, MRLog.NOTIFY);
                        //todo:to check
                        //return true;
                    }
                }
            }
        }
        return false;
    }
   /* private boolean abortFileTransfer() throws Exception {
        List<String> list = xmpp.getRequestIDs_fileTransfer();
        log.addInfo("checkFileTransferBusy:CHECK_BUSY File Transfer list.size()='" + list.size() + "' ", MRLog.INFO, MRLog.NOTIFY);
        if (list != null && list.size() > 0) {
            for (String id : list) {
                Object object = xmpp.getRequest(id);
                if(object instanceof IPVSSendFile){
                    IPVSSendFile sendFile=(IPVSSendFile)object;
                    sendFile.closeFile();
                }
                log.addInfo("checkFileTransferBusy:CHECK_BUSY 11111 File Transfer Busy='" + id + "' ='" + object.toString() + "'", MRLog.INFO, MRLog.NOTIFY);
                if (FileSMUtils.isExportImportFileTransferID(object.toString())) {
                    log.addInfo("checkFileTransferBusy:CHECK_BUSY File Transfer Busy='" + object.toString() + "' ", MRLog.INFO, MRLog.NOTIFY);
                    return true;
                }
            }
        }
        log.addInfo("checkFileTransferBusy ExportImort:CHECK_BUSY =not busy", MRLog.INFO, MRLog.NOTIFY);
        return false;
    }*/
    private void saveRequestDocForEvent(Document messageDoc, String fromJID, String filePath) throws Exception {
        Element queueEL = MRXMLUtils.getElementXpath(messageDoc, "//" + AMConst.EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE);
        MRXMLUtils.setAttributeValue(queueEL, "msJID", fromJID);
        FileUtils.writeFile(filePath, messageDoc.asXML());

    }
    protected void callUpdgradeFileRequest(Element queueEL, String fileNID) throws MRException, Exception {
        String requestName = "/com/ipvs/assetadminservice/xml/UpgradeMediaStreamFileResourceRequest.xml";
        Document requestDoc = MRXMLUtils.loadXMLResource(requestName, log);
        MRXMLUtils.setValueXpath(requestDoc, "//" + MediaStoreServiceConst.MEDIA_STREAM_FILE_RESOURCE_NID, fileNID);

        String title = MRXMLUtils.getAttributeValueXpath(queueEL, "Internal/@title", "");
        MRRequest request = MRRequest.createRequest(MRRequest.createRequest(this.userJID), requestDoc, requestName, ServiceClient.ASSETADMINSERVICE);
        UpgradeMediaStreamFileResourceRequestHandler handler = new UpgradeMediaStreamFileResourceRequestHandler();
        log.addInfo("After Import : Updgrade file='"+title+"' :=" + fileNID, MRLog.INFO, MRLog.NOTIFY);
        handler.getResponse(request, xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
    }
    
	@Override
	public void reset() throws Exception {
		storeAgentList.remove(this.userJID);
		storeServerList = null;
		mediaStoreList = null;
	}
}

