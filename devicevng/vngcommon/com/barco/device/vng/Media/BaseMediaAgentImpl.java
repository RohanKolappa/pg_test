package com.barco.device.vng.Media;

import java.io.IOException;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.List;
import java.util.StringTokenizer;
import java.util.UUID;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.common.DeviceDBI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.device.vng.Media.streamingserver.DeviceStreamingServerImpl;
import com.barco.device.vng.Media.streamingserver.DummyStreamingServerImpl;
import com.barco.device.vng.Media.streamingserver.V2DSSAPI;
import com.barco.device.vng.utils.MediaTimelineUtils;
import com.barco.utils.MediaUtils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;
import com.barco.utils.mr.XMPPTransaction;




class StreamData {
	Document streamData;
	Document mediasourceData;
	StreamData(Document streamData, Document mediasourceData) {
		this.streamData = streamData;
		this.mediasourceData = mediasourceData;
	}
}

/**
 * Instantiates DeviceStreamingServerImpl, that gets configured with a
 * NetworkResource (configured from db.getDeviceConfig()) which manages tcp/udp
 * ports and multicast address usage.
 * 
 * Spawns a thread that reads SS-event-xml from local port 10002
 * (SSEventListener), to forward source events as bus events (SourceInfo,
 * SourceEvent) to the jid of the stream destination.
 */
public abstract class BaseMediaAgentImpl implements MediaAgentI {
    DeviceFrameworkI deviceFrameWork;
    SourceTable sourceTable;
    DeviceTable deviceTable;
    PortTable portTable;
    Hashtable<String, String> destTable;
	ArrayList<String> roomNIDList = new ArrayList<String>();
	
	StreamingServerI streamingServer = null;
	Socket ssEventSocket = null;
	SSEventListener ssEventListener = null;
    
    // TBD make this protected
	protected Hashtable<String, StreamData> streamDataCache;
    
    //public static final int DeleterPortID = 2000;
   
    public static final int INPUT_OK = 3;
    public static final int INPUT_RELAY_WAITFORCREATERELAYCONNECTIONRESPONSE = 1;
    public static final int INPUT_RELAY_ERROR = 203;
    public static final String INTERNAL_INSTANCE ="INTERNAL_INSTANCE";

	public abstract void updateInputElement(String inputType, Element inputElement, String streamType,
			Document streamDataDoc, Document mediaSourceDataDoc, Document sourceTableDoc)  throws Exception;
	
	public abstract void setupExternalInput(String inputNID, Element inputElement, String sourceNID, String inputType, String streamType,
			Document streamDataDoc, long offset, long startTimecode, long syncTC, double timescale, Document mediaSourceDataDoc,
			Document sourceTableDoc) throws Exception;
	public abstract void teardownExternalInput(Element inputElement) throws Exception;

	public abstract void updateOutputElement(Element outputElement, String outputType, Document streamDataDoc) throws Exception;
	
    protected abstract void updateInstanceElement(Element instanceElement, String sourceNID, String sourcePortElementID, Document streamDataDoc, long offset, long syncTC,
				double timescale, Document mediaSourceDataDoc, String timelineID)  throws Exception;
	
	public void initAgent(final DeviceFrameworkI deviceFrameWork) {
		
    	this.deviceFrameWork = deviceFrameWork;
		DeviceDBI deviceDB = deviceFrameWork.getDeviceDB();
        this.streamDataCache = new Hashtable<String, StreamData>();
        this.deviceTable = new DeviceTable(deviceFrameWork.getLogger(), deviceDB);
        this.portTable = new PortTable(deviceFrameWork.getLogger(), deviceDB);
        this.destTable = new Hashtable<String,String>();       
      
    	// Create the right streaming server object
    	boolean dummy = false;
    	//dummy = true;
    	if(dummy) {
        	this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:initAgent using DummyStreamingServerImpl"); 
    		streamingServer = new DummyStreamingServerImpl(deviceFrameWork.getLogger(), deviceDB, null);
    		try {
		    streamingServer.init();
		} catch (ServiceException e) {
		    // TODO Auto-generated catch block
		    e.printStackTrace();
		} catch (Exception e) {
		    // TODO Auto-generated catch block
		    e.printStackTrace();
		}
    	}
    	else
    	{
    		streamingServer = new DeviceStreamingServerImpl(deviceFrameWork.getLogger(), deviceDB, null);
    		try {
		    streamingServer.init();
		} catch (ServiceException e) {
		    // TODO Auto-generated catch block
		    e.printStackTrace();
		} catch (Exception e) {
		    // TODO Auto-generated catch block
		    e.printStackTrace();
		}
    	}

    	streamingServer.getNetworkResource().updateNetworkResource(deviceDB.getDeviceConfig());
        this.sourceTable = new SourceTable(deviceFrameWork.getLogger(), deviceDB, streamingServer);

        // Start the SSEventListener
        final String deviceIP =  deviceDB.getIPAddress();
		try {
			try {
				if(ssEventListener!=null) {
					this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:initAgent ssEventListener already started. No need to start again.");
					this.ssEventListener.setDevice(deviceFrameWork);
					this.ssEventListener.setSourceTable(this.sourceTable);
					return;
				}
			} catch (Exception e) {
				//ignore exceptions
			}
			ssEventSocket = new Socket(deviceIP, DeviceStreamingServerImpl.SSEVENTPORT);
			
			ssEventListener = new SSEventListener(deviceFrameWork, ssEventSocket.getInputStream(), ssEventSocket.getOutputStream(), sourceTable);
			ssEventListener.start();
        	this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:initAgent ssEventListener started thread");
        	
        	new Thread(new Runnable() {

        		@Override
        		public void run() {
        			
        			while(true) {
        				if(ssEventListener.resetRequired) {
        					try {
								ssEventSocket = new Socket(deviceIP, DeviceStreamingServerImpl.SSEVENTPORT);
								ssEventListener = new SSEventListener(deviceFrameWork, ssEventSocket.getInputStream(), ssEventSocket.getOutputStream(), sourceTable);
								ssEventListener.start();
							} catch (UnknownHostException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (SocketException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							} catch (IOException e) {
								// TODO Auto-generated catch block
								e.printStackTrace();
							}
        				}
        				try {
							Thread.sleep(2000); //every 20 seconds look for socket connection is ok or not
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
        			}
        		}
        	}).start();
        	
		}
        catch(Exception e) {
        	e.printStackTrace();
        	this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:initAgent ssEventListener error=" + e.toString() +" deviceIP:="+deviceIP); 
        }

	}
	
	protected String getInstanceType(String sourceType) {
	    String instanceType = MediaRoomServiceConst.SHARED;
	    if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(sourceType)) {
	        instanceType = MediaRoomServiceConst.DEDICATED;
	    }
	    return instanceType;
	}

	protected String getInputType(String sourceType, Document streamDataDoc) {
		String inputType = sourceType;
		try {
	        String relayNID = XMLUtils.getValueXpath(streamDataDoc, "//relayNID", "");
	        if (!"".equals(relayNID)) {
	            inputType = MediaRoomServiceConst.INPUT_RELAY;
	        }
		}
        catch(Exception e) {this.deviceFrameWork.getLogger().info("DMSMediaAgentImpl:getInputType error=" + e.toString()); }
		return inputType;
	}
    

	protected void getStatus(Element rootElement) throws Exception {
	    rootElement.add((Element)portTable.portInstanceListDoc.getRootElement().clone());
	    rootElement.add((Element)deviceTable.deviceInstanceListDoc.getRootElement().clone());
	    rootElement.add((Element)sourceTable.getDoc().getRootElement().clone());
	}
    

	protected Document getStreamData(String streamNID) {
		StreamData data = this.streamDataCache.get(streamNID);
		if(data == null)
			return null;
		return data.streamData;
	}

	protected Document getMediaSourceData(String streamNID) {
		StreamData data = this.streamDataCache.get(streamNID);
		if(data == null)
			return null;
		return data.mediasourceData;
	}
	
	protected void updateMediaSourceData(String streamNID, Document mediaSourceData) {
		StreamData data = this.streamDataCache.get(streamNID);
		if(data != null)
			data.mediasourceData = mediaSourceData;
	}

	
	protected Element createInputElement(String ssInstanceID, String sourceNID, String inputType, String streamType,
			Document streamDataDoc, long offset, long startTimecode, long syncTC, double timescale, Document mediaSourceDataDoc)
	    throws Exception {
	    //this.log.info(this.userJID + ": SourceTable::createInput userJID=" + this.userJID);
		
	    Element inputElement = XMLUtils.stringToElement("<Input/>");
	    //String relayNID = XMLUtils.getElementXpath(streamDataDoc, "//relayNID").getValue();
	    XMLUtils.addAttribute(inputElement,"ssInstanceID", ssInstanceID);	
	    XMLUtils.addAttribute(inputElement,"inputType", inputType);
	    XMLUtils.addAttribute(inputElement,"streamType", streamType);
	    XMLUtils.addAttribute(inputElement,"sourceNID", sourceNID);
	    XMLUtils.addAttribute(inputElement,"offset", String.valueOf(offset));
	    XMLUtils.addAttribute(inputElement,"roomNID", "");
	    XMLUtils.addAttribute(inputElement,"streamNID", "");
	    XMLUtils.addAttribute(inputElement,"startTime", "0");
	    XMLUtils.addAttribute(inputElement,"startTimecode", String.valueOf(startTimecode));
	    XMLUtils.addAttribute(inputElement,"syncTC", String.valueOf(syncTC));
	    XMLUtils.addAttribute(inputElement,"timescale", String.valueOf(timescale));
	    
	    String deviceNID = XMLUtils.getAttributeValue(this.sourceTable.sourceTableDoc.getRootElement(),"NID");
	    String inputNID = this.deviceTable.addInputInstanceNID(deviceNID, inputElement);
	    XMLUtils.addAttribute(inputElement,"NID", inputNID);
	    
		this.updateInputElement(inputType, inputElement, streamType, streamDataDoc, mediaSourceDataDoc, this.sourceTable.getDoc());

	    return inputElement;
	}

	private Element addSourceTableInstance(String sourceNID, String sourceType, String userRoomMediaSourceNID, 
			Document streamDataDoc, long offset, long syncTC, double timescale, Document mediaSourceDataDoc, String timelineID,
			String sourceTableInstanceNID, String instanceType, String streamType)  throws Exception {
        Element instanceElement = XMLUtils.stringToElement("<Instance/>");
        XMLUtils.addAttribute(instanceElement,"NID", sourceTableInstanceNID);
        XMLUtils.addAttribute(instanceElement,"instanceType", instanceType);
        XMLUtils.addAttribute(instanceElement,"timelineID", timelineID);
        String startTimecodeStr = XMLUtils.getValueXpath(mediaSourceDataDoc, "//startTimecode","");
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
	    String inputNID = this.deviceTable.addInputInstanceNID(this.deviceFrameWork.getDeviceDB().getDeviceResourceNID(), inputElement);
	    XMLUtils.addAttribute(inputElement,"NID", inputNID);
	    this.setupExternalInput(inputNID, inputElement, sourceNID, inputType, streamType, streamDataDoc, offset, startTimecode, syncTC, timescale, mediaSourceDataDoc, mediaSourceDataDoc);

	    return instanceElement;
	}
	

    protected String generateSourceInstanceNID(String sourceNID,String sourcePortElementID, String streamType, String instanceType, String mediaSourceNID, String timelineID) {
        if (MediaRoomServiceConst.SHARED.equals(instanceType)) {
            return sourceNID + "." + streamType + "." + MediaRoomServiceConst.SHARED;
        } else {
            return sourceNID + "." + streamType + "." + mediaSourceNID + "." + timelineID;
        }
    }

	protected Element findOrCreateSourceTableInstance(String sourceNID,String portId, String sourceType, String instanceType, String userRoomMediaSourceNID, 
			Document streamDataDoc, long offset, long syncTC, double timescale, Document mediaSourceDataDoc, String timelineID)
	    throws Exception {
		
	    String streamType = XMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
	    String sourceTableInstanceNID = this.generateSourceInstanceNID(sourceNID, portId, streamType, instanceType, userRoomMediaSourceNID, timelineID);
	    Element instanceElement = XMLUtils.getElementXpath(this.sourceTable.getDoc(), "//Instance[@NID='" + sourceTableInstanceNID + "']");
	    if (instanceElement == null) {
	    	instanceElement = this.addSourceTableInstance(sourceNID, sourceType, userRoomMediaSourceNID, 
	    			streamDataDoc, offset, syncTC, timescale, mediaSourceDataDoc, timelineID,
	    			sourceTableInstanceNID, instanceType, streamType);
	    } else {
	    	updateInstanceElement(instanceElement, sourceNID,portId, streamDataDoc, offset, syncTC, timescale, mediaSourceDataDoc, timelineID);
	    }
	    return instanceElement;
	}
	



	private Element createMediaSourceElement(String roomNID,
			String mediaSourceNID, String mediaSourceType, long startTimecode) throws Exception {
	    Element msElement = XMLUtils.stringToElement("<MediaSource/>");
	    XMLUtils.addAttribute(msElement,"NID", mediaSourceNID);
	    XMLUtils.addAttribute(msElement,"roomNID", roomNID);
	    XMLUtils.addAttribute(msElement,"mediaSourceType", mediaSourceType);
	    XMLUtils.addAttribute(msElement,"startTimecode", String.valueOf(startTimecode));
	    return msElement;
	}

	
	public Element cloneMediaSourceElement(Element msElement) throws Exception {
		return this.createMediaSourceElement(
				msElement.attributeValue("roomNID"),
				msElement.attributeValue("mediaSourceNID"), 
				msElement.attributeValue("mediaSourceType"), 
				Long.parseLong(msElement.attributeValue("startTimecode")));
	}
	

	private Element getMediaSourceElement(Element instanceElement, String roomNID,
			String mediaSourceNID, String mediaSourceType, long startTimecode)
	    throws Exception {
	    Element msElement = XMLUtils.getElementXpathFromElement(instanceElement, "//MediaSource[@NID='" + mediaSourceNID + "']");
	
	    if (msElement == null) {
	    	msElement = this.createMediaSourceElement(roomNID, mediaSourceNID, mediaSourceType, startTimecode);
		    instanceElement.add(msElement);
	    }
	
	    return msElement;
	}
	
	protected Element createOutputElement(String ssInstanceNID, String outputType, Document streamDataDoc, Element inputElement) throws Exception {
	    // this.log.info(this.userJID + ": SourceTable::createOutput userJID=" + this.userJID);
	
	    Element outputElement = XMLUtils.stringToElement("<Output/>");
	    XMLUtils.addAttribute(outputElement,"ssInstanceID", ssInstanceNID);
	    XMLUtils.addAttribute(outputElement,"outputType", outputType);
    	String destNID = XMLUtils.getValueXpath(streamDataDoc, "//destNID","");
	    XMLUtils.addAttribute(outputElement,"destNID", destNID);

	    // Set the streamType and destNID
	    String streamType = XMLUtils.getAttributeValue(inputElement,"streamType");
	    XMLUtils.addAttribute(outputElement,"streamType", streamType);
	    XMLUtils.addAttribute(outputElement,"trickPlay", XMLUtils.getValueXpath(streamDataDoc, "//trickPlay", ""));
		XMLUtils.addAttribute(outputElement, "streamConnected", "true");
		
		this.updateOutputElement(outputElement, outputType, streamDataDoc);

	    return outputElement;
	}
	
	/*
	 * Starts the stream using the information provided in the streamDataDoc
	 */
	protected Element getSourceTableInstance(DeviceFrameworkI deviceFramework, String requestNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc)
	    throws Exception {
	    XMPPTransaction.startBlock("SourceTable.getStreamElement");
	
	    // Checks to see if the requestor needs to connect to a specific port
	    String tcpPort = XMLUtils.getValueXpath(streamDataDoc, "//callbackPort", "");
	    if(!"".equals(tcpPort)) { // Unmanaged Rx port, check to see if overlaps with configuredPool
	    	int tcpPortNumber = Integer.parseInt(tcpPort);
	    	if (this.sourceTable.streamingServer.getNetworkResource().isReservedTCPPort(tcpPortNumber)){
	    		throw new ServiceException(MediaRoomServiceExceptionCodes.OVERLAPPING_UNAMANAGED_V2D_DEST_PORT, XMLUtils.generateErrorXML("tcpPort", tcpPort));
	    	}
	    }

	    //log.info(this.userJID + ": SourceTable: getInstanceElement - Start", MRLog.OK, MRLog.NOTIFY);
	    XMPPTransaction.startBlock("SourceTable.getInstanceElement");
	
	    String mediaSourceNID = XMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID","");
	    String userRoomMediaSourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID","");
	    if("".equals(userRoomMediaSourceNID))
	    	userRoomMediaSourceNID = mediaSourceNID;
	    Element instanceElement = null;
	    
	    //default timescale 
	    double timescale =1;
	    
	    // Get/Create the instanceElement - 
	    String timelineID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID","");
	    String trackStateAtStart = XMLUtils.getValueXpath(streamDataDoc, "//trackStateAtStart", "");
	    long trackOffsetAtStart = Long.parseLong(XMLUtils.getValueXpath(streamDataDoc, "//trackOffsetAtStart", ""));
	    long mediaOffset = trackOffsetAtStart; // TBD account for startOffset
	    long syncTC = 0;
	    String sourcePortElementID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourcePortElementID", "");
		
	    // Check if we need to switch to Lookback Source
	    String sourceType = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
	    if(MediaRoomServiceConst.INPUT_ENCODER.equals(sourceType) &&
	    		(MediaRoomServiceConst.TRACK_STARTED_LOOKBACK.equals(trackStateAtStart) ||
	    		 MediaRoomServiceConst.TRACK_STOPPED_LOOKBACK.equals(trackStateAtStart)) ) { 
	        String lookbackSourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceNID","");
	        String lookbackSourceAgentJID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceAgentJID","");
	        String lookbackSourceType = MediaRoomServiceConst.INPUT_MEDIAFILE;
	        Document newStreamDataDoc = (Document) streamDataDoc.clone();
	        if (this.deviceFrameWork.getMyJID().equals(lookbackSourceAgentJID)) { // Make sure we are not relaying to ourselves
	            XMLUtils.setValueXpath(newStreamDataDoc, "//relayNID", "");
	        }
		    String instanceType = this.getInstanceType(lookbackSourceType);
	        instanceElement = this.findOrCreateSourceTableInstance(lookbackSourceNID,sourcePortElementID, lookbackSourceType, instanceType, userRoomMediaSourceNID, newStreamDataDoc, mediaOffset, syncTC, timescale, mediaSourceDataDoc, timelineID);
	    }
	    else {
		    String sourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID","");
		    String instanceType = this.getInstanceType(sourceType);
	    	instanceElement = this.findOrCreateSourceTableInstance(sourceNID, sourcePortElementID, sourceType, instanceType, userRoomMediaSourceNID, streamDataDoc, mediaOffset, syncTC, timescale, mediaSourceDataDoc, timelineID);
		}

	    XMPPTransaction.endBlock("SourceTable.getInstanceElement");
	    
	    return instanceElement;
	}

	protected void updateMediaTimeline(String inputType, Element instanceElement, Element inputElement, Element streamElement, 
			Document streamDataDoc, long mediaOffset) throws Exception {
	    Element mediaTimeline = MediaUtils.createMediaTimelineElement();
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
	    String trackStateAtStart = XMLUtils.getValueXpath(streamDataDoc, "//trackStateAtStart", "");
	    if(		MediaRoomServiceConst.TRACK_STOPPED_CURRENT.equals(trackStateAtStart) ||
	    		MediaRoomServiceConst.TRACK_STOPPED_LOOKBACK.equals(trackStateAtStart) ) {
	    	this.sourceTable.stopSSOutputAt(instanceElement, streamElement, mediaOffset);
	    	timescale = 0;
            if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
                this.sourceTable.streamingServer.stopSSInput(inputElement);
            }
	    }

	    MediaTimelineUtils.updateMediaTimelineElement(streamElement.element("MediaTimeline"), 
	    		now, mediaOffset, tc,timescale,
	    		MediaRoomServiceConst.START, trackStateAtStart, MediaRoomServiceConst.START, MediaRoomServiceConst.START);
	}

	protected Document generateStreamStatusDoc(Element streamElement, int state) throws Exception {
		Document streamStatusDoc = MediaUtils.createStreamStatusDoc();
		XMLUtils.setValueXpath(streamStatusDoc, "//state", String.valueOf(state));
		Element outputElement = streamElement.element("Output");
		String streamURL = outputElement.attributeValue("streamURL");
		XMLUtils.setValueXpath(streamStatusDoc, "//URL", streamURL);
		// System.out.println("BaseMediaAgentImpl:generateStreamStatusDoc streamElement=" + streamElement.asXML());
		// Replace the MediaTimeline
		Element statusElement = streamStatusDoc.getRootElement();
		statusElement.element("MediaTimeline").detach();
		Element mediaTimelineEl = (Element)streamElement.element("MediaTimeline");
		if(mediaTimelineEl != null ) {
			statusElement.add((Element)mediaTimelineEl.clone());
		}
		String streamNID = streamElement.attributeValue("NID");
		String roomNID = streamElement.attributeValue("roomNID");
		statusElement.addAttribute("streamNID", streamNID);
		statusElement.addAttribute("roomNID", roomNID);
		// System.out.println("BaseMediaAgentImpl:generateStreamStatusDoc streamStatusDoc=" + streamStatusDoc.asXML());
		return streamStatusDoc;
	}
	
	void printSourceTable(String atDesc) throws Exception {
		 //System.out.println("++++++++++++++++ " + atDesc + "\n" + this.sourceTable.sourceTableDoc.asXML());
	}

	@Override
	public Document startStream(DeviceFrameworkI deviceFramework, String requestNID, String streamNID, Document streamDataDoc, Document mediaSourceDataDoc)
		throws Exception {

	    this.printSourceTable("startStream BEGIN");
	    Element streamElement = XMLUtils.getElementXpath(this.sourceTable.getDoc(), "//Stream[@NID='" + streamNID + "']");
	    if (streamElement != null)

	    return generateStreamStatusDoc(streamElement, MediaAgentI.WAITING_FOR_STREAMSTARTED);

	    // Get the sourceTableInstance
	    Element instanceElement = this.getSourceTableInstance(deviceFramework, requestNID, streamNID, streamDataDoc, mediaSourceDataDoc);
	    Element inputElement = instanceElement.element("Input");
	    String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");
	    int state = Integer.parseInt(XMLUtils.getAttributeValueWithDefault(inputElement, "state", "0"));
	    if((state == BaseMediaAgentImpl.INPUT_RELAY_ERROR))
	    	throw new ServiceException(MediaRoomServiceExceptionCodes.RELAY_STREAM_ERROR, "Relay Stream has error");
	    String sourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID","");	
	    // Get the mediaSourceElement
	    XMPPTransaction.startBlock("SourceTable.getMediaSourceElement");
	    String roomNID = XMLUtils.getValueXpath(streamDataDoc, "//roomNID","");
	    String mediaSourceNID = XMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID","");//sourceNID	    
	    String sourceType = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
	    String timecode = XMLUtils.getValueXpath(mediaSourceDataDoc, "//startTimecode","");
	    long startTimecode = 0;
	    try {startTimecode = Long.parseLong(timecode);} catch (Exception e) {}
	    Element mediaSourceElement = this.getMediaSourceElement(instanceElement, roomNID, mediaSourceNID, sourceType, startTimecode);
	    XMPPTransaction.endBlock("SourceTable.getMediaSourceElement");

	    // Add the stream Element to the mediaSourceElement
	    String destAgentJID = XMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");
	    XMPPTransaction.startBlock("SourceTable.addStreamElement");
	
	    streamElement = createStreamElement(streamNID, destAgentJID, roomNID);
	    mediaSourceElement.add(streamElement);
	    
	    // Add the outputElement for the destination in the stream
	    long trackOffsetAtStart = Long.parseLong(XMLUtils.getValueXpath(streamDataDoc, "//trackOffsetAtStart", "0"));
	    long mediaOffset = trackOffsetAtStart; // TBD account for startOffset
	    String userRoomMediaSourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID","");
	    String destType = XMLUtils.getValueXpath(streamDataDoc, "//destType","");
	    
	    String outputType = destType;
	   
	    if(MediaRoomServiceConst.OUTPUT_DECODER.equals(outputType)) 
		outputType = MediaRoomServiceConst.OUTPUT_STREAM;	    // We are on a relay 
	    String ssInstanceID = this.sourceTable.generateID();
	    Element outputElement = this.createOutputElement(ssInstanceID, outputType, streamDataDoc, inputElement);
	    this.sourceTable.addOutput(instanceElement, outputElement, inputElement, streamDataDoc,mediaSourceDataDoc, streamElement, mediaOffset, roomNID, streamNID, userRoomMediaSourceNID);
	    String outputNID = this.deviceTable.addOutputInstanceNID(this.deviceFrameWork.getDeviceDB().getDeviceResourceNID(), outputElement);
	    XMLUtils.addAttribute(outputElement,"NID", outputNID);
	    this.portTable.addPortInstance(roomNID, streamNID, sourceNID, inputElement, outputElement);
	  
	    // Update the mediaTimeline
	    this.updateMediaTimeline(inputType, instanceElement, inputElement, streamElement, streamDataDoc, mediaOffset);
	    
	    // Add the stream and mediaSource to the cache
	    if(!sourceNID.equals(XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourcePortElementID", "")+"_"+INTERNAL_INSTANCE)) {
	    	this.streamDataCache.put(streamNID, new StreamData(streamDataDoc, mediaSourceDataDoc)); 
	    }
	    
	   this.printSourceTable("startStream END");
	   String sourceAgentJID = XMLUtils.getValueXpath(streamDataDoc, "//sourceAgentJID", "");
	   //TBD: This is temporary hack. we have to check actual stream state from output element and send status based on that.
		if(sourceAgentJID.equals(destAgentJID)) {
			return generateStreamStatusDoc(streamElement, MediaAgentI.STREAM_STARTED);
		}
		
		return generateStreamStatusDoc(streamElement, MediaAgentI.WAITING_FOR_STREAMSTARTED);

	}

	protected void releaseMediaSourceElement(Element msElement) throws Exception {
	    Element instanceElement = msElement.getParent();
	    instanceElement.remove(msElement);
	
	    if (instanceElement.element("MediaSource") == null) {
			this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:releaseMediaSourceElement instanceElement is empty deleting Instance");	    	
	    	deleteSourceTableInstance(instanceElement);
	    }
	    else {
			this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:releaseMediaSourceElement instanceElement not empty=" + XMLUtils.documentToString(this.sourceTable.sourceTableDoc));
			
	    }
	}
	
	protected void deleteSourceTableInstance(Element instanceElement) throws Exception {
        Element inputElement = instanceElement.element("Input");
	    this.sourceTable.deleteSourceInstance(instanceElement);
	    String inputNID = XMLUtils.getAttributeValue(inputElement,"NID");
	    this.deviceTable.deleteInputInstanceNID(this.deviceFrameWork.getDeviceDB().getDeviceResourceNID(), inputNID, inputElement);
	    this.teardownExternalInput(inputElement);
	}
	
	protected void deleteOutput(Element instanceElement, Element streamElement, Element inputElement, Element outputElement,String streamNID, Document streamDataDoc)  throws Exception{
		this.sourceTable.stopAndDeleteOutput(instanceElement, streamElement, inputElement, outputElement, streamNID, streamDataDoc);
	}

	@Override
	public Document stopStream(String streamNID) throws Exception {
	
		XMPPTransaction.startBlock("SourceTable.releaseStreamElement");
		this.printSourceTable("stopStream BEGIN");
	
	    String streamXPath = "//Stream[@NID='" + streamNID + "']";
	    Element streamElement = XMLUtils.getElementXpath(this.sourceTable.getDoc(), streamXPath);
	
	    if (streamElement == null) {
    		throw new ServiceException(MediaRoomServiceExceptionCodes.NOTFOUND, "Stream not found for streamNID=" + streamNID);
	    }
	
	     
	    Document streamDataDoc = null;
	    if(this.streamDataCache.containsKey(streamNID))
	    		streamDataDoc = this.streamDataCache.get(streamNID).streamData;
	
	    Element msElement = streamElement.getParent();
	
	    //Get the Instances ID's to delete the instances from device,src port and stream  
	    Element instanceElement = msElement.getParent();
	    Element inputElement = instanceElement.element("Input");
	    Element outputElement = XMLUtils.getElementXpathFromElement(streamElement, "Output");
		this.deleteOutput(instanceElement, streamElement, inputElement, outputElement, streamNID, streamDataDoc);
        this.deviceTable.deleteOutputInstanceNID(this.deviceFrameWork.getDeviceDB().getDeviceResourceNID(), XMLUtils.getAttributeValue(outputElement,"NID"), outputElement);
	    this.portTable.deletePortInstance(streamNID);

	    msElement.remove(streamElement);
	    this.streamDataCache.remove(streamNID);
	
	    // If mediaSourceNID is empty remove mediaSourceNID
	    // If instanceNID is empty remove instanceNID
	    if (msElement.elements().isEmpty()) {
	        this.releaseMediaSourceElement(msElement);
	    }
	
	    this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl::stopStream streamCache size=" + this.streamDataCache.size() +
	    		XMLUtils.documentToString(this.sourceTable.getDoc()));
	    
	
	    XMPPTransaction.endBlock("SourceTable.releaseStreamElement");
		this.printSourceTable("stopStream END");
	
    	return generateStreamStatusDoc(streamElement,MediaAgentI.WAITING_FOR_STREAMSTARTED);
	    
	}
	
	protected void addDest(String streamNID, String sourceAgentJID) {
		this.destTable.put(streamNID, sourceAgentJID);		
	}
	
	protected String removeDest(String streamNID) {
		String sourceAgentJID = this.destTable.get(streamNID);
		this.destTable.remove(streamNID);
		return sourceAgentJID;
	}

	/*
	 * Source Table Data Example
	<SourceTable>
	<Instance NID="default.mediastreamsrcportresourcelist.mssprl_57084738-4839-41b1-946f-36aadc24e8de.V2D.SHARED" 
	 instanceType="SHARED" timelineID="2b6292aa-6b18-45b4-8577-7ac787e8e874.StreamTrack">
	 <Input ssInstanceID="2001" inputType="ENCODER" streamType="V2D" 
	  sourceNID="default.mediastreamsrcportresourcelist.mssprl_57084738-4839-41b1-946f-36aadc24e8de" offset="1315" 
	  roomNID="" streamNID="" startTime="1363268384956" startTimecode="1363271281714" syncTC="0" timescale="1.0" 
	  sourcePortElementID="1" isMulticast="false" 
	  NID="default.assetresourcelist.arl_b01aa808-5def-4e78-99f1-da79cc26d429.DeviceStatus.ServiceElementsStatus.StreamingServiceStatus.InstanceList.2001" 
	  state="3"/>
	  <MediaSource NID="dc06d72c-4256-4f6e-b45a-c61027fe8a37" roomNID="2b6292aa-6b18-45b4-8577-7ac787e8e874" 
	   mediaSourceType="ENCODER" startTimecode="1363271281714">
	   <Stream NID="683b5d84-6134-40d5-a2a9-ba1b60c52053" 
	    destAgentJID="dcuser1@localhost/dcpc1" roomNID="2b6292aa-6b18-45b4-8577-7ac787e8e874" state="8">
	    <Output ssInstanceID="2002" outputType="OUTPUT_STREAM" destNID="" streamType="V2D" trickPlay="true" 
	     streamURL="v2d://10.51.50.1/port=6060?bandwidth=10240000?avoption=Video?ismulticast=false?enablekbm=false?authscheme=TOKEN?authdata=4f60b616-14e5-4b0e-917e-09e81c005afe" 
	     NID="default.assetresourcelist.arl_b01aa808-5def-4e78-99f1-da79cc26d429.DeviceStatus.ServiceElementsStatus.StreamingServiceStatus.InstanceList.2002"/>
		<MediaTimeline id="">
		 <Start WC="1363268384979" Offset="1315" TC="1363271281714"/>
		 <Last WC="1363268384979" Offset="1315" TC="1363271281714" TimeScale="1.0" action="Start" lastState="STARTED_CURRENT" requestedAction="Start" sourceAction="Start" ssTC="" ssWC=""/>
		</MediaTimeline>
	   </Stream></MediaSource> 
	  </Instance>
	</SourceTable>
	*/

	private ArrayList<String> getStreamNIDList(String xpath) {
		ArrayList<String> streamNIDList = new ArrayList<String>();
		try {
			// String s = this.sourceTable.sourceTableDoc.asXML();
			// this.deviceFrameWork.getLogger().info(s);
			List<?> list = XMLUtils.getListOfNodesXpath(this.sourceTable.sourceTableDoc, xpath);
			for(Object o : list) {
				Element e = (Element)o;
				String streamNID = e.attributeValue("NID");
				streamNIDList.add(streamNID);
			}
		}
		catch(Exception e) {
			this.deviceFrameWork.getLogger().info("TestMediaAgent:getStreamNIDList error=" + e.toString());
		}
		return streamNIDList;
	}
	
	@Override
	public ArrayList<String> getStreamNIDListByRoomNID(String roomNID) {
		return this.getStreamNIDList("//Stream[@roomNID='" + roomNID + "']");
	}
	
	@Override
	public ArrayList<String> getStreamNIDListByDestAgentJID(String destUserJID) {
		return this.getStreamNIDList("//Stream[@destAgentJID='" + destUserJID + "']");		
	}
	
	@Override
	public void addRoomNID(String roomNID) {
		this.roomNIDList.add(roomNID);
	}
	
	@Override
	public void deleteRoomNID(String roomNID) {
		this.roomNIDList.remove(roomNID);
	}
	
	public ArrayList<String> getRoomNIDList() {
		return this.roomNIDList;
	}



	////////////////////////////////////////
	// Rx Instance methods
	//////////////////////////////////////////
	
	protected Hashtable<String, String> parseV2DStreamURL(String streamURL) {
		// Generate the streamType, authToken, profileXML from the streamURL
		Hashtable<String, String> table = new Hashtable<String, String>();
		StringTokenizer st = new StringTokenizer(streamURL, "?");
		String s = st.nextToken(); // Consume the IP port token
		StringTokenizer st1 = new StringTokenizer(s, ":");
		String streamType = st1.nextToken().toUpperCase();		
		if(!MediaRoomServiceConst.V2D.equals(streamType))
			return null;
		while(st.hasMoreTokens()) {
			s = st.nextToken();
			StringTokenizer stt = new StringTokenizer(s, "=");
			String name = stt.nextToken();
			String value = stt.nextToken();
			table.put(name, value);
		}
		String[]  params = { "Bandwidth", "AVOption", "IsMulticast", "EnableKbm"};
		
		// Convert the Bandwidth to Kbits
		try {	
			long bwInBits = Long.parseLong(table.get("bandwidth"));
			long bwInKbits = bwInBits / 1024;
			table.put("bandwidth", String.valueOf(bwInKbits));
		}
		catch(Exception e) {
			// Cannot parse, will end up being default
			table.put("bandwidth",  V2DSSAPI.DefaultBandwidth);
		}

		StringBuffer sb = new StringBuffer("<profileXML>");
		for(String n : params) {
			sb.append("<" + n + ">" + table.get(n.toLowerCase()) + "</" + n + ">");			
		}
		sb.append("</profileXML>");
		table.put("profileXML", sb.toString());

		return table;
	}

	public void startPlaying(DeviceFrameworkI deviceFramework, String streamNID, String destPortElementID,	String streamURL)
	throws Exception {

		Hashtable<String, String> table = this.parseV2DStreamURL(streamURL);
		if(table == null)
			throw new ServiceException(ServiceExceptionCodes.RESOURCENOTFOUND, "Cannot play URL=" + streamURL);
		String streamType = MediaRoomServiceConst.V2D;

		Element profileXML = XMLUtils.stringToElement(table.get("profileXML"));	
		String mediaDestNID = table.get("authdata");
		if(mediaDestNID==null) {
			mediaDestNID = UUID.randomUUID().toString();
		}
			
		String destType = MediaRoomServiceConst.OUTPUT_DECODER;
		// String streamNID = "stream." + destPortElementID;
		String roomNID = "room." + destPortElementID;

		this.startPlayingInternal(deviceFramework, streamNID, roomNID, 
				destType, 
				streamURL, 
				streamType,
				destPortElementID,
				mediaDestNID, 
				profileXML);
	}

	void addElement(Document doc, String name, String value) throws Exception {
		Element e = XMLUtils.stringToElement("<" + name + "/>");
		if(value==null) 
			value ="";
		e.setText(value);
		doc.getRootElement().add(e);
	}

	public void startPlayingInternal(DeviceFrameworkI deviceFramework, String streamNID, String roomNID, 
			String destType, String streamURL, String streamType, String destPortElementID, String mediaDestNID, Element profileXML)
		throws Exception {

		this.printSourceTable("startPlaying BEGIN1");

		Document streamDataDoc = XMLUtils.stringToDocument("<data/>");
		addElement(streamDataDoc, "mediaDestNID", mediaDestNID);
		streamDataDoc.getRootElement().add(profileXML);
		addElement(streamDataDoc, "destPortElementID", destPortElementID);
		addElement(streamDataDoc, "destType", destType);
		addElement(streamDataDoc, "streamType", streamType);

		Document mediaSourceDataDoc = XMLUtils.stringToDocument("<data/>");

		// System.out.println("SourceTable:createRxInstance");
		String sourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID", "");
		String inputType = MediaRoomServiceConst.INPUT_STREAM;
		String instanceType = MediaRoomServiceConst.DEDICATED;
		long startTimecode = 0;
		long syncTC = 0;
		double timescale =1;
		String mediaSourceNID = XMLUtils.getValueXpath(streamDataDoc, "//mediaSourceNID", "");
		String timelineID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID", "");		
		String sourceType = inputType;
		String userRoomMediaSourceNID = roomNID + "." + mediaSourceNID;
		long mediaOffset = 0;
		
		//Create the InputInstance
		   
    	Element instanceElement = this.findOrCreateSourceTableInstance(sourceNID, destPortElementID,
    			sourceType, instanceType, userRoomMediaSourceNID, streamDataDoc, mediaOffset, syncTC, timescale, mediaSourceDataDoc, timelineID);
		XMLUtils.addAttribute(instanceElement,"instanceType", instanceType);
	    Element inputElement = instanceElement.element("Input");
		XMLUtils.addAttribute(inputElement,"streamURL", streamURL);
		XMLUtils.addAttribute(inputElement,"streamNID", streamNID);
		// Carry over the enable Kbm from input stream
    	String enablekbm = XMLUtils.getValueXpath(streamDataDoc, "//EnableKbm", "false");
		XMLUtils.addAttribute(inputElement,"EnableKbm", enablekbm);

		// Get the mediaSourceElement
	    Element mediaSourceElement = this.getMediaSourceElement(instanceElement, roomNID, userRoomMediaSourceNID, sourceType, startTimecode);

		// Add the stream Element to the mediaSourceElement
	    String destAgentJID = XMLUtils.getValueXpath(streamDataDoc, "//destAgentJID","");
	    XMPPTransaction.startBlock("SourceTable.addStreamElement");
	    Element streamElement = XMLUtils.stringToElement("<Stream/>");
	    XMLUtils.addAttribute(streamElement,"NID", streamNID);
	    XMLUtils.addAttribute(streamElement,"destAgentJID", destAgentJID);
	    XMLUtils.addAttribute(streamElement,"roomNID", roomNID);
	    XMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
	    mediaSourceElement.add(streamElement);

		// Add the outputElement for the destination in the stream
	    String outputType = XMLUtils.getValueXpath(streamDataDoc, "//destType","");
	    String ssInstanceID = this.sourceTable.generateID();
	    Element outputElement = this.createOutputElement(ssInstanceID, outputType, streamDataDoc, inputElement);
	    this.sourceTable.addOutput(instanceElement, outputElement, inputElement, streamDataDoc,mediaSourceDataDoc, streamElement, mediaOffset, roomNID, streamNID, userRoomMediaSourceNID);
	    String outputNID = this.deviceTable.addOutputInstanceNID(this.deviceFrameWork.getDeviceDB().getDeviceResourceNID(), outputElement);
	    XMLUtils.addAttribute(outputElement,"NID", outputNID);
	    this.portTable.addPortInstance(roomNID, streamNID, sourceNID, inputElement, outputElement);
	   
	    startInput(streamDataDoc, inputElement);
	    
	    this.printSourceTable("startPlaying END");
	    
	}
	
	protected void startInput(Document streamDataDoc, Element inputElement)  throws Exception{
		// Start the input
		this.streamingServer.updateSSNetworkInput(inputElement, streamDataDoc, MediaRoomServiceConst.START);
	}


	public void stopPlaying(String streamNID) throws Exception {
		
	    this.printSourceTable("stopPlaying BEGIN");

	    Element inputElement = null;
		inputElement = XMLUtils.getElementXpath(this.sourceTable.sourceTableDoc, "//Input[@streamNID='" + streamNID + "']");
		
		if(inputElement == null) {
			return;
		}
		Element instanceElement = inputElement.getParent();
	    Element msElement = instanceElement.element("MediaSource");
	    Element streamElement = msElement.element("Stream");
	    Element outputElement = streamElement.element("Output");

		this.sourceTable.stopAndDeleteOutput(instanceElement, streamElement, inputElement, outputElement, streamNID, null);

  	  // this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:stopPlaying deleteOutput done");

        this.deviceTable.deleteOutputInstanceNID(this.deviceFrameWork.getDeviceDB().getDeviceResourceNID(), XMLUtils.getAttributeValue(outputElement,"NID"), outputElement);
		// this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:stopPlaying deleteOutputInstanceNID done");
		
	    this.portTable.deletePortInstance(streamNID);

	    msElement.remove(streamElement);
	    this.streamDataCache.remove(streamNID);
	
	    // If mediaSourceNID is empty remove mediaSourceNID
	    // If instanceNID is empty remove instanceNID
		// this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:stopPlaying before releaseMediaSourceElement");	    	
	    if (msElement.elements().isEmpty()) {
	        this.releaseMediaSourceElement(msElement);
	    }
	    else {
			this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:stopPlaying msElement not empty=" + XMLUtils.documentToString(this.sourceTable.sourceTableDoc));	    	
	    }
	    this.printSourceTable("stopPlaying END");

		this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:stopPlaying sourceTable=" + XMLUtils.documentToString(this.sourceTable.sourceTableDoc));		
	}
	
	
	@Override
	public Document getPublishInfo()  throws Exception {
	    Hashtable<String, StreamData> streamList = this.streamDataCache;	    
	    this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:getPublishInfo=" + this.streamDataCache);
		Enumeration<String> keys = streamList.keys();
		Element streamInfoList = XMLUtils.stringToElement("<StreamInfoList />");
		while(keys.hasMoreElements()) {		
			String key = keys.nextElement();
			//MediaAgentI.STREAM_STARTED
			//Element output = XMLUtils.getElementXpath(this.sourceTable.sourceTableDoc, "//Stream[@NID='"+key+"']/Output");
			Element stream = XMLUtils.getElementXpath(this.sourceTable.sourceTableDoc, "//Stream[@NID='"+key+"']");
			if(stream == null) {
				// This should not be null. Remove key from streamDataCache and continue for next stream.
				this.streamDataCache.remove(key);
				continue;
			}
			String streamURL = XMLUtils.getAttributeValue(stream.element("Output"), "streamURL");			
			Document streamStatusDoc = null;
			if ("".equals(streamURL)){
			    streamStatusDoc = this.generateStreamStatusDoc(stream, MediaAgentI.START_ERROR);
			}
			else{
			    streamStatusDoc = this.generateStreamStatusDoc(stream, MediaAgentI.STREAM_STARTED);
			}
			//this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:getPublishInfo= OUTPUT" + output.asXML()+"#### StreamURL: "+streamURL);
			Element streamInfoDoc = XMLUtils.stringToElement("<StreamInfo />");
			XMLUtils.addAttribute(streamInfoDoc, "streamNID", key);
			StreamData entry = streamList.get(key);
			Document mediaSourceDataDoc = entry.mediasourceData;			
			Document streamDataDoc = entry.streamData;	
			XMLUtils.setValueXpath(streamDataDoc, "//streamURL", streamURL);
			Element streamDataDocInfo = XMLUtils.stringToElement("<StreamDataDoc />");
			Element mediaSourceDataDocInInfo = XMLUtils.stringToElement("<MediaSourceDataDoc />");			
			streamDataDocInfo.add((Element)streamDataDoc.getRootElement().clone());		
			streamDataDocInfo.add((Element)streamStatusDoc.getRootElement().clone());
			mediaSourceDataDocInInfo.add((Element)mediaSourceDataDoc.getRootElement().clone());
			streamInfoDoc.add(streamDataDocInfo);
			streamInfoDoc.add(mediaSourceDataDocInInfo);
			streamInfoList.add(streamInfoDoc);			
		}
		Document mediaServiceInfoDoc = XMLUtils.stringToDocument("<"+DeviceAdminConst.MEDIA_SERVICE_INFO+"/>");
		mediaServiceInfoDoc.getRootElement().add(streamInfoList);	
		//this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:getPublishInfo= SourceTableDoc" + XMLUtils.documentToString(this.sourceTable.sourceTableDoc));
		this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:getPublishInfo= MediaServiceInfo Doc" + XMLUtils.documentToString(mediaServiceInfoDoc));
		return mediaServiceInfoDoc;
	}
		
	public Document getStatus(String xpath) throws Exception{
		return this.sourceTable.getDoc();
	}
	
	public Element createStreamElement(String streamNID ,String destAgentJID,String roomNID) throws Exception{
	    
	    Element streamElement = XMLUtils.stringToElement("<Stream/>");
	    XMLUtils.addAttribute(streamElement,"NID", streamNID);
	    XMLUtils.addAttribute(streamElement,"destAgentJID", destAgentJID);
	    XMLUtils.addAttribute(streamElement,"roomNID", roomNID);
	    XMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
	    return streamElement;
	}


    public static String getStreamNIDXML(String streamNID) {
        return "<streamNIDListItem>" + streamNID + "</streamNIDListItem>";
    }
    
	
	// Subset of the StreamDataDoc that is required for setup of streams
	
	// Document streamDataDoc = XMLUtils.stringToDocument("<data/>");
	
	// Required for playing
	// addElement(streamDataDoc, streamDataDoc1, "mediaDestNID");
	// addElement(streamDataDoc, streamDataDoc1, "profileXML");
	// addElement(streamDataDoc, streamDataDoc1, "destPortElementID");
	// addElement(streamDataDoc, streamDataDoc1, "destType");
	// addElement(streamDataDoc, streamDataDoc1, "streamType");

	// Incremental Base attributes required for startStream
	// addElement(streamDataDoc, streamDataDoc1, "mediaSourceNID");
	// addElement(streamDataDoc, streamDataDoc1, "profileNID");
	// addElement(streamDataDoc, streamDataDoc1, "destNID");
	// addElement(streamDataDoc, streamDataDoc1, "destAgentJID");
	// addElement(streamDataDoc, streamDataDoc1, "resolvedSourceIP");
	
	// Document mediaSourceDataDoc = XMLUtils.stringToDocument("<data/>");
	// Incremental Base attributes required for startStream
	// addElement(mediaSourceDataDoc, mediaSourceDataDoc1, "trackNID");
	// addElement(mediaSourceDataDoc, mediaSourceDataDoc1, "sourceNID");
	// addElement(mediaSourceDataDoc, mediaSourceDataDoc1, "sourceType");
	// addElement(mediaSourceDataDoc, mediaSourceDataDoc1, "streamType");
//	public abstract void createThumbnailStream(DeviceFrameworkI deviceFramework, Document streamStreamDataDoc, Document mediaSourceDataDoc, Element inputElement, Element instanceElement, Element mediaSourceElement, String streamNID) throws Exception;
//	public abstract void tearDownThumbnailStream(Document streamStreamDataDoc, Element inputElement, Element instanceElement, Element mediaSourceElement) throws Exception;
	
	
	/**
	 * handle device config updates 
	 * @param doc
	 * @throws Exception
	 */
	public void handleConfigUpdated(Document notificationDocument) throws Exception{
		//TODO: Testing Padmaja
		//Media agent maintaining cache of multicast ipaddress range, control ports. they have to reset based on config update
		//Media agent has to remove splash screen if the port type changing source and add splash screen if port type changes to destination 
		List<String> listOfCommands =  new ArrayList<String>();
		listOfCommands.add("//MulticastIpAddressRange");        
		listOfCommands.add("//TxCtrlPortRange");
		listOfCommands.add("//V2DStreamsRxConfig/RxMediaPortRange");
		listOfCommands.add("//UDPStreamsRxConfig/RxMediaPortRange");
		listOfCommands.add("//RTPStreamsRxConfig/RxMediaPortRange");
		listOfCommands.add("//RTPStreamsRxConfig/RxMediaPortRange");
		
		boolean isStreamingConfigUpdate = false;
		for(String command: listOfCommands) 
		{
			if(XMLUtils.getElementXpath(notificationDocument, command) != null) {
				isStreamingConfigUpdate =true;
				break;
			}
		}

		if(!isStreamingConfigUpdate){
			return;
		}
		try {
			//this will take care of update network resource parameters
			streamingServer.getNetworkResource().updateNetworkResource(notificationDocument);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
	public List<String> getStreamNIDListByPortID(String portID) throws Exception{		
		return getStreamNIDList("//Instance[Input[@sourcePortElementID='" + portID + "']]/.//Stream[@NID!='"+portID+"_THUMBNAIL']");
	}
	
	public boolean startPlayingInternalRx(DeviceFrameworkI deviceFramework, String streamNID, String roomNID, 
		String destType, String streamURL, String streamType, String destPortElementID, String mediaDestNID, Element profileXML)
			throws Exception {
	    
	    this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:startPlayingInternalRx:sourceTable is: "+this.sourceTable.sourceTableDoc.asXML());
	    Element output = XMLUtils.getElementXpath(this.sourceTable.sourceTableDoc, "//Output[@destPortElementID='" + destPortElementID + "']");
	    if(output != null) {			
			// add and delete network port. we are reusing the hardware port.
			// if output exists only add input.
			// we should not crate input which is creating problem in streaming server. 
	
			Element streamElement = output.getParent();
			Element instanceElement = streamElement.getParent().getParent();
			Element inputElement = instanceElement.element("Input");
			streamElement.addAttribute("NID", streamNID);
			inputElement.addAttribute("streamURL", streamURL);
			inputElement.addAttribute("streamNID", streamNID);
			Document streamDataDoc = XMLUtils.stringToDocument("<data/>");
			addElement(streamDataDoc, "mediaDestNID", mediaDestNID);
			streamDataDoc.getRootElement().add(profileXML);
			addElement(streamDataDoc, "destPortElementID", destPortElementID);
			addElement(streamDataDoc, "destType", destType);
			addElement(streamDataDoc, "streamType", streamType);
			this.deviceFrameWork.getLogger().info("BaseMediaAgentImpl:startPlayingInternalRx:Output is: "+output.asXML());		
			this.streamingServer.updateSSOutput(output, streamDataDoc);
			startInput(streamDataDoc, inputElement);
			return false;
	    }
	    else{
	    	return true;
	    }

	    
	}
	public void stopPlayingRx(String streamNID) throws Exception {
		
		//delete v2dnetwork port only
		Element streamElement = XMLUtils.getElementXpath(this.sourceTable.sourceTableDoc, "//Stream[@NID='" + streamNID + "']");
		Element instanceElement = streamElement.getParent().getParent();
		Element inputElement = instanceElement.element("Input");
		try {
			this.streamingServer.stopSSInput(inputElement);      	   
		}
		catch(Exception e) {
			this.deviceFrameWork.getLogger().info("XPMediaAgentImpl:stop input  error=" + e.toString());
		}
		try {
			this.streamingServer.deleteSSInput(inputElement);              	   
		}
		catch(Exception e) {
			this.deviceFrameWork.getLogger().info("XPMediaAgentImpl:delete Input error=" + e.toString());
		}
		String newStreamNID = UUID.randomUUID().toString();
		streamElement.addAttribute("NID",newStreamNID);
		inputElement.addAttribute("streamNID",newStreamNID);
		inputElement.addAttribute("streamURL", MediaRoomServiceConst.SPLASHSCREEN_URL);		
	}
	
	
	@Override
	public void handleStopped(Document eventDoc)
			throws Exception {
		// TODO Auto-generated method stub
		
	}
	
	@Override
	public void reset() {
		// TODO Auto-generated method stub
		
	}	
}
