package com.barco.utils.mr;
 

import java.util.ArrayList;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.packet.Packet;

import com.ipvs.xmpp.client.params.DBParams;
import com.ipvs.xmpp.client.smack.IPVSServiceDiscoveryManager;
import com.ipvs.xmpp.client.smack.XMPPRequestHandler;

 

/**
 * The interface for implementation of the XMPP server
 * 
 * @author miyer
 * 
 */
public interface  XMPPI1 {
	public static final String MEDIAROOMINVITE = "MediaRoomInvite";
    public static final String LOGIN_TYPE_SMACK = "smack";
    public static final String LOGIN_TYPE_COMP = "comp";
    public abstract String getServiceDomain() ;
    public abstract void setServiceDomain(String serviceDomain) ;
    public abstract void setSuperUser(String superUser) ;
    public abstract String getSuperUser() ;
    public abstract String getXMPPServerName();
    public abstract String getXMPPUserJid();
    public abstract String getUser() ;
    public abstract boolean isConnected() ;
    public abstract String getServiceName() ;
    

    
    //////////////// Start/stop threads cleanly ////////////////// // 
    public abstract void start();
    public abstract void stop();
    
    //////////////Connection ////////////////// // 
    public abstract void login(String serverip, int port, String userJID, String password, String resource) throws Exception;
    public abstract void logout() throws Exception; // TBD put this back

    //////////////Time ////////////////// // 
    public abstract long getCurrentTimeMillis();

    //////////////Messaging ////////////////// // 
    public abstract void sendMessage(String userJID, String message,
    		String extensionNamespace) throws Exception;
    public abstract void sendPacket(Packet p) throws Exception;
    //////////////Presence ////////////////// // 
    public abstract void subscribePresence(String userJID) throws Exception;
    

    //////////////Room ////////////////// // 
    public abstract void addRoom(String roomNID, String nickname) throws Exception;
    public abstract void sendInvite(String roomNID, String toJID) throws Exception;
    public abstract void joinRoom(String roomNID, String nickname) throws Exception;
    public abstract boolean isJoined(String roomNID) throws Exception;
    public abstract void deleteRoom(String roomNID) throws Exception;
    public abstract void sendRoomMessage(String roomNID, String message,
    		String extensionNamespace) throws Exception;
    //public abstract String[] getMyRooms() throws Exception;
    public abstract boolean isAvailable(String roomNID, String userJID) throws Exception;
    // public abstract boolean isOwnerAvailable(String roomNID, String userJID) throws Exception;
    // public abstract boolean isOwner(String roomNID, String userJID) throws Exception;
    public abstract void leaveRoom(String roomNID, String userJID) throws Exception;
    public abstract ArrayList<String> getRoomNIDList();
    //?
    //public abstract boolean isPresent(String deviceOwnerJID) throws Exception ;
  
    
    // Async ipvsdb
    //////////////Async ipvsdb Subscribe/unSubscribe //////////////////
    public abstract Object ipvsdb_request(DBParams dbParams) throws Exception ;
    public abstract boolean ipvsdb_Subscribe(DBParams dbParams) throws Exception ;
    public abstract boolean ipvsdb_updateSubscriptions(DBParams dbParams) throws Exception ;
    public abstract boolean ipvsdb_Unsubscribe(DBParams dbParams) throws Exception ;
    
    //////////////Async ipvsdb getSubscriptionList //////////////////
    public abstract Document ipvsdb_getSubscribers(DBParams dbParams) throws Exception ;
    public abstract Document ipvsdb_getSubscriptions(DBParams dbParams) throws Exception ;
    
    //////////////Async ipvsdb get //////////////////
    public abstract Element ipvsdb_getData(DBParams dbParams) throws Exception ;
    
    //////////////Async ipvsdb add //////////////////
    public abstract Document ipvsdb_setData(DBParams dbParams) throws Exception ;
    public abstract Document ipvsdb_addNode(DBParams dbParams) throws Exception ;
    public abstract Document ipvsdb_addNodeWithNodeID(DBParams dbParams) throws Exception ;

    //////////////Async ipvsdb deleteNode //////////////////
    public abstract Document ipvsdb_deleteNode(DBParams dbParams) throws Exception ;
    
    ////////////// Async ipvsdb DiscoItems //////////////////
    public abstract boolean ipvsdb_nodeExists(DBParams dbParams) throws Exception ;
    public abstract List<String> ipvsdb_getChildren(DBParams dbParams) throws Exception ;
    
    ////////////// Async ipvsdb importDB exportDB //////////////////
    public abstract Document ipvsdb_importDB(DBParams dbParams) throws Exception ;
    public abstract Document ipvsdb_exportDB(DBParams dbParams) throws Exception ;
    public abstract Document ipvsdb_deleteDB(DBParams dbParams) throws Exception ;
        
    public long getServerTimeOffset() ;

    
    
    public abstract XMPPEventQueue getXMPPEventQueue(String userJID) ;
    public abstract void releaseXMPPEventQueue(XMPPEventQueue queue) ;
    public abstract void addEvent(String fromJID, String message);
    public abstract void handleEvent(String fromJID, String message);

    
    ///////////////////// new //////////////////////////////
    //public void customPubSubSet(String filename) ;
    //public void customPubSubGet(String filename)  ;
    
    // File Transfer routines
	public abstract void getFileStatus(String toJID, String srcFileName,
			String destFileName);
	public abstract void sendFileStatus(String toJID, String fileName);
	public abstract void getFileStop(String fromJID, String srcFileName,
			String destFileName);
	public abstract void sendFileStop(String toJID, String fileName);
	void sendFile(String toJID, String fileName, String fileXferID);
	void getFile(String fromJID, String srcFileName, String destFileName,
			String fileXferID);

	//public PresenseHandler getPresenseHandler();
    public boolean isUserPresenceAvailable(String touser) ;
    public void subscribeUserPresence(String touser) throws Exception ;
    public void unSubscribeUserPresence(String touser) throws Exception ;
	
	////////
    public abstract String iq_request(String description, Packet packet, long timeout) throws Exception ;
    public abstract void addPacketListener(PacketListener packetListener, PacketFilter packetFilter);
    public abstract void removePacketListener(PacketListener packetListener);
    public abstract PacketCollector createPacketCollector(PacketFilter packetFilter) ;
    public abstract void addRequest(Packet xmppPacket, XMPPRequestHandler p) ;
    public abstract void delRequest(XMPPRequestHandler p) ;
    //new added to check file transfer status
    public abstract void addRequest(String id, XMPPRequestHandler p) ; 
    public abstract XMPPRequestHandler getRequest(String id) ; 
    public abstract List<String> getRequestIDs() ; 
    public abstract List<String> getRequestIDs_fileTransfer() ; 
    
    public IPVSServiceDiscoveryManager getIPVSServiceDiscoveryManager() ;
    public void setIPVSServiceDiscoveryManager(IPVSServiceDiscoveryManager serviceDiscoveryManager) ;    
    ////////////////// packet listener/collector for component //////////start////
    public abstract String getIPVSConnectionType();
    public abstract void loginComplete();
    public abstract void logoutComplete();  
    public abstract String getLoginType() ;
    public abstract boolean isFileTransferToLocalIP(String jid) throws Exception ;

}
