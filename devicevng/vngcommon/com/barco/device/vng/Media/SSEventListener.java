package com.barco.device.vng.Media;

import java.io.InputStream;
import java.io.OutputStream;
import java.util.List;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.device.handler.MediaStore.MediaStoreServiceConst;
import com.barco.device.vng.utils.XMLStreamReaderThread;
import com.barco.utils.Utils;
import com.barco.utils.XMLUtils;

/**
 * Forwards events from the streaming-server (source), as SourceEvent or
 * SourceInfo events, over the bus, to the jid of the destination of the
 * associated stream. To identify this jid and the stream-NID, it reads from the
 * SourceTable.
 * <p>
 * This thread reads xml (one element at a time) from an InputStream, converts
 * it (convertSSEventToMessage) into a SourceEvent or SourceInfo event, and
 * sends it on the bus.
 * <p>
 * The input xml stream contains SS-events. The SS-event contains an
 * ssInstanceID that identifies a stream output in the SourceTable. The
 * associated stream has a jid, towards whom this event will be sent.
 */
class SSEventListener extends XMLStreamReaderThread {
	

	DeviceFrameworkI device;
	SourceTable sourceTable;
	
	public SSEventListener(DeviceFrameworkI device, InputStream input, OutputStream output, SourceTable sourceTable) {
		super(device.getLogger(), input, output);
		this.device = device;
		this.sourceTable = sourceTable;
	}
	
	public void setDevice(DeviceFrameworkI device) {
		this.device = device;
	}


	public void setSourceTable(SourceTable sourceTable) {
		this.sourceTable = sourceTable;
	}

	
	String getSourceEventDoc(int eventCode,String streamNID,int eventLevel,long startTC,String timelineStr) {
		
		return "<SourceEventData>" +
				"<eventCode>" + String.valueOf(eventCode) + "</eventCode>" +
				"<streamNID>" + streamNID + "</streamNID>" +
				"<status>" + String.valueOf(eventLevel) + "</status>" +
				"<startTC>" + String.valueOf(startTC) + "</startTC>" +  
				"<streamNIDList/>" + 
				"<sourceEventActionData>" + timelineStr + "</sourceEventActionData>" +
					"</SourceEventData>";
	}

	String getStartedData(Document doc, String streamNID, int eventLevel) throws Exception {
		int eventCode = 111;
		// long WC = XMLUtils.getLongValue(doc, "//Header/WallClock", 0);
		long startTC = XMLUtils.getLongValue(doc, "//Data/StartTimeStamp", 0);
		long startWC= XMLUtils.getLongValue(doc, "//Data/StartWallClock", 0);
		String timelineStr = 
			"<Last Offset=\"0\" TC=\"" + String.valueOf(startTC) + "\" TimeScale=\"1.0\" WC=\"" + String.valueOf(startWC) +"\"" +
					" action=\"Start\" lastState=\"STARTED_CURRENT\" requestedAction=\"Start\" sourceAction=\"Start\"" +
					" ssTC=\"" + String.valueOf(startTC) + "\" ssWC=\"" + String.valueOf(startWC) + "\"/>";
		return getSourceEventDoc(eventCode, streamNID, eventLevel, startTC, timelineStr);
	}
	
	// TBD Map the SSEvent to Data
	String getInfoData(Document doc, String streamNID, int eventLevel) throws Exception {
		return null;
	}
	
	private Message convertSSEventToMessage(String xml) throws Exception {
		Document doc = DocumentHelper.parseText(xml);
	
		int ssStatus = XMLUtils.getIntValue(doc, "//Header/Status", 2);
		String eventType = XMLUtils.getValueXpath(doc, "//EventType", "");
		String id = XMLUtils.getValueXpath(doc, "//Header/Id", "");	
		String type = XMLUtils.getValueXpath(doc, "//Type", "");
		Element outputElement = XMLUtils.getElementXpath(this.sourceTable.sourceTableDoc, 
				"//Output[@ssInstanceID='" + id + "']");
		
		if(eventType.equals("Progress") && type.equals("DMSRecorderPort")) {
			//System.out.println("EventDoc:="+doc.asXML());			
			handleRecordingProgressNotification(outputElement,id, doc);
			return null;
		}
		
		if(eventType.equals("Progress") && type.equals("DeleterPort")) {
			handleDeletorPortProgressNotification(doc);			
			return null;
		}
		
		if(type.equals("DMSMediaPort")) {
			handleDMSMediaEvent(doc,id,eventType);			
			return null;
		}
		
		//System.out.println("EventDoc:="+doc.asXML());
		
		if(outputElement == null) {			
			return null;
		}
	
		int eventLevel = 0; // Error status
		if(ssStatus == 2 || ssStatus == 3)
			eventLevel = MediaAgentI.EVENT_LEVEL_OK;

		Element streamElement = outputElement.getParent();
		String destJID = streamElement.attributeValue("destAgentJID");
		String streamNID = streamElement.attributeValue("NID");

		String service = "Layout";
		String data = null;
		String eventName = "";
		if("Started".equals(eventType) || "Connected Started".equals(eventType)) {
			eventName = "SourceEvent";
			data = this.getStartedData(doc, streamNID, eventLevel);
		} else {
			eventName = "SourceInfo";
			data = this.getInfoData(doc, streamNID, eventLevel);
		}
		
		if(data == null)
			return null;
		
		Document eventDoc = DocumentHelper.parseText(data);
		Message message = Message.createEvent(eventDoc.getRootElement(), 
				destJID, this.device.getMyJID(),
				service, eventName, String.valueOf(eventLevel), Level.INFO);

		return message;
	}
	
	private void handleDMSMediaEvent(Document doc,String ssInstanceID,String eventType) {
		
		try {
			if(!eventType.equals("EndOfMedia")) {
				return;
			}
			
			Element inputElement = XMLUtils.getElementXpath(sourceTable.sourceTableDoc,"//Input[@ssInstanceID='" + ssInstanceID + "']");
			if (inputElement == null) {
			    return;
			}
			
			Element instanceElement = inputElement.getParent();   
			if (instanceElement == null) {
				return;
			}
			        
			List<?> streamList = XMLUtils.selectNodesXpath(instanceElement, "./MediaSource/Stream");
			if ((streamList == null) || (streamList.size() == 0)) {
			    return;
			}        		
		

			for(Object object:streamList) {
				Element streamElement = (Element)object;
				String data = 	getSourceEventDoc(MediaAgentI.END_OF_MEDIA_REACHED, XMLUtils.getAttributeValueWithDefault(streamElement,MediaStoreServiceConst.NID , ""), 
						MediaAgentI.EVENT_LEVEL_ERROR, 0, "<Exception><Code>"+MediaAgentI.END_OF_MEDIA_REACHED+"</Code><Description>End of Media</Description></Exception>");
				String destJID = streamElement.attributeValue("destAgentJID");
				
				Document eventDoc = DocumentHelper.parseText(data);
				Message message = Message.createEvent(eventDoc.getRootElement(), 
						destJID, this.device.getMyJID(),
						DeviceAdminConst.LAYOUT_SERVICE, "SourceEvent", String.valueOf(MediaAgentI.EVENT_LEVEL_ERROR), Level.INFO);
				device.sendMessage(message);				
			}			
		
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	private void handleDeletorPortProgressNotification(Document doc) throws Exception{
	    String sizeDeltaDeleted = XMLUtils.getValueXpath(doc, "//DeletedBytes", "");
		String dirPath = XMLUtils.getValueXpath(doc, "//MediaDirectory", "");
		if(dirPath.contains("purge")) {
			dirPath=dirPath.replace("/purge", "");
			String id = XMLUtils.getValueXpath(doc, "//Header/Id", "");
			sendPurgeFilesEvent(sizeDeltaDeleted, dirPath, id);
		} else {
			dirPath=dirPath.replace("/trash", "");
			
			//if this notification is for temporary dir change the path to /data1 to update the status.
			if(dirPath.equals("/data/mediadir")){
				String dataDirPath = XMLUtils.getValueXpath(device.getDeviceDB().getDeviceConfig(), "//HardDiskEntry[MountPoint='/data1']/MediaStore/RootDirPath", "");
			    dirPath = dataDirPath  +"/"  +MediaStoreServiceConst.DEFAULT_MEDIA_STORE_NAME + "/";
			}
			
			sendDiskSpaceUpdateEvent("0",sizeDeltaDeleted, dirPath, true);
		}
		
	}
	
	private void handleRecordingProgressNotification(Element outputElement,String id, Document doc) throws Exception {
		String status  = XMLUtils.getValueXpath(doc, "//Data/MediaStatus", "");
		if("Stopped".equals(status)){			
			String fileUUID = XMLUtils.getValueXpath(doc, "//MediaName", "");
	    	String dirPath = XMLUtils.getValueXpath(doc, "//MediaDirectory", "");
	    	String size = XMLUtils.getValueXpath(doc, "//BytesWritten","");
	    	String playLength = XMLUtils.getValueXpath(doc, "//PlayLength","");
	    	
			String ssEventXML = "<SSEvent state='Stopped' fileUUID='"+fileUUID+"'"
					+ " ssInstanceID='"+id+"' dirPath='"+dirPath+"' size='"+size+"' playLength='"+playLength+"'/>";
			
		    Message eventMessage = Message.createEvent(XMLUtils.stringToElement(ssEventXML),device.getMyJID() , device.getMyJID(),
					DeviceAdminConst.LAYOUT_SERVICE, "SSEvent", String.valueOf(Level.INFO), Level.INFO);
			device.handleMessage(eventMessage);
			
			return;
		} else if("Started".equals(status)) {
			String sizeDeltaAdded = XMLUtils.getValueXpath(doc, "//BytesWrittenDelta", "");
			String sizeDeltaDeleted = XMLUtils.getValueXpath(doc, "//BytesDeletedDelta", "");
			String dirPath = XMLUtils.getValueXpath(doc, "//MediaDirectory", "");
			sendDiskSpaceUpdateEvent(sizeDeltaAdded,sizeDeltaDeleted, dirPath, false);
		}
		
		return;		
	}
	
	
	private void sendPurgeFilesEvent(String sizeDeltaDeleted,String dirPath, String id) throws Exception {
        
		if(Utils.getLongValue(sizeDeltaDeleted,0)==0 && !dirPath.contains("purge")) {
			return;
		}
			
	 	
    	String mediaStoreEventXML = "<MediaStoreEvent type='PurgeFileProgressEvent'  dirPath='"+dirPath+"' id='"+id+"'></MediaStoreEvent>";
    	
    	Message eventMessage = Message.createEvent(XMLUtils.stringToElement(mediaStoreEventXML),device.getMyJID() , device.getMyJID(),
				DeviceAdminConst.MEDIA_STORE_SERVICE, "MediaStoreEvent", String.valueOf(Level.INFO), Level.INFO);
    	
		device.handleMessage(eventMessage);

	}
	
	
	private void sendDiskSpaceUpdateEvent(String sizeDeltaAdded,String sizeDeltaDeleted,String dirPath, boolean publishSizeUpdate) throws Exception {
        
		if(Utils.getLongValue(sizeDeltaAdded, 0) ==0 && Utils.getLongValue(sizeDeltaDeleted,0)==0) {
			return;
		}
			
	 	
    	String mediaStoreEventXML = "<MediaStoreEvent type='DiskSpaceUpdateEvent'  dirPath='"+dirPath+"' sizeDeltaAdded='"+sizeDeltaAdded+"' sizeDeltaDeleted='"+sizeDeltaDeleted+"' publishUpdate='"+publishSizeUpdate+"'></MediaStoreEvent>";
    	
    	Message eventMessage = Message.createEvent(XMLUtils.stringToElement(mediaStoreEventXML),device.getMyJID() , device.getMyJID(),
				DeviceAdminConst.MEDIA_STORE_SERVICE, "MediaStoreEvent", String.valueOf(Level.INFO), Level.INFO);
    	
		device.handleMessage(eventMessage);

	}

	public void handleXML(String xml) {
		try {
			Message message = convertSSEventToMessage(xml);
			if(message != null)
				this.device.sendMessage(message);
		}
		catch(Exception e) {
        	this.device.getLogger().info("BaseMediaAgentImpl:handleSSEvent error=" + e.toString()); 
			e.printStackTrace();
        	this.device.getLogger().info("BaseMediaAgentImpl:handleSSEvent xml=" + xml);        	
		}
/*
String  errorDetails = MRXMLUtils.getValueXpath(actionDoc, "//ErrorDetail", "");
<SSNotification>

  <Header>
    <EventType>Started</EventType>
    <Id>2002</Id>
    <Type>V2DDecoderPort</Type>
    <Status>2</Status>
    <WallClock>1363560236103</WallClock>
  </Header>

  <Data>
    <QueueId>2001</QueueId>
    <Status>Started</Status>
    <StartTimeStamp>1363560236102</StartTimeStamp>
    <StartWallClock>1363560236103</StartWallClock>
  </Data>

</SSNotification>

<Event>
	<Header eventAgentJID="tx1@localhost/tx1" eventLevel="200"
		eventName="SourceEvent" eventWallclock="1363541600468" serviceName="Layout" />
	<Data>
		<SourceEventData>
			<eventCode>111</eventCode>
			<action>SourceEvent</action>
			<streamNID>d6034a32-1442-47ac-8ef1-b0e150d38502</streamNID>
			<startTC>1363538973745</startTC>

			<streamNIDList />
			<status>200</status>
			<sourceEventActionData>
				<Last Offset="1277" TC="1363541600379" TimeScale="1.0"
					WC="1363541600379" action="Start" lastState="STARTED_CURRENT"
					requestedAction="Start" sourceAction="Start" ssTC="1363538973745"
					ssWC="1363538973746" />
			</sourceEventActionData>
		</SourceEventData>
	</Data>
	<Log />
</Event>
*/
	}
}
