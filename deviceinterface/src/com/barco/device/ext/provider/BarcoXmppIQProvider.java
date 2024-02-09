
package com.barco.device.ext.provider;

import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.provider.IQProvider;
import org.xmlpull.v1.XmlPullParser;

public class BarcoXmppIQProvider extends BarcoXmppParser implements IQProvider {

	@Override
	public IQ parseIQ(XmlPullParser parser) throws Exception {	
		
		return new BarcoXmppIQ(this.parse(parser));
	}

}