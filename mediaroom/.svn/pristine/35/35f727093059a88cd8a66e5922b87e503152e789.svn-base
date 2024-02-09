package com.ipvs.mediaroomservice.handler;

import java.net.URLDecoder;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRAuthorization;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.AgentResponseHandlerI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRRequestHandlerI;
import com.ipvs.common.service.MRResponseHandlerI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.datamodel.MediaRoomAuthorization;
import com.ipvs.mediaroomservice.datamodel.MediaRoomObject;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomPEM;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.mediaroomservice.impl.RoomDB;
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class AddTrackActionRequestHandler implements MRRequestHandlerI, AgentResponseHandlerI {
  
    public MRAuthorization authorizeRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
        log.addTraceBegin(request);

        //int authType = MediaRoomObject.STREAM_TYPEID;
        int role = MediaRoomPEM.VISITOR_ROLE;
        String token = "testCommonToken";
        String authority = "testAuthority";
        String roomPolicyNID = MRConst.NULL;
        String streamPolicyNID = MRConst.NULL;

        try {
            @SuppressWarnings("unused")
            Element data = MediaRoomServiceUtils.cloneDataElement(request);
            MediaRoomAuthorization auth = new MediaRoomAuthorization(MRConst.NULL, MRConst.NULL, MediaRoomObject.STREAM_TYPEID, role, roomPolicyNID, streamPolicyNID, token, authority);
            log.addTraceEnd(request);

            return auth;
        } catch (Exception e) {
            System.out.println(e.toString());

            return null;
        }
    }

    
    private void sendSourceAgentRequest(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		String toAgentJID, String streamNID, long mediaTC,String mediaUUID,String mediaParentDirNID) throws Exception {
    	
	    String fromAgentName = MediaRoomServiceConst.APPSERVERAGENT;
	    String toAgentName = MediaRoomServiceConst.SOURCESTREAMAGENT;
	    String responseHandlerName = this.getClass().getName();
	    
	    
	    Document requestXMLDoc= getTimecodeOffsetRequestDocument( mediaParentDirNID, log, mediaUUID, mediaTC);
	    // Build the GetOffsetForTimecode Request
	    String clientData = request.getClientData();
	    MRXMLUtils.getElementXpath(requestXMLDoc, "//"+MRConst.CLIENT_DATA).setText(clientData);
	    String requestName = MRXMLUtils.getAttributeValueXpath(requestXMLDoc,MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, "");
	    MRRequest getOffsetRequest = MRRequest.createRequest(request, requestXMLDoc, requestName, request.getServiceName());	  	   
	    Element data = MRXMLUtils.getElementXpath(requestXMLDoc,getOffsetRequest.getRequestDataXPath());
	
    	 MRXMLUtils.addElement(data,MRXMLUtils.newElement("streamNID"));
    	 MRXMLUtils.addElement(data,MRXMLUtils.newElement("prentDirNID"));
    	 MRXMLUtils.setValueXpath(data, "streamNID", streamNID);
    
	    String dataStr = MRXMLUtils.elementToString(data);
	  
        String info = "GetOffsetTimeCpdeRequestHandler::sendAgentRequestForUserRequest " + 
        			fromAgentName + " to " + toAgentName;
	    // String requestName = getOffsetRequest.getRequestName();
	    Document userRequestDoc = request.marshalRequestObject(log, responseHandlerName);
	    clientData = URLEncoder.encode(MRXMLUtils.documentToString(userRequestDoc), "UTF-8");
	    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, null, fromAgentName, toAgentJID, toAgentName, requestName, dataStr,
	    		info, request.getNID(), request.getUserJID(), MRConst.MEDIA_STORE_ADMIN_SERVICE, clientData);
    }
    
    private Element addTrackAction(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory, 
    		String roomNID, String streamNID, 
    		String requestedAction, String actionData, String requestedTimeScale, boolean agentResponse, 
    		String scopedStreamNIDList, String scopedTimelineId) throws Exception {
    	
        log.addInfo("AddTrackActionRequestHandler:addTrackAction=" + requestedAction + " scopedStreamNIDList=" + scopedStreamNIDList);

        String trackAction = requestedAction;
        
  	  	// com.ipvs.common.XMPPTransaction.DEBUG = true;
  	  	com.ipvs.common.XMPPTransaction.startBlock("AddTrackActionRequest");
  	  	
  	  	// Find the trackNID, trackDataDoc, sourceType, playLength, syncInfo
        String streamDataDocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID);
        if(streamDataDocStr == null)
        	throw new MRException(MediaRoomServiceExceptionCodes.STREAMNOTFOUND,MRXMLUtils.generateErrorXML("streamNID", streamNID));
        Document streamDataDoc = MRXMLUtils.stringToDocument(streamDataDocStr);
        String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID","");
        Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID));
	    String lookbackSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceNID","");
	    if("".equals(lookbackSourceNID) && 
	    		!MediaRoomServiceConst.TRACK_PAUSE.equals(trackAction) &&
	    		!MediaRoomServiceConst.TRACK_CATCHUP.equals(trackAction)) {
	    	throw new MRException(MediaRoomServiceExceptionCodes.PVR_NOTAVAILABLE, "");
	    }
	   
	    	  
	    String xpath = "//StreamTrack//lookbackSourceFilePlayLength";
        String sourceType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");        
        String sharedPVRStartTC = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileStartTC","");
        String trackNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID","");       
        Document trackDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, trackNID));
        boolean sync = false;
	    String syncData = MRXMLUtils.getValueXpath(trackDataDoc, "//" + MediaRoomServiceConst.SYNCDATA, "");
	     
	    if(!"".equals(syncData)) {
        	sync = true;
	    }
	    String playLength = MediaRoomServiceUtils.getPlayLength(request, log, db, roomNID, xpath,sync);  
	    
        String syncSourceUUID = "";
        String syncSourceParentDirNID = "";
        String syncAgentJID = "";
        long startTimecode = 0;
    	String trimLength = "";

        if(MediaRoomServiceConst.INPUT_ENCODER.equals(sourceType)) {
        	if(!lookbackSourceNID.isEmpty()) {
        		trimLength = playLength;
        	}
        }
        else {
        	trimLength = MediaRoomServiceConst.DEFAULT_TRIMLENGTH;
        }
        
	    if(trimLength.isEmpty()){
	    	trimLength = MediaRoomServiceConst.DEFAULT_TRIMLENGTH;
	    }
	    
         if(!"".equals(syncData)) {
        	sync = true;
        	String syncMediaSourceNID = syncData;
        	Document syncMediaSourceDataDoc = MRXMLUtils.stringToDocument(
        			MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, syncMediaSourceNID));
        	syncSourceUUID = MRXMLUtils.getValueXpath(syncMediaSourceDataDoc, "//lookbackSourceFileUUID", "");
        	syncSourceParentDirNID = MRXMLUtils.getValueXpath(syncMediaSourceDataDoc, "//lookbackSourceFileParentDirNID", "");
        	syncAgentJID = MRXMLUtils.getValueXpath(syncMediaSourceDataDoc, "//sourceAgentJID", "");
        	String syncSourceType = MRXMLUtils.getValueXpath(syncMediaSourceDataDoc, "//sourceType", "");
        	if(MediaRoomServiceConst.INPUT_ENCODER.equals(syncSourceType)) {
                MediaTimeline trackTimeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc, log);
        	  	startTimecode = trackTimeline.getStartWallclock();
        	}
        }

        Element timelineUpdateList = MRXMLUtils.stringToElement("<timelineupdatelist/>");

        String action = null;
        /////////////////////////////////////////////////////////////////
        // Per Timeline : Begin
        /////////////////////////////////////////////////////////////////
	    
        ArrayList<String> sourceAgentJIDList = new ArrayList<String>();
        
        // Session Timeline Change
        if("".equals(scopedTimelineId)) {
	        MediaTimeline timeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc, log);
		    
	        // Create the streamlist
	        Element streamNIDList = MRXMLUtils.stringToElement("<streamNIDList/>");
	        Document trackdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, trackNID);
	        List<?> streamList = MRXMLUtils.getListXpath(trackdoc, "//Stream");
	        
	        for(Object o : streamList) {
	        	Element stream = (Element)o;
	        	String streamNIDListItem = stream.attributeValue(MediaRoomServiceConst.NID);
	        	Element mediaTimelineElement = MRXMLUtils.getElementXpath(stream, ".//MediaTimeline");
	        	String timelineId = mediaTimelineElement.attributeValue("id");
	        	if(!"".equals(timelineId) && !trackNID.equals(timelineId)) { // This is a previously scoped PVR stream
	        		String id = stream.attributeValue(MediaRoomServiceConst.ID);
	    	    	throw new MRException(MediaRoomServiceExceptionCodes.STREAMTIMELINENOTINSYNCWITHSESSION, 
	    	    			"Stream Timeline Not In Sync With Session Timeline streamNID=" + streamNIDListItem + " id=" + id);
	        	}
	        	streamNIDList.add(MRXMLUtils.stringToElement("<streamNID>" + streamNIDListItem + "</streamNID>"));
	        	String sourceAgentJID = MRXMLUtils.getValueXpath(stream, ".//sourceAgentJID","");
	        	if(!sourceAgentJIDList.contains(sourceAgentJID))
	        		sourceAgentJIDList.add(sourceAgentJID);
	        }
	        
	        // Handle the action
	        Element timelineUpdate = timeline.handleTrackAction(request, xmpp, log, db, 
	        		roomNID, requestedAction, trackAction, actionData, requestedTimeScale, sourceType, playLength, sharedPVRStartTC,
	        		sync, startTimecode, syncSourceUUID, syncSourceParentDirNID, syncAgentJID, 
	        		streamNIDList, sourceAgentJIDList, trimLength);
	        timelineUpdateList.add(timelineUpdate);
	        
	        // Update the RoomDB
		    RoomDB.getRoomDB().updateTimeLine(xmpp, request, log, db, roomNID, trackNID, trackDataDoc, timeline);
	        action = timeline.getLastSourceAction();
        }
        else {
        	// We create a timelineUpdate for each stream
        	StringTokenizer st = new StringTokenizer(scopedStreamNIDList, ",");
        	while(st.hasMoreTokens()) {
        		String streamNIDValue = st.nextToken();
        		Element streamNIDList = MRXMLUtils.stringToElement("<streamNIDList/>");
        		streamNIDList.add(MRXMLUtils.stringToElement("<streamNID>" + streamNIDValue + "</streamNID>"));
    	        Document streamdoc = MediaRoomServiceUtils.readElement(roomNID, request, log, db, streamNIDValue);
	        	String sourceAgentJID = MRXMLUtils.getValueXpath(streamdoc, ".//sourceAgentJID","");
	        	if(!sourceAgentJIDList.contains(sourceAgentJID))
	        		sourceAgentJIDList.add(sourceAgentJID);
    	        MediaTimeline timeline = MediaTimeline.createTimelineFromStreamDoc(streamdoc, log, streamNIDValue);
    	        Element timelineUpdate = timeline.handleTrackAction(request, xmpp, log, db, 
    	        		roomNID, requestedAction, trackAction, actionData,requestedTimeScale, sourceType, playLength, sharedPVRStartTC,
    	        		sync, startTimecode, syncSourceUUID, syncSourceParentDirNID, syncAgentJID, 
    	        		streamNIDList, sourceAgentJIDList, trimLength);
    	        timelineUpdateList.add(timelineUpdate);
	        	if(action == null)
	        		action = timeline.getLastSourceAction();
        	}
        }

	    /////////////////////////////////////////////////////////////////
        // Per Timeline : End
        /////////////////////////////////////////////////////////////////
        
                
        Element actiondata = MediaTimeline.createTimelineActionDataFromList(timelineUpdateList);

	    
        // String action = MediaRoomServiceConst.TIMELINEUPDATEREQUEST;
        String actionNID = String.valueOf(System.currentTimeMillis());
        String timelineUpdateRequest = StreamAgentUtils.createSourceStreamAgentTimelineUpdateRequestMessage(
				action, actiondata, actionNID, log);
        // System.out.println("AddTrackActionRequestHandler:addTrackAction updateRequest=" + timelineUpdateRequest);
    	for(String sourceAgentJID: sourceAgentJIDList) {
    		ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, roomNID, AppServerAgent.class.getName(), sourceAgentJID, 
        		MediaRoomServiceConst.SOURCESTREAMAGENT, 
        		MediaRoomServiceConst.TIMELINEUPDATEREQUEST,
        		timelineUpdateRequest,
            "AddTrackActionRequestHandler:addTrackAction:=" + request.getUserJID() + "/" + streamNID + " > " + action + " Message to " +
            ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.SOURCESTREAMAGENT),
            request.getNID(),request.getAppServerJID(),MRConst.MEDIA_SERVICE,"");
    		
    	    
    	}
    
        Element trackDataElement = (Element)trackDataDoc.getRootElement().clone();
        trackDataElement.setName("TrackData");
        com.ipvs.common.XMPPTransaction.startBlock("AddTrackActionRequest");
        return trackDataElement;
    }

    // converts mediaTC to offset, returns null if it requires the sourceAgent to do the conversion
    private String convertMediaTCToOffset(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, 
    		MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory,
    		String roomNID, String streamNID, String actiondata) throws Exception {
        String streamDataDocStr = MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, streamNID);
        if(streamDataDocStr == null)
        	throw new MRException(MediaRoomServiceExceptionCodes.STREAMNOTFOUND,MRXMLUtils.generateErrorXML("streamNID", streamNID));

        // Get the track using the streamNID
        Document streamDataDoc = MRXMLUtils.stringToDocument(streamDataDocStr);
        String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID","");
        Document mediaSourceDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, mediaSourceNID));
        String sourceType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
        
        // Check if PVR is enabled for the source
    	long mediaTC = Long.parseLong(actiondata);
    	if(MediaRoomServiceConst.INPUT_MEDIAFILE.equals(sourceType)) { 
        	    String sourceAgentJID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceAgentJID", "");
        	    String mediaUUID =MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileUUID", "");	
        	    String mediaParentDirNID =MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileParentDirNID", "");
        	 	this.sendSourceAgentRequest(request, xmpp, log, db, 
        				resourceFactory, serviceAgentFactory, sourceAgentJID, streamNID, mediaTC,mediaUUID,mediaParentDirNID);
        	 	System.out.println("AddTrackActionRequestHandler:addTrackAction sendAgentRequest");
        		return null;
    	}
    	// Live so we compute offset using startWallClock
        String trackNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID","");
        Document trackDataDoc = MRXMLUtils.stringToDocument(MediaRoomServiceUtils.getObjectData(roomNID, request, db, log, trackNID));
        MediaTimeline trackTimeline = MediaTimeline.createTimelineFromTrackDataDoc(trackDataDoc, log);
        long startWallClock = trackTimeline.getStartWallclock();
        actiondata = String.valueOf(mediaTC - startWallClock);
        return actiondata;
    }        


    public Document getResponse(MRRequest request, XMPPI xmpp, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory)
        throws Exception {
    	
    	Document doc = request.getMessage().getDocument();
    	String roomNID = MRXMLUtils.getValueXpath(doc, "//roomNID", "");
    	String streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");
    	String updateAction = MRXMLUtils.getValueXpath(doc, "//updateAction", "");
    	String timeScale = MRXMLUtils.getValueXpath(doc, "//timeScale", "");
    	String actiondata = MRXMLUtils.getValueXpath(doc, "//actiondata", "");
    	String scopedStreamNIDList = MRXMLUtils.getValueXpath(doc, "//scopedStreamNIDList", "");
    	String scopedTimelineId = MRXMLUtils.getValueXpath(doc, "//scopedTimelineId", "");
    	
    	if("".equals(roomNID))
        	roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, streamNID);

    	// Convert MediaTC to Offset
        if(MediaRoomServiceConst.TRACK_RESUMEFROMMEDIATC.equals(updateAction)) {
        	actiondata = this.convertMediaTCToOffset(request, xmpp, log, db, 
        			resourceFactory, serviceAgentFactory, roomNID, streamNID, actiondata);
        	
        	if(actiondata == null)
        		return null; // Will be handled when the agentResponse is returned
        	updateAction = MediaRoomServiceConst.TRACK_RESUMEFROM;
        }

        Element trackDataElement = this.addTrackAction(request, xmpp, log, db, resourceFactory, serviceAgentFactory, roomNID, streamNID,updateAction, actiondata,timeScale, false, 
        		scopedStreamNIDList, scopedTimelineId);

        if(trackDataElement == null)
        	return null;
        
        return request.createResponseData(trackDataElement);
    }
    
    public Document getTimecodeOffsetRequestDocument(String dirNID,
			 MRLog log, String mediaUUID,long timecode) throws MRException, Exception {
		String mediaStoreRequestName="GetOffsetTimecodeRequest";
		Document requestDocTemplate = MRXMLUtils.loadXMLResource(SessionServiceConst.DEFAULT_REQUEST_TEMPLATE, log);
		Document requestDoc = (Document)requestDocTemplate.clone();
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.REQUEST_NAME, mediaStoreRequestName);
		MRXMLUtils.setAttributeValueXPath(requestDoc.getRootElement(), MRConst.HEADER_XPATH, MRConst.SERVICE_NAME, MRConst.MEDIA_STORE_ADMIN_SERVICE);
		MRXMLUtils.addAttribute(MRXMLUtils.getElementXpath(requestDoc, MRConst.HEADER_XPATH), MRConst.TYPE,  MRConst.REQUEST_TYPE_RPC);
		Element DataEL = MRXMLUtils.getElementXpath(requestDoc, "//Data");
		
		Element requestDataEl = MRXMLUtils.addElement(DataEL, MRXMLUtils.newElement(mediaStoreRequestName + "Data"));
		Element requestEl = MRXMLUtils.newElement(mediaStoreRequestName);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.BY_FILE_ID, mediaUUID);
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.AT_TC, String.valueOf(timecode));
		MRXMLUtils.addAttribute(requestEl, SessionServiceConst.DIR_NID, dirNID);
		MRXMLUtils.addElement(requestDataEl, (Element)requestEl.detach());
		return requestDoc;
	}
    
	public void handleAgentResponse(MRResponseHandlerI responseHandler,
			MRRequest request, XMPPI xmpp, MRLog log, MRDB db,
			MRResourceFactoryI resourceFactory,
			ServiceAgentFactoryI serviceAgentFactory) throws Exception {

	   Document responseDoc = request.getResult();
       int state = Integer.parseInt(MRXMLUtils.getAttributeValueXpath(responseDoc, MRConst.HEADER_XPATH,MRConst.STATE, "0"));

	   String requestXMLFilePath = "/com/ipvs/mediaroomservice/xml/" + MediaRoomServiceUtils.ADDTRACKACTION_REQUEST_XML;
	   
       Document requestXMLDoc = MRXMLUtils.loadXMLResource(requestXMLFilePath, log);
       String clientData = MRXMLUtils.getValueXpath(responseDoc, "//"+MRConst.CLIENT_DATA, "");
       Document userRequestDoc = MRXMLUtils.stringToDocument((URLDecoder.decode(clientData, "UTF-8")));
       MRXMLUtils.setValueXpath(requestXMLDoc, "//"+MRConst.CLIENT_DATA, 
      			MRXMLUtils.getValueXpath(userRequestDoc, "//"+MRConst.CLIENT_DATA, ""));
       MRRequest userRequest = MRRequest.unMarshalRequestObject(userRequestDoc, 
    		   requestXMLDoc, "", log);
       
       if(state == 200) {
    	   String roomNID = null; // This will cause a hit need to find this value
    	   String streamNID = MRXMLUtils.getValueXpath(userRequestDoc, "//streamNID", "");
    	   Element data = MRXMLUtils.getElementXpath(responseDoc, "//GetOffsetTimecodeResponseData/Data");
    	   String offset = MRXMLUtils.getAttributeValue(data, SessionServiceConst.BOOKMARK_OFFSET);
    	   log.addInfo("AppServerAgent: handleAgentResponse offset =" +offset +" streamNID:="+ streamNID);	   
       	   
    	   String actiondata = offset;
    	   //MRXMLUtils.getValueXpath(responseDoc, "//StreamOffset", "");
    	   String scopedStreamNIDList = MRXMLUtils.getValueXpath(userRequestDoc, "//scopedStreamNIDList", "");
    	   String scopedTimelineId = MRXMLUtils.getValueXpath(userRequestDoc, "//scopedTimelineId", "");
    	   String timeScale = MRXMLUtils.getValueXpath(userRequestDoc, "//timeScale", "");
    	    
         
    	   roomNID = MediaRoomServiceUtils.getRoomNID(request, xmpp, log, db, streamNID);
       
    	   Element trackDataElement = this.addTrackAction(userRequest, xmpp, log, db, resourceFactory, serviceAgentFactory, 
    			   roomNID, streamNID, MediaRoomServiceConst.TRACK_RESUMEFROM, actiondata,timeScale, true, scopedStreamNIDList, scopedTimelineId);
           userRequest.setResult(userRequest.createResponseData(trackDataElement));
       }

       String logData = MRXMLUtils.getValueXpath(responseDoc, "//Log", "");
       userRequest.setState(state);
       if(state == MRRequest.STATE_EXCEPTION) {
    	   String codeStr = MRXMLUtils.getValueXpath(responseDoc, "//Code", 
    			   String.valueOf(MediaRoomServiceExceptionCodes.AGENTREQUESTFAILED));
    	   int code = Integer.parseInt(codeStr);
    	   String description = MRXMLUtils.getValueXpath(responseDoc, "//Description", 
    	   			"AgentRequestFailed");
    	   MRException mrException = new MRException(code, description);
    	   userRequest.setException(mrException);
       }
       responseHandler.handleResponse(userRequest, logData);
	}

}
