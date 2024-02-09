package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

public class AddViewerRequest implements MRMessageI {
	/**
	 * @param roomNID	room Node ID to add the viewer to
	 * @param destNID	dest Node ID to add as the viewer
	 * @param viewerDescription	the description to be added to this viewer instance
	 */
	public String roomNID;
	public String destNID;
	public String description;
	
	public int getRequestType() { return MediaRoomRequestTypes.ADDVIEWER; }

	public String getRoomNID() {
		return roomNID;
	}

	public void setRoomNID(String roomNID) {
		this.roomNID = roomNID;
	}

	public String getDestNID() {
		return destNID;
	}

	public void setDestNID(String destNID) {
		this.destNID = destNID;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}

}
