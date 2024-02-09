package com.ipvs.nimbus;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.dom4j.Node;


public class MRSCMessage {
	
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
	String extNamespce;
	
	Level logLevel;
	String resultString;
	String  messageBusClassName = null;

	public MRSCMessage(String protocol, Document messageDoc, String type, 
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
		this.resultString = null;
	}

	
	/*
	
	public static SCMessage parseMessageUsingIQ(Element iq, String serviceName) throws Exception {
		String type = REQUEST;
		String to = agentMessage.attributeValue("to");
		String from = agentMessage.attributeValue("from");
		Level logLevel = Level.ERROR;
		try {logLevel = logLevels[Integer.parseInt(agentMessage.attributeValue("logLevel"))];} catch(Exception e){};
		String service = null;
		String rootName = null;
		String clientData = null;
		String clientCallback= null;
		Element headerElement = XMLUtils.getElementXpath(messageDoc, "//Header");
		service = headerElement.attributeValue("serviceName");
		if(REQUEST.equals(type) || RESPONSE.equals(type)) {
			rootName = headerElement.attributeValue("requestName");
			clientData = XMLUtils.getValueXpath(messageDoc, "//ClientData", "");
			clientCallback = XMLUtils.getValueXpath(messageDoc, "//ClientCallback", "");
		}
		else { // Is EVENT
			rootName = headerElement.attributeValue("eventName");			
		}
    	return new SCMessage(XMPPIQ, messageDoc, type, service, rootName, clientData,clientCallback, from, to, logLevel);
	}


	*/
	
	


	public int getState() {
		return state;
	}

	public void setState(int state) {
		this.state = state;
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
	
	public String getProtocol() {
		return this.protocol;
	}
	
	public static void setValueXpath(Document doc, String xpath, String value) throws Exception {
		Node node = doc.selectSingleNode(xpath);
		// TBD handle the null case
		if(node == null) {
			System.out.println("XMLUtils:setValueXpath node not found for xpath=" + xpath);
			(new Exception()).printStackTrace();
			return;
			
		}
		node.setText(value);
	}
	
	private void setCallback(String callbackData) {
		this.clientCallback = callbackData;
		
	}

	public void setMessageDocString(Document messageDoc) {
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

	public String getResultString() {
		return resultString;
	}

	public void setResult(String resultString) {
		this.resultString = resultString;
	}

	public String getMessageBusClassName() {
		return messageBusClassName;
	}

	public void setMessageBusClassName(String messageBusClassName) {
		this.messageBusClassName = messageBusClassName;
	}


	public String getId() {
		return id;
	}


	public void setId(String id) {
		this.id = id;
	}


	public String getExtNamespce() {
		return extNamespce;
	}


	public void setExtNamespce(String extNamespce) {
		this.extNamespce = extNamespce;
	}


	
}
