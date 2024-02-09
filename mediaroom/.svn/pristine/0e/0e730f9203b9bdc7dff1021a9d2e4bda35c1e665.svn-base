package com.ipvs.xmpp.client.smack;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.Namespace;
import org.dom4j.QName;
import org.jivesoftware.smackx.Form;
import org.jivesoftware.smackx.FormField;

import com.ipvs.common.XMPPI;
import com.ipvs.xmpp.client.smack.MultiUserChatImpl.HostedRoom;

public class IPVSMultiUserChat {

    public static final String MUCSERVICE = "conference.localhost";

    private Hashtable<String, MultiUserChatImpl> mucTable = new Hashtable<String, MultiUserChatImpl>();

    private int userCount(XMPPI connection, String roomNID) {
        int count = 0;
        //System.out.print("IPVSMultiUserChat: room disco :");
        try {
            IPVSServiceDiscoveryManager discoManager = connection.getIPVSServiceDiscoveryManager();
            Document doc = discoManager.discoverItems(roomNID + "@" + IPVSMultiUserChat.MUCSERVICE);
            Element root = doc.getRootElement();
            Element query = root.element(new QName("query", Namespace.get("http://jabber.org/protocol/disco#items")));
            if (query == null) {
                throw new Exception("No response or invalid response from server.");
            }
            List<?> list = query.elements(new QName("item", Namespace.get("http://jabber.org/protocol/disco#items")));
            count = list.size();
        } catch (Exception e) {
            e.printStackTrace();
        }
        return count;
    }

    // TBD create the appropriate exception class
    public String getRoomJID(String roomNID) throws Exception {
        MultiUserChatImpl muc = this.getMUC(roomNID);
        if (muc == null)
            throw new Exception();
        return muc.getRoom();
    }

    private synchronized MultiUserChatImpl addMUC(XMPPI connection, String roomNID) throws Exception {
        if (this.mucTable.containsKey(roomNID))
            return this.mucTable.get(roomNID);
        MultiUserChatImpl muc = new MultiUserChatImpl(connection, roomNID + "@" + IPVSMultiUserChat.MUCSERVICE);
        this.mucTable.put(roomNID, muc);
        //System.out.println("IPVSMultiUserChat:getMUC:mucTable size=" + this.mucTable.size() + 
        //" added roomNID=" + roomNID);
        return muc;
    }

    private synchronized MultiUserChatImpl getMUC(String roomNID) throws Exception {
        return this.mucTable.get(roomNID);
    }

    public synchronized void flushMUCMessages(String roomNID) {

    }

    private synchronized void releaseMUC(String roomNID) {
        this.mucTable.remove(roomNID);
        //System.out.println("IPVSMultiUserChat:releaseMUC:mucTable size=" + this.mucTable.size() +
        //" removed roomNID=" + roomNID);
    }

    public void addRoom(XMPPI connection, String roomNID, String nickname) throws Exception {
        //System.out.println("IPVSMultiUserChat::addRoom roomNID=" + roomNID);

        MultiUserChatImpl muc = this.addMUC(connection, roomNID);
        try {
            muc.create(nickname);
            Form form = new Form(Form.TYPE_SUBMIT);
            FormField field = new FormField("muc#roomconfig_maxusers");
            field.setType(FormField.TYPE_TEXT_SINGLE);
            form.addField(field);
            form.setAnswer("muc#roomconfig_maxusers", 100 + "");
            muc.sendConfigurationForm(form);
        } catch (Exception e) {
            //System.out.println("IPVSMultiUserChat::createRoom Failed roomNID=" + roomNID);        	
            e.printStackTrace();
            this.releaseMUC(roomNID);
        }
    }

    public void joinRoom(XMPPI connection, String roomNID, String nickname) throws Exception {
        if (this.getMUC(roomNID) != null) {
            //System.out.println("IPVSMultiUserChat::joinRoom Already joined roomNID=" + roomNID);
            return;
        }

        if (this.userCount(connection, roomNID) == 0) { // 
            //System.out.println("IPVSMultiUserChat::joinRoom Room Not found roomNID=" + roomNID);
            return;
        }

        //System.out.println("IPVSMultiUserChat::joinRoom roomNID=" + roomNID);
        MultiUserChatImpl muc = addMUC(connection, roomNID);
        try {
            muc.join(nickname);
            if (this.userCount(connection, roomNID) == 1) { // Owner not present
                //System.out.println("IPVSMultiUserChat::joinRoom Owner Not found roomNID=" + roomNID);
                muc.leave();
                throw new Exception();
            }
        } catch (Exception e) {
            //System.out.println("IPVSMultiUserChat::joinRoom Failed roomNID=" + roomNID);        	
            e.printStackTrace();
            this.releaseMUC(roomNID);
        }
    }

    public boolean isJoined(XMPPI connection, String roomNID) throws Exception {
        return (this.getMUC(roomNID) != null);
    }

    public void deleteRoom(XMPPI connection, String roomNID) throws Exception {
        //System.out.println("IPVSMultiUserChat::deleteRoom roomNID=" + roomNID);
        MultiUserChatImpl muc = this.getMUC(roomNID);
        if (muc == null) {
            //System.out.println("IPVSMultiUserChat::deleteRoom adding MUC roomNID=" + roomNID);
            muc = this.addMUC(connection, roomNID);
            muc.join("none");
        }
        muc.destroy("noReason", "noAlternateJID");
    }

    public void leaveRoom(XMPPI connection, String roomNID, String userJID) throws Exception {
        //System.out.println("IPVSMultiUserChat::leaveRoom roomNID=" + roomNID);
        MultiUserChatImpl muc = getMUC(roomNID);
        if (muc == null) {
            //System.out.println("IPVSMultiUserChat::leaveRoom not found roomNID=" + roomNID);
            return;
        }
        try {
            muc.leave();
        } catch (Exception e) {
            e.printStackTrace();
        }
        this.releaseMUC(roomNID);
    }

    public ArrayList<String> getRoomNIDList(XMPPI connection) {
        ArrayList<String> list = new ArrayList<String>();

        try {
            Collection<HostedRoom> roomlist = MultiUserChatImpl.getHostedRooms(connection, IPVSMultiUserChat.MUCSERVICE);
            Iterator<HostedRoom> i = roomlist.iterator();

            while (i.hasNext()) {
                HostedRoom room = i.next();
                String roomNID = room.getName();
                list.add(roomNID);
            }
        } catch (Exception e) {
            e.printStackTrace();
        }

        return list;
    }

    @SuppressWarnings("unchecked")
    public boolean isAvailable(XMPPI connection, String roomNID, String roomUserJID) throws Exception {
        IPVSServiceDiscoveryManager discoManager = connection.getIPVSServiceDiscoveryManager();
        Document doc = discoManager.discoverItems(roomNID + "@" + IPVSMultiUserChat.MUCSERVICE);
        Element root = doc.getRootElement();
        Element query = root.element(new QName("query", Namespace.get("http://jabber.org/protocol/disco#items")));
        if (query == null) {
            throw new Exception("No response or invalid response from server.");
        }
        String roomUser = (new StringTokenizer(roomUserJID, "@")).nextToken();
        List<Element> list = (List<Element>) query.elements(new QName("item", Namespace.get("http://jabber.org/protocol/disco#items")));
        for (Element item : list) {
            String itemID = item.attributeValue("jid");
            int i = itemID.indexOf("/");
            String rUserJID = itemID.substring(i + 1);
            String rUser = (new StringTokenizer(rUserJID, "@")).nextToken();

            if (rUser.equals(roomUser)) {
                return true;
            }
        }

        return false;
    }

}
