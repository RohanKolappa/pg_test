package com.ipvsserv.xmpp.mdc;

import java.util.Collection;

import org.xmpp.packet.JID;
import org.xmpp.packet.Presence;

import com.ipvsserv.xmpp.session.XmppSessionImpl;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface PresenceHandler {
    
    public XmppSessionImpl getXmppSession();
    
    public void setXmppSession(XmppSessionImpl router);
    
    public void setEnableLog(boolean bEnable);
    
    public void clear();
    
    public void addPresneceListner(PresenceListener presneceListner);
    
    public void removePresneceListner(PresenceListener presneceListner);
    
    public void probePresences();
    
    public Collection<String> getShowPresences(JID subscriber);
    
    public void presenceSubscriptionNotRequired(String user);
    
    public void presenceSubscriptionRequired(String user);
    
    public boolean isUserInPresenceSubList(String user);
    
    public boolean isUserPresenceAvailable(String user);
    
    public void process(Presence presence);
    
}
