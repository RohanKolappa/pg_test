package com.barco.device.xmpp;


import java.util.Random;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentException;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.jivesoftware.smack.packet.IQ;

import com.barco.device.common.Message;
import com.barco.device.common.MessageBusI;
import com.barco.device.common.MessageBusListenerI;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.barco.device.ext.xmppclient.BarcoXmppXMLClient;
import com.barco.utils.XMLUtils;

public class XMPPMessageBus extends BarcoXmppXMLClient implements MessageBusI{

	MessageBusListenerI listener;
	int state;
	int discoPacketID = 0;
	
	public XMPPMessageBus() {
		this.listener = null;
		this.state = MessageBusI.DISCONNECTED;
	}
	
	@Override
	public void init(MessageBusListenerI listener, Logger log) {
		this.logger = log;
		this.listener = listener;
	}
	
	@Override
	protected void handleXML(String xml) {
		//this.logger.debug("XMPPMessageBus:handleMessage :="+xml);
		if(!xml.startsWith("<message")) {
			if(xml.startsWith("<iq")){			
			   try {
				   Document iqDoc = DocumentHelper.parseText(xml);
				   String id = XMLUtils.getAttributeValueWithDefault(iqDoc.getRootElement(), "id", "0");
				   if(id.equals(String.valueOf(discoPacketID))) {
					   handleDiscoveryIQResponse(iqDoc);
				   }
				} catch (Exception e) {				
					e.printStackTrace();
				}				
			}
			//this.logger.debug(xml);
			return;
		}
			
		Message message = null;
		try {
			message = Message.parseMessage(xml);
			// TBD Can set the to and from here if required
		}
		catch(Exception e) {
			// TBD handle the exception
			e.printStackTrace();
			this.listener.handleException(new ServiceException(
					ServiceExceptionCodes.INVALIDMESSAGE, xml));
			return;
		}
		if(message.getService() == null) { // Debug message
			this.listener.handleException(new ServiceException(
					ServiceExceptionCodes.INVALIDMESSAGE, xml));
			return;
		}
		try {
			this.listener.handleMessage(message);
		}
		catch(ServiceException se) {
			// TBD handle the exception
			this.listener.handleException(se);
		}
		catch(Exception e) {
			e.printStackTrace();
			new ServiceException(ServiceExceptionCodes.HANDLERERROR, e.toString());			
		}
	}

	private void handleDiscoveryIQResponse(Document doc) {
		try {
			String type = XMLUtils.getAttributeValueWithDefault(doc.getRootElement(), "type", "");
			Element scServiceFeature = XMLUtils.getElementXpath(doc, "//feature[@var='BarcoSCService']");
			if(type.equals(IQ.Type.ERROR.toString()) || scServiceFeature == null) {	
				Thread.sleep(500);
				sendSCDiscoveryMessage();
				return;
			}
			
			String SCDiscoveryEventXML = "<SCServiceReadyEvent/>"; 
			Message message = Message.createEvent(DocumentHelper.parseText(SCDiscoveryEventXML).getRootElement(), this.getMyJID(), this.getAppServerAgentJID(), "DeviceAdmin", "SCServiceReadyEvent", "",Level.INFO);
			listener.handleMessage(message);
			//Duplicate responses are coming from server adding this one to aviod multiple device login requests to server
			discoPacketID = 0;
		} catch (DocumentException e) {
			e.printStackTrace();
		} catch (Exception e) {
			e.printStackTrace();
		}			
	}

	public void handleDisconnected(String reason) {
		this.state = MessageBusI.DISCONNECTED;
		this.listener.handleException(new ServiceException(ServiceExceptionCodes.TERMINATED, reason));
	}

	@Override
	public void sendMessage(Message message) throws Exception {
		String to = message.getTo();
		if( (to == null) || ("".equals(to)))
			throw new ServiceException(ServiceExceptionCodes.HANDLERERROR, "XMPPMessageBus:sendMessage to=" + to);			
				
		String s = "<message to='" + message.getTo() + "'><x xmlns='com.ipvs.agentmessage'>" + 
					message.getMessageDoc().getRootElement().asXML() + "</x></message>";
		this.logger.debug("XMPPMessageBus:sendMessage to=" + message.getTo());
		this.handleRequest(s);
	}
	
	@Override
	public int getState() {
		return state;
	}

	@Override
	public void login(String username, String password,  String resource, String ipaddress,
			String appserverAgentJID) throws ServiceException, Exception {
		super.handleLoginRequest(username,password,resource,ipaddress,appserverAgentJID);
	}

	@Override
	public void logout() throws ServiceException, Exception {
		super.handleLogoutRequest();		
	}

	@Override
	public void sendSCDiscoveryMessage() throws ServiceException, Exception {
		
	   // MIYE Required for debugging with DeviceApp running the AppServer
	   if(discoPacketID == 0) discoPacketID = new Random().nextInt();
	   
	   //scoPacketID = new Random().nextInt();
       String discoveryIQ="<iq id=\""+discoPacketID+"\" to=\""+getAppServerAgentJID()+"\" type=\"get\">	<query xmlns=\"http://jabber.org/protocol/disco#info\" /></iq>";
       handleRequest(discoveryIQ);
	}
}
