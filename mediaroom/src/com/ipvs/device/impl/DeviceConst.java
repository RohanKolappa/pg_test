package com.ipvs.device.impl;

public class DeviceConst {
    // Config update Commands 
    public static final String UPGRADE_CONFIG = "UpgradeConfig";
    public static final String FACTORY_CONFIG = "FactoryConfig";
    public static final String APPLY_CONFIG = "ApplyConfig";

    //xpath constants
    public static final String USERPATH = "//user";
    public static final String PASSWORDPATH = "//password";
    public static final String SERVERPATH = "//server";
    public static final String RESOURCEPATH = "//resource";
    public static final String PORTPATH = "//port";
    public static final String DOMAINPATH = "//domain";
    public static final String STATUSFILE_UNIX = "/tmp/status";
    public static final String STATUSFILE_WINDOWS = "status";
    public static final String TRIGGERNAME = "triggername";
    public static final String BASECFGPREFIX = "/com/ipvs/xsd/objectXml/";
    public static final String BASECFGSUFFIX = ".xml";
    public static final String CFGHANDLER = "/com/ipvs/xsd/xml/confighandler.xml";
    public static final String STATUSHANDLER = "/com/ipvs/xsd/xml/statushandler.xml";
    public static final String DEVICESTATUSFILE = "/tmp/Device.xml";
    public static final String NOTIFICATIONHANDLER = "/com/ipvs/xsd/xml/notificationhandler.xml";

    //cli default port
    public static final int CLI_PORT = 10007;
    
    //jetty server port
    public static final int HTTP_SERVER_PORT = 10080;
    
    //Notification handler constants
    public static final String CLI_CONFIG = "CLIConfig";
    public static final String CLI_STATUS = "CLIStatus";
    public static final String SS_NOTIFICATION = "SSNotification";
    public static final String SYSTEM_NOTIFICATION = "SystemNotification";
    public static final String NOTIFICATION_TYPE = "NotificationType";
    public static final String EVENT_SOURCE_XPATH = "EventSourceList/EventSource";
    public static final String EVENT_TYPE_XPATH = "EventTypeList/Type";
    public static final String DATA_XPATH_LIST_XPATH = ".//DataXPathList/XPath";
    public static final String MEDIA_ROOM_EVENT = "MediaRoomEvent";
    public static final String MEDIA_ROOM_EVENT_XML_PATH = "MediaRoomEventXMLPath";
    public static final String SERVICE_XPATH = "ServiceList/Service";
    public static final String NOTIFICATION = "Notification";
    public static final String ACTION = "action";
    public static final String ACTION_DATA ="ActionData";

    public static final String MEDIA_FILE_CONVERTER = "MediaFileConverter";
    public static final String MEDIA_FILE_NFS_UPLOAD = "NfsUpload";
    
    
    //HA Constants
    public static final String HA_CONF_FILE_PATH = "/usr/local/exist/deviceapp.ha.conf";
   // public static final String HA_CONF_PROPERTY_SERVER = "SERVER";
    public static final String HA_CONF_PROPERTY_REPLICATE = "REPLICATE";
    public static final String HA_CONF_PROPERTY_SECONDARY = "SECONDARY";
    public static final String HA_CONF_PROPERTY_DBDIR = "DBDIR";
    public static final String DOWNLOAD_DIR_PATH = "/data/download/";
    public static final String DOWNLOAD_DB_REQUEST_XML_PATH = "/com/ipvs/systemservice/xml/DownloadDBRequest.xml";
    public static final String UPGARDE_REQUEST_XML_PATH = "/com/ipvs/systemservice/xml/UpgradeRequest.xml";
    public static final String DOWNLOAD_DIR_EL_NAME = "DownloadDirPath"; 
    public static final String DOWNLOAD_TGZ_FILE_NAME = "download_db.tgz";
    public static final String DATA_DIR_NAME = "data/";
    public static final String DATA_SAVE_DIR_NAME = "data_save/";
    public static final String UPLOAD_DIR_NAME = "upload/";
    
    //Error Constants
    public static final String OK = "OK";
    public static final String ERRORS = "ERRORS";
    public static final String FAILED = "FAILED";
    
    public static final String DEVICE_XSL_ROOT = "/com/ipvs/device/xsl/";
    public static final String DEVICE_CONFIG_HANDLER_XML ="/com/ipvs/xsd/xml/confighandler.xml";
    public static final String DEVICE_CONFIG ="DeviceConfig";
    
    
    public static final String INIT_CONFIG_TASK_NAME ="//DeviceConfig";
    public static final String INIT_STATUS_TASK_NAME ="//DeviceStatus";
    public static final String INIT_PORT_TYPE_UPDATE_TASK_NAME ="InitPortResourceType";
	public static final String API_KEY_TABLE_FILE_PATH ="/var/status/APIKeyTable.xml";    
}
