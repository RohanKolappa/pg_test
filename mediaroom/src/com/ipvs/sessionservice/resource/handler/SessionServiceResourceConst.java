package com.ipvs.sessionservice.resource.handler;


public class SessionServiceResourceConst {
	
	public enum FILE_WHERETYPE_ENUMS {
		// Supported media types for file query
		MediaClip, 
		MediaImage, 
		MediaGroup;
		
		public static boolean contains(String value) {
			for (FILE_WHERETYPE_ENUMS val : FILE_WHERETYPE_ENUMS.values()) {
				if (val.toString().equals(value)) {
                    return true;
                }
			}
			return false;
		}
	};
	
	 public enum OBJECT_TYPE_ENUMS  {
		 Device, Users, MediaRoom, Profiles, Ports, Media, ALL;
		 
		 public static boolean contains(String value) {
				for (OBJECT_TYPE_ENUMS val : OBJECT_TYPE_ENUMS.values()) {
					if (val.toString().equals(value)) {
                        return true;
                    }
				}
				return false;
			}
	 };
	 
     //Device constants
     public static final String PASSWORD_ATTRIBUTE = "password";
     public static final String UNMANAGED_ATTRIBUTE = "unManaged";
     public static final String UNMANNED_ATTRIBUTE = "unManned";
     public static final String DEVICE_PRIVATE_DATA_ELEMENT = "DevicePrivateData";
     public static final String GROUPS_ELEMENT = "Groups";
     public static final String BOTUSER_ATTRIBUTE = "botUser";
     public static final String ASSET_TEMPLATE_ATTRIBUTE = "assetTemplate";
     public static final String FEATURE_ELEMENT = "Feature";
     public static final String FEATURE_NAME_ATTRIBUTE = "Name";
     public static final String FEATURE_MODE = "Mode";
     public static final String FEATURE_UNMANNED = "UnManned";
	 public static final String TITLE_POST_FIX_ATTRIBUTE = "titlePostFix";	   
     public static final String INSTANCE_INFO = "InstanceInfo";
     public static final String MAX_INSTANCE_COUNT_ELEMEMNT = "MaxInstanceCount";     
     public static final String MAX_INSTANCE_COUNT_ATTRIBUTE = "maxInstanceCount";
     public static final String DEVICE_ELEMENT_ID_ATTRIBUTE = "deviceElementId";
     public static final String DEVICE_ELEMENT_ID_ELEMENT = "DeviceElementID";
     public static final String STREAM_TYPE_LIST_ELEMENT = "StreamTypeList";
     public static final String STREAM_TYPE_ELEMENT = "StreamType";
     public static final String FEATURE_LIST_ELEMENT = "FeatureList";
     public static final String UNMANNED_FEATURE_LIST_ELEMENT = "UnMannedFeatureList";
     public static final String UNMANAGED_FEATURE_LIST_ELEMENT = "UnManangedFeatureList";
     public static final String ASSET_RESOURCE_NID_ELEMENT = "AssetResourceNID";
     public static final String MEDIA_STREAM_LIST = "MediaStreamList";
     public static final String MEDIA_STREAM_PROFILE_INFO = "MediaStreamProfileInfo";
     public static final String NID_VALUE = "NIDValue";
     public static final String DEVICE_NAME = "DeviceName";
     public static final String DEVICE_PASSWORD = "DevicePassword";
     public static final String ADD_DEVICE_ATTRIBUTE = "addDevice";
     public static final String INFO_ELEMENT = "Info";
     public static final String CONFIGURABLE_ATTRIBUTE = "configurable";
     public static final String STREAMING_SERVICE_STATUS="StreamingServiceStatus";
     public static final String INSTANCE_LIST="InstanceList";
     public static final String BOT_USER_ELEMENT = "BotUser";
     public static final String ENABLE_ATTRIBUTE = "enable";
     public static final String KEY_DATA_ELEMENT = "KeyData";
     public static final String ASSET_PRIVATE_DATA_ELEMENT ="AssetPrivateData";
     public static final String APPLY_TAGS_TO_PORTS_ATTRIBUTE ="applyTagsToPorts";      
     public static final String TITLE = "Title";     
    
     public static final String BY_NID_ATTRIBUTE = "byNID";
     public static final String BY_JID_ATTRIBUTE = "byJID";
     public static final String BY_TITLE_ATTRIBUTE = "byTitle";
     public static final String DEVICE_CONFIG_ELEMENT = "DeviceConfig";
     public static final String RESOURCE_POINTERS_ELEMENT = "ResourcePointers";
     public static final String UPDATE_FEATURE_LIST_ATTRIBUTE = "updateFeatureList";
     public static final String UPDATE_FEATURE_LIST_IF_UNMANNED_ATTRIBUTE = "updateFeatureListIfUnmanned";
     public static final String UPDATE_FEATURE_LIST_IF_UNMANAGED_ATTRIBUTE = "updateFeatureListIfUnmanaged";
     public static final String TEMPLATE_ASSET_RESOURCE_NID = "TemplateAssetResourceNID";
     public static final String TEMPLATE = "TEMPLATE";
     public static final String ENABLE_UNMANNED_MODE = "enableUnmannedMode";
     public static final String DISPLAY_CONFIG_NODE = "displayConfigNode";
     public static final String DISPLAY_CONFIG_STATUS_NODE = "displayStatusNode";
     public static final String ASSET_RESOURCE_NID_ATTRIBUTE = "assetResourceNID";
     public static final String NOT_AVAILABLE="NOT_AVAILABLE";
     
     public static final String OBSERVER_ELEMENT = "Observer";
     public static final String OBSERVER_INFO_ELEMENT = "ObserverInfo";
     public static final String STREAM_URL_ATTRIBUTE = "streamURL";
     public static final String MEDIA_STREAM_URL_ELEMENT = "MediaStreamURL";
     
     public static final String PVR_INFO_ELEMENT = "PVRInfo";
     public static final String MEDIA_STREAM_DIR_RESOURCE_NID_ATTRIBUTE = "mediaStreamDirResourceNID";
          
     public static final String BY_USER_NAME_ATTRIBUTE = "byUsername";
     public static final String BY_NAME_ATTRIBUTE = "byName";
     public static final String BY_TITLE_CONST = "BY_TITLE_CONST";
     
     public static final String DESCRIPTION_XPATH = "//Info/Description";
     public static final String PASSWORD_XPATH = "//AssetPrivateKey/KeyData";
     public static final String DO_NOT_SAVE_TO_FLASH_ATTRIBUTE = "doNotSaveToFlash";
     public static final String JID_ATTRIBUTE="jid";
     
     public static final String TRICK_PLAY_FEATURE_NAME = "TrickPlay";
     
     public static final String API_KEY_TABLE = "ApiKeyTable";
     public static final String API_KEY_ELEMENT_NAME= "ApiKey";
     public static final String KEY_ATTRIBUTE= "key";
     public static final String SHARED_SECRET_ATTRIBUTE= "sharedSecret";
     public static final String VALID_INTERVAL_ATTRIBUTE= "validInterval";
     public static final long API_KEY_VALID_INTERVAL= 7200;//in seconds
     public static final long REDIRECT_EXPIRE_TIME = 300;//in seconds
     public static final String EXPIRE_TIME = "expireTime";
     public static final String DIRID_CONST = "_DIRID_CONST";
     public static final String TIMESTAMP_CONST = "_TIME_STAMP_CONST";
     public static final String API_KEY_CONST = "_API_KEY_CONST";
     public static final String SIGNATURE_CONST = "_SIGNATURE_CONST";
     public static final String ACCESS_PATH_CONST = "_ACCESS_PATH_CONST";
     public static final String SNAPSHOTURL_ATTRIBUTE = "snapshotURL";
     public static final String UPLOADURL_ATTRIBUTE = "uploadURL";
     public static final String MEDIAURL_ATTRIBUTE = "mediaURL";
     public static final String THUMBNAILURL_ATTRIBUTE = "thumbnailURL";
     public static final String DOWNLOADURL_ATTRIBUTE ="downloadURL";
     public static final String SELECTFORMAT_SNAPSHOTURL = "SNAPSHOTURL";
     public static final String SELECTFORMAT_UPLOADURL = "UPLOADURL";
     public static final String SELECTFORMAT_BASICURL = "BASICURL";
     public static final String SELECTFORMAT_EXTENDEDURL = "EXTENDEDURL";
     public static final String SELECTFORMAT_EXTENDED = "EXTENDED";
     public static final String MS_IPADDRESS = "ipAddress";
     public static final String VIEWSNAPSHOTURL = "ViewSnapshotURL";
     public static final String RECORDINGTHUMBNAILPARAMETERS = "ThumbnailDMSAgentAndDirID";
     public static final String IMPORTURL = "ImportURL";
     public static final String EXPORTFILEURL ="ExportFileURL";
     public static final String _FILENID_ = "_FILENID_";
     public static final String _GROUPNID_ = "_GROUPNID_";
     public static final String _QUEUEID_ = "_QUEUEID_";
     public static final String _DSTNID_ = "_DSTNID_";
     public static final String URL_ATTRIBUTE = "url";
     
     //exportURL persistence constants
     public static final String PERSISTENCE_PERMANENT="permanent";
     public static final String PERSISTENCE_TEMPORARY="temporary";
     
     //export & rest request via NodeJS
     public static final String EXPORT_TYPE="export";
     
     //offload constants
     public static final String PENDING_ATTRIBUTE="pending";
     public static final String ERROR_ATTRIBUTE="error";
     
     //dir storage config constants
     public static final String WHERE_MODE = "whereMode"; 
     public static final String DIR_SPANNING = "DirSpanning";
     public static final String CAPACITY_MANAGEMENT = "CapacityManagement";
     public static final String MODE_ATTRIBUTE = "mode"; 
     public static final String DISK_FULL_THRESHOLD = "diskFullThreshold";
     public static final String PORT_ELEMENT = "Port";
     public static final String PORT_NID = "portNID";
     public static final String SCHEDULE_ATTRIBUTE_NAME = "schedule";
     public static final String _DIRNID_ = "_DIRNID_";
     public static final String _DISKCAPACITYREACHED_ = "_DISKCAPACITYREACHED_";
     public static final String _SKIPERRORS_ = "_SKIPERRORS_";
     public static final String _DATETIMESTAMP_ = "_DATETIMESTAMP_";
     
     public static final String DISCOVERY_TEMPLATE = "discoverytemplate";
     public static final String MULTICAST_IP_ADDRESS_POOL = "MulticastIPAddressPool";
     
     public static final String ADD_DEVICE_REQUEST = "AddDeviceRequest";
     
     public static final String BY_DIR_NID = "byDirNID";
     public static final String BY_UUID = "byUUID";
     public static final String BY_DIR_ID_ATTRIBUTE = "byDirID";
     
     public static final String REMOTELY_OPERATED_MODE = "RemotelyOperatedMode"; 
     public static final String REMOTELY_OPERATED_MODE_ATTR = "remotelyOperatedMode"; 
     
     public static final String AUDIO_ENCODING = "audioEncoding";
     public static final String VIDEO_ENCODING = "videoEncoding";
     public static final String HLSCONFIG = "HLSConfig";
     public static final String H264_CODEC = "H264";
     public static final String MPEG4_GENERIC_CODEC = "mpeg4-generic";
     public static final String MP4A_LATM_CODEC = "mp4a-latm";
     public static final String MEDIA_STREAM_INFO_ELEMENT = "MediaStreamInfo";
}



