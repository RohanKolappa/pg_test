package com.ipvs.device.mediastore;

import java.util.List;
import java.util.Set;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.DeviceAppI;
import com.ipvs.device.impl.DeviceStreamingServerImpl;
import com.ipvs.mediaroomservice.streamingserver.StreamingServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreDirectoryInfo;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceConst;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceExceptionCodes;
import com.ipvs.mediastoreservice.impl.MediaStoreServiceUtils;
import com.ipvs.mediastoreservice.impl.MediaVersionManager;
import com.ipvs.utils.Utils;


public class DeviceMediaStoreAdminServerAgent extends MediaStoreAdminServerAgent {

    private String configFile = null;
    DeviceAppI deviceApp;
    
    public DeviceMediaStoreAdminServerAgent(String localConfigFile, DeviceAppI deviceApp) {
        this.configFile = localConfigFile;
        this.deviceApp = deviceApp;
    }

    protected void handleNASAvailable(String storeName) throws Exception {
	    Document nasStatusDoc = MediaStoreServiceUtils.getNASDriveStatus(log);
	    System.out.println("NASStatausDoc :="+nasStatusDoc.asXML());
	    Document deviceXML = MRXMLUtils.loadXMLFile(this.configFile, log);
	    Element nasConfigEntryEL = MRXMLUtils.getElementXpath(deviceXML, "//NASTable/NASEntry[MediaStore/Name='" + storeName + "']");
	    createMediaStore(nasConfigEntryEL, nasStatusDoc, false);
    }


    // get the media store Info for given media Stream dir NID    
    public MediaStoreDirectoryInfo getMediaStoreInfoBasedOnDirNID(String mediaStreamDirNID)
        throws MRException, Exception {
        Set<String> storeSet = mediaStoreList.keySet();
        MediaStoreDirectoryInfo storeInfo = null;

        for (String storeName : storeSet) {
            storeInfo = mediaStoreList.get(storeName);
            
            if(storeInfo.getMediaStoreDirNID() == null) {
            	continue;
            }

            if (storeInfo.getMediaStoreDirNID().equals(mediaStreamDirNID) || storeInfo.getPvrDirNID().equals(mediaStreamDirNID) || storeInfo.getRecycleBinNID().equals(mediaStreamDirNID)) {
                return storeInfo;
            }
        }

        return null;
    }

    //override to accept the device media store table instance   
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, Document setServiceDomainResponseDoc)
        throws Exception {
        Document deviceXML = MRXMLUtils.loadXMLFile(this.configFile, log);
        List<?> nasDriveList = null;
        List<?> internalDriveList = null;

        if (deviceXML != null) {
            internalDriveList = MRXMLUtils.getListXpath(deviceXML, "//HardDiskEntry");
            nasDriveList = MRXMLUtils.getListXpath(deviceXML, "//NASTable/NASEntry");
        }

        //this will create the default storage 
        StreamingServerI myStreamingServer = new DeviceStreamingServerImpl(userJID, log, db, xf, this.deviceApp);
        initInternal(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc, myStreamingServer);

        //create Internal and NAS Media Stores
        if ((mediaStorePortResourceNID != null) && !mediaStorePortResourceNID.isEmpty()) {
            createMediaStores(internalDriveList, true);
            createMediaStores(nasDriveList, false);
        }

        // if no media stores are present then send get agent state event
        if ((this.mediaStoreList == null) || this.mediaStoreList.isEmpty()) {
            sendAgentStateEvent();

            return;
        }

        //Initialize Media Store Version constants
        for (String mediaVersion : MediaVersionManager.getVersionList()) {
            createMediaStoreSever(mediaVersion, xf, log, db, resourceFactory, userJID);
        }

        Set<String> keySet = this.mediaStoreList.keySet();
        MediaStoreDirectoryInfo storeDirInfo = null;

        for (String storeDirName : keySet) {
            storeDirInfo = this.mediaStoreList.get(storeDirName);
            callSynchDirRequest(storeDirInfo, AMConst.AVAILABLE);

            if (storeDirInfo.getMediaStoreState().equals(MediaStoreServiceConst.START_SYNCH)) {              	
               // send the get media stream file max revision request to app server.
                sendGetMaxRevisionRequestToAppserverAgent(storeDirInfo, log, xf.xmpp(userJID));
                storeDirInfo.setMediaStoreState(MediaStoreServiceConst.BUSY);             
            }
        }

        sendAgentStateEvent();
    }
    
    private void createMediaStore(Element configEntryEl, Document diskStatusDoc, boolean isInternal)
	    throws MRException, Exception {
	    String listIndex = MRXMLUtils.getAttributeValue(configEntryEl, "ListIndex", "");
	    String dirName = MRXMLUtils.getValueXpath(configEntryEl, "MediaStore/Name", "");
	
	    Element statusEL = MediaStoreServiceUtils.getDiskEntry(dirName, diskStatusDoc);
	
	    if (statusEL == null) {
	        return;
	    }
	
	    if (!MediaStoreServiceUtils.checkDiskStatus(MRXMLUtils.elementToDocument(statusEL))) {
	        return;
	    }
	
	    String dirPath = MRXMLUtils.getValueXpath(configEntryEl, "MediaStore/RootDirPath", "");
	
	    if (!isInternal) {
	        dirPath = dirPath + "/" + listIndex + "/";
	    } else {
	        dirPath = dirPath + "/" + MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME + "/";
	    }
	
	    String readOnly = MRXMLUtils.getValueXpath(configEntryEl, "ReadOnly", "");
	
	    long size = Utils.getLongValue(MRXMLUtils.getValueXpath(statusEL, "Parameter[@name='Total']", ""), 0);
	    long sizeUsed = Utils.getLongValue(MRXMLUtils.getValueXpath(statusEL, "Parameter[@name='Used']", ""), 0);
	
	    createMediaStoreDirectory(dirPath, dirName, dirName, mediaStorePortResourceNID, Utils.getBoolean(readOnly, false), size, sizeUsed, isInternal);
	}


    private void createMediaStores(List<?> storeConfigList, boolean isInternal)
        throws MRException, Exception {
        if ((storeConfigList == null) || storeConfigList.isEmpty()) {
            return;
        }

        Document diskStatusDoc = MediaStoreServiceUtils.getDiskStatusDoc(isInternal, log);

        for (Object object : storeConfigList) {
            createMediaStore((Element) object, diskStatusDoc, isInternal);
        }
    }


    public void checkAvailableDiskSpace(String destFileParentDirNID) throws MRException, Exception {
    	MediaStoreDirectoryInfo storedirInfo = getMediaStoreInfoBasedOnDirNID(destFileParentDirNID);
        // MediaStoreDirectoryInfo storedirInfo = this.getMediaStoreInfoBasedOnStreamFileNID(mediaStreamFileNID);
        Element statusEntryEL = null;

        Document diskStatusDoc = MediaStoreServiceUtils.getDiskStatusDoc(storedirInfo.isInternal(), log);

        statusEntryEL = MediaStoreServiceUtils.getDiskEntry(storedirInfo.getStoreDeviceElementID(), diskStatusDoc);

        if (statusEntryEL != null) {
            diskStatusDoc = MRXMLUtils.elementToDocument(statusEntryEL);
        } else {
            diskStatusDoc = null;
        }

        if (diskStatusDoc == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.GET_AVAILABLE_DISK_SPACE_FAILED, "CLI Command Result is Empty");
        }

        String status = MRXMLUtils.getValueXpath(diskStatusDoc, "//Parameter[@name='Status']", "");

        String availableBytes = MRXMLUtils.getValueXpath(diskStatusDoc, "//Parameter[@name='Available']", "");

        if (((status == null) || status.equals("False") || !status.equals("Ok")) && ((availableBytes == null) || (availableBytes.length() == 0))) {
            throw new MRException(MediaStoreServiceExceptionCodes.GET_AVAILABLE_DISK_SPACE_FAILED, "Command Status Falied ");
        }

        long availableDiskSpace = 0;
        availableDiskSpace = Utils.getLongValue(availableBytes, 0);
        log.addInfo("Available Disk Space on Device : " + availableDiskSpace + " Bytes", MRLog.NOTIFY, MRLog.NOTIFY);

        long diskFullThreshold = storedirInfo.getDiskFullThreshold();
        if ((availableDiskSpace <= diskFullThreshold) || (availableDiskSpace <= Utils.getLongValue(MediaStoreServiceConst.DISK_QUOTA_EXCEED_LIMIT, 0))) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_AVAILABLE_DISK_SPACE, "No Disk space Available on Device To Record Media");
        }
    }
    
    public void checkAvailableDiskSpaceForExportImportFile(long blobSize, String type, String title, String destFileParentDirNID) throws MRException, Exception {
        MediaStoreDirectoryInfo storedirInfo = getMediaStoreInfoBasedOnDirNID(destFileParentDirNID);
        // MediaStoreDirectoryInfo storedirInfo = this.getMediaStoreInfoBasedOnStreamFileNID(mediaStreamFileNID);
        Element statusEntryEL = null;

        Document diskStatusDoc = MediaStoreServiceUtils.getDiskStatusDoc(storedirInfo.isInternal(), log);

        statusEntryEL = MediaStoreServiceUtils.getDiskEntry(storedirInfo.getStoreDeviceElementID(), diskStatusDoc);

        if (statusEntryEL != null) {
            diskStatusDoc = MRXMLUtils.elementToDocument(statusEntryEL);
        } else {
            diskStatusDoc = null;
        }

        if (diskStatusDoc == null) {
            throw new MRException(MediaStoreServiceExceptionCodes.GET_AVAILABLE_DISK_SPACE_FAILED, "CLI Command Result is Empty");
        }

        String status = MRXMLUtils.getValueXpath(diskStatusDoc, "//Parameter[@name='Status']", "");

        String availableBytes = MRXMLUtils.getValueXpath(diskStatusDoc, "//Parameter[@name='Available']", "");

        if (((status == null) || status.equals("False") || !status.equals("Ok")) && ((availableBytes == null) || (availableBytes.length() == 0))) {
            throw new MRException(MediaStoreServiceExceptionCodes.GET_AVAILABLE_DISK_SPACE_FAILED, "Command Status Falied ");
        }

        long availableDiskSpace = 0;
        availableDiskSpace = Utils.getLongValue(availableBytes, 0);
        long diskFullThreshold = storedirInfo.getDiskFullThreshold();
        if ((availableDiskSpace <= diskFullThreshold + blobSize * 2)) {
            throw new MRException(MediaStoreServiceExceptionCodes.NO_AVAILABLE_DISK_SPACE, "No Disk space available on device. (blobSize='" + blobSize + "' availableDiskSpace='" + availableDiskSpace + "' diskFullThreshold='" + diskFullThreshold + "')");
        }
        log.addInfo("ForExportImportFile: Available Disk Space on Device. (" + type + "='" + title + "' blobSize='" + blobSize + "' diskFullThreshold='" + diskFullThreshold + "' availableDiskSpace='" + availableDiskSpace + "')", MRLog.NOTIFY, MRLog.NOTIFY);
    }    
}
