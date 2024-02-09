package com.barco.device.vng.service.provider;

import java.util.ArrayList;

import com.barco.device.agent.AgentImpl;
import com.barco.device.service.ServiceI;
import com.barco.device.vng.DeviceAdmin.XPDeviceAdminAgentImpl;
import com.barco.device.vng.Layout.XPLayoutAgentImpl;
import com.barco.device.vng.Media.XPMediaAgentImpl;
import com.barco.vng.common.DeviceDB;

public class XPServiceProvider implements ServiceI {

	ArrayList<String> devices;
	public XPServiceProvider(){
		devices = new ArrayList<String>();		
		devices.add("V2D-XP100-Device");	
		devices.add("V2D-XP200-Device");
		devices.add("V2D-XP200S-Device");
		devices.add("V2D-XP220-Device");
		devices.add("V2D-XP220S-Device");
	}
	
	public AgentImpl getAgentImpl() throws Exception {
		AgentImpl agentImpl = new AgentImpl();
		agentImpl.deviceAdminAgent= new XPDeviceAdminAgentImpl();
		agentImpl.mediaAgent = new XPMediaAgentImpl();
		agentImpl.layoutAgent = new XPLayoutAgentImpl();
		agentImpl.mediaStoreAgent = null;
		agentImpl.deviceDB = new DeviceDB();
		

		return agentImpl;
	}

	@Override
	public ArrayList<String> getSupportedDevices() {
		// TODO Auto-generated method stub
		return devices;
	}

}
