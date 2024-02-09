package com.barco.nimbus.messagebus;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;
import org.osgi.service.component.annotations.Activate;
import org.osgi.service.component.annotations.Component;
import org.osgi.service.component.annotations.Deactivate;
import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;
import org.xmpp.packet.PacketExtension;
import org.xmpp.packet.Presence;

import com.barco.nimbus.api.MessageBusI;
import com.barco.nimbus.api.MessageBusListenerI;
import com.barco.nimbus.api.common.SCException;
import com.barco.nimbus.api.common.SCMessage;
import com.barco.nimbus.messagebus.xmpp.XMPPComponent;
import com.barco.nimbus.messagebus.xmpp.XMPPComponentListener;
import com.barco.nimbus.utils.common.XMLUtils;

@Component
public class MessageBus implements MessageBusI, XMPPComponentListener {

	// connection parameters.
	private static final String serverIP = "localhost";
	private static final int serverPort = 5275;
	private static final String domain = "managementserver";
	private static final String secret = "test";

	private static Logger logger = Logger.getLogger(MessageBus.class);

	private XMPPComponent xc;
	private MessageBusListenerI listener;
	private int xcState = MessageBusI.DISCONNECTED;

	@Activate
	public void start() throws Exception {
		logger.info(":start");

		try {
			login();
			logger.info(":start messagebus logged in");
		} catch (Exception e) {
			logger.error(":start messagebus failed to login", e);
		}
	}

	@Deactivate
	public void stop() throws Exception {
		logger.info(":stop");
		this.logout();
	}

	@Override
	public void registerListener(MessageBusListenerI listener) {
		this.listener = listener;
	}

	@Override
	public int getState() {
		return xcState;
	}

	/**
	 * Sends the message to the to address in the message The protocol decides
	 * the handling XMPPIQ: can either be a
	 * Request/Response(xmlns="com.ipvs.session") XMPPMessage : can have either
	 * be a Request/Response/Event(xmlns= "com.barco.AgentMessage") or a
	 * Notification("com.ipvs.session")
	 */
	@Override
	public void sendMessage(SCMessage message) throws Exception {
		String from = message.getFrom();
		String to = message.getTo();
		String id = message.getId();
		if ((to == null) || to.isEmpty()) {
			logger.info(":sendMessage to not found, discarding message");
			return;
		}
		if (MessageBusUtils.XMPPIQ.equals(message.getProtocol())) {

			Element header = XMLUtils.getElementXpath(message.getMessageDoc(),
					"//Header");
			String deprecated = XMLUtils
					.getAttributeValue(header, "deprecated");
			Element data = XMLUtils.getElementXpath(message.getMessageDoc(),
					"//Data");
			Element responseData = (Element) data.elements().get(0);
			if (message.getState() == SCMessage.REQUEST_STATE_ERROR) {
				String errorXML = responseData.detach().asXML();
				errorXML = "<error code='400' type='modify' ><x xmlns='com.ipvs.session'>"
						+ errorXML + "</x></error>";
				Element context = message.getContext();
				String queryXML = "";
				if(context != null){
				context.setName(message.getContextRootName());
				 queryXML = "<query xmlns='"
						+ MessageBusUtils.XMLNS_SESSION + "'>"
						+ message.getContext().asXML() + "</query>";
				}else{
					queryXML = "<query xmlns='"
								+ MessageBusUtils.XMLNS_SESSION + "'>"
								+ errorXML  + "</query>";
				}
				this.xc.sendIQ(from, to, message.getClientData(), queryXML,
						IQ.Type.error, errorXML);
			} else {
				// Need to extract the response expected in the IQ query
				Element queryChild = MessageBusUtils.getDataElement(
						responseData, message.getRootName(), deprecated);
				String queryXML = "<query xmlns='"
						+ MessageBusUtils.XMLNS_SESSION + "'>"
						+ queryChild.detach().asXML() + "</query>";
				this.xc.sendIQ(from, to, message.getClientData(), queryXML,
						IQ.Type.result, null);
			}
		} else if (MessageBusUtils.XMPPMessage.equals(message.getProtocol())) {
			String extensionXML = null;
			String namespace = null;
			if (SCMessage.NOTIFICATION.equals(message.getType())) {
				
				Element root =message.getMessageDoc().getRootElement();
				if(message.getExtNamespace()!=null && !"".equals(message.getExtNamespace().trim()))
					namespace = message.getExtNamespace().trim();
				else
					namespace = MessageBusUtils.XMLNS_SESSION;
								
				extensionXML = "<x xmlns='" +namespace
						+ "'>"
						+ root.asXML()
						+ "</x>";				
			} else {
				extensionXML = "<x xmlns='"
						+ MessageBusUtils.XMLNS_AGENTMESSAGE + "'>"
						+ message.getMessageDoc().getRootElement().asXML()
						+ "</x>";
			}
			this.xc.sendMessage(from, to, id, extensionXML);
		} else
			logger.error(":sendMessage not supported protocol="
					+ message.getProtocol());
	}

	private void login() throws SCException, Exception {
		xc = new XMPPComponent();
		xc.setListener(this);
		this.xc.login(serverIP, serverPort, domain, secret);
		this.xcState = MessageBusI.CONNECTED;
	}

	private void logout() throws SCException, Exception {
		if (this.xc == null)
			return;
		this.xc.logout();
		this.xcState = MessageBusI.DISCONNECTED;
		this.xc = null;
	}

	@Override
	public void handleDisconnected() {
		this.xcState = MessageBusI.DISCONNECTED;
	}

	@Override
	public void handleMessage(Message message) {
		try {
			PacketExtension extension = message.getExtension("x",
					MessageBusUtils.XMLNS_AGENTMESSAGE);
			if (extension == null)
				return; // Ignore the message since it does not have the right
						// xmlns
			Element x = extension.getElement();
			Element agentMessage = (Element) x.element("AgentMessage").detach();
			agentMessage = XMLUtils.removeNamespace(agentMessage,
					MessageBusUtils.XMLNS_AGENTMESSAGE);
			SCMessage scMessage = MessageBusUtils
					.parseMessageUsingAgentMessage(agentMessage);
			if (this.listener != null)
				this.listener.handleMessage(scMessage);
		} catch (Exception e) {
			logger.warn(":handleMessage; exception occured while handle message.", e);
		}
	}

	@Override
	public void handleIQ(IQ iq) {
		try {
			String from = iq.getFrom().toString();
			String to = iq.getTo().toString();
			if (!IQ.Type.get.equals(iq.getType())
					&& !IQ.Type.set.equals(iq.getType()))
				return;// Not a request

			String clientData = iq.getID();
			Element query = iq.getChildElement();
			Element queryChild = (Element) query.elements().get(0);
			queryChild = (Element) queryChild.detach();
			queryChild = XMLUtils.removeNamespace(queryChild,
					MessageBusUtils.XMLNS_SESSION);
			// Check if deprecated
			if (!queryChild.getName().endsWith("Request")) {
				queryChild.addAttribute("deprecated", "true");
			}
			SCMessage scMessage = MessageBusUtils.parseMessageUsingQuery(from,
					to, clientData, queryChild);
			if (this.listener != null)
				this.listener.handleMessage(scMessage);
		} catch (Exception e) {
			logger.warn(":handleIQ; exception occured while handle IQ.", e);
		}
	}

	@Override
	public void handlePresence(Presence presence) {
		String presenceType = presence.getElement().attributeValue("type");
		if (!"unavailable".equals(presenceType))
			return;
		String userJID = presence.getElement().attributeValue("from");
		String messageStr = "<AgentMessage type='Request' to='' from=''>"
				+ "<Request serviceVer='2.31.11'>"
				+ "<Header serviceName='systemservice' requestName='DeviceLogoutRequest' logLevel='0'><ClientData/><ClientCallback/></Header>"
				+ "<Data><DeviceLogoutRequestData serviceVersion='2.31.11' softwareVersion='4.1.r7.b8' userJID='"
				+ userJID + "'/></Data>" + "</Request></AgentMessage>";
		try {
			Document messageDoc = XMLUtils.stringToDocument(messageStr);
			SCMessage logoutMessage = MessageBusUtils
					.parseMessageUsingAgentMessage(messageDoc.getRootElement());
			if (this.listener != null)
				this.listener.handleMessage(logoutMessage);
		} catch (Exception e) {
			logger.warn(":handlePresence; exception occured while handle presence.", e);
		}
	}

	/*
	 * public static SCMessage createEventUsingEventElement(Element
	 * eventElement,String toJID,String fromJID,String serviceName,String
	 * eventName,String eventLevel,Level logLevel) throws Exception {
	 * 
	 * Document doc = DocumentHelper.createDocument(); Element agentMessageEl =
	 * createAgentMessaget(EVENT, toJID, fromJID); Element eventEl =
	 * DocumentHelper.createElement("Event"); Element headerEl =
	 * DocumentHelper.createElement("Header");
	 * headerEl.addAttribute("serviceName", serviceName);
	 * headerEl.addAttribute("eventName", eventName);
	 * headerEl.addAttribute("eventAgentJID", fromJID);
	 * headerEl.addAttribute("eventWallclock",
	 * String.valueOf(System.currentTimeMillis()));
	 * headerEl.addAttribute("eventLevel", eventLevel); eventEl.add(headerEl);
	 * Element dataEl = DocumentHelper.createElement("Data");
	 * if(eventElement.getName().equals(eventName + "Data")) {// handle the case
	 * when eventDataElement has multiple children dataEl.add(eventElement); }
	 * else { Element eventDataEl =
	 * DocumentHelper.createElement(eventName+"Data");
	 * eventDataEl.add(eventElement); dataEl.add(eventDataEl); }
	 * eventEl.add(dataEl); agentMessageEl.add(eventEl);
	 * doc.setRootElement(agentMessageEl); return new SCMessage(XMPPMessage,
	 * doc, EVENT, serviceName, eventName, "", "", fromJID, toJID, logLevel); }
	 * 
	 * 
	 * public static SCMessage createRequestUsingRequestDocument(Document
	 * requestDocument,String toJID,String fromJID) throws Exception { Document
	 * doc = DocumentHelper.createDocument(); Element agentMessageEl
	 * =createAgentMessaget(REQUEST, toJID, fromJID);
	 * agentMessageEl.add((Element)requestDocument.getRootElement().clone());
	 * 
	 * Level logLevel = Level.INFO; Element headerElement =
	 * XMLUtils.getElementXpath(requestDocument, "//Header");
	 * 
	 * String service = headerElement.attributeValue("serviceName"); String
	 * rootName = headerElement.attributeValue("requestName"); String clientData
	 * = XMLUtils.getValueXpath(requestDocument, "//ClientData", ""); String
	 * callback = XMLUtils.getValueXpath(requestDocument, "//ClientCallback",
	 * ""); doc.add(agentMessageEl); return new SCMessage(XMPPMessage, doc,
	 * REQUEST, service, rootName, clientData, callback, fromJID, toJID,
	 * logLevel); }
	 * 
	 * 
	 * public static Element createAgentMessage(String agentType,String
	 * toJID,String fromJID) throws Exception { Element agentMessageEl =
	 * DocumentHelper.createElement("AgentMessage");
	 * agentMessageEl.addAttribute("type", agentType);
	 * agentMessageEl.addAttribute("to", toJID);
	 * agentMessageEl.addAttribute("from", fromJID); return agentMessageEl; }
	 */

	public SCMessage createResponseUsingRequestMessage(String to, SCMessage m)
			throws Exception {

		// logger.info("MessageBus:createResponseUsingRequestMessage messageDoc="
		// + m.getMessageDoc().asXML());
		String deprecated = "false";
		if (m.getResult() == null)
			throw new Exception(
					"MessageBus:createResponseUsingRequestMessage no result found");

		Element context = null;
		if (m.getState() == SCMessage.REQUEST_STATE_ERROR) {
		//	String s = m.getMessageDoc().asXML();
			if (m.getContextRootName() != null) {
				context = (Element) XMLUtils.getElementXpath(m.getMessageDoc(),
						"//" + m.getRootName()).clone();
			}
		}

		// Create the responseDoc using the requestMessageDoc
		Document responseDoc = m.getMessageDoc();
		responseDoc.getRootElement().addAttribute("to", m.getFrom());
		responseDoc.getRootElement().addAttribute("from", m.getTo());
		responseDoc.getRootElement().addAttribute("type", SCMessage.RESPONSE);

		// Change the REQUEST element to the RESPONSE element
		Element response = XMLUtils.getElementXpath(responseDoc, "//"
				+ SCMessage.REQUEST);
		response.setName(SCMessage.RESPONSE);

		// Clear the existing requestData from the Data element
		Element data = XMLUtils.getElementXpath(responseDoc, "//Data");
		Element requestData = (Element) data.elements().get(0);
		if (requestData != null && requestData.elements().size() > 0) {
			Element dataElement = (Element) requestData.elements().iterator()
					.next();
			if (dataElement != null)
				deprecated = dataElement.attributeValue("deprecated");
		}
		data.remove(requestData);

		// Create the responseData element
		String requestDataName = m.getRootName() + SCMessage.REQUEST + "Data";
		String responseDataName = requestDataName.replace(SCMessage.REQUEST,
				SCMessage.RESPONSE);
		if (m.getState() == SCMessage.REQUEST_STATE_ERROR) {
			// Set the result as the error, Get the context
			data.add(m.getResult().getRootElement());
		} else {
			// Create/Reuse the responseDataElement
			Element responseDataElement = XMLUtils.getElementXpath(m
					.getResult().getDocument(), "//" + responseDataName);
			if (responseDataElement != null) // Some handlers return the
												// responseDataElement
				data.add(responseDataElement.detach());
			else { // Create the response Element
				data.addElement(responseDataName);
				data.element(responseDataName).add(
						m.getResult().getRootElement());
			}
		}

		// Update the header
		XMLUtils.getElementXpath(responseDoc, "//Header").addAttribute("state",
				"" + m.getState());
		XMLUtils.getElementXpath(responseDoc, "//Header").addAttribute(
				"deprecated", "" + deprecated);
		Element agentMessage = XMLUtils.getElementXpath(responseDoc,
				"//AgentMessage");
		SCMessage responseMessage = MessageBusUtils
				.parseMessageUsingAgentMessage(agentMessage);

		// Update the state and protocol
		responseMessage.setState(m.getState()); // Should not need this
		responseMessage.setProtocol(m.getProtocol());
		responseMessage.setContextRootName(m.getContextRootName());
		responseMessage.setContext(context);

		return responseMessage;
	}

	@Override
	public String getJID() {
		try {
			return this.xc.getDomain() + ".localhost";
		} catch (Exception e) {
			return null;
		}
	}

	@Override
	public void subscribeUserPresence(String userJID) throws Exception {
		if (getJID().equals(userJID))
			return;
		this.xc.subscribeUserPresence(getJID(), userJID);
	}

	@Override
	public void unSubscribeUserPresence(String userJID) throws Exception {
		if (getJID().equals(userJID))
			return;
		this.xc.unSubscribeUserPresence(getJID(), userJID);
	}

	@Override
	public void addDiscoFeature(String feature) {	
		this.xc.addDiscoFeature(feature);
	}

}
