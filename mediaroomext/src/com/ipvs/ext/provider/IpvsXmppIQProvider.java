package com.ipvs.ext.provider;

import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.provider.IQProvider;
import org.xmlpull.v1.XmlPullParser;


public class IpvsXmppIQProvider extends IpvsXmppParser implements IQProvider {

	public IQ parseIQ(XmlPullParser parser) throws Exception {
		return new IpvsXmppIQ(this.parse(parser));
	}
}
