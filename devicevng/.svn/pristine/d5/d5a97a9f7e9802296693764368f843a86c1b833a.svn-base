package com.barco.device.vng.Media;

import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class V2DTxMediaAgentImpl extends BaseMediaAgentImpl{


	public String getInstanceType(String sourceType) {
	    String instanceType = MediaRoomServiceConst.SHARED;
	    return instanceType;
	}

	public String getInputType(String sourceType, Document streamDataDoc) {
		return sourceType;
	}

	public void updateInputElement(String inputType, Element inputElement, String streamType,
			Document streamDataDoc, Document mediaSourceDataDoc, Document sourceTableDoc)  throws Exception {
	    // Set the inputType and input type specific parameters
		/* TBD Rx
	    if(MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) { // For an Rx Input
	        String destPortElementID = MRXMLUtils.getValueXpath(streamDataDoc, "//destPortElementID", "");
	        MRXMLUtils.addAttribute(inputElement,"destPortElementID", destPortElementID);
	    }
	    */
	    if(MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)){ // For a Tx Input
		    String sourcePortElementID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourcePortElementID", "");
			String isMulticast = XMLUtils.getValueXpath(streamDataDoc, "//IsMulticast", "false");
		    XMLUtils.addAttribute(inputElement,"sourcePortElementID", sourcePortElementID);
	        XMLUtils.addAttribute(inputElement,"isMulticast", isMulticast);

	        // Check if we have other inputs that use the same sourcePortElementID
	    	Element prevInputElement = XMLUtils.getElementXpath(sourceTableDoc,"//Input[@sourcePortElementID='" + sourcePortElementID + "']");
	    	if(prevInputElement != null) { // Check if we can reuse the sourcePortElement
	    		String prevIsMulticast = prevInputElement.attributeValue("isMulticast");
	            if("false".equals(isMulticast) || "false".equals(prevIsMulticast)) {
	            	throw new ServiceException(MediaRoomServiceExceptionCodes.ENCODERPORTNOTAVAILABLE, "Encoder Port Not Available");
	            }
	        }
	    	else {
	        	// Check if we have enough encoder resources by finding the number of unique sourcePortElementId
	        	Hashtable<String,String> portIDTable = new Hashtable<String, String>();
	        	List<?> inputList = XMLUtils.selectNodesXpath(sourceTableDoc.getRootElement(), "//Input[@type='" + MediaRoomServiceConst.INPUT_ENCODER + "']");
	        	for(Object i : inputList) {
	        		String portID = ((Element)i).attributeValue("sourcePortElementID");
	        		portIDTable.put(portID, portID);
	        	}
	        	int maxEncoderInstances = sourceTable.streamingServer.getMaxEncoderInstances(streamType);
	            if((maxEncoderInstances > 0) && (portIDTable.size() >= maxEncoderInstances))
	            	throw new ServiceException(MediaRoomServiceExceptionCodes.MAXENCODERINSTANCESEXCEEDED, "Max Encoder Instances Exceeded");
	    	}
	    }
	}

	public void setupExternalInput(String inputNID, Element inputElement, String sourceNID, String inputType, String streamType,
			Document streamDataDoc, long offset, long startTimecode, long syncTC, double timescale, Document mediaSourceDataDoc,
			Document sourceTableDoc) throws Exception {
        XMLUtils.addAttribute(inputElement,"state", String.valueOf(BaseMediaAgentImpl.INPUT_OK));
	}

	 public void teardownExternalInput(Element inputElement) throws Exception {
		 // Nothing to be done
	 }

	 public void updateOutputElement(Element outputElement, String outputType, Document streamDataDoc) throws Exception {
	    // Set output specific parameters
	    /* TBD Rx
	    if(MediaRoomServiceConst.OUTPUT_DECODER.equals(outputType)) { // For an Rx Input
		    String destPortElementID = MRXMLUtils.getAttributeValue(inputElement,"destPortElementID");
		    MRXMLUtils.addAttribute(outputElement,"destPortElementID", destPortElementID);
	    } else if(MediaRoomServiceConst.OUTPUT_STREAM.equals(outputType)) {
	    */
	 }

	@Override
	public void startPostingThumbnail(String sourceNID, String portId, String publishURL)
			throws Exception {
		// TODO Auto-generated method stub

	}

	@Override
	public void stopPostingThumbnail(String sourceNID,String portId) throws Exception {
		// TODO Auto-generated method stub

	}

	@Override
	protected void updateInstanceElement(Element instanceElement,
			String sourceNID, String sourcePortElementID,
			Document streamDataDoc, long offset, long syncTC, double timescale,
			Document mediaSourceDataDoc, String timelineID) throws Exception {
		// TODO Auto-generated method stub

	}

	@Override
	public void handleTimelineUpdate(DeviceFrameworkI deviceFramework,
			Document doc) throws Exception {
		// TODO Auto-generated method stub

	}

	@Override
	public void handleMediaSourceUpdate(DeviceFrameworkI deviceFramework,
			Document doc) throws Exception {
		// TODO Auto-generated method stub

	}

	@Override
	public void releaseUDPStartPort(String destUDPStartPort, String streamType)
			throws Exception {
		// TODO Auto-generated method stub
	}

	@Override
	public String updateNextUDPStartPort(String streamType) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void handleConnected(Document eventDoc) throws Exception {
		// TODO Auto-generated method stub

	}
}
