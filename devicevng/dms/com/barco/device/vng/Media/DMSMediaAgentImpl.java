package com.barco.device.vng.Media;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.common.StreamEntry;
import com.barco.device.vng.utils.MediaTimelineUtils;
import com.barco.device.vng.utils.VNGConst;
import com.barco.utils.MediaUtils;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;
import com.barco.utils.mr.MediaRoomServiceConst;
import com.barco.utils.mr.MediaRoomServiceExceptionCodes;
import com.barco.utils.mr.XMPPTransaction;



public class DMSMediaAgentImpl extends RelayMediaAgentImpl {

    
	public void updateInputElement(String inputType, Element inputElement, String streamType,
			Document streamDataDoc, Document mediaSourceDataDoc, Document sourceTableDoc)  throws Exception {
		if(MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
	        String mediaUUID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileUUID", "");
	        String mediaParentDirNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileParentDirNID", "");
	        String dirPath = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceFileParentDirPath","");
	        XMLUtils.addAttribute(inputElement,"mediaUUID", mediaUUID);
	        XMLUtils.addAttribute(inputElement,"mediaParentDirNID", mediaParentDirNID);
	        XMLUtils.addAttribute(inputElement,"dirPath", dirPath);

	        //add ffTracks supported or not
	        String ffTracks = XMLUtils.getValueXpath(streamDataDoc, "//FFTracks", "");
	        if(ffTracks.isEmpty()){
	        	XMLUtils.addAttribute(inputElement,"ffTracks",String.valueOf(Boolean.FALSE));
	        } else {
	        	XMLUtils.addAttribute(inputElement,"ffTracks",String.valueOf(Boolean.TRUE));
	        }
		}
	}


	 public void updateOutputElement(Element outputElement, String outputType, Document streamDataDoc) throws Exception {
	    if(outputType.equals(MediaRoomServiceConst.OUTPUT_RECORD) ||
		    		outputType.equals(MediaRoomServiceConst.OUTPUT_PVR)) {
	    	String destFileUUID  = XMLUtils.getValueXpath(streamDataDoc, "//destFileUUID","");
	    	String destFileParentDirNID  = XMLUtils.getValueXpath(streamDataDoc, "//destFileParentDirNID","");
	    	XMLUtils.addAttribute(outputElement,"destFileUUID", destFileUUID);
	    	XMLUtils.addAttribute(outputElement,"destFileParentDirNID", destFileParentDirNID);
	    	String dirPath = XMLUtils.getValueXpath(streamDataDoc, "//destFileParentDirPath","");
	    	XMLUtils.addAttribute(outputElement,"dirPath", dirPath);
	    }
	    
		String streamURLType = XMLUtils.getValueXpath(streamDataDoc, "//streamURLType", "");
	    //do not start rtp stream for output stream. Get SDP should trigger starting output stream
		if(MediaRoomServiceConst.HTTP.equals(streamURLType)) {
			XMLUtils.addAttribute(outputElement, "streamConnected", "false");
		}
	}



	public void stopSSOutputOnDiskFull(Element streamElement, int eventCode) throws Exception {
		Element mediaSourceEl = streamElement.getParent();
		Element instanceEl = mediaSourceEl.getParent();
		Element inputEl = instanceEl.element("Input");
		long offset = Utils.getLongValue(XMLUtils.getAttributeValue(inputEl,"offset"), 0);
		streamElement.addAttribute(VNGConst.ERROR_CODE, String.valueOf(eventCode));
		this.sourceTable.stopSSOutputAt(instanceEl, streamElement, offset);
	}



	@Override
	public void startPostingThumbnail(String sourceNID,String portId, String publishURL)
			throws Exception {
		
	    

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

	public void sendUpdateSyncAction(Element timelineUpdate,long syncTC) throws Exception {
		String requestNID = "";

		// Generate the new actiondata
		XMLUtils.addAttribute(timelineUpdate, "syncTC", String.valueOf(syncTC));

		// Now replace the actiondata in the original request
		Document timelineUpdateRequestData = XMLUtils.stringToDocument("<TimelineUpdateRequestData><actiondata/></TimelineUpdateRequestData>");
		Element actionData = timelineUpdateRequestData.getRootElement().element("actiondata");
		actionData.add((Element) timelineUpdate.clone());

		String syncSourceAgentJIDList = timelineUpdate.attributeValue("syncSourceAgentJIDList");
		StringTokenizer st = new StringTokenizer(syncSourceAgentJIDList, ",");
		while (st.hasMoreTokens()) {
			Document requestDoc = XMLUtils.createRequestDocument("Media","TimelineUpdateRequest", this.deviceFrameWork.getMyJID(),requestNID, "", this.getClass().getName(), timelineUpdateRequestData.getRootElement().asXML());
			Message syncTimelineUpdateRequest = Message.createRequest(requestDoc, st.nextToken(),this.deviceFrameWork.getMyJID());
			this.deviceFrameWork.sendMessage(syncTimelineUpdateRequest);
		}
	}

	boolean propogateTimelineUpdateToRelaySource(Element msElement, String timescale, Element originalTimelineUpdate, String actionNID) throws Exception {

		//String s = originalTimelineUpdate.asXML();
		Element mediaTimeline = originalTimelineUpdate.element("MediaTimeline");
        // Do not propogate if it is not a relay
		Element instanceElement = msElement.getParent();
        Element inputElement = instanceElement.element("Input");
        if (!MediaRoomServiceConst.INPUT_RELAY.equals(inputElement.attributeValue("inputType")))
            return false;

		// Do not propoage if timelineID does not match what is in the Instance
	    String timelineID = mediaTimeline.attributeValue("id");
	    if (!timelineID.equals(instanceElement.attributeValue("timelineID")))
	    	return false;

        // Do not propogate if it is a switch update
	    String sourceaction = mediaTimeline.element("Last").attributeValue("sourceAction");
        if (MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(sourceaction) || MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(sourceaction))
            return false;

        // Do not propogate start/stop if the source/instance is shared
        String instanceType = instanceElement.attributeValue("instanceType");
        if (MediaRoomServiceConst.SHARED.equals(instanceType)) {
            if (MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(sourceaction) || MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceaction)) {
                return false;
            }
        }

        // Do not propogate of the relay upstream is not yet setup
        String streamNID = inputElement.attributeValue("streamNID");
        if("".equals(streamNID))
        	return false;

		StreamEntry entry = deviceFrameWork.getDeviceDB().getStreamCache().getStreamEntry(streamNID);
        if(entry == null)
			throw new ServiceException(MediaRoomServiceExceptionCodes.RELAYDESTINSTANCENOTFOUND, "Relay instance not found for streamNID=" + streamNID);

        // String roomNID = inputElement.attributeValue("roomNID");
        Document relayStreamDataDoc = entry.streamDataDoc;

        // Generate the new relayTimelineUpdate
		Element relayTimelineUpdate = (Element)originalTimelineUpdate.clone();
		Element relayMediaTimeline = relayTimelineUpdate.element("MediaTimeline");

		Element last = relayMediaTimeline.element("Last");
		long offset = Long.parseLong(last.attributeValue("Offset"));

        // Adjust the offset to match the relayMediaSource Start
        Document relayMediaSourceDataDoc = entry.mediasourceDataDoc;
        if(relayMediaSourceDataDoc == null)
			throw new ServiceException(MediaRoomServiceExceptionCodes.RELAYDESTINSTANCENOTFOUND, "Relay mediasource not found for streamNID=" + streamNID);
        long relayRoomMediaSourceStartTimecode = Utils.getLongValue(XMLUtils.getValueXpath(relayMediaSourceDataDoc, "//startTimecode", null),0);
        if(relayRoomMediaSourceStartTimecode > 0) { // This is a PVR relay stream for live source which has a startTimecode
            long userRoomMediaSourceStartTimecode = Utils.getLongValue(msElement.attributeValue("startTimecode"),0);
            // long timelineOffset = Long.parseLong(actiondata);
            // long relayRoomTimelineOffset = userRoomMediaSourceStartTimecode + timelineOffset - relayRoomMediaSourceStartTimecode;
            // This will ensure that the PVR source will set the tc= mediaSource.startTimecode + relayRoomTimelineOffset
            // This is the same tc value as it would have been on this relay
            offset = userRoomMediaSourceStartTimecode + offset - relayRoomMediaSourceStartTimecode;
            last.addAttribute("Offset", String.valueOf(offset));
        }

        Element relayStreamNIDList = XMLUtils.stringToElement("<streamNIDList><streamNID>" + streamNID + "</streamNID></streamNIDList>");
    	Element tmpStreamNIDList = relayTimelineUpdate.element("streamNIDList");
    	relayTimelineUpdate.remove(tmpStreamNIDList);
    	relayTimelineUpdate.add(relayStreamNIDList);

        // This is the timelineID that is used when creating the InputInstance in the relaySource
        String relayTimelineID = XMLUtils.getValueXpath(relayMediaSourceDataDoc, "//trackNID","");
        relayMediaTimeline.addAttribute("id",relayTimelineID);

        // Generate the actiondata
    	Element actiondata = XMLUtils.stringToElement("<actiondata/>");
    	actiondata.add(relayTimelineUpdate);

		String data =  "<TimelineUpdateRequestData>" + relayTimelineUpdate.asXML() +	"</TimelineUpdateRequestData>";
		Document requestDocument = XMLUtils.createRequestDocument("Media", "TimelineUpdateRequest", deviceFrameWork.getMyJID(),
				"", "", "", data);
        String relaySourceAgentJID = XMLUtils.getValueXpath(relayStreamDataDoc, "//sourceAgentJID","");
		Message timlineUpdateRequestMessage = Message.createRequest(requestDocument, relaySourceAgentJID, deviceFrameWork.getMyJID());
		deviceFrameWork.sendRequest(timlineUpdateRequestMessage, requestDocument, this);


    	return true;
	}

	@Override
	public void handleTimelineUpdate(DeviceFrameworkI device, Document doc)
			throws Exception {
		device.getLogger().info("DMSMediaAgentImpl: handleTimelineUpdate Start");
		String actionNID = XMLUtils.getValueXpath(doc, "//actionNID", "");
    	List<?> timelineUpdateList = XMLUtils.getListOfNodesXpath(doc, "//timelineupdate");
    	for(Object o: timelineUpdateList) {
    		Element timelineUpdate = (Element)o;
        	this.handleSingleTimelineUpdate(device, actionNID, timelineUpdate);
    	}
	}

	public void handleSingleTimelineUpdate(DeviceFrameworkI device, String actionNID, Element timelineUpdate)
			throws Exception {
		device.getLogger().info("DMSMediaAgentImpl: handleSingleTimelineUpdate Start");

		/*
		Element timelineUpDataEl = XMLUtils.getElementXpath(doc, "//TimelineUpdateRequestData");
		Document dataDoc = XMLUtils.elementToDocument(timelineUpDataEl);
		Document data = (Document)dataDoc.clone();
		*/

		// Get StreamNIDList from the timeline update
		List<?> streamNIDList = XMLUtils.selectNodesXpath(timelineUpdate, "//streamNID");

		/*
		 * Eliminate the Rx Streams???
		 */
		if (streamNIDList.isEmpty()) {
			device.getLogger().info("DMSMediaAgentImpl: handleSingleTimelineUpdate no streams found in streamNIDList");
			return;
		}

		Element mediaTimeline = timelineUpdate.element("MediaTimeline");
		boolean sync = "true".equals(timelineUpdate.attributeValue("sync"));
		long syncTC = 0;
		Element last = mediaTimeline.element("Last");
		long offset = Long.parseLong(last.attributeValue("Offset"));
		String timescale = last.attributeValue("TimeScale");
		String sourceAction = last.attributeValue("sourceAction");


		StringBuffer sb = null;

		for (Object o : streamNIDList) {
			Element streamNIDElement = (Element) o;

			if (sb == null) {
				sb = new StringBuffer("Stream[");
			} else {
				sb.append(" or ");
			}
			sb.append("@NID='" + streamNIDElement.getText() + "' and ./Output/@streamConnected='true'");
		}
		sb.append("]");
		String streamXPath = sb.toString();

		// fetch the mediasource element list from the sourcetable
		Document sourceTableDoc = this.sourceTable.getDoc();
		String mediasourceStreamXPath = streamXPath;

		//if sync is true make sure we are starting all the streams at once
		if(sync) {
			mediasourceStreamXPath = mediasourceStreamXPath.replace("and ./Output/@streamConnected='true'", "");
		}
		List<?> mediaSourceElList = XMLUtils.getListOfNodesXpath(sourceTableDoc, "//MediaSource[" + mediasourceStreamXPath + "]");
		List<?> streamElList = XMLUtils.getListOfNodesXpath(sourceTableDoc, "//" + streamXPath);

		// TBR - MIYE
		// if (streamElList.isEmpty()) {
		//	device.getLogger().info("DMSMediaAgentImpl: handleSingleTimelineUpdate no streams found in sourceTableDoc for xpath=" + streamXPath);
		// }

		if (sync) {
			if (MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction)) {
				syncStopOutput(streamElList, offset);
			} else {

				syncTC = Long.parseLong(timelineUpdate.attributeValue("syncTC"));
				if (syncTC == 0) {
					String syncAgentJID = (timelineUpdate.attributeValue("syncAgentJID"));
					syncAgentJID = (syncAgentJID == null) ? "" : syncAgentJID;

					// send the sync source events
					if (device.getMyJID() != null && syncAgentJID.equals(device.getMyJID())) {
						String syncSourceUUID = timelineUpdate.attributeValue("syncSourceUUID");
						Element inpEl = XMLUtils.getElementXpath(this.sourceTable.getDoc(), "//Input[@mediaUUID='" + syncSourceUUID + "']");
						String dirPath = XMLUtils.getAttributeValue(inpEl, "dirPath");
						syncTC = this.sourceTable.getSyncTC(syncSourceUUID, offset, dirPath);
						if (syncTC > 0) {
							this.sendUpdateSyncAction(timelineUpdate, syncTC);
						} else {
							device.getLogger().info(
									"DMSMediaAgent: failed to find syncTC for mediaUUID="
											+ syncSourceUUID + " offset="
											+ offset);
						}
					}
					return;
				}
			}
		}


		// TBR - MIYE
		// if (mediaSourceElList.isEmpty()) {
		//	device.getLogger().info("DMSMediaAgentImpl: handleSingleTimelineUpdate no mselements found in sourceTableDoc for xpath=" + streamXPath);
		// }

		Hashtable<String, Element> syncSourceList = new Hashtable<String, Element>();
		for (Object ms : mediaSourceElList) {
			Element msElement = (Element) ms;
			String sourceType = msElement.attributeValue("mediaSourceType");

			if (MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction)) {
				timescale = "0";
			}
			
			// If this is a MEDIAFILE, switch action should be changed to updateOffset
			if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(sourceType)) {
				if (MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(sourceAction)
						|| MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(sourceAction)
						|| MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(sourceAction)) {
					sourceAction = MediaRoomServiceConst.SOURCE_UPDATEOFFSET;
					last.addAttribute("sourceAction", sourceAction);
				}
			}

			// List<?> sourceStreamList = XMLUtils.getListOfNodesXpath(XMLUtils.elementToDocument(msElement), "//" + streamXPath);
			List<?> sourceStreamList = XMLUtils.selectNodesXpath(msElement, ".//" + mediasourceStreamXPath);

			if(propogateTimelineUpdateToRelaySource(msElement, String.valueOf(timescale), timelineUpdate, actionNID)) {
				Element instanceElement = msElement.getParent();
		        this.sourceTable.stopSSOuputList(sourceStreamList, instanceElement, offset);
		        this.sourceTable.startSSOuputList(sourceStreamList, instanceElement, offset);
				continue;
			}

			this.updateStreamTimelineForSource(device, actionNID, sourceAction, offset,syncTC,
					timescale, mediaTimeline, sourceStreamList, msElement,
					syncSourceList, device.getMyJID(), sync);
		}

		// start inputs in sync
		if (!syncSourceList.isEmpty()) {
			this.sourceTable.startSSInputListSync(syncSourceList);
		}

		// Forward the timeline update to source,
		// Send timeline update response to SC


		// Send timeline event to destination.
		Iterator<?> i = null;

		if (streamElList == null) {
			return;
		}

		i = streamElList.iterator();

		while (i.hasNext()) {
			Element sElement = ((Element) i.next());
			String streamNID = sElement.attributeValue("NID");
			Document streamDataDoc = this.getStreamData(streamNID);
			String destAgentJID = XMLUtils.getValueXpath(streamDataDoc,
					"//destAgentJID", "");
			long WC = Long.parseLong(last.attributeValue("WC"));

			String lastState = last.attributeValue("lastState");
			String trackAction = last.attributeValue("action");
			String userAction = last.attributeValue("requestedAction");
			long tc = syncTC;
			if (MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction)) {
				timescale = String.valueOf(0);
			}

			MediaTimelineUtils.updateMediaTimelineElement(
					sElement.element("MediaTimeline"), WC, offset, tc,
					Double.valueOf(timescale), trackAction, lastState,
					userAction, sourceAction);

			Document streamStatusDoc = generateStreamStatusDoc(sElement,
					MediaAgentI.STREAM_STARTED);
			Element updateEl = getTimelineUpdateEventData(
					streamStatusDoc.getRootElement(), last, streamNID);

			Message timelineUpdateEvent = Message.createEvent(updateEl, destAgentJID,
					device.getMyJID(), "Layout", "SourceEvent",
					String.valueOf(MediaAgentI.EVENT_LEVEL_OK), Level.INFO);
			this.deviceFrameWork.sendMessage(timelineUpdateEvent);
		}

	}

	private void updateStreamTimelineForSource(	DeviceFrameworkI device, String actionNID,
			String sourceAction, long offset,long syncTC, String timescale,
			Element timelineIDEl, List<?> streamNIDList, Element msElement,
			Hashtable<String, Element> syncSourceList, String userJID, boolean sync) throws Exception {

		// TBR - MIYE
		// device.getLogger().info("DMSMediaAgentImpl:updateStreamTimelineForSource Start");

		String sourceNID = msElement.attributeValue("NID");
		String sourceType = msElement.attributeValue("mediaSourceType");
		Element instanceEl = msElement.getParent();

		if (MediaRoomServiceConst.SOURCE_STOPOUTPUT.equals(sourceAction)) {


			String inputType = instanceEl.element("Input").attributeValue("inputType");

			this.sourceTable.stopSSOutputAt(instanceEl,	msElement.element("Stream"), offset);

			if (MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
				if (msElement.elements().size() == streamNIDList.size()
						&& sourceTable.isSourceSupportFFTracks(instanceEl.element("Input"))) // All streams are stopped.
					// check source supports FFtracks to
					// ensure not stopping the input that was not started in the first place
					// bugfix #47435 (Non FF media will not pause/resume in a mixed session with other FF media playing)
					this.sourceTable.streamingServer.stopSSInput(instanceEl.element("Input"));
			}

		} else if (MediaRoomServiceConst.SOURCE_STARTOUTPUT.equals(sourceAction)) {
            //checkAvailableDiskSpace(msElement.element("Stream"));  ------------- TBD
            this.sourceTable.startSSOuputList(streamNIDList, msElement.getParent(), offset);

		} else if (MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(sourceAction)
				|| MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(sourceAction)
				|| MediaRoomServiceConst.SOURCE_UPDATEOFFSET.equals(sourceAction)) {


			this.sourceTable.stopSSOuputList(streamNIDList, instanceEl, offset);

			String timelineID = timelineIDEl.attributeValue("id");
			String streamNID = msElement.element("Stream").attributeValue("NID");
			Document streamDataDoc = this.getStreamData(streamNID);
			Document mediaSourceDataDoc = this.getMediaSourceData(streamNID);


			String newSourceNID = sourceNID;
	        String newSourceType = sourceType;

	        if (MediaRoomServiceConst.SOURCE_SWITCHCURRENT.equals(sourceAction)) {
	            // Use the original source
	            newSourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceNID","");
	            newSourceType = XMLUtils.getValueXpath(mediaSourceDataDoc, "//sourceType","");
	        } else if ((MediaRoomServiceConst.SOURCE_SWITCHLOOKBACK.equals(sourceAction) ||
        			(MediaRoomServiceConst.SOURCE_UPDATEOFFSET.equals(sourceAction)))) {
	        	// Use Lookback Media source
	        	newSourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceNID","");
	            if("".equals(newSourceNID)) {
	            	throw new Exception(XMLUtils.generateErrorXML("updateAction", MediaRoomServiceExceptionCodes.PVR_NOTAVAILABLE +":"+ sourceAction));
	            }
	            String newSourceAgentJID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//lookbackSourceAgentJID","");
	            newSourceType = MediaRoomServiceConst.INPUT_MEDIAFILE;
	            streamDataDoc = (Document) streamDataDoc.clone();

	            // Make sure we are not relaying to ourselves
	            if (userJID.equals(newSourceAgentJID)) {
	                XMLUtils.setValueXpath(streamDataDoc, "//relayNID", "");
	            }

	        }

	        // switch the input
	        Element newInstanceEl = switchInput(msElement, newSourceNID, newSourceType,
					streamDataDoc, offset, syncTC, Double.valueOf(timescale),
					mediaSourceDataDoc, timelineID, streamNIDList);


			// start the streaming server output and input instances.
			this.sourceTable.startSSOuputList(streamNIDList, newInstanceEl, offset);

			Element inputEL = newInstanceEl.element("Input");
			String inputType = XMLUtils.getAttributeValue(inputEL, "inputType");
			String instanceNID = XMLUtils.getAttributeValue(newInstanceEl, "NID");

			// Support sync start only for media file input sources
			if (sync && MediaRoomServiceConst.INPUT_MEDIAFILE.equals(inputType)) {
				// check source supports FFtracks to
				// ensure not stopping the input that was not started in the first place
				// bugfix #47435 (Non FF media will not pause/resume in a mixed session with other FF media playing)
				if (!syncSourceList.containsValue(instanceNID)
						&& this.sourceTable.isSourceSupportFFTracks(inputEL))
					syncSourceList.put(instanceNID, newInstanceEl);
			} else {
				if (this.sourceTable.isSourceSupportFFTracks(inputEL)) {
					startSSInputList(streamNIDList, newInstanceEl);
				}

			}

		}else {
            throw new Exception(XMLUtils.generateErrorXML("updateAction", MediaRoomServiceExceptionCodes.ACTIONNOTFOUND +":"+sourceAction));
        }
		// TBR - MIYE
		// device.getLogger().info("DMSMediaAgentImpl:updateStreamTimelineForSource End");
	}

	public Element switchInput(Element msElement, String newSourceNID,
			String newSourceType, Document streamDataDoc, long offset,
			long syncTC, double timescale, Document mediaSourceDataDoc,
			String timelineID, List<?> streamList) throws Exception {

		XMPPTransaction.startBlock("ReleaseMediaSourceElement");
		if (msElement.elements().size() == streamList.size()) {
			this.releaseMediaSourceElement(msElement);
		} else {
			Element newMSElement = this.cloneMediaSourceElement(msElement);
			for (Object o : streamList) {
				Element streamElement = (Element) o;
				msElement.remove(streamElement);
				newMSElement.add(streamElement.detach());
			}
			msElement = newMSElement;
		}
		XMPPTransaction.endBlock("ReleaseMediaSourceElement");

		// Get the new instanceElement for the mediaSourceNID
		String mediaSourceNID = msElement.attributeValue("NID");
		String userRoomMediaSourceNID = XMLUtils.getValueXpath(mediaSourceDataDoc, "//userRoomMediaSourceNID", "");
		if ("".equals(userRoomMediaSourceNID))
			userRoomMediaSourceNID = mediaSourceNID;

		XMPPTransaction.startBlock("GetInstanceElement");

		String sourcePortElementID = XMLUtils.getValueXpath(mediaSourceDataDoc,	"//sourcePortElementID", "");
		String instanceType = this.getInstanceType(newSourceType);
		Element newInstanceElement = this.findOrCreateSourceTableInstance(
				newSourceNID, sourcePortElementID, newSourceType, instanceType,
				userRoomMediaSourceNID, streamDataDoc, offset, syncTC,
				timescale, mediaSourceDataDoc, timelineID);

		XMPPTransaction.endBlock("GetInstanceElement");

		// Add the mediaSourceElement to the instanceElement
		newInstanceElement.add(msElement);
		this.portTable.updatePortInstance(msElement, newInstanceElement);

		return newInstanceElement;

	}

    // Start the Streaming Server Input Instance
	public void startSSInputList(List<?> streamList, Element instanceElement)
			throws Exception {
		Iterator<?> streamListIterator = streamList.iterator();

		while (streamListIterator.hasNext()) {
			Element streamElement = (Element) streamListIterator.next();
			this.streamingServer.startSSInput(instanceElement, streamElement);
		}
	}


	@Override
	public void handleMediaSourceUpdate(DeviceFrameworkI deviceFramework,
			Document doc) throws Exception {
		String streamNID = XMLUtils.getValueXpath(doc, "//streamNID", "");
        Document mediaSourceDataDoc = MediaUtils.parseDataDoc(XMLUtils.getElementXpath(doc, "//mediasourcedatadoc"));
        this.updateMediaSourceData(streamNID, mediaSourceDataDoc);
	}


	public void syncStopOutput(List<?> streamList, long offset)
			throws Exception {
		ArrayList<Element> syncStreamList = new ArrayList<Element>();
		boolean isPlayback = false;
		// check do we have any playback is going on device
		for (Iterator<?> i = streamList.iterator(); i.hasNext();) {
			Element streamElement = (Element) i.next();
			Element instanceElement = streamElement.getParent().getParent();
			Element inputElement = instanceElement.element("Input");
			String inputType = XMLUtils.getAttributeValue(inputElement, "inputType");
			if (inputType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)) {
				isPlayback = true;
				break;
			}
		}
		for (Iterator<?> i = streamList.iterator(); i.hasNext();) {
			Element streamElement = (Element) i.next();
			Element instanceElement = streamElement.getParent().getParent();
			Element msElement = streamElement.getParent();
			String inputSyncSourceType = XMLUtils.getAttributeValue(msElement, "mediaSourceType");
			Element inputElement = instanceElement.element("Input");
			long inputSyncTC = Long.parseLong(XMLUtils.getAttributeValue(inputElement, "syncTC"));
			// Check if file and not sync started then we cannot sync stop it
			if ((inputSyncTC == 0)	&& inputSyncSourceType.equals(MediaRoomServiceConst.INPUT_MEDIAFILE)) {
				// These files will eventually get stopped serially, without sync.
				continue;
			}
			String inputType = XMLUtils.getAttributeValue(inputElement, "inputType");

			// if playing back a file from downstream don't add those streams to sync stream list
			if (isPlayback	&& inputType.equals(MediaRoomServiceConst.INPUT_RELAY)) {
				continue;
			}

			syncStreamList.add(streamElement);
		}
		if (!syncStreamList.isEmpty())
			this.sourceTable.stopSSOutputListSync(syncStreamList, offset);

	}


	@Override
	public void handleStopped(Document eventDoc) throws Exception {

		Element ssEvent  = XMLUtils.getElementXpath(eventDoc, "//SSEvent");
		String fileUUID = XMLUtils.getAttributeValue(ssEvent, "fileUUID");

		if(this.sourceTable.getGCStream(fileUUID)==null) {
			return;
		}

		Element outputElement = this.sourceTable.removeStreamFromGCList(fileUUID);
		sourceTable.deleteOutput(outputElement, null);
		deviceFrameWork.getLogger().info("DMSMediaAgent: Got Stopped Event. Delete  output ssInstanceID:="+fileUUID);
	}

	public Element getTimelineUpdateEventData(Element streamStatusEl, Element lastEl, String streamNID) throws Exception {

		int eventCode = MediaAgentI.TIMELINEUPDATEEVENT;

		String data = "<SourceEventData>" +
				"<eventCode>" + String.valueOf(eventCode) + "</eventCode>" +
				"<streamNID>" + streamNID + "</streamNID>" +
				"<status>" + String.valueOf(MediaAgentI.EVENT_LEVEL_OK) + "</status>" +
				"<sourceEventActionData>" + streamStatusEl.asXML() + lastEl.asXML() + "</sourceEventActionData>" +
				"</SourceEventData>";
		Element dataEl = XMLUtils.stringToElement(data);

		return dataEl;
	}


	protected void deleteOutput(Element instanceElement, Element streamElement, Element inputElement, Element outputElement,String streamNID, Document streamDataDoc)  throws Exception{

		String outputType = outputElement.attributeValue("outputType");
		//if it is recording postpone delete output till we receive stopped event from streaming server
		if(outputType.equals(MediaRoomServiceConst.OUTPUT_RECORD)) {
			String destFileUUID = outputElement.attributeValue("destFileUUID");
			sourceTable.addStreamToGCList(destFileUUID, outputElement);
			long offset = Utils.getLongValue(XMLUtils.getAttributeValue(inputElement,"offset"), 0);
			try {
				sourceTable.stopSSOutputAt(instanceElement, streamElement, offset);
			} catch (Exception e) {
				e.printStackTrace();
			}
			deviceFrameWork.getLogger().info("DMSMediaAgent: Stop output. wait for streaming stopped event to delete the output. destFileUUID:="+destFileUUID);
			return;
		}


		this.sourceTable.stopAndDeleteOutput(instanceElement, streamElement, inputElement, outputElement, streamNID, streamDataDoc);
	}


	@Override
	public void releaseUDPStartPort(String destUDPStartPort, String streamType)
			throws Exception {
		this.streamingServer.getNetworkResource().releaseUDPStartPort(destUDPStartPort, streamType);
	}


	@Override
	public String updateNextUDPStartPort(String streamType) throws Exception {
		return this.streamingServer.getNetworkResource().getNextUDPStartPort(streamType);
	}


	@Override
	public void handleConnected(Document eventDoc) throws Exception {
		Element ssEvent  = XMLUtils.getElementXpath(eventDoc, "//SSEvent");
		String ssInstanceId = XMLUtils.getAttributeValue(ssEvent, "id");
		deviceFrameWork.getLogger().debug("DMSMediaAgentImpl: start streaming server instanc for RTP PStream. ssInstanceId:="+ssInstanceId);
		System.out.println(sourceTable.sourceTableDoc.asXML());
		Element outputElement= XMLUtils.getElementXpath(sourceTable.sourceTableDoc, "//Output[@ssInstanceID='"+ssInstanceId+"']");
		if(outputElement == null) {
			deviceFrameWork.getLogger().error("DMSMediaAgentImpl: no stream found for instance. ssInstanceID:="+ssInstanceId);
			return;
		}
		
		Element instanceElement  = outputElement.getParent().getParent().getParent();
		Element streamElement = outputElement.getParent();

		sourceTable.startSSOutput(instanceElement, streamElement, 0);	
		this.streamingServer.startSSInput(instanceElement, streamElement);
		
		XMLUtils.addAttribute(outputElement, "streamConnected", "true");
		deviceFrameWork.getLogger().info("DMSMediaAgentImpl: started streaming server instanc for RTP PStream. streamNID:="+ssInstanceId);

	}
	
}
