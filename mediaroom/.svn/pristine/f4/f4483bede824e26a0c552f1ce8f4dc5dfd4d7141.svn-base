package com.ipvs.xmpp.client.params;

import java.util.ArrayList;
import java.util.List;

public class MUCOwnerParams extends XmppParams {
    private List<Item> items = new ArrayList<Item>();
    private Destroy destroy;
    private String extensionsXML = "";

    public Destroy getDestroy() {
        return destroy;
    }

    public void setDestroy(Destroy destroy) {
        this.destroy = destroy;
    }

    public void addItem(Item item) {
        synchronized (items) {
            items.add(item);
        }
    }

    @Override
    public String toXML() {
        StringBuilder buf = new StringBuilder();
        buf.append("<query xmlns=\"http://jabber.org/protocol/muc#owner\">");
        synchronized (items) {
            for (int i = 0; i < items.size(); i++) {
                Item item = items.get(i);
                buf.append(item.toXML());
            }
        }
        if (getDestroy() != null) {
            buf.append(getDestroy().toXML());
        }
        // Add packet extensions, if any are defined.
        buf.append(getExtensionsXML());
        buf.append("</query>");
        return buf.toString();
    }

    private String getExtensionsXML() {
        return extensionsXML;
    }

    public void setExtensionsXML(String extensionsXML) {
        this.extensionsXML = extensionsXML;
    }

    public static class Item {

        private String actor;
        private String reason;
        private String affiliation;
        private String jid;
        private String nick;
        private String role;

        public Item(String affiliation) {
            this.affiliation = affiliation;
        }

        public String getActor() {
            return actor;
        }

        public String getReason() {
            return reason;
        }

        public String getAffiliation() {
            return affiliation;
        }

        public String getJid() {
            return jid;
        }

        public String getNick() {
            return nick;
        }

        public String getRole() {
            return role;
        }

        public void setActor(String actor) {
            this.actor = actor;
        }

        public void setReason(String reason) {
            this.reason = reason;
        }

        public void setJid(String jid) {
            this.jid = jid;
        }

        public void setNick(String nick) {
            this.nick = nick;
        }

        public void setRole(String role) {
            this.role = role;
        }

        public String toXML() {
            StringBuilder buf = new StringBuilder();
            buf.append("<item");
            if (getAffiliation() != null) {
                buf.append(" affiliation=\"").append(getAffiliation()).append("\"");
            }
            if (getJid() != null) {
                buf.append(" jid=\"").append(getJid()).append("\"");
            }
            if (getNick() != null) {
                buf.append(" nick=\"").append(getNick()).append("\"");
            }
            if (getRole() != null) {
                buf.append(" role=\"").append(getRole()).append("\"");
            }
            if (getReason() == null && getActor() == null) {
                buf.append("/>");
            } else {
                buf.append(">");
                if (getReason() != null) {
                    buf.append("<reason>").append(getReason()).append("</reason>");
                }
                if (getActor() != null) {
                    buf.append("<actor jid=\"").append(getActor()).append("\"/>");
                }
                buf.append("</item>");
            }
            return buf.toString();
        }
    };

    public static class Destroy {
        private String reason;
        private String jid;

        public String getJid() {
            return jid;
        }

        public String getReason() {
            return reason;
        }

        public void setJid(String jid) {
            this.jid = jid;
        }

        public void setReason(String reason) {
            this.reason = reason;
        }

        public String toXML() {
            StringBuilder buf = new StringBuilder();
            buf.append("<destroy");
            if (getJid() != null) {
                buf.append(" jid=\"").append(getJid()).append("\"");
            }
            if (getReason() == null) {
                buf.append("/>");
            } else {
                buf.append(">");
                if (getReason() != null) {
                    buf.append("<reason>").append(getReason()).append("</reason>");
                }
                buf.append("</destroy>");
            }
            return buf.toString();
        }

    }
}