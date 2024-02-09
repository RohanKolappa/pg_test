package com.barco.device.common;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.utils.mr.MRXMLUtils;

public class Message {
	
	public static final String REQUEST = "Request";
	public static final String RESPONSE = "Response";
	public static final String NOTIFICATION = "Notification";

	Document messageDoc;
	String to;
	String from;
	String type;
	String rootName;
	String service;
	String clientData;
	int logLevel;
	Document result;

	public Message(Document messageDoc, String type, 
			String service, String rootName, String clientData, String from, String to, int logLevel) {
		this.messageDoc = messageDoc;
		this.type = type;
		this.service = service;
		this.rootName = rootName;
		this.clientData = clientData;
		this.from = from;
		this.to = to;
		this.logLevel = logLevel;
		this.result = null;
	}
	
	public static Message parseMessage(String messageXML) throws Exception {
		Document doc = DocumentHelper.parseText(messageXML);
		Element agentMessage = MRXMLUtils.getElementXpath(doc, "//AgentMessage");
		Document messageDoc = MRXMLUtils.elementToDocument(agentMessage);
		String type = agentMessage.attributeValue("type");
		String to = agentMessage.attributeValue("to");
		String from = agentMessage.attributeValue("from");
		int logLevel = LoggerI.ERROR;
		try {logLevel = Integer.parseInt(agentMessage.attributeValue("logLevel"));} catch(Exception e){};
		String service = null;
		String rootName = null;
		String clientData = null;
		if(REQUEST.equals(type) || RESPONSE.equals(type)) {
			service = MRXMLUtils.getValueXpath(messageDoc, "//servicename", "");
			rootName = MRXMLUtils.getValueXpath(messageDoc, "//requestname", "");
			clientData = MRXMLUtils.getValueXpath(messageDoc, "//clientdata", "");
		}
		return new Message(messageDoc, type, service, rootName, clientData, from, to, logLevel);
	}

	public static Message createResponse(String to, Message m) throws Exception {
		Document responseDoc = m.getMessageDoc();
		responseDoc.getRootElement().addAttribute("to", m.getFrom());
		responseDoc.getRootElement().addAttribute("from", m.getTo());
		responseDoc.getRootElement().addAttribute("type", Message.RESPONSE);
		Element agentMessage = responseDoc.getRootElement();
		Element response = (Element)agentMessage.elements().get(0);
		response.setName(Message.RESPONSE);
		Element requestData = (Element)response.elements().get(1);
		String requestDataName = requestData.getName();
		String responseDataName = requestDataName.replace(Message.REQUEST, Message.RESPONSE);
		requestData.setName(responseDataName);
		requestData.remove((Element)requestData.elements().get(0));
		agentMessage.add(m.getResult().getRootElement());
		return Message.parseMessage(responseDoc.asXML());
	}

	public int getLogLevel() {
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
	
	public void updateClientData(String data) throws Exception {
		this.setClientData(data);
		MRXMLUtils.setValueXpath(messageDoc, "//clientdata", data);
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
		this.result = result;
	}


}
