package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

/**
 * The request to create a media room
 * @author miyer
 *
 */
public class CreateMediaRoomRequest implements MRMessageI {
	/**
	 * @param roomConfigNID the roomConfig node id
	 * @param description the room description
	 */
	public String roomConfigNID;
	public String description;
	
	public int getRequestType() { return MediaRoomRequestTypes.CREATEMEDIAROOM; }

	public String getRoomConfigNID() {
		return roomConfigNID;
	}

	public void setRoomConfigNID(String roomConfigNID) {
		this.roomConfigNID = roomConfigNID;
	}

	public String getDescription() {
		return description;
	}

	public void setDescription(String description) {
		this.description = description;
	}
}
