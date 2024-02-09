package com.ipvs.xmpp.client.smack;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Random;

import org.dom4j.Document;
import org.dom4j.Element;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.packet.Message;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.Presence;
import org.jivesoftware.smackx.packet.StreamInitiation;

import com.ipvs.common.MRException;
import com.ipvs.common.MRExceptionCodes;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPEvent;
import com.ipvs.common.XMPPEventQueue;
import com.ipvs.common.XMPPI;
import com.ipvs.common.XMPPTransaction;
import com.ipvs.xmpp.client.params.DBParams;
import com.ipvs.xmpp.client.smack.ft.FileTransferManager;
import com.ipvs.xmpp.client.smack.ft.FileTransferRequest;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public abstract class IPVSSession implements XMPPI {
    public static final Random wheel = new Random();
    public static final String NODEINFO = "/data";
    public static final String NODEITEMID = "owner";
    public static final int IPVSTIMEOUT = 10000;
    public static final String DOWNLOADDIR = "download/";
    protected String admin = null;
    protected int port;
    protected String server;
    protected String servername;
    protected String myjid;
    protected String bareJID;
    protected String pubsubjid;
    protected String resource;
    protected String user;
    protected String serviceName;
    protected String password;
    protected String serviceDomain = null;
    protected Thread thread;
    protected boolean bstopThread = false;
    protected boolean bConnectionSuccess = false;
    protected Hashtable<String, XMPPRequestHandler> pending = null;

    protected long serverTimeOffset = 0;
    protected boolean bServerTimeOffsetInitialized = false;
    protected IPVSMultiUserChat ipvsMultiUserChat = new IPVSMultiUserChat();

    protected BytestreamsLocalIPHandler bsLocalIPHandler = null;
    //protected PacketCollector collector;
    protected FileTransferManager fxfer;
    protected PresenseHandler presenseHandler = null;

    protected IPVSServiceDiscoveryManager iPVSServiceDiscoveryManager = null;

    ////////////////////////// login /logout ///////////////////////////////
    public abstract void login(String serverip, int port, String userJID, String password, String resource) throws Exception;

    public abstract void logout() throws Exception;

    public abstract void start();

    public abstract void stop();

    public abstract void sendPacket(Packet packet);

    public abstract void addPacketListener(PacketListener packetListener, PacketFilter packetFilter);

    public abstract void removePacketListener(PacketListener packetListener);

    public abstract PacketCollector createPacketCollector(PacketFilter packetFilter);

    public abstract void loginComplete();

    public abstract void logoutComplete();

    ////////////////////////// login /logout ///////////////////////////////

    public void setSuperUser(String admin) {
        this.admin = admin;
    }

    public String getSuperUser() {
        return this.admin;
    }

    public String getServiceDomain() {
        return serviceDomain;
    }

    public void setServiceDomain(String serviceDomain) {
        this.serviceDomain = serviceDomain;
    }

    /*
    public IPVSSession() {
    }*/

    public String getXMPPServerName() {
        return servername;
    }

    public String getXMPPUserJid() {
        return myjid;
    }

    ////////////////////////// XMPPEventQueue ///////////////////////////////
    //protected Vector<XMPPEventQueue> eventQueueList = new Vector<XMPPEventQueue>();
    protected XMPPEventQueue xmppEventQueue = null;

    public synchronized XMPPEventQueue getXMPPEventQueue(String userJID) {
        if (xmppEventQueue == null) {
            xmppEventQueue = new XMPPEventQueue(userJID);
        }
        return xmppEventQueue;
        //this.eventQueueList.add(queue);
        //return queue;
    }

    public synchronized void releaseXMPPEventQueue(XMPPEventQueue queue) {
        xmppEventQueue = null;
    }

    public synchronized void addEvent(String fromJID, String message) {

        if (xmppEventQueue != null) {
            xmppEventQueue.addXMPPEvent(new XMPPEvent(fromJID, message));
        }
    }

    public void handleEvent(String fromJID, String message) {
        //    	System.out.println("====================================================== " + message);
        try {
            this.addEvent(fromJID, message);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    ////////////////////////// pendingRequest ///////////////////////////////
    
    public synchronized void clearPendingListOnShutdown() {
    	Collection<XMPPRequestHandler> requestList = pending.values();
    	Iterator<XMPPRequestHandler> requestListIterator = requestList.iterator();
    	while(requestListIterator.hasNext()) {    		
    		XMPPRequestHandler request = requestListIterator.next();
    		request.t.setException(new Exception("shutdown"));
    	}
    }

    public synchronized void createPendingList() {
        pending = new Hashtable<String, XMPPRequestHandler>();
    }

    public synchronized void addRequest(Packet xmppPacket, XMPPRequestHandler p) {
        if (pending.containsValue(p)) {
            MRLog.println("Object already exists in queue" + p.getId());
            pending.remove(p.getId());
        }
        p.setId(xmppPacket.getPacketID());
        addRequest(p.getId(), p);//pending.put(p.getId(), p);
        sendPacket(xmppPacket);
    }

    public synchronized void addRequestWithoutSending(Packet xmppPacket, XMPPRequestHandler p) {
        if (pending.containsValue(p)) {
            MRLog.println("Object already exists in queue" + p.getId());
            pending.remove(p.getId());
        }
        p.setId(xmppPacket.getPacketID());
        pending.put(p.getId(), p);
    }

    public synchronized void delRequest(XMPPRequestHandler p) {
        pending.remove(p.getId());
    }

    public synchronized XMPPRequestHandler findRequest(String tid) {
        XMPPRequestHandler p = null;
        p = pending.get(tid);
        return p;
    }

    //new added to check file transfer status
    public synchronized void addRequest(String id, XMPPRequestHandler p) {
        pending.put(id, p);
    }

    public synchronized XMPPRequestHandler getRequest(String id) {
        return pending.get(id);
    }

    public synchronized List<String> getRequestIDs() {
        List<String> list = new ArrayList<String>();
        for (Enumeration<String> e = pending.keys(); e.hasMoreElements();) {
            String object = e.nextElement();
            list.add(object);
        }
        return list;
    }

    public synchronized List<String> getRequestIDs_fileTransfer() {
        List<String> list = new ArrayList<String>();
        for (Enumeration<String> e = pending.keys(); e.hasMoreElements();) {
            String id = e.nextElement();
            Object object = getRequest(id);
            if (object instanceof IPVSSendFile || object instanceof IPVSGetFile) {
                list.add(id);
            }
        }
        return list;
    }

    ////////////////////////// utility ///////////////////////////////
    public void toXML() {
        MRLog.println("<XMPPSession>");
        MRLog.println("<port>" + this.port + "</port>");
        MRLog.println("<user>" + this.user + "</user>");
        MRLog.println("<password>" + this.password + "</password>");
        MRLog.println("<resource>" + this.resource + "</resource>");
        MRLog.println("<server>" + this.server + "</server>");
        MRLog.println("<domain>" + this.server + "</domain>");
        MRLog.println("<servername>" + this.servername + "</servername>");
        MRLog.println("<admin>" + this.admin + "</admin>");
        MRLog.println("<pubsubjid>" + this.pubsubjid + "</pubsubjid>");
        MRLog.println("<myjid>" + this.myjid + "</myjid>");
        MRLog.println("</XMPPSession>");
    }

    ////////////////////////// PubSub ///////////////////////////////

    ////////////////////////// login /logout ///////////////////////////////

    public boolean processMessages(Packet packet) {
        boolean result = false;
        if (packet == null) {
            return result;
        } else {
            String msgid = null;
            msgid = packet.getPacketID();
            XMPPRequestHandler p = null;
            if (msgid != null) {
                if (!(packet instanceof Message)) {
                    p = findRequest(msgid);
                }
            }
            if (p != null) {
                p.process(packet);
            } else {
                /*if(packet.toXML().indexOf("FileXferID") !=-1){
                    MRLog.println("***************************** IN from "+ packet.getFrom() +" "+packet.toXML());
                 }else if(packet.toXML().indexOf("queue_state") !=-1){
                    MRLog.println("***************************** IN from "+ packet.getFrom() +" "+packet.toXML());
                }*/
                this.handleEvent(packet.getFrom(), packet.toXML());
                String from = packet.getFrom();
                if ((from != null) && (packet instanceof StreamInitiation)) {
                    StreamInitiation si = (StreamInitiation) packet;
                    if (si.getType() == IQ.Type.SET) {
                        String id = "";
                        FileTransferRequest request = new FileTransferRequest(fxfer, si);
                        id = packet.getFrom() + request.getFileName();
                        p = findRequest(id);
                        if (p != null) {
                            p.process(packet);
                        } else {
                            request.reject();
                            MRLog.println("Could not find request for Stream Initiation");
                        }
                    }
                }
            }
        }
        if(presenseHandler!=null) {
        	presenseHandler.processPacket(packet);
        }
        result = true;
        return result;
    }

    ////////////////////////// Room ///////////////////////////////

    public void addRoom(String roomNID, String nickname) throws Exception {
        this.ipvsMultiUserChat.addRoom(this, roomNID, nickname);
    }

    public void joinRoom(String roomNID, String nickname) throws Exception {
        this.ipvsMultiUserChat.joinRoom(this, roomNID, nickname);
    }

    public boolean isJoined(String roomNID) throws Exception {
        return this.ipvsMultiUserChat.isJoined(this, roomNID);
    }

    public static final String MEDIAROOMINVITE = "MediaRoomInvite";
    public static final String SMACK_CONNECTION = "smack_connection";
    public static final String COMPONENT_CONNECTION = "component_connection";
    
    private void checkToJIDEmptyOrNot(String toJID) throws Exception {
    	if(toJID==null|| toJID.isEmpty()){
    		throw new MRException(MRExceptionCodes.TO_JID_EMPTY, "Can not send Message, toJID is empty");
    	}
    }

    public void sendInvite(String roomNID, String toJID) throws Exception {
        // MRLog.println("XMPP::sendInvite roomNID=" + roomNID + " toJID=" + toJID);
    	checkToJIDEmptyOrNot(toJID);    	
        String message = "<" + MEDIAROOMINVITE + "><roomNID>" + roomNID + "</roomNID></" + MEDIAROOMINVITE + ">";
        sendMessage(toJID, message, null);
    }

    public void deleteRoom(String roomNID) throws Exception {
        this.ipvsMultiUserChat.deleteRoom(this, roomNID);
    }

    public void sendRoomMessage(String roomNID, String message, String extensionNamespace) throws Exception {
        // this.ipvsMultiUserChat.sendRoomMessage(this.connection, roomNID, toJID, message);
        String roomJID = this.ipvsMultiUserChat.getRoomJID(roomNID);
        Message m = new Message(roomJID, Message.Type.groupchat);
        m.setFrom(getXMPPUserJid());
        IPVSMessageExtension agentMessageElement = new IPVSMessageExtension(extensionNamespace);
        agentMessageElement.setXML(message);
        m.addExtension(agentMessageElement);
        /*if(!isUserPresenceAvailable(m.getTo())){
            System.out.println(" myjid " + myjid +" "+ m.toXML());
            throw new MRException(MRExceptionCodes.USER_NOT_AVAILABLE, "<error><User>" + m.getTo()+ "</User></error>");
        }*/
        sendPacket(m);
    }

    public ArrayList<String> getRoomNIDList() {
        return this.ipvsMultiUserChat.getRoomNIDList(this);
    }

    public boolean isAvailable(String roomNID, String roomUserJID) throws Exception {
        return this.ipvsMultiUserChat.isAvailable(this, roomNID, roomUserJID);
    }

    public void leaveRoom(String roomNID, String userJID) throws Exception {
        this.ipvsMultiUserChat.leaveRoom(this, roomNID, userJID);
    }

    ////////////////////////// Room ///////////////////////////////

    ////////////////////////// sendMessage / presence ///////////////////////////////
    @Override
    public void sendMessage(String userJID, String message, String extensionNamespace) throws Exception {
    	checkToJIDEmptyOrNot(userJID);
        Message m = new Message();
        if (extensionNamespace == null)
            m.setBody(message);
        else {
            // message = URLDecoder.decode(message, "UTF-8");
            IPVSMessageExtension ipvsMessageExtension = new IPVSMessageExtension(extensionNamespace);
            ipvsMessageExtension.setXML(message);
            m.addExtension(ipvsMessageExtension);
        }
        m.setFrom(getXMPPUserJid());
        m.setTo(userJID);
        //System.out.println("***************************** OUT:" +m.getTo()+" "+ m.toXML());        
        sendPacket(m);
    }

    public void subscribePresence(String user) throws XMPPException,Exception {
    	checkToJIDEmptyOrNot(user);
        Presence p = new Presence(Presence.Type.subscribe);
        p.setTo(user);
        p.setFrom(getXMPPUserJid());
        sendPacket(p);
    }

    ////////////////////////// sendMessage / presence ///////////////////////////////

    ////////////////////////// xmpp calls  ///////////////////////////////
    public Object ipvsdb_request(DBParams dbParams) throws Exception {
        return ipvsdb_request(dbParams, null);
    }

    private Object ipvsdb_request(DBParams dbParams, Packet packet) throws Exception {
        XMPPTransaction transaction = new XMPPTransaction(dbParams.getNodeID(), dbParams.getAction(), dbParams.getNodeID(), dbParams.getDBCallTimeOut());
        
        // TBR MIYE Remove once we have identified that the db query is blocked
        
        long startTime = System.currentTimeMillis();
        
        DBRequest dbCall = new DBRequest();
        dbParams.setToJID(pubsubjid);
        dbParams.setFromJID(myjid);
        dbCall.init(this, dbParams, transaction);
        dbCall.process(packet);
        transaction.waitForResponse(dbParams.getDBCallTimeOut());
        Object response = transaction.getResponse();
        transaction = null;
        
        long duration = System.currentTimeMillis() - startTime;
        if(duration > 5000) {
            MRLog.println("IPVSSession:ipvsdb_request: query=" + dbParams.toXML() + " duration =" + duration +" packet:="+dbCall.getId());
        }
        	
        return response;
    }

    ////////////////////// ipvsdb_getChildren
    @SuppressWarnings("unchecked")
    @Override
    public List<String> ipvsdb_getChildren(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null)
            return (List<String>) ipvsdb_request(dbParams);
        return new ArrayList<String>();
    }

    ////////////////////// ipvsdb_nodeExists
    @Override
    public boolean ipvsdb_nodeExists(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null)
            return true;
        return false;
    }

    ////////////////////// ipvsdb_getData
    @Override
    public Element ipvsdb_getData(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Element)
            return (Element) response;
        return null;
    }

    ////////////////////// ipvsdb_getData
    @Override
    public Document ipvsdb_addNode(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }

    @Override
    public Document ipvsdb_addNodeWithNodeID(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }

    @Override
    public Document ipvsdb_setData(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }

    ////////////////////// ipvsdb_deleteNode
    @Override
    public Document ipvsdb_deleteNode(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }

    ////////////////////// ipvsdb_Subscribe ipvsdb_Unsubscribe
    @Override
    public boolean ipvsdb_Subscribe(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null)
            return true;
        return false;
    }

    @Override
    public boolean ipvsdb_updateSubscriptions(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null)
            return true;
        return false;
    }
    
    @Override
    public boolean ipvsdb_Unsubscribe(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null)
            return true;
        return false;
    }
    
    ////////////////////// ipvsdb_Subscribe getCurrentTimeMillis
    public long getCurrentTimeMillis() {
        long ms = 0;
        long offset = 0;
        // Comment this out if you do not want to use the servertime
        offset = getServerTimeOffset();
        ms = System.currentTimeMillis() + offset;
        return ms;
    }

    public long getServerTimeOffset() {
        if (bServerTimeOffsetInitialized) {
            return serverTimeOffset;
        }

        XMPPTransaction transaction = new XMPPTransaction("TIME", "GetServerTime", "none");
        try {
            GetServerTimeOffset dbCall = new GetServerTimeOffset();
            dbCall.init(this, transaction, myjid);
            dbCall.process(null);
            ;
            transaction.waitForResponse();
            serverTimeOffset = Long.parseLong((String) transaction.getResponse());
            bServerTimeOffsetInitialized = true;
        } catch (Exception e) {
            e.printStackTrace();
        }
        return serverTimeOffset;

    }
    
    //////////////////////// getSubscriptionList //////////////////////////////
    
    public Document ipvsdb_getSubscribers(DBParams dbParams) throws Exception {
    	Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }
    
    public Document ipvsdb_getSubscriptions(DBParams dbParams) throws Exception {
    	Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }
    ////////////////////////// getSubscriptionList ends /////////////////////////
    
    
    @Override
    public void sendFile(String toJID, String fileName, String fileXferID) {
        //SmackConfiguration.setPacketReplyTimeout(20*1000);
        IPVSSendFile sf = new IPVSSendFile();
        XMPPTransaction t = new XMPPTransaction(fileName, fileName, fileName);
        sf.init(this, t, toJID);
        sf.sendFile(fxfer, toJID, fileName, fileXferID);
        sf.setId(toJID + fileName);
        //MRLog.println("xmpp:sendFile: start:" + toJID + fileName);
        addRequest(sf.getId(), sf);
    }

    @Override
    public void getFile(String fromJID, String srcFileName, String destFileName, String fileXferID) {
        //SmackConfiguration.setPacketReplyTimeout(20*1000);
        IPVSGetFile gf = new IPVSGetFile();
        XMPPTransaction t = new XMPPTransaction(fromJID, srcFileName, srcFileName);
        gf.init(this, fxfer, t, fromJID);
        gf.GetFile(fromJID, srcFileName, destFileName, fileXferID);
        gf.setId(fromJID + srcFileName);
        //MRLog.println("xmpp:getFile: start:" + fromJID + srcFileName);
        addRequest(gf.getId(), gf);
    }

    @Override
    public void getFileStatus(String toJID, String srcFileName, String destFileName) {
        String tid;
        tid = toJID;
        IPVSGetFile gf;
        gf = (IPVSGetFile) this.findRequest(tid);
        if (gf != null) {
            gf.checkStatus();
        }
    }

    @Override
    public void getFileStop(String fromJID, String srcFileName, String destFileName) {
        String tid;
        tid = fromJID;
        IPVSGetFile gf;
        gf = (IPVSGetFile) this.findRequest(tid);

        if (gf != null) {
            //MRLog.println("xmpp:getFileStop: close " + fromJID +" srcFileName "+ srcFileName);
            gf.closeFile();
        } else {
            MRLog.println("xmpp:getFileStop: Could not close " + fromJID + " srcFileName " + srcFileName);
        }
    }

    @Override
    public void sendFileStatus(String toJID, String fileName) {
        // TODO Auto-generated method stub
        String tid;
        tid = toJID;
        IPVSSendFile sf;
        sf = (IPVSSendFile) this.findRequest(tid);
        if (sf != null) {
            sf.checkStatus();
        }
    }

    @Override
    public void sendFileStop(String toJID, String fileName) {
        // TODO Auto-generated method stub
        String tid;
        tid = toJID + fileName;
        IPVSSendFile sf;
        sf = (IPVSSendFile) this.findRequest(tid);
        if (sf != null) {
            sf.closeFile();
        } else {
            MRLog.println("Could not close " + toJID);
        }

    }

    ////////////////////////// process messages ///////////////////////////////
    ////////////////////////// added for component testing ////////////////////

    public String iq_request(String description, Packet packet, long timeout) throws Exception {
        XMPPTransaction transaction = new XMPPTransaction(description, description, description, timeout);
        XmppRequest dbCall = new XmppRequest();
        dbCall.init(this, description, transaction);
        dbCall.process(packet);
        transaction.waitForResponse();
        Object response = transaction.getResponse();
        transaction = null;
        if (response != null) {
            if (response instanceof Packet) {
                return ((Packet) response).toXML();
            }
            return response.toString();
        }
        return null;
    }

    /*
    private PresenseHandler presenseHandler=null;
    public XMPPConnection getConnection() {
        return connection;
    }
    */
    ////////////////////////// added for component testing ////////////////////
    public IPVSServiceDiscoveryManager getIPVSServiceDiscoveryManager() {
        return iPVSServiceDiscoveryManager;
    }

    public void setIPVSServiceDiscoveryManager(IPVSServiceDiscoveryManager serviceDiscoveryManager) {
        iPVSServiceDiscoveryManager = serviceDiscoveryManager;
    }

    ///////////// import export ///////////
    public Document ipvsdb_importDB(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }

    public Document ipvsdb_exportDB(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }

    public Document ipvsdb_deleteDB(DBParams dbParams) throws Exception {
        Object response = ipvsdb_request(dbParams);
        if (response != null && response instanceof Document)
            return (Document) response;
        return null;
    }
    /*public PresenseHandler getPresenseHandler(){
        return presenseHandler;
    }*/
    
    @Override
    public String getUser() {
        return myjid;
    }
    
    public boolean isFileTransferToLocalIP(String jid) throws Exception {
        return bsLocalIPHandler.isFileTransferToLocalIP(jid);
    }

	public abstract boolean isConnected() ;
}
