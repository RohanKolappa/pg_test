package com.ipvs.device.impl;

import org.dom4j.Document;

import com.ipvs.common.service.MRClient;
import com.ipvs.common.service.MRRequest;

public interface DeviceRequestHandlerI {

	public abstract Document handleDeviceAgentRequest(MRRequest request,String configFile,
			MRClient client) throws Exception;

}
