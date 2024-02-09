package com.barco.device.vng.service.provider;

import java.util.ArrayList;

import com.barco.device.agent.AgentImpl;
import com.barco.device.service.ServiceI;
import com.barco.device.vng.DeviceAdmin.V2DRxDeviceAdminAgentImpl;
import com.barco.device.vng.Layout.V2DRxLayoutAgentImpl;
import com.barco.device.vng.Media.V2DRxMediaAgentImpl;
import com.barco.vng.common.DeviceDB;

public class V2DRxServiceProvider implements ServiceI {

	ArrayList<String> devices;
	public V2DRxServiceProvider(){		
		ArrayList<String> supportedDevices = new ArrayList<String>();		
		supportedDevices.add("V2D-Rx-Device");	
		devices = supportedDevices;
	}
	
	@Override
	public AgentImpl getAgentImpl() throws Exception {
		AgentImpl agentImpl = new AgentImpl();
		agentImpl.mediaAgent = new V2DRxMediaAgentImpl();
		agentImpl.layoutAgent = new V2DRxLayoutAgentImpl();
		agentImpl.mediaStoreAgent = null;
		agentImpl.deviceAdminAgent = new V2DRxDeviceAdminAgentImpl();
		agentImpl.deviceDB = new DeviceDB();
		
		return agentImpl;
	}
	
	@Override
	public ArrayList<String> getSupportedDevices() {
		// TODO Auto-generated method stub
		return devices;
	}
}
