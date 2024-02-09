package com.ipvsserv.xmpp.session;

import org.xmpp.packet.JID;
import org.xmpp.packet.Packet;

public interface InternalJID {
    
    public JID getAddress() ;
    public abstract boolean isStarted();
    
    /////////////////// route
    public abstract void sendPacket(Packet packet) throws Exception;
    public abstract void processPacket(Packet packet) throws Exception;
    public abstract void processPacket(String to, String packet) throws Exception;
    
}
