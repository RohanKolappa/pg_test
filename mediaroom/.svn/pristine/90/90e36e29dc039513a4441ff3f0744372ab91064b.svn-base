package com.ipvs.xmpp.client.smack.packets;

import se.su.it.smack.pubsub.elements.PubSubElement;

public class IPVSGenericElement extends PubSubElement {

    private String xmlstr;
	public String getName() {
		return "create"; 		
	}
	public  void setXML( String payload) {
		xmlstr = payload;
		return; 		
	}
	public String toXML() {
		if (xmlstr.isEmpty())
			return super.toXML();
		else 
			return xmlstr;
	}
	public IPVSGenericElement(String node) { super(node); }
	public IPVSGenericElement() { super(); }
	

}
