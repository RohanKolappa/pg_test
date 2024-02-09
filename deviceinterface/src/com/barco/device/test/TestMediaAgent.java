package com.barco.device.test;

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.MediaAgentI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.utils.MediaUtils;
import com.barco.utils.XMLUtils;

public class TestMediaAgent implements MediaAgentI{
	
	DeviceFrameworkI deviceFramework;
	Document doc;
	ArrayList<String> roomNIDList = new ArrayList<String>();

	@Override
	public void initAgent(DeviceFrameworkI deviceFramework) {
		this.deviceFramework = deviceFramework;
		try {
			doc = XMLUtils.stringToDocument("<StreamList/>");
		}
		catch(Exception e) {
			this.deviceFramework.getLogger().info("TestMediaAgent:initAgent error=" + e.toString());
		}
	}

	@Override
	public Document startStream(DeviceFrameworkI device, String requestNID,
			String streamNID, Document streamDataDoc,
			Document mediaSourceDataDoc) throws Exception {
		Element streamData = (Element)streamDataDoc.getRootElement().clone();
		streamData.addAttribute("streamNID", streamNID);
		doc.getRootElement().add(streamData);
		Document streamStatusDoc = MediaUtils.createStreamStatusDoc();
		XMLUtils.setValueXpath(streamStatusDoc, "//state", String.valueOf(MediaAgentI.WAITING_FOR_STREAMSTARTED)); 
		XMLUtils.setValueXpath(streamStatusDoc, "//URL", 
				"v2d://10.51.50.1/port=6060?bandwidth=10240000?avoption=Both?ismulticast=false?enablekbm=false?authscheme=TOKEN?authdata=fc654255-b350-432e-a95a-8a9c0d89c7e0");
		return streamStatusDoc;
	}

	@Override
	public Document stopStream(String streamNID) throws Exception {
		Element streamData = XMLUtils.getElementXpath(doc, "//streamdatadoc[@streamNID='" + streamNID + "");
		if(streamData != null)
			doc.remove(streamData);
		// TODO Auto-generated method stub
		Document streamStatusDoc = MediaUtils.createStreamStatusDoc();
		XMLUtils.setValueXpath(streamStatusDoc, "//state", String.valueOf(MediaAgentI.STREAM_STOPPED)); 
		return streamStatusDoc;
	}

	@Override
	public Document getPublishInfo() throws Exception {
		return doc;
	}
	
	public ArrayList<String> getStreamNIDListByRoomNID(String roomNID) {
		return this.getStreamNIDList("//streamdatadoc[roomNID='" + roomNID + "']");
	}
	
	public ArrayList<String> getStreamNIDListByDestAgentJID(String destUserJID) {
		return this.getStreamNIDList("//streamdatadoc[destUserJID='" + destUserJID + "']");		
	}
	
	public void addRoomNID(String roomNID) {
		this.roomNIDList.add(roomNID);
	}
	
	public void deleteRoomNID(String roomNID) {
		this.roomNIDList.remove(roomNID);
	}
	
	public ArrayList<String> getRoomNIDList() {
		return this.roomNIDList;
	}


	private ArrayList<String> getStreamNIDList(String xpath) {
		ArrayList<String> streamNIDList = new ArrayList<String>();
		try {
			List<?> list = XMLUtils.getListOfNodesXpath(doc, xpath);
			for(Object o : list) {
				Element e = (Element)o;
				String streamNID = e.attributeValue("streamNID");
				streamNIDList.add(streamNID);
			}
		}
		catch(Exception e) {
			this.deviceFramework.getLogger().info("TestMediaAgent:getStreamNIDList error=" + e.toString());
		}
		return streamNIDList;
	}

	@Override
	public void startPlaying(DeviceFrameworkI deviceFramework,
			String streamNID, String destPortElementID, String streamURL) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void stopPlaying(String streamNID) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public Document getStatus(String xpath) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void startPostingThumbnail(String sourceNID, String portId, String publishURL) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void stopPostingThumbnail(String sourceNID, String portId) throws Exception {
		// TODO Auto-generated method stub		
	}
	
	/**
	 * handle device config updates 
	 * @param doc
	 * @throws Exception
	 */
	public void handleConfigUpdated(Document doc) throws Exception{
		
	}

	@Override
	public List<String> getStreamNIDListByPortID(String portID) throws Exception {
		return null;
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
	public void handleStopped(Document eventDoc)
			throws Exception {
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

	@Override
	public void reset() {
		// TODO Auto-generated method stub
		
	}

}
