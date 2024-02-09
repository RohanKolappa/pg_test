package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

public class AddMediaSourceRequest implements MRMessageI {
	/**
	 * @param roomNID	room Node ID to add the media to
	 * @param trackNID	track Node ID to add the media to within the room
	 * @param sourceNID	source Node ID to add as the media
	 * @param mediaDescription the description to be added to the media instance
	 */
	
	public String roomNID;
	public String trackNID;
	public String description;

	public String sourceNID;
	public long startTimeCode;
	public long startOffset;
	
	public int getRequestType() { return MediaRoomRequestTypes.ADDMEDIA; }

	public String getRoomNID() {
		return roomNID;
	}

	public void setRoomNID(String roomNID) {
		this.roomNID = roomNID;
	}

	public String getTrackNID() {
		return trackNID;
	}

	public void setTrackNID(String trackNID) {
		this.trackNID = trackNID;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public String getSourceNID() {
		return sourceNID;
	}

	public void setSourceNID(String sourceNID) {
		this.sourceNID = sourceNID;
	}

	public long getStartTimeCode() {
		return startTimeCode;
	}

	public void setStartTimeCode(long startTimeCode) {
		this.startTimeCode = startTimeCode;
	}

	public long getStartOffset() {
		return startOffset;
	}

	public void setStartOffset(long startOffset) {
		this.startOffset = startOffset;
	}
}