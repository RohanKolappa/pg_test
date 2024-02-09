package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

public class AddStreamConfigRequest extends AddStreamRequest {
	public String roomConfigNID;
	
	public int getRequestType() { return MediaRoomRequestTypes.ADDSTREAMCONFIG; }
	
}
