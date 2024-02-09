package com.barco.nimbus.messagebus.xmpp;
import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;
// import org.xmpp.component.ComponentException;
import org.xmpp.packet.Presence;

public class XMPPComponentTest implements XMPPComponentListener{

	public static void main(String[] args) {
		XMPPComponent xc = new XMPPComponent();
		// XMPPAbstractComponent xc = new XMPPAbstractComponent();
		// DummyAbstractComponent xc = new DummyAbstractComponent();

		String serverIP = "10.51.51.72";
		int serverPort = 5275;
		// String domain = "nimbus";
		// String secret = "nimbus";
		String domain = "managementserver";
		String secret = "test";
		try {
			xc.login(serverIP, serverPort, domain, secret);
			System.out.println("LoggedIn");
			try {
				Thread.sleep(100000);
			}
			catch(Exception e) {
				System.out.println("Exception =" + e.toString());
			}
			xc.logout();
			System.out.println("LoggedOut");
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}

	@Override
	public void handleDisconnected() {
		System.out.println("XMPPComponentTest:handleDisconnected");
	}

	@Override
	public void handleMessage(Message message) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleIQ(IQ iq) {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handlePresence(Presence presence) {
		// TODO Auto-generated method stub
		
	}

}
