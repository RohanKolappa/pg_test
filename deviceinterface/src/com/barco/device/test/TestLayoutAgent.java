package com.barco.device.test;


import org.dom4j.Document;

import com.barco.device.agent.LayoutAgentI;
import com.barco.device.common.DeviceFrameworkI;

public class TestLayoutAgent implements LayoutAgentI {

	DeviceFrameworkI deviceFramework;
	Document displayConfig;
	
	@Override
	public void initAgent(DeviceFrameworkI deviceFramework) {
		this.deviceFramework = deviceFramework;
	}

	@Override
	public void updateDisplayConfig(Document displayConfig) {
		this.displayConfig = displayConfig;
	}

	@Override
	public Document getDisplayConfig() {
		return this.displayConfig;
	}

	@Override
	public Document getPublishInfo() throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void startPlaying(String streamNID, String destPortElementID, String URL) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void stopPlaying(String streamNID) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public Document getStatus(String xpath) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}
	
	/**
	 * handle device config updates 
	 * @param doc
	 * @throws Exception
	 */
	public void handleConfigUpdated(Document doc) throws Exception
	{
		
	}

	@Override
	public void reset() {
		// TODO Auto-generated method stub
		
	}


}
