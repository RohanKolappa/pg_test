package com.ipvs.mediaroomservice.impl;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;

public class MediaTimeline {


    private static Element gMediaTimelineTemplate = null;
    Element mediaTimelineElement;

    MediaTimeline(Element mediaTimelineElement) throws MRException,Exception{
    	this.mediaTimelineElement = mediaTimelineElement;
    }

	public String getId() {
		return mediaTimelineElement.attributeValue("id");
	}

	public String getLastState() {
		return mediaTimelineElement.element("Last").attributeValue("lastState");
	}
	
	public String getLastAction() {		
		return mediaTimelineElement.element("Last").attributeValue("action");
	}

	public String getLastSourceAction() {		
		return mediaTimelineElement.element("Last").attributeValue("sourceAction");
	}

	public long getLastOffset() { 
		return Long.valueOf(mediaTimelineElement.element("Last").attributeValue("Offset"));
	}
	
	public long getStartWallclock() { 
		return Long.valueOf(mediaTimelineElement.element("Start").attributeValue("WC"));
	}

	public long getStartOffset() { 
		return Long.valueOf(mediaTimelineElement.element("Start").attributeValue("Offset"));
	}

	
	public long getLastActionWallClock() {
		return Long.valueOf(mediaTimelineElement.element("Last").attributeValue("WC"));
	}

	public double getLastTimescale() {
		return Double.valueOf(mediaTimelineElement.element("Last").attributeValue("TimeScale"));
	}

	public String getLastRequestedAction() {
		return mediaTimelineElement.element("Last").attributeValue("requestedAction");
	}

    
	public static void updateMediaTimelineElement(Element mediaTimelineElement,
			long WC, long offset, long tc, double timescale, String action, String lastState, String requestedAction, String sourceAction) throws Exception {
		Element last = mediaTimelineElement.element("Last");
		last.addAttribute("WC", String.valueOf(WC));
		last.addAttribute("TimeScale", String.valueOf(timescale));
		last.addAttribute("TC", String.valueOf(tc));
		last.addAttribute("Offset", String.valueOf(offset));
		last.addAttribute("lastState", lastState);
		last.addAttribute("action", action);
		last.addAttribute("requestedAction", requestedAction);
		last.addAttribute("sourceAction", sourceAction);
		last.addAttribute("ssTC", "");
		last.addAttribute("ssWC", "");
	}
	
	public static void adjustStreamMediaTimeline(Element mediaTimelineElement,
			long ssTC, long ssWC, MRLog log) throws Exception {
		Element last = mediaTimelineElement.element("Last");
		String tc = last.attributeValue("TC");
		String wc = last.attributeValue("WC");
		last.addAttribute("ssTC", String.valueOf(ssTC));
		last.addAttribute("ssWC", String.valueOf(ssWC));
        log.addInfo("StreamTable:adjustStreamMediaTimeline from WC=" + wc + " > " + ssWC +  " TC=" + tc + " > " + ssTC);
	}

	
    public static Element createMediaTimelineElement(MRLog log) throws Exception {
    	if(gMediaTimelineTemplate == null) {
    		Document streamInstance = RoomDB.getStreamInstanceXML(log);
    		gMediaTimelineTemplate = (Element)MRXMLUtils.getElementXpath(streamInstance, "//MediaTimeline").detach();
    	}
    	return (Element)gMediaTimelineTemplate.clone();
    }

    public static Element createTimelineUpdate(MediaTimeline mediaTimeline, Element streamNIDList, ArrayList<String>  sourceAgentJIDList,
    		String roomNID, boolean sync, String syncSourceUUID,String syncSourceParentDirNID, String syncAgentJID, long syncTC) throws Exception {
    	Element timelineUpdate = MRXMLUtils.stringToElement("<timelineupdate/>");    	
        if(sync) {
        	timelineUpdate.addAttribute("sync", MRConst.TRUE);
        	timelineUpdate.addAttribute("syncSourceUUID", syncSourceUUID);
        	timelineUpdate.addAttribute("syncSourceParentDirNID", syncSourceParentDirNID);
        	timelineUpdate.addAttribute("syncAgentJID", syncAgentJID);
        	timelineUpdate.addAttribute("syncTC", String.valueOf(syncTC));
        	timelineUpdate.addAttribute("roomNID", roomNID);
        	StringBuffer syncSourceAgentJIDListStr = new StringBuffer();
        	for(String s: sourceAgentJIDList)
        		syncSourceAgentJIDListStr.append(s + ",");
        	timelineUpdate.addAttribute("syncSourceAgentJIDList", syncSourceAgentJIDListStr.toString());
        }
        else
        	timelineUpdate.addAttribute("sync", MRConst.FALSE);
        Element timelineElement = (Element)mediaTimeline.mediaTimelineElement.clone();
        timelineElement.setName("MediaTimeline");
        timelineUpdate.add(timelineElement);
        timelineUpdate.add(streamNIDList);
        return timelineUpdate;
    }

    public static Element createTimelineActionData(Element timelineUpdate) throws Exception {
    	Element actiondata = MRXMLUtils.stringToElement("<actiondata/>");
    	actiondata.add((Element)timelineUpdate.clone());
    	return actiondata;
    }

    public static Element createTimelineActionDataFromList(Element timelineUpdateList) throws Exception {
    	Element actiondata = MRXMLUtils.stringToElement("<actiondata/>");
    	// System.out.println("MediaTimeline:createTimelineActionDataFromList doc=" + MRXMLUtils.elementToString(timelineUpdateList));
    	List<?> list = MRXMLUtils.getListXpath(timelineUpdateList, ".//timelineupdate");
    	for(Object o: list) {
    		Element timelineUpdate = (Element)o;
    		actiondata.add((Element)timelineUpdate.clone());
    	}
    	return actiondata;
    }

    
    public static long getOffsetFromTimelineUpdate(Element timelineUpdate) throws Exception {
    	Element last = MRXMLUtils.getElementXpath(timelineUpdate, ".//Last");
        long offset = Long.parseLong(last.attributeValue("Offset"));
        return offset;
    }

    public static void setOffsetInTimelineUpdate(Element timelineUpdate, long offset) throws Exception {
    	Element last = MRXMLUtils.getElementXpath(timelineUpdate, ".//Last");
    	last.addAttribute("Offset", String.valueOf(offset));
    }
    
    public static void setStreamNIDListInTimelineUpdate(Element timelineUpdate, Element streamNIDList) throws Exception {
    	Element tmpStreamNIDList = timelineUpdate.element("streamNIDList");
    	timelineUpdate.remove(tmpStreamNIDList);
    	timelineUpdate.add(streamNIDList);
    }
    
    public static MediaTimeline createTimelineFromTrackDataDoc(Document trackDataDoc, MRLog log) throws Exception {
    	Element mediaTimelineElement = MRXMLUtils.getElementXpath(trackDataDoc, "//TrackMediaTimeline");
    	return new MediaTimeline((Element)mediaTimelineElement.clone());
    }

    public static MediaTimeline createTimelineFromStreamDoc(Document streamDoc, MRLog log, String id) throws Exception {
    	Element mediaTimelineElement = MRXMLUtils.getElementXpath(streamDoc, "//MediaTimeline");
    	mediaTimelineElement.addAttribute("id", id);
    	return new MediaTimeline((Element)mediaTimelineElement.clone());
    }

    private static long isValidOffset(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, 
    		String trackAction,long now,long createTimeStamp, long lastTrackOffset,
    		String sourceType, String playLength,String sharedPVRStartTC, String trimLength) throws MRException,Exception{
    	if(trackAction.equals(MediaRoomServiceConst.TRACK_CATCHUP)){
    		return lastTrackOffset;
    	}    		

    	long maxOffset  = 0;    	
    	long minOffset =0;
      	if(sourceType.equals(MediaRoomServiceConst.INPUT_ENCODER)){
    		maxOffset = now - createTimeStamp;
    		long defaultTrimLength = Long.parseLong(trimLength); 
    		if(sharedPVRStartTC.isEmpty()) {
    			minOffset = ((maxOffset - defaultTrimLength) < 0 ? 0 : (maxOffset - defaultTrimLength));
    		} else {
    			
    			long lSharedPVRStartTC = Utils.getLongValue(sharedPVRStartTC,0);
			    if(lSharedPVRStartTC != 0) {
			    	
			    	//find pvr buffer size from current
    				long pvrBufferSize = now - lSharedPVRStartTC;
    					
    				//if pvr buffer size exceeds by trimlength set trimlength as pvr Buffer size 
    				if(pvrBufferSize > defaultTrimLength) {
    					pvrBufferSize = defaultTrimLength;
    				}
    			
    				//calculate pvr buffer start TC.
    				long pvrBufferStartTC = now - pvrBufferSize;
    				
    				//find the gap between pvr buffer start tc and creation time of the stream that going to be min offset.
    				minOffset = pvrBufferStartTC - createTimeStamp;    				
    			} else {
    				minOffset = ((maxOffset - defaultTrimLength) < 0 ? 0 : (maxOffset - defaultTrimLength));
    			}
    		}
    		//max offset equal to live offset. If the live offset - current offset is less than limit close to live
    		// then change the last track offset to live offset -  limit close to live.
    		if((maxOffset - lastTrackOffset) < MediaRoomServiceConst.TOO_CLOSE_TO_LIVE_LIMIT){
    			log.addInfo("Offset reached too close to live. Set the last Track Offset MAX_OFFSET:"+maxOffset+"-TO_CLOSE_TO_LIVE_LIMIT:"
    					+MediaRoomServiceConst.TOO_CLOSE_TO_LIVE_LIMIT);
    			lastTrackOffset = maxOffset - MediaRoomServiceConst.TOO_CLOSE_TO_LIVE_LIMIT;
    		}
    		
    	} else if(sourceType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)){
    		maxOffset = Long.parseLong(playLength);  
    	}
    	    	
    	MRLog.getInstance(MRLog.CONSOLE).addInfo("IsValidOffset : Action := "+ trackAction +" MIN_OFFSET := "+minOffset +" MAX_OFFSET := "+maxOffset +" LastTrackOffset := "+lastTrackOffset,MRLog.INFO,MRLog.NOTIFY);
		if(lastTrackOffset > maxOffset ){
			// throw new MRException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, "Invalid OffSet=" + lastTrackOffset);
			log.addInfo("IsValidOffset :  LastTrackOffset is Greater than Max offset. Set the LastTrackOffset as MaxOffset:="+maxOffset,MRLog.INFO,MRLog.NOTIFY);
			return maxOffset - 1;
		}		
		
		if(lastTrackOffset < minOffset) {
			log.addInfo("IsValidOffset :  LastTrackOffset is Less than Min offset. Set the LastTrackOffset as MinOffset:="+minOffset,MRLog.INFO,MRLog.NOTIFY);
			return minOffset;
		}   		
		
    	return lastTrackOffset;
    	
    }

    public Element handleTrackAction(MRRequest request, XMPPI xmpp, MRLog log,MRDB db, 
    		String roomNID,	String requestedAction, String trackAction, String actionData,String requestedTimeScale, String sourceType, String playLength,String sharedPVRStarTC,
    		boolean sync, long startTimecode, String syncSourceUUID, String syncSourceParentDirNID, String syncAgentJID,
    		Element streamNIDList, ArrayList<String> sourceAgentJIDList, String trimLength) throws MRException,Exception{

    	String lastTrackState = this.getLastState();
    	long lastActionWallClock = this.getLastActionWallClock();
    	long startWallClock = this.getStartWallclock();
    	long lastTrackOffset = this.getLastOffset();
    	double lastTimescale = this.getLastTimescale();
    	

    	long wallClock = xmpp.getCurrentTimeMillis();
        lastTrackOffset = lastTrackOffset + (wallClock - lastActionWallClock) * (long)lastTimescale;
	    
	    //If the action is skip then change it into resume from and calculate the offset for skip operations
	    if (MediaRoomServiceConst.TRACK_SKIP_FORWARD.equals(trackAction) || MediaRoomServiceConst.TRACK_SKIP_BACKWARD.equals(trackAction)) {
	        Long lactionData = Long.parseLong(actionData);
	
	        if (MediaRoomServiceConst.TRACK_SKIP_FORWARD.equals(trackAction)) {
	            lactionData = lastTrackOffset + lactionData;
	        } else if (MediaRoomServiceConst.TRACK_SKIP_BACKWARD.equals(trackAction)) {
	            lactionData = lastTrackOffset - lactionData;
	        }      
	
	        actionData = String.valueOf(lactionData);
	        trackAction = MediaRoomServiceConst.TRACK_RESUMEFROM;
	        
	        // Check if we are skipping past live  and set to catchup
	      	if( (sourceType.equals(MediaRoomServiceConst.INPUT_ENCODER)) &&
	          	(lactionData > (wallClock - startWallClock)) ) {
	      		trackAction = MediaRoomServiceConst.TRACK_CATCHUP;
	      	}

	    }
	    
	    double newTimeScale = Utils.getDoubleValue(requestedTimeScale, lastTimescale);
	   
	
	    Document trackStateMachineDoc = MediaRoomServiceUtils.getTrackStateMachine(log);
	    Element transition = MRXMLUtils.getElementXpath(trackStateMachineDoc, "//Transition[@beginstate='" + lastTrackState + "' and @trackaction='" + trackAction + "']");
	
	    if (transition == null) {
	        // throw new MRException(MediaRoomServiceExceptionCodes.ACTIONTRANSACTIONNOTFOUND,MRXMLUtils.generateErrorXML("action", trackAction) );
	        throw new MRException(MediaRoomServiceExceptionCodes.ACTIONTRANSACTIONNOTFOUND, "Transition not found for beginState=" + lastTrackState + 
	        		" trackAction=" + trackAction + " with timelineId=" + this.getId());
	    }
	    
	    lastTrackState = MRXMLUtils.getAttributeValue(transition,"endstate");
	
	    log.addInfo("AddTrackActionRequestHandler:transition=" + MRXMLUtils.elementToString(transition));
	
	    // Update the lastTrackOffset
	    if (MediaRoomServiceConst.TRACK_CATCHUP.equals(trackAction)) {
	        lastTrackOffset = wallClock - startWallClock;
	    } else if (MediaRoomServiceConst.TRACK_RESUMEFROM.equals(trackAction) || MediaRoomServiceConst.TRACK_PAUSEAT.equals(trackAction)) {
	        lastTrackOffset = Long.parseLong(actionData);
	    }        
	    
	    //Check is valid offset or not
	    lastTrackOffset = MediaTimeline.isValidOffset(request, xmpp, log, db, trackAction, wallClock, startWallClock, lastTrackOffset, sourceType, playLength,sharedPVRStarTC, trimLength);
		lastActionWallClock = wallClock;
	    String lastSourceAction = MRXMLUtils.getAttributeValue(transition,"sourceaction");
	    
	    // Set the timescale
	    
	    if(	MediaRoomServiceConst.TRACK_STARTED_CURRENT.equals(lastTrackState) ||
	    	MediaRoomServiceConst.TRACK_STARTED_LOOKBACK.equals(lastTrackState) ) {
	    	//if user is not requesting new timescale on resume use default times scale
	    	if(requestedTimeScale.isEmpty() && lastTimescale==0) {
	    		newTimeScale = 1;
	    	}
	    } else {
	    	newTimeScale = 0;
	    }
	    		
	    long lastTC = 0;
	    MediaTimeline.updateMediaTimelineElement(this.mediaTimelineElement, lastActionWallClock, lastTrackOffset, lastTC, newTimeScale, 
	    		trackAction, lastTrackState, requestedAction, lastSourceAction);

	    long syncTC = 0;
        if(sync && (startTimecode != 0))
  	  		syncTC = startTimecode + this.getLastOffset();

        Element timelineUpdate = MediaTimeline.createTimelineUpdate(this, streamNIDList, sourceAgentJIDList, roomNID, 
        		sync, syncSourceUUID, syncSourceParentDirNID, syncAgentJID, syncTC);

        return timelineUpdate;
    }

}
