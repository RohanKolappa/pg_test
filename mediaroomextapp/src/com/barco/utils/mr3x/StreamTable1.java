package com.barco.utils.mr;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRMessage;
import com.ipvs.common.service.MRRequest;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceConst;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceExceptionCodes;
import com.ipvs.mediaroomservice.impl.MediaRoomServiceUtils;
import com.ipvs.mediaroomservice.impl.MediaTimeline;
import com.ipvs.mediaroomservice.impl.SourceStreamAgent;
import com.ipvs.mediaroomservice.impl.StreamAgentUtils;
import com.ipvs.mediaroomservice.streamingserver.StreamingServerI;
import com.ipvs.mediastoreservice.impl.MediaStoreAdminServerAgent;
import com.ipvs.mediastoreservice.impl.MediaStoreServerI;
import com.ipvs.sessionservice.impl.SessionServiceConst;
import com.ipvs.utils.Utils;

class StreamData1 {
	Document streamData;
	Document mediasourceData;
	StreamData(Document streamData, Document mediasourceData) {
		this.streamData = streamData;
		this.mediasourceData = mediasourceData;
	}
}

public class StreamTable1 {

	String userJID;
    MRLog log;
    XMPPI xmpp;
    long runningNumber = 2000;
    Document sourceTableDoc;
    DeviceTable deviceTable;
    PortTable portTable;
    String deviceNID;
    
    // TBD make this protected
	private Hashtable<String, StreamData> streamDataCache;
    public StreamingServerI streamingServer;
    private ServiceAgentFactoryI serviceAgentFactory;
    
    //public static final int DeleterPortID = 2000;
   
    public static final int INPUT_OK = 3;
    private static final int INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE = 1;
   // private static final int INPUT_RELAY_WAITFORADDSTREAMACTIONRESPONSE = 2;
    public static final int INPUT_RELAY_ERROR = 203;

    
	public StreamTable(String userJID, String deviceNID, String deviceType,
		StreamingServerI streamingServer, MRLog log, XMPPI xmpp,ServiceAgentFactoryI serviceAgentFactory) {
		this.userJID = userJID;
		this.streamingServer = streamingServer;
		this.log = log;
		this.xmpp = xmpp; 
		this.serviceAgentFactory = serviceAgentFactory; 
        try {
        	this.sourceTableDoc = MRXMLUtils.stringToDocument("<SourceTable/>");
        	if(deviceNID != null)
        	    MRXMLUtils.addAttribute(this.sourceTableDoc.getRootElement(),"NID", deviceNID);
        	MRXMLUtils.addAttribute(this.sourceTableDoc.getRootElement(),"userJID",userJID);
        }
        catch(Exception e) {
        	e.printStackTrace();
        }
        this.deviceTable = new DeviceTable(userJID, deviceType, log);
        this.portTable = new PortTable(userJID, log);
        this.deviceNID = deviceNID;
        this.streamDataCache = new Hashtable<String, StreamData>();
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
	
    public void getStatus(Element rootElement) throws Exception {
	    rootElement.add((Element)portTable.portInstanceListDoc.getRootElement().clone());
	    rootElement.add((Element)deviceTable.deviceInstanceListDoc.getRootElement().clone());
	    rootElement.add((Element)sourceTableDoc.getRootElement().clone());
	}
    
    public List<?> getStreamList(String streamXPath) throws Exception {
	    return MRXMLUtils.getListXpath(this.sourceTableDoc, streamXPath);
    }

    public Element getStream(String streamXPath) throws Exception {
	    return MRXMLUtils.getElementXpath(this.sourceTableDoc, streamXPath);
    }

    private String generateID() {
        this.runningNumber++;
        return String.valueOf(runningNumber);
    }

    private String buildSourceTableInstanceNID(String sourceNID, String streamType, String instanceType, String sourceType, String mediaSourceNID, String timelineID) {
        if (MediaRoomServiceConst.SHARED.equals(instanceType)) {
            return sourceNID + "." + streamType + "." + MediaRoomServiceConst.SHARED;
        } else {
            return sourceNID + "." + streamType + "." + mediaSourceNID + "." + timelineID;
        }
    }

    // Stop the Streaming Server Output Instance
    public void stopSSOuputList(List<?> streamList, Element instanceElement, long offset)
        throws MRException, Exception {
	    Iterator<?> streamListIterator = streamList.iterator();
	    while (streamListIterator.hasNext()) {
	        Element streamElement = (Element) streamListIterator.next();
	        this.stopSSOutputAt(instanceElement, streamElement, offset);
	    }
    }
    
    public void stopSSOutputListSync(List<Element> streamElementList, long offset) throws Exception {
    	// System.out.println("SourceTable:stopSSOutputListSync streamElementList count=" + streamElementList.size());
        this.streamingServer.stopSSOutputAtSync(streamElementList, offset);
        for(Iterator<Element> i = streamElementList.iterator(); i.hasNext();)
            MRXMLUtils.addAttribute(i.next(),"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
	}
    
    public void startSSInputListSync(Hashtable<String,Element> syncSourceList) throws Exception {
    	// System.out.println("SourceTable:stopSSOutputListSync streamElementList count=" + streamElementList.size());
    	this.streamingServer.startSSInputAtSync(syncSourceList);
	}

    // Start the Streaming Server Output Instance
    public void startSSOuputList(List<?> streamList, Element instanceElement, long offset)
        throws MRException, Exception {
        Iterator<?> streamListIterator = streamList.iterator();

        while (streamListIterator.hasNext()) {
            Element streamElement = (Element) streamListIterator.next();
            this.startSSOutput(instanceElement, streamElement, offset);
        }
    }

    public void startSSOutput(Element instanceElement, Element streamElement, long offset) throws Exception {
	    int state = Integer.parseInt(MRXMLUtils.getAttributeValue(streamElement,"state"));
	
	    if (state != MediaRoomServiceConst.STREAM_STARTED && isDestinationSupportedTrickPlay(streamElement, instanceElement)) {	    	
	        this.streamingServer.startSSOutput(instanceElement, streamElement);
	        MRXMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STARTED));
	    }
	}
    
    private boolean isDestinationSupportedTrickPlay(Element streamElement, Element instanceElement) {  
    	
    	Element outputElement = streamElement.element("Output");
        Element inputElement = instanceElement.element("Input");
       	boolean trickPlay = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(outputElement, "trickPlay"), false);
    	double timescale = Utils.getDoubleValue(MRXMLUtils.getAttributeValue(inputElement, "timescale"), 1);     	
    	return ((timescale > 1 || timescale < 1) && !trickPlay) ?  false : true;
	}
    
    
    //if the input is media file verify the ffTracks is enabled or not for slow motion and fast forward    
    public boolean isSourceSupportFFTracks(Element inputEL){
    	
//    	String inputType = MRXMLUtils.getAttributeValue(inputEL,"inputType");
//    	
//    	if(MediaRoomServiceConst.INPUT_ENCODER.equals(inputType) || MediaRoomServiceConst.INPUT_RELAY.equals(inputType) 
//        		|| MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) {
//    		return true;
//    	}
           
       	double timescale = Utils.getDoubleValue(MRXMLUtils.getAttributeValue(inputEL, "timescale"), 1);        
    	boolean isInputSupportFFTracks = Utils.getBooleanValue(MRXMLUtils.getAttributeValue(inputEL, "ffTracks"), false);	    
    	return (timescale > 1 && !isInputSupportFFTracks) ? false : true;    	
    }
    
    public void stopSSOutputAt(Element instanceElement, Element streamElement, long offset)throws Exception {
	    int state = Integer.parseInt(MRXMLUtils.getAttributeValue(streamElement,"state"));
	
	    if (state != MediaRoomServiceConst.STREAM_STOPPED) {
	        this.streamingServer.stopSSOutputAt(instanceElement, streamElement, offset);
	        MRXMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
	    }
	}

    public long getCurrentTimeStamp(Element streamElement) throws MRException,Exception {
    	Element outputElement = MRXMLUtils.getElementXpath(streamElement, "Output");
    	return this.streamingServer.getCurrentTC(outputElement);
    }

    public long getOffset(String mediaUUID,String timecodeStr, String srcDirPath) throws MRException,Exception {
    	if(mediaUUID.isEmpty()) {
    		throw new MRException(MediaRoomServiceExceptionCodes.MEDIA_UUID_NOT_FOUND,"Media UUID Not Found");
    	}           	
    	long timecode = Long.parseLong(timecodeStr);              
    	return this.streamingServer.getOffsetAtTC(timecode,mediaUUID,srcDirPath);
    }
    
    public long getSyncTC(String sourceUUID, long offset,String rootDir) throws Exception {
    	return this.streamingServer.getTCAtOffset(sourceUUID, offset, rootDir);
    }

    private String getParentMediaSourceNID(Document mediaSourceDataDoc, String mediaSourceNID) throws Exception {
        String userRoomMediaSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID", "");
        if("".equals(userRoomMediaSourceNID))
        	userRoomMediaSourceNID = mediaSourceNID;
        return userRoomMediaSourceNID;
    }

	private Element createInput(String sourceNID, String inputType, String streamType,
			Document streamDataDoc, long offset, long startTimecode, long syncTC, double timescale, Document mediaSourceDataDoc)
	    throws Exception {
		
		
	    this.log.addInfo(this.userJID + ": SourceTable::createInput userJID=" + this.userJID);
	
	    Element inputElement = MRXMLUtils.stringToElement("<Input/>");
	    //String relayNID = MRXMLUtils.getElementXpath(streamDataDoc, "//relayNID").getValue();
	    String ssInstanceNID = this.generateID();
	    MRXMLUtils.addAttribute(inputElement,"ssInstanceID", ssInstanceNID);
	
	
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
	    
	    // Set the inputType and input type specific parameters
	    if(MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) { // For an Rx Input
	        String destPortElementID = MRXMLUtils.getValueXpath(streamDataDoc, "//destPortElementID", "");
	        MRXMLUtils.addAttribute(inputElement,"destPortElementID", destPortElementID);	     
	    }
	    else if(MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
	        String mediaUUID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileUUID", "");
	        String mediaParentDirNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileParentDirNID", "");
	        MRXMLUtils.addAttribute(inputElement,"mediaUUID", mediaUUID);
	        MRXMLUtils.addAttribute(inputElement,"mediaParentDirNID", mediaParentDirNID);
	        updateMediaStorePath(inputElement, mediaParentDirNID);	  
	        
	        //add ffTracks supported or not
	        String ffTracks = MRXMLUtils.getValueXpath(streamDataDoc, "//FFTracks", "");
	        if(ffTracks.isEmpty()){
	        	MRXMLUtils.addAttribute(inputElement,"ffTracks",String.valueOf(Boolean.FALSE));
	        } else {
	        	MRXMLUtils.addAttribute(inputElement,"ffTracks",String.valueOf(Boolean.TRUE));
	        }
	        
	    } else if(MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)){ // For a Tx Input
		    String sourcePortElementID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourcePortElementID", "");
    		String isMulticast = MRXMLUtils.getValueXpath(streamDataDoc, "//IsMulticast", "false");
		    MRXMLUtils.addAttribute(inputElement,"sourcePortElementID", sourcePortElementID);	    	
            MRXMLUtils.addAttribute(inputElement,"isMulticast", isMulticast);	    	
		    
            // Check if we have other inputs that use the same sourcePortElementID
        	Element prevInputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc,"//Input[@sourcePortElementID='" + sourcePortElementID + "']");
        	if(prevInputElement != null) { // Check if we can reuse the sourcePortElement
        		String prevIsMulticast = prevInputElement.attributeValue("isMulticast");
                if("false".equals(isMulticast) || "false".equals(prevIsMulticast)) {
                	throw new MRException(MediaRoomServiceExceptionCodes.ENCODERPORTNOTAVAILABLE, "Encoder Port Not Available");
                }
            }
        	else {
	        	// Check if we have enough encoder resources by finding the number of unique sourcePortElementId
	        	Hashtable<String,String> portIDTable = new Hashtable<String, String>();
	        	List<?> inputList = MRXMLUtils.getListXpath(this.sourceTableDoc, "//Input[@type='" + MediaRoomServiceConst.INPUT_ENCODER + "']");
	        	for(Object i : inputList) {
	        		String portID = ((Element)i).attributeValue("sourcePortElementID");
	        		portIDTable.put(portID, portID);
	        	}
	        	int maxEncoderInstances = this.streamingServer.getMaxEncoderInstances(streamType);
	            if((maxEncoderInstances > 0) && (portIDTable.size() >= maxEncoderInstances))
                	throw new MRException(MediaRoomServiceExceptionCodes.MAXENCODERINSTANCESEXCEEDED, "Max Encoder Instances Exceeded");
        	}
	    } else if(MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
	    	// No attributes to add
	    } else {
    		throw new MRException(MediaRoomServiceExceptionCodes.INVALID_INPUT_TYPE,"Invalid InputType=" + inputType);
	    }
	
	    this.streamingServer.createSSInput(inputElement, streamDataDoc, mediaSourceDataDoc);
	    
	    //Moved from Get instance Element Method -- to start the relay stream using input nid -- by padmaja
	    String deviceNID = MRXMLUtils.getAttributeValue(this.sourceTableDoc.getRootElement(),"NID");
	    String inputNID = this.deviceTable.addInputInstanceNID(deviceNID, inputElement);
	    MRXMLUtils.addAttribute(inputElement,"NID", inputNID);
	
	    String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID", "");
	    String userRoomMediaSourceNID = this.getParentMediaSourceNID(mediaSourceDataDoc, mediaSourceNID);
	    if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
	        com.ipvs.common.XMPPTransaction.startBlock("SourceTable.createRelay");
	        createRelayRoom(sourceNID, offset, startTimecode, syncTC, streamDataDoc, inputNID, userRoomMediaSourceNID);
	        com.ipvs.common.XMPPTransaction.endBlock("SourceTable.createRelay");
	        MRXMLUtils.addAttribute(inputElement,"state", String.valueOf(StreamTable.INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE));
	    }
	    else {
	        MRXMLUtils.addAttribute(inputElement,"state", String.valueOf(StreamTable.INPUT_OK));
	    }
	    return inputElement;
	}
	
	public void updateMediaStorePath(Element element,String parentDirNID)  throws MRException,Exception {
		MediaStoreAdminServerAgent mediaStoreAdminServerAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(this.userJID);
    	String rootPath = mediaStoreAdminServerAgent.getMediaStoreRootPathBasedOnDirNID(parentDirNID);    	
    	MRXMLUtils.addAttribute(element,"dirPath", rootPath);		
	}

	private Element createOutput(String outputType, Document streamDataDoc, Element inputElement) throws Exception {
	    this.log.addInfo(this.userJID + ": SourceTable::createOutput userJID=" + this.userJID);
	
	    Element outputElement = MRXMLUtils.stringToElement("<Output/>");
	    MRXMLUtils.addAttribute(outputElement,"ssInstanceID", this.generateID());
	    String destNID = "";
	    
	    // Set output specific parameters
	    if(MediaRoomServiceConst.OUTPUT_DECODER.equals(outputType)) { // For an Rx Input
		    String destPortElementID = MRXMLUtils.getAttributeValue(inputElement,"destPortElementID");
		    MRXMLUtils.addAttribute(outputElement,"destPortElementID", destPortElementID);
	    } else if(MediaRoomServiceConst.OUTPUT_STREAM.equals(outputType)) {
	    	destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID","");
	    } else if(outputType.equals(MediaRoomServiceConst.OUTPUT_RECORD) || 
		    		outputType.equals(MediaRoomServiceConst.OUTPUT_PVR)) {
	    	destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//destNID","");
	    	String destFileUUID  = MRXMLUtils.getValueXpath(streamDataDoc, "//destFileUUID","");
	    	String destFileParentDirNID  = MRXMLUtils.getValueXpath(streamDataDoc, "//destFileParentDirNID","");
	    	MRXMLUtils.addAttribute(outputElement,"destFileUUID", destFileUUID);
	    	MRXMLUtils.addAttribute(outputElement,"destFileParentDirNID", destFileParentDirNID);
	    	updateMediaStorePath(outputElement,destFileParentDirNID);
	    	initRecordOrPVRFile(streamDataDoc,destNID,destFileUUID,destFileParentDirNID);	    	
	    } else {
    		throw new MRException(MediaRoomServiceExceptionCodes.INVALID_OUTPUT_TYPE,"Invalid OutputType=" + outputType);
	    }

	    
	    MRXMLUtils.addAttribute(outputElement,"outputType", outputType);
	    // Set the streamType and destNID
	    String streamType = MRXMLUtils.getAttributeValue(inputElement,"streamType");
	    MRXMLUtils.addAttribute(outputElement,"streamType", streamType);
	    MRXMLUtils.addAttribute(outputElement,"destNID", destNID);
	    MRXMLUtils.addAttribute(outputElement,"trickPlay", MRXMLUtils.getValueXpath(streamDataDoc, "//trickPlay", ""));

	    //perf-logs this.log.addInfo("SourceTable::createSSOutput - start" + this.userJID, MRLog.OK, MRLog.NOTIFY);
	    this.streamingServer.createSSOutput(outputElement, inputElement, streamDataDoc);
	    /*if(streamDataDoc == null) {
	    	return outputElement;
	    }*/
	    
	    String outputNID = this.deviceTable.addOutputInstanceNID(deviceNID, outputElement);
	    MRXMLUtils.addAttribute(outputElement,"NID", outputNID);
	
	    //perf-logs this.log.addInfo("SourceTable::createSSOutput - End" + this.userJID, MRLog.OK, MRLog.NOTIFY);
	    return outputElement;
	}
    
    private void initRecordOrPVRFile(Document streamDataDoc,String destNID, String destFileUUID, String destFileParentDirNID) throws MRException,Exception {    	
    	MediaStoreAdminServerAgent storeAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(this.userJID);
    	MediaStoreServerI mediaStoreServer = storeAgent.getMediaStoreServer();
    	String rootDir = storeAgent.getMediaStoreRootPathBasedOnDirNID(destFileParentDirNID);
    	Element actionDataElement = MRXMLUtils.stringToElement("<UpdateProfileRequestData><streamType>"+MRXMLUtils.getValueXpath(streamDataDoc, "//streamType", "")+"</streamType></UpdateProfileRequestData>");
    	Element profileXML = MRXMLUtils.getElementXpath(streamDataDoc, "//profileXML");    	
    	MRXMLUtils.addElement(actionDataElement,(Element)profileXML.clone()); 
    	
    	if(mediaStoreServer.isMediaStoreExist(rootDir, destFileUUID)) {
      		log.addInfo("Media Store File Exist mediaStoreFile:="+destFileUUID +" mediaStreamFile:="+destNID,MRLog.INFO,MRLog.NOTIFY);
      		return;
      	}
    	
    	log.addInfo("Media Store File Does Not Exist mediaStoreFile:="+destFileUUID +" mediaStreamFile:="+destNID,MRLog.INFO,MRLog.NOTIFY);      	    	
    	
    	mediaStoreServer.initRecordOrPVRFile(destFileUUID, destNID, MRXMLUtils.elementToDocument(actionDataElement), rootDir, null, null);    
	}

	private void deleteInput(Element inputElement) throws Exception {
	    this.log.addInfo(this.userJID + ": SourceTable:deleteInput deleting ssInstanceID=" + MRXMLUtils.getAttributeValue(inputElement,"ssInstanceID"));
	
	    this.streamingServer.deleteSSInput(inputElement);
	
	    String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");
	    String ssInstanceID = MRXMLUtils.getAttributeValue(inputElement,"ssInstanceID");
	
	    if (MediaRoomServiceConst.INPUT_RELAY.equals(inputType)) {
	        this.log.addInfo(this.userJID + ": SourceTable:delete Relay ssInstanceID=" + ssInstanceID);
	
	        @SuppressWarnings("unused")
	        String streamNID = MRXMLUtils.getAttributeValue(inputElement,"streamNID");
	        String roomNID =MRXMLUtils.getAttributeValue( inputElement,"roomNID");
	
	        this.log.addInfo(this.userJID + ":SourceTable:deleteInput Started shutdown of Relay roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
	
	        /*
	        @SuppressWarnings("unused")
	        Document response = this.addRelayInputStreamAction(streamNID, "Stop");
	        this.deletedRelayStreams.add(streamNID);
	        */
	        this.deleteRelayRoom(roomNID);
	    }
	
	    String inputNID = MRXMLUtils.getAttributeValue(inputElement,"NID");
	    String deviceNID = MRXMLUtils.getAttributeValue(this.sourceTableDoc.getRootElement(),"NID");
	    this.deviceTable.deleteInputInstanceNID(deviceNID, inputNID, inputElement);
	}

	private void deleteOutput(Element outputElement, Document streamDataDoc) throws Exception {
		
	    try {
	        this.log.addInfo(this.userJID + ": SourceTable:deleteOutput deleting ssInstanceID=" + MRXMLUtils.getAttributeValue(outputElement,"ssInstanceID"));
	        this.streamingServer.deleteSSOutput(outputElement, streamDataDoc);
	        this.deviceTable.deleteOutputInstanceNID(deviceNID, MRXMLUtils.getAttributeValue(outputElement,"NID"), outputElement);
	    } catch (Exception exp) {
	        //Ignore the Error
	    	exp.printStackTrace();
	        this.log.addInfo(this.userJID + ": Warning : SourceTabel Delete Output Failed : " + exp.toString());
	    }
	}


    private void releaseInstanceElement(Element instanceElement) throws Exception {
        Element sourceTableElement = instanceElement.getParent();
        Element inputElement = instanceElement.element("Input");
        deleteInput(inputElement);

        // String deviceNID = this.sourceTableDoc.getRootElement().getAttributeValue("NID");
        // this.deleteInputInstanceNID(deviceNID, inputElement.getAttributeValue("NID"), inputElement);
        sourceTableElement.remove(instanceElement);
    }



    
	private Element getInstanceElement(MRRequest request, String sourceNID, String sourceType, String userRoomMediaSourceNID, 
			Document streamDataDoc, long offset, long syncTC, double timescale, Document mediaSourceDataDoc, String timelineID)
	    throws Exception {
		
		// System.out.println("SourceTable:getInstanceElement");

	    String instanceType = MediaRoomServiceConst.SHARED;

	    String streamType = MRXMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
	
	    if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(sourceType)) {
	        instanceType = MediaRoomServiceConst.DEDICATED;
	    }
	
	    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: buildSourceTableInstanceNID - Start", MRLog.OK, MRLog.NOTIFY);
	    String sourceTableInstanceNID = this.buildSourceTableInstanceNID(sourceNID, streamType, instanceType, sourceType, userRoomMediaSourceNID, timelineID);
	
	    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: buildSourceTableInstanceNID - End", MRLog.OK, MRLog.NOTIFY);
	
	    // Get the instance Element
	    Element instanceElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Instance[@NID='" + sourceTableInstanceNID + "']");
	
	    if (instanceElement == null) {
	        instanceElement = MRXMLUtils.stringToElement("<Instance/>");
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
	
	        // Set the input Type
	        // DVITest: Get the streamType from the stream
	        // String streamType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//streamType", "");

	        String relayNID = MRXMLUtils.getValueXpath(streamDataDoc, "//relayNID", "");
	        String inputType = sourceType;
	        if (!"".equals(relayNID)) {
	            inputType = MediaRoomServiceConst.INPUT_RELAY;
	        }
	
	        Element inputElement = this.createInput(sourceNID, inputType, streamType,
	        		streamDataDoc, offset, startTimecode, syncTC, timescale,mediaSourceDataDoc);
	        instanceElement.add(inputElement);
	        this.sourceTableDoc.getRootElement().add(instanceElement);
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

	private Element cloneMediaSourceElement(Element msElement) throws Exception {
		return this.createMediaSourceElement(
				msElement.attributeValue("roomNID"),
				msElement.attributeValue("mediaSourceNID"), 
				msElement.attributeValue("mediaSourceType"), 
				Long.parseLong(msElement.attributeValue("startTimecode")));
	}

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

	public Element setupStreamElement(String streamNID, Document streamDataDoc, Document mediaSourceDataDoc, String actionNID)
	    throws Exception {
	    com.ipvs.common.XMPPTransaction.startBlock("SourceTable.getStreamElement");
	
	    Element streamElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Stream[@NID='" + streamNID + "']");
	
	    if (streamElement != null) {
	        return streamElement;
	    }
	    
	    String tcpPort = MRXMLUtils.getValueXpath(streamDataDoc, "//callbackPort", "");
	    if(!"".equals(tcpPort)) { // Unmanaged Rx port, check to see if overlaps with configuredPool
	    	int tcpPortNumber = Integer.parseInt(tcpPort);
	    	if (this.streamingServer.getNetworkResource().isReservedTCPPort(tcpPortNumber)){
	    		throw new MRException(MediaRoomServiceExceptionCodes.OVERLAPPING_UNAMANAGED_V2D_DEST_PORT, MRXMLUtils.generateErrorXML("tcpPort", tcpPort));
	    	}
	    }

	    String requestNID = "requestNID";
	    Document doc = MRXMLUtils.stringToDocument("<tmp/>");
	    MRRequest request = MRRequest.createRequest(userJID, MRLog.INFO, requestNID, "serviceName", "requestName", new MRMessage(doc), "");
	    String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID","");
	    String mediaSourceNID = MRXMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID","");
	    String sourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID","");
	    String userRoomMediaSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID","");
	    String sourceType = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
	    String offset = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//startOffset","");
	    String timecode = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//startTimecode","");
	
	    long startOffset = 0;
	
	    try {
	        startOffset = Long.parseLong(offset);
	    } catch (Exception e) {
	        // Invalid or null offset
	    }
	
	    long startTimecode = 0;
	
	    try {
	        startTimecode = Long.parseLong(timecode);
	    } catch (Exception e) {
	        // Invalid or null offset
	    }
	
	    long syncTC = 0;
	    
	    //log.addInfo(this.userJID + ": SourceTable: getInstanceElement - Start", MRLog.OK, MRLog.NOTIFY);
	    com.ipvs.common.XMPPTransaction.startBlock("SourceTable.getInstanceElement");
	
	    if("".equals(userRoomMediaSourceNID))
	    	userRoomMediaSourceNID = mediaSourceNID;
	    
	    Element instanceElement = null;
	    
	    //default timescale 
	    double timescale =1;
	    
	    // Check if the stream has to join at an offset
	    String trackStateAtStart = MRXMLUtils.getValueXpath(streamDataDoc, "//trackStateAtStart", "");
	    long trackOffsetAtStart = Long.parseLong(MRXMLUtils.getValueXpath(streamDataDoc, "//trackOffsetAtStart", ""));
	    long mediaOffset = trackOffsetAtStart; // TBD account for startOffset
	    String timelineID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID","");
	    if(MediaRoomServiceConst.INPUT_ENCODER.equals(sourceType) &&
	    		(MediaRoomServiceConst.TRACK_STARTED_LOOKBACK.equals(trackStateAtStart) ||
	    		 MediaRoomServiceConst.TRACK_STOPPED_LOOKBACK.equals(trackStateAtStart)) ) { // Need to switch to lookback source
	        String newSourceNID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceNID","");
	        String newSourceAgentJID = MRXMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceAgentJID","");
	        String newSourceType = MediaRoomServiceConst.INPUT_MEDIAFILE;
	        Document newStreamDataDoc = (Document) streamDataDoc.clone();
	        if (this.userJID.equals(newSourceAgentJID)) { // Make sure we are not relaying to ourselves
	            MRXMLUtils.setValueXpath(newStreamDataDoc, "//relayNID", "");
	        }
	    	instanceElement = this.getInstanceElement(request, newSourceNID, newSourceType, userRoomMediaSourceNID, newStreamDataDoc, mediaOffset, syncTC, timescale, mediaSourceDataDoc, timelineID);
	    }
	    else {
	    	instanceElement = this.getInstanceElement(request, sourceNID, sourceType, userRoomMediaSourceNID, streamDataDoc, mediaOffset, syncTC, timescale, mediaSourceDataDoc, timelineID);
	    }
	    com.ipvs.common.XMPPTransaction.endBlock("SourceTable.getInstanceElement");
	
	    //  log.addInfo(this.userJID + ": SourceTable: getInstanceElement - End", MRLog.OK, MRLog.NOTIFY);
	    Element inputElement = instanceElement.element("Input");
        String inputType = MRXMLUtils.getAttributeValue(inputElement,"inputType");
        int state = Integer.parseInt(MRXMLUtils.getAttributeValue(inputElement, "state", "0"));
        if((state == StreamTable.INPUT_RELAY_ERROR))
        	throw new MRException(MediaRoomServiceExceptionCodes.RELAY_STREAM_ERROR, "Relay Stream has error");
	
	    // Get the mediaSource Element
	    com.ipvs.common.XMPPTransaction.startBlock("SourceTable.getMediaSourceElement");
	
	    Element msElement = this.getMediaSourceElement(instanceElement, roomNID, mediaSourceNID, sourceType, startTimecode);
	    com.ipvs.common.XMPPTransaction.endBlock("SourceTable.getMediaSourceElement");
	
	    // Add the stream Element
	    com.ipvs.common.XMPPTransaction.startBlock("SourceTable.addStreamElement");
	
	    streamElement = MRXMLUtils.stringToElement("<Stream/>");
	    MRXMLUtils.addAttribute(streamElement,"NID", streamNID);
	
	    // Update the destAgentJID        
	    String destAgentJID = MRXMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");
	    MRXMLUtils.addAttribute(streamElement,"destAgentJID", destAgentJID);
	    MRXMLUtils.addAttribute(streamElement,"roomNID", roomNID);
	    MRXMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
	    Element mediaTimeline = MediaTimeline.createMediaTimelineElement(log);
		long now = xmpp.getCurrentTimeMillis();
		Element start = mediaTimeline.element("Start");
		start.addAttribute("WC", String.valueOf(now));		
		start.addAttribute("Offset", String.valueOf(offset));
		long tc = now;
		if(MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
	        String mediaUUID = inputElement.attributeValue("mediaUUID");
	        String mediaParentDirNID = inputElement.attributeValue("mediaParentDirNID");
            MediaStoreAdminServerAgent storeAdminAgent = MediaStoreAdminServerAgent.getMediaStoreAgent(this.userJID);
            String rootDir = storeAdminAgent.getMediaStoreRootPathBasedOnDirNID(mediaParentDirNID);
	        tc = this.getSyncTC(mediaUUID, mediaOffset, rootDir);
		}
		start.addAttribute("TC", String.valueOf(tc));
	    streamElement.add(mediaTimeline);
	
	    msElement.add(streamElement);
	    this.streamDataCache.put(streamNID, new StreamData(streamDataDoc, mediaSourceDataDoc));
	    // System.out.println("SourceTable::getStreamElement streamCache size=" + this.streamDataCache.size());
	
	    // Set the outputType
	    String destType = MRXMLUtils.getValueXpath(streamDataDoc, "//destType","");
	    String outputType = destType;
     	if(MediaRoomServiceConst.OUTPUT_DECODER.equals(outputType)) 
     		outputType = MediaRoomServiceConst.OUTPUT_STREAM;	    // We are on a relay 

	    Element outputElement = this.createOutput(outputType, streamDataDoc, inputElement);
	    streamElement.add(outputElement);
	
	    this.startSSOutput(instanceElement, streamElement, startOffset);
	
	    this.streamingServer.startSSInput(instanceElement, streamElement);
	
	    long t = xmpp.getCurrentTimeMillis();
	
	    this.log.addInfo(this.userJID + ": SourceTable::getStreamURL setInstanceInfo ...");
	
	    this.portTable.addPortInstance(roomNID, streamNID, sourceNID, inputElement, outputElement);
	
	    //perf-logs this.log.addInfo(this.userJID + ": SourceTable: updateStreamInstance - End", MRLog.OK, MRLog.NOTIFY);
	    this.log.addInfo(this.userJID + ": SourceTable::getStreamURL setInstanceInfo ... done=" + (System.currentTimeMillis() - t));
	
	    com.ipvs.common.XMPPTransaction.endBlock("SourceTable.addStreamElement");
	    com.ipvs.common.XMPPTransaction.endBlock("SourceTable.getStreamElement");
	    MediaRoomServiceUtils.printlog("SourceTable:getStreamElement sourceTable=" + MRXMLUtils.documentToString(this.sourceTableDoc));
	    if(		MediaRoomServiceConst.TRACK_STOPPED_CURRENT.equals(trackStateAtStart) ||
	    		MediaRoomServiceConst.TRACK_STOPPED_LOOKBACK.equals(trackStateAtStart) ) {
	    	this.stopSSOutputAt(instanceElement, streamElement, trackOffsetAtStart);
	    	 timescale =0;
            if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
                this.streamingServer.stopSSInput(inputElement, streamElement);
            }
	    }

	    MediaTimeline.updateMediaTimelineElement(streamElement.element("MediaTimeline"), 
	    		now, mediaOffset, tc,timescale,
	    		MediaRoomServiceConst.START, trackStateAtStart, MediaRoomServiceConst.START, MediaRoomServiceConst.START);
	    return streamElement;
	}
		

	public void releaseMediaSourceElement(Element msElement) throws Exception {
	    Element instanceElement = msElement.getParent();
	    instanceElement.remove(msElement);
	
	    if (instanceElement.element("MediaSource") != null) {
	        return;
	    }
	    this.releaseInstanceElement(instanceElement);
	}

	public void teardownStreamElement(String streamNID) throws Exception {
	
		com.ipvs.common.XMPPTransaction.startBlock("SourceTable.releaseStreamElement");
	
	    String streamXPath = "//Stream[@NID='" + streamNID + "']";
	    Element streamElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, streamXPath);
	
	    if (streamElement == null) {
	        this.log.addInfo("SourceAgent: Failed to find sourceTable entry for stream NID=" + streamNID);
	
	        return;
	    }
	
	    Document streamDataDoc = null;
	    if(this.streamDataCache.containsKey(streamNID))
	    		streamDataDoc = this.streamDataCache.get(streamNID).streamData;
	
	    Element msElement = streamElement.getParent();
	
	    //Get the Instances ID's to delete the instances from device,src port and stream  
	    Element instanceElement = msElement.getParent();
	    Element inputElement = instanceElement.element("Input");
	    Element outputElement = MRXMLUtils.getElementXpath(streamElement, "Output");
	
	    this.stopSSOutputAt(instanceElement, streamElement, Long.parseLong(MRXMLUtils.getAttributeValue(inputElement,"offset")));
	    deleteOutput(outputElement, streamDataDoc);
		    
	    msElement.remove(streamElement);
	    this.streamDataCache.remove(streamNID);
	
	    // If mediaSourceNID is empty remove mediaSourceNID
	    // If instanceNID is empty remove instanceNID
	    if (msElement.elements().isEmpty()) {
	        this.releaseMediaSourceElement(msElement);
	    }
	
	    this.portTable.deletePortInstance(streamNID);
	    MediaRoomServiceUtils.printlog("SourceTable::releaseStreamElement streamCache size=" + this.streamDataCache.size() +
	    		MRXMLUtils.documentToString(this.sourceTableDoc));
	    
	
	    com.ipvs.common.XMPPTransaction.endBlock("SourceTable.releaseStreamElement");
	    
	}

	/////////////////////////////////////////////////////////
	// Relay input methods
	//////////////////////////////////////////////////////////
	private void createRelayRoom(String sourceNID, long offset, long startTimecode, long syncTC,
			Document streamDataDoc, String inputNID,
			String userRoomMediaSourceNID)     throws Exception {
	    String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + SourceStreamAgent.CREATERELAYCONNECTION_REQUEST_XML;
	    Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	    MRXMLUtils.setValueXpath(requestDoc, "//clientdata",inputNID);
	
	    MRXMLUtils.setValueXpath(requestDoc, "//sourceNID",sourceNID);
	    MRXMLUtils.setValueXpath(requestDoc, "//name",this.userJID);
	    MRXMLUtils.setValueXpath(requestDoc, "//description",MediaRoomServiceConst.RELAY_ROOM);
	    
	    //If the connection is live then change the start time code to start timecode + offset.
	    if(startTimecode > 0) {
	         startTimecode += offset;
	         offset=0;
	    }
	    
		MRXMLUtils.setValueXpath(requestDoc, "//startOffset",String.valueOf(offset));
	    MRXMLUtils.setValueXpath(requestDoc, "//startTimecode",String.valueOf(startTimecode));
	
	    String profileNID = MRXMLUtils.getValueXpath(streamDataDoc, "//profileNID","");
	    MRXMLUtils.getElementXpath(requestDoc, "//profileNID").setText(profileNID);
	    
	    Element profileXML = MRXMLUtils.getElementXpath(streamDataDoc, "//profileXML");
	    MRXMLUtils.copyContent(requestDoc, "//profileXML" , profileXML);
	    
	    String destNID = MRXMLUtils.getValueXpath(streamDataDoc, "//relayNID","");
	    MRXMLUtils.getElementXpath(requestDoc, "//destNID").setText(destNID);
	    MRXMLUtils.getElementXpath(requestDoc, "//relayNID").setText(MediaRoomServiceConst.COMPUTERELAYUSINGPOLICY);
	    MRXMLUtils.getElementXpath(requestDoc, "//userRoomMediaSourceNID").setText(userRoomMediaSourceNID);
	
	    String roomNID = null;
	    String agentJID =  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory);
	    String agentClassName = AppServerAgent.class.getName();
	    String fromAgentClassName = SourceStreamAgent.class.getName();
	    String info = "SourceTable::createRelayRoom > Sending Request CreateRelayConnection";
	
	    String requestNID = "none";
	    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, fromAgentClassName, roomNID, agentJID, agentClassName, requestDoc, info, requestNID);
	}
	private void deleteRelayRoom(String roomNID) throws Exception {
	    if ((roomNID == null) || (roomNID.length() == 0)) {
	        return;
	    }
	
	    log.addInfo("SourceTable:=" + this.userJID + ":deleteRelayRoom roomNID=" + roomNID, MRLog.OK, MRLog.NOTIFY);
	
	    String xmlFilePath = "/com/ipvs/mediaroomservice/xml/" + SourceStreamAgent.DELETEMEDIAROOM_REQUEST_XML;
	    Document requestDoc = MRXMLUtils.loadXMLResource(xmlFilePath, log);
	    MRXMLUtils.getElementXpath(requestDoc, "//roomNID").setText(roomNID);
	
	    String agentJID =  AppServerAgentUtils.getAppServerAgentJID(userJID,serviceAgentFactory);
	    String agentClassName = AppServerAgent.class.getName();
	    String fromAgentClassName = SourceStreamAgent.class.getName();
	    String info = "SourceTable::deleteRelayRoom > Sending Request DeletMediaRoom";
	
	    String requestNID = "none";
	    ServiceAgentUtils.sendAgentRequestMessage(xmpp, log, fromAgentClassName, null, agentJID, agentClassName, requestDoc, info, requestNID);
	}

	public void handleRelayInputStreamStarted(Element inputElement, String streamNID, String streamURL, String action, Document streamDataDoc) throws Exception {
	    log.addInfo("SourceTable:=" + this.userJID + " processing relayInpuStreamStarted streamNID=" + streamNID, MRLog.OK, MRLog.NOTIFY);
	    MRXMLUtils.addAttribute(inputElement,"streamURL", streamURL);
	    this.streamingServer.updateSSNetworkInput(inputElement, streamDataDoc, action);
	}
	
	public void stopSSOutputOnDiskFull(Element streamElement, int eventCode) throws Exception {
		Element mediaSourceEl = streamElement.getParent();
		Element instanceEl = mediaSourceEl.getParent();
		Element inputEl = instanceEl.element("Input");
		long offset = Utils.getLongValue(MRXMLUtils.getAttributeValue(inputEl,"offset"), 0);
		streamElement.addAttribute(SessionServiceConst.ERROR_CODE, String.valueOf(eventCode));
		stopSSOutputAt(instanceEl, streamElement, offset);
	}


	private boolean checkResponseOK(Document responseDoc,MRClient client, Element inputElement) {
		try {
			String stateStr = MRXMLUtils.getValueXpath(responseDoc, "//header/state", "");
			int state = 0;
			try {
				state = Integer.parseInt(stateStr);
			} catch(Exception e){}
			if(state == MRRequest.STATE_DONE)
				return true;
			
			Element exceptionEL = MRXMLUtils.getElementXpath(responseDoc, "//Exception");
			String streamNID = MRXMLUtils.getAttributeValue(inputElement, "streamNID", null);    			  
			if(streamNID != null) { 
				String actionData = MRXMLUtils.elementToString(exceptionEL);
				Document doc = StreamAgentUtils.createSourceEventMessage(streamNID, 
						MediaRoomServiceConst.SOURCEEVENT, MediaRoomServiceConst.RELAY_SOURCE_STARTED,
						StreamTable.getStreamNIDXML(streamNID), 0, actionData, MRLog.ERROR, "", log);
				
				
				//Document doc = MRXMLUtils.stringToDocument(sourceEventXML);
				// String actionData = MRXMLUtils.getValueXpath(doc, "//sourceEventActionData", "");
				// this.handleSourceEvents(client, doc, MediaRoomServiceConst.SOURCEEVENT, actionData);
				Element instanceEL = inputElement.getParent();
			    List<?> streamsList = MRXMLUtils.getListXpath(instanceEL, "MediaSource/Stream");
				this.sendMessageToStreamList(client, doc, MediaRoomServiceConst.SOURCEEVENT, actionData, streamsList);
				for(Object streamElement: streamsList) {
					stopStream(MRXMLUtils.getAttributeValue((Element)streamElement, "NID"));	
				}
				
			}
		} catch(MRException exp){
			exp.printStackTrace();
		}catch(Exception exp){
			exp.printStackTrace();
		}
		return false;
	}

	private void handleAddStreamActionResponse(String fromJID, Document responseDoc,MRClient client) throws Exception {
		String inputNID = MRXMLUtils.getValueXpath(responseDoc, "//clientdata", "");
    	Element inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@NID='" + inputNID + "']");
    	if(inputElement == null)
    		return;
	   	if(!checkResponseOK(responseDoc, client, inputElement)) {	   		
	        return;
	   	}
	   	MRXMLUtils.addAttribute(inputElement,"state", String.valueOf(StreamTable.INPUT_OK));	    
	}
	
	private void handleCreateRelayConnectionResponse(String fromJID, Document responseDoc,MRClient client) throws Exception {
		String inputNID = MRXMLUtils.getValueXpath(responseDoc, "//clientdata", "");
    	Element inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@NID='" + inputNID + "']");
		String roomNID = MRXMLUtils.getValueXpath(responseDoc, "//roomNID", "");
    	if(inputElement == null) {
			this.deleteRelayRoom(roomNID);
    		return;
    	}
	   	if(!checkResponseOK(responseDoc, client, inputElement)) {	   	    	   			   	    
	        return;
	   	}
	   	
	    String streamNID = MRXMLUtils.getValueXpath(responseDoc, "//streamNID", "");		 
	   	int inputElementState = Utils.getIntValue(MRXMLUtils.getAttributeValue(inputElement, "state"),0);
	   	
	   	if(inputElementState == StreamTable.INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE){
	   		updateRealyRoomInfoInInputElement(inputElement,roomNID, streamNID);
	   	}
	    
	    MediaRoomServiceUtils.printlog("StreamTable:handleCreateRelayConnectionResponse roomNID=" + roomNID + " streamNID=" + streamNID);
		Element relayData = MRXMLUtils.stringToElement(
				"<data><mediaSourceNID/><RelayData roomNID='" + roomNID + "' streamNID='" + streamNID + "'/></data>");

		Element instanceEL = (Element)inputElement.getParent().clone();
	    this.sendMediaSourceEvent(client, instanceEL, MRLog.INFO, 
	    		MediaRoomServiceConst.RELAYEVENT, relayData);

	    com.ipvs.common.XMPPTransaction.endBlock("SourceTable.startRelayStream");
	}
	
	private void updateRealyRoomInfoInInputElement(Element inputElement, String roomNID, String streamNID) {
		MRXMLUtils.addAttribute(inputElement,"roomNID", roomNID);
		MRXMLUtils.addAttribute(inputElement,"streamNID", streamNID);
		MRXMLUtils.addAttribute(inputElement,"state", String.valueOf(StreamTable.INPUT_OK));
		log.addInfo("updateRealyRoomInfoInInputElement: Update Input element with relay room information",MRLog.INFO,MRLog.NOTIFY);
	}

	public void handleResponse(String fromJID,Document responseDoc,MRClient client) {
		try {
			String requestName = MRXMLUtils.getValueXpath(responseDoc, "//requestname", "");
			this.log.addInfo("SourceTable::handleResponse:=" + this.userJID + " > Received Response " + requestName, MRLog.OK, MRLog.NOTIFY);
			if(requestName.equals(SourceStreamAgent.CREATERELAYCONNECTION_REQUEST)){
				handleCreateRelayConnectionResponse(fromJID, responseDoc, client);
			} else if(requestName.equals(SourceStreamAgent.ADDSTREAMACTION_REQUEST)){
				handleAddStreamActionResponse(fromJID, responseDoc, client);
			}
		} catch(MRException exp){
			exp.printStackTrace();
		}catch(Exception exp){
			exp.printStackTrace();
		}
	}

	public void handleRelayInputStreamResponse(MRClient client, Document doc, String action, String streamNID, String streamURL, Document streamDataDoc)throws Exception {
		Element inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@streamNID='" + streamNID + "']");
		// System.out.println("StreamTable:handleRelayInputStreamResponse inputElement=" + MRXMLUtils.elementToString(inputElement));
		if (inputElement == null) {
			inputElement = MRXMLUtils.getElementXpath(this.sourceTableDoc, "//Input[@sourceNID='" + MRXMLUtils.getValueXpath(streamDataDoc, "//sourceNID", "") + "']");
			if(inputElement == null || !MediaRoomServiceConst.START.equals(action)) {
				return;
			}
			
			log.addInfo("handleRelayInputStreamResponse: Did not find input stream. Found input stream based on SourceNID",MRLog.INFO,MRLog.NOTIFY);
			
		}
		
		// Check if this is a relay input
        if (!MediaRoomServiceConst.INPUT_RELAY.equals(MRXMLUtils.getAttributeValue(inputElement,"inputType"))) {
            return;
        }
        
		int  state  = Utils.getIntValue(MRXMLUtils.getAttributeValue(inputElement, "state",""),0);
		if(state==StreamTable.INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE){
			String roomNID = MRXMLUtils.getValueXpath(streamDataDoc, "//roomNID", "");
			updateRealyRoomInfoInInputElement(inputElement, roomNID, streamNID);
		}
	
		// Check if this is a response from the RelaySource and hup the relayInput if required
		if (MediaRoomServiceConst.START.equals(action) || MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(action) || MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(action) ||
		        MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(action) || MediaRoomServiceConst.SOURCE_UPDATEOFFSET.equals(action)) {
		    int status = MRLog.OK;
		    String logData = null;
		    int eventCode = MediaRoomServiceConst.RELAY_SOURCE_STARTED;
		    
		    String nowStr = String.valueOf(xmpp.getCurrentTimeMillis());
		    String actionData = this.userJID + "/" + action + "/" + nowStr;
		    int inputState = INPUT_OK;
		    try {
		        status = Integer.parseInt(MRXMLUtils.getValueXpath(doc, "//status", ""));
		        if(status == MRLog.ERROR){
		        	//eventCode = Integer.parseInt(MRXMLUtils.getValueXpath(doc, "//log/Code", String.valueOf(eventCode)));
		        	logData = MRXMLUtils.getValueXpath(doc, "//log", "");
		        	actionData = StreamTable.childrenToString(MRXMLUtils.getElementXpath(doc, "//actiondata"));
		        }
		    } catch (Exception e) {
		        status = MRLog.ERROR;
		        inputState = INPUT_RELAY_ERROR;
		    }
		
		    if (status == MRLog.OK) {
		        try {
		            this.handleRelayInputStreamStarted(inputElement, streamNID, streamURL, action, streamDataDoc);
		        } catch (Exception exp) {
		        	exp.printStackTrace();
		            logData = exp.toString();
		            status = MRLog.ERROR;
		        }
		    }
		    
		    MRXMLUtils.setAttributeValue(inputElement,"state", String.valueOf(inputState));			 

		    doc = StreamAgentUtils.createSourceEventMessage(streamNID, MediaRoomServiceConst.SOURCEEVENT, eventCode,
		            StreamTable.getStreamNIDXML(streamNID), 0, actionData, status, logData, log);
		   // doc = MRXMLUtils.stringToDocument(sourceEventXML);
		    action = MediaRoomServiceConst.SOURCEEVENT;
		}
		
		// Check if this is a SourceEvent that needs to be propogated
		if (MediaRoomServiceConst.SOURCEEVENT.equals(action)) {
		    String actionData = MRXMLUtils.elementChildrenToString(MRXMLUtils.getElementXpath(doc, "//sourceEventActionData"));
		    this.sendMessageToAllDestinations(client, doc, action, actionData);
		}
	}

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
		/* Moved to after the output is started
		try {// TBR when testing with actual RX
			this.streamingServer.updateSSNetworkInput(inputElement, streamDataDoc, action);
		}catch(Exception e) {
			e.printStackTrace();
		}
		*/
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
	    
		/* Start the input after the output is started */
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

    
    public Element switchInput(Element msElement, String newSourceNID, String newSourceType, Document streamDataDoc, long offset, long syncTC,double timescale, Document mediaSourceDataDoc, String timelineID,
    		List<?> streamList) throws Exception {
    	
        com.ipvs.common.XMPPTransaction.startBlock("ReleaseMediaSourceElement");
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
        com.ipvs.common.XMPPTransaction.endBlock("ReleaseMediaSourceElement");

        // Get the new instanceElement for the mediaSourceNID
        String mediaSourceNID = MRXMLUtils.getAttributeValue(msElement,"NID");
        com.ipvs.common.XMPPTransaction.startBlock("GetInstanceElement");

        Element newInstanceElement = this.getInstanceElement(MRRequest.createRequest(userJID), newSourceNID, newSourceType, mediaSourceNID, streamDataDoc, offset, syncTC,timescale, mediaSourceDataDoc, timelineID);
        com.ipvs.common.XMPPTransaction.endBlock("GetInstanceElement");
        // Add the mediaSourceElement to the instanceElement
        newInstanceElement.add(msElement);
        this.portTable.updatePortInstance(msElement, newInstanceElement);
        MediaRoomServiceUtils.printlog("SourceTable:switchInput sourceTable=" + MRXMLUtils.documentToString(this.sourceTableDoc));

        return newInstanceElement;
    }

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
    
    public void deleteRecordPortOnProgressStoppedEventFromStreamingServer(String ssInstanceId) throws MRException,Exception {
    	try {
			streamingServer.deleteRecordPort(ssInstanceId);
		} catch (Exception e) {
			//ignore the exception
		}
    }

}
