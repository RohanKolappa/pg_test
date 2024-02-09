package com.ipvs.ext.xmppclient;

import java.io.StringReader;
import java.util.Properties;

import org.xmlpull.mxp1.MXParser;
import org.xmlpull.v1.XmlPullParser;

import com.ipvs.ext.provider.IpvsXmppParser;

public abstract class IpvsXmppXMLClient implements IpvsXmppClientListener{
	
	IpvsXmppClient client = null;
	int logLevel = IpvsXmppLoggerI.ERROR;

	protected abstract void handleXML(String s);
	
	private void setClient(IpvsXmppClient xmppClient) {
		this.client = xmppClient;
	}
	
	private void resetClient() {
		this.client = null;
	}
			
	public void printError(String s) {
		IpvsXmppLog.log("StreamClient:printError " + s, IpvsXmppLoggerI.ERROR, this.logLevel);
		this.handleXML("<client status='error'><ipvserror code=''><description>" + s + "</description></ipvserror></client>");
	}

	public void handlePacketXML(String packetXML) {
		IpvsXmppLog.log(packetXML, IpvsXmppLoggerI.DEBUG, this.logLevel);
		this.handleXML(packetXML);
	}

	public void handleDisconnected(String reason) {
		this.printError(reason);
		this.resetClient();
	}	
	
	private void handleLoginRequest(String loginRequest) throws Exception {
		IpvsXmppLog.log("StreamClient:handleLoginRequest ...", IpvsXmppLoggerI.DEBUG, this.logLevel);
		IpvsXmppClient xmppClient = new IpvsXmppClient(this, this.logLevel);
		XmlPullParser parser = new MXParser();
		parser.setInput(new StringReader(loginRequest));
		Properties p = IpvsXmppParser.parseProperties(parser);
		xmppClient.setAppServerJID(p.getProperty("managementserver", "managementserver.localhost"));
		xmppClient.login(
				p.getProperty("ip", "127.0.0.1"),
				p.getProperty("port", "5222"),
				p.getProperty("user", "superuser-default"),
				p.getProperty("password", "superuser-default"),
				p.getProperty("resource", "pc"));
		this.setClient(xmppClient);
	}

	private void handleLog(String logRequest) throws Exception {
		IpvsXmppLog.log("StreamClient:handleLog ...", IpvsXmppLoggerI.DEBUG, this.logLevel);
		XmlPullParser parser = new MXParser();
		parser.setInput(new StringReader(logRequest));
		Properties p = IpvsXmppParser.parseProperties(parser);
		String level = p.getProperty("level");
		this.setLogLevel(Integer.parseInt(level));
	}
	
	protected void setLogLevel(int logLevel) {
		this.logLevel = logLevel;
	}
	
	protected void setLogger(IpvsXmppLoggerI logger) {
		IpvsXmppLog.setLogger(logger);
	}

	private void handleHeartbeat(String heartbeat) throws Exception {
		IpvsXmppLog.log("StreamClient:handleHeartbest", IpvsXmppLoggerI.DEBUG, this.logLevel);
	}

	private void handleLogoutRequest() throws Exception {
		IpvsXmppLog.log("StreamClient:handleLogoutRequest", IpvsXmppLoggerI.DEBUG, this.logLevel);
		this.client.logout();
		this.resetClient();
	}
	
	public void logout() {
		if(this.client == null)
			return;
		try {
			this.handleLogoutRequest();
		}
		catch(Exception e) {
			this.printError("StreamClient:logout Failed :" + e.toString() + IpvsXmppStreamClient.getHelpString());		
		}
	}

	public void handleRequest(String request) throws Exception {
		IpvsXmppLog.log("StreamClient:handleRequest request=" + request, IpvsXmppLoggerI.DEBUG, this.logLevel);
		
		if(request.startsWith("<help")) {
			this.handleXML(IpvsXmppXMLClient.getHelpString());
		}
		else if(request.startsWith("<heartbeat")) {
			this.handleHeartbeat(request);
		}
		else if(request.startsWith("<log ")) {
			this.handleLog(request);
		}
		else if(request.startsWith("<login")) {
			if(this.client != null) {
				this.printError("Already Logged in");
				return;
			}
			this.handleLoginRequest(request);
		}
		else  {
			if(this.client == null) {
				this.printError("Not Logged in");
				return;
			}
			else if(request.startsWith("<logout")) {
				this.handleLogoutRequest();
			}
			else if(request.startsWith("<message ")) {
				this.client.sendMessage(request);
			}
			else if(request.startsWith("<iq")) {
				this.client.sendRequest(request);
			}
			else {
				this.printError("Invalid request");
				return;				
			}
		}
		this.handleXML("<client status='ok'/>");
	}
		
	public static String getHelpString() {
		return "<help>\n" + "<!--\n" + 
			"login   : <login ip='192.168.1.1' user='myname' password='mypassword' resource='mypc'/>\n" +
			"logout  : <logout/>\n" +
			"request : <iq id='123'><query><GetSession/></query></iq>\n" +
			"message : <message to='joe@localhost/pc'><body><anymessage/></body></message>\n" +
			"heartbeat: <heartbeat/>\n" +
			"logLevel: <log level='1'/>\n" +
			"help    : <help/>\n" + "-->\n" +
			   "</help>";
	}
		
}
