package com.barco.device.vng.service.provider;

import java.util.ArrayList;

import com.barco.device.agent.AgentImpl;
import com.barco.device.service.ServiceI;
import com.barco.device.test.TestMediaStoreAgent;
import com.barco.device.vng.DeviceAdmin.DMSDeviceAdminAgentImpl;
import com.barco.device.vng.Layout.DMSLayoutAgentImpl;
import com.barco.device.vng.Media.DMSMediaAgentImpl;
import com.barco.devicevng.MediaStore.DMSMediaStoreAgentImpl;
import com.barco.vng.common.DeviceDB;


public class DMSServiceProvider implements ServiceI {	

	ArrayList<String> devices;
	public DMSServiceProvider(){
		devices = new ArrayList<String>();		
		devices.add("DMS-100-Device");	
		devices.add("DMS-1000-Device");
		devices.add("DMS-120-Device");
		devices.add("DMS-1200-Device");
		devices.add("DMS-1500-Device");
		devices.add("DMS-1600-Device");
		devices.add("DMS-1700-Device");
		devices.add("DMS-200-Device");
		devices.add("DMS-25-Device");	
		devices.add("DMS-1550-Device");
		devices.add("DMS-150-Device");
	}
	@Override
	public ArrayList<String> getSupportedDevices() {
		// TODO Auto-generated method stub
		return devices;
	}
	private boolean dummy =false;
	
	@Override
	public AgentImpl getAgentImpl() throws Exception {
		AgentImpl agentImpl = new AgentImpl();
		agentImpl.mediaAgent = new DMSMediaAgentImpl();
		agentImpl.layoutAgent = new DMSLayoutAgentImpl();
		agentImpl.mediaStoreAgent = new DMSMediaStoreAgentImpl();
		
		//dummy =true; //testing in debug mode
		if(dummy){
			agentImpl.mediaStoreAgent = new TestMediaStoreAgent();
		} 
		agentImpl.deviceAdminAgent=new DMSDeviceAdminAgentImpl();
		agentImpl.deviceDB = new DeviceDB();
		agentImpl.HTTPServerPort= 10080;
		
		return agentImpl;
	}

}
