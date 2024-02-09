package com.ipvs.mediaroomservice.xsd;

import com.ipvs.mediaroomservice.impl.MediaRoomRequestTypes;

public class ControlMediaSourceRequest implements MRMessageI {
	/**
	 * @param mediaNIDList		the media Node IDs to control
	 * @param controlAction	the control Action
	 * @param controlArgs	control arguments to use for the control Action
	 */
	
	public String[] mediaNIDList;
	public String action;
	public String args;
	
	public int getRequestType() { return MediaRoomRequestTypes.CONTROLMEDIA; }
}
