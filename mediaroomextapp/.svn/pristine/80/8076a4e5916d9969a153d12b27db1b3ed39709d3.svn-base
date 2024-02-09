package com.barco.utils.mr;

import org.apache.commons.lang.StringEscapeUtils;
import org.jivesoftware.smack.packet.PacketExtension;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

public class IPVSMessageExtension1 implements PacketExtension {

    public static final String IPVS_AGENTMESSAGE_XMLNS = "com.ipvs.agentmessage";
    public static final String IPVS_SESSION_XMLNS = "com.ipvs.session";
    public static final String IPVS_EXTENSION_ELEMENTNAME = "x";

    String xml;
    String namespace;

    public IPVSMessageExtension1(String namespace) {
        this.namespace = namespace;
    }

    public String getElementName() {
        return IPVS_EXTENSION_ELEMENTNAME;
    }

    public String getNamespace() {
        return this.namespace;
    }

    public String toXML() {
        return this.xml;
    }

    public void setXML(String xml) {
        this.xml = xml;
    }

    public static void elementToString(Node node, StringBuffer buf, PacketExtension packetExtension, boolean addNamespace) {
        if (node == null) {
            return;
        }
        short type = node.getNodeType();
        switch (type) {
        case Node.CDATA_SECTION_NODE:
        case Node.TEXT_NODE:
            buf.append(StringEscapeUtils.escapeXml(node.getNodeValue()));
            break;
        case Node.ELEMENT_NODE:
            buf.append("<").append(node.getNodeName());
            NamedNodeMap attrs = node.getAttributes();
            for (int i = 0; i < attrs.getLength(); i++) {
                Node a = attrs.item(i);
                String name = a.getNodeName();
                String value = a.getNodeValue();
                if (name.equals("xmlns") && value.equals("jabber:client")) {
                    if (addNamespace) {
                        buf.append(" ").append(name).append("=\"").append(packetExtension.getNamespace()).append("\"");
                    }
                } else if (name.equals("xmlns:stream")) {
                    //skip
                } else {
                    if (!addNamespace && value != null && value.equalsIgnoreCase(packetExtension.getNamespace())) {
                        //neglect 
                    } else {
                        buf.append(" ").append(name).append("=\"").append(StringEscapeUtils.escapeXml(value)).append("\"");
                    }
                }
            }
            NodeList children = node.getChildNodes();
            if (children != null && children.getLength() > 0) {
                buf.append(">");
                for (int i = 0; i < children.getLength(); i++) {
                    elementToString(children.item(i), buf, packetExtension, addNamespace);
                }
                buf.append("</").append(node.getNodeName()).append(">");
            } else {
                buf.append("/>");
            }
        }
    }

}
