package com.barco.utils.mr;

public final class MediaRoomServiceConst {
    public static final String DESTSTREAMAGENT = "com.ipvs.mediaroomservice.impl.DestStreamAgent";
    public static final String SOURCESTREAMAGENT = "com.ipvs.mediaroomservice.impl.SourceStreamAgent";
    public static final String SOURCECLIENTAGENT = "com.ipvs.mediaroomservice.impl.SourceClientAgent";
    public static final String CONFIGSTREAMAGENT = "com.ipvs.mediaroomservice.impl.ConfigStreamAgent";
    public static final String APPSERVERAGENT = "com.ipvs.mediaroomservice.impl.AppServerAgent";
    public static final String MEDIASTOREADMINREQUEST = "MediaStoreAdminRequest";
    public static final String MEDIASTOREADMINRESPONSE = "MediaStoreAdminResponse";
    public static final String OPERATOR = "Operator";
    public static final String STREAMTRACK = "StreamTrack";
    public static final String SYNCSTREAMTRACK = "SyncStreamTrack";
    public static final String RECORDTRACK = "RecordTrack";
    public static final String PVRTRACK = "PVRTrack";
    public static final String SYSTEMTRACKLIST = "systemtracklist";
    public static final String TRACKLIST = "tracklist";
    public static final String TRACK = "track";
    public static final String SYNCDATA = "syncdata";
    public static final String LIVE = "LIVE";
    public static final String WILDCARD = "*";
    public static final String NAME = "Name";
    public static final String VALUE = "Value";
    public static final String MEDIAROOM = "MediaRoom";
    public static final String APPLICATIONDATA = "ApplicationData";
    public static final String COLLABORATION = "Collaboration";
    public static final String CONNECTION = "Connection";
    public static final String MEDIASOURCE = "MediaSource";
    public static final String MEDIADEST = "MediaDestination";
    public static final String NID = "NID";    
    public static final String MEDIASOURCENID = "MediaSourceNID";
    public static final String MEDIADESTNID = "MediaDestNID";
    public static final String ID = "id";
    public static final String PRESENCE = "presence";
    public static final String JID = "jid";
    public static final String TITLE = "title";
    public static final String PAUSE = "Pause";
    public static final String STARTREQUEST = "StartRequest";
    public static final String STOPREQUEST = "StopRequest";
    public static final String ABORTREQUEST = "AbortRequest";
    public static final String TIMELINEUPDATEREQUEST = "TimelineUpdateRequest";
    public static final String MEDIASOURCEUPDATEREQUEST = "MediaSourceUpdateRequest";
    public static final String START = "Start";
    public static final String STOP = "Stop";
    public static final String DELETE = "Delete";
    public static final String ABORT = "Abort";
    public static final String RESTART = "Restart";
    public static final String WATCH = "Watch";
    public static final String UNWATCH = "UnWatch";
    public static final String NOTIFYREQUEST = "NotifyRequest";
    public static final String ACTIONREQUEST = "ActionRequest";
    public static final String ACTIONRESPONSE = "ActionResponse";
    public static final String SOURCEEVENT = "SourceEvent";
    public static final String RELAYEVENT = "RelayEvent";
    public static final String DESTINATIONEVENT = "DestEvent";
    public static final String SSEVENT = "SSEvent";
    public static final String STREAMEVENT = "StreamEvent";
    public static final String NOTIFYRESPONSE = "NotifyResponse";
    public static final String AGENTEVENT = "AgentEvent";
    public static final String TIMELINEEVENT = "TimeLineEvent";
    public static final String SYSTEMEVENT = "SystemEvent";

    // Stream States
    public static final int STREAM_STOPPED = 0;
    public static final int WAITING_FOR_XAUTH = 1;
    public static final int WAITING_FOR_CONTACTJID = 2;
    public static final int WAITING_FOR_DEST = 3;
    public static final int WATCH_SENTTO_DEST = 4;
    public static final int WAITING_FOR_SOURCE = 5;
    public static final int WAITING_FOR_STREAMURL = 6;
    public static final int WAITING_FOR_STREAMSTARTED = 7;
    public static final int STREAM_STARTED = 8;
    public static final int STREAM_PAUSED = 9;
    public static final int DENIED_XAUTH = 10;
    public static final int REVOKED_XAUTH = 11;
    public static final int END_OF_MEDIA = 12;
    public static final int STREAM_DELETED = -1;

    public static final int SETUP_ERROR = 201;
    public static final int START_ERROR = 202;
    public static final int SOURCE_ERROR = 203;
    public static final int DEST_ERROR = 205;    

    //Destination Event Codes
    public static final int RECORDING_STARTED = 101;
    public static final int RECORDING_STOPPED = 102;
    public static final int MEDIA_STORE_ERROR = 103;  
    public static final int RECORDING_ERROR = 104;  

    //Source Event Codes
    public static final int RELAY_SOURCE_STARTED = 110;
    public static final int STREAM_START_TC = 111;
    public static final int END_OF_MEDIA_REACHED = 112;
    public static final int SOURCE_UNAVAILABLE = 113;
    public static final int SOURCE_AVAILABLE = 114;
    public static final int SOURCE_DISCONNECTED = 115;
    public static final int SOURCE_CONNECTED = 116;
    public static final int DEST_DISCONNECTED = 117;
    public static final int DISK_QUOTA_EXCEEDED = 118;
    
    //SourceEvent Error Codes - starts with 200
    public static final int MEDIA_ERROR = 204;
    
    
   /* //New event codes for book mark updates
    public static final int ADD_BOOKMARK = 16;    
    public static final int UPDATE_BOOKMARK = 17;
    public static final int DELETE_BOOKMARK = 18;*/
    
    public static final int UNKNOWN_EVENT = 100;
    


    // Track PVR Actions
    public static final String TRACK_PAUSE = "Pause";
    public static final String TRACK_PAUSEAT = "PauseAt";
    public static final String TRACK_RESUME = "Resume";
    public static final String TRACK_PLAY = "Play";
    public static final String TRACK_RESUMEFROM = "ResumeFrom";
    public static final String TRACK_RESUMEFROMMEDIATC = "ResumeFromMediaTC";
    public static final String TRACK_CATCHUP = "Catchup";
    public static final String TRACK_SKIP_FORWARD = "SkipForward";
    public static final String TRACK_SKIP_BACKWARD = "SkipBackward";

    // Track States
    public static final String TRACK_STARTED_CURRENT = "STARTED_CURRENT";
    public static final String TRACK_STOPPED_CURRENT = "STOPPED_CURRENT";
    public static final String TRACK_STARTED_LOOKBACK = "STARTED_LOOKBACK";
    public static final String TRACK_STOPPED_LOOKBACK = "STOPPED_LOOKBACK";

    // Source PVR Actions
    public static final String SOURCE_STARTOUTPUT = "StartOutput";
    public static final String SOURCE_STOPOUTPUT = "StopOutput";
    public static final String SOURCE_SWITCHLOOKBACK = "SwitchLookback";
    public static final String SOURCE_SWITCHCURRENT = "SwitchCurrent";
    public static final String SOURCE_UPDATEOFFSET = "UpdateOffset";
    
    public static final String INPUT_ENCODER = "ENCODER";
    public static final String INPUT_RELAY = "RELAY";
    public static final String INPUT_STREAM = "INPUT_STREAM";
    public static final String INPUT_MEDIAFILE = "MEDIAFILE";
    
    public static final String SHARED = "SHARED";
    public static final String DEDICATED = "DEDICATED";
    
    public static final String OUTPUT_STREAM = "OUTPUT_STREAM";
    public static final String OUTPUT_RECORD = "RECORD";
    public static final String OUTPUT_PVR = "PVR";
    public static final String OUTPUT_DECODER = "OutputDecoder";
    public static final String OUTPUT_IMAGE_WRITER = "OutputImageWriter";

    public static final String TRUE = "True";
    public static final String FALSE = "False";

    //Get Time Stamp const
    public static final String GETCURRENTTIMESTAMPREQUEST = "GetCurrentTimeStampRequest";
    public static final String GETOFFSETATTIMECODEREQUEST = "GetOffsetAtTimecodeRequest";
    public static final String GETTIMECODEATOFFSETREQUEST = "GetTimeCodeAtOffsetRequest";

    //device Instance Const
    public static final String V2D_NETWORK_DEVICE_INSTANCE = "V2DStreamRxInstance";
    public static final String DMS_RECORDED_DEVICE_INSTANCE = "StreamWriterInstance";
    public static final String DMS_MEDIA_DEVICE_INSTANCE = "StreamReaderInstance";
    public static final String V2D_STREAM_TX_INSTANCE = "V2DStreamTxInstance";
    public static final String V2D_STREAM_RX_INSTANCE = "V2DStreamRxInstance";
    public static final String IMAGE_WRITER_INSTANCE = "ImageWriterInstance";
    public static final String V2D_ENCODER_DEVICE_INSTANCE = "V2DEncoderInstance";
    public static final String V2D_DECODER_DEVICE_INSTANCE = "V2DDecoderInstance";
    public static final String SRC_PORT_DEVICE_INPUT_INSTANCE = "DeviceStreamingServiceEncoderInstanceNID";
    public static final String SRC_PORT_DEVICE_OUPUT_INSTANCE = "DeviceStreamingServiceTransmitInstanceNID";
    public static final String RELAY_PORT_INPUT_INSTANCE = "DeviceStreamingServiceReceiveInstanceNID";
    public static final String RELAY_PORT_OUTPUT_INSTANCE = "DeviceStreamingServiceTransmitInstanceNID";
    public static final String STORE_PORT_READER_INPUT_INSTANCE = "DeviceStreamingServiceReaderInstanceNID";
    public static final String STORE_PORT_WRITER_INPUT_INSTANCE = "DeviceStreamingServiceReceiveInstanceNID";
    public static final String STORE_PORT_WRITER_OUTPUT_INSTANCE = "DeviceStreamingServiceWriterInstanceNID";
    public static final String STORE_PORT_READER_OUTPUT_INSTANCE = "DeviceStreamingServiceTransmitInstanceNID";

    //resource Instance Const
    public static final String MEDIASTREAM_RESOURCE = "MediaStreamResource";

    //StreamingServer Instances type
    public static final String V2D_NETWORK_PORT = "V2DNetworkPort";
    public static final String DMS_RECORDER_PORT = "DMSRecorderPort";    
    public static final String DMS_MEDIA_PORT = "DMSMediaPort";
    public static final String V2D_DECODER_PORT = "V2DDecoderPort";
    public static final String RTP_NETWORK_PORT = "RTPNetworkPort";
    public static final String RTP_DECODER_PORT = "RTPDecoderPort";
    public static final String UDP_NETWORK_PORT = "UDPNetworkPort";
    public static final String UDP_DECODER_PORT = "UDPDecoderPort";
    public static final String MPEGTS_NETWORK_PORT = "MPEGTSNetworkPort";
    public static final String MPEGTS_DECODER_PORT = "MPEGTSDecoderPort";
    
    public static final String V2D_ENCODER_PORT = "V2DEncoderPort";
    public static final String V2D_HARDWARE_DECODER_PORT  = "V2DHardWarePort"; //ImageWriterPort
    public static final String IMAGE_WRITER_PORT  = "ImageWriterPort";
    public static final String DVI_ENCODER_PORT = "DVIEncoderPort";
    public static final String DVI_TRANSPORT_PORT = "DVITransportPort";
    public static final String DVI_DECODER_PORT  = "DVIDecoderPort";
    public static final String DVI_HARDWARE_DECODER_PORT  = "DVIHardWarePort";

    
    public static final String DELETER_PORT = "DeleterPort";
    

    // Streaming Server Event Constants
    public static final String SSINSTANCE_STARTED = "Started";
    public static final String SSINSTANCE_STOPPED = "Stopped";
    public static final String ERROR_STOPPED = "Error Stopped";
    public static final String SSINSTANCE_CONNECTED = "Connected";
    public static final String SSINSTANCE_DISCONNECTED = "Disconnected";
    public static final String SSINSTANCE_END_OF_MEDIA = "EndOfMedia";
    public static final String SSINSTANCE_CONNECTED_STARTED = "Connected Started";
    public static final String SSINSTANCE_CONNECTED_STOPPED = "Connected Stopped";
    public static final String SSINSTANCE_ERRORS = "Error";
    public static final String SSEVENT_PROGRESS = "Progress";
    public static final String DISK_QUOTA_EXCEEDED_EVENT = "DiskQuotaExceeded";


    public static final String YES = "yes";
    public static final String NO = "no";
    public static final String RELAY_CONNECTION = "RELAY_CONNECTION";
        public static final String RELAY_ROOM = "RELAY";
    //source events Status Constants    
    public static final int STREAMINGSERVEREVENTS = 201;
    
    //recording constants
    public static final String MEDIACLIP = "MediaClip";
    public static final String PVRMEDIACLIP = "PVRMediaClip";
    public static final String MEDIAGROUP = "MediaGroup";
    public static final String MEDIAIMAGE = "MediaImage";
    public static final String MEDIAGROUPID = "MediaGroupID";    
    public static final String CLIPID = "ClipID";    
    public static final String PLAYLIST = "Playlist";
    
    
    public static final String COMPUTERELAYUSINGPOLICY = "ComputeRelayUsingPolicy";
    public static final String DEFAULT_TRIMLENGTH = String.valueOf(4 * 3600 * 1000);
    public static final String DEFAULT_DEST_UDP_START_PORT = "0";
    //public static final String DEFAULT_TRIMLENGTH = String.valueOf(60000);  //testing
    
    public static final String DEFAULT_RXPORTELEMENTID = "DefaultRxPort";
    public static final String DEFAULT_TXPORTELEMENTID = "DefaultTxPort";
    public static final String STREAMSRC = "StreamSrc";
    public static final String STREAMDST = "StreamDst";

    
    // StreamType
    public static final String V2D = "V2D";
    public static final String RTP = "RTP";
    public static final String MPEGTS = "MPEGTS";
    public static final String UDP = "UDP";
    public static final String HTTP = "HTTP";
    public static final String PROTOCOL_UDP = "udp";
    public static final String DVI = "DVI";
    public static final String SNAPSHOT = "SNAPSHOT";

    public static final String PROTOCOL_FILE = "file";
    public static final String PROTOCOL_HTTP = "http";

    public static final String SDPTEMPLATE = "SDP-Template-Src";
    
    //profile elemenet
    public static final String MEDIA_STREAM_PROFILE_INFO = "MediaStreamProfileInfo";
    //profile elemenet
    public static final String MEDIA_STREAM_PROFILE = "MediaStreamProfile";
  
    
    //Media Stream File Request
    public static final String ADD_DEST_FILE_REQUEST = "AddDestFileRequest";
    public static final String DELETE_DEST_FILE_REQUEST = "DeleteDestFileRequest";
    public static final String UPDATE_DEST_FILE_METADATA_REQUEST = "UpdateDestFileMetaDataRequest";
    public static final String ADD_BOOKMARK_REQUEST = "AddBookMarkRequest";    
    public static final String UPDATE_BOOKMARK_REQUEST = "UpdateBookMarkRequest";
    public static final String DELETE_BOOKMARK_REQUEST = "DeleteBookMarkRequest";
    public static final String GET_SRC_MEDIA_FILE_TRACK_REQUEST = "GetSrcMediaFileTrackRequest";
    public static final String UPDATE_DEST_FILE_PROFILE_REQUEST ="UpdateDestFileProfileRequest";
    
    public static final String MediaStreamDstPortResource = "MediaStreamDstPortResource";
    public static final String MediaStreamSrcPortResource = "MediaStreamSrcPortResource";
    public static final String MediaStreamIOPortResource = "MediaStreamIOPortResource";
    public static final String MediaStreamRelayPortResource = "MediaStreamRelayPortResource";
    public static final String AVAILABLE = "available";
    public static final String UNAVAILABLE = "unavailable";
    public static final String STREAM_STATE_XPATH = "Status/state";
    public static final String STREAM_RELAYSTATE_XPATH = "Status/relayState";
    public static final String STREAM_SRC_PRESENCE_XPATH = "//MediaSource";
    public static final String PRESENCE_ATTRIBUTE_XPATH = "presence";
    public static final String STREAM_DST_PRESENCE_XPATH = "//MediaDest";
    public static final String THUMBNAIL_PATH = "/data/DSS/data/portal/site/thumbnails/";
    public static final String LOCAL1 = "LOCAL1"; 
    public static final String LOCAL2 = "LOCAL2";
    public static final String GLOBAL = "GLOBAL";
    public static final String DNSNAME = "DNSNAME";
    public static final String SPLASHSCREEN_URL = "v2d://0.0.0.0/port=6060?bandwidth=0?avoption=Both?ismulticast=true?enablekbm=false?authscheme=TOKEN?authdata=1234";
    public static boolean ENABLE_LOG = false;
   
    public static final long TOO_CLOSE_TO_LIVE_LIMIT = 2000;
    
    
    public static final String ACTION_DATA = "actionData";
	public static final String AUTHORIZE = "Authorize";
	public static final String TOKEN = "TOKEN";
	
	public static final String FFCREATOR_TOOL = "/home/onair/TV/bin/FFCreator";
    
}
    
    

