package com.barco.nimbus.messagebus;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.nimbus.api.common.SCMessage;
import com.barco.nimbus.utils.common.XMLUtils;

public class MessageBusUtils {

	// Protocol
	public static final String XMPPIQ = "xmppiq";
	public static final String XMPPMessage  = "xmppmessage";
	public static final String HTTP = "http";
	public static final String XMLNS_AGENTMESSAGE = "com.barco.agentmessage";
	public static final String XMLNS_SESSION = "com.ipvs.session";
	public static final String XMLNS_IPVSDBSERVICE = "ipvs:mediaroom:ipvsdbservice";
	
	
	public static SCMessage parseMessageUsingAgentMessage(Element agentMessage) throws Exception {
		Document messageDoc = XMLUtils.elementToDocument((Element)agentMessage.detach());
		String type = agentMessage.attributeValue("type");			
		
		String to = agentMessage.attributeValue("to");
		String from = agentMessage.attributeValue("from");
		Level logLevel = Level.ERROR;
		try {logLevel = SCMessage.logLevels[Integer.parseInt(agentMessage.attributeValue("logLevel"))];} catch(Exception e){};
		String service = null;
		String rootName = null;
		String clientData = null;
		String clientCallback= null;
		Element headerElement = XMLUtils.getElementXpath(messageDoc, "//Header");
		service = headerElement.attributeValue("serviceName");
		if(SCMessage.REQUEST.equals(type) || SCMessage.RESPONSE.equals(type)) {
			rootName = headerElement.attributeValue("requestName");
			rootName = rootName.replace("Request", ""); // Strip the Request Suffix
			clientData = XMLUtils.getValueXpath(messageDoc, "//ClientData", "");
			clientCallback = XMLUtils.getValueXpath(messageDoc, "//ClientCallback", "");
		}
		else { // Is EVENT
			rootName = headerElement.attributeValue("eventName");			
		}
    	return new SCMessage(XMPPMessage, messageDoc, type, service, rootName, clientData,clientCallback, from, to, logLevel);
	}

	public static SCMessage parseMessageUsingQuery(String from, String to, String clientData, Element root) throws Exception {
		String rootName = null;
		String clientCallback= null;
		String service = "sessionservice";
		rootName = root.getName();
		String contextRootName = rootName;
		rootName = rootName.replace("Request", ""); // Strip the Request Suffix
		root.setName(rootName);
		String type = SCMessage.REQUEST;			
		Level logLevel = Level.ERROR;
		String messageStr = "<AgentMessage type='Request' to='' from=''>"
				+ "<Request serviceVer='2.31.11'>" 
				+ "<Header serviceName='sessionservice' requestName='" + rootName + "Request' logLevel='0'><ClientData>" + clientData + "</ClientData><ClientCallback/></Header>"
				+ "<Data/>"
				+ "</Request></AgentMessage>";
		Document messageDoc = XMLUtils.stringToDocument(messageStr);
		Element data = XMLUtils.getElementXpath(messageDoc, "//Data");
		Element requestData = XMLUtils.stringToElement("<" + rootName + "RequestData/>");
		requestData.add(root);
		data.add(requestData);
    	SCMessage message = new SCMessage(XMPPIQ, messageDoc, type, service, rootName, clientData,clientCallback, from, to, logLevel);
		message.setContextRootName(contextRootName);
    	
		// Logger.getLogger(MessageBus.class).info("MessageBus:parseMessageUsingQuery messageDoc=" + messageDoc.asXML());
    	
    	return message;
	}
	
	public static Element getDataElement(Element responseData, String requestRootName, String deprecated) throws Exception {
		Element dataElement = (Element) (responseData.elements().iterator().next());
		if ("true".equals(deprecated))
			return dataElement;

		if (!requestRootName.equals(dataElement.getName())) {// Some responses do not include aroot element
			Document responseDoc = XMLUtils.stringToDocument("<" + requestRootName + "/>");
			responseDoc.getRootElement().add(dataElement.detach());
			dataElement = responseDoc.getRootElement();
		}
		dataElement.setName(requestRootName + "Response");
		return dataElement;
	}

}
