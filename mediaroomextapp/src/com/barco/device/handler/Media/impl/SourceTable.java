package com.barco.device.handler.Media.impl;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.utils.mr.MRException;
import com.barco.utils.mr.MRLog;
import com.barco.utils.mr.MRXMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;
import com.barco.utils.mr.Utils;

public class SourceTable {
    long runningNumber = 2000;
    Document sourceTableDoc;
    public StreamingServerI streamingServer;
    MRLog log;
    String userJID;
    
    public SourceTable(String deviceNID, String userJID, StreamingServerI streamingServer, MRLog log) {
		this.streamingServer = streamingServer;		
        try {
        	this.sourceTableDoc = MRXMLUtils.stringToDocument("<SourceTable/>");
        	if(deviceNID != null)
        	    MRXMLUtils.addAttribute(this.sourceTableDoc.getRootElement(),"NID", deviceNID);
        	MRXMLUtils.addAttribute(this.sourceTableDoc.getRootElement(),"userJID",userJID);
        }
        catch(Exception e) {
        	e.printStackTrace();
        }
        this.log = log;
        this.userJID = userJID;
	}
    
    Document getDoc() {
    	return this.sourceTableDoc;
    }

    public List<?> getStreamList(String streamXPath) throws Exception {
	    return MRXMLUtils.getListXpath(this.sourceTableDoc, streamXPath);
    }

    public Element getStream(String streamXPath) throws Exception {
	    return MRXMLUtils.getElementXpath(this.sourceTableDoc, streamXPath);
    }

    public String generateID() {
        this.runningNumber++;
        return String.valueOf(runningNumber);
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

    /*
	private void deleteInput(Element inputElement) throws Exception {
	    this.log.addInfo(this.userJID + ": SourceTable:deleteInput deleting ssInstanceID=" + MRXMLUtils.getAttributeValue(inputElement,"ssInstanceID"));
	    this.streamingServer.deleteSSInput(inputElement);
	}
	*/

	void addOutput(Element instanceElement, Element outputElement, Element inputElement, 
			Document streamDataDoc, Element streamElement, long startOffset,
			String roomNID, String streamNID, String sourceNID) throws Exception {
	    this.streamingServer.createSSOutput(outputElement, inputElement, streamDataDoc);	    
	    streamElement.add(outputElement);
	    this.startSSOutput(instanceElement, streamElement, startOffset);
	    this.streamingServer.startSSInput(instanceElement, streamElement);
	    long t = System.currentTimeMillis();
	    this.log.addInfo(this.userJID + ": SourceTable::getStreamURL setInstanceInfo ...");
	    this.log.addInfo(this.userJID + ": SourceTable::getStreamURL setInstanceInfo ... done=" + (System.currentTimeMillis() - t));
	}
    
    public void deleteOutput(Element instanceElement, Element streamElement, Element inputElement, Element outputElement, String streamNID, 
    		Document streamDataDoc) throws Exception {
	    this.stopSSOutputAt(instanceElement, streamElement, Long.parseLong(MRXMLUtils.getAttributeValue(inputElement,"offset")));
        this.log.addInfo(this.userJID + ": SourceTable:deleteOutput deleting ssInstanceID=" + MRXMLUtils.getAttributeValue(outputElement,"ssInstanceID"));
        this.streamingServer.deleteSSOutput(outputElement, streamDataDoc);
    }

    public void addSourceInstance(Element instanceElement,Element inputElement, 
    		Document streamDataDoc, Document mediaSourceDataDoc) throws Exception  {
	    this.streamingServer.createSSInput(inputElement, streamDataDoc, mediaSourceDataDoc);
        instanceElement.add(inputElement);
        this.sourceTableDoc.getRootElement().add(instanceElement);
	}

    public void deleteSourceInstance(Element instanceElement) throws Exception {
        Element inputElement = instanceElement.element("Input");
	    this.streamingServer.deleteSSInput(inputElement);
        this.sourceTableDoc.getRootElement().remove(instanceElement);
    }

}
