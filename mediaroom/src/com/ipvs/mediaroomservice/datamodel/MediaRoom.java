package com.ipvs.mediaroomservice.datamodel;
import com.ipvs.common.MRConst;

/**
 * The Room object which has pointers to all the media, viewers, streams, authorizations
 * @author miyer
 *
 */
public class MediaRoom extends MediaRoomObject {

	public String group;
	public String authRootNID;
	public String trackRootNID;
	public String viewerRootNID;
	public MediaRoomAuthorization[] authList;
	public Track[] trackList;
	public Viewer[] viewerList;

	
    public MediaRoom(String pDescription, String pGroup) {
    	super(MRConst.NULL, pDescription);
    	authRootNID = MRConst.NULL;
    	trackRootNID = MRConst.NULL;
    	viewerRootNID = MRConst.NULL;
    	group = pGroup;
    }

	public int getType() { return ROOM_TYPEID; }
}
