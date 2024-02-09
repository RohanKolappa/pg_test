package com.ipvs.xmpp.client.smack.packets;

import org.dom4j.Document;
import org.dom4j.io.Dom4jXMPPPacketReader;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.provider.IQProvider;
import org.xmlpull.v1.XmlPullParser;

public class ResponseIQProvider implements IQProvider {

    public IQ parseIQ(XmlPullParser parser) throws Exception {
        ResponseElement iq = new ResponseElement();
        parser.nextTag();
        Dom4jXMPPPacketReader dom4jBuilder = new Dom4jXMPPPacketReader();
        dom4jBuilder.setXPPParser(parser);
        Document doc = dom4jBuilder.parseDocument(ResponseElement.IPVSDBSERVICE);
        iq.setResponseEL(doc.getRootElement());
        return iq;
    }

}
