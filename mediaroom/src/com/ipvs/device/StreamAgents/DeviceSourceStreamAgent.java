package com.ipvs.device.StreamAgents;

import org.dom4j.Document;

import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.device.DeviceAppI;
import com.ipvs.device.impl.DeviceAgentUtils;
import com.ipvs.device.impl.DeviceStreamingServerImpl;
import com.ipvs.mediaroomservice.impl.SourceStreamAgent;


public class DeviceSourceStreamAgent extends SourceStreamAgent {

	Document deviceXML = null;
	DeviceAgentUtils deviceAgentUtils = null;
	DeviceAppI deviceApp;

	public DeviceSourceStreamAgent(String localConfigFile, DeviceAppI deviceApp) {
		this.deviceApp = deviceApp;
		try {
			this.deviceXML = MRXMLUtils.loadXMLFile(localConfigFile, log);
			deviceAgentUtils = DeviceAgentUtils.getInstance();
			deviceAgentUtils.setDeviceXML(deviceXML);
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
	
	
	@Override
	public void handleConfigUpdates(String localConfigFile, String updateParentXPath,  String updateXPath, String userJID, ServiceMessageHandlerI messageHandler)
    throws MRException, Exception {
		this.deviceXML = MRXMLUtils.loadXMLFile(localConfigFile, log);
		this.streamingServer.getNetworkResource().updateNetworkResource(deviceXML);
		this.streamingServer.setupInternalInstances(this.deviceXML);
		this.deviceAgentUtils.setDeviceXML(this.deviceXML);
	}
	
	@Override
	public void init(XMPPF xf, MRLog log, MRDB db, MRResourceFactoryI resourceFactory, String userJID,Document setServiceDomainResponseDoc) throws Exception {
    	// System.out.println("DeviceSourceStreamAgent::init");
		DeviceStreamingServerImpl deviceStreamingServer = new DeviceStreamingServerImpl(userJID, log, db, xf, this.deviceApp);
		String myDeviceType = deviceXML.getRootElement().getName();
		String myDeviceNID = null;
		try {
			deviceStreamingServer.getNetworkResource().updateNetworkResource(deviceXML);
			myDeviceNID = MRXMLUtils.getValueXpath(setServiceDomainResponseDoc, "//deviceNID", "");
		} catch (Exception e) {
			e.printStackTrace();
		}
		this.initInternal(xf, log, db, resourceFactory, userJID, myDeviceNID, myDeviceType, deviceStreamingServer);
	}
}
