package com.barco.device.ext.xmppclient;

import java.io.StringReader;
import java.util.Properties;

import org.apache.log4j.Logger;
import org.jivesoftware.smack.Chat;
import org.jivesoftware.smack.ConnectionConfiguration;
import org.jivesoftware.smack.ConnectionListener;
import org.jivesoftware.smack.MessageListener;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.XMPPConnection;
import org.jivesoftware.smack.filter.OrFilter;
import org.jivesoftware.smack.filter.PacketTypeFilter;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.packet.Message;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.PacketExtension;
import org.jivesoftware.smack.packet.Presence;
import org.jivesoftware.smack.provider.ProviderManager;
import org.xmlpull.mxp1.MXParser;

import com.barco.device.ext.provider.BarcoXmppExtension;
import com.barco.device.ext.provider.BarcoXmppExtensionProvider;
import com.barco.device.ext.provider.BarcoXmppIQ;
import com.barco.device.ext.provider.BarcoXmppParser;

class BarcoXmppClient implements PacketListener, ConnectionListener, MessageListener {
	
	XMPPConnection connection;
	BarcoXmppClientListener listener;
	String appServerJID;
	String userJID;
	Logger logger;
	public static String QUERY = "query";
	public static String EXTENSION = "x";
	int logLevel;
	
	public BarcoXmppClient(BarcoXmppClientListener listener, Logger log) {
		this.connection = null;
		this.listener = listener;
		this.appServerJID = "managementserver.localhost";
		this.logger = log;
	}
	
	public void setAppServerJID(String appServerJID) {
		if(!appServerJID.isEmpty()) {
			this.appServerJID = appServerJID;
		}
	}
	
	public void setListener(BarcoXmppClientListener listener) {
		this.listener = listener;
	}
	
	public String getUserJID() {
		return this.userJID;
	}
	
	public Logger getLogger() {
		return this.logger;
	}
	
	public void login(String ip, String port, String user, String password, String resource) throws Exception {
		this.logger.info("Connecting to XMPP Server at:" + ip + ":" + port + " for user=" + user);
		String service = "localhost";
		ConnectionConfiguration connConfig =
			new ConnectionConfiguration(ip, Integer.parseInt(port), service);
		XMPPConnection c = new XMPPConnection(connConfig);
		try {
			c.connect();
		} catch(Exception e) {
			e.printStackTrace();
			c.disconnect();
			throw e;
		}
		try {
			c.login(user, password, resource);

		} catch(Exception e) {
			e.printStackTrace();
			c.disconnect();
			throw e;
		}
		this.connection = c;		
		ProviderManager pm = ProviderManager.getInstance();
		/*
		if(pm.getIQProvider(BarcoXmppClient.QUERY, BarcoXmppParser.BARCONS) == null) { 
			pm.addIQProvider(BarcoXmppClient.QUERY, BarcoXmppParser.BARCONS, new BarcoXmppIQProvider());  
		} */

		if(pm.getExtensionProvider(BarcoXmppClient.EXTENSION, BarcoXmppParser.BARCONS) == null) {
			pm.addExtensionProvider(BarcoXmppClient.EXTENSION, BarcoXmppParser.BARCONS, new BarcoXmppExtensionProvider());
		}

		// this.connection.addPacketListener(this,  new PacketTypeFilter(IQ.class));
		OrFilter orFilter = new OrFilter(new PacketTypeFilter(Message.class),new PacketTypeFilter(IQ.class) );
		orFilter.addFilter(new PacketTypeFilter(Presence.class));
		this.connection.addPacketListener(this, orFilter);
		this.connection.addConnectionListener(this);
		this.userJID = user + "@localhost/" + resource;
	}
	
	public void logout() {		
		this.connection.disconnect();
		connection = null;
	}
	
	private void checkConnection() throws Exception {
		if(connection == null) {
			throw new Exception("Not Logged In");
		}
		if(!connection.isConnected()) {
			throw new Exception("Not Connected");
		}
	}
	
	public void sendMessage(String message) throws Exception {
		this.checkConnection();
		
		MXParser parser = new MXParser();
		parser.setInput(new StringReader(message));
		
		// message tag
		Properties p = BarcoXmppParser.parseProperties(parser);
		String to = p.getProperty("to", String.valueOf(System.currentTimeMillis()));
		
		// body tag
		parser.next();
		
		// body child
		parser.nextToken();
		BarcoXmppParser providerParser = new BarcoXmppParser();
		String bodyChild = providerParser.parse(parser);
		
		// Chat chat = connection.getChatManager().createChat(to, this);
		// chat.sendMessage(bodyChild);
		
		String body = "<x xmlns='" + BarcoXmppParser.BARCONS + "'>" + bodyChild + "</x>";
        Message m = new Message();
        BarcoXmppExtension ipvsMessageExtension = new BarcoXmppExtension(body);
        m.addExtension(ipvsMessageExtension);
        m.setFrom(userJID);
        m.setTo(to);
		this.connection.sendPacket(m);
	}
	
	
	public void sendIQ(String message) throws Exception {
		this.checkConnection();
		
		MXParser parser = new MXParser();
		parser.setInput(new StringReader(message));
		
		// iq tag
		Properties p = BarcoXmppParser.parseProperties(parser);
		String id = p.getProperty("id", String.valueOf(System.currentTimeMillis()));
		
		// query tag
		String name = null;
		while(name == null) {
			parser.next();
			name = parser.getName();
		}
		
		BarcoXmppParser providerParser = new BarcoXmppParser();
		String queryXML = providerParser.parse(parser);

		  
		BarcoXmppIQ requestIQ = new BarcoXmppIQ(queryXML);
		requestIQ.setTo(appServerJID);		
		requestIQ.setPacketID(id);
		this.connection.sendPacket(requestIQ);
	}

	private boolean isIpvsPacket(Packet packet) {
	
		// Check for Message
		PacketExtension pe = packet.getExtension(BarcoXmppParser.BARCONS);
		if(pe != null)
			return (pe instanceof BarcoXmppExtension);
		return false;
	}
	
	public void processPacket(Packet packet) {
		String s = packet.toXML();
		//this.logger.debug("Received packet=" + s);
		if(!isIpvsPacket(packet) && !(packet instanceof IQ))
			return;
		
		listener.handlePacketXML(s);
	}

	@Override
	public void connectionClosed() {
		//listener.handleDisconnected("<Shutdown/>");
		//we should not handle normal disconnected or connection closed on logout. We should handle only  connection closed on error
	}

	@Override
	public void connectionClosedOnError(Exception e) {
		listener.handleDisconnected("<Shutdown/>");
		//we have to close xmpp connection if server shutdown happened		
	    this.connection.disconnect();
		
	}

	@Override
	public void reconnectingIn(int arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void reconnectionFailed(Exception arg0) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void reconnectionSuccessful() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void processMessage(Chat arg0, Message message) {
		if(!isIpvsPacket(message))
			return;
		String packetXML = message.toXML();
		listener.handlePacketXML(packetXML);
		
	}

}
