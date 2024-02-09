package com.ipvs.xmpp.client.smack.packets;

import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.provider.IQProvider;
import org.xmlpull.v1.XmlPullParser;

public class BytestreamsLocalIPProvider implements IQProvider {

    public IQ parseIQ(XmlPullParser parser) throws Exception {
        BytestreamsLocalIP toReturn = new BytestreamsLocalIP();
        String localIP = parser.getAttributeValue("", "bytestreamsLocalIP");
        toReturn.setBytestreamsLocalIP(localIP);
        return toReturn;
    }


}
