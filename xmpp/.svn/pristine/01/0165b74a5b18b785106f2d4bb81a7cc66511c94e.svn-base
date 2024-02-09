package com.ipvsserv.xmpp.session;

import java.util.Hashtable;
import java.util.Map;

import org.xmpp.packet.JID;

public class InternalJIDManager {
    private static InternalJIDManager instance = null;
    
    public static synchronized InternalJIDManager getInstance() {
        if (instance == null) {
            instance = new InternalJIDManager();
        }
        return instance;
    }
    
    private InternalJIDManager() {}
    
    private Map<String, InternalJID> internalJIDs = new Hashtable<String, InternalJID>();
    
    public InternalJID getInternalJID(JID jid) {
        return internalJIDs.get(jid.toString());
    }
    public boolean isInternalJID(String jid) {
        return !(internalJIDs.get(jid)==null);
    }
    
    public InternalJID getInternalJID(String jid) {
        return internalJIDs.get(jid);
    }
    
    public void addInternalJID(InternalJID xmppSession) {
        internalJIDs.put(xmppSession.getAddress().toString(), xmppSession);
    }
    
    public void removeInternalJID(InternalJID xmppSession) {
        internalJIDs.remove(xmppSession.getAddress().toString());
    }
    
    
}
