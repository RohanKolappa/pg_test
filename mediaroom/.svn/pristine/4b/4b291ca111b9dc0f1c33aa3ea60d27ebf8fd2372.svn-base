package com.ipvs.xmpp.client.smack.packets;

import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Namespace;
import org.dom4j.QName;
import org.dom4j.io.Dom4jXMPPPacketReader;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.provider.IQProvider;
import org.xmlpull.v1.XmlPullParser;

import com.ipvs.sessionservice.impl.SessionServiceConst;


public class SessionIQProvider implements IQProvider {
    private static String skipNameSpace=null;
    public IQ parseIQ(XmlPullParser parser) throws Exception {
        SessionIQ iq = new SessionIQ();
        Document doc = DocumentHelper.createDocument();
        QName qname = new QName("query", Namespace.get(IPVSMessageExtension.IPVS_SESSION_XMLNS));
        doc.add(DocumentHelper.createElement(qname));
        for (int i = 0; i < parser.getAttributeCount(); i++) {
            String attrName = parser.getAttributeName(i);
            String attrValue = parser.getAttributeValue(i);
            //it never gets in here ????????
            if (SessionServiceConst.SERVICE_VER.equals(attrName)) {
                doc.getRootElement().addAttribute(SessionServiceConst.SERVICE_VER, attrValue);
                //break;
            }
        }
        //parser.nextTag();
        Dom4jXMPPPacketReader dom4jBuilder = new Dom4jXMPPPacketReader();
        try {
            dom4jBuilder.setXPPParser(parser);
            doc = dom4jBuilder.parseDocument(doc, doc.getRootElement(), skipNameSpace);
            //todo: propagate the document....the api in MW -- need not parse the document again
            iq.setChildElementXML(doc.getRootElement().asXML());
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            dom4jBuilder.setXPPParser(null);
            dom4jBuilder = null;
        }
        return iq;
    }

}
