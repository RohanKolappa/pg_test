package com.ipvs.common.service;

import com.ipvs.common.MRException;

public interface ServiceAgentObserverI {
	public void handleMessage(String message) throws MRException;
}
