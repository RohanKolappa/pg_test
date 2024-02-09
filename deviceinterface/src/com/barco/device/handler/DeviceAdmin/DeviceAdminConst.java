package com.barco.device.handler.DeviceAdmin;


public class DeviceAdminConst {

	public static final String DIRTY_TAG="Dirty";
	public static final String CLEAN_FLAG="CLEAN";
	public static final String MODIFY_FLAG="MODIFY";
	public static final String ADD_FLAG="ADD";
	public static final String INIT_FLAG="INIT";
	public static final String DELETE_FLAG="DELETE";
	public static final String ERROR_FLAG="ERROR";
	
	
	public static final String NETWORK_PORT = "NetworkPort";
	public static final String SRC_PORT = "SrcPort";
	public static final String DST_PORT = "DstPort";
	public static final String RELAY_PORT = "RelayPort";
	public static final String STORE_PORT = "StorePort";
	public static final String DEVICE_ELEMENT_NAME ="Device";
	public static final String PORT_LIST_ELEMENT_NAME ="PortList";
	public static final String DEVICE_ADMIN_SERVICE_INFO ="DeviceAdminServiceInfo";
	public static final String MEDIA_SERVICE_INFO ="MediaServiceInfo";
	public static final String LAYOUT_SERVICE_INFO ="LayoutServiceInfo";
	public static final String MEDIASTORE_SERVICE_INFO ="MediaStoreServiceInfo";
	public static final String PORT_ELEMENT_NAME ="Port";
	public static final String TYPE_ATTRIBUTE_NAME ="type";
	public static final String ID_ATTRIBUTE_NAME ="id";
	public static final String MAX_INSTANCE_COUNT_ATTRIBUTE_NAME ="maxInstanceCount";
	public static final String STREAM_TYPE_ATTRIBUTE ="streamType";
	public static final String SOFTWARE_VERSION_ATTRIBUTE_NAME ="softwareVersion";
	public static final String READY_ATTRIBUTE_NAME ="ready";
	public static final String PORT_ID_ELEMENT_NAME ="PortID";
	public static final String NETWORK_STATUS_ELEMENT_NAME = "NetworkStatus";
	
	
	public static final String SOFTWARE_VERSION_XPATH = "//SoftwareImageStatus/Parameters/Parameter[@name='Version']";
	public static final String ETHERNETPORTCONFIG_XPATH ="//EthernetPortConfig";
	
	//Events
	public static final String DEVICE_CONFIG_UPDATE_EVENT = "DeviceConfigUpdateEvent";
	public static final String LOGIN_EVENT = "LoginEvent";
	public static final String DEVICE_STATUS_UPDATE_EVENT = "DeviceStatusUpdateEvent";
	public static final String DEVICE_ADMIN_SERVICE = "DeviceAdmin";
	public static final String MEDIA_STORE_SERVICE = "MediaStore";
	public static final String LAYOUT_SERVICE = "Layout";
	public static final String MEDIA_SERVICE = "Media";
	public static final String INTERNAL = "Internal";
	public static final String SC_SHUTDOWN_EVENT = "SCShutdownEvent";
	
	public static final String API_KEY_TABLE_FILE_PATH ="/var/status/APIKeyTable.xml";
	
	public static final String THUMBNAIL = "thumbnail";
    
	
}

