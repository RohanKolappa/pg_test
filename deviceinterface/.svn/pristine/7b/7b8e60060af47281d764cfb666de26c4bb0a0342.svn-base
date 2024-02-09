package com.barco.device.ext.provider;

import org.jivesoftware.smack.packet.PacketExtension;


public class BarcoXmppExtension implements PacketExtension {
	public static final String BARCO_EXTENSION_ELEMENTNAME = "x";

	String xml;
	public BarcoXmppExtension(String xml) {
		this.xml = xml;
	}
	public String toXML() {
		return this.xml;
	}
	public String getElementName() {
		return BARCO_EXTENSION_ELEMENTNAME;
	}
	public String getNamespace() {
		return BarcoXmppParser.BARCONS;
	}	
}
