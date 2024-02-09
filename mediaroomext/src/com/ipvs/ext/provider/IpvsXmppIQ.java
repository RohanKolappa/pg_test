package com.ipvs.ext.provider;

import org.jivesoftware.smack.packet.IQ;

public class IpvsXmppIQ extends IQ {
	String xml;
	public IpvsXmppIQ(String xml) {
		this.xml = xml;
	}
	public String getChildElementXML() {
		return xml;
	}	
}
