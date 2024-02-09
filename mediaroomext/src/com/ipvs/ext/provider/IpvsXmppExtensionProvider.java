package com.ipvs.ext.provider;

import org.jivesoftware.smack.packet.PacketExtension;
import org.jivesoftware.smack.provider.PacketExtensionProvider;
import org.xmlpull.v1.XmlPullParser;


public class IpvsXmppExtensionProvider extends IpvsXmppParser implements PacketExtensionProvider {

	public PacketExtension parseExtension(XmlPullParser parser) throws Exception {
		return new IpvsXmppExtension(this.parse(parser));
	}

}
