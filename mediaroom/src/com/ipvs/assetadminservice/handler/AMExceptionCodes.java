package com.ipvs.assetadminservice.handler;


public class AMExceptionCodes {
	public static final int OBJECT_DOES_NOT_EXIST = 5001;
	public static final int METHOD_DOES_NOT_EXIST = 5002;

	public static final int PARAM_NOT_PROVIDED = 5003;
	public static final int SERVER_ERROR = 5004;

	
	public static final int PARAM_DOES_NOT_EXIST_IN_DB= 5005;
	public static final int OBJECTTYPE_NOT_PROVIED = 5006;
	public static final int SERVICE_ERROR = 5007;
	public static final int OBJECTTYPE_AND_NID_NOT_PROVIED = 5008;
	public static final int ACCESS_DENY_TO_READ = 5009;
	public static final int ACCESS_DENY_TO_DELETE = 5010;
	public static final int ACCESS_DENY_TO_UPDATE = 5011;
	public static final int ACCESS_DENY_TO_ADD = 5012;
	public static final int CUSTOM_VALIDATIONS_FAILED = 5013;
	public static final int INVALID_EMPTY_DIR_ACTION = 5014; 
	public static final int EMPTRY_RECYCLE_BIN_REQUEST_FAILED = 5015;
	public static final int RECYCLE_BIN_NOT_CONFIGURED = 5016;
    public static final int EXPORT_DB_ERROR=5017;
    public static final int IMPORT_DB_ERROR=5018;
    public static final int DELETE_DB_ERROR=5019;

    public static final int DETACH_DIR_ERROR=5020;
    public static final int ATTACH_DIR_ERROR=5021;
    public static final int DIR_TRANSITION_NOT_FOUND=5022;
    public static final int DIR_NOT_READY=5023;
    public static final int SEND_FILE_LOCAL_FAILED=5024;
    public static final int DIR_NOT_DETACHED=5025;
    public static final int RECEIVE_FILE_LOCAL_FAILED=5026;
    public static final int DIR_ATTACH_FILE_NOT_FOUND=5027;
    public static final int MEDIA_FILE_BUSY = 5028;
    
    public static final int INVALID_DATA = 5029;
    public static final int NO_PORT_FOUND_IN_DATABASE = 5030;
	
	//Media store Syn operation Exception codes
	public static final int MEDIA_STORE_REVISION_MISSING = 5100;
	public static final int MEDIA_STREAM_DIR_NID_MISSING = 5101;	
	public static final int MSSING_INPUT_VALUE = 5102;	
	public static final int UPDATE_MEDIA_STREAM_FILE_FAILED = 5103;	
	public static final int UPGRADE_MEDIA_STREAM_FILE_FAILED = 5104;
	public static final int MEDIA_STREAM_DIR_AGENT_MISSING = 5105;
    public static final int MEDIA_IMPORT_EXPORT_EXCEPTION = 5106;
    
    public static final int MEDIA_EXPORT_EMPTY_MEDIA_EXCEPTION = 5107;
    public static final int MEDIA_EXPORT_INVALID_TYPE_EXCEPTION = 5108;
    public static final int MEDIA_EXPORT_INVALID_STATE_EXCEPTION = 5109;
    public static final int MEDIA_EXPORT_FILE_NOT_FOUND_EXCEPTION = 5110;
    public static final int MEDIA_EXPORT_FILE_NEEDS_UPGRADE_EXCEPTION = 5111;
    public static final int MEDIA_EXPORT_PARENT_DIR_NOT_FOUND_EXCEPTION = 5112;
    public static final int MEDIA_EXPORT_DIR_JID_NOT_FOUND_EXCEPTION = 5113;
    public static final int MEDIA_EXPORT_EMPTY_DEST_FOLDER_PATH_EXCEPTION = 5114;
    public static final int MEDIA_IMPORT_EMPTY_SRC_FOLDER_PATH_EXCEPTION = 5115;
    public static final int MEDIA_OFFLOAD_EMPTY_NAS_URL_PATH_EXCEPTION = 5116;
        
	
	//ChangePassword Request Error Codes
	public static final int NO_USER_FOUND = 5200;
	public static final int OLD_PASSWORD_MISSING = 5201;
	public static final int NEW_PASSWORD_MISSING = 5202;
	public static final int NEW_PASSWORD_AND_RETYPE_PASSWORD_NOT_MATCHING = 5203;
	public static final int USER_PASSWORD_NOT_MATCHING = 5204;
	
    public static final int USER_NOT_AVAILABLE = 5205;

    public static final int VALID_ERROR_DUPLICATE= 5206;
    public static final int VALID_ERROR_SPACE_IN_NAME= 5207;
    public static final int VALID_ERROR_EMPTY_NAME= 5208;
    public static final int VALID_ERROR_CAPITAL_LETTERS_IN_NAME= 5209;
    public static final int VALID_ERROR_SPECIAL_CHARACTERS_IN_NAME= 5210;
    public static final int VALID_TRIM_LENGTH = 5211;
    public static final int VALID_ERROR_SPECIAL_CHARACTERS_IN_EXPORT_SCHEDULE = 5212;
    public static final int VALID_ERROR_EXPORT_SCHEDULE_RANGE = 5213;
    
    public static final int VALID_ERROR_SPECIAL_CHARACTERS_IN_METADATA = 5214;
    
    public static final int MEDIA_PREPARE_FILE_NOT_FOUND_EXCEPTION = 5215;
    public static final int MEDIA_PREPARE_PARENT_DIR_NOT_FOUND_EXCEPTION = 5216;
    public static final int VALID_ERROR_OFFSET = 5217;
    public static final int VALID_ERROR_CONTACT_JID = 5218;
    public static final int VALID_ERROR_MEDIACLIP = 5219;
    public static final int VALID_ERROR_PVRMEDIACLIP = 5220;
}
