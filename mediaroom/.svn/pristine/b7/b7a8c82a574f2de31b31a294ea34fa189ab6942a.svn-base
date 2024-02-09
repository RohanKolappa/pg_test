package com.ipvs.mediaroomservice.datamodel;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRNodeI;

public abstract class MediaRoomObject implements MRNodeI {
	public static final int ROOMCONFIG_TYPEID = 101;
	public static final int ROOM_TYPEID = 102;
	public static final int TRACK_TYPEID = 103;
	public static final int MEDIA_TYPEID = 104;
	public static final int VIEWER_TYPEID = 105;
	public static final int STREAM_TYPEID = 106;
	public static final int AUTHORIZATION_TYPEID = 107;
	
	public static final int STATE_ADDED = 101;
	public static final int STATE_STARTED = 102;
	public static final int STATE_STOPPED = 103;
	public static final int STATE_EXCEPTION = 501;
	
	
	protected String NID;
	public String roomNID;
	public String description;
	public String authNID;
	public int state;	// One of ADDED, STARTED or STOPPED

    
    public MediaRoomObject(String pRoomNID, String pDescription) {
    	NID = null;
    	roomNID = pRoomNID;
    	description = pDescription;
    	authNID = MRConst.NULL;
    	state = STATE_ADDED;
    }
    
    public abstract int getType();

	public String getNID() {
		return NID;
	}

	public void setNID(String nid) {
		NID = nid;
	}
}
