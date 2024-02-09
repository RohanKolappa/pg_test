package com.ipvs.ext.xmppclient;

import java.io.StringReader;
import java.util.Collection;
import java.util.Properties;

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

import com.ipvs.ext.provider.IpvsXmppExtension;
import com.ipvs.ext.provider.IpvsXmppExtensionProvider;
import com.ipvs.ext.provider.IpvsXmppIQ;
import com.ipvs.ext.provider.IpvsXmppIQProvider;
import com.ipvs.ext.provider.IpvsXmppParser;


class IpvsXmppClient implements PacketListener, ConnectionListener, MessageListener{
	
	XMPPConnection connection;
	IpvsXmppClientListener listener;
	String appServerJID;
	String userJID;
	public static String QUERY = "query";
	public static String EXTENSION = "x";
	int logLevel;
	
	public IpvsXmppClient(IpvsXmppClientListener listener, int logLevel) {
		this.connection = null;
		this.listener = listener;
		this.appServerJID = "managementserver.localhost";
	}
	
	public void setAppServerJID(String appServerJID) {
		this.appServerJID = appServerJID;
	}
	
	public void setListener(IpvsXmppClientListener listener) {
		this.listener = listener;
	}
	
	public String getUserJID() {
		return this.userJID;
	}
	
	public void login(String ip, String port, String user, String password, String resource) throws Exception {
		IpvsXmppLog.log("Connecting to XMPP Server at:" + ip + ":" + port + " for user=" + user, IpvsXmppLoggerI.INFO, this.logLevel);
		String service = "localhost";
		ConnectionConfiguration connConfig =
			new ConnectionConfiguration(ip, Integer.parseInt(port), service);
		XMPPConnection c = new XMPPConnection(connConfig);
		try {
			c.connect();
		} catch(Exception e) {
			e.printStackTrace();
			throw e;
		}
		try {
			c.login(user, password, resource);

		} catch(Exception e) {
			e.printStackTrace();
			throw e;
		}
		this.connection = c;
		ProviderManager pm = ProviderManager.getInstance();
		if(pm.getIQProvider(IpvsXmppClient.QUERY, IpvsXmppParser.IPVSNS) == null) {
			pm.addIQProvider(IpvsXmppClient.QUERY, IpvsXmppParser.IPVSNS, new IpvsXmppIQProvider());
		}

		if(pm.getExtensionProvider(IpvsXmppClient.EXTENSION, IpvsXmppParser.IPVSNS) == null) {
			pm.addExtensionProvider(IpvsXmppClient.EXTENSION, IpvsXmppParser.IPVSNS, new IpvsXmppExtensionProvider());
		}

		// this.connection.addPacketListener(this,  new PacketTypeFilter(IQ.class));
		OrFilter orFilter = new OrFilter(
				new PacketTypeFilter(Message.class),new PacketTypeFilter(IQ.class) );
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
		Properties p = IpvsXmppParser.parseProperties(parser);
		String to = p.getProperty("to", String.valueOf(System.currentTimeMillis()));
		
		// body tag
		parser.next();
		
		// body child
		parser.nextToken();
		IpvsXmppParser providerParser = new IpvsXmppParser();
		String bodyChild = providerParser.parse(parser);
		
		// Chat chat = connection.getChatManager().createChat(to, this);
		// chat.sendMessage(bodyChild);
		
		String body = "<x xmlns='" + IpvsXmppParser.IPVSNS + "'>" + bodyChild + "</x>";
        Message m = new Message();
        IpvsXmppExtension ipvsMessageExtension = new IpvsXmppExtension(body);
        m.addExtension(ipvsMessageExtension);
        m.setFrom(userJID);
        m.setTo(to);
		this.connection.sendPacket(m);
	}
	
	public void sendRequest(String request) throws Exception {
		this.checkConnection();
		
		MXParser parser = new MXParser();
		parser.setInput(new StringReader(request));
		
		// iq tag
		Properties p = IpvsXmppParser.parseProperties(parser);
		String id = p.getProperty("id", String.valueOf(System.currentTimeMillis()));
		
		// query tag
		String name = null;
		while(name == null) {
			parser.next();
			name = parser.getName();
		}
		
		// query child
		name = null;
		while(name == null) {
			parser.nextToken();
			name = parser.getName();
		}
		
		
		IpvsXmppParser providerParser = new IpvsXmppParser();
		String queryChild = providerParser.parse(parser);

		String xml = "<" + IpvsXmppClient.QUERY + " xmlns='" + IpvsXmppParser.IPVSNS + "'>" + queryChild + "</" + IpvsXmppClient.QUERY + ">";
		IpvsXmppLog.log("Sending xml=" + xml, IpvsXmppLoggerI.DEBUG, this.logLevel);

		IpvsXmppIQ requestIQ = new IpvsXmppIQ(xml);
		requestIQ.setTo(appServerJID);
		requestIQ.setType(IQ.Type.SET);
		requestIQ.setPacketID(id);

		this.connection.sendPacket(requestIQ);
	}
	
	private boolean isIpvsPacket(Packet packet) {
		if(packet instanceof IQ)
			return (packet instanceof IpvsXmppIQ);
		// Check for Message
		PacketExtension pe = packet.getExtension(IpvsXmppParser.IPVSNS);
		if(pe != null)
			return (pe instanceof IpvsXmppExtension);
		return false;
	}
	
	public void processPacket(Packet packet) {
		String s = packet.toXML();
		IpvsXmppLog.log("Received packet=" + s, IpvsXmppLoggerI.DEBUG, this.logLevel);
		if(!isIpvsPacket(packet))
			return;
		listener.handlePacketXML(s);
	}

	@Override
	public void connectionClosed() {
		listener.handleDisconnected("<ShutDown/>");
		// TODO Auto-generated method stub
		
	}

	@Override
	public void connectionClosedOnError(Exception e) {
		listener.handleDisconnected(e.toString());
		// TODO Auto-generated method stub
		
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
