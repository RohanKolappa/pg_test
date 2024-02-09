package com.barco.device.handler.Media.impl;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.base.DeviceI;
import com.barco.device.base.MediaAgentI;
import com.barco.utils.mr.MRException;
import com.barco.utils.mr.MRLog;
import com.barco.utils.mr.MRMessage;
import com.barco.utils.mr.MRRequest;
import com.barco.utils.mr.MRXMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;
import com.barco.utils.mr.MediaTimeline;
import com.barco.utils.mr.Utils;
import com.barco.utils.mr.XMPPI;
import com.barco.utils.mr.XMPPTransaction;


class StreamData {
	Document streamData;
	Document mediasourceData;
	StreamData(Document streamData, Document mediasourceData) {
		this.streamData = streamData;
		this.mediasourceData = mediasourceData;
	}
}


public abstract class BaseMediaAgentImpl implements MediaAgentI {

	DeviceI device;
	String userJID;
    MRLog log;
    XMPPI xmpp;
    String deviceNID;
    SourceTable sourceTable;
    DeviceTable deviceTable;
    PortTable portTable;
    Hashtable<String, String> destTable;

    // TBD make this protected
	private Hashtable<String, StreamData> streamDataCache;
    
    //public static final int DeleterPortID = 2000;
   
    public static final int INPUT_OK = 3;
    public static final int INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE = 1;
    public static final int INPUT_RELAY_ERROR = 203;

	public abstract void updateInputElement(String inputType, Element inputElement, String streamType,
			Document streamDataDoc, Document mediaSourceDataDoc, Document sourceTableDoc)  throws Exception;
	
	public abstract void setupExternalInput(String inputNID, Element inputElement, String sourceNID, String inputType, String streamType,
			Document streamDataDoc, long offset, long startTimecode, long syncTC, double timescale, Document mediaSourceDataDoc,
			Document sourceTableDoc) throws Exception;
	public abstract void teardownExternalInput(Element inputElement) throws Exception;

	public abstract void updateOutputElement(Element outputElement, String outputType, Document streamDataDoc) throws Exception;
	
	public BaseMediaAgentImpl(String userJID, String deviceNID, String deviceType,
		StreamingServerI streamingServer, MRLog log, XMPPI xmpp,
		Document deviceDoc) {
		this.userJID = userJID;
		this.log = log;
		this.xmpp = xmpp; 
        this.deviceNID = deviceNID;
        this.streamDataCache = new Hashtable<String, StreamData>();
        this.sourceTable = new SourceTable(userJID, deviceNID, streamingServer, log);
        this.deviceTable = new DeviceTable(userJID, deviceType, log, deviceDoc);
        this.portTable = new PortTable(userJID, log);
        this.destTable = new Hashtable<String,String>();
	}

	public String getInstanceType(String sourceType) {
	    String instanceType = MediaRoomServiceConst.SHARED;
	    if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(sourceType)) {
	        instanceType = MediaRoomServiceConst.DEDICATED;
	    }
	    return instanceType;
	}

	public String getInputType(String sourceType, Document streamDataDoc) {
		String inputType = sourceType;
		try {
	        String relayNID = MRXMLUtils.getValueXpath(streamDataDoc, "//relayNID", "");
	        if (!"".equals(relayNID)) {
	            inputType = MediaRoomServiceConst.INPUT_RELAY;
	        }
		}
        catch(Exception e) {this.log.addInfo("DMSMediaAgentImpl:getInputType error=" + e.toString()); }
		return inputType;
	}
    

    public void getStatus(Element rootElement) throws Exception {
	    rootElement.add((Element)portTable.portInstanceListDoc.getRootElement().clone());
	    rootElement.add((Element)deviceTable.deviceInstanceListDoc.getRootElement().clone());
	    rootElement.add((Element)sourceTable.getDoc().getRootElement().clone());
	}
    

	public Document getStreamData(String streamNID) {
		StreamData data = this.streamDataCache.get(streamNID);
		if(data == null)
			return null;
		return data.streamData;
	}

	public Document getMediaSourceData(String streamNID) {
		StreamData data = this.streamDataCache.get(streamNID);
		if(data == null)
			return null;
		return data.mediasourceData;
	}
	
	public void updateMediaSourceData(String streamNID, Document mediaSourceData) {
		StreamData data = this.streamDataCache.get(streamNID);
		if(data != null)
			data.mediasourceData = mediaSourceData;
	}

	
	protected Element createInputElement(String ssInstanceID, String sourceNID, String inputType, String streamType,
			Document streamDataDoc, long offset, long startTimecode, long syncTC, double timescale, Document mediaSourceDataDoc)
	    throws Exception {
	    //this.log.addInfo(this.userJID + ": SourceTable::createInput userJID=" + this.userJID);
		
	    Element inputElement = MRXMLUtils.stringToElement("<Input/>");
	    //String relayNID = MRXMLUtils.getElementXpath(streamDataDoc, "//relayNID").getValue();
	    MRXMLUtils.addAttribute(inputElement,"ssInstanceID", ssInstanceID);	
	    MRXMLUtils.addAttribute(inputElement,"inputType", inputType);
	    MRXMLUtils.addAttribute(inputElement,"streamType", streamType);
	    MRXMLUtils.addAttribute(inputElement,"sourceNID", sourceNID);
	    MRXMLUtils.addAttribute(inputElement,"offset", String.valueOf(offset));
	    MRXMLUtils.addAttribute(inputElement,"roomNID", "");
	    MRXMLUtils.addAttribute(inputElement,"streamNID", "");
	    MRXMLUtils.addAttribute(inputElement,"startTime", "0");
	    MRXMLUtils.addAttribute(inputElement,"startTimecode", String.valueOf(startTimecode));
	    MRXMLUtils.addAttribute(inputElement,"syncTC", String.valueOf(syncTC));
	    MRXMLUtils.addAttribute(inputElement,"timescale", String.valueOf(timescale));

		this.updateInputElement(inputType, inputElement, streamType, streamDataDoc, mediaSourceDataDoc, this.sourceTable.getDoc());

	    return inputElement;
	}

	private Element addSourceTableInstance(String sourceNID, String sourceType, String userRoomMediaSourceNID, 
			Document streamDataDoc, long offset, long syncTC, double timescale, Document mediaSourceDataDoc, String timelineID,
			String sourceTableInstanceNID, String instanceType, String streamType)  throws Exception {
        Element instanceElement = MRXMLUtils.stringToElement("<Instance/>");
        MRXMLUtils.addAttribute(instanceElement,"NID", sourceTableInstanceNID);
        MRXMLUtils.addAttribute(instanceElement,"instanceType", instanceType);
        MRXMLUtils.addAttribute(instanceElement,"timelineID", timelineID);
        String startTimecodeStr = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//startTimecode","");
        long startTimecode = 0;
        try {
            startTimecode = Long.parseLong(startTimecodeStr);
        } catch (Exception e) {
            // Not yet set
        }
        String inputType = this.getInputType(sourceType, streamDataDoc);
	    String ssInstanceID = this.sourceTable.generateID();
		Element inputElement = this.createInputElement(ssInstanceID, sourceNID, inputType, streamType, 
				streamDataDoc, offset, startTimecode, syncTC, timescale, mediaSourceDataDoc);
		

		this.sourceTable.addSourceInstance(instanceElement, inputElement, streamDataDoc, mediaSourceDataDoc);
	    String inputNID = this.deviceTable.addInputInstanceNID(deviceNID, inputElement);
	    MRXMLUtils.addAttribute(inputElement,"NID", inputNID);
	    this.setupExternalInput(inputNID, inputElement, sourceNID, inputType, streamType, streamDataDoc, offset, startTimecode, syncTC, timescale, mediaSourceDataDoc, mediaSourceDataDoc);

	    return instanceElement;
	}
	

    private String generateSourceInstanceNID(String sourceNID, String streamType, String instanceType, String sourceType, String mediaSourceNID, String timelineID) {
        if (MediaRoomServiceConst.SHARED.equals(instanceType)) {
            return sourceNID + "." + streamType + "." + MediaRoomServiceConst.SHARED;
        } else {
            return sourceNID + "." + streamType + "." + mediaSourceNID + "." + timelineID;
        }
    }

	protected Element findOrCreateSourceTableInstance(String sourceNID, String sourceType, String userRoomMediaSourceNID, 
			Document streamDataDoc, long offset, long syncTC, double timescale, Document mediaSourceDataDoc, String timelineID)
	    throws Exception {
		
	    String streamType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
	    String instanceType = this.getInstanceType(sourceType);
	    String sourceTableInstanceNID = this.generateSourceInstanceNID(sourceNID, streamType, instanceType, sourceType, userRoomMediaSourceNID, timelineID);
	    Element instanceElement = MRXMLUtils.getElementXpath(this.sourceTable.getDoc(), "//Instance[@NID='" + sourceTableInstanceNID + "']");
	    if (instanceElement == null) {
	    	instanceElement = this.addSourceTableInstance(sourceNID, sourceType, userRoomMediaSourceNID, 
	    			streamDataDoc, offset, syncTC, timescale, mediaSourceDataDoc, timelineID,
	    			sourceTableInstanceNID, instanceType, streamType);
	    }
	    return instanceElement;
	}
	

	private Element createMediaSourceElement(String roomNID,
			String mediaSourceNID, String mediaSourceType, long startTimecode) throws Exception {
	    Element msElement = MRXMLUtils.stringToElement("<MediaSource/>");
	    MRXMLUtils.addAttribute(msElement,"NID", mediaSourceNID);
	    MRXMLUtils.addAttribute(msElement,"roomNID", roomNID);
	    MRXMLUtils.addAttribute(msElement,"mediaSourceType", mediaSourceType);
	    MRXMLUtils.addAttribute(msElement,"startTimecode", String.valueOf(startTimecode));
	    return msElement;
	}

	/*
	private Element cloneMediaSourceElement(Element msElement) throws Exception {
		return this.createMediaSourceElement(
				msElement.attributeValue("roomNID"),
				msElement.attributeValue("mediaSourceNID"), 
				msElement.attributeValue("mediaSourceType"), 
				Long.parseLong(msElement.attributeValue("startTimecode")));
	}
	*/

	private Element getMediaSourceElement(Element instanceElement, String roomNID,
			String mediaSourceNID, String mediaSourceType, long startTimecode)
	    throws Exception {
	    Element msElement = MRXMLUtils.getElementXpath(instanceElement, "//MediaSource[@NID='" + mediaSourceNID + "']");
	
	    if (msElement == null) {
	    	msElement = this.createMediaSourceElement(roomNID, mediaSourceNID, mediaSourceType, startTimecode);
		    instanceElement.add(msElement);
	    }
	
	    return msElement;
	}
	
	public Element createOutputElement(String ssInstanceNID, String outputType, Document streamDataDoc, Element inputElement) throws Exception {
	    // this.log.addInfo(this.userJID + ": SourceTable::createOutput userJID=" + this.userJID);
	
	    Element outputElement = MRXMLUtils.stringToElement("<Output/>");
	    MRXMLUtils.addAttribute(outputElement,"ssInstanceID", ssInstanceNID);
	    MRXMLUtils.addAttribute(outputElement,"outputType", outputType);
    	String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID","");
	    MRXMLUtils.addAttribute(outputElement,"destNID", destNID);

	    // Set the streamType and destNID
	    String streamType = MRXMLUtils.getAttributeValue(inputElement,"streamType");
	    MRXMLUtils.addAttribute(outputElement,"streamType", streamType);
	    MRXMLUtils.addAttribute(outputElement,"trickPlay", MRXMLUtils.getValueXpath(streamDataDoc, "//trickPlay", ""));

		this.updateOutputElement(outputElement, outputType, streamDataDoc);

	    return outputElement;
	}
	
	/*
	 * Starts the stream using the information provided in the streamDataDoc
	 */
	public Element getSourceTableInstance(DeviceI device, String requestNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc)
	    throws Exception {
	    XMPPTransaction.startBlock("SourceTable.getStreamElement");
	
	    // Checks to see if the requestor needs to connect to a specific port
	    String tcpPort = MRXMLUtils.getValueXpath(streamDataDoc, "//callbackPort", "");
	    if(!"".equals(tcpPort)) { // Unmanaged Rx port, check to see if overlaps with configuredPool
	    	int tcpPortNumber = Integer.parseInt(tcpPort);
	    	if (this.sourceTable.streamingServer.getNetworkResource().isReservedTCPPort(tcpPortNumber)){
	    		throw new MRException(MediaRoomServiceExceptionCodes.OVERLAPPING_UNAMANAGED_V2D_DEST_PORT, MRXMLUtils.generateErrorXML("tcpPort", tcpPort));
	    	}
	    }

	    //log.addInfo(this.userJID + ": SourceTable: getInstanceElement - Start", MRLog.OK, MRLog.NOTIFY);
	    XMPPTransaction.startBlock("SourceTable.getInstanceElement");
	
	    String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID","");
	    String userRoomMediaSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID","");
	    if("".equals(userRoomMediaSourceNID))
	    	userRoomMediaSourceNID = mediaSourceNID;
	    Element instanceElement = null;
	    
	    //default timescale 
	    double timescale =1;
	    
	    // Get/Create the instanceElement - 
	    String timelineID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID","");
	    String trackStateAtStart = MRXMLUtils.getValueXpath(streamDataDoc, "//trackStateAtStart", "");
	    long trackOffsetAtStart = Long.parseLong(MRXMLUtils.getValueXpath(streamDataDoc, "//trackOffsetAtStart", ""));
	    long mediaOffset = trackOffsetAtStart; // TBD account for startOffset
	    long syncTC = 0;
	    
	    // Check if we need to switch to Lookback Source
	    String sourceType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
	    if(MediaRoomServiceConst.INPUT_ENCODER.equals(sourceType) &&
	    		(MediaRoomServiceConst.TRACK_STARTED_LOOKBACK.equals(trackStateAtStart) ||
	    		 MediaRoomServiceConst.TRACK_STOPPED_LOOKBACK.equals(trackStateAtStart)) ) { 
	        String lookbackSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceNID","");
	        String lookbackSourceAgentJID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceAgentJID","");
	        String lookbackSourceType = MediaRoomServiceConst.INPUT_MEDIAFILE;
	        Document newStreamDataDoc = (Document) streamDataDoc.clone();
	        if (this.userJID.equals(lookbackSourceAgentJID)) { // Make sure we are not relaying to ourselves
	            MRXMLUtils.setValueXpath(newStreamDataDoc, "//relayNID", "");
	        }
	        instanceElement = this.findOrCreateSourceTableInstance(lookbackSourceNID, lookbackSourceType, userRoomMediaSourceNID, newStreamDataDoc, mediaOffset, syncTC, timescale, mediaSourceDataDoc, timelineID);
	    }
	    else {
		    String sourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID","");
	    	instanceElement = this.findOrCreateSourceTableInstance(sourceNID, sourceType, userRoomMediaSourceNID, streamDataDoc, mediaOffset, syncTC, timescale, mediaSourceDataDoc, timelineID);
		}

	    XMPPTransaction.endBlock("SourceTable.getInstanceElement");
	    
	    return instanceElement;
	}

	void updateMediaTimeline(String inputType, Element instanceElement, Element inputElement, Element streamElement, 
			Document streamDataDoc, long mediaOffset) throws Exception {
	    Element mediaTimeline = MediaTimeline.createMediaTimelineElement(log);
		long now = System.currentTimeMillis();
		Element start = mediaTimeline.element("Start");
		start.addAttribute("WC", String.valueOf(now));		
		start.addAttribute("Offset", String.valueOf(mediaOffset));
		long tc = Long.parseLong(inputElement.attributeValue("startTimecode"));
		if(tc == 0)
			tc = now;

		start.addAttribute("TC", String.valueOf(tc));
	    streamElement.add(mediaTimeline);

	    long timescale = 1;
	    String trackStateAtStart = MRXMLUtils.getValueXpath(streamDataDoc, "//trackStateAtStart", "");
	    if(		MediaRoomServiceConst.TRACK_STOPPED_CURRENT.equals(trackStateAtStart) ||
	    		MediaRoomServiceConst.TRACK_STOPPED_LOOKBACK.equals(trackStateAtStart) ) {
	    	this.sourceTable.stopSSOutputAt(instanceElement, streamElement, mediaOffset);
	    	timescale = 0;
            if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
                this.sourceTable.streamingServer.stopSSInput(inputElement, streamElement);
            }
	    }

	    MediaTimeline.updateMediaTimelineElement(streamElement.element("MediaTimeline"), 
	    		now, mediaOffset, tc,timescale,
	    		MediaRoomServiceConst.START, trackStateAtStart, MediaRoomServiceConst.START, MediaRoomServiceConst.START);
	}


	public Element startStream(DeviceI device, String requestNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc)
		    throws Exception {
		
	    Element streamElement = MRXMLUtils.getElementXpath(this.sourceTable.getDoc(), "//Stream[@NID='" + streamNID + "']");
	    if (streamElement != null)
	        return streamElement;


	    // Get the sourceTableInstance
		Element instanceElement = this.getSourceTableInstance(device, requestNID, streamNID, streamDataDoc, mediaSourceDataDoc);
	    Element inputElement = instanceElement.element("Input");
        String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");
        int state = Integer.parseInt(MRXMLUtils.getAttributeValue(inputElement, "state", "0"));
        if((state == BaseMediaAgentImpl.INPUT_RELAY_ERROR))
        	throw new MRException(MediaRoomServiceExceptionCodes.RELAY_STREAM_ERROR, "Relay Stream has error");


	    // Get the mediaSourceElement
        XMPPTransaction.startBlock("SourceTable.getMediaSourceElement");
	    String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID","");
	    String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID","");
	    String sourceType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
	    String timecode = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//startTimecode","");
	    long startTimecode = 0;
	    try {startTimecode = Long.parseLong(timecode);} catch (Exception e) {}
	    Element mediaSourceElement = this.getMediaSourceElement(instanceElement, roomNID, mediaSourceNID, sourceType, startTimecode);
	    XMPPTransaction.endBlock("SourceTable.getMediaSourceElement");
	
	    // Add the stream Element to the mediaSourceElement
	    String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");
	    XMPPTransaction.startBlock("SourceTable.addStreamElement");
	    streamElement = MRXMLUtils.stringToElement("<Stream/>");
	    MRXMLUtils.addAttribute(streamElement,"NID", streamNID);
	    MRXMLUtils.addAttribute(streamElement,"destAgentJID", destAgentJID);
	    MRXMLUtils.addAttribute(streamElement,"roomNID", roomNID);
	    MRXMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
	    mediaSourceElement.add(streamElement);
	    
	    // Add the outputElement for the destination in the stream
	    long trackOffsetAtStart = Long.parseLong(MRXMLUtils.getValueXpath(streamDataDoc, "//trackOffsetAtStart", "0"));
	    long mediaOffset = trackOffsetAtStart; // TBD account for startOffset
	    String userRoomMediaSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID","");
	    String destType = MRXMLUtils.getValueXpath(streamDataDoc, "//destType","");
	    String sourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID","");
	    String outputType = destType;
     	if(MediaRoomServiceConst.OUTPUT_DECODER.equals(outputType)) 
     		outputType = MediaRoomServiceConst.OUTPUT_STREAM;	    // We are on a relay 
	    String ssInstanceID = this.sourceTable.generateID();
		Element outputElement = this.createOutputElement(ssInstanceID, outputType, streamDataDoc, inputElement);
		this.sourceTable.addOutput(instanceElement, outputElement, inputElement, streamDataDoc, streamElement, mediaOffset, roomNID, streamNID, userRoomMediaSourceNID);
	    String outputNID = this.deviceTable.addOutputInstanceNID(deviceNID, outputElement);
	    this.portTable.addPortInstance(roomNID, streamNID, sourceNID, inputElement, outputElement);
	    MRXMLUtils.addAttribute(outputElement,"NID", outputNID);

	    // Update the mediaTimeline
	    this.updateMediaTimeline(inputType, instanceElement, inputElement, streamElement, streamDataDoc, mediaOffset);
	    
	    // Add the stream and mediaSource to the cache
	    this.streamDataCache.put(streamNID, new StreamData(streamDataDoc, mediaSourceDataDoc));	    
	    return streamElement;
	}

	public void releaseMediaSourceElement(Element msElement) throws Exception {
	    Element instanceElement = msElement.getParent();
	    instanceElement.remove(msElement);
	
	    if (instanceElement.element("MediaSource") == null) {
	    	deleteSourceTableInstance(instanceElement);
	    }
	}
	
	public void deleteSourceTableInstance(Element instanceElement) throws Exception {
        Element inputElement = instanceElement.element("Input");
	    this.sourceTable.deleteSourceInstance(instanceElement);
	    String inputNID = MRXMLUtils.getAttributeValue(inputElement,"NID");
	    this.deviceTable.deleteInputInstanceNID(deviceNID, inputNID, inputElement);
	    this.teardownExternalInput(inputElement);
	}

	public Element stopStream(String streamNID) throws Exception {
	
		XMPPTransaction.startBlock("SourceTable.releaseStreamElement");
	
	    String streamXPath = "//Stream[@NID='" + streamNID + "']";
	    Element streamElement = MRXMLUtils.getElementXpath(this.sourceTable.getDoc(), streamXPath);
	
	    if (streamElement == null) {
    		throw new MRException(MediaRoomServiceExceptionCodes.NOTFOUND, "Stream not found for streamNID=" + streamNID);
	    }
	
	    Document streamDataDoc = null;
	    if(this.streamDataCache.containsKey(streamNID))
	    		streamDataDoc = this.streamDataCache.get(streamNID).streamData;
	
	    Element msElement = streamElement.getParent();
	
	    //Get the Instances ID's to delete the instances from device,src port and stream  
	    Element instanceElement = msElement.getParent();
	    Element inputElement = instanceElement.element("Input");
	    Element outputElement = MRXMLUtils.getElementXpath(streamElement, "Output");
	    
	    this.sourceTable.deleteOutput(instanceElement, streamElement, inputElement, outputElement, streamNID, streamDataDoc);
        this.deviceTable.deleteOutputInstanceNID(deviceNID, MRXMLUtils.getAttributeValue(outputElement,"NID"), outputElement);
	    this.portTable.deletePortInstance(streamNID);

	    msElement.remove(streamElement);
	    this.streamDataCache.remove(streamNID);
	
	    // If mediaSourceNID is empty remove mediaSourceNID
	    // If instanceNID is empty remove instanceNID
	    if (msElement.elements().isEmpty()) {
	        this.releaseMediaSourceElement(msElement);
	    }
	
	    log.addInfo("SourceTable::releaseStreamElement streamCache size=" + this.streamDataCache.size() +
	    		MRXMLUtils.documentToString(this.sourceTable.getDoc()));
	    
	
	    XMPPTransaction.endBlock("SourceTable.releaseStreamElement");
	    
	    return streamElement;
	    
	}
	
	public void addDest(String streamNID, String sourceAgentJID) {
		this.destTable.put(streamNID, sourceAgentJID);		
	}
	
	public String removeDest(String streamNID) {
		String sourceAgentJID = this.destTable.get(streamNID);
		this.destTable.remove(streamNID);
		return sourceAgentJID;
	}

	public void setDevice(DeviceI device) {
		this.device = device;
	}

	/*
	////////////////////////////////////////
	// Rx Instance methods
	//////////////////////////////////////////
	public void createRxInstance(MRClient client, Document doc, String action, String streamNID, String streamURL, Document streamDataDoc, Document mediaSourceDataDoc, String actionNID)
	throws Exception {
		
		// System.out.println("SourceTable:createRxInstance");
		String sourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID", "");
		String mediaSourceType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType", "");
		String inputType = MediaRoomServiceConst.INPUT_STREAM;
		String instanceType = MediaRoomServiceConst.DEDICATED;
		long offset = 0;
		long startTimecode = 0;
		long syncTC = 0;
		double timescale =1;
		String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID", "");
		String streamType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
	
		//Element inputElement = createInput(sourceNID, inputType, streamDataDoc, offset, startTimecode, mediaSourceDataDoc);
		//Element instanceElement = getInstanceElement(null, sourceNID, inputType, mediaSourceNID, streamDataDoc, offset, mediaSourceDataDoc);
		String timelineID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID", "");
		String sourceTableInstanceNID = this.buildSourceTableInstanceNID(sourceNID, streamType, instanceType, inputType, mediaSourceNID, timelineID);
		Element inputElement = this.createInput(sourceNID, inputType, streamType,
				streamDataDoc, offset, startTimecode, syncTC,timescale, mediaSourceDataDoc);
		 
		Element instanceElement = MRXMLUtils.stringToElement("<Instance/>");
		MRXMLUtils.addAttribute(instanceElement,"NID", sourceTableInstanceNID);
		MRXMLUtils.addAttribute(instanceElement,"instanceType", instanceType);
		instanceElement.add(inputElement);
		MRXMLUtils.addAttribute(inputElement,"streamURL", streamURL);
		MRXMLUtils.addAttribute(inputElement,"streamNID", streamNID);

		// Carry over the enable Kbm from input stream
    	String enablekbm = MRXMLUtils.getValueXpath(streamDataDoc, "//EnableKbm", "false");
		MRXMLUtils.addAttribute(inputElement,"EnableKbm", enablekbm);
		
		this.sourceTableDoc.getRootElement().add(instanceElement);
		
		//Element outputElement = createOutput(streamDataDoc, inputElement);
		Element outputElement = null;
		try {
			outputElement = createOutput(MediaRoomServiceConst.OUTPUT_DECODER, null, inputElement);
		}catch(Exception e) {
			throw e;
		}
		
	    Element streamElement = MRXMLUtils.stringToElement("<Stream/>");
	    MRXMLUtils.addAttribute(streamElement,"NID", streamNID);
	    // Update the destAgentJID        
	    String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");
	    String roomNID =  MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID","");
	    MRXMLUtils.addAttribute(streamElement,"roomNID", roomNID);
	    MRXMLUtils.addAttribute(streamElement,"destAgentJID", destAgentJID);
	    MRXMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
	    streamElement.add(outputElement);
	    Element mediaSourceElement = MRXMLUtils.stringToElement("<MediaSource/>");
	    MRXMLUtils.addAttribute(mediaSourceElement,"NID", mediaSourceNID);
	    MRXMLUtils.addAttribute(mediaSourceElement,"mediaSourceType", mediaSourceType);
	    MRXMLUtils.addAttribute(mediaSourceElement,"startTimecode", String.valueOf(startTimecode));
	    mediaSourceElement.add(streamElement);
	    instanceElement.add(mediaSourceElement);
	    this.startSSOutput(instanceElement, streamElement , offset);
	    
		try {// TBR when testing with actual RX
			this.streamingServer.updateSSNetworkInput(inputElement, streamDataDoc, action);
		}catch(Exception e) {
			MRXMLUtils.setAttributeValue(streamElement, "state", String.valueOf(MediaRoomServiceConst.START_ERROR));
			throw e;
		}
	    MediaRoomServiceUtils.printlog("SourceTable:createRxInstance sourceTable=" + MRXMLUtils.documentToString(this.sourceTableDoc));
	}
	public void deleteRxInstance(String streamNID) throws Exception {
		Element inputElement = null;
		inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@streamNID='" + streamNID + "']");
		if(inputElement == null) {
			return;
		}	
		String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");
		if(!MediaRoomServiceConst.INPUT_STREAM.equals(inputType))
			return;
		Element instanceElement = null;
		try {
			instanceElement = MRXMLUtils.getParentElement(inputElement);
			if(instanceElement ==null) {
				return;
			}
		    Element msElement = instanceElement.element("MediaSource");
		    Element streamElement = msElement.element("Stream");
		    Element outputElement = streamElement.element("Output");
		    this.deleteOutput(outputElement, null);
		} catch(Exception exp){
			exp.printStackTrace();
		}
		this.releaseInstanceElement(instanceElement);
		
	}

	*/
    
	/*
    public Element switchInput(Element msElement, String newSourceNID, String newSourceType, Document streamDataDoc, long offset, long syncTC,double timescale, Document mediaSourceDataDoc, String timelineID,
    		List<?> streamList) throws Exception {
    	
        XMPPTransaction.startBlock("ReleaseMediaSourceElement");
    	if(msElement.elements().size() == streamList.size()) {
    		this.releaseMediaSourceElement(msElement);
    	}
    	else {
    	    Element newMSElement = this.cloneMediaSourceElement(msElement);
    		for(Object o:streamList) {
    			Element streamElement = (Element)o;
    			msElement.remove(streamElement);
    			newMSElement.add(streamElement);
    		}
    		msElement = newMSElement;
    	}
        XMPPTransaction.endBlock("ReleaseMediaSourceElement");

        // Get the new instanceElement for the mediaSourceNID
        String mediaSourceNID = MRXMLUtils.getAttributeValue(msElement,"NID");
        XMPPTransaction.startBlock("GetInstanceElement");

        Element newInstanceElement = this.getInstanceElement(MRRequest.createRequest(userJID), newSourceNID, newSourceType, mediaSourceNID, streamDataDoc, offset, syncTC,timescale, mediaSourceDataDoc, timelineID);
        XMPPTransaction.endBlock("GetInstanceElement");
        // Add the mediaSourceElement to the instanceElement
        newInstanceElement.add(msElement);
        this.portTable.updatePortInstance(msElement, newInstanceElement);
        MediaRoomServiceUtils.printlog("SourceTable:switchInput sourceTable=" + MRXMLUtils.documentToString(this.sourceTableDoc));

        return newInstanceElement;
    }
    */

	/*
    ////////////////////////////////////////////////
    // Static Methods Used by StreamUtils
    //////////////////////////////////////////////////
    
	private static String childrenToString(Element parent) throws MRException, Exception {
	    if (parent == null) {
	        return "";
	    }
	    List<?> children = parent.elements();
	    StringBuffer sb = new StringBuffer();
	    for (Iterator<?> i = children.iterator(); i.hasNext();) {
	        sb.append(MRXMLUtils.elementToString((Element) i.next()));
	    }
	    return sb.toString();
	}

	private void sendMediaSourceEvent( 
			MRClient client,
			Element instanceEL,
			int eventLevel,
			String eventName,
			Element mediaSourceEventData) throws Exception {
		List<?> mediaSourceList = MRXMLUtils.getListXpath(instanceEL, ".//MediaSource");
		for(Object o:mediaSourceList) {
			Element mediaSource = (Element)o;
            String mediaSourceNID = MRXMLUtils.getAttributeValue(mediaSource,"NID");
            MRXMLUtils.getElementXpath(mediaSourceEventData, ".//mediaSourceNID").setText(mediaSourceNID);
            String roomNID = MRXMLUtils.getAttributeValue(mediaSource,"roomNID");
            String info = "SourceStreamAgent:=" + client.getUserJID() + 
            	"/" + mediaSourceNID + " > MediaSourceEvent";
            long eventWallclock = client.getXmpp().getCurrentTimeMillis();
            String logStr = "";
            ServiceAgentUtils.sendAgentEvent(
            		client.getXmpp(),
            		client.getLog(), 
            		roomNID, 
            		MediaRoomServiceConst.SOURCESTREAMAGENT, 
            		MediaRoomServiceConst.WILDCARD, 
            		MediaRoomServiceConst.DESTSTREAMAGENT, 
            		eventLevel, 
            		client.getUserJID(), 
            		eventWallclock, 
            		MediaRoomServiceConst.RELAYEVENT, 
            		mediaSourceEventData, 
            		logStr, 
            		info);
        }
    }

	public void sendMessagToAllDestinationsInternal(List<?> streamsList, MRClient client, String action, int eventCode, String actiondata, int status, String logdata, String streamList,
	        long startTC) throws MRException, Exception {
        Iterator<?> objStreamListIterator = streamsList.iterator();
        Element streamEL = null;
        String outputStreamNID = null;
        String outputRoomNID = null;
        Document streamDataDoc = null;
        StringBuffer streamListBuff = null;

        while (objStreamListIterator.hasNext()) {       	
            streamListBuff = new StringBuffer("");
            streamListBuff.append(streamList);
            streamEL = (Element) objStreamListIterator.next();
            outputStreamNID = MRXMLUtils.getAttributeValue(streamEL,"NID");
            outputRoomNID = MRXMLUtils.getAttributeValue(streamEL,"roomNID");
            streamListBuff.append(StreamTable.getStreamNIDXML(outputStreamNID));
            streamDataDoc = streamDataCache.get(outputStreamNID).streamData; 

            if (streamDataDoc != null) {
                String name = MediaRoomServiceConst.DESTSTREAMAGENT;
                String info = "SourceStreamAgent:=" + client.getUserJID() + "/" + outputStreamNID + " > SourceEvent Message action=" + action + " to " +
                    ServiceAgentUtils.getAgentShortName(name);
                Document response = StreamAgentUtils.createSourceEventMessage(outputStreamNID, action, eventCode, streamListBuff.toString(), startTC, actiondata, status, logdata, client.getLog());
                long eventWallclock = client.getXmpp().getCurrentTimeMillis();
                int eventLevel = status;
                ServiceAgentUtils.sendAgentEvent(client.getXmpp(),client.getLog(), outputRoomNID, "", MediaRoomServiceConst.WILDCARD, MediaRoomServiceConst.DESTSTREAMAGENT, eventLevel, client.getUserJID(), eventWallclock, MediaRoomServiceConst.SOURCEEVENT, response.getRootElement(), "", info);
            }
        }
    }

	public void sendMessageToAllDestinations(MRClient client, Document doc, String action, String actiondata)
	throws MRException, Exception {
		//perf-logs this.log.addInfo(this.userJID + ": SourceTable: propogateMessageToDestinations - Start", MRLog.OK, MRLog.NOTIFY);
		String streamNID = MRXMLUtils.getValueXpath(doc, "//streamNID", "");
	
		if ((streamNID == null)) {
		    return;
		}
	
		Element instanceEL = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Instance[Input/@streamNID='" + streamNID + "']");
		
		List<?> streamList = null;
	
		if (instanceEL == null) {
		    instanceEL = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Stream[@NID='" + streamNID + "']");
		    
		    if (instanceEL == null) {
		        return;
		    }
	
		    List<Element> outputStreams = new ArrayList<Element>();
		    outputStreams.add(instanceEL);
		    streamList = new ArrayList<Element>(outputStreams);
		} else {
		    streamList = MRXMLUtils.getListXpath(instanceEL, "MediaSource/Stream");
		}		
		this.sendMessageToStreamList(client, doc, action, actiondata, streamList);
	}
	
	public void sendMessageToStreamList(MRClient client, Document doc, String action, String actiondata, 
			List<?> streamList)throws MRException, Exception {
					
		if ((streamList == null) || (streamList.size() == 0)) {
		    return;
		}
	
		String status = MRXMLUtils.getValueXpath(doc, "//status", "");
	
		if ((status == null) || (status.trim().length() == 0)) {
		    status = "" + MRLog.ERROR;
		}
	
		String logData = StreamTable.childrenToString(MRXMLUtils.getElementXpath(doc, "//log"));
		String streamNIDList = StreamTable.childrenToString(MRXMLUtils.getElementXpath(doc, "//streamNIDList"));
		String startTC = MRXMLUtils.getValueXpath(doc, "//startTC", "");
		String strEventCode = MRXMLUtils.getValueXpath(doc, "//eventCode", "");
	
		this.sendMessagToAllDestinationsInternal(streamList, client, action,
		    (((strEventCode == null) || (strEventCode.trim().length() == 0)) ? MediaRoomServiceConst.UNKNOWN_EVENT : Integer.parseInt(strEventCode)), actiondata, Integer.parseInt(status), logData,
		    streamNIDList, ((startTC == null) || (startTC.trim().length() == 0)) ? 0 : Long.parseLong(startTC));
	
		//perf-logs this.log.addInfo(this.userJID + ": SourceTable: propogateMessageToDestinations - End", MRLog.OK, MRLog.NOTIFY);
	}

    public static String getStreamNIDXML(String streamNID) {
        return "<streamNIDListItem>" + streamNID + "</streamNIDListItem>";
    }
    
    */

}
