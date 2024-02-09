package com.barco.device.handler.Layout;



import java.net.URLEncoder;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.base.DeviceI;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.base.ResponseHandlerI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.device.common.StreamEntry;
import com.barco.device.handler.MediaStore.AddFileRequestHandler;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.utils.MediaUtils;
import com.barco.utils.XMLUtils;

public class SetupRequestHandler implements MessageHandlerI, ResponseHandlerI{

	@Override	
	public void handleMessage(Message message, DeviceI device) throws Exception { //push sessionId and connectionId
		Document doc = message.getMessageDoc();
		String streamNID = XMLUtils.getValueXpath(doc, "//streamNID", "");

		Document streamDataDoc = MediaUtils.parseDataDoc(XMLUtils.getElementXpath(doc, "//streamdatadoc"));
		Document mediaSourceDataDoc = MediaUtils.parseDataDoc(XMLUtils.getElementXpath(doc, "//mediasourcedatadoc"));
		String roomNID = XMLUtils.getValueXpath(streamDataDoc, "//roomNID", "");
		String sourceAgentJID = XMLUtils.getValueXpath(streamDataDoc, "//sourceAgentJID", "");
		String destPortElementID = XMLUtils.getValueXpath(streamDataDoc,  "//destPortElementID", "");
		boolean preempt = false;
		try {
			StreamEntry se = this.setupStream(message, device, streamNID, destPortElementID, roomNID, sourceAgentJID, 
					streamDataDoc, mediaSourceDataDoc, preempt);
			Document resultDoc = XMLUtils.elementToDocument((Element)se.status.clone());
			message.setResult(resultDoc);
		}
		catch(ServiceException e) {
			e.printStackTrace();
			Document statusDoc = MediaUtils.createStreamStatusDoc();
		
			StreamEntry se = new StreamEntry(streamNID, destPortElementID, roomNID, sourceAgentJID, 
					streamDataDoc, mediaSourceDataDoc, preempt, (Element)(statusDoc.getRootElement().clone()));	
			
			se.status.element("lastEventData").add(XMLUtils.stringToElement("<Exception><Code>"+MediaAgentI.DEST_ERROR+"</Code><Description>"+e.getCode() + ":" + e.getData()+"</Description></Exception>"));
			se.setStatus(MediaAgentI.DEST_ERROR, device, true);
			throw e;
		}
		catch(Exception e) {
			e.printStackTrace();
			Document statusDoc = MediaUtils.createStreamStatusDoc();
		
			StreamEntry se = new StreamEntry(streamNID, destPortElementID, roomNID, sourceAgentJID, 
					streamDataDoc, mediaSourceDataDoc, preempt, (Element)(statusDoc.getRootElement().clone()));	
			
			se.status.element("lastEventData").add(XMLUtils.stringToElement("<Exception><Code>"+MediaAgentI.DEST_ERROR+"</Code><Description>"+e.toString()+"</Description></Exception>"));
			se.setStatus(MediaAgentI.DEST_ERROR, device, true);
			throw e;
		}

	}

	public StreamEntry setupStream(Message message, DeviceI device,
			String streamNID, String destPortElementID, String roomNID, String sourceAgentJID, 
			Document streamDataDoc, Document mediaSourceDataDoc, boolean preempt) throws Exception {

		// Testing for error
		// if(message != null)
		//	throw new ServiceException(ServiceExceptionCodes.RESOURCENOTAVAILABLE, "SetupRequestHandler:handleMessage portElementID Resource not available streamNID=" + streamNID);

		// Check if the streamNID exists
		StreamEntry se = device.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
		if(se != null) { // Teardown the previous stream
			try {
				TeardownRequestHandler handler = new TeardownRequestHandler();
				handler.teardownStream(device, streamNID);
			}
			catch(Exception e) {
				device.getLogger().info("SetupStreamRequestHandler:handleMessage stopping prev streamNID=" + streamNID + " error=" + e.toString());
				e.printStackTrace();
			}						
		}

	// Check if we have streams for this destPortElementIID
		if(!"".equals(destPortElementID)) { // There is a destPortElementID specified, this is empty in case of recordings
			String destNID = XMLUtils.getValueXpath(streamDataDoc, "//destNID", "");
			se = device.getDeviceDB().getStreamCache().getStreamByDestination(destPortElementID, destNID);
			System.out.println("destPortElementIID :="+destPortElementID);
			System.out.println("streamDataDoc :="+streamDataDoc.asXML());
			System.out.println("destNID :="+destNID);
			if(se != null) {
				if(!se.preempt)
					throw new ServiceException(ServiceExceptionCodes.RESOURCENOTAVAILABLE, "SetupRequestHandler:handleMessage portElementID Resource not available streamNID=" + streamNID);
				else {
					try {
						TeardownRequestHandler handler = new TeardownRequestHandler();
						handler.teardownStream(device, streamNID);
					}
					catch(Exception e) {
						device.getLogger().info("SetupStreamRequestHandler:handleMessage stopping prev streamNID=" + streamNID + " error=" + e.toString());
						e.printStackTrace();
					}

				}
			}
		}
		
		int streamState = MediaAgentI.WAITING_FOR_STREAMURL;
		//setup pvr/recording streams 
		String destType = XMLUtils.getValueXpath(streamDataDoc, "//destType", ""); 
		if(destType.equals("PVR") || destType.equals("RECORD")) {
			streamState  = MediaAgentI.WAITING_FOR_STREAMSTARTED;
			String dirNID = XMLUtils.getValueXpath(streamDataDoc, "//destFileParentDirNID", "");
			String fileUUID = XMLUtils.getValueXpath(streamDataDoc, "//destFileUUID", "");

			String dirRootPath = device.getMediaStoreAgentImpl().getDirPath(dirNID, "");
			Element destFileParentDirPath = DocumentHelper.createElement("destFileParentDirPath");
			destFileParentDirPath.setText(dirRootPath);
			streamDataDoc.getRootElement().add(destFileParentDirPath);

			String mediaType = MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaClip.toString();
			if(destType.equals("PVR")){
				mediaType = MediaStoreServiceConst.MEDIA_TYPE_ENUMS.PVRMediaClip.toString();
			}

			String streamType = XMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
			String destNID = XMLUtils.getValueXpath(streamDataDoc, "//destNID", "");

			Element fileElement = (Element)XMLUtils.getElementXpath(streamDataDoc, "//FileInfo").clone();
			fileElement.setName("File");
			fileElement.addAttribute("type", mediaType);
			fileElement.addAttribute("streamType", streamType);			
			Element profileXML =DocumentHelper.createElement("ProfileXML");
			Element profileElement  = (Element)XMLUtils.getElementXpath(streamDataDoc, "//profileXML/"+streamType+"MediaStreamProfile").clone();
			profileElement.setName(streamType+"MediaStreamProfileInfo");
			profileXML.add(profileElement);
			fileElement.add(profileXML);			
			
			new AddFileRequestHandler().initRecording(fileElement, device,  null, dirNID,MediaStoreServiceConst.BUSY,fileUUID,destNID);
			
			Element groupElement = (Element)XMLUtils.getElementXpath(streamDataDoc, "//FileInfo/GroupInfo");
			
			//add media group file in store if information present in stream data doc		
			if(groupElement != null) {
				groupElement = (Element)groupElement.clone();
				
				groupElement.setName("File");
				groupElement.addAttribute("type", MediaStoreServiceConst.MEDIA_TYPE_ENUMS.MediaGroup.toString());
				groupElement.addAttribute("streamType", "");
				groupElement.addAttribute(MediaStoreServiceConst.MEDIA_GROUP_ID_ATTRIBUTE_NAME, XMLUtils.getAttributeValueWithDefault(fileElement, MediaStoreServiceConst.MEDIA_GROUP_ID_ATTRIBUTE_NAME,""));
				groupElement.addAttribute(MediaStoreServiceConst.CLIP_ID_ATTRIBUTE_NAME, XMLUtils.getAttributeValueWithDefault(fileElement, MediaStoreServiceConst.CLIP_ID_ATTRIBUTE_NAME,""));
				
				String groupFileUUID = XMLUtils.getAttributeValueWithDefault(groupElement,"UUID","");
				String groupFileNID = XMLUtils.getAttributeValueWithDefault(groupElement,"NID","");
		
				new AddFileRequestHandler().initRecording(groupElement, device, null, dirNID, MediaStoreServiceConst.BUSY, groupFileUUID, groupFileNID);
			}
			
		}
		
		// Check if we need a destination UDP port
		this.getDestIPPort(device, streamDataDoc);

		Document statusDoc = MediaUtils.createStreamStatusDoc();
		se = new StreamEntry(streamNID, destPortElementID, roomNID, sourceAgentJID, 
				streamDataDoc, mediaSourceDataDoc, preempt, (Element)(statusDoc.getRootElement().clone()));		
		device.getDeviceDB().getStreamCache().addStreamEntry(streamNID, se);
		String streamURL = XMLUtils.getValueXpath(se.streamDataDoc, "//streamURL", "");
		if("".equals(streamURL)) // This is to be signaled
			this.sendStartRequest(device, streamNID,streamState);
		else{ 
			this.startPlaying(device, streamNID, destPortElementID, streamURL);
			device.getLogger().debug("SetupRequestHandler:handleMessage: Autostart stream state set to 8");        	
			se.setStatus(MediaAgentI.STREAM_STARTED, device, true);
		}
		return se;

	}

	protected void sendStartRequest(DeviceI device, String streamNID,int streamState) throws Exception {
		String requestNID = "";
		String clientdata = streamNID;
		String clientcallback = "";
		StreamEntry se = device.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
		if(se == null)
			throw new ServiceException(ServiceExceptionCodes.RESOURCENOTFOUND, "SetupRequestHandler:sendStartRequest streamNID=" + streamNID);
		se.status.element("state").setText(String.valueOf(streamState));
		String data =  "<StartRequestData>" + 
				"<streamNID>" + streamNID + "</streamNID>" +
				"<streamdatadoc>" + URLEncoder.encode(XMLUtils.documentToString(se.streamDataDoc), "UTF-8") + "</streamdatadoc>" +
				"<mediasourcedatadoc>" + URLEncoder.encode(XMLUtils.documentToString(se.mediasourceDataDoc), "UTF-8") + "</mediasourcedatadoc>" +
				"</StartRequestData>";
		Document requestDocument = XMLUtils.createRequestDocument("Media", "StartRequest", device.getMyJID(),
				requestNID, clientdata, clientcallback, data);
		Message startRequestMessage = Message.createRequest(requestDocument, se.sourceAgentJID, device.getMyJID());
		device.sendRequest(startRequestMessage, requestDocument, this);
	}

	@Override
	public void handleResponse(Message message, Document context, DeviceI device)
			throws Exception {
		Element statusElement = XMLUtils.getElementXpath(message.getMessageDoc(), "//StreamStatus");
		String streamNID = statusElement.attributeValue("streamNID");
		String URL = XMLUtils.getValueXpath(message.getMessageDoc(), "//URL", "");
		StreamEntry se = device.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
		device.getLogger().debug("SetupStreamRequestHandler:handleResponse streamURL=" + URL + " streamNID=" + streamNID);

		if(se == null)
			throw new ServiceException(ServiceExceptionCodes.RESOURCENOTFOUND, "SetupRequestHandler:handleResponse streamNID=" + streamNID);
		
		se.status = (Element)(statusElement.clone());
		
		String destType = XMLUtils.getValueXpath(se.streamDataDoc, "//destType", ""); 
		
		//in case of recording and pvr stream url going to be none We no need to update the stream state as waiting for stream start.
		if(destType.equals("PVR") || destType.equals("RECORD")) {
			device.getLogger().info("SetupStreamRequestHandler:handleResponse for recording streams do not update stream state");
			return;
		}
	
		se.status.element("state").setText(String.valueOf(MediaAgentI.WAITING_FOR_STREAMSTARTED));

		this.startPlaying(device, streamNID, se.destPortElementID, URL);

		
		se.publishStatus(device, true);
		//device.addLog("SetupStreamRequestHandler:handleResponse xml=" + message.getMessageDoc().asXML(), LoggerI.DEBUG, LoggerI.DEBUG);
	}

	@Override
	public void handleError(Message response, Document context,
			DeviceI device) throws Exception {
		Document doc = response.getMessageDoc();
		Element errorElement = XMLUtils.getElementXpath(doc, "//BarcoError");
		String streamNID = XMLUtils.getValueXpath(doc, "//ClientData", "");
		StreamEntry se = device.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
		if(se == null)
			throw new ServiceException(ServiceExceptionCodes.RESOURCENOTFOUND, "SetupRequestHandler:handleResponse streamNID=" + streamNID);
		se.status.element("state").setText(String.valueOf(MediaAgentI.START_ERROR));
		Element lastEventData = se.status.element("lastEventData");
		if(lastEventData != null)
			se.status.remove(lastEventData);
		se.status.add(errorElement.detach());

		device.getLogger().debug("SetupStreamRequestHandler:handleError streamNID=" + streamNID + " error=" + errorElement.asXML());
		se.publishStatus(device, true);
	}

	public void startPlaying(DeviceI device, String streamNID, String destPortElementID, String URL) {
		device.getLayoutAgentImpl().startPlaying(streamNID, destPortElementID, URL);
		try {
			device.getMediaAgentImpl().startPlaying(device, "Layout." + streamNID, destPortElementID, URL);
		}
		catch(Exception e) {
			e.printStackTrace();
			device.getLogger().error("SetupRequestHandler:startPlaying error=" + e.toString());
		}
	}
	
	private boolean needsUDPDestPort(String streamType, String destType) {
		return ((!("PVR").equals(destType) && !("RECORD").equals(destType)) 
				&& (("RTP").equals(streamType) || ("MPEGTS").equals(streamType) || ("UDP").equals(streamType)));
	}
	   

	private void getDestIPPort(DeviceI device, Document streamDataDoc)
			throws Exception { // TBD
		// Set the destIP and destPort
		String streamType = XMLUtils.getValueXpath(streamDataDoc, "//streamType", "");
		String destType = XMLUtils.getValueXpath(streamDataDoc, "//destType", "");
		
		if (!this.needsUDPDestPort(streamType, destType))
			return;

		String destStreamURL = XMLUtils.getValueXpath(streamDataDoc, "//destStreamURL", "");
		//TBD:: Should check for WebClient-Device?
		if (!"".equals(destStreamURL)) // Is Un Managed destination 
			return;

		String streamURL = XMLUtils.getValueXpath(streamDataDoc, "//streamURL", "");
		if (!"".equals(streamURL)) 
			return;
		
		String destUDPStartPort = XMLUtils.getValueXpath(streamDataDoc,	"//destUDPStartPort", "");
		if (!"".equals(destUDPStartPort))
			return;

		destUDPStartPort = device.getMediaAgentImpl().updateNextUDPStartPort(streamType);
		
		device.getLogger().debug("SetupRequestHandler:getDestIPPort Allocating port destUDPStartPort=" + destUDPStartPort + " destType=" + destType + " streamType=" + streamType);
		
		XMLUtils.setValueXpath(streamDataDoc, "//destUDPStartPort",	destUDPStartPort);
	}
	    
	    


}
