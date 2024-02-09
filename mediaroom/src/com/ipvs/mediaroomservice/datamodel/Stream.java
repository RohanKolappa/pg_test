package com.ipvs.mediaroomservice.datamodel;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRLog;

/**
 * Represnts the stream between media and a viewer
 * @author miyer
 *
 */
public class Stream extends MediaRoomObject{
	public String mediaNID;
	public String viewerNID;
	public String profileNID;
	public int action;
	public String actionRequestNID;
	public int actionLogLevel;
	public String sourcePortOwnerJID;
	public String destPortOwnerJID;
	public long streamStartTime;
	public String profileXML;
	
    public Stream(String pRoomNID, String pDescription,
    		String pMediaNID, String pViewerNID, String pProfileNID, 
    		String pSourcePortOwnerJID, String pDestPortOwnerJID,
    		String pProfileXML) {
    	super(pRoomNID, pDescription);
    	mediaNID = pMediaNID;
    	viewerNID = pViewerNID;
    	profileNID = pProfileNID;
    	sourcePortOwnerJID = pSourcePortOwnerJID;
    	destPortOwnerJID = pDestPortOwnerJID;
    	clearAction();
    	actionLogLevel = MRLog.ERROR;
    	streamStartTime = 0;
    	profileXML = pProfileXML;
    }
    
    public void setAction(int action, String requestNID, int logLevel) {
    	this.action = action;
    	this.actionLogLevel = logLevel;
    	this.actionRequestNID = requestNID;
    }
    
    public void clearAction() {
    	setAction(0, MRConst.NULL, MRLog.ERROR);
    }
    
    public int getAction() {
    	return this.action;
    }

    public int getActionLogLevel() {
    	return this.actionLogLevel;
    }

    public String getActionRequestNID() {
    	return this.actionRequestNID;
    }

	public int getType() { return STREAM_TYPEID; }
	
}

