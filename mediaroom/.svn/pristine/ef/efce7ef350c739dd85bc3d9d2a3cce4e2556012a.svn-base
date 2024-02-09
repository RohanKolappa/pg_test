package com.ipvs.xmpp.client.smack.packets;

import java.io.ByteArrayInputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;

import org.jivesoftware.smack.packet.PacketExtension;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;


public class UpdateEventElement implements PacketExtension {
    private Element eventEL;
    
    public String getElementName() {
         return "updateEvent";
    }
    
    public String getNamespace() {
        return ResponseElement.IPVSDBSERVICE;
    }
    
    public String toXML() {
        return getEventXML();
    }

    public Element getEventEL() {
        return eventEL;
    }

    public void setEventEL(Element eventEL) {
         
        this.eventEL = eventEL;
    }

    public synchronized void setResponseXML(String xml) {
        try {
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = dbf.newDocumentBuilder();
            Document doc = db.parse(new ByteArrayInputStream(xml.getBytes()));
            setEventEL(doc.getDocumentElement());
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    public static void elementToString(Node node, StringBuffer buf,
    		PacketExtension packetExtension) {
    	IPVSMessageExtension.elementToString(node, buf, packetExtension, true);
    }
    
    public Element getEventDataEL() {
        return getEventDataEL(eventEL);
    }
    private Element getEventDataEL(Element node) {
        if(eventEL==null){
            return null;
        }
        NodeList children = node.getElementsByTagName("eventData");
        for (int i = 0; i < children.getLength(); i++) {
            return (Element)children.item(i);
        }            
        return null;
    }    
     
    public String getEventXML() {
        StringBuffer buf = new StringBuffer();
        elementToString(eventEL, buf, this);
        return buf.toString();
    }
    public String getgetEventDataXML() {
        StringBuffer buf = new StringBuffer();
        elementToString(getEventDataEL(eventEL), buf, this);
        return buf.toString();
    }
   
}
