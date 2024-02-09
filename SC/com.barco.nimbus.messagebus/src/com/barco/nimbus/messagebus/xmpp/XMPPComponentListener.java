package com.barco.nimbus.messagebus.xmpp;

import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;
import org.xmpp.packet.Presence;

public interface XMPPComponentListener {
	
	public abstract void handleMessage(Message message);
	public abstract void handleIQ(IQ iq);
	public abstract void handlePresence(Presence presence);
	
	public abstract void handleDisconnected();

}
