package com.ipvsserv.xmpp.session;

import org.xmpp.packet.Packet;
/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface XmppSessionEventListener {
    public void processPacket(Packet packet, XmppSession comp, int ID);
    
    public void connectionLost(XmppSession comp, int ID);
    
    public void loginComplete(XmppSession comp);
    
    public void logoutComplete(XmppSession comp);
    
    //may be not needed 
    public void loginComplete(XmppSession comp, int ID);
    
    public void logoutComplete(XmppSession comp, int ID);
    
    //////////////////////// new  ////////////////////////
    //public abstract void cancelPacketCollector(PacketCollector packetCollector);
    
    //public abstract PacketCollector createPacketCollector(PacketFilter packetFilter);
    
}
