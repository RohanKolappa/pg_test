package com.barco.nimbus.messagebus.xmpp;

import java.util.HashSet;
import java.util.Random;
import java.util.Set;

import org.apache.log4j.Logger;
import org.dom4j.Element;
import org.jivesoftware.whack.ExternalComponentManager;
import org.xmpp.component.Component;
import org.xmpp.component.ComponentException;
import org.xmpp.component.ComponentManager;
import org.xmpp.packet.IQ;
import org.xmpp.packet.IQ.Type;
import org.xmpp.packet.JID;
import org.xmpp.packet.Message;
import org.xmpp.packet.Packet;
import org.xmpp.packet.PacketExtension;
import org.xmpp.packet.Presence;

import com.barco.nimbus.utils.common.XMLUtils;

public class XMPPComponent implements Component {
	public static final String NAMESPACE_DISCO_INFO = "http://jabber.org/protocol/disco#info";
	Set<String> featureList = new HashSet<String>();
	protected Set<String> discoInfoFeatureNamespaces() {
	    return featureList;
	}
	
	public void addDiscoFeature(String feature) {
		featureList.add(feature);
	}
		
	private ExternalComponentManager mgr = null;
	private String domain = null;
	private XMPPComponentListener listener;
	
	public String getName() {
		return "Nimbus";
	}
	
	public String getDescription() {
		return "Nimbus Component";
	}
	
	public String getDomain() {
		return this.domain;
	}

	public void setListener(XMPPComponentListener listener) {
		this.listener = listener;
	}
	
	public void login(String serverIP, int serverPort, String domain, String secret) throws Exception {
		ExternalComponentManager mgr = new ExternalComponentManager(serverIP, serverPort);
		this.domain = domain;
		mgr.setSecretKey(domain, secret);
		mgr.addComponent(domain, this);
	}
	
	public void logout() {
		if(mgr == null)
			return;
		try {
			mgr.removeComponent(this.domain);
			mgr = null;
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	private boolean processDiscoInfo(IQ iq) {
		// Logger.getLogger(XMPPComponent.class).debug("XMPPComponent:processIQ=" + iq.toXML());	
		final Type type = iq.getType();
		final Element childElement = iq.getChildElement();
		String namespace = null;
		if (childElement != null) {
			namespace = childElement.getNamespaceURI();
		}
		if (type == Type.get) {
			if (NAMESPACE_DISCO_INFO.equals(namespace)) {
				final IQ replyPacket = IQ.createResultIQ(iq);
				final Element responseElement = replyPacket.setChildElement("query",
						NAMESPACE_DISCO_INFO);
				// features
				responseElement.addElement("feature").addAttribute("var",
						NAMESPACE_DISCO_INFO);
				for (final String feature : discoInfoFeatureNamespaces()) {
					responseElement.addElement("feature").addAttribute("var", feature);
				}
				mgr.sendPacket(this, replyPacket);
				return true;
			}
		}
		return false; // Not a discoInfo request
	}

	
	@Override
	public void processPacket(Packet packet) {
		if (packet instanceof IQ) {
			if(this.processDiscoInfo((IQ) packet))
				return;
			if(this.listener != null)
				this.listener.handleIQ((IQ) packet);
		} else if (packet instanceof Message) {
			if(this.listener != null)
				this.listener.handleMessage((Message) packet);
		} else if (packet instanceof Presence) {
			if(this.listener != null)
				this.listener.handlePresence((Presence) packet);
		}
	}
	
	public void sendMessage(String from, String to, String id, String extensionXML) throws Exception {
		Message message = new Message();
		message.setTo(to);
		message.setFrom(from);
		message.setID(id);
		// message.setBody(body);
		
		Element extensionElement = XMLUtils.stringToElement(extensionXML);
        PacketExtension extension = new PacketExtension(extensionElement);
		message.addExtension(extension);
		
		mgr.sendPacket(this, message);
	}


	public void sendIQ(String from, String to, String id, String queryXML, IQ.Type type, String errorXML) throws Exception {
		
		IQ iq = new IQ();
		iq.setFrom(from);
		iq.setID(id);
		iq.setTo(to);
		iq.setType(type);
		Element query = XMLUtils.stringToElement(queryXML);
		iq.setChildElement(query);
		if(IQ.Type.error.equals(type)) {
			Element extensionElement = XMLUtils.stringToElement(errorXML);
	        PacketExtension extension = new PacketExtension(extensionElement);
			iq.addExtension(extension);
		}
		mgr.sendPacket(this, iq);

	}

	@Override
	public void initialize(JID jid, ComponentManager componentManager)
			throws ComponentException {
		Logger.getLogger(XMPPComponent.class).info("XMPPComponent:initialize");
		mgr = (ExternalComponentManager) componentManager;
	}

	@Override
	public void start() {
		Logger.getLogger(XMPPComponent.class).info("XMPPComponent:start");
	}

	@Override
	public void shutdown() {
		Logger.getLogger(XMPPComponent.class).info("XMPPComponent:shutdown");
		if(this.listener != null)
			this.listener.handleDisconnected();
	}

	public void subscribeUserPresence(String myJID, String userJID) throws Exception{
	    Presence presenceSubscribe = new Presence(Presence.Type.subscribe);
	    presenceSubscribe.setTo(userJID);
	    presenceSubscribe.setFrom(myJID);
	    mgr.sendPacket(this, presenceSubscribe);
	}

	public void unSubscribeUserPresence(String myJID, String userJID) throws Exception{
	    Presence presenceSubscribe = new Presence(Presence.Type.unsubscribe);
	    presenceSubscribe.setTo(userJID);
	    presenceSubscribe.setFrom(myJID);
	    mgr.sendPacket(this, presenceSubscribe);
	}


}
