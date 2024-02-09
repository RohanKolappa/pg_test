package com.ipvs.common;

import org.dom4j.Element;

import com.ipvs.common.factory.ServiceAgentFactoryI;

public interface AsynchDBRequestResponseListenerI {
	public void handleAsynchDBRequestResponse(String dbAction,Element responseElement,Object callBackDataObject,int status, String logData, ServiceAgentFactoryI serviceAgentFactory)  throws Exception;
}
