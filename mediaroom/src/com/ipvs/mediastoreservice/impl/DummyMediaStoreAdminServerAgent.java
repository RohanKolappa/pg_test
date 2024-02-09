package com.ipvs.mediastoreservice.impl;

import java.util.Set;

import org.dom4j.Document;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.mediaroomservice.streamingserver.DummyStreamingServerImpl;
import com.ipvs.mediaroomservice.streamingserver.StreamingServerI;
import com.ipvs.utils.Utils;

public class DummyMediaStoreAdminServerAgent extends MediaStoreAdminServerAgent {
    public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID, Document setServiceDomainResponseDoc)
	    throws Exception {
    	
	    StreamingServerI myStreamingServer = new DummyStreamingServerImpl(userJID, log, db, xf, null);
	    
	    initInternal(xf, log, db, resourceFactory, userJID, setServiceDomainResponseDoc, myStreamingServer);
	
	
	    MediaStoreServerI mediaStoreServer = new DummyMediaStoreServerImpl(xf, log, db, resourceFactory, userJID);
	    storeServerList.put(MediaVersionManager.getServiceVersion(log), mediaStoreServer);
	
	    if ((this.mediaStorePortResourceNID != null) && !this.mediaStorePortResourceNID.isEmpty()) {
	        MediaStoreDirectoryInfo storeDirInfo = createMediaStoreDirectory("/data/DSS/data/portal/site/default/", MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME, MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME, this.mediaStorePortResourceNID, false,
	                Utils.getLongValue(MediaStoreServiceConst.LIMIT_ON_AVAILABLE_DISK_SPACE, 0), 0, true);
	
	        callSynchDirRequest(storeDirInfo, AMConst.AVAILABLE);
	        storeDirInfo.setMediaStoreState(MediaStoreServiceConst.READY);
	        MediaStoreServiceUtils.updateMediaStreamDirResourceState(storeDirInfo.getMediaStoreDirNID(), MediaStoreServiceConst.READY, MRRequest.createRequest(this.userJID), xf.xmpp(userJID), log, db, resourceFactory, xf.sf(userJID));
	    }
	
	    sendAgentStateEvent();
	}
    
    public void checkAvailableDiskSpace(String parentDirNID) throws MRException, Exception {
        log.addInfo("Check available disk space ", MRLog.NOTIFY, MRLog.NOTIFY);
    }

    public void checkAvailableDiskSpaceForExportImportFile(long blobSize, String type, String title,  String destFileParentDirNID) throws MRException, Exception {
        log.addInfo("Check available disk space For "+type+" '"+title+"' File with blobSize "+blobSize+". ", MRLog.NOTIFY, MRLog.NOTIFY);
    }
    
    protected void handleNASAvailable(String storeName) throws Exception {
        log.addInfo("Handle NAS Available storeName=" + storeName, MRLog.NOTIFY, MRLog.NOTIFY);    	
    }
    
    // get the media store Info for given media Stream dir NID  -- for device simulation
    public MediaStoreDirectoryInfo getMediaStoreInfoBasedOnDirNID(String mediaStreamDirNID)
        throws MRException, Exception {
        Set<String> storeSet = mediaStoreList.keySet();

        for (String storeName : storeSet) {
            if (storeName.equals(MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME)) {
                return mediaStoreList.get(storeName);
            }
        }

        return null;
    }

}
