package com.ipvs.mediaroomservice.datamodel;

import com.ipvs.common.MRConst;
import com.ipvs.mediaroomservice.xsd.MediaRoomConfigResourceFilter;
/** 
 * @author miyer
 *
 */
public class MediaSource extends MediaRoomObject {
	public boolean resolved;
	public String sourcePortNID;
	public MediaRoomConfigResourceFilter filter;
	public String defaultTrackNID;
	public String defaultMediaNID;
	public long startTimeCode;
	public long startOffset;
	public int lastAction;
	public long lastActionTimeStamp;
	public long lastActionTimeCode;
	public long lastActionOffset;
	public String streamRootNID;
	public Stream[] streamList;

    public MediaSource(String pRoomNID, String pDescription,
    		String pSourcePortNID, long pStartTimeCode, long pStartOffset) {
    	super(pRoomNID, pDescription);
    	init();
    	resolved = true;
    	sourcePortNID = pSourcePortNID;
    	startTimeCode = pStartTimeCode;
    	startOffset = pStartOffset;
    }
    
    public MediaSource(String pRoomNID, String pDescription,
    		MediaRoomConfigResourceFilter pFilter) {
    	super(pRoomNID, pDescription);
    	init();
    	resolved = false;
    	filter = pFilter;
    }
    
    public MediaSource(String pRoomNID, String pDescription,
    		String pSourcePortNID, long pStartTimeCode, long pStartOffset,
    		String pDefaultTrackNID, String pDefaultMediaNID) {
    	super(pRoomNID, pDescription);
    	init();
    	resolved = true;
    	sourcePortNID = pSourcePortNID;
    	startTimeCode = pStartTimeCode;
    	startOffset = pStartOffset;
    	defaultTrackNID = pDefaultTrackNID;
    	defaultMediaNID = pDefaultMediaNID;
    }

    private void init() {
    	sourcePortNID = MRConst.NULL;
    	startTimeCode = 0;
    	startOffset = 0;
    	resolved = true;
    	defaultTrackNID = MRConst.NULL;
    	defaultMediaNID = MRConst.NULL;
    	lastAction = 0;
    	lastActionTimeStamp = 0;
    	lastActionTimeCode = 0;
    	lastActionOffset = 0;
    	streamRootNID = MRConst.NULL;
    }    

	public int getType() { return MEDIA_TYPEID; }
}
