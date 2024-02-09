package com.ipvs.xmpp.client.smack.packets;

import org.jivesoftware.smack.packet.PacketExtension;
import org.jivesoftware.smack.provider.PacketExtensionProvider;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.dom2_builder.DOM2XmlPullBuilder2;

public class UpdateEventExtentionProvider implements PacketExtensionProvider {

    
    public PacketExtension parseExtension(XmlPullParser parser) throws Exception {
        UpdateEventElement event = new UpdateEventElement();
        //parser.nextTag();
        DOM2XmlPullBuilder2 domBuilder = new DOM2XmlPullBuilder2();
        event.setEventEL(domBuilder.parseSubTree(parser));
        return event;
    }
   
 
}
