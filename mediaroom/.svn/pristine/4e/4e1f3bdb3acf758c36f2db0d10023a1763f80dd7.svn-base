package com.ipvs.mediaroomservice.impl;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.MRRequestI;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;

public interface MediaRoomObserverI {

	public void handleStreamStatusChanged(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, 
    		String roomNID, Element streamElement, Element sourceElement);
	public void handlePresenceChanged(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, 
    		String roomNID, Element mediaElement);
	public void handleRoomDeleted(XMPPI xmpp, MRRequestI request, MRLog log, MRDB db, 
    		String roomNID, Document roomDoc) throws Exception;
	public Element getData(MRRequestI request, XMPPI xmpp, MRLog log, MRDB db, 
			String roomNID) throws Exception;
    public void handleTimeLineChanged(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, 
    		String roomNID, String trackNID, Document trackDataDoc, String requestedAction);
    public void handleStreamAdded(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, Element streamElement, 
    		Element sourceElement)throws MRException, Exception;
    public void handleStreamDeleted(XMPPI xmpp, MRRequest request, MRLog log, MRDB db, String roomNID, 
    		Element streamElement, Element sourceElement, int state)throws MRException, Exception;

}
