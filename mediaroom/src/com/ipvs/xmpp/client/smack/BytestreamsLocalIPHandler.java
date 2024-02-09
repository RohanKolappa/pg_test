package com.ipvs.xmpp.client.smack;

import java.net.InetAddress;
import java.net.UnknownHostException;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.QName;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.filter.AndFilter;
import org.jivesoftware.smack.filter.IQTypeFilter;
import org.jivesoftware.smack.filter.PacketTypeFilter;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.XMPPError;
import org.jivesoftware.smackx.filetransfer.OutgoingFileTransfer;

import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.xmpp.client.smack.packets.BytestreamsLocalIP;

public class BytestreamsLocalIPHandler {
    private XMPPI connection = null;

    public boolean isFileTransferToLocalIP(String jid) throws Exception {
        boolean IsDestLocalhost = false;
        String localhost = discoverLocalIP();
        BytestreamsLocalIP query = new BytestreamsLocalIP();
        query.setType(IQ.Type.GET);
        query.setTo(jid);
        query.setFrom(connection.getXMPPUserJid());
        String answer = connection.iq_request("BytestreamsLocalIP", query, OutgoingFileTransfer.getResponseTimeout());
        if (answer == null) {
            throw new Exception("No response from server.");
        }
        Document doc = MRXMLUtils.stringToDocument(answer);
        Element root = doc.getRootElement();
        String type = root.attributeValue("type");
        if (type.equalsIgnoreCase("error")) {
            throw new Exception("No response response from server.");
        }
        Element queryResult = root.element(new QName("query", Namespace.get(BytestreamsLocalIP.NAMESPACE)));
        if (queryResult == null) {
            throw new Exception("Invalid response from server.");
        }
        String remotelocalhost = MRXMLUtils.getAttributeValue(queryResult, "bytestreamsLocalIP", "").trim();
        if (remotelocalhost.equals(localhost)) {
            IsDestLocalhost = true;
        }
        return IsDestLocalhost;
    }

    public BytestreamsLocalIPHandler(XMPPI connection) {
        this.connection = connection;
        init();
    }

    public String discoverLocalIP() throws UnknownHostException {
        return InetAddress.getLocalHost().getHostAddress();
    }

    public void init() {
        AndFilter filter = new AndFilter(new IQTypeFilter(IQ.Type.GET), new PacketTypeFilter(BytestreamsLocalIP.class));
        PacketListener packetListener = new PacketListener() {
            public void processPacket(Packet packet) {
                BytestreamsLocalIP bytestreamsLocalIP = (BytestreamsLocalIP) packet;
                if (bytestreamsLocalIP != null && bytestreamsLocalIP.getType() == IQ.Type.GET) {
                    String localIP = null;
                    try {
                        localIP = discoverLocalIP();
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                    if (localIP == null) {
                        bytestreamsLocalIP.setType(IQ.Type.ERROR);
                        bytestreamsLocalIP.setError(new XMPPError(XMPPError.Condition.item_not_found));
                    } else {
                        bytestreamsLocalIP.setType(IQ.Type.RESULT);
                        bytestreamsLocalIP.setBytestreamsLocalIP(localIP);
                    }
                    try {
                        bytestreamsLocalIP.setTo(bytestreamsLocalIP.getFrom());
                        bytestreamsLocalIP.setFrom(connection.getXMPPUserJid());
                        //MRLog.println("bytestreamsLocalIP response " + bytestreamsLocalIP.toXML());
                        connection.sendPacket(bytestreamsLocalIP);
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

            }
        };
        connection.addPacketListener(packetListener, filter);

    }

}
