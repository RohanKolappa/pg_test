package com.ipvs.xmpp.client.component;

import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadFactory;

import org.jivesoftware.smack.ConnectionListener;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.PacketReader;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.packet.Authentication;
import org.jivesoftware.smack.packet.Bind;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.Registration;
import org.jivesoftware.smack.packet.RosterPacket;
import org.jivesoftware.smack.packet.StreamError;
import org.jivesoftware.smack.packet.XMPPError;
import org.jivesoftware.smack.provider.IQProvider;
import org.jivesoftware.smack.provider.ProviderManager;
import org.jivesoftware.smack.util.PacketParserUtils;
import org.xmlpull.mxp1.MXParser;
import org.xmlpull.v1.XmlPullParser;
import org.xmlpull.v1.XmlPullParserException;

import com.ipvsserv.xmpp.session.XmppSession;

public class PacketReaderImpl implements PacketReader {
    private static int counter = 0;
    private static List<MXParser> parsers = new ArrayList<MXParser>();

    private static MXParser checkoutMXParser() {
        synchronized (parsers) {
            if (parsers.size() > 0) {
                counter--;
                return parsers.remove(0);
            }
        }
        MXParser parser = new MXParser();
        try {
            parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, true);
        } catch (XmlPullParserException e) {
            e.printStackTrace();
        }
        counter--;
        return parser;
    }

    private static void checkInMXParser(MXParser parser) {
        synchronized (parsers) {
            counter++;
            if (counter < 50) {
                parsers.add(parser);
            } else {
                parser = null;
            }
        }
    }

    /////////////////////////  packet processing ////////////////
    public void convertPacket(org.xmpp.packet.Packet xmpppacket) {
        if (xmpppacket != null) {
            convertPacket(xmpppacket.toXML());
        }
    }

    public void convertPacket(String xmpppacket) {
        if (xmpppacket == null)
            return;
        MXParser parser = checkoutMXParser();
        try {
            //parser.setFeature(XmlPullParser.FEATURE_PROCESS_NAMESPACES, true);
            Reader reader = new StringReader(xmpppacket);
            parser.setInput(reader);
            parsePackets(parser);
            // parsePackets(this.);
        } catch (XmlPullParserException xppe) {
            xppe.printStackTrace();
            return;
        } finally {
            checkInMXParser(parser);
        }
    }

    ///////////////// parse //////////////////

    private ExecutorService listenerExecutor;

    private XmppSession connection;
    //private XmlPullParser parser;

    private Collection<PacketCollector> collectors = new ConcurrentLinkedQueue<PacketCollector>();
    protected final Map<PacketListener, ListenerWrapper> listeners = new ConcurrentHashMap<PacketListener, ListenerWrapper>();
    protected final Collection<ConnectionListener> connectionListeners = new CopyOnWriteArrayList<ConnectionListener>();

    protected PacketReaderImpl(final XmppSession connection) {
        this.connection = connection;
        this.init();
    }

    //////////////
    private void parsePackets(MXParser parser) {
        try {
            int eventType = parser.getEventType();
            do {
                if (eventType == XmlPullParser.START_TAG) {
                    if (parser.getName().equals("message")) {
                        processPacket(PacketParserUtils.parseMessage(parser));
                    } else if (parser.getName().equals("iq")) {
                        processPacket(parseIQ(parser));
                    } else if (parser.getName().equals("presence")) {
                        processPacket(PacketParserUtils.parsePresence(parser));
                    }
                    // We found an opening stream. Record information about it, then notify
                    // the connectionID lock so that the packet reader startup can finish.
                    else if (parser.getName().equals("stream")) {

                    } else if (parser.getName().equals("error")) {
                        throw new XMPPException(parseStreamError(parser));
                    } else if (parser.getName().equals("features")) {
                        parseFeatures(parser);
                    } else if (parser.getName().equals("proceed")) {

                    } else if (parser.getName().equals("failure")) {
                        String namespace = parser.getNamespace(null);
                        if ("urn:ietf:params:xml:ns:xmpp-tls".equals(namespace)) {
                            // TLS negotiation has failed. The server will close the connection
                            throw new Exception("TLS negotiation has failed");
                        } else if ("http://jabber.org/protocol/compress".equals(namespace)) {
                        } else {
                        }
                    } else if (parser.getName().equals("challenge")) {
                    } else if (parser.getName().equals("success")) {
                    } else if (parser.getName().equals("compressed")) {
                    }
                } else if (eventType == XmlPullParser.END_TAG) {
                }
                eventType = parser.next();
                //} while (!done && eventType != XmlPullParser.END_DOCUMENT && thread == readerThread);
            } while (eventType != XmlPullParser.END_DOCUMENT);
        } catch (Exception e) {
        }
    }

    private void processPacket(Packet packet) {
        if (packet == null) {
            return;
        }

        // Loop through all collectors and notify the appropriate ones.
        for (PacketCollector collector : collectors) {
            collector.processPacket(packet);
        }

        // Deliver the incoming packet to listeners.
        listenerExecutor.submit(new ListenerNotification(packet));
    }

    private StreamError parseStreamError(XmlPullParser parser) throws IOException, XmlPullParserException {
        StreamError streamError = null;
        boolean done = false;
        while (!done) {
            int eventType = parser.next();

            if (eventType == XmlPullParser.START_TAG) {
                streamError = new StreamError(parser.getName());
            } else if (eventType == XmlPullParser.END_TAG) {
                if (parser.getName().equals("error")) {
                    done = true;
                }
            }
        }
        return streamError;
    }

    private void parseFeatures(XmlPullParser parser) throws Exception {
    }

    private IQ parseIQ(XmlPullParser parser) throws Exception {
        IQ iqPacket = null;

        String id = parser.getAttributeValue("", "id");
        String to = parser.getAttributeValue("", "to");
        String from = parser.getAttributeValue("", "from");
        IQ.Type type = IQ.Type.fromString(parser.getAttributeValue("", "type"));
        XMPPError error = null;

        boolean done = false;
        while (!done) {
            int eventType = parser.next();

            if (eventType == XmlPullParser.START_TAG) {
                String elementName = parser.getName();
                String namespace = parser.getNamespace();
                if (elementName.equals("error")) {
                    error = PacketParserUtils.parseError(parser);
                } else if (elementName.equals("query") && namespace.equals("jabber:iq:auth")) {
                    iqPacket = parseAuthentication(parser);
                } else if (elementName.equals("query") && namespace.equals("jabber:iq:roster")) {
                    iqPacket = parseRoster(parser);
                } else if (elementName.equals("query") && namespace.equals("jabber:iq:register")) {
                    iqPacket = parseRegistration(parser);
                } else if (elementName.equals("bind") && namespace.equals("urn:ietf:params:xml:ns:xmpp-bind")) {
                    iqPacket = parseResourceBinding(parser);
                }
                // Otherwise, see if there is a registered provider for
                // this element name and namespace.
                else {
                    Object provider = ProviderManager.getInstance().getIQProvider(elementName, namespace);
                    if (provider != null) {
                        if (provider instanceof IQProvider) {
                            iqPacket = ((IQProvider) provider).parseIQ(parser);
                        } else if (provider instanceof Class) {
                            iqPacket = (IQ) PacketParserUtils.parseWithIntrospection(elementName, (Class) provider, parser);
                        }
                    }
                }
            } else if (eventType == XmlPullParser.END_TAG) {
                if (parser.getName().equals("iq")) {
                    done = true;
                }
            }
        }
        // Decide what to do when an IQ packet was not understood
        if (iqPacket == null) {
            if (IQ.Type.GET == type || IQ.Type.SET == type) {
                // If the IQ stanza is of type "get" or "set" containing a child element
                // qualified by a namespace it does not understand, then answer an IQ of
                // type "error" with code 501 ("feature-not-implemented")
                iqPacket = new IQ() {
                    public String getChildElementXML() {
                        return null;
                    }
                };
                iqPacket.setPacketID(id);
                iqPacket.setTo(from);
                iqPacket.setFrom(to);
                iqPacket.setType(IQ.Type.ERROR);
                iqPacket.setError(new XMPPError(XMPPError.Condition.feature_not_implemented));
                //connection.sendPacket(iqPacket);
                return iqPacket;
            } else {
                // If an IQ packet wasn't created above, create an empty IQ packet.
                iqPacket = new IQ() {
                    public String getChildElementXML() {
                        return null;
                    }
                };
            }
        }

        // Set basic values on the iq packet.
        iqPacket.setPacketID(id);
        iqPacket.setTo(to);
        iqPacket.setFrom(from);
        iqPacket.setType(type);
        iqPacket.setError(error);

        return iqPacket;
    }

    private Authentication parseAuthentication(XmlPullParser parser) throws Exception {
        Authentication authentication = new Authentication();
        boolean done = false;
        while (!done) {
            int eventType = parser.next();
            if (eventType == XmlPullParser.START_TAG) {
                if (parser.getName().equals("username")) {
                    authentication.setUsername(parser.nextText());
                } else if (parser.getName().equals("password")) {
                    authentication.setPassword(parser.nextText());
                } else if (parser.getName().equals("digest")) {
                    authentication.setDigest(parser.nextText());
                } else if (parser.getName().equals("resource")) {
                    authentication.setResource(parser.nextText());
                }
            } else if (eventType == XmlPullParser.END_TAG) {
                if (parser.getName().equals("query")) {
                    done = true;
                }
            }
        }
        return authentication;
    }

    private RosterPacket parseRoster(XmlPullParser parser) throws Exception {
        RosterPacket roster = new RosterPacket();
        boolean done = false;
        RosterPacket.Item item = null;
        while (!done) {
            int eventType = parser.next();
            if (eventType == XmlPullParser.START_TAG) {
                if (parser.getName().equals("item")) {
                    String jid = parser.getAttributeValue("", "jid");
                    String name = parser.getAttributeValue("", "name");
                    // Create packet.
                    item = new RosterPacket.Item(jid, name);
                    // Set status.
                    String ask = parser.getAttributeValue("", "ask");
                    RosterPacket.ItemStatus status = RosterPacket.ItemStatus.fromString(ask);
                    item.setItemStatus(status);
                    // Set type.
                    String subscription = parser.getAttributeValue("", "subscription");
                    RosterPacket.ItemType type = RosterPacket.ItemType.valueOf(subscription);
                    item.setItemType(type);
                }
                if (parser.getName().equals("group") && item != null) {
                    item.addGroupName(parser.nextText());
                }
            } else if (eventType == XmlPullParser.END_TAG) {
                if (parser.getName().equals("item")) {
                    roster.addRosterItem(item);
                }
                if (parser.getName().equals("query")) {
                    done = true;
                }
            }
        }
        return roster;
    }

    private Registration parseRegistration(XmlPullParser parser) throws Exception {
        Registration registration = new Registration();
        Map<String, String> fields = null;
        boolean done = false;
        while (!done) {
            int eventType = parser.next();
            if (eventType == XmlPullParser.START_TAG) {
                // Any element that's in the jabber:iq:register namespace,
                // attempt to parse it if it's in the form <name>value</name>.
                if (parser.getNamespace().equals("jabber:iq:register")) {
                    String name = parser.getName();
                    String value = "";
                    if (fields == null) {
                        fields = new HashMap<String, String>();
                    }

                    if (parser.next() == XmlPullParser.TEXT) {
                        value = parser.getText();
                    }
                    // Ignore instructions, but anything else should be added to the map.
                    if (!name.equals("instructions")) {
                        fields.put(name, value);
                    } else {
                        registration.setInstructions(value);
                    }
                }
                // Otherwise, it must be a packet extension.
                else {
                    registration.addExtension(PacketParserUtils.parsePacketExtension(parser.getName(), parser.getNamespace(), parser));
                }
            } else if (eventType == XmlPullParser.END_TAG) {
                if (parser.getName().equals("query")) {
                    done = true;
                }
            }
        }
        registration.setAttributes(fields);
        return registration;
    }

    private Bind parseResourceBinding(XmlPullParser parser) throws IOException, XmlPullParserException {
        Bind bind = new Bind();
        boolean done = false;
        while (!done) {
            int eventType = parser.next();
            if (eventType == XmlPullParser.START_TAG) {
                if (parser.getName().equals("resource")) {
                    bind.setResource(parser.nextText());
                } else if (parser.getName().equals("jid")) {
                    bind.setJid(parser.nextText());
                }
            } else if (eventType == XmlPullParser.END_TAG) {
                if (parser.getName().equals("bind")) {
                    done = true;
                }
            }
        }

        return bind;
    }

    private class ListenerNotification implements Runnable {
        private Packet packet;

        public ListenerNotification(Packet packet) {
            this.packet = packet;
        }

        public void run() {
            for (ListenerWrapper listenerWrapper : listeners.values()) {
                listenerWrapper.notifyListener(packet);
            }
        }
    }

    private static class ListenerWrapper {

        private PacketListener packetListener;
        private PacketFilter packetFilter;

        public ListenerWrapper(PacketListener packetListener, PacketFilter packetFilter) {
            this.packetListener = packetListener;
            this.packetFilter = packetFilter;
        }

        public void notifyListener(Packet packet) {
            if (packetFilter == null || packetFilter.accept(packet)) {
                packetListener.processPacket(packet);
            }
        }
    }

    /////////////////////////////
    protected void init() {
        // Create an executor to deliver incoming packets to listeners. We'll use a single
        // thread with an unbounded queue.
        listenerExecutor = Executors.newSingleThreadExecutor(new ThreadFactory() {

            public Thread newThread(Runnable runnable) {
                Thread thread = new Thread(runnable, "Smack Listener Processor (" + connection.connectionCounterValue + ")");
                thread.setDaemon(true);
                return thread;
            }
        });
    }

    public PacketCollector createPacketCollector(PacketFilter packetFilter) {
        PacketCollector collector = new PacketCollector(this, packetFilter);
        collectors.add(collector);
        // Add the collector to the list of active collector.
        return collector;
    }

    public void cancelPacketCollector(PacketCollector packetCollector) {
        collectors.remove(packetCollector);
    }

    public void addPacketListener(PacketListener packetListener, PacketFilter packetFilter) {
        ListenerWrapper wrapper = new ListenerWrapper(packetListener, packetFilter);
        listeners.put(packetListener, wrapper);
    }

    public void removePacketListener(PacketListener packetListener) {
        listeners.remove(packetListener);
    }

    @Override
    public void startup() throws XMPPException {

    }

    @Override
    public void cleanup() {

    }

    @Override
    public void shutdown() {

    }

}