package com.ipvs.mediaroomservice.datamodel;
import com.ipvs.common.MRConst;
import com.ipvs.mediaroomservice.xsd.MediaRoomConfigResourceFilter;
/**
 * Represnts the Viewer
 * @author miyer
 *
 */
public class Viewer extends MediaRoomObject {
	public boolean resolved;
	public	String destPortNID;
	public MediaRoomConfigResourceFilter filter;
	
    public Viewer(String pRoomNID, String pDescription,
    		String pDestPortNID) {
    	super(pRoomNID, pDescription);
    	init();
    	resolved = true;
    	destPortNID = pDestPortNID;
    }
    
    public Viewer(String pRoomNID, String pDescription,
    		MediaRoomConfigResourceFilter pFilter) {
    	super(pRoomNID, pDescription);
    	resolved = false;
    	filter = pFilter;
    }
    
    private void init() {
    	destPortNID = MRConst.NULL;
    	resolved = true;
    }
    
	public int getType() { return VIEWER_TYPEID; }
}

