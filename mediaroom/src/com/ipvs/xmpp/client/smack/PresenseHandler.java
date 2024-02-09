package com.ipvs.xmpp.client.smack;

import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.QName;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.Presence;
import org.jivesoftware.smack.packet.RosterPacket;
import org.jivesoftware.smack.util.StringUtils;

import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.xmpp.client.params.RosterParams;
import com.ipvs.xmpp.client.smack.packets.IpvsIQ;

public class PresenseHandler {
    private Map<String, Map<String, Presence.Type>> presenceMap = new ConcurrentHashMap<String, Map<String, Presence.Type>>();
    private XMPPI connection = null;

    public PresenseHandler() {
    }

    public void init(XMPPI connection) {
        this.connection = connection;
        reload();
    }

    public void reload() {
        IpvsIQ iq = new IpvsIQ();
        iq.setParams(new RosterParams());
        try {
            connection.sendPacket(iq);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private Map<String, Presence.Type> getPresences(String user) {
        String bareJID = StringUtils.parseBareAddress(user);
        synchronized (presenceMap) {
            return presenceMap.get(bareJID);
        }
    }

    public boolean isUserPresence(String user) {
        Map<String, Presence.Type> fullPresences = getPresences(user);
        if (fullPresences != null) {
            synchronized (fullPresences) {
                if (fullPresences.containsKey(user)) {
                    return true;
                }
                String bareJID = StringUtils.parseBareAddress(user);
                if (fullPresences.containsKey(bareJID)) {
                    return true;
                }
            }
        }
        return false;

    }

    private Presence.Type getPresenceType(String user) {
        Map<String, Presence.Type> fullPresences = getPresences(user);
        if (fullPresences != null) {
            synchronized (fullPresences) {
                if (fullPresences.containsKey(user)) {
                    return fullPresences.get(user);
                }
                String bareJID = StringUtils.parseBareAddress(user);
                if (fullPresences.containsKey(bareJID)) {
                    return fullPresences.get(user);
                }
            }
        }
        return Presence.Type.unavailable;

    }

    public Presence getPresence(String user) {
        //not saving the resource in list
        Presence.Type type = getPresenceType(user);
        if (type == null) {
            Presence presence = new Presence(Presence.Type.unavailable);
            presence.setFrom(user);
            return presence;
        } else {
            Presence presence = null;
            presence = new Presence(Presence.Type.unavailable);
            presence.setFrom(user);
            return presence;
        }
    }

    public String getPresenceList(String calledFrom) {
        StringBuffer buf = new StringBuffer();
        buf.append("<presence cound='" + presenceMap.size() + "' >");
        for (String user : presenceMap.keySet()) {
            buf.append("<user jid='" + user + "' presence='" + presenceMap.get(user).toString() + "'>");

        }
        buf.append("</presence>");
        System.out.println(calledFrom + " presence:::::::::::" + buf.toString());
        return buf.toString();
    }

    public void processPacket(Packet packet) {
        if (packet instanceof RosterPacket) {
            processPacketRoster(packet);
        } else if (packet instanceof Presence) {
            processPacketPresence(packet);
        } else {
            //do noting
        }
    }

    /**
     * Listens for all presence packets and processes them.
     */
    //private class PresencePacketListener implements PacketListener {
    private void processPacketPresence(Packet packet) {
        Presence presence = (Presence) packet;
        String user = presence.getFrom();
        String bareJID = StringUtils.parseBareAddress(user);
        if (isMUCPresence(packet)) {
            return;
        }
        if (presence.getType() == Presence.Type.available) {
            Map<String, Presence.Type> fullPresences = null;
            synchronized (presenceMap) {
                fullPresences = presenceMap.get(bareJID);
                if (fullPresences == null) {
                    fullPresences = new ConcurrentHashMap<String, Presence.Type>();
                }
                fullPresences.put(user, presence.getType());
                presenceMap.put(bareJID, fullPresences);
            }
        } else if (presence.getType() == Presence.Type.unavailable) {
            Map<String, Presence.Type> fullPresences = null;
            synchronized (presenceMap) {
                fullPresences = presenceMap.get(bareJID);
                if (fullPresences != null) {
                    fullPresences.remove(user);
                }
                if (fullPresences == null || fullPresences.size() == 0) {
                    presenceMap.remove(bareJID);
                }
            }
        } else if (presence.getType() == Presence.Type.subscribe) {
            Presence response = new Presence(Presence.Type.subscribed);
            response.setTo(presence.getFrom());
            try {
                connection.sendPacket(response);
            } catch (Exception e) {
                e.printStackTrace();
            }
        } else if (presence.getType() == Presence.Type.unsubscribe) {
            Presence response = new Presence(Presence.Type.unsubscribed);
            response.setTo(presence.getFrom());
            try {
                connection.sendPacket(response);
            } catch (Exception e) {
                e.printStackTrace();
            }
        }

        //getPresenceList("PresencePacketListener");
    }

    //}

    /**
     * Listens for all roster packets and processes them.
     */
    // private class RosterPacketListener implements PacketListener {
    private void processPacketRoster(Packet packet) {
        //System.out.println("RosterPacketListener " + packet.toXML());
        Document doc;
        try {

            doc = MRXMLUtils.stringToDocument(packet.toXML());
            Element queryEL = MRXMLUtils.getDescendentWithoutNamespace(doc.getRootElement(), "query");
            //List<?> items = queryEL.getChildren("item", Namespace.getNamespace("jabber:iq:roster"));
            Namespace rosterNS = Namespace.get("jabber:iq:roster");
            List<?> items = queryEL.elements(QName.get("item", rosterNS));
            for (Iterator<?> iterator = items.iterator(); iterator.hasNext();) {
                Element el = (Element) iterator.next();
                String user = MRXMLUtils.getAttributeValue(el, "jid", "").trim();
                String subscription = MRXMLUtils.getAttributeValue(el, "subscription", "").trim();
                String bareJID = StringUtils.parseBareAddress(user);
                if (subscription.equals("remove")) {
                    Map<String, Presence.Type> fullPresences = null;
                    synchronized (presenceMap) {
                        fullPresences = presenceMap.get(bareJID);
                        if (fullPresences != null) {
                            fullPresences.remove(user);
                        }
                        if (fullPresences == null || fullPresences.size() == 0) {
                            presenceMap.remove(bareJID);
                        }
                    }
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        //getPresenceList("RosterPacketListener");			
    }

    //}

    private boolean isMUCPresence(Packet packet) {
        try {
            String XMLNSMUC = "http://jabber.org/protocol/muc#user";
            // String xmlns = packet.getXmlns(); // Returns null for some reason
            String xmlns = packet.toXML();
            if (xmlns.indexOf(XMLNSMUC) != -1) {
                return true;
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return false;
    }

}
/*
<WRITE><iq id="fMfb1-2" type="get"><query xmlns="jabber:iq:roster"></query></iq></WRITE>
<READ><iq id="fMfb1-2" to="managementserver@localhost/managementserver" type="result">
<query xmlns="jabber:iq:roster">
<item jid="adminuser@localhost" subscription="to"></item>
<item jid="tx1@localhost" subscription="to"></item><item jid="superuser-default@localhost" subscription="to"></item>
<item jid="dcuser1@localhost" subscription="to"></item><item jid="srcuser1@localhost" subscription="to"></item>
<item jid="srcrelay1@localhost" subscription="to"></item><item jid="ipvsdbservice.localhost" subscription="from"></item>
<item jid="managementserver@localhost" subscription="both"></item></query></iq></READ>
 */