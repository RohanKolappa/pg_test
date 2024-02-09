package com.ipvs.ext.provider;

import org.jivesoftware.smack.packet.PacketExtension;


public class IpvsXmppExtension implements PacketExtension {
	// public static String IPVSMESSAGE = "ipvsmessage";
    public static final String IPVS_EXTENSION_ELEMENTNAME = "x";

	String xml;
	public IpvsXmppExtension(String xml) {
		this.xml = xml;
	}
	public String toXML() {
		return this.xml;
	}
	public String getElementName() {
		return IPVS_EXTENSION_ELEMENTNAME;
	}
	public String getNamespace() {
		return IpvsXmppParser.IPVSNS;
	}	
}
