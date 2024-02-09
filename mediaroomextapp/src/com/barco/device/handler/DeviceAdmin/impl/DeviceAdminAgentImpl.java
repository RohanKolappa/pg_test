package com.barco.device.handler.DeviceAdmin.impl;

import com.barco.device.base.DeviceAdminAgentI;
import com.barco.device.base.DeviceI;
import com.barco.utils.mr.MRLog;

public class DeviceAdminAgentImpl extends DeviceAdminAgentI{	
	
	DeviceI device;
	
	public DeviceAdminAgentImpl(MRLog log, String configFile,DeviceServerI deviceServer){
		super(log,configFile,deviceServer);
	}

	@Override
	public void setDevice(DeviceI device) {
		this.device = device;
	}
}
