package com.barco.device.service.deafult.provider;

import java.util.ArrayList;

import com.barco.device.agent.AgentImpl;
import com.barco.device.service.ServiceI;

public class DefaultServiceProvider implements ServiceI {

	ArrayList<String> devices;
	public DefaultServiceProvider(){
		ArrayList<String> supportedDevices = new ArrayList<String>();
		supportedDevices.add("default");		
		supportedDevices.add("MS");
		devices = supportedDevices;
	}
	public AgentImpl getAgentImpl() throws Exception{
		// TODO Auto-generated method stub
		return new AgentImpl();
	}
	@Override
	public ArrayList<String> getSupportedDevices() {		
		return devices;
	}
	
	

}
