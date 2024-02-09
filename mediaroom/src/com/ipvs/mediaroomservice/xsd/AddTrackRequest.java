package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

public class AddTrackRequest implements MRMessageI {
	/**
	 * @param roomNID	the room Node ID
	 * @param description	A text description of the track
	 * @param trackType	indicates whether it is public or private track
	 * @param defaultTrackNID the trackNID of the default track to fall back to
	 * @param defaultMediaSourceNID the mediaSourceNID of the default media to fall back to
	 */
	public String roomNID;
	public String description;
	public int trackType;
	public String defaultTrackNID;
	public String defaultMediaSourceNID;
	
	public int getRequestType() { return MediaRoomRequestTypes.ADDTRACK; }

	public String getRoomNID() {
		return roomNID;
	}

	public void setRoomNID(String roomNID) {
		this.roomNID = roomNID;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public int getTrackType() {
		return trackType;
	}

	public void setTrackType(int trackType) {
		this.trackType = trackType;
	}

	public String getDefaultTrackNID() {
		return defaultTrackNID;
	}

	public void setDefaultTrackNID(String defaultTrackNID) {
		this.defaultTrackNID = defaultTrackNID;
	}

	public String getDefaultMediaSourceNID() {
		return defaultMediaSourceNID;
	}

	public void setDefaultMediaSourceNID(String defaultMediaSourceNID) {
		this.defaultMediaSourceNID = defaultMediaSourceNID;
	}
}
