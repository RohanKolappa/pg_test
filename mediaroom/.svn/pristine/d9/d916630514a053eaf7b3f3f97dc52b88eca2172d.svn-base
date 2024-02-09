package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

public class AddStreamRequest implements MRMessageI {
	/**
	 * @param viewerNID	viewer Node ID
	 * @param roomNID	room Node ID of the media
	 * @param mediaSourceNID	media Node ID within the room
	 * @param profile	profile which describes the various stream/port parameters
	 */
	
	public String roomNID;
	public String viewerNID;
	public String description;
	public String trackNID;
	public String mediaSourceNID;
	public String profileNID;
	public String profileXML;
	
	public String getProfileXML() {
		return profileXML;
	}

	public void setProfileXML(String profileXML) {
		this.profileXML = profileXML;
	}

	public int getRequestType() { return MediaRoomRequestTypes.ADDSTREAM; }

	public String getRoomNID() {
		return roomNID;
	}

	public void setRoomNID(String roomNID) {
		this.roomNID = roomNID;
	}

	public String getViewerNID() {
		return viewerNID;
	}

	public void setViewerNID(String viewerNID) {
		this.viewerNID = viewerNID;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

	public String getTrackNID() {
		return trackNID;
	}

	public void setTrackNID(String trackNID) {
		this.trackNID = trackNID;
	}

	public String getMediaSourceNID() {
		return mediaSourceNID;
	}

	public void setMediaSourceNID(String mediaSourceNID) {
		this.mediaSourceNID = mediaSourceNID;
	}

	public String getProfileNID() {
		return profileNID;
	}

	public void setProfileNID(String profileNID) {
		this.profileNID = profileNID;
	}

}
