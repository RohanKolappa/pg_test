package com.ipvs.mediaroomservice.stream;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRConst;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.ServiceAgentI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.DestStreamAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.Utils;

public class SourceSM {

    private XMPPF xf;
    private MRLog log;
    private String userJID;
    Document streamDocTemplate = null;
    StreamUtils streamUtils;
    StreamTable streamTable;
    

    public SourceSM(XMPPF xf, MRLog log, 
    		String userJID, 
    		StreamUtils streamUtils) {
        this.xf = xf;
        this.log = log;
        this.userJID = userJID;
        this.streamUtils = streamUtils;
        this.streamTable = streamUtils.getStreamTable();
    }
    
    Document getRelayStreamDataDoc(String streamNID) throws Exception {
	    ServiceAgentI destAgent = this.xf.sf(userJID).getServiceAgent(MediaRoomServiceConst.DESTSTREAMAGENT);
	    DestStreamAgent destStreamAgent = (DestStreamAgent)destAgent;
	    return destStreamAgent.getStreamDataDoc(streamNID);
    }
    
    Document getRelayMediaSourceDataDoc(String streamNID) throws Exception {
	    ServiceAgentI destAgent = this.xf.sf(userJID).getServiceAgent(MediaRoomServiceConst.DESTSTREAMAGENT);
	    DestStreamAgent destStreamAgent = (DestStreamAgent)destAgent;
	    return destStreamAgent.getMediaSourceDataDoc(streamNID);
    }
    
    public void updateMediaSourceData(String streamNID, Document mediaSourceData) {
    	this.streamTable.updateMediaSourceData(streamNID, mediaSourceData);
    }
    
    Document getDestTableDoc() throws Exception {
	    ServiceAgentI destAgent = this.xf.sf(userJID).getServiceAgent(MediaRoomServiceConst.DESTSTREAMAGENT);
	    DestStreamAgent destStreamAgent = (DestStreamAgent)destAgent;
	    return destStreamAgent.getDestTableDoc();
    }
    
    Document getSourceTableDoc() throws Exception {
    	return this.streamTable.sourceTableDoc;
    }

    
    public void handleTimelineUpdate(MRClient client, String requestNID, String actionNID,  
    		String streamXPath, Element timelineUpdate,
    		boolean sync, long syncTC) throws Exception {

        // this.log.addInfo("SourceSM:handleTimelineUpdate", MRLog.OK, MRLog.NOTIFY);

        // com.ipvs.common.XMPPTransaction.DEBUG = true;
        String streamURL = "";
        long offset = MediaTimeline.getOffsetFromTimelineUpdate(timelineUpdate);
        int status = MRLog.OK;
        String logdata = "";
        List<?> streamList = null;
        Hashtable<String, Element> syncSourceList = new Hashtable<String, Element>();
	    String sourceAction = timelineUpdate.element("MediaTimeline").element("Last").attributeValue("sourceAction");
        try {
        	
            //Find the list of all streams
            streamList = MRXMLUtils.getListXpath(this.getSourceTableDoc(), "//" + streamXPath);
            
            // eliminate rx streams
    		for (Iterator<?> i = streamList.iterator(); i.hasNext();) {
               	Element streamElement = (Element)i.next();
               	Element outputElement = streamElement.element("Output");
               	String outputType = outputElement.attributeValue("outputType");
               	if(MediaRoomServiceConst.OUTPUT_DECODER.equals(outputType)) // RxInstance
               		i.remove();
    		}
            
            if(streamList.size() == 0) {
                // System.out.println("SourceTable:handleSourceAction no streams found streamListCount=" + streamList.size());
            	return;
            }
            
            /*
            System.out.println("SourceTable:handleSourceAction sync=" + 
            		mediaSourceListDoc.getRootElement().getAttributeValue(MediaRoomServiceConst.SYNC));
            */

            // Check if we need to stop the outputs in sync
            if (MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction) ) {
            	if(sync) {
            		ArrayList<Element> syncStreamList = new ArrayList<Element>();
            		boolean isPlayback = false;
            		//check do we have any playback is going on device
            		for (Iterator<?> i = streamList.iterator(); i.hasNext();) {
                       	Element streamElement = (Element)i.next();
                      	Element instanceElement = streamElement.getParent().getParent();
                       	Element inputElement = instanceElement.element("Input");
                       	String inputType = MRXMLUtils.getAttributeValue(inputElement, "inputType","");
                       	if(inputType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)){
                       		isPlayback = true;
                       		break;
                       	}
               	    }
                    for (Iterator<?> i = streamList.iterator(); i.hasNext();) {
                    	Element streamElement = (Element)i.next();
                    	Element instanceElement = streamElement.getParent().getParent();
                    	Element msElement = streamElement.getParent();
                    	String inputSyncSourceType = MRXMLUtils.getAttributeValue(msElement,"mediaSourceType");
                    	Element inputElement = instanceElement.element("Input");
                    	long inputSyncTC = Long.parseLong(MRXMLUtils.getAttributeValue(inputElement,"syncTC"));
                    	// Check if file and not sync started then we cannot sync stop it
                    	if( (inputSyncTC == 0) && 
                    		inputSyncSourceType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)) {
                    		continue;
                    	}
                     	String inputType = MRXMLUtils.getAttributeValue(inputElement, "inputType","");
                        
                     	//if playing back a file from downstream don't add those streams to synch stream list                     	
                    	if(isPlayback && inputType.equals(MediaRoomServiceConst.INPUT_RELAY)){
                    		continue;
                    	}                    	
                    	
                		syncStreamList.add(streamElement);
            	    }
                    this.log.addInfo("SourceTable:handleSourceAction sync streams found streamListCount=" + syncStreamList.size(), MRLog.OK, MRLog.NOTIFY);
                    if(!syncStreamList.isEmpty())
                    	this.streamTable.stopSSOutputListSync(syncStreamList, offset);
            	}
            }
 
            String mediaSourceXPath = "//MediaSource[" + streamXPath + "]";
            List<?> mediaSourceList = MRXMLUtils.getListXpath(this.getSourceTableDoc(), mediaSourceXPath);
            // System.out.println("SourceTable:handleSourceAction Processing mediaSourceList size=" + mediaSourceList.size());
            for(Iterator<?> i = mediaSourceList.iterator(); i.hasNext();) {
            	Element msElement = (Element)i.next();
	            Element instanceElement =msElement.getParent();
	            String inputType = instanceElement.element("Input").attributeValue("inputType");
	            if(MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) // Rx input
	            	continue;
	            
	            // If this is a MEDIAFILE, we switch has to be changed to updateOffset      
	            String mediaSourceType = MRXMLUtils.getAttributeValue(msElement,"mediaSourceType");
	            if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(mediaSourceType)) {
	                if (MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(sourceAction) || MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(sourceAction)
	                		|| MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(sourceAction)) {
	                    sourceAction = MediaRoomServiceConst.SOURCE_UPDATEOFFSET;
	            	    timelineUpdate.element("MediaTimeline").element("Last").addAttribute("sourceAction", sourceAction);
	                }
	            }
	
	            // List<?> sourceStreamList = msElement.elements("Stream");
	            List<?> sourceStreamList = MRXMLUtils.getListXpath(msElement, ".//" + streamXPath);
	            com.ipvs.common.XMPPTransaction.startBlock("SourceTable.handleSourceActionInternal");
	            this.updateStreamTimeLineForSource(actionNID, timelineUpdate, 
	            		sourceStreamList, instanceElement, msElement,
	            		sync, syncTC, syncSourceList);
	            com.ipvs.common.XMPPTransaction.endBlock("SourceTable.handleSourceActionInternal");
            }
            if(!syncSourceList.isEmpty()) {
            	// System.out.println("SourceTable::handleSourceAction syncSourceList=" + syncSourceList);
            	this.streamTable.startSSInputListSync(syncSourceList);
            }
        } catch (MRException exp) {
            exp.printStackTrace();
            status = exp.getCode();
            logdata = MediaRoomServiceUtils.generateErrorXML(exp.getCode(), exp.getData());
        } catch (Exception exp) {
            exp.printStackTrace();
            status = MRLog.ERROR;
            logdata = MediaRoomServiceUtils.generateErrorXML(MediaRoomServiceExceptionCodes.OTHERS, exp.toString());
        }

        Iterator<?> i = null;

        if (streamList == null) {
            return;
        }

        i = streamList.iterator();

	    double timescale = Utils.getDoubleValue(timelineUpdate.element("MediaTimeline").element("Last").attributeValue("TimeScale"),1);	  
	    long tc = syncTC;
        if (MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction) ) {
        	timescale = 0;
        }
        // long WC = xmpp.getCurrentTimeMillis();
    	Element last = MRXMLUtils.getElementXpath(timelineUpdate, ".//Last");
        long WC = Long.parseLong(last.attributeValue("WC"));
        while (i.hasNext()) {
            Element sElement = ((Element) i.next());
            String streamNID = MRXMLUtils.getAttributeValue(sElement,"NID");

            Document tmpStreamDataDoc = this.streamTable.getStreamData(streamNID);
            Element outputElement = sElement.element("Output");
            streamURL = MRXMLUtils.getAttributeValue(outputElement,"streamURL");
            Element instanceElement =sElement.getParent().getParent();
            Element inputElement = instanceElement.element("Input");
            if(MediaRoomServiceConst.INPUT_STREAM.equals(inputElement.attributeValue("inputType")) ) // Rx Input
            	continue;
            String relayRoomNID = MRXMLUtils.getAttributeValue(inputElement,"roomNID");
            String relayStreamNID = MRXMLUtils.getAttributeValue(inputElement,"streamNID");

    	    String lastState = timelineUpdate.element("MediaTimeline").element("Last").attributeValue("lastState");
    	    String trackAction = timelineUpdate.element("MediaTimeline").element("Last").attributeValue("action");
    	    String userAction = timelineUpdate.element("MediaTimeline").element("Last").attributeValue("requestedAction");
    	    Element mediaTimelineElement = sElement.element("MediaTimeline");
    	    String timelineID = timelineUpdate.element("MediaTimeline").attributeValue("id");
    	    mediaTimelineElement.addAttribute("id", timelineID);
    	    // System.out.println("SourceSM:handleTimelineUpdate sElement=");
    	    // System.out.println(MRXMLUtils.elementToString(sElement) + "\n sourceTable=" + MRXMLUtils.documentToString(this.getSourceTableDoc()));
    	    MediaTimeline.updateMediaTimelineElement(mediaTimelineElement, 
    	    		WC, offset, tc, timescale, trackAction, lastState, userAction, sourceAction);
    	    
    	    //update error status in stream element
    	    if(status!=MRLog.OK){
    	    	updateErrorInStreamElement(sElement, status);
    	    }
    	    
    	    // This to be replaced with handling the SS Stop Event
            if (MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction) ) {
	    	    long ssTC = this.streamTable.getCurrentTimeStamp(sElement);
	    	    long ssWC = this.xf.xmpp(userJID).getCurrentTimeMillis();
	    	    sElement.addAttribute("ssTC", String.valueOf(ssTC));
	    	    sElement.addAttribute("ssWC", String.valueOf(ssWC));
	    	    MediaTimeline.adjustStreamMediaTimeline(sElement.element("MediaTimeline"), ssTC, ssWC, this.log);
            }
    	    
            String responseActionData = "";

    	    String clientData = "";
    	    String clientCallback = MediaRoomServiceConst.DESTSTREAMAGENT;

            this.streamUtils.sendDestMessage(tmpStreamDataDoc, client, requestNID, streamNID, userAction, "TimelineUpdate", responseActionData, 

            		streamURL, relayRoomNID, relayStreamNID, actionNID, status, logdata, sElement.element("MediaTimeline"),
            		clientData, clientCallback);

        }

        // System.out.println("SourceSM:handleTimelineUpdate sourceTable=" + MRXMLUtils.documentToString(this.getSourceTableDoc()));
        // com.ipvs.common.XMPPTransaction.DEBUG = false;
    }
    
    private void updateErrorInStreamElement(Element streamElement,int state){	    
	    MRXMLUtils.addAttribute(streamElement, SessionServiceConst.ERROR_CODE, ""+ state);	    
    }
    
    private boolean propogateSourceActionToRelaySource(Element instanceElement, Element msElement, Element timelineUpdate, String actionNID)
        throws Exception {
        Element inputElement = instanceElement.element("Input");
	    String sourceaction = timelineUpdate.element("MediaTimeline").element("Last").attributeValue("sourceAction");
        // Check if it is relay
        if (!MediaRoomServiceConst.INPUT_RELAY.equals(MRXMLUtils.getAttributeValue(inputElement,"inputType"))) {
            return false;
        }

        // Do not propogate switch actions
        if (MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(sourceaction) || MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(sourceaction)) {
            return false;
        }

        // Do not propogate start/stop if the source/instance is shared
        String instanceType = MRXMLUtils.getAttributeValue(instanceElement,"instanceType");

        if (MediaRoomServiceConst.SHARED.equals(instanceType)) {
            if (MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(sourceaction) || MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceaction)) {
                return false;
            }
        }

        // Propogate the source action
        String streamNID = MRXMLUtils.getAttributeValue(inputElement,"streamNID");
        if("".equals(streamNID)) {
        	System.out.println("SourceTable:handlePropogateSourceAction Relay streamNID not set inputElement=" + MRXMLUtils.elementToString(inputElement));
        	return false;
        }
        
        String roomNID = MRXMLUtils.getAttributeValue(inputElement,"roomNID");
        Document relayStreamDataDoc = this.getRelayStreamDataDoc(streamNID);
        if(relayStreamDataDoc == null) {
        	System.out.println(
        			"SourceTable:handlePropogateSourceAction Relay stream not found streamNID=" + 
        			streamNID);
        	MediaRoomServiceUtils.printlog(MRXMLUtils.documentToString(this.getSourceTableDoc()) + MRXMLUtils.documentToString(this.getDestTableDoc()));
            throw new MRException(MediaRoomServiceExceptionCodes.RELAYDESTINSTANCENOTFOUND, MRXMLUtils.generateErrorXML("streamNID", streamNID));
        }
        String relaySourceAgentJID = MRXMLUtils.getValueXpath(relayStreamDataDoc, "//sourceAgentJID","");

        // Build the mediaSourceListDataDoc
        Document relayMediaSourceListDataDoc = MRXMLUtils.stringToDocument("<MediaSourceList><MediaSource/></MediaSourceList>");
        Document relayMediaSourceDataDoc = this.getRelayMediaSourceDataDoc(streamNID);
        if(relayMediaSourceDataDoc == null) {
        	System.out.println(
        			"SourceTable:handlePropogateSourceAction Relay mediaSource not found streamNID=" + 
        			streamNID);
        	MediaRoomServiceUtils.printlog(MRXMLUtils.documentToString(this.getSourceTableDoc()) + MRXMLUtils.documentToString(this.getDestTableDoc()));
            throw new MRException(MediaRoomServiceExceptionCodes.RELAYDESTINSTANCENOTFOUND, MRXMLUtils.generateErrorXML("streamNID", streamNID));
        }
        Element mediaSourceElement = MRXMLUtils.getElementXpath(relayMediaSourceListDataDoc, "//MediaSource");
        mediaSourceElement.add((Element)MRXMLUtils.getElementXpath(relayMediaSourceDataDoc, "//Data").clone());
        String mediaSourceNID = MRXMLUtils.getValueXpath(relayStreamDataDoc, "//mediaSourceNID", "");
        MRXMLUtils.addAttribute(mediaSourceElement,"NID", mediaSourceNID);
        
        
        long relayRoomMediaSourceStartTimecode = Utils.getLongValue(MRXMLUtils.getValueXpath(mediaSourceElement, "//startTimecode", null),0);
        if(relayRoomMediaSourceStartTimecode > 0) { // This is a PVR relay stream for live source which has a startTimecode
            long userRoomMediaSourceStartTimecode = Utils.getLongValue(msElement.attributeValue("startTimecode"),0);
            // long timelineOffset = Long.parseLong(actiondata);
            // long relayRoomTimelineOffset = userRoomMediaSourceStartTimecode + timelineOffset - relayRoomMediaSourceStartTimecode;
            // This will ensure that the PVR source will set the tc= mediaSource.startTimecode + relayRoomTimelineOffset
            // This is the same tc value as it would have been on this relay
            long offset = MediaTimeline.getOffsetFromTimelineUpdate(timelineUpdate);
            offset = userRoomMediaSourceStartTimecode + offset - relayRoomMediaSourceStartTimecode;
            MediaTimeline.setOffsetInTimelineUpdate(timelineUpdate, offset);
        }

        Element streamNIDList = MRXMLUtils.stringToElement("<streamNIDList><NID>" + streamNID + "</NID></streamNIDList>");
        MediaTimeline.setStreamNIDListInTimelineUpdate(timelineUpdate, streamNIDList);
        // This is the timelineID that is used when creating the InputInstance in the relaySource
        String timelineID = timelineUpdate.element("MediaTimeline").attributeValue("id");
        String relayTimelineID = MRXMLUtils.getValueXpath(relayMediaSourceDataDoc, "//trackNID","");
        timelineUpdate.element("MediaTimeline").addAttribute("id",relayTimelineID);        
        
        // Generate the actiondata
        Element actiondata = MediaTimeline.createTimelineActionData(timelineUpdate);
        
        // Put back the original timelineID
        timelineUpdate.element("MediaTimeline").addAttribute("id",timelineID);
        String timelineUpdateRequest = StreamAgentUtils.createSourceStreamAgentTimelineUpdateRequestMessage(sourceaction, 
        		actiondata, actionNID, log);
        ServiceAgentUtils.sendAgentRequestMessage(xf.xmpp(userJID), log, roomNID, "",relaySourceAgentJID, 
        		MediaRoomServiceConst.SOURCESTREAMAGENT, MediaRoomServiceConst.TIMELINEUPDATEREQUEST,
            timelineUpdateRequest,
            "AddTrackActionRequestHandler:addTrackAction:=" + this.userJID + "/" + streamNID + " > " + sourceaction + " Message to " +
            ServiceAgentUtils.getAgentShortName(MediaRoomServiceConst.SOURCESTREAMAGENT), "","",MRConst.MEDIA_SERVICE,"");

        this.log.addInfo("SourceTable:=" + this.userJID + ": propogating action to relay source agentJID=" + relaySourceAgentJID, MRLog.OK, MRLog.NOTIFY);

        return true;
    }



    //Check the Available Disk Space For Start and Stop Record output
    private void checkAvailableDiskSpace(Element streamElement) throws MRException, Exception {
        Element outputElement = streamElement.element("Output");
        String outputType = MRXMLUtils.getAttributeValue(outputElement,"outputType");       

        if (!outputType.equals(MediaRoomServiceConst.OUTPUT_RECORD)) {
            return;
        }
        
        String destNID = MRXMLUtils.getAttributeValue(outputElement, "destNID", "");
        String destFileParentDirNID = MRXMLUtils.getAttributeValue(outputElement, "destFileParentDirNID", "");

        MediaRoomServiceUtils.checkAvailableDiskSpace(this.userJID,destNID, destFileParentDirNID);
    }


    private void updateStreamTimeLineForSource(String actionNID, 
    		Element timelineUpdate, 
    		List<?> streamList, Element instanceElement,
        Element msElement,
        boolean sync, long syncTC, Hashtable<String, Element> syncSourceList) throws Exception {
    	
    	long offset = MediaTimeline.getOffsetFromTimelineUpdate(timelineUpdate);
        String timelineID = timelineUpdate.element("MediaTimeline").attributeValue("id");
        double timescale = Utils.getDoubleValue(timelineUpdate.element("MediaTimeline").element("Last").attributeValue("TimeScale"),1);
        
        if (	(timelineID.equals(instanceElement.attributeValue("timelineID"))) &&
        		this.propogateSourceActionToRelaySource(instanceElement, msElement, timelineUpdate, actionNID)) {
        	
        	 Element inputElement = instanceElement.element("Input");
        	 MRXMLUtils.setAttributeValue(inputElement, "timescale", String.valueOf(timescale));
        	   
            this.streamTable.stopSSOuputList(streamList, instanceElement, offset);
            this.streamTable.startSSOuputList(streamList, instanceElement, offset);

            return;
        }

        Iterator<?> i = streamList.iterator();
        Element streamElement = ((Element) i.next());
        String streamNID = MRXMLUtils.getAttributeValue(streamElement,"NID");
        Document streamDataDoc = this.streamTable.getStreamData(streamNID);
        Document mediaSourceDataDoc = this.streamTable.getMediaSourceData(streamNID);        
	    String sourceAction = timelineUpdate.element("MediaTimeline").element("Last").attributeValue("sourceAction");

        if (MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction)) {
            this.streamTable.stopSSOuputList(streamList, instanceElement, offset);

            Element inputElement = instanceElement.element("Input");
            String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");

            if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
            	if(msElement.elements().size() == streamList.size() && streamTable.isSourceSupportFFTracks(inputElement)) // All streams are stopped. 
            		//make sure we are checking source supports FFtracks to take care not stopping the input if we are not started bug #47435
            		this.streamTable.streamingServer.stopSSInput(inputElement, streamElement);
            }

            /*
            try {
                this.syncTrackOffset(trackNID, streamElement, streamDataDoc);
            } catch (Exception e) {
                e.printStackTrace();
            }
            */
        } else if (MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(sourceAction)) {
            checkAvailableDiskSpace(streamElement);
            this.streamTable.startSSOuputList(streamList, instanceElement, offset);
        } else if (	MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(sourceAction) || 
        			MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(sourceAction) || 
        			MediaRoomServiceConst.SOURCE_UPDATEOFFSET.equals(sourceAction)) {
            this.streamTable.stopSSOuputList(streamList, instanceElement, offset);

            
            com.ipvs.common.XMPPTransaction.startBlock("SwitchInput");
            // System.out.println("SourceSM:updateStreamTimeLineForSource timelineUpdate=" + MRXMLUtils.elementToString(timelineUpdate));
            instanceElement = this.switchInput(instanceElement, msElement, streamDataDoc, mediaSourceDataDoc, sourceAction, offset, syncTC, timescale, timelineID, streamList);
            com.ipvs.common.XMPPTransaction.endBlock("SwitchInput");
            this.streamTable.startSSOuputList(streamList, instanceElement, offset);
            String instanceNID = MRXMLUtils.getAttributeValue(instanceElement,"NID");
    	    Element inputEL = instanceElement.element("Input");
    	    String inputType = MRXMLUtils.getAttributeValue(inputEL,"inputType");
    	    
    	    // Support sync start only for media file input sources
            if(sync && MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
            	if(!syncSourceList.containsValue(instanceNID) && streamTable.isSourceSupportFFTracks(inputEL))            		
            		syncSourceList.put(instanceNID, instanceElement);
    	    }
            else {
            	if(streamTable.isSourceSupportFFTracks(inputEL)) {
            		this.streamTable.streamingServer.startSSInput(instanceElement, streamElement);
            	}
            	
            }
        } else {
            throw new MRException(MediaRoomServiceExceptionCodes.ACTIONNOTFOUND, MRXMLUtils.generateErrorXML("updateAction", sourceAction));
        }
    }

    // This should try to switch the inputElement in the current instanceElement
    // if not then it has to create a new instanceElement with the given inputElement
    public Element switchInput(Element instanceElement, Element msElement, 
    		Document streamDataDoc, Document mediaSourceDataDoc, String action, long offset, long syncTC,double timescale, String timelineID, List<?> streamList)
        throws Exception {
        String sourceType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
        String sourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID","");
        String newSourceNID = sourceNID;
        String newSourceType = sourceType;

        if (MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(action)) {
            // Use the original source
            newSourceNID = sourceNID;
            newSourceType = sourceType;
        } else if ((MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(action) || 
        			(MediaRoomServiceConst.SOURCE_UPDATEOFFSET.equals(action)))) { // Lookback Media
            newSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceNID","");
            if("".equals(newSourceNID)) {
            	throw new MRException(MediaRoomServiceExceptionCodes.PVR_NOTAVAILABLE,MRXMLUtils.generateErrorXML("updateAction",action));
            }
            String newSourceAgentJID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceAgentJID","");
            newSourceType = MediaRoomServiceConst.INPUT_MEDIAFILE;
            // Generate the streamDataDoc so that it represents request for lookbackMedia
            // The streamDataDoc passed is used to create the InputInstance
            // If relayNID is set then the InputInstance created is a relayInstance
            streamDataDoc = (Document) streamDataDoc.clone();

            if (this.userJID.equals(newSourceAgentJID)) { // Make sure we are not relaying to ourselves
                MRXMLUtils.setValueXpath(streamDataDoc, "//relayNID", "");                
            }
        } else {
            throw new MRException(MediaRoomServiceExceptionCodes.INVALID_UPDATEACTION,MRXMLUtils.generateErrorXML("updateAction",action));
        }
        return this.streamTable.switchInput(msElement, newSourceNID, newSourceType, streamDataDoc, offset, syncTC, timescale,mediaSourceDataDoc, timelineID, streamList);
    }

	public void handleResponse(String fromJID,Document responseDoc,MRClient client) {
		this.streamTable.handleResponse(fromJID, responseDoc, client);
	}
	
	public Element setupStreamElement(String streamNID, Document streamDataDoc, Document mediaSourceDataDoc, String actionNID) throws Exception {
		return this.streamTable.setupStreamElement(streamNID, streamDataDoc, mediaSourceDataDoc, actionNID);
	}

	public void teardownStreamElement(String streamNID) throws Exception {
		this.streamTable.teardownStreamElement(streamNID);
	}
	
	public Document getStreamDataDoc(String streamNID) {
		return this.streamTable.getStreamData(streamNID);
	}

	public Document getMediaSourceDataDoc(String streamNID) {
		return this.streamTable.getMediaSourceData(streamNID);
	}

	



}
