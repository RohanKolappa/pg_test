package com.barco.device.ext.provider;

import org.jivesoftware.smack.packet.PacketExtension;
import org.jivesoftware.smack.provider.PacketExtensionProvider;
import org.xmlpull.v1.XmlPullParser;


public class BarcoXmppExtensionProvider extends BarcoXmppParser implements PacketExtensionProvider {

	public PacketExtension parseExtension(XmlPullParser parser) throws Exception {
		return new BarcoXmppExtension(this.parse(parser));
	}

}
