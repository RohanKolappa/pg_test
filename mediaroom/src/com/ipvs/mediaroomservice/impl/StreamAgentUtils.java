package com.ipvs.mediaroomservice.impl;

import java.net.URLEncoder;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRXMLUtils;

public class StreamAgentUtils {
	
    public static final String SOURCE_EVENT_INSTANCE_XML = "/com/ipvs/mediaroomservice/agent/SourceEventInstance.xml";
    public static final String DEST_STREAM_AGENT_REQUEST_XML = "/com/ipvs/mediaroomservice/agent/DestStreamAgentRequest.xml";
    public static final String SOURCE_STREAM_AGENT_ACTION_REQUEST_XML = "/com/ipvs/mediaroomservice/agent/SourceStreamAgentActionRequest.xml";
    public static final String SOURCE_STREAM_AGENT_TIMELINEUPDATE_REQUEST_XML = "/com/ipvs/mediaroomservice/agent/SourceStreamAgentTimelineUpdateRequest.xml";
    public static final String SOURCE_STREAM_AGENT_REQUEST_XML = "/com/ipvs/mediaroomservice/agent/SourceStreamAgentRequest.xml";
    public static final String SOURCE_STREAM_AGENT_RESPONSE_XML = "/com/ipvs/mediaroomservice/agent/SourceStreamAgentResponse.xml";
    
    public static Document sourceEventTemplate = null;
    public static Document sourceStreamAgentActionRequestTemplate = null;
    public static Document sourceStreamAgentTimelineUpdateRequestTemplate = null;
    public static Document sourceStreamAgentResponseTemplate = null;
    public static Document destStreamAgentRequestTemplate = null;
    public static Element streamStatusTemplate = null;

    public static String createSourceStreamAgentActionRequestMessage(String streamNID, String userAction, String updateAction, Element actiondata, 
    		long wallClock, String actionNID, Document streamDataDoc, Document mediaSourceDataDoc, 
    		MRLog log) throws Exception {

    	String requestData = "";
        if(MediaRoomServiceConst.STOPREQUEST.equals(updateAction)) {
        	requestData = "<data>" + 
            		"<streamNID>" + streamNID + "</streamNID>" +
            		   "</data>";
        }
        else {
	    	String streamdatadocStr = "";
	        String mediasourcedatadocStr = "";
	        try {
	            streamdatadocStr = URLEncoder.encode(MRXMLUtils.documentToString(streamDataDoc), "UTF-8");
	            mediasourcedatadocStr = URLEncoder.encode(MRXMLUtils.documentToString(mediaSourceDataDoc), "UTF-8");
	        } catch (Exception e) {
	            e.printStackTrace();
	        }
	        requestData = "<data>" + 
	        		"<streamNID>" + streamNID + "</streamNID>" +
	        		"<streamdatadoc>" + streamdatadocStr + "</streamdatadoc>" +
	        		"<mediasourcedatadoc>" + mediasourcedatadocStr + "</mediasourcedatadoc>" +
	        		   "</data>";
        }
        return requestData;

        /*
    	if(StreamAgentUtils.sourceStreamAgentActionRequestTemplate == null)
    		StreamAgentUtils.sourceStreamAgentActionRequestTemplate = MRXMLUtils.loadXMLResource(StreamAgentUtils.SOURCE_STREAM_AGENT_ACTION_REQUEST_XML, log);
    	Document doc = (Document)StreamAgentUtils.sourceStreamAgentActionRequestTemplate.clone();

        
        MRXMLUtils.setValueXpath(doc, "//streamNID", streamNID);
        MRXMLUtils.setValueXpath(doc, "//useraction", userAction);
        MRXMLUtils.setValueXpath(doc, "//action", updateAction);
        Element tmpactiondata = MRXMLUtils.getElementXpath(doc, "//actiondata");
        Element parent = tmpactiondata.getParent();
        parent.remove(tmpactiondata);
        parent.add(actiondata);
        MRXMLUtils.setValueXpath(doc, "//wallClock", String.valueOf(wallClock));
        MRXMLUtils.setValueXpath(doc, "//actionNID", actionNID);
        MRXMLUtils.setValueXpath(doc, "//streamdatadoc", streamdatadocStr);
        MRXMLUtils.setValueXpath(doc, "//mediasourcedatadoc", mediasourcedatadocStr);
        */
      
     /*
        <data>
    	<streamNID>f3256f5e-362d-4709-a085-ef0521634037</streamNID>
    	<useraction>Start</useraction>
    	<action>StartRequest</action>
    <!--  Setup(Start), Teardown(Stop)-->	
    	
    <!--  Offset -->
    	<wallClock>0</wallClock>
    	<actionNID>9bc15289-c1a3-49d6-9c90-2befa14072a0</actionNID>
    <!--  Id of the TimeLine -->
    	<streamdatadoc>%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0A%3Cdata%3E%0A++++++++++++++++++%3CroomNID%3E9bc15289-c1a3-49d6-9c90-2befa14072a0%3C%2FroomNID%3E%0A++++++++++++++++++%3CmediaSourceNID%3E22b7b7e2-78b1-4d73-ac62-9433960bdf55%3C%2FmediaSourceNID%3E%0A++++++++++++++++++%3CmediaDestNID%3Ea331b34e-df19-40a3-bb6c-069f202edcbb%3C%2FmediaDestNID%3E%0A++++++++++++++++++%3CprofileNID%3Edefault.mediastreamprofilelist.mspl_933326e5-3e67-486f-85ee-912b5b7e3e0e%3C%2FprofileNID%3E%0A++++++++++++++++++%3CprofileXML%3E%3CV2DMediaStreamProfile%3E%0A++++%3CGroups%3E%0A++++++++%3CTag+Name%3D%22Network%22+Value%3D%22LAN%22%2F%3E%0A++++%3C%2FGroups%3E%0A++++%3CTitle%3EV2D_LAN%3C%2FTitle%3E%0A++++%3CDescription%3EV2D_LAN%3C%2FDescription%3E%0A++++%3CStreamType%3EV2D%3C%2FStreamType%3E%0A++++%3CPriority%3E1%3C%2FPriority%3E%0A++++%3CStreamActionInfo%3E%0A++++++++%3CAllowPause%3Etrue%3C%2FAllowPause%3E%0A++++++++%3CAllowSkipback%3Etrue%3C%2FAllowSkipback%3E%0A++++++++%3CAllowSkipfwd%3Etrue%3C%2FAllowSkipfwd%3E%0A++++%3C%2FStreamActionInfo%3E%0A++++%3CInfo%3E%0A++++++++%3CV2DStreamConfig%3E%0A++++++++++++%3CIsMulticast%3Efalse%3C%2FIsMulticast%3E%0A++++++++++++%3CEnableSRDTranscode%3Etrue%3C%2FEnableSRDTranscode%3E%0A++++++++++++%3CSRDTranscodeBandwidth%2F%3E%0A++++++++++++%3CAVOption%3EVideo%3C%2FAVOption%3E%0A++++++++%3C%2FV2DStreamConfig%3E%0A++++++++%3CV2DEncoderAudioConfig%3E%0A++++++++++++%3CSampleRate%2F%3E%0A++++++++++++%3CSampleSize%2F%3E%0A++++++++++++%3CMonoStereo%2F%3E%0A++++++++++++%3CAudioEncodeEnable%2F%3E%0A++++++++%3C%2FV2DEncoderAudioConfig%3E%0A++++++++%3CV2DEncoderConnectionParameterConfig%3E%0A++++++++++++%3CBurstRatio%3E1.00%3C%2FBurstRatio%3E%0A++++++++++++%3CBurstDuration%3E4%3C%2FBurstDuration%3E%0A++++++++++++%3CCompression%3E%0A++++++++++++++++%3CHigh%3E6%3C%2FHigh%3E%0A++++++++++++++++%3CLow%3E4%3C%2FLow%3E%0A++++++++++++%3C%2FCompression%3E%0A++++++++++++%3CBandwidth%3E10000%3C%2FBandwidth%3E%0A++++++++++++%3CRefresh%3E%0A++++++++++++++++%3CMin%3E1%3C%2FMin%3E%0A++++++++++++++++%3CMax%3E10%3C%2FMax%3E%0A++++++++++++%3C%2FRefresh%3E%0A++++++++++++%3CSlice%3E%0A++++++++++++++++%3CMin%3E1%3C%2FMin%3E%0A++++++++++++++++%3CMax%3E13%3C%2FMax%3E%0A++++++++++++%3C%2FSlice%3E%0A++++++++++++%3CBlockThreshold%3E4%3C%2FBlockThreshold%3E%0A++++++++++++%3CColorSampling%3E4%3A2%3A2%3C%2FColorSampling%3E%0A++++++++++++%3CFrameRateDivider%3E1%3C%2FFrameRateDivider%3E%0A++++++++++++%3CIFrameInterval%2F%3E%0A++++++++%3C%2FV2DEncoderConnectionParameterConfig%3E%0A++++++++%3CV2DEncoderConnectionParameterKbmConfig%3E%0A++++++++++++%3CAllowKMSwitch%3Etrue%3C%2FAllowKMSwitch%3E%0A++++++++++++%3CKMIdleTime%3E0%3C%2FKMIdleTime%3E%0A++++++++++++%3CEnableKbm%3Efalse%3C%2FEnableKbm%3E%0A++++++++%3C%2FV2DEncoderConnectionParameterKbmConfig%3E%0A++++++++%3CV2DRecorderConfig%3E%0A++++++++++++%3CFFTracks%3E1-256%3C%2FFFTracks%3E%0A++++++++++++%3CThumbnailConfig%3E%0A++++++++++++++++%3CEnable%3Etrue%3C%2FEnable%3E%0A++++++++++++%3C%2FThumbnailConfig%3E%0A++++++++%3C%2FV2DRecorderConfig%3E%0A++++%3C%2FInfo%3E%0A%3C%2FV2DMediaStreamProfile%3E%3C%2FprofileXML%3E%0A++++++++++++++++++%3CrelayNID%2F%3E%0A++++++++++++++++++%3CmediarelayNID%2F%3E%0A++++++++++++++++++%3CdestNID%3Edefault.mediastreamrelayportresourcelist.msrprl_29ed3e1b-e69a-4854-a9dc-4ce97ca85d0b%3C%2FdestNID%3E%0A++++++++++++++++++%3CdestPortNID%3Edefault.mediastreamrelayportresourcelist.msrprl_29ed3e1b-e69a-4854-a9dc-4ce97ca85d0b%3C%2FdestPortNID%3E%0A++++++++++++++++++%3CdestIP%2F%3E%0A++++++++++++++++++%3CdestUDPStartPort%2F%3E%0A++++++++++++++++++%3CdestAgentJID%3Esrcrelay1%40localhost%2Fsrcrelay1%3C%2FdestAgentJID%3E%0A++++++++++++++++++%3CdestFileUUID%2F%3E%0A++++++++++++++++++%3CdestFileParentDirNID%2F%3E%0A++++++++++++++++++%3CcallbackPort%2F%3E%0A++++++++++++++++++%3CdestStreamURL%2F%3E%0A++++++++++++++++++%3CsourceNID%3Edefault.mediastreamsrcportresourcelist.mssprl_57084738-4839-41b1-946f-36aadc24e8de%3C%2FsourceNID%3E%0A++++++++++++++++++%3CsourcePortNID%3Edefault.mediastreamsrcportresourcelist.mssprl_57084738-4839-41b1-946f-36aadc24e8de%3C%2FsourcePortNID%3E%0A++++++++++++++++++%3CxAuthAgentJID%2F%3E%0A%0A++++++++++++++++++%3CstreamGroups%3E%0A+++++++++++++++++++++%3CTag+Name%3D%22track%22+Value%3D%229bc15289-c1a3-49d6-9c90-2befa14072a0.StreamTrack%22%2F%3E%0A++++++++++++++++++%3C%2FstreamGroups%3E%0A%0A++++++++++++++++++%3CstreamOwnerUserJID%3Esrcrelay1%40localhost%2Fsrcrelay1%3C%2FstreamOwnerUserJID%3E%0A++++++++++++++++++%3CstreamURLType%2F%3E%0A++++++++++++++++++%3CsourceAgentJID%3Etx1%40localhost%2Ftx1%3C%2FsourceAgentJID%3E%0A++++++++++++++++++%3CstartWallClock%3E1362714182584%3C%2FstartWallClock%3E%0A++++++++++++++++++%3CtrimLength%2F%3E%0A++++++++++++++++++%3CtrackStateAtStart%3ESTARTED_CURRENT%3C%2FtrackStateAtStart%3E%0A++++++++++++++++++%3CtrackOffsetAtStart%3E178%3C%2FtrackOffsetAtStart%3E%0A++++++++++++++++++%3CsourceNetworkAddressRule%2F%3E%0A++++++++++++++++++%3CdestNetworkAddressRule%2F%3E%0A++++++++++++++++++%3CpublishedSourceIP%3E10.51.50.1%3C%2FpublishedSourceIP%3E%0A++++++++++++++++++%3CpublishedDestIP%3E10.51.51.14%3C%2FpublishedDestIP%3E%0A++++++++++++++++++%3CresolvedSourceIP%3E10.51.50.1%3C%2FresolvedSourceIP%3E%0A++++++++++++++++++%3CresolvedDestIP%3E10.51.51.14%3C%2FresolvedDestIP%3E%0A++++++++++++++++++%3CdestPortElementID%2F%3E%0A++++++++++++++++++%3CdestType%3EOUTPUT_STREAM%3C%2FdestType%3E%0A++++++++++++++++++%3CstreamType%3EV2D%3C%2FstreamType%3E%0A++++++++++++++++++%3CstreamURL%2F%3E%0A++++++++++++++++++%3CwindowId%2F%3E%0A++++++++++++++++++%3CcanvasId%2F%3E%0A++++++++++++++++++%3CtrickPlay%3Etrue%3C%2FtrickPlay%3E%0A+++++++++++++++%3C%2Fdata%3E</streamdatadoc>
    <!--  streamdatadoc corresponding to the streamNID -->
    	<mediasourcedatadoc>%3C%3Fxml+version%3D%221.0%22+encoding%3D%22UTF-8%22%3F%3E%0A%3Cdata%3E%0A++++++++++++++++++%3CroomNID%3E9bc15289-c1a3-49d6-9c90-2befa14072a0%3C%2FroomNID%3E%0A++++++++++++++++++%3CsourceNID%3Edefault.mediastreamsrcportresourcelist.mssprl_57084738-4839-41b1-946f-36aadc24e8de%3C%2FsourceNID%3E%0A++++++++++++++++++%3CstartOffset%3E0%3C%2FstartOffset%3E%0A++++++++++++++++++%3CstartTimecode%3E1362714182406%3C%2FstartTimecode%3E%0A++++++++++++++++++%3CuserRoomMediaSourceNID%3E187d8ae1-9c4d-4f3d-b5bf-00ab73c155e7%3C%2FuserRoomMediaSourceNID%3E%0A++++++++++++++++++%3CsourceType%3EENCODER%3C%2FsourceType%3E%0A++++++++++++++++++%3CsourcePortNID%3Edefault.mediastreamsrcportresourcelist.mssprl_57084738-4839-41b1-946f-36aadc24e8de%3C%2FsourcePortNID%3E%0A++++++++++++++++++%3CsourcePortElementID%3E1%3C%2FsourcePortElementID%3E%0A++++++++++++++++++%3CsourceAgentJID%3Etx1%40localhost%2Ftx1%3C%2FsourceAgentJID%3E%0A++++++++++++++++++%3CstreamType%3EV2D%3C%2FstreamType%3E%0A++++++++++++++++++%3CstreamURL%2F%3E%0A++++++++++++++++++%3CstreamProfile%2F%3E%0A++++++++++++++++++%3CtrackNID%3E9bc15289-c1a3-49d6-9c90-2befa14072a0.StreamTrack%3C%2FtrackNID%3E%0A++++++++++++++++++%3CstreamTrackMediaSourceNID%2F%3E%0A++++++++++++++++++%3CpvrEnabled%2F%3E%0A++++++++++++++++++%3ClookbackSourceNID%2F%3E%0A++++++++++++++++++%3ClookbackSourceAgentJID%2F%3E%0A++++++++++++++++++%3ClookbackSourceFileUUID%2F%3E%0A++++++++++++++++++%3ClookbackSourceFileParentDirNID%2F%3E%0A++++++++++++++++++%3ClookbackSourceFilePlayLength%2F%3E%0A++++++++++++++++++%3ClookbackSourceFileStartTC%2F%3E%0A++++++++++++++++++%3CxAuthAgentJID%2F%3E%0A++++++++++++++++++%3CmonitorAgentJID%2F%3E%0A+++++++++++++++%3C%2Fdata%3E</mediasourcedatadoc>
    <!--  Setup/Teardown=mediasourcedatadoc -->
    <actiondata/></data>
    */
        // String s = MRXMLUtils.elementToString(MRXMLUtils.getElementXpath(doc, "//data"));
        // return MRXMLUtils.elementToString(MRXMLUtils.getElementXpath(doc, "//data"));
        
    }

    public static String createSourceStreamAgentTimelineUpdateRequestMessage(String updateAction, Element actiondata, 
    		String actionNID, MRLog log) throws Exception {

    	if(StreamAgentUtils.sourceStreamAgentTimelineUpdateRequestTemplate == null)
    		StreamAgentUtils.sourceStreamAgentTimelineUpdateRequestTemplate = MRXMLUtils.loadXMLResource(StreamAgentUtils.SOURCE_STREAM_AGENT_TIMELINEUPDATE_REQUEST_XML, log);
    	Document doc = (Document)StreamAgentUtils.sourceStreamAgentTimelineUpdateRequestTemplate.clone();
        
        MRXMLUtils.setValueXpath(doc, "//action", updateAction);
        Element tmpactiondata = MRXMLUtils.getElementXpath(doc, "//actiondata");
        Element parent = tmpactiondata.getParent();
        parent.remove(tmpactiondata);
        parent.add(actiondata);
        MRXMLUtils.setValueXpath(doc, "//actionNID", actionNID);
        return MRXMLUtils.elementToString(MRXMLUtils.getElementXpath(doc, "//data"));
    }
    
    public static String createSourceStreamAgentResponseMessage(String roomNID, String streamNID, String userAction, String request, String actionData, 
    		String streamURL, String relayRoomNID, String relayStreamNID, String actionNID, int status, String logdata,
        MRLog log, Element mediaTimeline) throws Exception {
    	if(StreamAgentUtils.streamStatusTemplate == null) {
    		Document streamDocTemplate = RoomDB.getStreamInstanceXML(log);
    		StreamAgentUtils.streamStatusTemplate = MRXMLUtils.getElementXpath(streamDocTemplate, "//Status");
    		streamStatusTemplate.add(MRXMLUtils.stringToElement("<URL/>"));
    		streamStatusTemplate.setName("StreamStatus");
        	// System.out.println("StreamAgentUtils:createSourceStreamAgentResponseMessage template=" + streamStatusTemplate.asXML());
    	}
    	
    	streamStatusTemplate.addAttribute("streamNID", streamNID);
    	streamStatusTemplate.addAttribute("roomNID", roomNID);
    	streamStatusTemplate.element("state").setText(String.valueOf(status));
    	streamStatusTemplate.element("URL").setText(streamURL);
    	if(mediaTimeline != null) {
	    	streamStatusTemplate.remove(streamStatusTemplate.element("MediaTimeline"));
	    	streamStatusTemplate.add((Element)mediaTimeline.clone());
    	}
        Element relayData = streamStatusTemplate.element("relayData");

/*
=======

    	if(StreamAgentUtils.sourceStreamAgentResponseTemplate == null)
    		StreamAgentUtils.sourceStreamAgentResponseTemplate = MRXMLUtils.loadXMLResource(StreamAgentUtils.SOURCE_STREAM_AGENT_RESPONSE_XML, log);
    	Document doc = (Document)StreamAgentUtils.sourceStreamAgentResponseTemplate.clone();
        	

        MRXMLUtils.setValueXpath(doc, "//streamNID", streamNID);
        Element streamNIDElement = MRXMLUtils.getElementXpath(doc, "//streamNID");
        streamNIDElement.addAttribute("roomNID", roomNID);

        MRXMLUtils.setValueXpath(doc, "//useraction", userAction);
        MRXMLUtils.setValueXpath(doc, "//action", action);

        //MRXMLUtils.setValueXpath(doc, "//actiondata", actiondata);
        Element actionDataEl = MRXMLUtils.stringToElement("<actiondata>" + actionData + "</actiondata>");
        MRXMLUtils.replaceContent(doc, "//actiondata", actionDataEl);
        MRXMLUtils.setValueXpath(doc, "//actionNID", actionNID);
        MRXMLUtils.setValueXpath(doc, "//streamURL", streamURL);
        MRXMLUtils.setValueXpath(doc, "//status", String.valueOf(status));
        Element logDataEl = MRXMLUtils.stringToElement("<log>" + logdata + "</log>");
        MRXMLUtils.replaceContent(doc, "//Log", logDataEl);
        
        if(mediaTimeline != null) {
	        Element startTime= MRXMLUtils.getElementXpath(doc, "//startTime");
	        startTime.add((Element)mediaTimeline.clone());
        }
        
        Element relayData = MRXMLUtils.getElementXpath(doc, "//relayData");
>>>>>>> .merge-right.r69109
*/
        relayData.addAttribute("relayRoomNID", relayRoomNID);
        relayData.addAttribute("relayStreamNID", relayStreamNID);
        
    	return "<data>" + MRXMLUtils.elementToString(streamStatusTemplate) + "</data>";
    }


    public static Document createSourceEventMessageWithActionDataElement(String streamNID, String action, int eventCode, String streamList, 
    		long startTC, Element actiondataElement, int status, String logdata,MRLog log) throws Exception {

    	Document sourceEventDoc = MRXMLUtils.loadXMLResource(StreamAgentUtils.SOURCE_EVENT_INSTANCE_XML, log);
    	Element streamlist = MRXMLUtils.stringToElement("<streamlist>"+streamList+"</streamlist>");
    	MRXMLUtils.getElementXpath(sourceEventDoc, "//eventCode").setText(String.valueOf(eventCode));
    	MRXMLUtils.getElementXpath(sourceEventDoc, "//action").setText(action);
    	MRXMLUtils.getElementXpath(sourceEventDoc, "//streamNID").setText(streamNID);
    	MRXMLUtils.getElementXpath(sourceEventDoc, "//startTC").setText(String.valueOf(startTC));
    	Element sourceEventActionData = MRXMLUtils.getElementXpath(sourceEventDoc, "//sourceEventActionData");
    	actiondataElement.setName("sourceEventActionData");
    	Element parent = sourceEventActionData.getParent();
    	parent.remove(sourceEventActionData);
    	parent.add(actiondataElement);
    	
    	if (!streamList.equals("")){
    		MRXMLUtils.replaceContent(sourceEventDoc, "//streamNIDList" , streamlist);
    	} else {
    		MRXMLUtils.getElementXpath(sourceEventDoc, "//streamNIDList").setText(streamList);
   		}
    	MRXMLUtils.getElementXpath(sourceEventDoc, "//status").setText(String.valueOf(status));
    	return sourceEventDoc;
    }

    public static Document createSourceEventMessage(String streamNID, String action, int eventCode, String streamList, 
    		long startTC, String actiondata, int status, String logdata,MRLog log) throws Exception {
    	Element actiondataElement = MRXMLUtils.stringToElement("<actiondata>" + actiondata + "</actiondata>");
    	return StreamAgentUtils.createSourceEventMessageWithActionDataElement(streamNID, action, eventCode, streamList, startTC, 
    			actiondataElement, status, logdata, log);
    }

    private static Document createDestinationEventMessageInternal(String streamNID, String action, int eventCode, String event, 
    		long timeStamp,long wallclock , int status, MRLog log) throws Exception {
        
    	Document destEventDoc = MRXMLUtils.loadXMLResource(MediaRoomServiceUtils.DEST_EVENT_INSTANCE_XML, log);
    	MRXMLUtils.getElementXpath(destEventDoc, "//streamNID").setText(streamNID);
    	MRXMLUtils.getElementXpath(destEventDoc, "//eventCode").setText(String.valueOf(eventCode));
    	MRXMLUtils.getElementXpath(destEventDoc, "//action").setText(action);
    	MRXMLUtils.getElementXpath(destEventDoc, "//status").setText(String.valueOf(status));
        if(!event.isEmpty()) {
        	MRXMLUtils.getElementXpath(destEventDoc, "//"+event+"TimeStamp").setText(String.valueOf(timeStamp));
        	MRXMLUtils.getElementXpath(destEventDoc, "//"+event+"WallClock").setText(String.valueOf(wallclock));
        }
        return destEventDoc;
    }


    public static Document createDestinationEventMessage(String streamNID, String action, int eventCode, String event, 
    		long timeStamp,long wallclock , String actiondata, int status, String logdata,MRLog log) throws Exception {
    	Document destEventDoc = StreamAgentUtils.createDestinationEventMessageInternal(streamNID, action, eventCode, event, 
    			timeStamp, wallclock, status, log);
    	MRXMLUtils.getElementXpath(destEventDoc, "//destinationEventActionData").setText(actiondata);
    	return destEventDoc;
    }

    public static Document createDestinationEventDoc(String streamNID, String action, int eventCode, String event, 
    		long timeStamp,long wallclock , Element actiondata, int status, MRLog log) throws Exception {
    	Document destEventDoc = StreamAgentUtils.createDestinationEventMessageInternal(streamNID, action, eventCode, event, 
    			timeStamp, wallclock, status, log);
    	MRXMLUtils.getElementXpath(destEventDoc, "//destinationEventActionData").add(actiondata);
    	return destEventDoc;
    }




}
