package com.barco.device.ext.xmppclient;

import org.apache.log4j.Logger;

public abstract class BarcoXmppXMLClient implements BarcoXmppClientListener{
	
	protected BarcoXmppClient client = null;
	public Logger logger = null;
	
	protected abstract void handleXML(String s);
	
	private void setClient(BarcoXmppClient xmppClient) {
		this.client = xmppClient;
	}
	
	protected void resetClient() {
		this.client = null;
	}
			
	public void printError(String s) {
		this.handleXML("<client status='error'><ipvserror code=''><description>" + s + "</description></ipvserror></client>");
	}

	public void handlePacketXML(String packetXML) {
		this.handleXML(packetXML);
	}

	public void handleDisconnected(String reason) {
		this.printError(reason);
		this.resetClient();
	}	
	
	protected void handleLoginRequest(String username,String password,String resource,String ipaddress,String appserverAgentJID) throws Exception {
		this.logger.debug("StreamClient:handleLoginRequest ...");
		BarcoXmppClient xmppClient = new BarcoXmppClient(this, this.logger);
		xmppClient.setAppServerJID(appserverAgentJID);
		xmppClient.login(ipaddress,"5222", username,password,resource);
		this.setClient(xmppClient);
	}


	protected void handleHeartbeat(String heartbeat) throws Exception {
		this.logger.debug("StreamClient:handleHeartbeat");
	}

	protected void handleLogoutRequest() throws Exception {
		this.logger.debug("StreamClient:handleLogoutRequest");
		if(client!=null) {
			this.client.logout();
			this.resetClient();
		}
		
	}
	

	public void handleRequest(String request) throws Exception {
		this.logger.debug("StreamClient:handleRequest");
		
		if(request.startsWith("<help")) {
			this.handleXML(BarcoXmppXMLClient.getHelpString());
		}		
		else  {
			if(this.client == null) {
				this.printError("Not Logged in");
				return;
			} else if(request.startsWith("<message ")) {
				this.client.sendMessage(request);
			} else if(request.startsWith("<iq ")) {
				this.client.sendIQ(request);
			}
			else {
				this.printError("Invalid request");
				return;				
			}
		}
		//this.handleXML("<client status='ok'/>");
	}
		
	public static String getHelpString() {
		return "<help>\n" + "<!--\n" + 
			"login   : <login ip='192.168.1.1' user='myname' password='mypassword' resource='mypc'/>\n" +
			"logout  : <logout/>\n" +
			//"request : <iq id='123'><query><GetSession/></query></iq>\n" +
			"message : <message to='joe@localhost/pc'><body><anymessage/></body></message>\n" +
			"heartbeat: <heartbeat/>\n" +
			"help    : <help/>\n" + "-->\n" +
			   "</help>";
	}
	
	public String getMyJID() {
		if(client==null)
		{
			return "";
		}
		return this.client.userJID;
	}
	
	public String getAppServerAgentJID(){
		if(client ==null) return"";
		return this.client.appServerJID;
	}

}
