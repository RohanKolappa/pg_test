package com.ipvs.xmpp.client.smack;

import org.jivesoftware.smack.ConnectionConfiguration;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.Roster;
import org.jivesoftware.smack.SmackConfiguration;
import org.jivesoftware.smack.XMPPConnection;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.filter.ToContainsFilter;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.Presence;
import org.jivesoftware.smackx.ServiceDiscoveryManager;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPEventQueue;
import com.ipvs.common.XMPPI;
import com.ipvs.config.IPVSConst;
import com.ipvs.xmpp.client.smack.ft.FileTransferManager;

public class IPVSSessionSmackImpl extends IPVSSession implements Runnable {
    protected XMPPConnection connection;
    protected PacketCollector collector;

    ////////////////////////// login /logout ///////////////////////////////
    @Override
    public void login(String serverip, int port, String userJID, String password, String resource) throws Exception {
        long to = System.currentTimeMillis();
        this.server = serverip;
        this.port = port;
        // We disregard the user@domain/resource
        this.user = userJID;
        this.password = password;
        this.resource = resource;
        createPendingList();

        ConnectionConfiguration config = new ConnectionConfiguration(serverip, port);
        // XMPPConnection.DEBUG_ENABLED = false; //Findbugs
        connection = new XMPPConnection(config);

        try {
            connection.connect();
            MRLog.println("IPVSSession connect DONE " + (System.currentTimeMillis() - to));
            to = System.currentTimeMillis();
        } catch (XMPPException e) {
            bstopThread = true;

            // TODO Auto-generated catch block
            // e.printStackTrace();
            throw (e);
        }

        iPVSServiceDiscoveryManager = new IPVSServiceDiscoveryManager(this);
        servername = connection.getServiceName();
        myjid = user + "@" + servername + "/" + resource;
        bareJID = user + "@" + servername;

        xmppEventQueue = new XMPPEventQueue(myjid);
        pubsubjid = IPVSConst.getInstance().getIpvsDbComponentJIDPrefix() + "." + servername;
        PacketFilter filter = new ToContainsFilter(user + "@" + servername);
        collector = connection.createPacketCollector(filter);
         
        int retryTimeOut = 0;
        try {
        	//TODO make it data driven
            retryTimeOut = SmackConfiguration.getPacketReplyTimeout(); //current default to 5 sec in the smack lib
            SmackConfiguration.setPacketReplyTimeout(3*retryTimeOut);  
            
            connection.login(user, password, resource);
            MRLog.println("IPVSSession Login DONE " + (System.currentTimeMillis() - to));
        } catch (XMPPException e) {
            e.printStackTrace();
            bstopThread = true;
            connection.disconnect();
            collector = null;
            throw e;
        } finally {
        	SmackConfiguration.setPacketReplyTimeout(retryTimeOut);
        }

        fxfer = new FileTransferManager(this);
        bsLocalIPHandler = new BytestreamsLocalIPHandler(this);
        start();
    }

    @Override
    public void logout() throws Exception {
        try {
            bstopThread = true;
            collector.cancel();
            connection.disconnect();
        } catch (Exception e) {
            // Ignore the error on disconnect
            MRLog.println("Ignoring the Security Exception " + e.getMessage());
        }
        try {
        	fxfer.cleanup(this);
        } catch (Exception e) {
        }        
        fxfer=null;
        logoutComplete();
    }

    public void loginComplete() {

    }

    public void logoutComplete() {

    }

    @Override
    public void start() {
        if (thread == null) {
            thread = new Thread(this, "IPVSPacketCollector");
            thread.start();
        }

    }

    @Override
    public void stop() {
        try {
            thread.interrupt();
            System.out.println("Stop XMPP");
        } catch (Exception ex) {
            //neglect
            //TODO:remove this log after test. 
            try {
                //the thread may already be stopped ... run() exists if bstopThread=true... 
                /*
                //bug: 14998. Internal Error: Logout Failed message when Inactivity timeout set.
                <info time='1253231004890'>Logout userJID=ccuser1@localhost/ccpc1</info>
                <info time='1253231004890'>XMPPEventHandlerThread:: exiting</info>
                 Ignoring the Security Exception access denied (java.lang.RuntimePermission modifyThread)
                java.security.AccessControlException: access denied (java.lang.RuntimePermission modifyThread)
                at java.security.AccessControlContext.checkPermission(Unknown Source)
                at java.security.AccessController.checkPermission(Unknown Source)
                at java.lang.SecurityManager.checkPermission(Unknown Source)
                at sun.plugin2.applet.Applet2SecurityManager.checkAccess(Unknown Source)
                at java.lang.Thread.checkAccess(Unknown Source)
                at java.lang.Thread.interrupt(Unknown Source)
                at com.ipvs.xmpp.client.smack.IPVSSession.stop(IPVSSession.java:246)
                at com.ipvs.client.ServiceClient.logout(ServiceClient.java:250)             
                */
                if (thread != null) {
                    MRLog.println("IPVSSession thread: Error on interuption. isInterrupted? " + thread.isInterrupted() + " isAlive? " + thread.isAlive() + " bstopThread " + bstopThread);
                }
            } catch (Exception ex2) {
            }
        }
    }

    @Override
    public void run() {
        while (!bstopThread) {
            if (!processMessages()) {
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    // e.printStackTrace();
                    break;
                }
            }
        }
    }

    
    public boolean processMessages() {
        boolean result = false;
        if (connection == null) {
            return result;
        }
        if (!connection.isConnected()) {
            if (bConnectionSuccess == true) {
                MRLog.println("Sending shutdown");
                this.handleEvent(this.myjid, "<shutdown/>");
                this.clearPendingListOnShutdown();
                bConnectionSuccess = false;
            }
            return result;
        }
        if (bConnectionSuccess != true) {
            bConnectionSuccess = true;
            MRLog.println("Setting connection Success");
        }
        if (!connection.isAuthenticated()) {
            return result;
        }
        if (collector == null) {
            MRLog.println("Collector not yet set!!");
            return result;
        }
        Packet packet = collector.pollResult();
        if (packet == null) {
            return result;
        }

        return processMessages(packet);
    }

    public void sendPacket(Packet packet) {
        try {
            //System.out.println("OUT packet " + packet.getPacketID());
            if (packet.getFrom() == null || packet.getFrom().equals("")) {
                packet.setFrom(getXMPPUserJid());
            }
            //System.out.println("OUT packet " + packet.getPacketID());
            /*if(packet.toXML().indexOf("FileXferID") !=-1){
                MRLog.println("***************************** OUT to "+ packet.getTo() +" "+packet.toXML());
            }else if(packet.toXML().indexOf("queue_state") !=-1){
                MRLog.println("***************************** OUT to "+ packet.getTo() +" "+packet.toXML());
            }*/
        } catch (Exception e) {
            e.printStackTrace();
        }
        connection.sendPacket(packet);
    }

    public XMPPConnection getConnection() {
        return connection;
    }

    public void addPacketListener(PacketListener packetListener, PacketFilter packetFilter) {
        connection.addPacketListener(packetListener, packetFilter);
    }

    public void removePacketListener(PacketListener packetListener) {
        connection.removePacketListener(packetListener);
    }
    
    public PacketCollector createPacketCollector(PacketFilter packetFilter) {
        return connection.createPacketCollector(packetFilter);
    }

    public boolean isUserPresenceAvailable(String touser) {
        if (myjid.equals(touser) || bareJID.equals(touser)) {
            return true;
        }
        try {
            Roster roster = connection.getRoster();
            Presence presence = roster.getPresence(touser);
            if (presence != null) {
                if (presence.getType().compareTo(Presence.Type.available) == 0) {
                    return true;
                }
            }
        } catch (Exception ex) {
            MRLog.println(touser + " isUserPresenceAvailable exception using smack roster.");
            //ex.printStackTrace();
        }
        try {
            bsLocalIPHandler.isFileTransferToLocalIP(touser);
            return true;
        } catch (Exception ex) {
            MRLog.println(touser + " isUserPresenceAvailable exception using isFileTransferToLocalIP.");
            //ex.printStackTrace();
        }
        return false;
    }
    
    public void subscribeUserPresence(String touser) {
        if (myjid.equals(touser) || bareJID.equals(touser)) {
            return;
        }
        Presence presenceSubscribe = new Presence(Presence.Type.subscribe);
        presenceSubscribe.setTo(touser);
        presenceSubscribe.setFrom(myjid);
        sendPacket(presenceSubscribe);
    }
    
    public void unSubscribeUserPresence(String touser) {
        if (myjid.equals(touser) || bareJID.equals(touser)) {
            return;
        }
        Presence presenceSubscribe = new Presence(Presence.Type.unsubscribe);
        presenceSubscribe.setTo(touser);
        presenceSubscribe.setFrom(myjid);
        sendPacket(presenceSubscribe);
    }
    
    public String getIPVSConnectionType() {
        return IPVSSession.SMACK_CONNECTION;
    }
    
	public boolean isConnected() {
		return connection.isConnected();
	}

	@Override
    public String getServiceName() {
        return connection.getServiceName();
    }
    
	public String getLoginType() {
        return XMPPI.LOGIN_TYPE_SMACK;
    }

	@Override
	public void addDiscoFeature(String name) {
		ServiceDiscoveryManager serviceDiscoManager = ServiceDiscoveryManager.getInstanceFor(connection);
		serviceDiscoManager.addFeature(name);
	}   

}
