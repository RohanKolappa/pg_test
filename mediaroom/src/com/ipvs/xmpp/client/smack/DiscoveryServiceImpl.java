package com.ipvs.xmpp.client.smack;

import java.io.IOException;
import java.util.StringTokenizer;

import javax.jmdns.JmDNS;
import javax.jmdns.ServiceEvent;
import javax.jmdns.ServiceInfo;

import org.dom4j.Document;

import com.ipvs.client.ServiceAgentUtils;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.XMPPEvent;
import com.ipvs.common.XMPPEventQueue;
import com.ipvs.common.service.MRXMLUtils;

public class DiscoveryServiceImpl implements DiscoveryServiceI {

	protected XMPPEventQueue xmppEventQueue = null;
	private boolean runDiscoveryService = true;
	private JmDNS jmdns = null;
	private String diService = "_BarcoDI._tcp.local.";
	private String userJID = "";
	
	public DiscoveryServiceImpl(XMPPEventQueue xmppEventQueue, boolean runDiscoveryService) {
		this.xmppEventQueue = xmppEventQueue;
		this.runDiscoveryService = runDiscoveryService;
	}
	
	@Override
	public void startDiscoveryService() {
		try {
			jmdns = JmDNS.create();
			jmdns.addServiceListener(diService, this);
		} catch (IOException e) {
			e.printStackTrace();
		}

	}

	@Override
	public void stopDiscoveryService() {
		try {
			jmdns.removeServiceListener(diService, this);
			jmdns.close();
		} catch (Exception e) {
			e.printStackTrace();
		}

	}

	@Override
	public void serviceAdded(ServiceEvent event) {
		event.getDNS().getServiceInfo(event.getType(), event.getName(), 0);
	}

	@Override
	public void serviceRemoved(ServiceEvent event) {
		//System.out.println("JmDNSSessionImpl: ServiceRemoved : " + event.getName() + "." + event.getType());
		String notificationXML = this.createNotification(event, false);
		this.addEvent(this.userJID, notificationXML);
	}

	@Override
	public void serviceResolved(ServiceEvent event) {
		//System.out.println("JmDNSSessionImpl: ServiceResolved : " + event.getInfo());
		String notificationXML = this.createNotification(event, true);
		this.addEvent(this.userJID, notificationXML);
	}
	
	@Override
	public String getUserJID() {
		return this.userJID;
	}
	
	@Override
	public void setUserJID(String userJID) {
		this.userJID = userJID; 
	}
//////////////////////////XMPPEventQueue ///////////////////////////////
	

	public synchronized XMPPEventQueue getXMPPEventQueue(String userJID) {

		if (xmppEventQueue == null)
			xmppEventQueue = new XMPPEventQueue(userJID);

		return xmppEventQueue;

	}

	public synchronized void setXMPPEventQueue(XMPPEventQueue eventQueue) {
		this.xmppEventQueue = eventQueue;
	}

	public void addEvent(String fromJID, String message) {

		if (xmppEventQueue != null) {
			xmppEventQueue.addXMPPEvent(new XMPPEvent(fromJID, message));
		}
	}
	
	public String createNotification(ServiceEvent event, boolean available) {
		
		try {
			
			ServiceInfo info = event.getInfo();
			String registered = "No";
			String macaddress = "00:00:00:00:00:00";
			String deviceName = "";
			String deviceType = "";
			String hostAddress = "";
			int port = 0;
					
			if(info != null) {
				String serviceName = info.getName();
				if(info.getHostAddress() != null)
					hostAddress = info.getHostAddress();
				 
				port = info.getPort();
				
				String txtRecord = info.getTextString();
				if(txtRecord != null) {
					StringTokenizer st = new StringTokenizer(txtRecord.trim());
					
					while (st.hasMoreElements()) {
						String object = (String) st.nextElement();
						String [] values = object.split("=");
						if(values != null && values.length == 2) {
							registered = values[0].trim().startsWith("Registered") ? values[1].trim() : registered;
							macaddress = values[0].trim().startsWith("macaddress") ? values[1].trim() : macaddress;
						}
					}
				}
				String [] names = serviceName.split("\\s+");
				if(names != null && names.length == 3) {
					deviceName = names[2];
					deviceType = names[1];
				}
				else{
					return ""; // Drop events for devices not conforming to naming scheme.
				}
			}
			
			String data = "<Event>"
					+ "<Header serviceName='Discovery' eventName='DiscoveryEvent' eventAgentJID='' eventWallclock='' eventLevel=''/>"
					+ "<Data>" + "<DiscoveryEventData name='" + deviceName + "' ipaddress='" + hostAddress + "'" 
					+ " serviceType='" + event.getType() + "' registered='"+ registered +"' available='"+ available +"' type='"+ deviceType +"' macaddress='"+ macaddress +"' port= '" + port + "'/>"
 					+ "</Data></Event>";

			String from = deviceName + "@discovery/" + deviceType;
			String agentMessage = ServiceAgentUtils.getAgentMessage(from, "", "", this.userJID,ServiceClient.EVENT, data, "");
			 
			Document msgDoc = MRXMLUtils.stringToDocument(agentMessage);
			MRXMLUtils.removeNameSpace(msgDoc.getRootElement());
			return "<message from='" + from + "' to='" + this.userJID + "'>"  + MRXMLUtils.documentToStringOmitDeclaration(msgDoc) + "</message>";
			
		} catch (Exception e) {
			e.printStackTrace();
		}
		return "";
	}

}
