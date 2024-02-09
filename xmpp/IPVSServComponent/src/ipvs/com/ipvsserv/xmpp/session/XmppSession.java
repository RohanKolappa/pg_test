package com.ipvsserv.xmpp.session;

import java.util.concurrent.atomic.AtomicInteger;

import org.xmpp.packet.JID;
import org.xmpp.packet.Packet;

public abstract class XmppSession implements InternalJID{
    //even listeners 
    public abstract void addXmppSessionEventListener(XmppSessionEventListener xmppSessionEventListener);
    
    public abstract void removeXmppSessionEventListener(XmppSessionEventListener xmppSessionEventListener);
    
    ////////// login parameters ///////////
    public abstract String getServiceID();
    
    public abstract String getServiceDescription();
    
    public abstract String getSecretKey();
    
    public abstract String getHost();
    
    public abstract int getPort();
    
    public abstract int getNumberOfConnections();
    
    public abstract String getServiceDomain();
    
    public abstract JID getAddress();
    
    ////////// login parameters ///////////
    
    //////// login //////////
    public abstract void logout() throws Exception;
    
    public abstract void login() throws Exception;
    
    /////////////////// Start ConnectionLost
    public abstract boolean isStarted();
    
    public abstract boolean isConnectionLostToServer();
    
    public abstract boolean isShutdownInProgress();
    
    /////////////////// route
    public abstract void sendPacket(Packet packet) throws Exception;
    
    public abstract void processPacket(Packet packet) throws Exception;
    
    public abstract void sendPacket(String to, String packet) throws Exception ;
    
    /////////////////
    private static AtomicInteger connectionCounter = new AtomicInteger(0);
    public int connectionCounterValue = connectionCounter.getAndIncrement();
    //////////// query
    //public abstract IQ query(IQ packet, long timeout) throws Exception;
    //public abstract void query(IQ packet, PacketListener listener) throws Exception;
    
    ////////////////////////////////////////////////////// new /////////////
    
    //public abstract void addConnectionListener(ConnectionListener connectionListener);
    //public abstract void removeConnectionListener(ConnectionListener connectionListener);
    
    //////////////
   /* protected static boolean isFirstInitialization = false;
    protected final static Set<ConnectionCreationListener> connectionEstablishedListeners = new CopyOnWriteArraySet<ConnectionCreationListener>();
    
    public static void addConnectionCreationListener(ConnectionCreationListener connectionCreationListener) {
        connectionEstablishedListeners.add(connectionCreationListener);
    }
    
    public static void removeConnectionCreationListener(ConnectionCreationListener connectionCreationListener) {
        connectionEstablishedListeners.remove(connectionCreationListener);
    }
    
    protected void raiseEventConnectionCreationListener() {
        if (isFirstInitialization) {
            if (isFirstInitialization) {
                // Notify listeners that a new connection has been established
                for (ConnectionCreationListener listener : connectionEstablishedListeners) {
                    listener.connectionCreated(this);
                }
            }
        }
    }*/
    
}
