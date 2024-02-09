package com.barco.device.xmpp;


import com.barco.device.common.LoggerI;
import com.barco.device.common.Message;
import com.barco.device.common.MessageBusI;
import com.barco.device.common.MessageBusListenerI;
import com.barco.device.common.ServiceException;
import com.barco.device.common.ServiceExceptionCodes;
import com.ipvs.ext.xmppclient.IpvsXmppLoggerI;
import com.ipvs.ext.xmppclient.IpvsXmppXMLClient;

public class XMPPMessageBus extends IpvsXmppXMLClient implements MessageBusI{

	MessageBusListenerI listener;
	int state;
	
	public XMPPMessageBus() {
		this.listener = null;
		this.state = MessageBusI.DISCONNECTED;
		this.setLogLevel(IpvsXmppLoggerI.ERROR);
	}
	
	@Override
	protected void handleXML(String xml) {
		this.listener.handleLog("XMPPMessageBus:handleMessage xml=" + xml, LoggerI.DEBUG);
		if(!xml.startsWith("<message")) {
			this.listener.handleLog(xml, LoggerI.DEBUG);
			return;
		}
			
		Message message = null;
		try {
			message = Message.parseMessage(xml);
			// TBD Can set the to and from here if required
		}
		catch(Exception e) {
			// TBD handle the exception
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
			new ServiceException(ServiceExceptionCodes.HANDLERERROR, e.toString());			
		}
	}

	public void handleDisconnected(String reason) {
		super.handleDisconnected(reason);
		this.state = MessageBusI.DISCONNECTED;
		this.listener.handleException(new ServiceException(ServiceExceptionCodes.TERMINATED, reason));
	}

	@Override
	public void sendMessage(Message message) throws Exception {
		String to = message.getTo();
		if( (to == null) || ("".equals(to)))
			throw new ServiceException(ServiceExceptionCodes.HANDLERERROR, "XMPPMessageBus:sendMessage to=" + to);			
				
		String s = "<message to='" + message.getTo() + "'><x xmlns='com.ipvs.session'>" + 
					message.getMessageDoc().getRootElement().asXML() + "</x></message>";
		this.listener.handleLog("XMPPMessageBus:sendMessage s=" + s, LoggerI.DEBUG);
		this.handleRequest(s);
	}
	
	@Override
	public int getState() {
		return state;
	}

	@Override
	public void setListener(MessageBusListenerI listener) {
		this.listener = listener;
		
	}
	
}
