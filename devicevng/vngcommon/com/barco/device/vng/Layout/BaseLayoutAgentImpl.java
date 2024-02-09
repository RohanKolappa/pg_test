package com.barco.device.vng.Layout;

import java.util.Enumeration;
import java.util.Hashtable;

import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.agent.LayoutAgentI;
import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.StreamEntry;
import com.barco.device.handler.DeviceAdmin.DeviceAdminConst;
import com.barco.utils.XMLUtils;

public abstract class BaseLayoutAgentImpl implements LayoutAgentI {

	DeviceFrameworkI deviceFramework;
	Document displayConfig;
	
	public abstract void updateDisplay(Document displayConfig);
	
	@Override
	public Document getDisplayConfig() {
		// TODO Auto-generated method stub
		return displayConfig;
	}

	@Override
	public void updateDisplayConfig(Document displayConfig) {
		this.displayConfig = displayConfig;
		this.updateDisplay(displayConfig);
	}

	@Override
	public void initAgent(DeviceFrameworkI deviceFramework) {
		this.deviceFramework = deviceFramework;
	}

	@Override
	public Document getPublishInfo() throws Exception {		
		
		Hashtable<String, StreamEntry> streamList = this.deviceFramework.getDeviceDB().getStreamCache().getStreamList();
		Enumeration<String> keys = streamList.keys();
		Element streamInfoList = XMLUtils.stringToElement("<StreamInfoList />");
		while(keys.hasMoreElements()) {		
			String key = keys.nextElement();
			Element streamInfoDoc = XMLUtils.stringToElement("<StreamInfo />");
			XMLUtils.addAttribute(streamInfoDoc, "streamNID", key);
			StreamEntry entry = streamList.get(key);
			Document mediaSourceDataDoc = entry.mediasourceDataDoc;
			Document streamDataDoc = entry.streamDataDoc;
			Element streamDataDocInInfo = XMLUtils.stringToElement("<StreamDataDoc />");
			Element mediaSourceDataDocInInfo = XMLUtils.stringToElement("<MediaSourceDataDoc />");
			Element status = (Element)entry.status.clone();
			//Element oldStatus = XMLUtils.getElementXpath(streamDataDoc, "//status");
			//XMLUtils.replaceContent(oldStatus, status);
			
			streamDataDocInInfo.add((Element)streamDataDoc.getRootElement().clone());
			streamDataDocInInfo.add(status);
			mediaSourceDataDocInInfo.add((Element)mediaSourceDataDoc.getRootElement().clone());
			streamInfoDoc.add(streamDataDocInInfo);
			streamInfoDoc.add(mediaSourceDataDocInInfo);
			streamInfoList.add(streamInfoDoc);			
		}
		Document layOutServiceInfoDoc = XMLUtils.stringToDocument("<"+DeviceAdminConst.LAYOUT_SERVICE_INFO+" />");
		layOutServiceInfoDoc.getRootElement().add(streamInfoList);		
		return layOutServiceInfoDoc;
		
	}

	@Override
	public void startPlaying(String streamNID, String destPortElementID, String URL) {
		this.deviceFramework.getLogger().info("BaseLayoutAgentImpl:startPlaying streamNID=" + streamNID + " destPortElementID=" + destPortElementID + " URL=" + URL);
	}

	@Override
	public void stopPlaying(String streamNID) {
		this.deviceFramework.getLogger().info("BaseLayoutAgentImpl:stopPlaying streamNI" +
				"D=" + streamNID);
	}
	@Override
	public Document getStatus(String xpath) throws Exception {
		// TODO Auto-generated method stub			
		
		return null; //this.deviceFramework.getDeviceDB().getDeviceStatus();
	}
	
	
	/**
	 * handle device config updates 
	 * @param doc
	 * @throws Exception
	 */
	public void handleConfigUpdated(Document doc) throws Exception{
		//TODO: Handle layout updates
	}
	
	@Override
	public void reset() {
		// TODO Auto-generated method stub
		
	}	
}
