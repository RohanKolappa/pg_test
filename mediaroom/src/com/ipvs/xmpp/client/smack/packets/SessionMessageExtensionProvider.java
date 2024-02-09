package com.ipvs.xmpp.client.smack.packets;

import org.jivesoftware.smack.packet.PacketExtension;
import org.jivesoftware.smack.provider.PacketExtensionProvider;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.dom2_builder.DOM2XmlPullBuilder2;

public class SessionMessageExtensionProvider implements PacketExtensionProvider {

    public PacketExtension parseExtension(XmlPullParser parser) throws Exception {
    	IPVSMessageExtension sessionMessage = new IPVSMessageExtension(
    			IPVSMessageExtension.IPVS_SESSION_XMLNS);
        //parser.nextTag();
        DOM2XmlPullBuilder2 domBuilder = new DOM2XmlPullBuilder2();
		StringBuffer buf = new StringBuffer();
		IPVSMessageExtension.elementToString(domBuilder.parseSubTree(parser), buf, sessionMessage, true);
		sessionMessage.setXML(buf.toString());
        return sessionMessage;
    }
   
 
}
