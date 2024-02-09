package com.ipvs.xmpp.client.smack;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.QName;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.filter.PacketTypeFilter;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.XMPPError;
import org.jivesoftware.smackx.NodeInformationProvider;
import org.jivesoftware.smackx.packet.DiscoverInfo;
import org.jivesoftware.smackx.packet.DiscoverItems;

import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRXMLUtils;

public class IPVSServiceDiscoveryManager {
    private static final int MAX_DISCOVERY_RETRY = 3;

    private static String identityName = "Smack";
    private static String identityType = "pc";

    private XMPPI connection;
    private final List<String> features = new ArrayList<String>();
    private Map<String, NodeInformationProvider> nodeInformationProviders = new ConcurrentHashMap<String, NodeInformationProvider>();

    public IPVSServiceDiscoveryManager(XMPPI connection) {
        this.connection = connection;
        init();
    }

    public static String getIdentityName() {
        return identityName;
    }

    public static void setIdentityName(String name) {
        identityName = name;
    }

    public static String getIdentityType() {
        return identityType;
    }

    public static void setIdentityType(String type) {
        identityType = type;
    }

    private void sendPacket(Packet packet) {
        try {
            connection.sendPacket(packet);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    private void init() {
        // Listen for disco#items requests and answer with an empty result        
        PacketFilter packetFilter = new PacketTypeFilter(DiscoverItems.class);
        PacketListener packetListener = new PacketListener() {
            public void processPacket(Packet packet) {
                DiscoverItems discoverItems = (DiscoverItems) packet;
                // Send back the items defined in the client if the request is of type GET
                if (discoverItems != null && discoverItems.getType() == IQ.Type.GET) {
                    DiscoverItems response = new DiscoverItems();
                    response.setType(IQ.Type.RESULT);
                    response.setTo(discoverItems.getFrom());
                    response.setPacketID(discoverItems.getPacketID());
                    response.setNode(discoverItems.getNode());

                    // Add the defined items related to the requested node. Look for 
                    // the NodeInformationProvider associated with the requested node.  
                    NodeInformationProvider nodeInformationProvider = getNodeInformationProvider(discoverItems.getNode());
                    if (nodeInformationProvider != null) {
                        // Specified node was found
                        List<DiscoverItems.Item> items = nodeInformationProvider.getNodeItems();
                        if (items != null) {
                            for (DiscoverItems.Item item : items) {
                                response.addItem(item);
                            }
                        }
                    } else if (discoverItems.getNode() != null) {
                        // Return <item-not-found/> error since client doesn't contain
                        // the specified node
                        response.setType(IQ.Type.ERROR);
                        response.setError(new XMPPError(XMPPError.Condition.item_not_found));
                    }
                    sendPacket(response);
                }
            }
        };
        connection.addPacketListener(packetListener, packetFilter);

        // Listen for disco#info requests and answer the client's supported features 
        // To add a new feature as supported use the #addFeature message        
        packetFilter = new PacketTypeFilter(DiscoverInfo.class);
        packetListener = new PacketListener() {
            public void processPacket(Packet packet) {
                DiscoverInfo discoverInfo = (DiscoverInfo) packet;
                // Answer the client's supported features if the request is of the GET type
                if (discoverInfo != null && discoverInfo.getType() == IQ.Type.GET) {
                    DiscoverInfo response = new DiscoverInfo();
                    response.setType(IQ.Type.RESULT);
                    response.setTo(discoverInfo.getFrom());
                    response.setPacketID(discoverInfo.getPacketID());
                    response.setNode(discoverInfo.getNode());
                    // Add the client's identity and features only if "node" is null
                    if (discoverInfo.getNode() == null) {
                        // Set this client identity
                        DiscoverInfo.Identity identity = new DiscoverInfo.Identity("client", getIdentityName());
                        identity.setType(getIdentityType());
                        response.addIdentity(identity);
                        // Add the registered features to the response
                        synchronized (features) {
                            for (Iterator<String> it = getFeatures(); it.hasNext();) {
                                response.addFeature(it.next());
                            }
                        }
                    } else {
                        // Disco#info was sent to a node. Check if we have information of the
                        // specified node
                        NodeInformationProvider nodeInformationProvider = getNodeInformationProvider(discoverInfo.getNode());
                        if (nodeInformationProvider != null) {
                            // Node was found. Add node features
                            List<String> features = nodeInformationProvider.getNodeFeatures();
                            if (features != null) {
                                for (String feature : features) {
                                    response.addFeature(feature);
                                }
                            }
                            // Add node identities
                            List<DiscoverInfo.Identity> identities = nodeInformationProvider.getNodeIdentities();
                            if (identities != null) {
                                for (DiscoverInfo.Identity identity : identities) {
                                    response.addIdentity(identity);
                                }
                            }
                        } else {
                            // Return <item-not-found/> error since specified node was not found
                            response.setType(IQ.Type.ERROR);
                            response.setError(new XMPPError(XMPPError.Condition.item_not_found));
                        }
                    }
                    sendPacket(response);
                }
            }
        };
        connection.addPacketListener(packetListener, packetFilter);
    }

    private NodeInformationProvider getNodeInformationProvider(String node) {
        if (node == null) {
            return null;
        }
        return nodeInformationProviders.get(node);
    }

    public void setNodeInformationProvider(String node, NodeInformationProvider listener) {
        nodeInformationProviders.put(node, listener);
    }

    public void removeNodeInformationProvider(String node) {
        nodeInformationProviders.remove(node);
    }

    public Iterator<String> getFeatures() {
        synchronized (features) {
            return Collections.unmodifiableList(new ArrayList<String>(features)).iterator();
        }
    }

    public void addFeature(String feature) {
        synchronized (features) {
            features.add(feature);
        }
    }

    public void removeFeature(String feature) {
        synchronized (features) {
            features.remove(feature);
        }
    }

    public boolean includesFeature(String feature) {
        synchronized (features) {
            return features.contains(feature);
        }
    }

    public Document discoverInfo(String entityID) throws Exception {
        return discoverInfo(entityID, null);
    }

    public Document discoverInfo(String entityID, String node) throws Exception {
        // Discover the entity's info
        DiscoverInfo disco = new DiscoverInfo();
        disco.setType(IQ.Type.GET);
        disco.setTo(entityID);
        disco.setNode(node);

        // Create a packet collector to listen for a response.
        String answer = connection.iq_request("discoverInfo", disco, MRXMLDBUtils.TIME_OUT);
        if (answer == null) {
            throw new Exception("No response from server.");
        }
        Document doc = MRXMLUtils.stringToDocument(answer);
        Element root = doc.getRootElement();
        String type = root.attributeValue("type");
        if (type.equalsIgnoreCase("error")) {
            throw new Exception("No response or invalid response from server.");
        }
        return doc;
    }

    public Document discoverItems(String entityID) throws Exception {
        return discoverItems(entityID, null);
    }

    public Document discoverItems(String entityID, String node) throws Exception {
        // Discover the entity's items
        DiscoverItems disco = new DiscoverItems();
        disco.setType(IQ.Type.GET);
        disco.setTo(entityID);
        disco.setNode(node);

        // Create a packet collector to listen for a response.
        disco.setFrom(connection.getXMPPUserJid());
        String answer = null;
        int counter = 0;
        while( (counter < MAX_DISCOVERY_RETRY) && (answer == null) ){
           counter++;
           answer = connection.iq_request("discoverItems", disco, MRXMLDBUtils.TIME_OUT);
        }

        if (answer == null) {
            throw new Exception("No discoverItems response from server.");
        }
       
        Document doc = MRXMLUtils.stringToDocument(answer);
        Element root = doc.getRootElement();
        String type = root.attributeValue("type");
        if (type.equalsIgnoreCase("error")) {
            throw new Exception("No discoverItems response or invalid discoverItems response from server.");
        }
        return doc;
    }

    public boolean canPublishItems(String entityID) throws Exception {
        Document doc = discoverInfo(entityID);
        Element root = doc.getRootElement();
        String type = root.attributeValue("type");
        if (type.equalsIgnoreCase("error")) {
            throw new Exception("No response or invalid response from server.");
        }
        Element query = root.element(new QName("query", Namespace.get("http://jabber.org/protocol/disco#info")));
        if (query == null) {
            throw new Exception("No response or invalid response from server.");
        }
        if (root.asXML().indexOf("http://jabber.org/protocol/disco#publish") != -1) {
            return true;
        }
        return false;
    }

    public void publishItems(String entityID, DiscoverItems discoverItems) throws Exception {
        publishItems(entityID, null, discoverItems);
    }

    public void publishItems(String entityID, String node, DiscoverItems discoverItems) throws Exception {
        discoverItems.setType(IQ.Type.SET);
        discoverItems.setTo(entityID);
        discoverItems.setNode(node);

        String answer = connection.iq_request("publishItems", discoverItems, MRXMLDBUtils.TIME_OUT);
        if (answer == null) {
            throw new Exception("No publishItems response from server.");
        }
    }
}