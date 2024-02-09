package com.barco.device.vng.service.provider;

import java.util.ArrayList;


import com.barco.device.agent.AgentImpl;
import com.barco.device.service.ServiceI;
import com.barco.device.vng.DeviceAdmin.V2DTxDeviceAdminAgentImpl;
import com.barco.device.vng.Media.V2DTxMediaAgentImpl;
import com.barco.vng.common.DeviceDB;



public class V2DTxServiceProvider implements ServiceI {
	ArrayList<String> devices;	
	public V2DTxServiceProvider(){		
		devices = new ArrayList<String>();		
		devices.add("V2D-Tx-Device");			
	}
	
	@Override
	public AgentImpl getAgentImpl() throws Exception {   
		AgentImpl agentImpl = new AgentImpl();
		agentImpl.mediaAgent = new V2DTxMediaAgentImpl();
		agentImpl.mediaStoreAgent = null;
		agentImpl.deviceAdminAgent = new V2DTxDeviceAdminAgentImpl();
		agentImpl.deviceDB = new DeviceDB();
		
		return agentImpl;
	}
	
	@Override
	public ArrayList<String> getSupportedDevices() {
		// TODO Auto-generated method stub
		return devices;
	}

}
