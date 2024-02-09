package com.ipvs.mediaroomservice.datamodel;
import com.ipvs.common.MRConst;

/**
 * The Track object within a Room
 * 
 * @author miyer
 *
 */
public class Track  extends MediaRoomObject {
	public int trackType;
	public String mediaRootNID;
	public MediaSource[] mediaList;
	
	public static final int DEFAULT_TRACKTYPE=0;
	
    public Track(String pRoomNID, String pDescription, int pTrackType) {
    	super(pRoomNID, pDescription);
    	trackType = pTrackType;
    	mediaRootNID = MRConst.NULL;
    }
    
	public int getType() { return TRACK_TYPEID; }
}
