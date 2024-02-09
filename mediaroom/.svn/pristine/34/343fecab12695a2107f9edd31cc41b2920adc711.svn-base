package com.ipvs.test;

import java.util.Random;

import org.dom4j.Document;
import org.jivesoftware.smack.ConnectionConfiguration;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.XMPPConnection;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.filter.ToContainsFilter;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.IQ.Type;

import com.ipvs.common.service.MRXMLUtils;

public class TestXMPPUserRegistration extends Thread implements PacketListener{
	TestXMPPUserRegistration(){}
	static PacketCollector packetcollecter ;
	static String username = "padmaja"+new Random().nextInt();
	static XMPPConnection connection = null;
	static String userJID = null;
	public static void main(String[] args) {
		ConnectionConfiguration config = new ConnectionConfiguration("127.0.0.1", 5222);
		connection = new XMPPConnection(config);

		try {
			connection.connect();
			System.out.println("Connected to XMPP 10.51.51.65");

			IQ packet= new IQ() {					
				@Override
				public String getChildElementXML() {
				/*	return  "<query xmlns='jabber:iq:register'>" +
							" <username>"+username+"</username>"+
							" <password>Ipvs1234</password>"+
							" <fullname>Padmaja Vattikuti</fullname>"+
							" <email>padmaja@barco.com</email>"+
							" </query> ";*/
					return  "<query xmlns='jabber:iq:register'> </query> ";
				}
			};
			packet.setType(Type.GET);
			String serviceName = connection.getServiceName();
			System.out.println("serviceName :="+serviceName);
			PacketFilter filter = new ToContainsFilter(serviceName);
			userJID = username +"@" + connection.getServiceName()+"/"+username;
			connection.addPacketListener(new TestXMPPUserRegistration(), null);

			//	Thread  thread = new Thread(new TestXMPPUserRegistration(), "IPVSPacketCollector");
			//			thread.start();

			
			packet.setFrom(userJID);
			//packet.setTo("component.localhost");
			System.out.println("Send Regitration Packet " + packet.toXML());
			connection.sendPacket(packet);
			
			Thread.sleep(10000);
		} catch (XMPPException e) {
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		connection.disconnect();
		System.out.println("DisConnected from XMPP");

	}

	//	public void run(){
	//		System.out.println("In RunMethod");
	//		int counter = 1;
	//		while(counter <=5) {
	//			Packet packet = packetcollecter.pollResult();
	//			System.out.println("packet :="+packet);
	//			if(packet !=null) {
	//				System.out.println("Reply Packet:="+packet.toXML());
	//			}
	//			try {
	//				Thread.sleep(500);
	//			} catch (InterruptedException e) {				
	//				e.printStackTrace();
	//			}
	//			counter ++;
	//	}

	@Override
	public void processPacket(Packet packet) {

		System.out.println("Reply From Server :" + packet.toXML());

		try {
			Document document  = MRXMLUtils.stringToDocument(packet.toXML());
			MRXMLUtils.removeNameSpace(document.getRootElement());
			String xmppTitle = MRXMLUtils.getValueXpath(document, "//title", "");
			if(xmppTitle.equals("XMPP Client Registration")){				
			
			IQ packet1= new IQ() {					
				@Override
				public String getChildElementXML() {
				return  "<query xmlns='jabber:iq:register'>" +" <username>"+username+"</username>"+
						" <password>Ipvs1234</password>"+
						" <name>Padmaja Vattikuti</name>"+
						" <email>padmaja@barco.com</email>"+
							"<x xmlns='jabber:x:data' type='submit'>"+
							"<field type='hidden' var='FORM_TYPE'>"+
							" <value>jabber:iq:register</value>"+
							"</field>"+
							"<field label=\"Username\" var=\"username\" type=\"text-single\"><value>"+username+"</value></field>"+
							"<field label=\"Full name\" var=\"name\" type=\"text-single\"><value>Padmaja Vattikuti</value></field>"+
							"<field label=\"Email\" var=\"email\" type=\"text-single\"></field>"+
							"<field label=\"Password\" var=\"password\" type=\"text-private\"><value>Ipvs1234</value></field>"+
							"</x> </query> ";
					
					/*return  "<query xmlns='jabber:iq:register'>" +
					" <username>"+username+"</username>"+
					" <password>Ipvs1234</password>"+
					" <name>Padmaja Vattikuti</name>"+
					" <email>padmaja@barco.com</email>"+
					" </query> ";*/
				}
			};
			String serverJID = packet.getFrom();
			System.out.println("serverID:="+serverJID);
			packet1.setFrom(userJID);
			packet1.setTo(serverJID);
			packet1.setType(Type.SET);
			System.out.println("packet1:="+packet1.toXML());			
			connection.sendPacket(packet1);
			}


		} catch (Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
