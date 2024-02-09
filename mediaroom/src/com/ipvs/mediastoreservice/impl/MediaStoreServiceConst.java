package com.ipvs.mediastoreservice.impl;

import java.util.HashMap;

import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRRequest;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediastoreservice.handler.RepairMediaRequestHandler;
import com.ipvs.mediastoreservice.handler.UpgradeMediaRequestHandler;
import com.ipvs.mediastoreservice.handler.UpgradeMediaStoreDirRequestHandler;


public class MediaStoreServiceConst {
	
	public enum MEDIA_TYPE_ENUMS {
		// Supported media types for export/import
		MediaClip, 
		MediaImage, 
		MediaGroup,
		PVRMediaClip;
		
		public static boolean contains(String value) {
			for (MEDIA_TYPE_ENUMS val : MEDIA_TYPE_ENUMS.values()) {
				if (val.toString().equals(value))
					return true;
			}
			return false;
		}
	 }; 
	 
    //Media Store requests Constants
    public static final String ADD_MEDIA_STORE_FILE_REQUEST = "AddMediaStoreFileRequest";
    public static final String DELETE_MEDIA_STORE_FILE_REQUEST = "DeleteMediaStoreFileRequest";
    public static final String UPDATE_MEDIA_STORE_FILE_REQUEST = "UpdateMediaStoreFileRequest";
    public static final String GET_MEDIA_STORE_FILE_TRACK_REQUEST = "GetMediaStoreFileTrackRequest";
    public static final String SET_MEDIA_STORE_FILE_REQUEST = "SetFileRequest";
    public static final String DELETE_FILE_REQUEST = "DeleteFileRequest";
    public static final String UPDATE_MEDIA_PLAYLENGTH_REQUEST = "UpdateMediaPlayLengthRequest";
    public static final String UPDATE_MEDIA_STATUS_REQUEST = "UpdateMediaStatusRequest";
    public static final String UPDATE_MEDIA_PLAYLENGTH_RESPONSE = "UpdateMediaPlayLengthResponse";
    public static final String UPDATE_MEDIA_STATUS_RESPONSE = "UpdateMediaStatusResponse";
    public static final String UPDATE_MEDIA_PROFILE_REQUEST = "UpdateMediaProfileRequest";
    public static final String UPDATE_MEDIA_METADATA_REQUEST = "UpdateMediaMetaDataRequest";
    public static final String UPDATE_FILE_AUTOSTOARGE_RULES_REQUEST = "UpdateMediaAutoStorageRulesRequest";
    public static final String REPAIR_MEDIA_REQUEST = MRRequest.getRequestName(RepairMediaRequestHandler.class.getName());
    public static final String UPGRADE_MEDIA_REQUEST = MRRequest.getRequestName(UpgradeMediaRequestHandler.class.getName());
    public static final String UPGRADE_MEDIA_STORE_DIR_REQUEST  = MRRequest.getRequestName(UpgradeMediaStoreDirRequestHandler.class.getName());
    public static final String SET_FILE_REQUEST = "SetFileRequest";
    public static final String MEDIA_FILE_STORE_NID  = "MediaStoreFileNID";


    //Media Store files  and folder names
    public static final String MEDIA_STORE_DB_XML_FILE_NAME = "db.xml";
    public static final String MEDIA_STORE_INFO_FILE_NAME = "MediaStoreInfo.xml";
    public static final String MEDIA_STORE_BOOKMARKS_FOLDER_NAME = "bookmarks";
    public static final String MEDIA_STORE_BLOB_DIR = "blob";
    public static final String MEDIA_STORE_DB_DIR = "db";
    public static final String MEDIA_STORE_INDEX_FILE_NAME = "INDEX";

    //String constants used in media store service     
    public static final String MEDIA_STORE_FILE_NID = "MediaStoreFileNID";
    public static final String MEDIA_STREAM_FILE_RESOURCE_NID = "MediaStreamFileResourceNID";
    public static final String MEDIA_STREAM_DIR_RESOURCE_NID = "MediaStreamDirResourceNID";
    public static final String ACTION_DATA = "ActionData";
    public static final String LOG_ELEMENT = "log";
    public static final String REVISION = "Rev";
    public static final String REVISION_ELEMENT_NAME = "Revision";
    public static final String MEDIA_STORE_FILE_REVISION = "MediaStoreFile-Rev";
    public static final String NEXT_REVISION = "NextRevision";
    public static final String NID = "NID";
    public static final String PARENT_NID = "parentNID";
    public static final int DEFAULT_REVISION = 1;
    public static final int DEFAULT_STORE_FILE_REVISION = 0;
    public static final String MEDIA_STORE_FILE_INFO_ELEMENT = "MediaStoreFileInfo";
    public static final String MEDIA_STREAM_FILE_STATE_ELEMENT = "StateFlag";
    public static final String MEDIA_STORE_FILE_STATE_ELEMENT = "State";
    public static final String META_DATA_ELEMENT_NAME = "MetaData";
    public static final String TAG_ELEMENT_NAME = "Tag";
    public static final String NAME_ATTRIBUTE = "Name";
    public static final String VALUE_ATTRIBUTE = "Value";  
    public static final String MEDIA_STREAM_PARENT_DIR_ELEMENT_NAME = "ParentMediaDirNID";
    public static final String MEDIA_TYPE_ELEMENT_NAME = "Type";
    public static final String MEDIA_TITLE_ELEMENT_NAME = "Title";
    public static final String MEDIA_DESCRIPTION_ELEMENT_NAME = "Description";
    public static final String MEDIA_STREAM_TYPE_ELEMENT_NAME = "StreamType";
    public static final String INFO_ELEMENT_NAME = "Info";
    public static final String READ = "Read";
    public static final String WRITE = "Write";
    public static final String XFER = "Xfer";
    public static final String STREAM = "Stream";
    public static final String UUID = "UUID";
    public static final String TRUE = "true";
    public static final String FALSE = "false";
    public static final String META_DATA_TAG_ELEMENT_NAME = "Tag";
    public static final String META_DATA_TAG_ATT_NAME = "Name";
    public static final String ROOM_NID = "RoomNID";
    public static final String STREAM_NID = "StreamNID";
    public static final String ROOM_CONFIG_VERSION = "RoomConfigVersion";
    public static final String PLAY_LENGTH = "PlayLength";
    public static final String MEDIA_DIR_SIZE = "Size";
    public static final String MEDIA_DIR_SIZE_USED = "SizeUsed";
    public static final String MEDIA_DIR_DSIK_FULL_THRESHOLD = "diskFullThreshold";
    public static final String PVR_TRIM_LENGTH_EL_NAME = "TrimLength";
    public static final String DATE_CREATED = "DateCreated";
    public static final String DATE_MODIFIED = "DateModified";
    public static final String MEDIA_GROUP_ID_EL_NAME = "MediaGroupID";
    public static final String MEDIA_STREAM_FILE_NID = "MediaStreamFileNID";
    public static final String MEDIA_GROUP_SYNC_EL_NAME = "Sync";
    public static final String FILLER_INFO = "FillerInfo";
    public static final String FILLER_MEDIA_STREAM_FILE_NID = "FillerMediaStreamFileNID";
    public static final String FILLER_MEDIA_START_OFFSET = "FillerMediaStartOffset";
    public static final String FILLER_MEDIA_PLAYLENGTH = "FillerMediaPlayLength";
    public static final String PLAYLIST_ENTIRES = "PlaylistEntries";
    public static final String PLAYLIST_ENTRY = "PlaylistEntry";
    public static final String ID = "ID";
    public static final String PLAY_ORDER = "PlayOrder";
    public static final String LOOP_COUNT = "LoopCount";
    public static final String PLAY_LIST_ENTRY_START_OFFSET = "PlaylistEntryStartOffset";
    public static final String PLAY_LIST_ENTRY_PLAY_LENGTH = "PlaylistEntryPlayLength";
    public static final String MEDIA_START_OFFSET = "MediaStartOffset";
    public static final String MEDIA_PLAYLENGTH = "MediaPlayLength";
    public static final String BOOKMARK_NID = "BookmarkNID";
    public static final String BOOKMARK_LIST = "BookmarkList";
    public static final String MEDIA_FILE_DOC = "MediaFileDoc";
    public static final String WHERE_XPATH = "WhereXPath";
    public static final String GROUPS_EL_NAME = "Groups";
    public static final String CLIP_TYPE_EL_NAME = "ClipType";
    public static final String MEDIA_GROUP_FILE_RESOURCE_NID_EL_NAME = "MediaGroupFileResourceNID";
    public static final String FILE_TITLE_EL_NAME = "FileTitle";
    public static final String FILE_DESCRIPTION_EL_NAME = "FileDescription";
    public static final String OWNER_EL_NAME = "Owner";
    public static final String PERMISSION_EL_NAME = "Permissions";
    public static final String PROPERTIES_EL_NAME = "Properties";
    public static final String STREAM_TYPE = "StreamType";
    public static final String CLIP_ID = "ClipID";
    public static final String REC_INFO = "RecInfo";
    public static final String SESSION_ID = "SessionID";
    public static final String SESSION_CONFIG_NID = "SessionConfigNID";
    public static final String MEDIA_DIR_PATH = "MediaDirectory";
	public static final String BYTES_WRITTEN = "BytesWritten";
	public static final String BYTES_WRITTEN_DELTA = "BytesWrittenDelta";
	public static final String BYTES_DELETED_DELTA = "BytesDeletedDelta";
	public static final String BYPASS_AUTO_STORAGE_RULES = "bypassAutoStorageRules";
	public static final String BYPASS_AUTO_STORAGE_RULES_ELEMENT = "BypassAutoStorageRules";
	public static final String BY_ID_ATTRIBUTE_NAME = "byID";
	public static final String SET_BYPASS_AUTO_STORAGE_RULES_ELEMENT = "SetFileBypassAutoStorageRules";
	public static final String SET_FILE_INFO_ELEMENT = "SetFileInfo";

    //Store agents 
    public static final String MEDIA_STORE_ADMIN_SERVER_AGENT = "com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent";

    //Request states
    public static final int SUCCESS_STATE = MRRequest.STATE_DONE;
    public static final int FAILURE_STATE = MRLog.ERROR;

    //MediaRoom Request to Media Store Request Mapping.
    public static final HashMap<String, String> mediaroomRequestToMediaStoreRequestMapping = new HashMap<String, String>();

    //Media Store States
    public static final String READY = "Ready";
    public static final String NOT_READY = "NotReady";
    public static final String ERROR = "Error";
    public static final String UPGRADE_ERROR = "UpgradeError";
    public static final String BUSY = "Busy";
    public static final String START_SYNCH = "StartSynch";
    public static final String CREATED = "Created";
    public static final String DETACHED = "Detached";
    public static final String PREPARED = "Prepared";
    
    //Media store Event constants
    public static final String MEDIA_STORE_EVENT = "MediaStoreEvent";

    //Recording status 
    public static final String RECORDING_STARTED = "Recording";
    public static final String RECORDING_STOPPED = "Stopped";
    public static final String RECORDING_PAUSED = "Paused";

    //Media Store Admin Actions
    public static final String CREATE_FILE = "CreateFile";
    public static final String DELETE_FILE = "DeleteFile";
    public static final String UPDATE_FILE = "UpdateFile";
    public static final String UPDATE_START_TC = "UpdateStartTC";
    public static final String UPDATE_END_TC = "UpdateEndTC";
    public static final String UPDATE_FILE_STATUS = "UpdateFileStatus";

    //Trash dir 
    public static final String TRASH_DIR_PATH = "/data/DSS/data/portal/site/default/trash";

    //Media Track Index Constants
    public static final String MEDIA_STORE_FILE_TRACK = "MediaStoreFileTrack";
    public static final String MEDIA_TRACK = "MediaTrack";
    public static final String MEDIA_TRACK_ITEM = "MediaTrackItem";

  

    //default dir name
    public static final String DEFAULT_DIR = "NONE";
    public static final String GET_DISK_SPACE_CLI_CMD = "get_disk_status";
    public static final String GET_NAS_STATUS_CLI_CMD = "get_nas_status";

    //NAS Related Code Constants
    public static final String NAS_STATUS_OK = "Ok";

    //Dir Synch constants    
    public static final String DIR_NAME_EL_NAME = "DirName";
    public static final String DIR_UUID_FILE_NAME = "UUID.xml";
    public static final String PARENT_DIR_NID_EL_NAME = "ParentMediaDirNID";
    public static final String DEVICE_ELEMENT_ID_ATTRIBUTE_NAME = "deviceElementId";
    public static final String READ_ONLY_ATTRIBUTE_NAME = "readOnly";
    public static final String PRESENCE_ATTRIBUTE_NAME = "presence";
    public static final String PRIORITY_ATTRIBUTE_NAME = "priority";
    public static final String MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME = "MediaStorePortResourceNID";
    public static final String MEDIA_RELAY_PORT_RESOURCE_NID_EL_NAME = "MediaStreamRelayPortResourceNID";
    public static final String MEDIA_STORE_PORT_RESOURCE_NID_LIST_EL_NAME = "MediaStorePortResourceNIDList";
    public static final String SYNCH_MEDIA_STORE_PORT_RESOURCE_NID_EL_NAME = "synchMediaStorePortResourceNID";
    public static final String RECYCLE_BIN_DIR_NAME = "RECYCLEBIN";
    public static final String PVR_DIR_NAME = "PVR";
    public static final String LIMIT_ON_AVAILABLE_DISK_SPACE = "10737418240";    
    public static final String DISK_QUOTA_EXCEED_LIMIT = "5120000000";
    //public static final String DISK_QUOTA_EXCEED_LIMIT = "47322775961"; //testing
    public static final String DEFAULT_MEDIA_STORE_NAME = "default";
    public static final String STATE_EL_NAME = "State";
    public static final String MOUNT = "Mount";
    public static final String UNMOUNT = "Unmount";
    public static final String ONLINE = "Online";
    public static final String OFFLINE = "Offline";
    public static final String DELETE = "Delete";
    public static final int DEFAULT_PRIORITY = 1;
    public static final String STATE_UPDATE = "StateUpdate";

    //service version constants
    public static final String SERVICE_VERSION = "serviceVer";

    public static void init() {
        mediaroomRequestToMediaStoreRequestMapping.put(MediaRoomServiceConst.ADD_DEST_FILE_REQUEST, ADD_MEDIA_STORE_FILE_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(MediaRoomServiceConst.DELETE_DEST_FILE_REQUEST, DELETE_MEDIA_STORE_FILE_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(MediaRoomServiceConst.UPDATE_DEST_FILE_METADATA_REQUEST, UPDATE_MEDIA_METADATA_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(MediaRoomServiceConst.UPDATE_DEST_FILE_PROFILE_REQUEST, UPDATE_MEDIA_PROFILE_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(MediaRoomServiceConst.GET_SRC_MEDIA_FILE_TRACK_REQUEST, GET_MEDIA_STORE_FILE_TRACK_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(UPDATE_MEDIA_PLAYLENGTH_REQUEST, UPDATE_MEDIA_PLAYLENGTH_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(UPDATE_MEDIA_STATUS_REQUEST, UPDATE_MEDIA_STATUS_REQUEST);

        //Asset Management Requests
        mediaroomRequestToMediaStoreRequestMapping.put(AMConst.ASSET_ADD_REQUEST, ADD_MEDIA_STORE_FILE_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(AMConst.ASSET_REPLACE_REQUEST, UPDATE_MEDIA_STORE_FILE_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(AMConst.ASSET_DELETE_REQUEST, DELETE_MEDIA_STORE_FILE_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(AMConst.FILE_DELETE_REQUEST, DELETE_FILE_REQUEST);
        mediaroomRequestToMediaStoreRequestMapping.put(AMConst.FILE_REPLACE_REQUEST, SET_MEDIA_STORE_FILE_REQUEST);
        
        //Repair Request
        mediaroomRequestToMediaStoreRequestMapping.put(AMConst.REPAIR_MEDIA_STREAM_FILE_REQUEST, REPAIR_MEDIA_REQUEST);

        //Upgrade MediaStreamFile Request
        mediaroomRequestToMediaStoreRequestMapping.put(AMConst.UPGRADE_MEDIA_STREAM_FILE_REQUEST, UPGRADE_MEDIA_REQUEST);
        
        //Upgrade MediaStreamDir Request
        mediaroomRequestToMediaStoreRequestMapping.put(AMConst.UPGRADE_MEDIA_STREAM_DIR_REQUEST, UPGRADE_MEDIA_STORE_DIR_REQUEST);
       
    }
	public static String GET_FILE_LIST_XQUERY = "/com/ipvs/mediastoreservice/handler/xquery/GetFileListQuery.xml";
	public static String GET_MAX_REVISION_XQUERY = "/com/ipvs/mediastoreservice/handler/xquery/GetMaxRevisionQuery.xml";
	public static String XPATH = "_XPATH_";
	
	public static String DELETED = "Deleted";
	public static String DELETING = "Deleting";
	
	public static String MEDIA_IMAGE_TYPE = "MediaImage";
	public static String IMAGE_TYPE_ELEMENT = "ImageType";
	public static String SNAPSHOT_FILE_NAME = "snapshot_0";
	public static String SS_INDEX_FILE_NAME = "SSINDEX";
    public static final String MEDIA_DIR_SPANNING = "MediaDirSpanning";
    public static final String MEDIA_DIR_STORAGE_CONFIG = "MediaDirStorageConfig";
    public static final String THRESHOLD_RULE = "ThresholdRule";
    public static final String ILM_RULE = "ILMRule";
    public static final String CAPACITY_MANAGEMENT = "CapacityManagement";
    public static final String ENABLE_ATTRIBUTE = "enable";
    public static final String OFFLOAD_RULE = "OFFLOAD";
    public static final String DELETE_RULE = "DELETE";
    public static final String DISK_CAPACITY_REACHED_ATTRIBUTE = "diskCapacityReached";
    public static final String REDUCE_DISK_CAPACITY_ATTRIBUTE = "reduceDiskCapacity";
    public static final String OLDER_THAN_ATTRIBUTE = "olderThan";
    public static final String LAST_RUN_DATE="lastRunDate";
    public static final String LAST_RUN_TIME="lastRunTime";
    public static final String LAST_RUN_STATE="lastRunState";
    public static final String PURGE_FILES_ATTRIBUTE="purgeFiles";
    public static final String NAS_DRIVE_URL_ATTRIBUTE="nasDriveURL";
    public static final String NAS_URL_ATTRIBUTE="nasURL";
    public static final String FORMAT_ATTRIBUTE="format";
}

