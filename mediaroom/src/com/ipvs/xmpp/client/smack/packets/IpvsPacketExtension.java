package com.ipvs.xmpp.client.smack.packets;

import org.jivesoftware.smack.packet.PacketExtension;

import com.ipvs.xmpp.client.params.XmppParams;

public class IpvsPacketExtension  implements PacketExtension {
    public static final String IPVS_AGENTMESSAGE_XMLNS = "com.barco.agentmessage";
    public static final String IPVS_SESSION_XMLNS = "com.ipvs.session";
    public static final String IPVS_EXTENSION_ELEMENTNAME = "x";

    private XmppParams params=null;
    public IpvsPacketExtension(){
        params= new XmppParams();
    }
    public IpvsPacketExtension(String namespace){
        params= new XmppParams();
        params.setNamespace(namespace);
    }    
    public IpvsPacketExtension(XmppParams params){
        this.params=params;
    }
    public XmppParams getParams() {
        return params;
    }

    public void setParams(XmppParams params) {
        this.params = params;
    }
    public String getElementName() {
        return params.getElementName();
    }
    public String getNamespace() {
        return params.getNamespace();
    }
    public void setElementName(String elementName) {
        params.setElementName(elementName); 
    }
    public void setNamespace(String namespace) {
        params.setNamespace(namespace);
    }
    
    public String toXML() {
       return params.toXML();
    }   
    public void setXML(String xml){
        params.setXML(xml);
    }
}
