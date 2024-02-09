package com.ipvs.xmpp.client.params;

import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.TimeZone;

public class MUCInitialPresenceParams extends XmppParams {
    private String password;
    private History history;

    public String toXML() {
        StringBuilder buf = new StringBuilder();
        buf.append("<x xmlns=\"http://jabber.org/protocol/muc\">");
        if (getPassword() != null) {
            buf.append("<password>").append(getPassword()).append("</password>");
        }
        if (getHistory() != null) {
            buf.append(getHistory().toXML());
        }
        buf.append("</x>");
        return buf.toString();
    }

    public History getHistory() {
        return history;
    }

    public String getPassword() {
        return password;
    }

    public void setHistory(History history) {
        this.history = history;
    }

    public void setPassword(String password) {
        this.password = password;
    }

    public static class History {
        private int maxChars = -1;
        private int maxStanzas = -1;
        private int seconds = -1;
        private Date since;

        public int getMaxChars() {
            return maxChars;
        }

        public int getMaxStanzas() {
            return maxStanzas;
        }

        public int getSeconds() {
            return seconds;
        }

        public Date getSince() {
            return since;
        }

        public void setMaxChars(int maxChars) {
            this.maxChars = maxChars;
        }

        public void setMaxStanzas(int maxStanzas) {
            this.maxStanzas = maxStanzas;
        }

        public void setSeconds(int seconds) {
            this.seconds = seconds;
        }

        public void setSince(Date since) {
            this.since = since;
        }

        public String toXML() {
            StringBuilder buf = new StringBuilder();
            buf.append("<history");
            if (getMaxChars() != -1) {
                buf.append(" maxchars=\"").append(getMaxChars()).append("\"");
            }
            if (getMaxStanzas() != -1) {
                buf.append(" maxstanzas=\"").append(getMaxStanzas()).append("\"");
            }
            if (getSeconds() != -1) {
                buf.append(" seconds=\"").append(getSeconds()).append("\"");
            }
            if (getSince() != null) {
                SimpleDateFormat utcFormat = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS'Z'");
                utcFormat.setTimeZone(TimeZone.getTimeZone("UTC"));
                buf.append(" since=\"").append(utcFormat.format(getSince())).append("\"");
            }
            buf.append("/>");
            return buf.toString();
        }
    }

}
