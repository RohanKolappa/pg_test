package com.barco.device.common;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;

import com.barco.utils.XMLUtils;

public class Message {
	
	// Message.type: one of
	public static final String REQUEST = "Request";
	public static final String RESPONSE = "Response";
	public static final String EVENT = "Event";
	
	// what is this? 
	public static final String INFOTYPE = "INFO";
	public static final String ERRORTYPE = "ERROR";
	
	// Message.state: one of
	public static final int REQUEST_STATE_DONE=200;
	public static final int REQUEST_WAITING_FOR_ASYN_RESPONSE = 201;
	public static final int REQUEST_STATE_ERROR=0;
	
	public static final Level [] logLevels = {Level.OFF, Level.FATAL, Level.ERROR, Level.WARN, Level.INFO, Level.DEBUG};
	
	Document messageDoc;
	String to;
	String from;
	String type;
	String rootName;
	String service;
	String clientData;
	String clientCallback;
	int state;
	boolean asynch =false;
	
	Level logLevel;
	Document result;
	String  messageBusClassName = null;

	public Message(Document messageDoc, String type, 
			String service, String rootName, String clientData,String callbackData, String from, String to, Level logLevel) {
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
	}
	
	public static Message parseMessage(String messageXML) throws Exception {
		Document doc = DocumentHelper.parseText(messageXML);
		Element agentMessage = XMLUtils.getElementXpath(doc, "//AgentMessage");
		Document messageDoc = XMLUtils.elementToDocument((Element)agentMessage.detach());
		String type = agentMessage.attributeValue("type");			
		
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
    	return new Message(messageDoc, type, service, rootName, clientData,clientCallback, from, to, logLevel);
	}
	
	
	public static Message createEvent(Element eventElement,String toJID,String fromJID,String serviceName,String eventName,String eventLevel,Level logLevel) throws Exception {
		
		Document doc = DocumentHelper.createDocument();
		Element agentMessageEl = createAgentMessaget(EVENT, toJID, fromJID);
		Element eventEl = DocumentHelper.createElement("Event");
		Element headerEl = DocumentHelper.createElement("Header");
		headerEl.addAttribute("serviceName", serviceName);
		headerEl.addAttribute("eventName", eventName);
		headerEl.addAttribute("eventAgentJID", fromJID);
		headerEl.addAttribute("eventWallclock", String.valueOf(System.currentTimeMillis()));
		headerEl.addAttribute("eventLevel", eventLevel);
		eventEl.add(headerEl);
		Element dataEl = DocumentHelper.createElement("Data");
		if(eventElement.getName().equals(eventName + "Data")) {// handle the case when eventDataElement has multiple children 
			dataEl.add(eventElement);			
		}
		else {
			Element eventDataEl = DocumentHelper.createElement(eventName+"Data");
			eventDataEl.add(eventElement);
			dataEl.add(eventDataEl);
		}
		eventEl.add(dataEl);
		agentMessageEl.add(eventEl);
		doc.setRootElement(agentMessageEl);
		return new Message(doc, EVENT, serviceName, eventName, "", "", fromJID, toJID, logLevel);
	}
	
	
	public static Message createRequest(Document requestDocument,String toJID,String fromJID) throws Exception {
		Document doc = DocumentHelper.createDocument();
		Element agentMessageEl =createAgentMessaget(REQUEST, toJID, fromJID);
		agentMessageEl.add((Element)requestDocument.getRootElement().clone());
		Level logLevel = Level.INFO;
		Element headerElement = XMLUtils.getElementXpath(requestDocument, "//Header");
	
		String service = headerElement.attributeValue("serviceName");
		String rootName =  headerElement.attributeValue("requestName");
		String clientData = XMLUtils.getValueXpath(requestDocument, "//ClientData", "");
		String callback = XMLUtils.getValueXpath(requestDocument, "//ClientCallback", "");
		doc.add(agentMessageEl);
		return new Message(doc, REQUEST, service, rootName, clientData, callback, fromJID, toJID, logLevel);
	}
	
	
	public static Element createAgentMessaget(String agentType,String toJID,String fromJID) throws Exception {
		Element agentMessageEl = DocumentHelper.createElement("AgentMessage");
		agentMessageEl.addAttribute("type", agentType);
		agentMessageEl.addAttribute("to", toJID);
		agentMessageEl.addAttribute("from", fromJID);
		return agentMessageEl;
	}


	public static Message createResponse(String to, Message m) throws Exception {
		Document responseDoc = m.getMessageDoc();
		responseDoc.getRootElement().addAttribute("to", m.getFrom());
		responseDoc.getRootElement().addAttribute("from", m.getTo());
		responseDoc.getRootElement().addAttribute("type", Message.RESPONSE);
		
		//String s = responseDoc.asXML();
		Element response = XMLUtils.getElementXpath(responseDoc,  "//" + Message.REQUEST);
		response.setName(Message.RESPONSE);
		
		Element data = XMLUtils.getElementXpath(responseDoc, "//Data");
		Element requestData = (Element)data.elements().get(0);
		String requestDataName = requestData.getName();
		String responseDataName = requestDataName.replace(Message.REQUEST, Message.RESPONSE);
		
		data.remove(requestData);
		data.addElement(responseDataName);
	    if(m.getResult()!=null)
			data.element(responseDataName).add(m.getResult().getRootElement());

	    //update state
		XMLUtils.getElementXpath(responseDoc, "//Header").addAttribute("state", ""+m.getState());
		
		return Message.parseMessage(responseDoc.asXML());
	}
	
	


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
		this.result = result;
	}

	public String getMessageBusClassName() {
		return messageBusClassName;
	}

	public void setMessageBusClassName(String messageBusClassName) {
		this.messageBusClassName = messageBusClassName;
	}

	public boolean isAsynch() {
		return asynch;
	}

	public void setAsynch(boolean asynch) {
		this.asynch = asynch;
	}


}
