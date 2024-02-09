package com.barco.device.vng.Media;

import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.common.ServiceException;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class SourceTable {
	static final long STARTRUNNINGNUMBER = 2000;
	long runningNumber = 0;
	Document sourceTableDoc;
	Hashtable<String,Element> gcStreamList = new Hashtable<String,Element>();

	public StreamingServerI streamingServer;
	Logger log;
	DeviceDBI deviceDB;

	public SourceTable(Logger log, DeviceDBI deviceDB, StreamingServerI streamingServer) {
		this.log = log;
		this.deviceDB = deviceDB;
		this.streamingServer = streamingServer;
		this.resetRunningNumber();
		try {
			this.sourceTableDoc = XMLUtils.stringToDocument("<SourceTable/>");
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	Document getDoc() {
		return this.sourceTableDoc;
	}

	public List<?> getStreamList(String streamXPath) throws Exception {
		return XMLUtils.selectNodesXpath(this.sourceTableDoc.getRootElement(), streamXPath);
	}

	public Element getStream(String streamXPath) throws Exception {
		return XMLUtils.getElementXpath(this.sourceTableDoc, streamXPath);
	}

	public String generateID() {
		this.runningNumber++;
		return String.valueOf(runningNumber);
	}

	private void resetRunningNumber() {
		this.runningNumber = SourceTable.STARTRUNNINGNUMBER;
	}

	// Stop the Streaming Server Output Instance
	public void stopSSOuputList(List<?> streamList, Element instanceElement, long offset)
			throws ServiceException, Exception {
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
			XMLUtils.addAttribute(i.next(),"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
	}

	public void startSSInputListSync(Hashtable<String,Element> syncSourceList) throws Exception {
		// System.out.println("SourceTable:stopSSOutputListSync streamElementList count=" + streamElementList.size());
		this.streamingServer.startSSInputAtSync(syncSourceList);
	}

	// Start the Streaming Server Output Instance
	public void startSSOuputList(List<?> streamList, Element instanceElement, long offset)
			throws ServiceException, Exception {
		Iterator<?> streamListIterator = streamList.iterator();

		while (streamListIterator.hasNext()) {
			Element streamElement = (Element) streamListIterator.next();
			this.startSSOutput(instanceElement, streamElement, offset);
		}
	}

	public void startSSOutput(Element instanceElement, Element streamElement, long offset) throws Exception {
		int state = Integer.parseInt(XMLUtils.getAttributeValue(streamElement,"state"));
	
		if (state != MediaRoomServiceConst.STREAM_STARTED && isDestinationSupportedTrickPlay(streamElement, instanceElement)) {	    	
			this.streamingServer.startSSOutput(instanceElement, streamElement);
			XMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STARTED));
		}
	}

	private boolean isDestinationSupportedTrickPlay(Element streamElement, Element instanceElement) {  

		Element outputElement = streamElement.element("Output");
		Element inputElement = instanceElement.element("Input");
		boolean trickPlay = Utils.getBooleanValue(XMLUtils.getAttributeValue(outputElement, "trickPlay"), false);
		double timescale = Utils.getDoubleValue(XMLUtils.getAttributeValue(inputElement, "timescale"), 1);     	
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

		double timescale = Utils.getDoubleValue(XMLUtils.getAttributeValue(inputEL, "timescale"), 1);        
		boolean isInputSupportFFTracks = Utils.getBooleanValue(XMLUtils.getAttributeValue(inputEL, "ffTracks"), false);	    
		return (timescale > 1 && !isInputSupportFFTracks) ? false : true;    	
	}

	public void stopSSOutputAt(Element instanceElement, Element streamElement, long offset)throws Exception {
		int state = Integer.parseInt(XMLUtils.getAttributeValue(streamElement,"state"));
		if (state != MediaRoomServiceConst.STREAM_STOPPED) {
			this.streamingServer.stopSSOutputAt(instanceElement, streamElement, offset);
			XMLUtils.addAttribute(streamElement,"state", String.valueOf(MediaRoomServiceConst.STREAM_STOPPED));
		}
	}

	public long getCurrentTimeStamp(Element streamElement) throws ServiceException,Exception {
		Element outputElement = XMLUtils.getElementXpathFromElement(streamElement, "Output");
		return this.streamingServer.getCurrentTC(outputElement);
	}

	public long getOffset(String mediaUUID,String timecodeStr, String srcDirPath) throws ServiceException,Exception {
		if(mediaUUID.isEmpty()) {
			throw new ServiceException(MediaRoomServiceExceptionCodes.MEDIA_UUID_NOT_FOUND,"Media UUID Not Found");
		}           	
		long timecode = Long.parseLong(timecodeStr);              
		return this.streamingServer.getOffsetAtTC(timecode,mediaUUID,srcDirPath);
	}

	public long getSyncTC(String sourceUUID, long offset,String rootDir) throws Exception {
		return this.streamingServer.getTCAtOffset(sourceUUID, offset, rootDir);
	}

	/*
	private void deleteInput(Element inputElement) throws Exception {
	    this.log.info(this.deviceDB.getDeviceResourceNID() + ": SourceTable:deleteInput deleting ssInstanceID=" + MRXMLUtils.getAttributeValue(inputElement,"ssInstanceID"));
	    this.streamingServer.deleteSSInput(inputElement);
	}
	 */

	void addOutput(Element instanceElement, Element outputElement, Element inputElement, 
			Document streamDataDoc,Document mediaSourceDataDoc, Element streamElement, long startOffset,
			String roomNID, String streamNID, String sourceNID) throws Exception {
		this.streamingServer.createSSOutput(outputElement, inputElement, streamDataDoc, mediaSourceDataDoc);
		streamElement.add(outputElement);	    
		String streamConnected = XMLUtils.getAttributeValueWithDefault(outputElement, "streamConnected", "true");
	   
		//do not start stream if stream connected is false. this value is set false for RTP streams right now
		if(streamConnected.equals("true")) {
			this.startSSOutput(instanceElement, streamElement, startOffset);
			this.streamingServer.startSSInput(instanceElement, streamElement);
		} 
		
	}

	public void stopAndDeleteOutput(Element instanceElement, Element streamElement, Element inputElement, Element outputElement, String streamNID, 
			Document streamDataDoc) throws Exception {
		try {
			this.stopSSOutputAt(instanceElement, streamElement, Long.parseLong(XMLUtils.getAttributeValue(inputElement,"offset")));
		}
		catch(Exception e) {
			this.log.info("SourceTable:stopOutput error=" + e.toString());
		}
		this.log.info(this.deviceDB.getDeviceResourceNID() + ": SourceTable:deleteOutput deleting ssInstanceID=" + XMLUtils.getAttributeValue(outputElement,"ssInstanceID"));
		this.deleteOutput(outputElement, streamDataDoc);
		
	}
	
	
	public void deleteOutput(Element outputElement,Document streamDataDoc) throws Exception {
		try {
			this.streamingServer.deleteSSOutput(outputElement, streamDataDoc);
		}  catch(Exception e) {
			e.printStackTrace();
			this.log.info("SourceTable:deleteOutput error=" + e.toString());
		}
	}

	public void addSourceInstance(Element instanceElement,Element inputElement, 
			Document streamDataDoc, Document mediaSourceDataDoc) throws Exception  {
		this.streamingServer.createSSInput(inputElement, streamDataDoc, mediaSourceDataDoc);
		instanceElement.add(inputElement);
		this.sourceTableDoc.getRootElement().add(instanceElement);
	}

	public void deleteSourceInstance(Element instanceElement) throws Exception {
		Element inputElement = instanceElement.element("Input");
		try {
			this.streamingServer.stopSSInput(inputElement);      	   
		}
		catch(Exception e) {
			this.log.info("SourceTable:stopInput error=" + e.toString());
		}
		try {
			this.streamingServer.deleteSSInput(inputElement);              	   
		}
		catch(Exception e) {
			this.log.info("SourceTable:deleteInput error=" + e.toString());
		}
		this.sourceTableDoc.getRootElement().remove(instanceElement);
		List<?> children = this.sourceTableDoc.getRootElement().elements();
		if(children.isEmpty() && gcStreamList.isEmpty()) {
			this.log.info("SourceTable:deleteSourceInstance sourceTable is empty resetting runningNumber");
			this.resetRunningNumber();
		}
	}
	
	public void addStreamToGCList(String id,Element objectElement) {
		this.gcStreamList.put(id, objectElement);
	}
	
	public Element removeStreamFromGCList(String id) {
		List<?> children = this.sourceTableDoc.getRootElement().elements();
		if(children.isEmpty() && gcStreamList.isEmpty()) {
			this.log.info("SourceTable:removeStreamFromGCList sourceTable is empty resetting runningNumber");
			this.resetRunningNumber();
		}
		return this.gcStreamList.remove(id);
	}
	
	public Element getGCStream(String id) {
		return this.gcStreamList.get(id);
	}
}
