package com.ipvs.sessionservice.impl;

import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.handler.AddTrackActionRequestHandler;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.utils.Utils;

public class SessionTrackActionRequestHandler {

	private String getScopedStreamNIDList(Document sessionDoc, Document roomDoc, String connGroupId, String connId) throws Exception {
		StringBuffer sb = new StringBuffer();
		if(!"".equals(connId)) {
			StringTokenizer st = new StringTokenizer(connId, ",");
			while(st.hasMoreTokens()) {
				String id = st.nextToken();
				Element streamEl = MRXMLUtils.getElementXpath(roomDoc, ".//streamlist/Stream[@" + SessionServiceConst.ID + "='" + id + "']");
				if(sb.length() > 0)
					sb.append(",");
				if(streamEl != null)
					sb.append(MRXMLUtils.getAttributeValue(streamEl,SessionServiceConst.NID));
			}
		}
		else if(!"".equals(connGroupId)) { // No group
			List<?> list = MRXMLUtils.getListXpath(sessionDoc, "//Connection[@" + SessionServiceConst.CONN_GROUP_ID + "='" + connGroupId + "']");
			for(Object o:list) {
				Element connection = (Element)o;
				String id = MRXMLUtils.getAttributeValue(connection, SessionServiceConst.ID);
				Element streamEl = MRXMLUtils.getElementXpath(roomDoc, ".//streamlist/Stream[@" + SessionServiceConst.ID + "='" + id + "']");
				if(sb.length() > 0)
					sb.append(",");
				sb.append(MRXMLUtils.getAttributeValue(streamEl,SessionServiceConst.NID));
			}
		}
		return sb.toString();
	}

	private String getStreamNID(Element trackElement, Element actionElement) throws Exception {
		Element streamEl = MRXMLUtils.getElementXpath(trackElement, ".//streamlist/Stream");	
		if (streamEl == null)
		   throw new MRException(MediaRoomServiceExceptionCodes.STREAMNOTFOUND, "No stream found in track");
		
		// Check the offset type
		String offsetType = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.OFFSETTYPE);
		if(		!SessionServiceConst.MEDIAOFFSET.equals(offsetType) &&
				!SessionServiceConst.MEDIATIME.equals(offsetType))
			return MRXMLUtils.getAttributeValue(streamEl,"NID");
		
		// Check if mediaConnId is specified - Tested
		String mediaConnId = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.MEDIACONNID);
		if((mediaConnId != null) && (!"".equals(mediaConnId))) {
			streamEl = MRXMLUtils.getElementXpath(trackElement, ".//streamlist/Stream[@id='" + mediaConnId + "']");
			if(streamEl == null)
			   throw new MRException(MediaRoomServiceExceptionCodes.STREAMNOTFOUND, "No stream found for mediaConnId=" + mediaConnId);
			// System.out.println("SessionTrackActionRequestHandler:getStreamNID found stream for mediaConnId=" + mediaConnId);
			return MRXMLUtils.getAttributeValue(streamEl,"NID");
		}
		
		// Check if mediaId is specified and there is only 1 matching stream with the sourceNID=mediaId
		String mediaId = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.MEDIAID);
		if((mediaId != null) && (!"".equals(mediaId))) {
			String xpath = ".//streamlist/Stream[data/sourceNID='" + mediaId + "']";
			List<?> list = MRXMLUtils.getListXpath(trackElement, xpath);
			if(list.size() == 1) {
				streamEl = (Element)(list.iterator().next());
				// System.out.println("SessionTrackActionRequestHandler:getStreamNID found single stream for mediaId=" + mediaId);
				return MRXMLUtils.getAttributeValue(streamEl,"NID");
			}
			// System.out.println(
			//		"SessionTrackActionRequestHandler:getStreamNID unable to find stream using mediaId=" + mediaId + 
			//		"#streams="+ list.size() + " xpath=" + xpath + "\n" + 
			//		MRXMLUtils.elementToString(trackElement));
		}
		
		// Check if it is a live stream - Tested
		String mediaSourceNID = MRXMLUtils.getValueXpath(streamEl, ".//mediaSourceNID", "");
		Element mediaSource = MRXMLUtils.getElementXpath(trackElement, ".//MediaSource[@NID='" + mediaSourceNID + "']");
		String sourceType = MRXMLUtils.getValueXpath(mediaSource, ".//sourceType", "");
		if(MediaRoomServiceConst.INPUT_ENCODER.equals(sourceType)) {
			//System.out.println("SessionTrackActionRequestHandler:getStreamNID found live stream in session");
			return MRXMLUtils.getAttributeValue(streamEl,"NID");
		}
		
		// Check if synchronized - Tested
        String syncData = MRXMLUtils.getValueXpath(trackElement, ".//" + MediaRoomServiceConst.SYNCDATA, "");
        if(!"".equals(syncData)) {
			//System.out.println("SessionTrackActionRequestHandler:getStreamNID found sync track");
        	return MRXMLUtils.getAttributeValue(streamEl,"NID");
        }

		// Check if this is the only stream
		List<?> list = MRXMLUtils.getListXpath(trackElement, ".//streamlist/Stream");
		if(list.size() == 1) {
			streamEl = (Element)list.get(0);
			//System.out.println("SessionTrackActionRequestHandler:getStreamNID found single stream in session");
			return MRXMLUtils.getAttributeValue(streamEl,"NID");
		}
		

		// throw new MRException(MediaRoomServiceExceptionCodes.STREAMNOTFOUND, "Multiple streams found in track");
		// By default return the first stream
		streamEl = (Element)list.get(0);
		return MRXMLUtils.getAttributeValue(streamEl,"NID");
		
		
   }

   private String getActionData(XMPPI xmpp, MRRequest request, MRDB db, MRLog log, 
		   Element actionElement, String roomNID, String trackNID, String sessionType) throws Exception {
	   
	   String actionData = null;
       Document trackDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, trackNID));
       MediaTimeline timeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc, log);

	   String bookmarkId = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.BOOKMARKID);
	   if(bookmarkId != null) {
		   Document bookmarkDoc = MRXMLDBUtils.readElement(request, log, db, bookmarkId);
		   if(bookmarkDoc == null)
			   throw new MRException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, MRXMLUtils.generateErrorXML("bookmarkId", bookmarkId));    			
		   if(sessionType.equals(SessionServiceConst.LIVE)){
			   MRXMLUtils.addAttribute(actionElement,SessionServiceConst.MEDIATIME, MRXMLUtils.getValueXpath(bookmarkDoc, "//AtTC", ""));				 
		   } else {
		       MRXMLUtils.addAttribute(actionElement,SessionServiceConst.MEDIAOFFSET, MRXMLUtils.getValueXpath(bookmarkDoc, "//Offset", ""));
		   }
		   MRXMLUtils.addAttribute(actionElement,SessionServiceConst.MEDIAID, MRXMLUtils.getValueXpath(bookmarkDoc, "//MediaStreamFileResourceNID", ""));
	   }

	   
	   String startOffsetStr = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.STARTOFFSET);
	   String nowOffsetStr = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.NOWOFFSET);
	   String liveOffsetStr = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.LIVEOFFSET);
	   String mediaTimeStr = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.MEDIATIME);
	   String mediaOffsetStr = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.MEDIAOFFSET);
	   if(startOffsetStr != null) {
		   log.addInfo("SessionServiceUtils:getActionData newOffset=" + startOffsetStr +
				   MRXMLUtils.elementToString(actionElement), MRLog.INFO, MRLog.NOTIFY);
		   MRXMLUtils.addAttribute(actionElement,SessionServiceConst.OFFSETTYPE, SessionServiceConst.STARTOFFSET);
		   return startOffsetStr;
	   } else if(mediaOffsetStr != null) {
		   // TBD : actionData = mediaOffset + [startOffset of media w.r.t Track]
		   log.addInfo("SessionServiceUtils:getActionData newOffset=" + mediaOffsetStr +
				   MRXMLUtils.elementToString(actionElement), MRLog.INFO, MRLog.NOTIFY);
		   MRXMLUtils.addAttribute(actionElement,SessionServiceConst.OFFSETTYPE, SessionServiceConst.MEDIAOFFSET);
		   return mediaOffsetStr;
	   } else if(mediaTimeStr != null) {
	       MRXMLUtils.addAttribute(actionElement,SessionServiceConst.OFFSETTYPE, SessionServiceConst.MEDIATIME);
		   return mediaTimeStr;
	   }
	   
       String lastTrackState = timeline.getLastState();

       long lastTrackOffset = timeline.getLastOffset();
       
       long startWallClock = timeline.getStartWallclock();
       
       long lastActionWallClock = timeline.getLastActionWallClock();
       
       long now = xmpp.getCurrentTimeMillis();
       if (MediaRoomServiceConst.TRACK_STARTED_CURRENT.equals(lastTrackState) || MediaRoomServiceConst.TRACK_STARTED_LOOKBACK.equals(lastTrackState)) {
            lastTrackOffset = lastTrackOffset + (now - lastActionWallClock);
       }
       
	   if(nowOffsetStr != null) {
		   long nowOffset = 0;
		   try {
			   nowOffset = Long.parseLong(nowOffsetStr);
		   }
		   catch(Exception e) {
    			throw new MRException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, MRXMLUtils.generateErrorXML("nowOffset", nowOffsetStr));    			
		   }
		   actionData = String.valueOf(nowOffset + lastTrackOffset);
		   MRXMLUtils.addAttribute(actionElement,SessionServiceConst.OFFSETTYPE, SessionServiceConst.NOWOFFSET);
	   } else if(liveOffsetStr != null) {
		   long liveOffset = 0;
		   try {
			   liveOffset = Long.parseLong(liveOffsetStr);
		   }
		   catch(Exception e) {
    			throw new MRException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, MRXMLUtils.generateErrorXML("nowOffset", nowOffsetStr));    			
		   }
		   actionData = String.valueOf(liveOffset + now - startWallClock);
		   MRXMLUtils.addAttribute(actionElement,SessionServiceConst.OFFSETTYPE, SessionServiceConst.LIVEOFFSET);
	    } else {
	 	   return null;
	    }
	   log.addInfo("SessionServiceUtils:getActionData prevOffset=" + lastTrackOffset + " newOffset=" + actionData +
			   MRXMLUtils.elementToString(actionElement), MRLog.INFO, MRLog.NOTIFY);
	   return actionData;
   }

   private Document executeTrackAction(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			String roomNID, String streamNID,String updateAction,String actionData, 
			String scopedStreamNIDList, String scopedTimelineId,String timeScale)throws Exception{
	   
	   MRRequestHandlerI handler = new AddTrackActionRequestHandler();
	   Document requestDoc = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.ADDTRACK_ACTION_XML, log);
	   MRXMLUtils.getElementXpath(requestDoc, "//roomNID").setText(roomNID);
	   MRXMLUtils.getElementXpath(requestDoc, "//streamNID").setText(streamNID);
	   MRXMLUtils.getElementXpath(requestDoc, "//updateAction").setText(updateAction);
	   MRXMLUtils.getElementXpath(requestDoc, "//timeScale").setText(timeScale);
	   MRXMLUtils.getElementXpath(requestDoc, "//scopedStreamNIDList").setText(scopedStreamNIDList);
	   MRXMLUtils.getElementXpath(requestDoc, "//scopedTimelineId").setText(scopedTimelineId);
	   if (actionData!=null){
		   MRXMLUtils.getElementXpath(requestDoc, "//actiondata").setText(actionData);
	   }
	   String serviceName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.SERVICE_NAME, "");;
       String requestName =  MRXMLUtils.getAttributeValueXpath(requestDoc,MRConst.HEADER_XPATH,MRConst.REQUEST_NAME, "");

       MRMessage message = new MRMessage(requestDoc);
       String resourceGroup = "";
       request = MRRequest.createRequest(request.getUserJID(), MRLog.ERROR, "requestNID", serviceName, requestName, message, resourceGroup);
       Document response = handler.getResponse(request, xmpp, log, db, resourceFactory, serviceAgentFactory);
       return response;
   }

	public Document getTrackResponse(MRRequest request, XMPPI xmpp, MRLog log,
			MRDB db, MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory,
			String actionElementName, String updateAction) throws Exception {
		try {
			return this.getTrackResponseInternal(request, xmpp, log, db, resourceFactory, serviceAgentFactory, actionElementName, updateAction);
		}
		catch(Exception e) {
			String exceptionDescription = "";
			// e.printStackTrace();
	        if (e instanceof MRException) {
	            MRException mre = (MRException) e;
	            exceptionDescription = mre.getCode() + ":" + mre.getDescription();
	        } else {
	            exceptionDescription = MRException.getStackTrace(e);
	        }
			throw new MRException(SessionServiceExceptionCodes.PVR_ACTION_FAILED, exceptionDescription);
		}
	}

	public Document getTrackResponseInternal(MRRequest request, XMPPI xmpp, MRLog log,
				MRDB db, MRResourceFactoryI resourceFactory,
				ServiceAgentFactoryI serviceAgentFactory,
				String actionElementName, String updateAction) throws Exception {

    	String actionData=null;
    	//String whereXpath = "//MediaRoom";
    	Document requestDoc = request.getMessage().getDocument();
    	Element actionElement = MRXMLUtils.getElementXpath(requestDoc, "//" + actionElementName);
    	
		String sessionID = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.SESSION_ID);
		String connId = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.CONN_ID, "");
		String connGroupId = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.CONN_GROUP_ID, "");
    	if (	updateAction.equals(MediaRoomServiceConst.TRACK_SKIP_FORWARD) ||
    			updateAction.equals(MediaRoomServiceConst.TRACK_SKIP_BACKWARD)){
    		actionData = MRXMLUtils.getAttributeValue(actionElement,"skipOffset");
    		try {
    			Long.parseLong(actionData);
    		}
    		catch(Exception ex) {
    			throw new MRException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, MRXMLUtils.generateErrorXML("skipOffset", actionData));    			
    		}
    	}

    	Document sessionDoc = AppServerAgentUtils.getSessionManager(serviceAgentFactory).getSessionDoc(request, xmpp, log, db, resourceFactory, serviceAgentFactory, sessionID);
		if (sessionDoc==null){
			throw new MRException(SessionServiceExceptionCodes.SESSION_ID_DOES_NOTEXISTS, MRXMLUtils.generateErrorXML("sessionID", sessionID));
		}
    	String sessionType = MRXMLUtils.getAttributeValue(sessionDoc.getRootElement(), SessionServiceConst.TYPE_ATTRIBUTE);
    	if(SessionServiceConst.MIXED.equals(sessionType))
			throw new MRException(SessionServiceExceptionCodes.PVR_NOT_SUPPORTED_FOR_MIXED_SESSION, MRXMLUtils.generateErrorXML("sessionID", sessionID));

    	Document roomDoc = SessionServiceUtils.getMediaRoomDoc(sessionID, request, xmpp, log, db, resourceFactory, serviceAgentFactory);		
    	Element roomElement = MRXMLUtils.getElementXpath(roomDoc, "//MediaRoom");
		if (roomElement==null){
			throw new MRException(MediaRoomServiceExceptionCodes.ROOMIDNOTFOUND, MRXMLUtils.generateErrorXML("sessionID", sessionID));
		}
		String roomNID = MRXMLUtils.getAttributeValue(roomElement,"NID");
 
		// Update the actionData based on the TimeSpec
		String trackName = MediaRoomServiceConst.STREAMTRACK;
		if(SessionServiceConst.PAUSERECORD.equals(actionElementName) ||
				SessionServiceConst.RESUMERECORD.equals(actionElementName))
			trackName = MediaRoomServiceConst.RECORDTRACK;

		String trackNID = MediaRoomServiceUtils.getTrackNID(roomNID, trackName);
		String timeScale = MRXMLUtils.getAttributeValue(actionElement, SessionServiceConst.TIME_SCALE_ATTRIBUTE,"");	
		
		//does not support slow motion
		double requestedTimeScale = Utils.getDoubleValue(timeScale, 0);
		if(requestedTimeScale > 0 && requestedTimeScale < 1) {
			throw new MRException(SessionServiceExceptionCodes.SLOWMOTION_DOES_NOT_SUPPROTED, "Slow Motion does not supported");
		}
		
    	if (MediaRoomServiceConst.TRACK_RESUME.equals(updateAction)) {
    		actionData = this.getActionData(xmpp, request, db, log, actionElement, roomNID, trackNID, sessionType);
    		if(actionData != null) {
    			updateAction = MediaRoomServiceConst.TRACK_RESUMEFROM;
	    		try {
	    			Long.parseLong(actionData);
	    		}
	    		catch(Exception ex) {
	    			throw new MRException(MediaRoomServiceExceptionCodes.INVALID_OFFSET, MRXMLUtils.generateErrorXML("offset", actionData));    			
	    		}
    		}
    	}
    	
		// Special processing if mediaTime is specified
 	    String mediaTimeStr = MRXMLUtils.getAttributeValue(actionElement,SessionServiceConst.MEDIATIME);
 	    if(mediaTimeStr != null) {
 	    	updateAction = MediaRoomServiceConst.TRACK_RESUMEFROMMEDIATC;
 	    }

 	    // streamNID to be used for computing media based offset
		Element trackElement = roomElement.element(MediaRoomServiceConst.SYSTEMTRACKLIST).element(trackName);
 	    String streamNID = this.getStreamNID(trackElement, actionElement);
 	    
 	    // scoped list of stream to apply the action to, default value is all
		String scopedStreamNIDList = this.getScopedStreamNIDList(sessionDoc, roomDoc, connGroupId, connId);
		String scopedTimelineId = connGroupId;
		// If no group is specified and a connId is specifed, then use the connId as the timelineId
		if("".equals(scopedTimelineId) && !"".equals(connId))
			scopedTimelineId = connId;
		
		Document trackDataDoc = this.executeTrackAction(request, xmpp, log,   db, resourceFactory, serviceAgentFactory, 
				roomNID, streamNID,updateAction,actionData, 
				scopedStreamNIDList, scopedTimelineId, timeScale);
		
		if(trackDataDoc == null) { // TRACK_RESUMEFROMMEDIATC for Playback/
			log.addInfo("SessionTrackActionRequestHandler:getTrackResponse async updateAction=" + updateAction, MRLog.OK, MRLog.NOTIFY);
	        trackDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, trackNID));
		}
		
		Element timeLineElement = SessionServiceUtils.createTimeLine(xmpp, request, log, db, sessionID, trackNID, trackDataDoc, updateAction, timeScale);
    	return request.createResponseData((Element) timeLineElement.detach());
    }

}
