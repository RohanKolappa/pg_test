package com.barco.nimbus.api.common;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.nimbus.utils.common.XMLUtils;


public class SCMessage {
	
	// Message.type: one of
	public static final String REQUEST = "Request";
	public static final String RESPONSE = "Response";
	public static final String EVENT = "Event";
	public static final String NOTIFICATION = "Notification";
	
	// what is this? 
	public static final String INFOTYPE = "INFO";
	public static final String ERRORTYPE = "ERROR";
	
	// Message.state: one of
	public static final int REQUEST_STATE_DONE=200;
	public static final int REQUEST_STATE_ERROR=0;
	
	public static final Level [] logLevels = {Level.OFF, Level.FATAL, Level.ERROR, Level.WARN, Level.INFO, Level.DEBUG};
	
	Document messageDoc;
	String to;
	String from;
	String id;
	String type;
	String rootName;
	String service;
	String clientData;
	String clientCallback;
	int state;
	String protocol;
	String extNamespace;
	
	Level logLevel;
	Element context;
	String contextRootName;
	Document result;
	String  messageBusClassName = null;

	public SCMessage(String protocol, Document messageDoc, String type, 
			String service, String rootName, String clientData,String callbackData, String from, String to, Level logLevel) {
		this.protocol = protocol;
		this.messageDoc = messageDoc;
		this.type = type;
		this.service = service;
		this.rootName = rootName;
		this.clientData = clientData;
		this.clientCallback = callbackData;
		this.from = from;
		this.to = to;
		this.logLevel = logLevel;
		this.result = null;
		this.context = null;
		this.contextRootName = null;
	}	


	public int getState() {
		return state;
	}

	public void setState(int state) {
		this.state = state;
		if(this.messageDoc != null) {
			try {
				Element headerElement = XMLUtils.getElementXpath(messageDoc, "//Header");
				if(headerElement != null)
					headerElement.addAttribute("state", String.valueOf(state));
			}
			catch(Exception e) {
				e.printStackTrace();
			}
			
		}
	}

	public String getClientCallback() {
		return clientCallback;
	}

	public Level getLogLevel() {
		return this.logLevel;
	}

	public String getType() {
		return type;
	}

	public String getTo() {
		return to;
	}

	public void setTo(String to) {
		this.to = to;
	}

	public String getClientData() {
		return clientData;
	}

	public void setClientData(String clientData) {
		this.clientData = clientData;
	}
	
	public void updateResponseInfo(String clientData,String callbackData) throws Exception {
		this.setClientData(clientData);
		this.setCallback(callbackData);
		XMLUtils.setValueXpath(messageDoc, "//ClientCallback", callbackData);
		XMLUtils.setValueXpath(messageDoc, "//ClientData", clientData);
	}

	private void setCallback(String callbackData) {
		this.clientCallback = callbackData;
		
	}

	public void setMessageDoc(Document messageDoc) {
		this.messageDoc = messageDoc;
	}

	public void setRootName(String rootName) {
		this.rootName = rootName;
	}

	public void setService(String service) {
		this.service = service;
	}

	public void setFrom(String from) {
		this.from = from;
	}

	public Document getMessageDoc() {
		return messageDoc;
	}
	
	public String getService(){
		return service;
	}
	
	public String getRootName() {
		return rootName;
	}
	
	public String getFrom() {
		return from;
	}

	public Document getResult() {
		return result;
	}

	public void setResult(Document result) {
		this.setState(SCMessage.REQUEST_STATE_DONE);
		this.result = result;
	}
	
	public void setError(Document result) {
		this.setState(SCMessage.REQUEST_STATE_ERROR);
		this.result = result;
	}
	
	public Element getContext() {
		return context;
	}
	
	public String getContextRootName() {
		return this.contextRootName;
	}
	
	public void setContext(Element context) {
		this.context = context;
	}

	public void setContextRootName(String contextRootName) {
		this.contextRootName = contextRootName;
	}

	public String getMessageBusClassName() {
		return messageBusClassName;
	}

	public void setMessageBusClassName(String messageBusClassName) {
		this.messageBusClassName = messageBusClassName;
	}

	public String getProtocol() {
		return this.protocol;
	}

	public void setProtocol(String protocol) {
		this.protocol = protocol;
	}


	public String getId() {
		return id;
	}


	public void setId(String id) {
		this.id = id;
	}


	public String getExtNamespace() {
		return extNamespace;
	}


	public void setExtNamespace(String extNamespace) {
		this.extNamespace = extNamespace;
	}

	
	
}
