package com.barco.device.service;

import java.util.ArrayList;

import com.barco.device.agent.AgentImpl;

public interface ServiceI {
	

	public AgentImpl getAgentImpl() throws Exception;	
	
	public ArrayList<String> getSupportedDevices();
}
