package com.barco.devicevng.MediaStore;

import java.util.ArrayList;
import java.util.List;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Set;




public class MediaStoreDirectoryInfo {
    private String mediaStoreState = null;
    private String mediaStoreName = null;
    private int notSynchCount = 0;
    private String mediaStorePortResourceNID = null;
    private String mediaStoreRootPath = null;
    private String mediaStoreDeviceElementID = null;
    private boolean readOnly = false;
    private String mediaStoreDirNID = null;
    private String mediaStoreId = null;
    private String pvrDirNID = null;
    private String recycleBinNID = null;
    private long size = 0;
    private long deleterPort = 0;
    private long sizeUsed = 0;
    private boolean isInternal = true;
    private String mediaStoreServiceVer = "0";
    private boolean isWritable = true;
    private long diskFullThreshold = 0;
    private boolean diskQuotaExceeded = false;
    private String deviceServiceVersion = "";
    private Integer mediaStoreRevision = 0;
    private long reservedDiskSize = 0;
    private long diskQuotaExceedLimit;
    private DirPurgeFileProcess dirPurgeFileProcessInfo = null;
    private List<String> dirUUIDs=null;
   
    public List<String> getDirUUIDs() {
		return dirUUIDs;
	}

	public DirPurgeFileProcess getDirPurgePorcess() {
		return dirPurgeFileProcessInfo;
	}
    
    public boolean isDirPurgeFileProcessRunning() {
    	return (this.dirPurgeFileProcessInfo!=null && this.dirPurgeFileProcessInfo.getPurgeProcessState().equals("Running")? true:false);
    }

	public void createDirPurgeFileProcess(long deleterPort) {
		this.dirPurgeFileProcessInfo = new DirPurgeFileProcess();		
		this.dirPurgeFileProcessInfo.deleterPortID = ""+deleterPort;
		this.dirPurgeFileProcessInfo.setPurgeProcessState("NotRunning");

	}
	public void setDirUUIDs(List<String> dirUUIDs){
		this.dirUUIDs = dirUUIDs;
	}
	public void startDirPurgeFileProcess() {
		this.dirPurgeFileProcessInfo.setPurgeProcessState("Running");;
	}
	
	public void stopDirPurgeFileProcess() {
		this.dirPurgeFileProcessInfo.setPurgeProcessState("NotRunning");;
		this.dirPurgeFileProcessInfo.fileUUIDList.clear();
	}
	

	public MediaStoreDirectoryInfo(String mediaStoreName, String mediaStoreRootPath, String storeDeviceElementID, boolean readOnly, long size, long sizeUsed, boolean isInternal, long diskQuotaExceedLimit) {
        setMediaStoreName(mediaStoreName);
        setMediaStoreRootPath(mediaStoreRootPath);
        setMediaStoreDeviceElementID(storeDeviceElementID);
        setReadOnly(readOnly);
        setSize(size);
        setSizeUsed(sizeUsed);
        setInternal(isInternal);
        setDiskQuotaExceedLimit(diskQuotaExceedLimit);
    }

    public boolean isWritable() {
        return isWritable;
    }

    public void setWritable(boolean isWritable) {
        this.isWritable = isWritable;
    }

    public String getMediaStoreServiceVer() {
        return mediaStoreServiceVer;
    }

    public void setMediaStoreServiceVer(String dirServiceVer) {
        this.mediaStoreServiceVer = dirServiceVer;
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

    public String getMediaStoreUUID() {
        return mediaStoreId;
    }

    public void setMediaStoreUUID(String dirUUID) {
        this.mediaStoreId = dirUUID;
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

    public String getMediaStoreDeviceElementID() {
        return mediaStoreDeviceElementID;
    }

    public void setMediaStoreDeviceElementID(String storeDeviceElementID) {
        this.mediaStoreDeviceElementID = storeDeviceElementID;
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
        //MRLog.getInstance(MRLog.DAEMON).addInfo("Set the Media Store Name := " + storeName + " Status as " + mediaStoreState, MRLog.INFO, MRLog.NOTIFY);
    }

    public String getMediaStoreName() {
        return mediaStoreName;
    }

    public void setMediaStoreName(String storeName) {
        this.mediaStoreName = storeName;
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

	public String getDeviceServiceVersion() {
		return deviceServiceVersion;
	}

	public void setDeviceServiceVersion(String deviceServiceVersion) {
		this.deviceServiceVersion = deviceServiceVersion;
	}

	public Integer getDirRevision() {
		return mediaStoreRevision;
	}

	public void setDirRevision(Integer dirRevision) {
		this.mediaStoreRevision = dirRevision;
	}
	
	public long getReservedDiskSize() {
		return reservedDiskSize;
	}
	
	public void setReservedDiskSize(long reservedDiskSize) {
		this.reservedDiskSize = reservedDiskSize;
	}

	public long getDiskQuotaExceedLimit() {
		return diskQuotaExceedLimit;
	}

	public void setDiskQuotaExceedLimit(long diskQuotaExceedLimit) {
		this.diskQuotaExceedLimit = diskQuotaExceedLimit;
	}
	
	class DirPurgeFileProcess {
		private String deleterPortID = "";
		
		private HashMap<String,String> fileUUIDList = new HashMap<String,String>();
		static final String PURGEFILE_DIR_PATH="/purge";
		private String state = "";
		
		public String getDeleterPortID() {
			return deleterPortID;
		}
		public void setDeleterPortID(String deleterPortID) {
			this.deleterPortID = deleterPortID;
		}
		
		public void addFile (String fileUUID,String fileNID) {
			this.fileUUIDList.put(fileUUID,fileNID);
		}
		
		public String removeFile (String fileUUID) {
			return this.fileUUIDList.remove(fileUUID);
		}
		
		public String getPugeFileDir() {
			return PURGEFILE_DIR_PATH;
		}
		public void setPurgeProcessState(String state) {
			this.state = state;
		}
		
		public String getPurgeProcessState(){
			return this.state;
		}
	}
	
}
