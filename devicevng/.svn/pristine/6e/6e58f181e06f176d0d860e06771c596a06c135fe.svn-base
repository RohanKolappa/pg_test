package com.barco.device.vng.test;

import com.barco.device.test.BaseDeviceTest;

public class VNGDeviceTest extends BaseDeviceTest {
	

	/*protected AgentImpl getAgentImpl(DeviceDB deviceDB) throws Exception {
		String userJID = "userJID";
		String deviceNID = "deviceNID";		
		MRLog log = MRLog.getInstance(MRLog.CONSOLE);
		
		String deviceXML = FileUtils.readFileWithPath(VNGFileUtils.rootPath, VNGFileUtils.deviceXMLPath);
		Document deviceDoc = XMLUtils.stringToDocument(deviceXML);
		String deviceType = deviceDoc.getRootElement().getName();
		StreamingServerI streamingServer = new DummyStreamingServerImpl(userJID, log, null);
		streamingServer.getNetworkResource().updateNetworkResource(deviceDoc);

		DeviceAdminEngineI deviceEngine  = new DummyDeviceEngineImpl(log, deviceDB);
		AgentImpl agentImpl = new AgentImpl();
		agentImpl.mediaAgent = new XPMediaAgentImpl(userJID, deviceNID,deviceType,
				streamingServer, log, deviceDoc);
		agentImpl.deviceAdminAgent = new V2DTxDeviceAdminAgentImpl(log,  deviceEngine);
		agentImpl.layoutAgent = new XPLayoutAgentImpl(userJID, deviceNID,deviceType,
				streamingServer, log, deviceDoc);
		agentImpl.mediaStoreAgent = null;
		
		return agentImpl;
	}*/
	
	public static void main(String []args) throws Exception {
		configFile = args[0];
		statusFile = args[1];
		VNGDeviceTest deviceTest = new VNGDeviceTest();		
		deviceTest.run();
	}
}
