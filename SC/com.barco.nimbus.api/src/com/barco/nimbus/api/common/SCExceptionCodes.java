package com.barco.nimbus.api.common;

/**
 * Represents the common exception codes Each service will define the service
 * specfic exception codes
 * 
 * @author miyer
 * 
 */
public final class SCExceptionCodes {
	public static final int SYSTEMERROR = 1000;
	public static final int XMPPNOTFOUND = 1001;
	public static final int XMPPCONNECTIONFAILED = 1002;
	public static final int XMPPLOGINFAILED = 1003;
	public static final int DUPLICATELOGIN = 1004;
	public static final int LOGINNOTFOUND = 1005;
	public static final int XMPPLOGOUTFAILED = 1006;
	public static final int CANNOTUPDATENODE = 1007;
	public static final int SERVICENOTFOUND = 1008;
	public static final int REQUESTNOTFOUND = 1009;
	public static final int INVALIDREQUEST = 1010;
	public static final int ELEMENTALREADYPRESENT = 1011;
	public static final int CANNOTDELETECHILDRENEXIST = 1012;
	public static final int RESPONSEHANDLERFAILED = 1013;
	public static final int NODENOTFOUND = 1014;

	public static final int MRSCHEMA_XML_NOT_FOUND = 1015;
	public static final int MRSCHEMA_XML_NOT_VALID = 1016;
	public static final int FAILED_TO_CREATE_DB_NODE = 1017;
	public static final int FAILED_TO_CREATE_DB_SCHEMA_NODE = 1018;
	public static final int FAILED_TO_READ_DB_SCHEMA= 1019;
	public static final int SERVICENOTAVAILABLE= 1020;
	public static final int FAILED_TO_DELETE_ROOTNID= 1021;
	public static final int DB_NOT_INITIALIZED= 1022;
	public static final int XMPPADMIN_FAILED= 1024;
	public static final int ASSET_RESOURCE_NOTAVAILABLE= 1025;
	public static final int ASSET_TEMPLATE_NOTFOUND= 1026;
	public static final int ASSET_RESOURCE_ACCESSDENIED= 1027;
	public static final int DEVICEADMIN_FAILED = 1028;
	public static final int GET_DEVICE_STATUS_FAILED =1029;
	public static final int NO_RESPONSE_RECEIVED = 1030;
	public static final int XAUTH_DENIED= 1023;
	public static final int XAUTH_REVOKED= 1031;
	public static final int XML_NOT_FOUND = 1032;
	public static final int XML_NOT_VALID = 1033;
	public static final int SERVICE_VERSION_XML_NOT_FOUND = 1034;
	public static final int SERVICE_VERSION_XML_NOT_VALID = 1035;
	public static final int SERVICE_VERSION_ADD_ATTRB_FAILED = 1036;
	public static final int SET_SERVICE_DOMAIN_FAILED= 1020;
	public static final int CAN_NOT_LOGIN_USING_TEMPLATE= 1037;	
	public static final int RESOURCE_AUTHERIZATION_FAILED= 1038;
	public static final int CAN_NOT_APPLY_CONFIG = 1039;
	public static final int INVALID_SERVER_IPADDRESS = 1040;
	public static final int CAN_NOT_LOGIN_DEVICE_TYPE_MISMATCH = 1041;
	


	//Set service domain exceptions
	public static final int SERVICE_VERSION_MISMATCH = 1200;
	
	public static final int HANDLE_REQUEST_FAILED = 1201;	
	
	public static final int TO_JID_EMPTY = 1300;
	
	public static final int CHECKING_AVAILABLE_PRESENCE_FAILED = 1301;
	
	public static final int MAX_MEDIA_CLIENTS_EXCEEDED = 1302;
	
	public static final int DEVICE_ALREADY_IN_USE = 1303;
	
	public static final int FILE_XFER_INSTANCE_NOT_FOUND = 1304;
	
	//Link Local Exception
	public static final int LINK_LOCAL_JID_NOT_FOUND = 1400;
	public static final int DEVICE_MAC_ADDRESS_NOT_FOUND = 1401;
	public static final int GET_NETWORK_STATUS_FAILED = 1402;
	public static final int START_LINK_LOCAL_FAILED = 1403;
	public static final int CAN_NOT_FIND_LINK_LOCAL_SERVICE = 1404;
	public static final int DEVICE_IPADDRESS_NOT_FOUND = 1405;
	
	public static final int CANNOT_DELETE_ONLINE_DEVICE = 1406;
	
	public static final int DEVICE_OFFLINE = 1407;
	
	// jmDNS exception
	public static final int START_DISCOVERY_FAILED = 1408;
	public static final int CAN_NOT_FIND_DISCOVERY_SERVICE = 1409;
	public static final int PROFILE_EMPTY = 1410;
	
}
