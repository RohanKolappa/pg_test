package com.ipvsserv.nxd.mdc.sub;

import org.xmpp.packet.JID;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class PubsubSubscription {
    private String subNid = "";
    private String subColNid = "";
    
    public String getSubColNid() {
        return subColNid;
    }
    
    public void setSubColNid(String subColNid) {
        this.subColNid = subColNid;
    }
    
    private String subJid = "";
    
    public String getSubNid() {
        return subNid;
    }
    
    public void setSubNid(String subNode) {
        subNid = subNode;
    }
    
    public String getSubJid() {
        return subJid;
    }
    
    public void setSubJid(String subJid) {
        this.subJid = subJid;
    }
    
    public JID getJID() {
        try {
            return new JID(subJid);
        } catch (Exception ex) {
            return null;
        }
    }
    
    public String getBareJID() {
        JID jid = getJID();
        if (jid != null) {
            return jid.toBareJID();
        }
        return subJid;
    }
    
}
