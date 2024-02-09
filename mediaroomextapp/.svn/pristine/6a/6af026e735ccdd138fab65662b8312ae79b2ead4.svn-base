package com.barco.device.base;

import com.barco.device.handler.DeviceAdmin.impl.DeviceServerI;
import com.barco.utils.mr.MRLog;

public abstract class DeviceAdminAgentI implements AgentI {
	
	// To be changed to an interface by Padmaja - Iyer
	
	protected DeviceServerI deviceServer= null;
	protected MRLog log =null;
	String configFile = "/etc/config/Device.xml";
	
	protected DeviceAdminAgentI(MRLog log,String configFile,DeviceServerI deviceServer)
	{
		this.log = log;
		this.configFile = configFile;
		this.deviceServer = deviceServer;
	}
	
	public DeviceServerI getDeviceServer() {
		return deviceServer;
	}

}
