package org.jivesoftware.openfire.user;

import java.util.Iterator;

import org.dom4j.Element;
import org.xmpp.packet.IQ;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IpvsUsersIQ extends IQ {
    public static final String name = "request";
    public static final String MEDIAROOM_NAMESPACE = "ipvs:mediaroom:ipvsdbservice";

    public IpvsUsersIQ() {
        super();
    }

    public void addAttribute(String name, String value) {
        this.element.addAttribute(name, value);
    }

    public Element addPubsubElement(String type, String nid, String jid, String serviceDomain, String action, String query, String whereXPath, int startfrom, int countToFetch, Element dataEl) {
        Element pubsub = setChildElement(name, MEDIAROOM_NAMESPACE);
        Element items = pubsub.addElement(type);
        items.addAttribute("nodeID", nid);
        items.addAttribute("subscribeJID", jid);
        items.addAttribute("domain", serviceDomain);
        items.addAttribute("listName", "");
        items.addAttribute("objectType", "");
        items.addAttribute("childXpath", "");
        items.addElement("whereXPath").setText(whereXPath);
        items.addElement("countToFetch").setText(-1 + "");
        if(dataEl !=null)
        	items.addElement("data").add(dataEl);
        return pubsub;
    }

    public Element setChildElement(String name, String namespace) {
        for (Iterator<?> i = element.elementIterator(); i.hasNext();) {
            element.remove((Element) i.next());
        }
        return element.addElement(name, namespace);
    }
}
