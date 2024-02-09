package com.ipvsserv.xmpp.session;

import java.io.StringReader;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.XPPPacketReader;
import org.xmlpull.v1.XmlPullParserException;
import org.xmlpull.v1.XmlPullParserFactory;
import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;
import org.xmpp.packet.Packet;
import org.xmpp.packet.Presence;
import org.xmpp.packet.Roster;

public class IPVSPacketParser {
    private XmlPullParserFactory factory = null;
    private XPPPacketReader reader = null;
    private boolean shutdown = false;
    
    private static IPVSPacketParser parser=null;
    public static synchronized IPVSPacketParser getInstance() throws XmlPullParserException {
        if(parser==null){
            parser = new IPVSPacketParser();
        }
        return parser;
    }
    public boolean isShutdown() {
        return shutdown;
    }

    public void setShutdown(boolean shutdown) {
        this.shutdown = shutdown;
    }

    public IPVSPacketParser() throws XmlPullParserException {
        factory = XmlPullParserFactory.newInstance();
        reader = new XPPPacketReader();
        reader.setXPPFactory(factory);
    }

    public synchronized Document stringToDocument(String packetStr) throws Exception {
        return reader.read(new StringReader(packetStr));
    }

    public Packet parse(String packetStr) throws Exception {
        if (shutdown) {
            throw new Exception("IPVSPacketParser shutdown");
        }

        Document doc = stringToDocument(packetStr);
        Element root = doc.getRootElement();

        if (root == null) {
            // Stop reading the stream since the server has sent an end of stream element and
            // probably closed the connection
            return null;
        }

        Packet packet;
        String tag = root.getName();
        if ("message".equals(tag)) {
            packet = new Message(root);
        } else if ("presence".equals(tag)) {
            packet = new Presence(root);
        } else if ("iq".equals(tag)) {
            packet = getIQ(root);
        } else {
            throw new XmlPullParserException("Unknown packet type was read: " + tag);
        }
        // Request the component to process the received packet
        //component.processPacket(packet);
        return packet;
    }

    private IQ getIQ(Element doc) {
        Element query = doc.element("query");
        if (query != null && "jabber:iq:roster".equals(query.getNamespaceURI())) {
            return new Roster(doc);
        } else {
            return new IQ(doc);
        }
    }
}
