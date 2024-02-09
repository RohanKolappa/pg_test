package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

public class AddMediaSourceConfigRequest extends AddMediaSourceRequest  {
	public String roomConfigNID;
	public MediaRoomConfigResourceFilter filter;
	public int getRequestType() { return MediaRoomRequestTypes.ADDMEDIACONFIG; }
}
