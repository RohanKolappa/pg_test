package com.barco.utils.mr;

public final class SystemServiceConst {
    public static final String FILETRANSFERAGENT = "com.ipvs.systemservice.impl.FileTransferAgent";
    public static final String EXPORTSERVICEDBRESPONSEFILE = "/com/ipvs/systemservice/xml/ExportServiceDBResponse.xml";
    public static final String IMPORTSERVICEDBRESPONSEFILE = "/com/ipvs/systemservice/xml/ImportServiceDBResponse.xml";
    public static final String FILEXFERSETUPREQUESTFILE = "/com/ipvs/systemservice/xml/FileXferSetupRequest.xml";
    public static final String FILEXFERSTARTREQUESTFILE = "/com/ipvs/systemservice/xml/FileXferStartRequest.xml";
    public static final String FILEXFERSTOPREQUESTFILE = "/com/ipvs/systemservice/xml/FileXferStopRequest.xml";
    public static final String FILEXFERSETUPRESPONSEFILE = "/com/ipvs/systemservice/xml/FileXferSetupResponse.xml";
    public static final String FILEXFERSTARTRESPONSEFILE = "/com/ipvs/systemservice/xml/FileXferStartResponse.xml";
    public static final String FILEXFERSTOPRESPONSEFILE = "/com/ipvs/systemservice/xml/FileXferStopResponse.xml";
    public static final String FILEXFER_EVENT_INSTANCE_XML = "/com/ipvs/systemservice/xml/FileXferEventInstance.xml";

    public static final int  SAVE_DB_ERROR=6000;
    public static final int  RESTORE_DB_ERROR=6001;
    public static final int  GET_SERVICE_CONFIG_ERROR=6002;
    public static final int  SET_SERVICE_CONFIG_ERROR=6003;
    public static final int  GET_SERVICE_STATUS_ERROR=6004;
    public static final String SAVED_DATABASES_DIR="/data/saved_databases/";
    public static final String DEFAULT_IMPORTDB="importdb.tgz";

    public static final int  FILETRANSFER_SEND_FILE_ERROR=6005;
    public static final int  FILETRANSFER_GET_FILE_ERROR=6006;
    
    public static final int  HA_SERVICE_NOT_RUNNING=6007;
    public static final int  EXECUTE_COMMAND_FAILED=6008;
    
    //upgrade constants
    public static final String UPGRADE_DIR = "/data/mediaroom/upgrade";
    public static final String DOWNLOAD = "download";
    public static final String IMAGE_TAR_FILE_NAME = "image.tar";
    public static final String SOFTWARE_FILE_EXTENSION = ".tar";
    public static final String IMAGES_FOLDER_NAME = "IMAGES";
    public static final String _MS_IP_ADDRESS = "_MS_IP_ADDRESS";
    
    public static final int  FROM_DIR_PATH_REQUIRED=6009;
    public static final int  UPGRADE_DEVICE_FAILED=6010;
    public static final int  UPGRADE_FAILED=6011;
        
    public static final String SOFTWARE_URL = "http://"+_MS_IP_ADDRESS+"/mediaroom/upgrade_IMAGES";
    public static final String LOCAL_HOST = "127.0.0.1";

    public static final String SUPERUSER_DEFAULT = "superuser-default";       
    public static final String DMS_OBJECT_NAME = "DMS";    

    // Upgrade Device Constants        
    public static final String DEVICE_UPGRADE_DIR = "/home/onair/htdocs/public/mediaroom/upgrade_IMAGES";
    public static final String UPGRADE_DEVICE_EL_NAME = "UpgradeDevice";    
    public static final String UPGRADE_FEATURE_SERVICE_VERSION = "2.27.1";
    public static final String URL = "URL";
    public static final String NID = "NID";
    public static final String TITLE = "Title";    
    public static final String SERVICE_VERSION = "serviceVersion";
    public static final String OWNER_USER_JID = "ownerUserJID";    
    public static final String IP_ADDRESS = "ipAddress";
    public static final String SERVER_VERSION_FILE_NAME = "server.version";
    public static final String LIST = "List";
    public static final String MS_DEVICE_EL_NAME = "MSDevice";
    public static final String PASSWORD = "password";
    public static final String ADMIN_PASSWORD = "admin";
    public static final String EXCEPTION = "Exception";
    public static final String CODE = "Code";
    public static final String DESCRIPTION = "Description";
    public static final String ERROR = "error";
    public static final String TYPE = "type";
    public static final String IPVS_CLIENT_DEVICE = "IPVSClient-Device";
    
    public static final String GET_SOFTWARE_IMAGE_STATUS_CLI_CMD = "get_software_image_status";    
        
}
