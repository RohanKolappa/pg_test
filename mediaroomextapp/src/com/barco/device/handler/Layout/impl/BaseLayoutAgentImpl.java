package com.barco.device.handler.Layout.impl;

import org.dom4j.Document;

import com.barco.device.base.DeviceI;
import com.barco.device.base.LayoutAgentI;
import com.barco.device.handler.Media.impl.StreamingServerI;
import com.barco.utils.mr.MRLog;
import com.barco.utils.mr.XMPPI;

public abstract class BaseLayoutAgentImpl implements LayoutAgentI {

	DeviceI device;
	String userJID;
    MRLog log;
    XMPPI xmpp;
    String deviceNID;
	Document displayConfig;
	
	public abstract void updateDisplay(Document displayConfig);
	
	public BaseLayoutAgentImpl(String userJID, String deviceNID, String deviceType,
			StreamingServerI streamingServer, MRLog log, XMPPI xmpp,
			Document deviceDoc) {
			this.userJID = userJID;
			this.log = log;
			this.xmpp = xmpp; 
	        this.deviceNID = deviceNID;
			this.displayConfig = null;
	}

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

	public void setDevice(DeviceI device) {
		this.device = device;
	}

}
