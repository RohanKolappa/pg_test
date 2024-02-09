package com.ipvs.common.db;

import org.dom4j.Element;

import com.ipvs.common.MRConst;
import com.ipvs.common.service.MRXMLUtils;

public class ServiceRequestHeader {
	//private Element headerEL = null;
	private String serviceName = "";
	private String requestName = "";
	private int logLevel = 0;
	private String clientData = "";

	public Element getHeaderEL() throws Exception {
		Element	headerEL = MRXMLUtils.addElement("Header");
		MRXMLUtils.addAttribute(headerEL,MRConst.SERVICE_NAME, serviceName);
		MRXMLUtils.addAttribute(headerEL,MRConst.REQUEST_NAME, requestName);
		MRXMLUtils.addAttribute(headerEL,MRConst.LOG_LEVEL, logLevel + "");
		MRXMLUtils.addElement(headerEL,MRConst.CLIENT_DATA, clientData);
		MRXMLUtils.addElement(headerEL,MRConst.CLIENT_CALLBACK, "");
		return headerEL;
	}

	public String getServiceName() {
		return serviceName;
	}

	public String getRequestName() {
		return requestName;
	}

	public int getLogLevel() {
		return logLevel;
	}

	public String getClientData() {
		return clientData;
	}
 
	public void setServiceName(String serviceName) {
		this.serviceName = serviceName;
	}

	public void setRequestName(String requestName) {
		this.requestName = requestName;
	}

	public void setLogLevel(int logLevel) {
		this.logLevel = logLevel;
	}

	public void setClientData(String clientData) {
		this.clientData = clientData;
	}

}
/*
 * 
 * <header> <servicename>assetadminservice</servicename>
 * <requestname>AssetManagement</requestname> <loglevel>0</loglevel>
 * <clientdata>clientdata</clientdata> </header>
 * 
 */