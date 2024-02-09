package com.ipvs.xmpp.client.smack.packets;

import org.jivesoftware.smack.packet.IQ;

public class BytestreamsLocalIP extends IQ {
    public static String NAMESPACE = "com.ipvs.ft.info";
    public static String ELEMENTNAME = "query";
    private String bytestreamsLocalIP;

    public String getBytestreamsLocalIP() {
        return bytestreamsLocalIP;
    }

    public void setBytestreamsLocalIP(String bytestreamsLocalIP) {
        this.bytestreamsLocalIP = bytestreamsLocalIP;
    }

    public String getNamespace() {
        return NAMESPACE;
    }

    public String getElementName() {
        return ELEMENTNAME;
    }

     public String getChildElementXML() {
        StringBuilder buf = new StringBuilder();
        buf.append("<").append(getElementName()).append(" ");
        buf.append(" xmlns=\"").append(getNamespace()).append("\" ");
        if (getBytestreamsLocalIP() != null)
            buf.append(" bytestreamsLocalIP=\"").append(getBytestreamsLocalIP()).append("\"");
        buf.append("/>");
        return buf.toString();
    }
}

