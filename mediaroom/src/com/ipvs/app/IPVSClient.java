package com.ipvs.app;

import java.lang.reflect.Method;
import java.util.Iterator;
import java.util.Vector;

import org.dom4j.Document;
import org.dom4j.Element;
import org.jivesoftware.smack.ConnectionConfiguration;
import org.jivesoftware.smack.ConnectionListener;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.XMPPConnection;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.filter.PacketTypeFilter;
import org.jivesoftware.smack.packet.Message;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.StreamError;
import org.jivesoftware.smack.packet.IQ.Type;

import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;
import com.ipvs.xmpp.client.smack.packets.ResponseElement;
import com.ipvs.xmpp.client.smack.packets.SessionIQ;

public class IPVSClient extends XMPPConnection implements PacketListener,ConnectionListener {

	String appServerJID;
	IPVSMessageHandler msgHandler;
	static final String APPSERVER_DISCOVERY_IQ = 
		"<iq id='" + IPVSClient.class.getName() + "' to='ipvsdbservice.localhost' type='get'><request xmlns='ipvs:mediaroom:ipvsdbservice'><readSingle domain='default'  nodeID='default.serviceresourcelist'/></request></iq>";
	Vector pending;

	public IPVSClient(ConnectionConfiguration config, IPVSMessageHandler handler) throws XMPPException {
		super(config);		
		this.msgHandler = handler;
		this.appServerJID = null;
		this.pending = new Vector();
	}


	public static String parseAppServerJID(String xml) throws Exception {
		String ownerUserJID = null;
		Document doc = IPVSClientUtils.stringToDocument(xml);
		Element iq = IPVSClientUtils.getElementXpath(doc, "//iq");
		String id = IPVSClientUtils.getAttributeValue(iq,"id");
		if(!IPVSClient.class.getName().equals(id))
			return ownerUserJID;

		Element responseElement = IPVSClientUtils.getElementXpath(doc, "//x:response",ResponseElement.IPVSDBSERVICE);
		Element el = responseElement.element("DBResp").element("ServiceResource").element("Contact").element("OwnerUserJID");
		ownerUserJID = el.getStringValue();
		return ownerUserJID;

	}
	
	private synchronized String getAppServerJID() {
		return this.appServerJID;
	}
	
	private String getAppServerNotFoundError(){
		return "<xmpperror code='AppServerNotFound'/>";
	}
	
	private synchronized void setAppServerJID(String appServerJID) {
		System.out.println("IPVSClient:setAppServerJID appServerJID=" + appServerJID);
		if("".equals(appServerJID)){
			System.out.println("AppServerJID Empty. Return Error");
			this.msgHandler.handleMessage(getAppServerNotFoundError());
			this.pending.clear();
			return;
		}
		this.appServerJID = appServerJID;
		Iterator i = this.pending.iterator();
		while(i.hasNext()) {
			String messageXML = (String)i.next();
			try {
				this.sendPacket(messageXML, this.appServerJID);
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}
		this.pending.clear();
	}
	
	private synchronized void addToPending(String messageXML) {
		System.out.println("IPVSClient:addToPending messageXML=" + messageXML);
		this.pending.add(messageXML);
	}
	
	public void processPacket(Packet packet) {					
		String response = packet.toXML();
		if(this.getAppServerJID() == null) {
			try {
				String jid = IPVSClient.parseAppServerJID(response);
				if(jid != null)
					this.setAppServerJID(jid);
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}
		if(this.msgHandler != null)
			this.msgHandler.handleMessage(response);
	}

	public static IPVSClient login(String serverName, int port,
			String user, String password, String resourceID, IPVSMessageHandler handler) throws XMPPException{

		ConnectionConfiguration config = new ConnectionConfiguration(serverName, port);
		IPVSClient client = new IPVSClient(config, handler);

		long toTime = System.currentTimeMillis();


		//client.connect();
		System.out.println("IPVSClient connect DONE "+(System.currentTimeMillis() -toTime) );
		toTime = System.currentTimeMillis();

		PacketFilter filter = new PacketTypeFilter(Packet.class);

		client.login(user, password, resourceID);
		client.addPacketListener(client, filter);
		client.addConnectionListener(client);
	    String userJID = user + "@localhost"   + "/" + resourceID;				
		
		sendAppserverDiscoveryIQ(userJID, client);

		return client;

	}
		
	private static void sendAppserverDiscoveryIQ(String userJID,IPVSClient client) {
		try {
			Document appserverDiscoMessageDoc = IPVSClientUtils.stringToDocument(IPVSClient.APPSERVER_DISCOVERY_IQ);
			Element requestEL = (Element) IPVSClientUtils.getElementXpath(appserverDiscoMessageDoc,"//x:request",ResponseElement.IPVSDBSERVICE);
			requestEL.element("readSingle").addAttribute("subscribeJID", userJID);
			client.sendPacket(appserverDiscoMessageDoc.asXML(), "ipvsdbservice.localhost");			
		}catch(Exception e) {
			e.printStackTrace();
		}
	}


	public String sendPacket(String messageXML) throws Exception{
		String defaultJID = this.getAppServerJID();
		if(defaultJID == null) {
			this.addToPending(messageXML);
			return "queued";
		}
		if("".equals(defaultJID)){
			return getAppServerNotFoundError();
		}
		return this.sendPacket(messageXML, defaultJID);
	}
	
	private String sendPacket(String messageXML, String defaultJID) throws Exception{

		Document doc = IPVSClientUtils.stringToDocument(messageXML);
		if("iq".equals(doc.getRootElement().getName())) {
			
			Element iq = IPVSClientUtils.getElementXpath(doc, "//iq");
			String id = IPVSClientUtils.getAttributeValue(iq,"id");
			String to = IPVSClientUtils.getAttributeValue(iq,"to");
			String type = IPVSClientUtils.getAttributeValue(iq,"type");
			if((to == null) || ("".equals(to)))
				to = defaultJID;
			
			if(iq == null || id == null || type == null){
				System.out.println("Bad request id=" + id + " type=" + type);
				return "Error";
			}
			Element queryElement = IPVSClientUtils.getElementXpath(doc, "//iq/x:query");
			if(queryElement != null){
				String queryElementXML = IPVSClientUtils.elementToString(queryElement);
				sendIQPacket(id, to ,type, queryElementXML);
				return "iq";
			}
			Element requestElement = IPVSClientUtils.getElementXpath(doc, "//iq/x:request","ipvs:mediaroom:ipvsdbservice");
			if(requestElement != null){
				String requestElementXML = IPVSClientUtils.elementToString(requestElement);
				sendIQPacket(id, to,type, requestElementXML);
				return "iq";
			}
			System.out.println("Bad request no query or request element");
			return "Error";
		}
		else
		{
			String agentJID = IPVSClientUtils.getValueXpath(doc, "//agentmessageheader/agentJID", "");
			if("".equals(agentJID))
				agentJID = defaultJID;
			String message = "<x xmlns='" + IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS + "'>" + messageXML + "</x>";
			sendMessagePacket(agentJID, message, IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS);	
			return agentJID;
		}
	}

	private void sendIQPacket(String id, String to, String type, String queryElementXML) throws Exception {
		SessionIQ sessionElement = new SessionIQ();
		sessionElement.setPacketID(id);
		sessionElement.setTo(to);
		if(type.equals("get")){
			sessionElement.setType(Type.GET);
		}
		else{
			sessionElement.setType(Type.SET);
		}
		sessionElement.setChildElementXML(queryElementXML);

		sendPacket(sessionElement);

	}
	
	private void sendMessagePacket(String to, String message, String extensionNamespace) throws Exception {
		Message m = new Message();
		m.setTo(to);
		if(extensionNamespace == null)
			m.setBody(message);
		else {			
			IPVSMessageExtension ipvsMessageExtension = new IPVSMessageExtension(extensionNamespace);
			ipvsMessageExtension.setXML(message);
			m.addExtension(ipvsMessageExtension);
		}
		sendPacket(m);
	}
	
	
	public static void logout(IPVSClient ipClient) {
		try {
        Method methodObj =ipClient.getClass().getMethod("close", null);
        methodObj.invoke(ipClient, null);
		} catch(Exception exp){
			exp.printStackTrace();
		}
		//ipClient.close();		
	}


	
	public void connectionClosed() {
	}


	
	public void connectionClosedOnError(Exception ex) {
	//	System.out.println((ex instanceof XMPPException)+"connectionClosedOnError"+ex.getMessage());
		String reason = "";
		if (ex != null && ex instanceof XMPPException) {
            XMPPException xmppEx = (XMPPException)ex;
            StreamError error = xmppEx.getStreamError();
            reason = error.getCode();   
            this.msgHandler.handleMessage("<xmpperror code='"+reason+"'/>");
        } else {
        	 reason = ex.getMessage();
        	 this.msgHandler.handleMessage("<xmpperror code='"+reason+"'/>");

        }
	}



	public void reconnectingIn(int arg0) {		
	}



	public void reconnectionFailed(Exception arg0) {
	}



	public void reconnectionSuccessful() {
	}
	
	/*
	public static void main(String[] args) throws Exception {

		IPVSClient client = IPVSClient.login("10.1.5.190", 5222, "dcuser1", "dcuser1", "dcpc1", null);
		BufferedReader input = new BufferedReader(new InputStreamReader(System.in));
		String loginXML = input.readLine();
		System.out.println("appserverJID=" + client.appServerJID);
		IPVSClient.logout(client);
	}
	*/
}
