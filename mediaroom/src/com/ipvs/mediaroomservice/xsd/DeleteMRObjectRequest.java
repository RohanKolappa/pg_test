package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

public class DeleteMRObjectRequest implements MRMessageI {
	/**
	 * @param nodeID	the NodeID for the Viewer/Media/Track/Room/RoomConfig
	 */
	public String objectNID;
	
	public int getRequestType() { return MediaRoomRequestTypes.DELETEMROBJECT; }
}
