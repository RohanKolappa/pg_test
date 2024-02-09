package com.ipvs.xmpp.client.smack;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.QName;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.filter.AndFilter;
import org.jivesoftware.smack.filter.FromMatchesFilter;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.filter.PacketIDFilter;
import org.jivesoftware.smack.filter.PacketTypeFilter;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.packet.Presence;
import org.jivesoftware.smackx.Form;

import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.xmpp.client.params.MUCInitialPresenceParams;
import com.ipvs.xmpp.client.params.MUCOwnerParams;
import com.ipvs.xmpp.client.smack.packets.IpvsIQ;
import com.ipvs.xmpp.client.smack.packets.IpvsPacketExtension;

public class MultiUserChatImpl {
    private String room;
    private String subject;
    private String nickname = null;
    private boolean joined = false;
    private XMPPI session = null;

    public MultiUserChatImpl(XMPPI session, String room) {
        this.room = room;
        this.session = session;
    }

    public String toString() {
        return " " + room + " " + nickname + " joined " + joined;
    }

    //TODO: not used any more
    /* public void sendMessage(String text) throws Exception {
         System.out.println("MultiUserChatImpl sendMessage " +toString());
         Message message = new Message();
         message.setTo(room);
         message.setType(Message.Type.groupchat);
         message.setBody(text);
         session.sendPacket(message);
     }*/

    public synchronized void create(String nickname) throws Exception {
        //System.out.println("MultiUserChatImpl create " + toString());
        if (nickname == null || nickname.equals("")) {
            throw new IllegalArgumentException("Nickname must not be null or blank.");
        }
        // If we've already joined the room, leave it before joining under a new
        // nickname.
        if (joined) {
            throw new IllegalStateException("Creation failed - User already joined the room.");
        }
        // We create a room by sending a presence packet to room@service/nick
        // and signal support for MUC. The owner will be automatically logged into the room.
        Presence joinPresence = new Presence(Presence.Type.available);
        joinPresence.setTo(room + "/" + nickname);
        joinPresence.setFrom(session.getXMPPUserJid());
        // Indicate the the client supports MUC
        //joinPresence.addExtension(new MUCInitialPresence());
        IpvsPacketExtension extention = new IpvsPacketExtension();
        extention.setParams(new MUCInitialPresenceParams());
        joinPresence.addExtension(extention);

        // Wait for a presence packet back from the server.
        PacketFilter responseFilter = new AndFilter(new FromMatchesFilter(room + "/" + nickname), new PacketTypeFilter(Presence.class));
        PacketCollector response = session.createPacketCollector(responseFilter);
        //System.out.println(" MultiUserChatImpl create rquest"+ joinPresence.toXML());
        // Send create & join packet.
        session.sendPacket(joinPresence);
        // Wait up to a certain number of seconds for a reply.
        Presence presence = (Presence) response.nextResult(40 * 1000);
        //System.out.println(" MultiUserChatImpl create response "+ presence.toXML());
        // Stop queuing results
        response.cancel();

        if (presence == null) {
            throw new Exception("No response from server.");
        } else if (presence.getError() != null) {
            throw new Exception(presence.getError().toString());
        }
        // Whether the room existed before or was created, the user has joined the room
        this.nickname = nickname;
        joined = true;
        //userHasJoined();

        String xml = presence.toXML();
        Document doc = MRXMLUtils.stringToDocument(xml);
        Element elStatus = MRXMLUtils.getDescendentWithoutNamespace(doc.getRootElement(), "status");
        if (elStatus != null) {
            String code = MRXMLUtils.getAttributeValue(elStatus, "code", "").trim();
            if ("201".equals(code)) {
                // Room was created and the user has joined the room
                return;
            }
        }
        /*// Look for confirmation of room creation from the server
        MUCUser mucUser = getMUCUserExtension(presence);
        if (mucUser != null && mucUser.getStatus() != null) {
            if ("201".equals(mucUser.getStatus().getCode())) {
                // Room was created and the user has joined the room
                return;
            }
        }*/
        // We need to leave the room since it seems that the room already existed
        leave();
        throw new Exception("Creation failed - Missing acknowledge of room creation.");
    }

    /*    
    private MUCUser getMUCUserExtension(Packet packet) {
        System.out.println("MultiUserChatImpl getMUCUserExtension " +toString());
        if (packet != null) {
            // Get the MUC User extension
            return (MUCUser) packet.getExtension("x", "http://jabber.org/protocol/muc#user");
        }
        return null;
    }*/

    public void join(String nickname) throws Exception {
        //System.out.println("MultiUserChatImpl join " + toString());
        join(nickname, null, 40 * 1000);
    }

    //public synchronized void join(String nickname, String password, DiscussionHistory history, long timeout) throws Exception {
    public synchronized void join(String nickname, String password, long timeout) throws Exception {
        //System.out.println("MultiUserChatImpl join 2 " + toString());
        if (nickname == null || nickname.equals("")) {
            throw new IllegalArgumentException("Nickname must not be null or blank.");
        }
        // If we've already joined the room, leave it before joining under a new
        // nickname.
        if (joined) {
            leave();
        }
        // We join a room by sending a presence packet where the "to"
        // field is in the form "roomName@service/nickname"
        Presence joinPresence = new Presence(Presence.Type.available);
        joinPresence.setTo(room + "/" + nickname);
        joinPresence.setFrom(session.getXMPPUserJid());
        // Indicate the the client supports MUC
        //MUCInitialPresence mucInitialPresence = new MUCInitialPresence();
        MUCInitialPresenceParams mucInitialPresence = new MUCInitialPresenceParams();
        if (password != null) {
            mucInitialPresence.setPassword(password);
        }
        //if (history != null) {
        //   mucInitialPresence.setHistory(history.getMUCHistory());
        //}
        //joinPresence.addExtension(mucInitialPresence);
        IpvsPacketExtension extention = new IpvsPacketExtension();
        extention.setParams(mucInitialPresence);
        joinPresence.addExtension(extention);

        // Wait for a presence packet back from the server.
        PacketFilter responseFilter = new AndFilter(new FromMatchesFilter(room + "/" + nickname), new PacketTypeFilter(Presence.class));
        PacketCollector response = null;
        Presence presence;
        try {
            response = session.createPacketCollector(responseFilter);
            // Send join packet.
            session.sendPacket(joinPresence);
            // Wait up to a certain number of seconds for a reply.
            presence = (Presence) response.nextResult(timeout);
        } finally {
            // Stop queuing results
            if (response != null) {
                response.cancel();
            }
        }

        if (presence == null) {
            throw new Exception("No response from server.");
        } else if (presence.getError() != null) {
            throw new Exception(presence.getError().toString());
        }
        this.nickname = nickname;
        joined = true;
        userHasJoined();
    }

    public void destroy(String reason, String alternateJID) throws Exception {
        //System.out.println("MultiUserChatImpl destroy " + toString());
        // Create the reason for the room destruction

        MUCOwnerParams params = new MUCOwnerParams();
        MUCOwnerParams.Destroy destroy = new MUCOwnerParams.Destroy();
        destroy.setReason(reason);
        destroy.setJid(alternateJID);
        params.setDestroy(destroy);

        //MUCOwner iq = new MUCOwner();
        IpvsIQ iq = new IpvsIQ();
        iq.setTo(room);
        iq.setFrom(session.getXMPPUserJid());
        iq.setType(IQ.Type.SET);
        iq.setParams(params);

        String answer = session.iq_request("destroy_" + iq.getPacketID(), iq, 40 * 1000);
        if (answer == null) {
            throw new Exception("No response from server.");
        }
        //Document doc = MRXMLUtils.stringToDocument(answer);
        // Reset occupant information.
        //occupantsMap.clear();
        nickname = null;
        joined = false;
        userHasLeft();
    }

    public synchronized void leave() {
        //System.out.println("MultiUserChatImpl leave " + toString());
        // If not joined already, do nothing.
        if (!joined) {
            return;
        }
        // We leave a room by sending a presence packet where the "to"
        // field is in the form "roomName@service/nickname"
        Presence leavePresence = new Presence(Presence.Type.unavailable);
        leavePresence.setTo(room + "/" + nickname);
        leavePresence.setFrom(session.getXMPPUserJid());
        try {
            session.sendPacket(leavePresence);
        } catch (Exception e) {
            e.printStackTrace();
        }
        // Reset occupant information.
        //occupantsMap.clear();
        nickname = null;
        joined = false;
        userHasLeft();
        this.reset();
    }

    public void sendConfigurationForm(Form form) throws Exception {
        //System.out.println("MultiUserChatImpl sendConfigurationForm " + toString());
        //MUCOwner iq = new MUCOwner();
        MUCOwnerParams params = new MUCOwnerParams();
        IpvsIQ iq = new IpvsIQ();
        iq.setTo(room);
        iq.setFrom(session.getXMPPUserJid());
        iq.setType(IQ.Type.SET);
        //iq.setPacketExtension(form);
        //iq.addExtension(form.getDataFormToSend());
        params.setExtensionsXML(form.getDataFormToSend().toXML());
        iq.setParams(params);
        // Filter packets looking for an answer from the server.
        PacketFilter responseFilter = new PacketIDFilter(iq.getPacketID());
        PacketCollector response = session.createPacketCollector(responseFilter);
        // Send the completed configuration form to the server.
        session.sendPacket(iq);
        // Wait up to a certain number of seconds for a reply.
        IQ answer = (IQ) response.nextResult(40 * 1000);
        // Stop queuing results
        response.cancel();

        if (answer == null) {
            throw new Exception("No response from server.");
        } else if (answer.getError() != null) {
            throw new Exception(answer.getError().toString());
        }
    }

    public void reset() {
        /*try {
            roomListenerMultiplexor.removeRoom(room);
            // Remove all the PacketListeners added to the connection by this chat
            for (PacketListener connectionListener : connectionListeners) {
                connection.removePacketListener(connectionListener);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }*/
    }

    private synchronized void userHasLeft() {
        /*// Update the list of joined rooms through this connection
        List<String> rooms = joinedRooms.get(connection);
        if (rooms == null) {
            return;
        }
        rooms.remove(room);*/
    }

    private synchronized void userHasJoined() {
        /*// Update the list of joined rooms through this connection
        List<String> rooms = joinedRooms.get(connection);
        if (rooms == null) {
            rooms = new ArrayList<String>();
            joinedRooms.put(connection, rooms);
        }
        rooms.add(room);*/
    }

    ///////////////////

    public String getRoom() {
        return room;
    }

    public void setRoom(String room) {
        this.room = room;
    }

    public String getSubject() {
        return subject;
    }

    public void setSubject(String subject) {
        this.subject = subject;
    }

    public String getNickname() {
        return nickname;
    }

    public void setNickname(String nickname) {
        this.nickname = nickname;
    }

    public boolean isJoined() {
        return joined;
    }

    public void setJoined(boolean joined) {
        this.joined = joined;
    }

    @SuppressWarnings("unchecked")
    public static Collection<HostedRoom> getHostedRooms(XMPPI connection, String serviceName) throws Exception {
        List<HostedRoom> answer = new ArrayList<HostedRoom>();
        IPVSServiceDiscoveryManager discoManager = connection.getIPVSServiceDiscoveryManager();
        Document doc = discoManager.discoverItems(serviceName);
        Element root = doc.getRootElement();
        Element query = root.element(new QName("query", Namespace.get("http://jabber.org/protocol/disco#items")));
        if (query == null) {
            throw new Exception("No response or invalid response from server.");
        }
        List<Element> list = (List<Element>) query.elements(new QName("item", Namespace.get("http://jabber.org/protocol/disco#items")));
        for (Element item : list) {
            String itemID = item.attributeValue("jid");
            String name = item.attributeValue("name");
            answer.add(new HostedRoom(itemID, name));
        }
        /*DiscoverItems items = discoManager.discoverItems(serviceName);
           for (Iterator<DiscoverItems.Item> it = items.getItems(); it.hasNext(); answer.add(new HostedRoom( it.next())))
        ;*/
        return answer;
    }

    public static class HostedRoom {
        private String jid;
        private String name;

        public HostedRoom(String jid, String name) {
            this.jid = jid;
            this.name = name;
        }

        public String getJid() {
            return jid;
        }

        public String getName() {
            return name;
        }
    }
}

/*
<iq id="3my3I-74" to="54daaf91-bd69-4761-b5b4-a76ca73f58fb@conference.localhost" type="get">
<query xmlns="http://jabber.org/protocol/disco#items"></query></iq>
<iq id="3my3I-74" to="tx1@localhost/tx1" from="54daaf91-bd69-4761-b5b4-a76ca73f58fb@conference.localhost" type="result">
<query xmlns="http://jabber.org/protocol/disco#items">
<item jid="54daaf91-bd69-4761-b5b4-a76ca73f58fb@conference.localhost/dcuser1@192.168.1.102"/>
</query></iq>
*/