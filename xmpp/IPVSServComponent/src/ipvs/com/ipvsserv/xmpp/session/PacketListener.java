package com.ipvsserv.xmpp.session;

import org.xmpp.packet.Packet;
 
public interface PacketListener {
    public void processPacket(Packet packet);
}
