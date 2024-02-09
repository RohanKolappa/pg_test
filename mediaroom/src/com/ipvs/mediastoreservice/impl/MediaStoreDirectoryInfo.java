package com.ipvs.mediastoreservice.impl;

import com.ipvs.common.MRLog;


public class MediaStoreDirectoryInfo {
    private String mediaStoreState = null;
    private String storeName = null;
    private int notSynchCount = 0;
    private String mediaStorePortResourceNID = null;
    private String mediaStoreRootPath = null;
    private String storeDeviceElementID = null;
    private boolean readOnly = false;
    private String mediaStoreDirNID = null;
    private String dirUUID = null;
    private String pvrDirNID = null;
    private String recycleBinNID = null;
    private long size = 0;
    private long deleterPort = 0;
    private long sizeUsed = 0;
    private boolean isInternal = true;
    private String dirServiceVer = "0";
    private boolean isWritable = true;
    private long diskFullThreshold = 0;
    private boolean diskQuotaExceeded = false;
 
    public MediaStoreDirectoryInfo(String mediaStoreName, String mediaStoreRootPath, String storeDeviceElementID, String mediaStorePortResourceNID, 
    		boolean readOnly, long size, long sizeUsed, boolean isInternal) {
        setStoreName(mediaStoreName);
        setMediaStoreRootPath(mediaStoreRootPath);
        setStoreDeviceElementID(storeDeviceElementID);
        setMediaStorePortResourceNID(mediaStorePortResourceNID);
        setReadOnly(readOnly);
        setSize(size);
        setSizeUsed(sizeUsed);
        setInternal(isInternal);
    }

    public boolean isWritable() {
        return isWritable;
    }

    public void setWritable(boolean isWritable) {
        this.isWritable = isWritable;
    }

    public String getDirServiceVer() {
        return dirServiceVer;
    }

    public void setDirServiceVer(String dirServiceVer) {
        this.dirServiceVer = dirServiceVer;
    }

    public boolean isInternal() {
        return isInternal;
    }

    public void setInternal(boolean isInternal) {
        this.isInternal = isInternal;
    }

    public long getDeleterPort() {
        return deleterPort;
    }

    public void setDeleterPort(long deleterPort) {
        this.deleterPort = deleterPort;
    }

    public long getSize() {
        return size;
    }

    public void setSize(long size) {
        this.size = size;
    }

    public long getSizeUsed() {
        return sizeUsed;
    }

    public void setSizeUsed(long usedSize) {
        this.sizeUsed = usedSize;
    }

    public String getRecycleBinNID() {
        return recycleBinNID;
    }

    public void setRecycleBinNID(String recycleBinNID) {
        this.recycleBinNID = recycleBinNID;
    }

    public String getPvrDirNID() {
        return pvrDirNID;
    }

    public void setPvrDirNID(String pvrDirNID) {
        this.pvrDirNID = pvrDirNID;
    }

    public String getDirUUID() {
        return dirUUID;
    }

    public void setDirUUID(String dirUUID) {
        this.dirUUID = dirUUID;
    }

    public String getMediaStoreDirNID() {
        return mediaStoreDirNID;
    }

    public void setMediaStoreDirNID(String mediaStoreDirNID) {
        this.mediaStoreDirNID = mediaStoreDirNID;
    }

    public boolean isReadOnly() {
        return readOnly;
    }

    public void setReadOnly(boolean readOnly) {
        this.readOnly = readOnly;
    }

    public String getStoreDeviceElementID() {
        return storeDeviceElementID;
    }

    public void setStoreDeviceElementID(String storeDeviceElementID) {
        this.storeDeviceElementID = storeDeviceElementID;
    }

    public String getMediaStoreRootPath() {
        return mediaStoreRootPath;
    }

    public void setMediaStoreRootPath(String mediaStoreRootPath) {
        this.mediaStoreRootPath = mediaStoreRootPath;
    }

    public synchronized String getMediaStoreState() {
        return mediaStoreState;
    }

    public synchronized void setMediaStoreState(String mediaStoreState) {
        this.mediaStoreState = mediaStoreState;
        MRLog.getInstance(MRLog.DAEMON).addInfo("Set the Media Store Name := " + storeName + " Status as " + mediaStoreState, MRLog.INFO, MRLog.NOTIFY);
    }

    public String getStoreName() {
        return storeName;
    }

    public void setStoreName(String storeName) {
        this.storeName = storeName;
    }

    public int getNotSynchCount() {
        return notSynchCount;
    }

    public void setNotSynchCount(int notSynchCount) {
        this.notSynchCount = notSynchCount;
    }

    public String getMediaStorePortResourceNID() {
        return mediaStorePortResourceNID;
    }

    public void setMediaStorePortResourceNID(String mediaStorePortResourceNID) {
        this.mediaStorePortResourceNID = mediaStorePortResourceNID;
    }

    public void incrementNotSynchCount() {
        notSynchCount++;
    }

    public void decrementNotSynchCount() {
        notSynchCount--;
    }

    public long getDiskFullThreshold() {
        return diskFullThreshold;
    }

    public void setDiskFullThreshold(long diskFullThreshold) {
        this.diskFullThreshold = diskFullThreshold;
    }

	/**
	 * @return the diskQuotaExceeded
	 */
	public boolean isDiskQuotaExceeded() {
		return diskQuotaExceeded;
	}

	/**
	 * @param diskQuotaExceeded the diskQuotaExceeded to set
	 */
	public void setDiskQuotaExceeded(boolean diskQuotaExceeded) {
		this.diskQuotaExceeded = diskQuotaExceeded;
	}
}
