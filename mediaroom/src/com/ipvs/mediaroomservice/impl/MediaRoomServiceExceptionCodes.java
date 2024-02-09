package com.ipvs.mediaroomservice.impl;

public final class MediaRoomServiceExceptionCodes {
    public static final int ROOMCONFIGNOTFOUND = 2001;
    public static final int ROOMNOTFOUND = 2002;
    public static final int TRACKNOTFOUND = 2003;
    public static final int MEDIANOTFOUND = 2004;
    public static final int MEDIASOURCENOTFOUND = 2005;
    public static final int STREAMNOTFOUND = 2006;
    public static final int CANNOTHANDLESTREAMREQUEST = 2008;
    public static final int UPDATESTREAMACTIONOTFOUND = 2009;
    public static final int SOURCENOTFOUND = 2011;
    public static final int DESTNOTFOUND = 2012;
    public static final int PROFILENOTFOUND = 2013;
    public static final int PROFILEXMLINVALID = 2014;
    public static final int CONTACTNOTFOUND = 2015;
    public static final int XPATH_ELEMENT_MISSING = 2016;
    public static final int AGENTREQUESTFAILED = 2017;
    public static final int ROOMIDNOTFOUND = 2018;
    public static final int MEDIADESTNOTFOUND = 2019;
    public static final int MEDIADESTBUSY = 2020;
    public static final int MEDIASOURCEBUSY = 2021;
    public static final int RECORD_ERROR = 2022;
    public static final int CANNOT_USE_DSTPORT_AS_SOURCE = 2023;
    public static final int CANNOT_USE_SRCPORT_AS_DEST = 2024;
    public static final int STREAMURL_NOT_CONFIGURED = 2025;
    public static final int MEDIADEST_STOPPED = 2026;
    public static final int RESOURCE_NOT_A_PORT = 2027;
    public static final int DEST_NOT_SUPPORTED_FOR_STREAMTYPE = 2028;    
    public static final int CAN_NOT_START_STREAM_PROFILE_PARAMETERS_MISMATCH = 2029;
    public static final int SOURCE_BUSY = 2030;


    //Stream Action Errors
    public static final int STREAMOUTPUTNOTFOUND = 2032;
    public static final int ACTIONTRANSACTIONNOTFOUND = 2033;

    //Stream Action Errors
    public static final int STREAMALREADYPRESENT = 2042;
    public static final int STREAMNIDNOTFOUND = 2043;
    public static final int INVALID_OFFSET = 2045;
    public static final int SOURCEUNAVAILABLE = 2046;
    public static final int PVR_NOTAVAILABLE = 2047;
    public static final int STREAM_ALREADY_STARTED = 2048;
    public static final int STREAM_ALREADY_STOPPED = 2049;
    public static final int INVALID_UPDATEACTION = 2050;
    public static final int DESTUNAVAILABLE = 2051;
    public static final int DUPLICATEPVR = 2052;
    public static final int ROOMNODE_XMPPDELETE_FAILED = 2053;
    public static final int NOT_AUTHORIZED = 2054;
    public static final int AGENT_NOT_FOUND = 2055;
    public static final int SOURCEAGENTNOTFOUND = 2056;    
    public static final int USERJID_MISSING = 2057;
    public static final int RELAYDESTINSTANCENOTFOUND = 2058;
    public static final int INVALID_INPUT_TYPE = 2059;
    public static final int INVALID_OUTPUT_TYPE = 2060;
    public static final int OVERLAPPING_UNAMANAGED_V2D_DEST_PORT = 2061;
    public static final int DIRECT_CONNECTION_NOT_SUPPORTED_FOR_RTP = 2062;
    
    public static final int DEVICE_UNAVAILABLE = 2063;
    public static final int STREAMTIMELINENOTINSYNCWITHSESSION = 2064;
    
    public static final int MONITOR_AGENT_JID_NOT_FOUND = 2065;
    public static final int AUTH_AGENT_JID_NOT_FOUND = 2066;
    
	public static final int CAN_NOT_USE_REQUESTED_PROFILE =  2067;
	
	public static final int DEVICE_XML_NOT_FOUND = 2068;


    //PEM Exceptions Codes
    public static final int NO_MATCHING_SOURCES = 2100;
    public static final int NO_MATCHING_PROFILE = 2101;
    public static final int NO_MATCHING_MEDIA_FILES = 2102;
    public static final int NO_MATCHING_MEDIA_DIRECTORIES = 2103;
    public static final int NO_MATCHING_RELAYS = 2104;
    public static final int INPUTTYPENOTFOUND = 2105;
    public static final int OUTPUTTYPENOTFOUND = 2106;
    public static final int ACTIONNOTFOUND = 2107;
    public static final int STREAMTYPENOTFOUND = 2108;
    public static final int GET_SOURCE_LIST_FAILED = 2109;
    public static final int RELAY_CONFIGURATION_ERROR = 2110;


    //StreamingServer Errors
    public static final int NOAVAILABLEPORTS = 2300;
    public static final int NOAVAILABLEIPADDRESS = 2315;
    public static final int GETINSTANCESTATUSFAILED = 2301;
    public static final int ADDSSINSTANCEIDFAILED = 2302;
    public static final int UPDATESSINSTANCEIDFAILED = 2303;
    public static final int STARTSSINSTANCEIDFAILED = 2304;
    public static final int STOPSSINSTANCEIDFAILED = 2305;
    public static final int DELETESSINSTANCEIDFAILED = 2306;
    public static final int EXECUTESTREAMINGSERVERCMDFAILED = 2307;
    public static final int GETCURRENTTIMESTAMPFAILED = 2308;

    //Source Events Codes
    public static final int END_OF_THE_MEDIA = 2309;
    public static final int RELAY_DEVICE_DISCONNECTED = 2310;
    public static final int MEDIA_ERROR = 2311;
    public static final int DEST_DISCONNECTED = 2312;
    
    public static final int ENCODERPORTNOTAVAILABLE = 2313;
    public static final int MAXENCODERINSTANCESEXCEEDED = 2314;
    
    public static final int RELAY_STREAM_ERROR = 2315;

    //RoomDB Exception Codes
    public static final int NOTFOUND = 2401;
    public static final int NO_PERMISSION_TO_RECORD_MEDIA_ON_THIS_DIRECTORY = 2503;
    
    //StreamURL Exception Codes
    public static final int INVALID_DESTIP = 2601;      
    public static final int INVALID_DESTPORT = 2602;      
    public static final int INVALID_PROTOCOL = 2603;      

    // RTP Errors
	public static final int INVALID_SDP = 2604;

    public static final int OTHERS = 2900;

    //Media Stream File Request Exception Codes
    public static final int ADD_DEST_FILE_FAILED = 2200;
    public static final int UPDATE_METADATA_FAILED = 2201;
    public static final int MEDIA_STORE_PORT_NID_MISSING = 2202;
    public static final int MEDIA_STREAM_FILE_NID_NOT_FOUND = 2203;       
    public static final int DELETE_DEST_FILE_FAILED = 2204;
    public static final int UPDATE_DEST_FILE_PROFILE_FAILED = 2205;
	public static final int BOOKMARK_NID_NOT_FOUND = 2206;
	public static final int CLIPTYPE_NOT_FOUND = 2207;
	public static final int PARENT_MEDIA_DIR_MISSING = 2208;
	public static final int MEDIA_STREAM_DIR_NID_NOT_FOUND = 2209; 

	//Book mark handlers exceptions
    public static final int ADD_BOOK_MARK_FAILED = 2250;
    public static final int DELETE_BOOK_MARK_FAILED = 2251;
    public static final int UPDATE_BOOK_MARK_FAILED = 2252;       
    public static final int NO_BOOK_MARK_FOUND = 2253;
    public static final int GET_BOOKMARK_LIST_FAILED = 2254;
	public static final int MEDIA_UUID_NOT_FOUND = 2255;
	public static final int ACTION_NOT_SUPPORTED = 2256;
	
	//Get Destination dir exception code
	public static final int GET_PVR_DEST_MEDIA_DIR_FAILED = 2600;
	public static final int GET_DEST_MEDIA_DIR_FAILED = 2601;
	
	//NAT Exception Codes
	
	public static final int ERROR_GETTING_PUBLISHED_IP = 2701;
	public static final int ERROR_GETTING_RESOLVED_IP = 2702;
	public static final int INVALID_IP_ADDRESS = 2703;
	
	//App Server JID Missing
	public static final int APPSERVER_JID_NOT_FOUND = 2800;
	public static final int APPSERVER_JID_CONFLICT = 2801;
	
	// Port Exception Codes 	
	public static final int NO_VALUES_TO_UPDATE = 2901;
	
	public static final int EXECUTION_OF_COMMANDS_FIALED = 2902;
	
	public static final int RELAY_NOT_SUPPORTED_FOR_STREAM_TYPE=2903;
	
	
}
