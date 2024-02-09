package com.barco.device.vng.service.provider;

import java.util.ArrayList;

import com.barco.device.agent.AgentImpl;
import com.barco.device.service.ServiceI;
import com.barco.device.vng.DeviceAdmin.MSDeviceAdminAgentImpl;
//import com.barco.device.vng.Media.MSMediaAgentImpl;
import com.barco.vng.common.DeviceDB;

public class MSServiceProvider implements ServiceI {

	ArrayList<String> devices;
	public MSServiceProvider(){
		devices = new ArrayList<String>();	
		devices.add("MS-25-Device");
		devices.add("MS-100-Device");
		devices.add("MS-150-Device");
	}
	public AgentImpl getAgentImpl() throws Exception {
		AgentImpl agentImpl = new AgentImpl();
		/*agentImpl.mediaAgent = new MSMediaAgentImpl();
		agentImpl.layoutAgent = new XPLayoutAgentImpl(userJID, deviceNID,deviceType,
				streamingServer, log, deviceDoc);
		agentImpl.mediaStoreAgent = null;*/
		agentImpl.deviceAdminAgent = new MSDeviceAdminAgentImpl();
		agentImpl.deviceDB = new DeviceDB();
		agentImpl.HTTPServerPort= 10080;
		return agentImpl;
	}

	@Override
	public ArrayList<String> getSupportedDevices() {
		// TODO Auto-generated method stub
		return devices;
	}

}
