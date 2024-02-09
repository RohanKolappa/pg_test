package com.barco.device.vng.Media;

import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.UUID;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;

public class XPMediaAgentImpl extends BaseMediaAgentImpl{

	// MIYE TBR once handleConfigUpdated is called
	//boolean configUpdated = true;
	Set<String> destinationPortId = new HashSet<String>();

	public void initAgent(DeviceFrameworkI deviceFrameWork) {
		super.initAgent(deviceFrameWork);

		try {

			Document deviceDoc = this.deviceFrameWork.getDeviceDB().getDeviceConfig();
			List<?> portList = XMLUtils.getListOfNodesXpath(deviceDoc, "//XP200PortConfig/PortID[../Info[PortIOType='StreamDst']");

			Element element=null;
			for (Object object:portList) {
				element = (Element) object;
				createRxSplashScreenStream(element.getText());
			}

			portList = XMLUtils.getListOfNodesXpath(deviceDoc, "//XP200PortConfig/PortID[../Info[PortIOType='StreamSrc']");

			for (Object object:portList) {
				element = (Element) object;
				createThumbnailStream(element.getText());
			}

		} catch (Exception e) {
			//todo log error message
			e.printStackTrace();
		}
	}


	protected String generateSourceInstanceNID(String sourceNID,String portID, String streamType, String instanceType, String mediaSourceNID, String timelineID) {
        return portID + "." + streamType+ "." + MediaRoomServiceConst.SHARED;
    }

	private void createThumbnailStream(String portId) {

		String requestNID = UUID.randomUUID().toString();

		try {

			Document mediaroomDoc = XMLUtils.loadXMLResource("/com/ipvs/xsd/objectXml/MediaRoom.xml");
			Document profileXML = XMLUtils.loadXMLResource("/com/ipvs/xsd/objectXml/V2DMediaStreamProfile.xml");


			Element streamDataElement = XMLUtils.getElementXpath(mediaroomDoc, "//Stream/data");
			Element mediaSourcDataElement= XMLUtils.getElementXpath(mediaroomDoc, "//MediaSource/data");
			String prefix = portId+"_";
			XMLUtils.setValueXpathElement(streamDataElement, "./sourceNID", prefix+INTERNAL_INSTANCE);
			XMLUtils.setValueXpathElement(streamDataElement, "./destType", MediaRoomServiceConst.OUTPUT_IMAGE_WRITER);
			XMLUtils.setValueXpathElement(streamDataElement, "./streamType", "V2D");
			XMLUtils.setValueXpathElement(streamDataElement, "./trackOffsetAtStart", String.valueOf(0));
			Element profileXMLElement = XMLUtils.getElementXpathFromElement(streamDataElement, "profileXML");
			profileXMLElement.add(profileXML.getRootElement().detach());
			XMLUtils.setValueXpathElement(streamDataElement, "./mediaSourceNID",prefix+INTERNAL_INSTANCE+"_MEDIASOURCE_NID");

			XMLUtils.setValueXpathElement(mediaSourcDataElement, "./sourceNID", prefix+INTERNAL_INSTANCE);
			XMLUtils.setValueXpathElement(mediaSourcDataElement, "./sourcePortElementID", portId);
			XMLUtils.setValueXpathElement(mediaSourcDataElement, "./sourceType", MediaRoomServiceConst.INPUT_ENCODER);
			XMLUtils.setValueXpathElement(mediaSourcDataElement, "./trackNID", prefix+INTERNAL_INSTANCE+"_TRACK_NID");


			startStream(this.deviceFrameWork, requestNID, portId+"_"+INTERNAL_INSTANCE+"_STREAM_NID", XMLUtils.elementToDocument(streamDataElement), XMLUtils.elementToDocument(mediaSourcDataElement));
			System.out.println("sourceTable:="+this.sourceTable.getDoc().asXML());
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	private void createRxSplashScreenStream(String portId) throws Exception {
		deviceFrameWork.getLogger().info("XPMediaAgentImpl: CreateRxSplashscreen portId:="+portId);
		startPlaying(deviceFrameWork, UUID.randomUUID().toString(), portId, MediaRoomServiceConst.SPLASHSCREEN_URL);
	}


	private void teardownRxSplashScreenStream(String portId) throws Exception {
		deviceFrameWork.getLogger().info("XPMediaAgentImpl: teardownRxSplashScreenStream portId:="+portId);
		Element streamElement = XMLUtils.getElementXpath(this.sourceTable.sourceTableDoc, "//Stream[Output/@destPortElementID='" + portId + "']");
		if(streamElement == null){
			return;
		}
		super.stopPlaying(XMLUtils.getAttributeValueWithDefault(streamElement, "NID", ""));
	}

	public String getInstanceType(String sourceType) {
		String instanceType = MediaRoomServiceConst.SHARED;
		return instanceType;
	}

	public String getInputType(String sourceType, Document streamDataDoc) {
		return sourceType;
	}

	public void updateInputElement(String inputType, Element inputElement, String streamType,
			Document streamDataDoc, Document mediaSourceDataDoc, Document sourceTableDoc)  throws Exception {
		if(MediaRoomServiceConst.INPUT_STREAM.equals(inputType)) { // For an Rx Input
			String destPortElementID = XMLUtils.getValueXpath(streamDataDoc, "//destPortElementID", "");
			XMLUtils.addAttribute(inputElement,"destPortElementID", destPortElementID);
		}
		if(MediaRoomServiceConst.INPUT_ENCODER.equals(inputType)){ // For a Tx Input
			String sourcePortElementID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourcePortElementID", "");
			String isMulticast = XMLUtils.getValueXpath(streamDataDoc, "//IsMulticast", "false");
			XMLUtils.addAttribute(inputElement,"sourcePortElementID", sourcePortElementID);
			XMLUtils.addAttribute(inputElement,"isMulticast", isMulticast);


			// Check if we have other inputs that use the same sourcePortElementID
			Element prevInputElement = XMLUtils.getElementXpath(sourceTableDoc,"//Input[@sourcePortElementID='" + sourcePortElementID + "' and @sourceNID!='"+INTERNAL_INSTANCE+"']");
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
		if(MediaRoomServiceConst.OUTPUT_DECODER.equals(outputType)) { // For an Rx Input
			String destPortElementID = XMLUtils.getValueXpath(streamDataDoc,"//destPortElementID", "");
			XMLUtils.addAttribute(outputElement,"destPortElementID", destPortElementID);
		}
	}

	public void handleConfigUpdated(Document doc) throws Exception {

		//handle common config updates like port range,multicast ipaddress pool
		super.handleConfigUpdated(doc);

		//handle port type changed

		String newPortType = XMLUtils.getValueXpath(doc, "//Info/PortIOType","");
		if(newPortType.isEmpty()){
			return;
		}


		String portIDXpath = XMLUtils.getAttributeValueWithDefault(XMLUtils.getElementXpath(doc, "//DeviceConfig"), "xpath", "");

    	//get port id from xpath
    	String regex = "../PortID\\s*=\\s*(.*?)]";
		Matcher matcher = Pattern.compile(regex).matcher(portIDXpath);
		String portID = "";
		if(matcher.find()) {
			portID= matcher.group(1);
		}

		if(portID == null || portID.isEmpty()) {
			return;
		}

		deviceFrameWork.getLogger().debug("XPMediaAgentImpl:handleConfigUpdated portTypeChanged portId:="+portID+ " newType:="+newPortType);

		//special case to handle where if the port type not changed but in the back ground it is resetting the streaming server and deleting the splash screen
		teardownRxSplashScreenStream(portID);
		deviceFrameWork.getLogger().debug("XPMediaAgentImpl:handleConfigUpdated sourceTable after teardown splash screen:="+this.sourceTable.sourceTableDoc.asXML());

		if(newPortType.equals("StreamDst")){
			createRxSplashScreenStream(portID);
			deviceFrameWork.getLogger().debug("XPMediaAgentImpl:handleConfigUpdated sourceTable after creating decoder splash screen:="+this.sourceTable.sourceTableDoc.asXML());
		} else {
			createThumbnailStream(portID);
			deviceFrameWork.getLogger().debug("XPMediaAgentImpl:handleConfigUpdated sourceTable after creating thumbnail stream :="+this.sourceTable.sourceTableDoc.asXML());

		}
	}


	Hashtable<String, String> parseURL(String streamURL) {

		StringTokenizer st = new StringTokenizer(streamURL, "?");
		String s = st.nextToken(); // Consume the IP port token
		Hashtable<String, String> table = new Hashtable<String,String>();
		while(st.hasMoreTokens()) {
			s = st.nextToken();
			StringTokenizer stt = new StringTokenizer(s, "=");
			String name = stt.nextToken();
			String value = stt.nextToken();
			table.put(name, value);
		}
		return table;
	}

	boolean sameProfile(String prevStreamURL, String newStreamURL) {
		Hashtable<String, String> prevProfile = this.parseURL(prevStreamURL);
		Hashtable<String, String> newProfile = this.parseURL(newStreamURL);
		String[]  params = { "Bandwidth", "AVOption", "IsMulticast", "EnableKbm"};
		for(String s : params) {
			String param = s.toLowerCase();
			if(!prevProfile.get(param).equals(newProfile.get(param)))
				return false;
		}
		return true;
	}


	public void startPlayingInternal(DeviceFrameworkI deviceFramework, String streamNID, String roomNID,
			String destType, String streamURL, String streamType, String destPortElementID, String mediaDestNID, Element profileXML)
					throws Exception {

		if(super.startPlayingInternalRx(deviceFramework, streamNID, roomNID, destType, streamURL, streamType, destPortElementID, mediaDestNID, profileXML)){
		    super.startPlayingInternal(deviceFramework, streamNID, roomNID, destType, streamURL, streamType, destPortElementID, mediaDestNID, profileXML);
		}

	}

	//if the url is splash screen url don't add input
	protected void startInput(Document streamDataDoc, Element inputElement)  throws Exception{
		// Start the input
	    String streamURL = XMLUtils.getAttributeValueWithDefault(inputElement, "streamURL","");
	    if(!streamURL.equals(MediaRoomServiceConst.SPLASHSCREEN_URL))
		super.startInput(streamDataDoc, inputElement);

	}

	public void stopPlaying(String streamNID) throws Exception {

		super.stopPlayingRx(streamNID);
	}

	protected Element createOutputElement(String ssInstanceNID, String outputType, Document streamDataDoc, Element inputElement) throws Exception {
		Element outputElement = super.createOutputElement(ssInstanceNID, outputType, streamDataDoc, inputElement);

	    if(outputType.equals(MediaRoomServiceConst.OUTPUT_IMAGE_WRITER)) {
			String sourcePortElementID = XMLUtils.getAttributeValueWithDefault(inputElement, "sourcePortElementID","");
		    XMLUtils.addAttribute(outputElement,"dirPath", MediaRoomServiceConst.THUMBNAIL_PATH+sourcePortElementID);
		    XMLUtils.addAttribute(outputElement,"publishURL", "");
		    XMLUtils.addAttribute(outputElement,"destPortElementID", sourcePortElementID);
	    }

	    return outputElement;

	}


	@Override
	public void stopPostingThumbnail(String sourceNID,String portId) throws Exception {
		this.deviceFrameWork.getLogger().info("XPMediaAgentImpl::stopPostingThumbnail Begin");
		String outpuXpath = "//Instance[Input[@sourcePortElementID='" + portId + "']]/MediaSource/Stream/Output[@outputType='OutputImageWriter']";
	    Element outputElement = XMLUtils.getElementXpath(this.sourceTable.getDoc(), outpuXpath);
	    if(outputElement == null) {
	    	return;
	    }
	    outputElement.addAttribute("publishURL", "");

	    this.streamingServer.updateSSOutput(outputElement, null);

	    this.deviceFrameWork.getLogger().info("XPMediaAgentImpl::stopPostingThumbnail End");
	}

	@Override
	public void startPostingThumbnail(String sourceNID,String portId, String publishURL) throws Exception {

		this.deviceFrameWork.getLogger().info("XPMediaAgentImpl::startPostingThumbnail Begin");
		String outpuXpath = "//Instance[Input[@sourcePortElementID='" + portId + "']]/MediaSource/Stream/Output[@outputType='OutputImageWriter']";

	    Element outputElement = XMLUtils.getElementXpath(this.sourceTable.getDoc(), outpuXpath);
	    outputElement.addAttribute("publishURL", publishURL);

	    this.streamingServer.updateSSOutput(outputElement, null);

	    this.deviceFrameWork.getLogger().info("XPMediaAgentImpl::startPostingThumbnail End");
	}


	@Override
	protected void updateInstanceElement(Element instanceElement,
			String sourceNID,String sourcePortElementID, Document streamDataDoc, long offset, long syncTC,
			double timescale, Document mediaSourceDataDoc, String timelineID) throws Exception{

		XMLUtils.addAttribute(instanceElement,"timelineID" ,  XMLUtils.getValueXpath(mediaSourceDataDoc, "//trackNID",""));


		//find internal instance and update with actual stream information
		Element internalInputElement = XMLUtils.getElementXpathFromElement(instanceElement,"./Input[@sourcePortElementID='" + sourcePortElementID + "' and @sourceNID='"+sourcePortElementID+"_"+INTERNAL_INSTANCE+"']");
		if(internalInputElement != null)
		{
			String isMulticast = XMLUtils.getValueXpath(streamDataDoc, "//IsMulticast", "false");
			XMLUtils.addAttribute(internalInputElement, "sourceNID", XMLUtils.getValueXpath(streamDataDoc, "//sourceNID", ""));
			XMLUtils.addAttribute(internalInputElement,"isMulticast" ,  isMulticast);
		}

		Element inputElement = XMLUtils.getElementXpathFromElement(instanceElement, "./Input");
		String inputType = XMLUtils.getAttributeValue(inputElement,"inputType");

		//Thumbnail stream is created with default profile. when client is connecting we have to run set_tx_connection param
		if(inputType.equals(MediaRoomServiceConst.INPUT_ENCODER)){
			streamingServer.updateSSInput(inputElement, streamDataDoc);
		}

		return;

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
