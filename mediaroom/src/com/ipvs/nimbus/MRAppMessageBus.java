package com.ipvs.nimbus;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import org.apache.log4j.Level;
import org.dom4j.Document;
import org.dom4j.Element;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.packet.Packet;

import com.ipvs.common.XMPPEventQueue;
import com.ipvs.common.XMPPI;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.params.DBParams;
import com.ipvs.xmpp.client.smack.IPVSServiceDiscoveryManager;
import com.ipvs.xmpp.client.smack.XMPPRequestHandler;

public class MRAppMessageBus implements XMPPI {

	MRMessageBusI mrComponentMessageBus;
	// Protocol
	public static final String XMPPIQ = "xmppiq";
	public static final String XMPPMessage  = "xmppmessage";
	public static final String HTTP = "http";
	
	String superUser = null;
	
	MRAppMessageBus(MRMessageBusI mrComponentMessageBus) {
		this.mrComponentMessageBus = mrComponentMessageBus;
	}
	
	@Override
	public String getServiceDomain() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void setServiceDomain(String serviceDomain) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setSuperUser(String superUser) {
		this.superUser = superUser;
	}

	@Override
	public String getSuperUser() {
		return this.superUser;
	}

	@Override
	public String getXMPPServerName() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String getXMPPUserJid() {
		return this.mrComponentMessageBus.getJID();
	}

	@Override
	public String getUser() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean isConnected() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public String getServiceName() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void start() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void stop() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void login(String serverip, int port, String userJID,
			String password, String resource) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void logout() throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public long getCurrentTimeMillis() {
		return System.currentTimeMillis();
	}

	@Override
	public void sendMessage(String userJID, String message,
			String extensionNamespace) throws Exception {
		
		// Remove the xmlns header
		int beginIndex = message.indexOf(">") + 1;
		int endIndex = message.indexOf("</x>");
		String agentMessage = message.substring(beginIndex, endIndex);
		
		Document messageDoc = MRXMLUtils.stringToDocument(agentMessage);
		MRSCMessage mrSCMessage = null;
		String clientData = null;
		String clientCallback = null;
		String from = this.getXMPPUserJid();
		String to = userJID;
		Level logLevel = Level.INFO;
		if("AgentMessage".equals(messageDoc.getRootElement().getName())) {
			String type = messageDoc.getRootElement().attributeValue("type");
			Element headerElement = MRXMLUtils.getElementXpath(messageDoc, "//Header");
			String serviceName = headerElement.attributeValue("serviceName");
			
			messageDoc.getRootElement().addAttribute("from", from);

			String rootName = null;
			String protocol = XMPPMessage;
			int state = MRSCMessage.REQUEST_STATE_DONE;
			if(MRSCMessage.REQUEST.equals(type) || MRSCMessage.RESPONSE.equals(type)) {
				rootName = headerElement.attributeValue("requestName");
				rootName = rootName.replace("Request", ""); // Strip the Request Suffix
				clientData = MRXMLUtils.getValueXpath(messageDoc, "//ClientData", "");
				clientCallback = MRXMLUtils.getValueXpath(messageDoc, "//ClientCallback", "");
				if(MRSCMessage.RESPONSE.equals(type)) {
					Element rootElement = MRXMLUtils.getElementXpath(messageDoc, "//" + rootName);
					// if(rootElement != null) { 
					// This was an IQ request
					if(serviceName.startsWith("sessionservice")){
						protocol = XMPPIQ;
					}
					// }
					String stateStr = headerElement.attributeValue("state");
					state = Integer.parseInt(stateStr);
				}
			}
			else { // Is EVENT
				rootName = headerElement.attributeValue("eventName");			
			}

			// String serviceName = "Layout"
			// String rootName = "Setup";
			mrSCMessage = new MRSCMessage(
					protocol, messageDoc, type, serviceName, rootName, 
					clientData,clientCallback, from, to, logLevel);
			mrSCMessage.setState(state);
			mrSCMessage.setExtNamespce(extensionNamespace);
		}
		else { // This is a notification
			String type = MRSCMessage.NOTIFICATION;
			String serviceName = null;
			String rootName = messageDoc.getRootElement().getName();
			mrSCMessage = new MRSCMessage(
					XMPPMessage, messageDoc, type, serviceName, rootName, 
					clientData,clientCallback, from, to, logLevel);
			String nodeID= MRXMLUtils.getAttributeValueXpath(messageDoc, rootName, "nodeID", "");
			String id = nodeID + "__" + userJID + "__" + Utils.randomString(5);
	        mrSCMessage.setId(id);
	        mrSCMessage.setExtNamespce(extensionNamespace);
			
		}
		
		// Hack to prevent the DeviceLogin AsyncResponse from looping
		String selfJID = this.mrComponentMessageBus.getJID();
		if( (selfJID.equals(to) && selfJID.equals(from) && ("DeviceLogin".equals(mrSCMessage.getRootName()))))
			return; // Do not send this message
		
		this.mrComponentMessageBus.sendMessage(mrSCMessage);
	}

	@Override
	public void sendPacket(Packet p) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void subscribePresence(String userJID) throws Exception {
		this.mrComponentMessageBus.subscribeUserPresence(userJID);
	}

	@Override
	public void addRoom(String roomNID, String nickname) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void sendInvite(String roomNID, String toJID) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void joinRoom(String roomNID, String nickname) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean isJoined(String roomNID) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void deleteRoom(String roomNID) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void sendRoomMessage(String roomNID, String message,
			String extensionNamespace) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean isAvailable(String roomNID, String userJID) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void leaveRoom(String roomNID, String userJID) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public ArrayList<String> getRoomNIDList() {
		return new ArrayList<String>();
	}

	@Override
	public Object ipvsdb_request(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean ipvsdb_Subscribe(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean ipvsdb_updateSubscriptions(DBParams dbParams)
			throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean ipvsdb_Unsubscribe(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public Document ipvsdb_getSubscribers(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document ipvsdb_getSubscriptions(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Element ipvsdb_getData(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document ipvsdb_setData(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document ipvsdb_addNode(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document ipvsdb_addNodeWithNodeID(DBParams dbParams)
			throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document ipvsdb_deleteNode(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean ipvsdb_nodeExists(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public List<String> ipvsdb_getChildren(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document ipvsdb_importDB(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document ipvsdb_exportDB(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Document ipvsdb_deleteDB(DBParams dbParams) throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public long getServerTimeOffset() {
		// TODO Auto-generated method stub
		return 0;
	}

	@Override
	public XMPPEventQueue getXMPPEventQueue(String userJID) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void releaseXMPPEventQueue(XMPPEventQueue queue) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void addEvent(String fromJID, String message) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleEvent(String fromJID, String message) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void getFileStatus(String toJID, String srcFileName,
			String destFileName) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void sendFileStatus(String toJID, String fileName) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void getFileStop(String fromJID, String srcFileName,
			String destFileName) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void sendFileStop(String toJID, String fileName) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void sendFile(String toJID, String fileName, String fileXferID) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void getFile(String fromJID, String srcFileName,
			String destFileName, String fileXferID) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public boolean isUserPresenceAvailable(String touser) {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void subscribeUserPresence(String touser) throws Exception {
		this.mrComponentMessageBus.subscribeUserPresence(touser);
	}

	@Override
	public void unSubscribeUserPresence(String touser) throws Exception {
		this.mrComponentMessageBus.unSubscribeUserPresence(touser);
	}

	@Override
	public String iq_request(String description, Packet packet, long timeout)
			throws Exception {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void addPacketListener(PacketListener packetListener,
			PacketFilter packetFilter) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void removePacketListener(PacketListener packetListener) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public PacketCollector createPacketCollector(PacketFilter packetFilter) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void addRequest(Packet xmppPacket, XMPPRequestHandler p) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void delRequest(XMPPRequestHandler p) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void addRequest(String id, XMPPRequestHandler p) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public XMPPRequestHandler getRequest(String id) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public List<String> getRequestIDs() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public List<String> getRequestIDs_fileTransfer() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public IPVSServiceDiscoveryManager getIPVSServiceDiscoveryManager() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void setIPVSServiceDiscoveryManager(
			IPVSServiceDiscoveryManager serviceDiscoveryManager) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public String getIPVSConnectionType() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void loginComplete() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void logoutComplete() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public String getLoginType() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean isFileTransferToLocalIP(String jid) throws Exception {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public void addDiscoFeature(String name) {
		mrComponentMessageBus.addDiscoFeature(name);
	}

}
