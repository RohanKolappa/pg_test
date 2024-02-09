package com.ipvs.xmpp.client.smack.packets;

import org.jivesoftware.smack.packet.PacketExtension;
import org.jivesoftware.smack.provider.PacketExtensionProvider;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.dom2_builder.DOM2XmlPullBuilder2;

import com.ipvs.client.MRClientAgentUtils;

public class DeprecatedAgentMessageExtensionProvider implements PacketExtensionProvider {

    public PacketExtension parseExtension(XmlPullParser parser) throws Exception {
        IPVSMessageExtension agentMessage = new IPVSMessageExtension(
        		IPVSMessageExtension.DEPRECATED_IPVS_AGENTMESSAGE_XMLNS);
        DOM2XmlPullBuilder2 domBuilder = new DOM2XmlPullBuilder2();
		StringBuffer buf = new StringBuffer();
		IPVSMessageExtension.elementToString(domBuilder.parseSubTree(parser), buf, agentMessage, false);
		agentMessage.setXML(MRClientAgentUtils.convertLegacyAgentmessageToLatestFormat(buf.toString(), ""));
        return agentMessage;
    }
   
 
}
