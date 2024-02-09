package com.ipvs.common;


/**
 * This is the list of common constants to be used in the various packages
 * If the constant is specific to a package it can be added to the specific package
 * @author miyer
 *
 */
public class MRConst {
    public static enum OPERATION_ENUM  {Add,Update,UpdateAttribute,Delete}
	public static final String NULL = null;
    public static final String Version = "3.0.11";
    public static final String TRUE = "true";
    public static final String FALSE = "false";
    public static final String AGENT_STATE = "AgentState";
    public static final String AGENT_RESOURCE_STATE = "AgentResourceState";

    //Set Service Domain Element name 
    public static final String ASSET_PRIVATE_DATA = "AssetPrivateData";
    public static final String ASSET_PRIVATE_KEY_DATA = "AssetPrivateKeyData";
    public static final String ASSET_TEMPLATE_TITLE = "AssetTemplateTitle";
    public static final String SERVICE_VERSION = "ServiceVersion";
    public static final String SOFTWARE_VERSION = "SoftwareVersion";
    public static final String VERSION = "Ver";
    
    public static final String MRSCHEMAPATH = "/com/ipvs/common/service/MRSchema.xml";
    
    //Link Local Constants
    public static final String LINK_LOCAL_DOMAIN = "@linklocal";
    public static final String LOGIN_REQUEST_DATA = "LoginRequestData";
    public static final String CLIENT_KEY = "clientKey";
    
    public static final String REGISTERED = "Registered";
    public static final String UN_REGISTERED = "UnRegistered";
	public static final String ADD ="Add";
	public static final String UPDATE ="Update";
	public static final String UPDATE_ATTRIBUTE ="UpdateAttribute";
	public static final String DELETE ="Delete";
	public static final String XPATH_ATTRIBUTE ="xPath";
	public static final String OPERATION_ATTRIBUTE ="operation";
	
	public static final String AGENT_INIT_TASKS_XML_PATH ="/com/ipvs/client/AgentInitTasks.xml";
	public static final String TASK_EL_NAME="Task";
	public static final String TASK_NAME_ATTRIBUTE ="name";
	public static final String TASK_STATE_ATTRIBUTE ="state";
	public static final String TASK_REQUEST_NAME_ATTRIBUTE ="requestName";
	public static final String TASK_REQUEST_TOKEN_ATTRIBUTE ="requestToken";
	public static final String TASK_WAIT_TO_COMPLETE_ATTRIBUTE ="waitToComplete";
	public static final String TASK_PRIORITY_ATTRIBUTE ="priority";
	
	public static final String _CLIENTDATASEPERATOR_= "_CLIENTDATASEPERATOR_";
	
	
	//Services
	public static final String LAYOUT_SERVICE= "Layout";
	public static final String MEDIA_SERVICE= "Media";
	public static final String DEVICE_ADMIN_SERVICE= "DeviceAdmin";
	public static final String CONFIG_ADMIN_SERVICE= "Config"; //temporary
	public static final String MEDIA_STORE_ADMIN_SERVICE= "MediaStore";
	public static final String SESSION_RESOURCE_SERVICE= "sessionservice.resource";
	public static final String SESSION_SERVICE_SERVICE = "sessionservice.service";
	public static final String PUBLISH_FILE_DOWNLOAD_REQUEST="PublishFileDownloadStateRequest";
	public static final String FTP_SERVICE= "FileTransfer";
	public static final String PUBSUB_SERVICE = "Pubsub";
	public static final String OPERATOR_SERVICE="Operator";
	public static final String CLIENT_CALLBACK ="ClientCallback";	
	
	//request header constants
	public static final String SERVICE_NAME = "serviceName";
	public static final String REQUEST_NAME = "requestName";
	public static final String LOG_LEVEL = "logLevel";
	public static final String USER_JID = "userJID";
	public static final String REQUEST_NID = "requestNID";
	public static final String CLIENT_DATA = "ClientData";
	public static final String STATE = "state";
	public static final String HEADER_XPATH = "//Header";
	public static final String TYPE = "type";
	public static final String REQUEST_TYPE_GET ="GET";
	public static final String REQUEST_TYPE_RPC ="RPC";
	public static final String DEPRECATED ="deprecated";
	
	
}


