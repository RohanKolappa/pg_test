/*
 *
 */
package com.barco.device.agent;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;

import com.barco.device.common.DeviceFrameworkI;

/**
 * The Interface MediaAgentI.
 *
 * see https://wiki.barco.com/display/ipvs/SC-Feature+-+Connection+Management#
 */
public interface MediaAgentI  extends AgentI{

    // Stream States
    /** The Constant STREAM_STOPPED. */
    public static final int STREAM_STOPPED = 0;

    /** The Constant WAITING_FOR_XAUTH. */
    public static final int WAITING_FOR_XAUTH = 1;

    /** The Constant WAITING_FOR_CONTACTJID. */
    public static final int WAITING_FOR_CONTACTJID = 2;

    /** The Constant WAITING_FOR_DEST. */
    public static final int WAITING_FOR_DEST = 3;

    /** The Constant WATCH_SENTTO_DEST. */
    public static final int WATCH_SENTTO_DEST = 4;

    /** The Constant WAITING_FOR_SOURCE. */
    public static final int WAITING_FOR_SOURCE = 5;

    /** The Constant WAITING_FOR_STREAMURL. */
    public static final int WAITING_FOR_STREAMURL = 6;

    /** The Constant WAITING_FOR_STREAMSTARTED. */
    public static final int WAITING_FOR_STREAMSTARTED = 7;

    /** The Constant STREAM_STARTED. */
    public static final int STREAM_STARTED = 8;

    /** The Constant STREAM_PAUSED. */
    public static final int STREAM_PAUSED = 9;

    /** The Constant DENIED_XAUTH. */
    public static final int DENIED_XAUTH = 10;

    /** The Constant REVOKED_XAUTH. */
    public static final int REVOKED_XAUTH = 11;

    /** The Constant END_OF_MEDIA. */
    public static final int END_OF_MEDIA = 12;

    /** The Constant STREAM_DELETED. */
    public static final int STREAM_DELETED = -1;

    /** The Constant SETUP_ERROR. */
    public static final int SETUP_ERROR = 201;

    /** The Constant START_ERROR. */
    public static final int START_ERROR = 202;

    /** The Constant SOURCE_ERROR. */
    public static final int SOURCE_ERROR = 203;

    /** The Constant DEST_ERROR. */
    public static final int DEST_ERROR = 205;

    //Destination Event Codes
    /** The Constant RECORDING_STARTED. */
    public static final int RECORDING_STARTED = 101;

    /** The Constant RECORDING_STOPPED. */
    public static final int RECORDING_STOPPED = 102;

    /** The Constant MEDIA_STORE_ERROR. */
    public static final int MEDIA_STORE_ERROR = 103;

    /** The Constant RECORDING_ERROR. */
    public static final int RECORDING_ERROR = 104;

    //Source Event Codes
    /** The Constant RELAY_SOURCE_STARTED. */
    public static final int RELAY_SOURCE_STARTED = 110;

    /** The Constant STREAM_START_TC. */
    public static final int STREAM_START_TC = 111;

    /** The Constant END_OF_MEDIA_REACHED. */
    public static final int END_OF_MEDIA_REACHED = 112;

    /** The Constant SOURCE_UNAVAILABLE. */
    public static final int SOURCE_UNAVAILABLE = 113;

    /** The Constant SOURCE_AVAILABLE. */
    public static final int SOURCE_AVAILABLE = 114;

    /** The Constant SOURCE_DISCONNECTED. */
    public static final int SOURCE_DISCONNECTED = 115;

    /** The Constant SOURCE_CONNECTED. */
    public static final int SOURCE_CONNECTED = 116;

    /** The Constant DEST_DISCONNECTED. */
    public static final int DEST_DISCONNECTED = 117;

    /** The Constant DISK_QUOTA_EXCEEDED. */
    public static final int DISK_QUOTA_EXCEEDED = 118;

    /** The Constant TIMELINEUPDATEEVENT. */
    public static final int TIMELINEUPDATEEVENT = 119;

    //SourceEvent Error Codes - starts with 200
    /** The Constant MEDIA_ERROR. */
    public static final int MEDIA_ERROR = 204;

    // Presence
    /** The Constant MEDIAPRESENCEEVENTDATA. */
    public static final String MEDIAPRESENCEEVENTDATA = "MediaPresenceEventData";

	/** The Constant ROOMNID. */
	public static final String ROOMNID = "roomNID";

	/** The Constant JID. */
	public static final String JID = "jid";

	/** The Constant READY. */
	public static final String READY = "Ready";

	/** The Constant UNAVAILABLE. */
	public static final String UNAVAILABLE = "Unavailable";

	/** The Constant MEDIAROOM. */
	public static final String MEDIAROOM = "MediaRoom";

	/** The Constant TYPE. */
	public static final String TYPE = "type";

	/** The Constant VALUE. */
	public static final String VALUE = "value";

	// Event Level
	/** The Constant PUBSUB_SERVICE. */
	public static final String PUBSUB_SERVICE = "Pubsub";

    /** The Constant STREAMSTATUSEVENT. */
    public static final String STREAMSTATUSEVENT = "StreamStatusEvent";

	/** The Constant EVENT_LEVEL_OK. */
	public static final int EVENT_LEVEL_OK = 200;

	/** The Constant EVENT_LEVEL_ERROR. */
	public static final int EVENT_LEVEL_ERROR = 0;



	/**
	 * Start stream.
	 *
	 * @param deviceFramework
	 *            the device framework
	 * @param requestNID
	 *            the request nid
	 * @param streamNID
	 *            the stream nid
	 * @param streamDataDoc
	 *            For reference xml, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/objectXML/Mediaroom.xml">XML</a>.
	 *            Check Stream/Data element.
	 *            For reference xsd, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/xsd/mediaroom.mediaroomservice.datamodel.xsd">XSD</a>.
	 * @param mediaSourceDataDoc
	 *            For reference xml, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/objectXML/Mediaroom.xml">click here</a>
	 *            Check MediaSource/Data element.
	 *            For reference xsd, look here <a href="http://silsvn01.barco.com/svn/svnroot/model/trunk/xsd/mediaroom.mediaroomservice.datamodel.xsd">click here</a>
	 * @return the document
	 * @throws Exception
	 *             the exception
	 */
	public abstract Document startStream(DeviceFrameworkI deviceFramework, String requestNID,
			String streamNID, Document streamDataDoc, Document mediaSourceDataDoc) throws Exception ;

	/**
	 * Stop stream.
	 *
	 * @param streamNID
	 *            the stream nid
	 * @return the document
	 * @throws Exception
	 *             the exception
	 */
	public abstract Document stopStream(String streamNID) throws Exception;

	/**
	 * Gets the stream nid list by room nid.
	 *
	 * @param roomNID
	 *            the room nid
	 * @return the stream nid list by room nid
	 */
	public abstract ArrayList<String> getStreamNIDListByRoomNID(String roomNID);

	/**
	 * Gets the stream nid list by dest agent jid.
	 *
	 * @param destUserJID
	 *            the dest user jid
	 * @return the stream nid list by dest agent jid
	 */
	public abstract ArrayList<String> getStreamNIDListByDestAgentJID(String destUserJID);


	/**
	 * Gets the stream nid list by dest agent jid.
	 *
	 * @param destUserJID
	 *            the dest user jid
	 * @return the stream nid list by dest agent jid
	 * @throws Exception TODO
	 */
	public abstract List<String> getStreamNIDListByPortID(String portID) throws Exception;

	/**
	 * Adds the room nid.
	 *
	 * @param roomNID
	 *            the room nid
	 */
	public abstract void addRoomNID(String roomNID);

	/**
	 * Delete room nid.
	 *
	 * @param roomNID
	 *            the room nid
	 */
	public abstract void deleteRoomNID(String roomNID);
	public abstract ArrayList<String> getRoomNIDList();

	/**
	 * Start playing.
	 *
	 * @param deviceFramework
	 *            the device framework
	 * @param streamNID
	 *            the stream nid
	 * @param destPortElementID
	 *            the dest port element id
	 * @param streamURL
	 *            the stream url
	 * @throws Exception
	 *             the exception
	 */
	public void startPlaying(DeviceFrameworkI deviceFramework, String streamNID, String destPortElementID, String streamURL) throws Exception;

	/**
	 * Stop playing.
	 *
	 * @param streamNID
	 *            the stream nid
	 * @throws Exception
	 *             the exception
	 */
	public void stopPlaying(String streamNID) throws Exception;


	/**
	 * Start Posting Thumbnail.
	 *
	 * @param sourceNID
	 *            the source NID
	 * @param portId TODO
	 * @param publishURL
	 * 			  the publish URL
	 * @throws Exception
	 *             the exception
	 */
	public void startPostingThumbnail(String sourceNID, String portId, String publishURL) throws Exception;

	/**
	 * Stop Posting Thumbnail.
	 *
	 * @param sourceNID
	 *            the source NID
	 * @param portId TODO
	 * @param publishURL
	 * 			  the publish URL
	 * @throws Exception
	 *             the exception
	 */
	public void stopPostingThumbnail(String sourceNID, String portId) throws Exception;

	/**
	 * handle device config updates
	 * @param doc
	 * @throws Exception
	 */
	public void handleConfigUpdated(Document doc) throws Exception;

	/**
	 * handleTimeLineUpdate
	 *
	 * @param deviceFramework
	 *            the device framework
	 * @param doc
	 *            the timeline update doc
	 * @throws Exception
	 *             the exception
	 */
	public void handleTimelineUpdate(DeviceFrameworkI deviceFramework, Document doc) throws Exception;

	/**
	 * handleMediaSourceUpdate
	 *
	 * @param deviceFramework
	 *            the device framework
	 * @param doc
	 *            the media source update doc
	 * @throws Exception
	 *             the exception
	 */
	public void handleMediaSourceUpdate(DeviceFrameworkI deviceFramework, Document doc) throws Exception;


	/**
	 * This method handles stream stopped event coming from streaming server
	 * @param eventDoc
	 * @throws Exception
	 */
	public void handleStopped(Document eventDoc) throws Exception;

	/**
	 * This method releases the UDP start port for streamType using NetworkResource
	 * @param streamType
	 * @param destUDPStartPort
	 * @throws Exception
	 */

	public void releaseUDPStartPort(String destUDPStartPort, String streamType) throws Exception;
	/**
	 * This method returns the next available UDP start port using NetworkResource
	 * @param streamType
	 * @throws Exception
	 */
	public String updateNextUDPStartPort(String streamType) throws Exception;


	/**
	 * This method start the streaming engine objects if they are not started.
	 * @param eventDoc
	 * @throws Exception
	 */
	public void handleConnected(Document eventDoc) throws Exception;
}
