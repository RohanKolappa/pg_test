package com.ipvs.common.service;

import org.dom4j.Document;

import com.ipvs.common.MRException;

public interface XAuthServerAgentObserverI extends ServiceAgentObserverI {
	public void handleXAuthRequest(String message) throws MRException;
	public void handleXAuthCancelled(Document streamDataDoc)  throws Exception;
	public void handleXAuthDeleted(Document streamDataDoc)  throws Exception;
	
}
