package com.ipvs.assetadminservice.handler;

import com.ipvs.common.service.MRRequest;
import com.ipvs.sessionservice.resource.handler.DeleteFileRequestHandler;

public class AMConst {
    public static final String AssetResource = "AssetResource";
    public static final String Device = "Device";
    public static final String MediaDir = "MediaDir";
    public static final String MediaStreamDstPortResource = "MediaStreamDstPortResource";
    public static final String MediaStreamFileResource = "MediaStreamFileResource";
    public static final String MediaStreamRelayPortResource = "MediaStreamRelayPortResource";
    public static final String MediaStreamSrcPortResource = "MediaStreamSrcPortResource";
    public static final String StreamProfile = "StreamProfile";
    public static final String User = "User";
    public static final String IPVSAdmin = "IPVSAdmin";
    public static final String MEDIA_ROOM = "MediaRoom";
    public static final String COLLABORATION_CONFIG = "CollaborationConfig";
    public static final String SERVICE_RESOURCE ="ServiceResource";
    public static final String SERVICE_CONFIG ="ServiceConfig";
    public static final String DISPLAY_PROFILE ="DisplayProfile";
    
    //new codes for PEM implementation
    //Device object types
    public static final String DMS_1000_DEVICE_OBJECTYPE = "DMS-1000-Device";
    public static final String DMS_1500_DEVICE_OBJECTYPE = "DMS-1500-Device";
    public static final String DMS_25_DEVICE_OBJECTYPE = "DMS-25-Device";
    public static final String DMS_100_DEVICE_OBJECTYPE = "DMS-100-Device";
    public static final String DMS_120_DEVICE_OBJECTYPE = "DMS-120-Device";
    public static final String DMS_200_DEVICE_OBJECTYPE = "DMS-200-Device";
    public static final String MS_DEVICE_OBJECTYPE = "MS-Device";
    public static final String MS_25_DEVICE_OBJECTYPE = "MS-25-Device";
    public static final String APP_SERVICE_TYPE = "AppService";
    public static final String IPVS_CLIENT_DEVICE_OBJECTYPE = "IPVSClient-Device";
    public static final String V2D_RX_DEVICE_OBJECTYPE = "V2D-Rx-Device";
    public static final String V2D_TX_DEVICE_OBJECTYPE = "V2D-Tx-Device";
    public static final String V2D_XP200_DEVICE_OBJECTYPE = "V2D-XP200-Device";
    public static final String V2D_XP100_DEVICE_OBJECTYPE = "V2D-XP100-Device";
    public static final String V2D_XPi_DEVICE_OBJECTYPE = "V2D-XPi-Device";
    public static final String V2D_XP200S_DEVICE_OBJECTYPE = "V2D-XP200S-Device";
    public static final String V2D_XP220_DEVICE_OBJECTYPE = "V2D-XP220-Device";
    public static final String V2D_XP220S_DEVICE_OBJECTYPE = "V2D-XP220S-Device";
    public static final String ORB_500_DEVICE_OBJECTTYPE = "ORB-500-Device";    
    
    public static final String RTP_RX_DEVICE_OBJECTYPE = "RTP-Rx-Device";
    public static final String RTP_TX_DEVICE_OBJECTYPE = "RTP-Tx-Device";
    public static final String UDP_RX_DEVICE_OBJECTYPE = "UDP-Rx-Device";
    public static final String UDP_TX_DEVICE_OBJECTYPE = "UDP-Tx-Device";
    public static final String MPEGTS_RX_DEVICE_OBJECTYPE = "MPEGTS-Rx-Device";
    public static final String MPEGTS_TX_DEVICE_OBJECTYPE = "MPEGTS-Tx-Device";
    public static final String WEB_CLIENT_DEVICE = "WebClient-Device";
    public static final String PROXY_CLIENT_DEVICE = "Proxy-Device";
    public static final String ANY_OBJECTTYPE = "ANY";
    
    //New Barco devices
    public static final String MNA_110_ENCDP_DEVICE_OBJECTYPE = "MNA-110-ENCDP-Device";
    public static final String MNA_110_ENCDVI_DEVICE_OBJECTYPE = "MNA-110-ENCDVI-Device";
    public static final String MNA_110_ENCHDSDI_DEVICE_OBJECTYPE = "MNA-110-ENCHDSDI-Device";
    public static final String MNA_120_DECDVIAUDIO_DEVICE_OBJECTYPE = "MNA-120-DECDVIAudio-Device";
    public static final String MNA_120_DECDVI_DEVICE_OBJECTYPE = "MNA-120-DECDVI-Device";
    public static final String MNA_120_ENCANAAUDIO_DEVICE_OBJECTYPE = "MNA-120-ENCANAAudio-Device";
    public static final String MNA_120_ENCANA_DEVICE_OBJECTYPE = "MNA-120-ENCANA-Device";
    public static final String MNA_120_ENCDPAUDIO_DEVICE_OBJECTYPE = "MNA-120-ENCDPAudio-Device";
    public static final String MNA_120_ENCDP_DEVICE_OBJECTYPE = "MNA-120-ENCDP-Device";
    public static final String MNA_120_ENCDVIAUDIO_DEVICE_OBJECTYPE = "MNA-120-ENCDVIAudio-Device";
    public static final String MNA_120_ENCDVI_DEVICE_OBJECTYPE = "MNA-120-ENCDVI-Device";
    public static final String MNA_120_ENCHDSDIAUDIO_DEVICE_OBJECTYPE = "MNA-120-ENCHDSDIAudio-Device";
    public static final String MNA_120_ENCHDSDI_DEVICE_OBJECTYPE = "MNA-120-ENCHDSDI-Device";
    

    //Media stream Port object types
    public static final String MEDIA_STREAM_PORT_RESOURCE_OBJECTTYPE = "MediaStreamPortResource";
    public static final String MEDIA_STREAM_DST_PORT_RESOURCE_OBJECTTYPE = "MediaStreamDstPortResource";
    public static final String MEDIA_STREAM_SRC_PORT_RESOURCE_OBJECTTYPE = "MediaStreamSrcPortResource";
    public static final String MEDIA_STREAM_RELAY_PORT_RESOURCE_OBJECTTYPE = "MediaStreamRelayPortResource";
    public static final String MEDIA_STREAM_IO_PORT_RESOURCE_OBJECTTYPE = "MediaStreamIOPortResource";

    //Media Stream Profile objects	
    public static final String MEDIA_STREAM_PROFILE_OBJECTTYPE = "MediaStreamProfile";

    //public static final String STREAM_PROFILE_OBJECTTYPE = "StreamProfile";
    public static final String MPEGTS_MEDIA_STREAM_PROFILE_OBJECTTYPE = "MPEGTSMediaStreamProfile";
    public static final String UDP_MEDIA_STREAM_PROFILE_OBJECTTYPE = "UDPMediaStreamProfile";
    public static final String RTP_MEDIA_STREAM_PROFILE_OBJECTTYPE = "RTPMediaStreamProfile";
    public static final String V2D_MEDIA_STREAM_PROFILE_OBJECTTYPE = "V2DMediaStreamProfile";
    public static final String REQUEST_NAME = "AssetManagementRequest";

    public static final String EXPORT_IMPORT_FILE_LIST_ITEM_OBJECTTYPE = "ExportImportFileListItem";
    
    //device asset resource nid
    public static final String ASSER_RESOURCE_NID = "AssetResourceNID";

    // stateflag for assetresource
    public static final String STATE = "State";
    public static final String STATEFLAG = "StateFlag";
    public static final String STATEMSG = "StateMsg";
    public static final String ASSETSTATE_OFFLINE = "Offline";
    public static final String ASSETSTATE_READY = "Ready";

    
    //contact jid constants	
    public static final String OWNERUSERJID = "OwnerUserJID";
    public static final String PRESENCE = "Presence";    
    public static final String CONTACT = "Contact";
    public static final String USERLIST = "UserList";
    public static final String USER= "User";
    public static final String APPNAME= "appName";
    public static final String JID = "jid";  
    public static final String AVAILABLE = "available";
    public static final String UNAVAILABLE = "unavailable";

    //portContact NID 
    public static final String MEDIA_STREAM_SRC_PORT_RESOURCE_NID = "MediaStreamSrcPortResourceNID";
    public static final String MEDIA_STORE_PORT_RESOURCE_NID = "MediaStorePortResourceNID";
    public static final String MEDIA_STREAM_RELAY_PORT_RESOURCE_NID = "MediaStreamRelayPortResourceNID";
    public static final String MEDIA_STREAM_DST_PORT_RESOURCE_NID = "MediaStreamDstPortResourceNID";
    public static final String MEDIA_STREAM_IO_PORT_RESOURCE_NID = "MediaStreamIOPortResourceNID";
    public static final String NID_VALUE = "NIDValue";

    //media store objects
    public static final String MEDIA_STREAM_DIR_RESOURCE = "MediaStreamDirResource";
    public static final String MEDIA_STREAM_FILE_RESOURCE = "MediaStreamFileResource";
    public static final String MEDIA_STORE_PORT_RESOURCE = "MediaStorePortResource";
    public static final String BOOKMARK = "Bookmark";
    

    //Device Constances
    public static final String DEVICE_STATUS = "DeviceStatus";
    public static final String DEVICE_CONFIG = "DeviceConfig";
    
    //operations
    public static final String ASSET_ADD_REQUEST = "Add";
    public static final String ASSET_DELETE_REQUEST = "Delete";
    public static final String ASSET_REPLACE_REQUEST = "Replace";
    public static final String ASSET_GET_REQUEST = "Get";
    public static final String FILE_DELETE_REQUEST = "DeleteFile";
    public static final String FILE_REPLACE_REQUEST = "Replace";
   
    //requestname
    public static final String ASSET_REQUEST_NAME = MRRequest.getRequestName(AssetManagementRequestHandler.class.getName());    
    public static final String DELETE_REQUEST_NAME = MRRequest.getRequestName(DeleteFileRequestHandler.class.getName());
    public static final String REPAIR_MEDIA_STREAM_FILE_REQUEST = MRRequest.getRequestName(RepairMediaStreamFileRequestHandler.class.getName());    
    public static final String UPGRADE_MEDIA_STREAM_FILE_REQUEST = MRRequest.getRequestName(UpgradeMediaStreamFileResourceRequestHandler.class.getName());
    public static final String UPGRADE_MEDIA_STREAM_DIR_REQUEST = MRRequest.getRequestName(UpgradeMediaStreamDirResourceRequestHandler.class.getName());    
    
    //Recycle dir NId
    public static final String RECYCLE_BIN_DIR_NID = "RECYCLEBIN";
    
    //Recycle bin status.
    public static final String START_EMPTY_DIR_PROCESS = "Start";
    public static final String EMPTY_DIR_PROCESS_STARTED = "Started";
    public static final String STOP_EMPTY_DIR_PROCESS = "Stop";
    public static final String EMPTY_DIR_PROCESS_STOPPED = "Stopped";
	public static final String EMPTY_DIR_REQUEST = "EmptyDirRequest";
	public static final String EMPTY_DIR_ACTION = "EmptyDirProcess";
	
	public static final int PROCESS_STOPPED = 1001;
	public static final int PROCESS_IN_PROGRESS = 1002;
	

	//update resource status constants
	public static final String RESOURCE_STATE ="ResourceState";
	public static final String RESOURCE_STATE_LIST ="ResourceStateList";
	public static final String TYPE ="Type";
	public static final String INFO ="Info";
	//action elements
	public static final String ADD_REQUEST = "AddRequest";
	public static final String DELETE_REQUEST = "DeleteRequest";
	public static final String REPLACE_REQUEST = "ReplaceRequest";
	public static final String REPLACE_RESPONSE = "ReplaceResponse";
	public static final String GET_REQUEST = "GetRequest";
	public static final String GET_RESPONSE = "GetResponse";
	public static final String MEDIA_STREAM_DIR_NID = "MediaStreamDirNID";
	
	//upgrade Media Stream Dir const
	public static final String MEDIA_STREAM_DIR_RESOURCE_NID = "MediaStreamDirResourceNID";
	
	//ChangePassword Request Constants
	
	public static final String PASSWORD_EL_NAME =  "Password";
	public static final String OLD_PASSWORD_EL_NAME =  "OldPassword";
	public static final String NEW_PASSWORD_EL_NAME =  "NewPassword";
	public static final String RETYPE_PASSWORD_EL_NAME =  "RetypePassword";
	public static final String NID = "NID";
	public static final String BY_NID_ATTRIBUTE = "byNID";
	public static final String OLD_PASSWORD_ATTR_NAME =  "oldPassword";
	public static final String NEW_PASSWORD_ATTR_NAME =  "newPassword";
	
	
	// import export tags ///
    public static final String EXPORT_MEDIA_DIR = "exportMedia";
    public static final String IMPORT_MEDIA_DIR = "importMedia";
    public static final String DELETE_MEDIA_DIR = "deleteMedia";
    public static final String EXPORT_MEDIA_FILE = "exportMediaFile";
    public static final String IMPORT_MEDIA_FILE = "importMediaFile";
    public static final String DELETE_MEDIA_FILE = "deleteMediaDFile";
    
    // Default profiles              
    public static final String TITLE = "Title";
    public static final String DESCRIPTION = "Description";
    public static final String STREAM_TYPE = "StreamType";
    public static final String PRIORITY = "Priority";    
    public static final String IS_MULTICAST = "IsMulticast";    
    public static final String BANDWIDTH = "Bandwidth";
    public static final String ENABLE_KBM = "EnableKbm";
    public static final String ENABLE = "Enable";
    
    //Max Media Clients Constants
    public static final String LOGIN = "Login";
    public static final String PREFERENCES = "Preferences";
    public static final String MAX_MEDIA_CLIENTS ="maxMediaClients";
    
    public static final String MEDIA_RESTORE_REQUEST = "MediaRestore";
    public static final String MEDIA_DELETE_REQUEST = "MediaDelete";
    
    public static final String BY_ASSET_NID_ATTRIBUTE = "byAssetNID";
    public static final String DEVICE_ELEMENT_ID_ATTRIBUTE = "deviceElementId";
    public static final String TYPE_ATTRIBUTE = "type";
    public static final String PORT = "Port";
    
    public static final String DIR_NID_ATTRIBUTE = "dirNID";
    public static final String DIR_ID_ATTRIBUTE = "dirID";
    public static final String STORE_FILE_NID_ATTRIBUTE = "storeFileNID";
    public static final String MEDIA_STORE_FILE = "MediaStoreFile";
    public static final String REVISION_ATTRIBUTE = "revision";
    public static final String READ_ONLY_ATTRIBUTE = "readOnly";
    public static final String SELECTFORMAT_ATTRIBUTE = "selectFormat";
    public static final String SELECTFORMAT_NATIVE = "NATIVE";
    public static final String SELECTFORMAT_API = "API";
    public static final String SIZE_ATTRIBUTE = "size";
    
    public static final String ADMIN = "admin";
    //public static final String PORT_WINDOW_XPATH = "//DeviceStatus/AVCodecElementsStatus/XP200PortStatus/XP200DisplayStatus[../PortID=2]";
  
   public static final String PROPRIETARY_MEDIA_FORMAT = "Proprietary";
   
	public static final String SET_SERVICE_CLIENT_STATUS_ELEMENT = "SetServiceClientStatus";
	
		public static final String NAME = "name";
		public static final String VALUE = "value";
		
		public static final String UPDATE_ATTRIBUTE = "UpdateAttribute";
		  public static final String UPDATE = "Update";
		  public static final String ADD="Add";
		  public static final String DELETE="Delete";
		  
		  
			public static final String XPATH_ATTRIBUTE = "xPath";	
			public static final String OPERATION_ATTRIBUTE = "operation";
			
			public static final String DIRTY_TAG="Dirty";
			public static final String CLEAN_FLAG="CLEAN";
			
			public static final String TRUE ="true";
			
			

}

