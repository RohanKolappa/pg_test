package com.barco.device.handler.DeviceAdmin;

import java.io.File;
import java.util.HashMap;
import java.util.Random;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.jivesoftware.smack.ConnectionConfiguration;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.XMPPConnection;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.packet.IQ.Type;
import org.jivesoftware.smack.packet.Packet;
import org.jivesoftware.smack.packet.Registration;

import com.barco.device.base.DeviceI;
import com.barco.device.base.LoginInfo;
import com.barco.device.base.LoginInfo.LoginInfoEvents;
import com.barco.device.base.MessageHandlerI;
import com.barco.device.common.Message;
import com.barco.utils.XMLUtils;

/**
 * Spawns a thread to sends a self-registration 
 * {@link org.jivesoftware.smack.packet.Registration.Registration()}
 * to the SC (identified by the event data).
 */
public class ServiceDiscoveryEventHandler extends BaseEventHandler implements MessageHandlerI, PacketListener {

	XMPPConnection connection = null;
	int xmppPort = 5222; // default xmpp port
	SelfRegistration registration;
	private DeviceI device = null;
	private static Logger logger = null;
	private static String TYPE_DISCOVERY_COMPLETE = "SCDiscoveryComplete";

    public ServiceDiscoveryEventHandler() {}
   
	@Override
	public void handleMessage(Message message, DeviceI device) throws Exception {
	    
		//To Remove the reannounce file created by deviceapp
		File reannounceSC = new File("/var/status/reannounce.sc");
		
		setLogger(device.getLogger());
		setDevice(device);		
		String ipaddress = "";
		String foundSC = "false";
		Document msgDoc = message.getMessageDoc();
		
		Document deviceConfig = device.getDeviceDB().getDeviceConfig();		
		String xmppAgentConfigEnable = XMLUtils.getValueXpath(deviceConfig, "//XMPPAgentServiceConfig/Enable", "false");
		if(!String.valueOf(false).equals(xmppAgentConfigEnable)) {
			getLogger().info("ServiceDiscoveryEventHandler: handleMessage: Cannot process Discovery Events when xmpp is enabled!");
			if(reannounceSC.exists())
				reannounceSC.delete();
			
			return; // Should not process Discovery Events if XMPP is enabled.
		}
		
		
		ipaddress = XMLUtils.getValueXpath(msgDoc, "//ipaddress", "0.0.0.0");
		foundSC = XMLUtils.getValueXpath(msgDoc, "//foundsc", "false");
		String prevSCipaddr = device.getLoginInfo().getIpAddress();
		String deviceStatus = device.getLoginInfo().getLoginState();
		if(prevSCipaddr != null) {
			if(prevSCipaddr.equals(ipaddress) && foundSC.equals(String.valueOf(true)) && deviceStatus.equals((LoginInfo.LoginStateEnum.LoggedIn).toString())) {
				getLogger().info("ServiceDiscoveryEventHandler: handleMessage: Found the same SC again, ignoring the discovery!");
				
				if(reannounceSC.exists())
					reannounceSC.delete();
				
				return; //found the same SC again, ignore.
			}
				
		}
		
		String eventType = XMLUtils.getValueXpath(msgDoc, "//EventType", "");
		if(!TYPE_DISCOVERY_COMPLETE.equals(eventType) || foundSC.equals(String.valueOf(false))){
			//Did not find SC or some error.
			String error = XMLUtils.getValueXpath(msgDoc, "error", "");
		    getLogger().info("ServiceDiscoveryEventHandler: handleMessage: Service Discovery did not find anything, will try device logout ");
			if(!error.isEmpty())
				getLogger().error(error);
			
			// try to logout the device if it was loggedin and reset the IP
			device.logout();
			device.getLoginInfo().setIpAddress(null);
				
			return;
		}	
		
		//check last logged in server ip and currently found new sc ip is not same then set login event as discovered new event. 
		// this state is used to resets the agents.
		if(!ipaddress.equals(device.getLoginInfo().getLastLoggedInServerIP())){
			device.getLoginInfo().setLastEvent(LoginInfoEvents.DiscoveredNewSC);
		}
		
		device.getLoginInfo().setLastLoggedInServerIP(ipaddress);
		String deviceName = null;
		String password = null;
		try {
			deviceName = XMLUtils.getValueXpath(msgDoc, "//devicename","");
			password = XMLUtils.getValueXpath(msgDoc, "//password","");
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		if(deviceName.isEmpty()) {
		    //check if box is in factory default mode
		    /*
		     * if /etc/config/factory file exists, box is in factory default mode
		     * use the default deviname and generate password
		     */
		    	
			String serialNumber = XMLUtils.getValueXpath(msgDoc, "//serialnumber", "");
			String deviceType = device.getDeviceDB().getDeviceType();
			deviceType = deviceType.replace("Device", "");
			if(serialNumber.isEmpty()) {
				serialNumber = String.valueOf(new Random().nextInt(Integer.MAX_VALUE) + 1);
			}
		    
			//factory default mode
			if(new File("/etc/config/factory").isFile() || !(deviceType.startsWith("MS") || deviceType.startsWith("DMS"))){
			    deviceName = deviceType.toLowerCase()+serialNumber;
			    password  = String.valueOf(new Random().nextInt(Integer.MAX_VALUE) + 1);
			}
			//bootstrap UI/CLI is used to change the factory default settings
			/*
			 * get the xmpp username and password from deviceconfig
			 * devicename: devicename+serialnumber
			 * 
			 * XPs and TXs do not have the bootstrap UI yet, so use
			 * devicename as devicetype+serial_number.
			 */
			else {
				deviceName = XMLUtils.getValueXpath(deviceConfig, "//XMPPAgentServiceConfig/DeviceName", "")+"-"+serialNumber; 
				password = XMLUtils.getValueXpath(deviceConfig, "//XMPPAgentServiceConfig/DevicePassword", String.valueOf(new Random().nextInt(Integer.MAX_VALUE) + 1));
			} 
		}		

		ConnectionConfiguration config = new ConnectionConfiguration(ipaddress, xmppPort);
		connection = new XMPPConnection(config);
		getLogger().info("ServiceDiscoveryEventHandler: handleMessage: Connecting to XMPP server at " + ipaddress + " for user " + deviceName);
		registration = new SelfRegistration(ipaddress, deviceName, password, connection, this);
		Thread th = new Thread(registration);
		th.start();			
		
		// remove file
		if(reannounceSC.exists())
			reannounceSC.delete();
	}


	@Override
	public void processPacket(Packet packet) {
		String pktStr = packet.toXML();
		if(!pktStr.isEmpty()) {
			String ip = registration.getIpaddress();
			String uname = registration.getUsername();
			String pwd = registration.getPassword();
			registration.setResetThread(true);
			
			if(packet.getError() == null) { // if no error, registration succeeded. Re-login using new credentials.
				getLogger().info("ServiceDiscoveryEventHandler: processPacket: Received result packet, Attempting to re-login for user " + uname);
				this.relogin(ip, uname, pwd, getDevice());
			}
			
		}
	}
	
	
	DeviceI getDevice() {
		return this.device;
	}

	void setDevice(DeviceI deviceI) {
		this.device = deviceI;
	}


	static Logger getLogger() {
		return logger;
	}

	static void setLogger(Logger logger) {
		ServiceDiscoveryEventHandler.logger = logger;
	}


	private class SelfRegistration implements Runnable {
		
		public String ipaddress;
		public String username;
		public String password;
	    private XMPPConnection xmppConnection;
	    private PacketListener listener;
	    private volatile boolean resetThread = false;
		
	    String userJID = null;
	    HashMap<String, String> attributeMap;
	    
	    public SelfRegistration(String ipaddr, String username, String password, XMPPConnection conn, PacketListener listener) {
	    	setIpaddress(ipaddr);
	    	setUsername(username);
	    	setPassword(password);
	    	setXmppConnection(conn);
	    	setListener(listener);
	    	initAttributeMap(ipaddr, username, password);
		}
	    
	    
	    private void initAttributeMap(String ipaddr, String username, String password) {
	    	attributeMap = new HashMap<String, String>();
	    	attributeMap.put("username", username);
	    	attributeMap.put("password", password);
	    	attributeMap.put("email", "");
	    	attributeMap.put("fullname", username);
	    }
	    
		@Override
		public void run() {
			XMPPConnection con = getXmppConnection();
			try {
	            con.connect();
	            Registration packet = new Registration();
	            packet.setAttributes(attributeMap);
	            packet.setType(Type.SET);
	            
	            userJID = this.getUsername() +"@" + con.getServiceName()+"/" + this.getUsername();
	            con.addPacketListener(getListener(), null);
	            
	            packet.setFrom(userJID);
	            con.sendPacket(packet);
	        } catch (XMPPException e) {
	        	e.printStackTrace();
	        	con.disconnect();
	        	setResetThread(true);
	        	return;
	        }
			
			while(!isResetThread()) {
				try {
					Thread.sleep(1000);
				} catch (InterruptedException e) {
					Thread.currentThread().interrupt();
				}
			}
			
			if(con.isConnected())
				con.disconnect();
			
		}

		
		public void setIpaddress(String ipaddress) {
			this.ipaddress = ipaddress;
		}
		
		public String getIpaddress() {
			return this.ipaddress;
		}

		public String getUsername() {
			return username;
		}

		public void setUsername(String username) {
			this.username = username;
		}


		public void setPassword(String password) {
			this.password = password;
		}
		
		public String getPassword() {
			return this.password;
		}


		private XMPPConnection getXmppConnection() {
			return xmppConnection;
		}


		private void setXmppConnection(XMPPConnection xmppConnection) {
			this.xmppConnection = xmppConnection;
		}


		private PacketListener getListener() {
			return listener;
		}


		private void setListener(PacketListener listener) {
			this.listener = listener;
		}


		public boolean isResetThread() {
			return resetThread;
		}


		public void setResetThread(boolean resetThread) {
			this.resetThread = resetThread;
		}
		
	}

	
}
