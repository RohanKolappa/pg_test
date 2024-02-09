package com.ipvs.mediaroomservice.impl;

import com.ipvs.client.ServiceMessageHandlerI;
import com.ipvs.common.MRException;

public interface ConfigListenerI {
	public void handleConfigUpdates(String localConfigFile,String updateParentXPath,  String updateXPath, String userJID,ServiceMessageHandlerI messageHandler) throws MRException,Exception;
}
