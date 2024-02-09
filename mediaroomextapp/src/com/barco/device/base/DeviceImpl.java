package com.barco.device.base;



import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.LoggerI;
import com.barco.device.common.Message;
import com.barco.device.common.ServiceException;
import com.barco.device.http.HTTPMessageBus;
import com.barco.device.xmpp.XMPPMessageBus;
import com.barco.utils.mr.MRXMLUtils;
import com.ipvs.ext.xmppclient.IpvsXmppLoggerI;

class ResponseData {
	String clientData;
	Document context;
	ResponseHandlerI responseHandler;
	static ResponseData parseClientData(String clientData) throws Exception {
		// TBD Generate the values for the context and reponsseHandler and restore clientData
		ResponseData data = new ResponseData();
		String className = clientData;
		Class<?> c = Class.forName(className);
		data.responseHandler = (ResponseHandlerI)c.newInstance();
		return data;
	}
	static String generateClientData(String clientData, Document context, ResponseHandlerI responseHandler) {
		String handlerName = responseHandler.getClass().getName();
		clientData = handlerName;
		// TBD Serialize the context and responseHandler, clientData and update clientData
		return clientData;
	}
}

public class DeviceImpl implements DeviceI, IpvsXmppLoggerI {
	
	XMPPMessageBus xmpp;
	HTTPMessageBus http;
	LayoutAgentI layoutAgent;
	MediaAgentI mediaAgent;
	DeviceAdminAgentI deviceAdminAgent;
	MediaStoreAgentI mediaStoreAgent;
	int httpPort;
	String handlerPackage;
	int logLevel = LoggerI.ERROR;
	public static final int DefaultHTTPPort = 8080;
	public static final String DefaultHandlerPackage = "com.barco.device.handler";
	
	public DeviceImpl(	
			DeviceAdminAgentI deviceAdminAgent,
			LayoutAgentI layoutAgent,
			MediaAgentI mediaAgent,
			MediaStoreAgentI mediaStoreAgent,
			int httpPort,
			String handlerPackage) {
		this.xmpp = new XMPPMessageBus();
		this.http = new HTTPMessageBus();
		this.handlerPackage = handlerPackage;
		this.deviceAdminAgent = deviceAdminAgent;
		this.layoutAgent = layoutAgent;
		this.mediaAgent = mediaAgent;
		this.mediaStoreAgent = mediaStoreAgent;
		this.httpPort = httpPort;
		this.handlerPackage = handlerPackage;
	}

	public void handleSystemCommand(String s) throws Exception {
		if(s.startsWith("<start"))
			this.http.startServer(httpPort, "/device");
		else if(s.startsWith("<stop"))
			this.http.stopServer();
		else
			xmpp.handleRequest(s);
	}
	
	@Override
	public void handleException(ServiceException exception) {
		this.addLog(exception.toString(), LoggerI.ERROR, this.logLevel);
		
	}

	public void handleMessageInternal(Message message) {
		this.addLog(message.getRootName() + " from=" + message.getFrom() + " begin", LoggerI.INFO, message.getLogLevel());
		long startTime = System.currentTimeMillis();
		try {
			String handlerName = handlerPackage + "." + message.getService() + "." + message.getRootName() + "Handler";
			Class<?> c = Class.forName(handlerName);
			MessageHandlerI handler = (MessageHandlerI)c.newInstance();
			handler.handleMessage(message, this);
		}
		catch(ServiceException se) {
			try {message.setResult(MRXMLUtils.stringToDocument(se.toString()));}
			catch(Exception e) {this.addLog(e.toString(), LoggerI.ERROR, LoggerI.ERROR);}
		}
		catch(Exception e) {
			this.addLog(e.toString(), LoggerI.ERROR, LoggerI.ERROR);
		}
        long duration = System.currentTimeMillis() - startTime;
		this.addLog(message.getRootName() + " from=" + message.getFrom() +
				" duration=" + duration, LoggerI.INFO, message.getLogLevel());		
	}


	public void handleResponseInternal(Message message, Document context, ResponseHandlerI responseHandler) {
		this.addLog(message.getRootName() + " from=" + message.getFrom() + " begin", LoggerI.INFO, message.getLogLevel());
		long startTime = System.currentTimeMillis();
		try {
			Element ipvserror = MRXMLUtils.getElementXpath(message.getMessageDoc(), "//ipvserror");
				if(ipvserror != null) {
					responseHandler.handleError(message, context, this);
					this.addLog(message.getRootName() + " from=" + message.getFrom() +
							" error=" + ipvserror.asXML(), LoggerI.ERROR, message.getLogLevel());
				}
				else					
					responseHandler.handleResponse(message, context, this);
		}
		catch(ServiceException se) {
			try {message.setResult(MRXMLUtils.stringToDocument(se.toString()));}
			catch(Exception e) {this.addLog(e.toString(), LoggerI.ERROR, LoggerI.ERROR);}
		}
		catch(Exception e) {
			this.addLog(e.toString(), LoggerI.ERROR, LoggerI.ERROR);
		}
        long duration = System.currentTimeMillis() - startTime;
		this.addLog(message.getRootName() + " " + Message.RESPONSE + " from=" + message.getFrom() +
				" duration=" + duration, LoggerI.INFO, message.getLogLevel());
	}

	@Override
	public void handleMessage(Message message) throws Exception {

		if(Message.RESPONSE.equals(message.getType())) {
			ResponseData responseData = ResponseData.parseClientData(message.getClientData());
			if(responseData != null) {
				message.updateClientData(responseData.clientData);
				this.handleResponseInternal(message, responseData.context, responseData.responseHandler);
			}
			else {
				this.addLog(message.getRootName()+" responseData is null ignoring response", LoggerI.ERROR, message.getLogLevel());
			}
			return;
		}
		
		this.handleMessageInternal(message);
		
		if(Message.REQUEST.equals(message.getType())) {
			Message response = Message.createResponse(message.getFrom(), message);
			this.sendMessage(response);
		}
	}

	@Override
	public void log(String s, int level, int ref) {
		if(level <= ref)
			System.out.println(s);
	}

	@Override
	public DeviceAdminAgentI getDeviceAdminAgentImpl() {
		return deviceAdminAgent;
	}

	@Override
	public LayoutAgentI getLayoutAgentImpl() {
		return layoutAgent;
	}

	@Override
	public MediaAgentI getMediaAgentImpl() {
		return mediaAgent;
	}

	@Override
	public MediaStoreAgentI getMediaStoreAgentImpl() {
		return mediaStoreAgent;
	}

	@Override
	public void sendMessage(Message m) throws Exception {
		this.xmpp.sendMessage(m);
	}

	@Override
	public void login(String ip, String user, String password, String resource,
			String managementserver) throws Exception {
		this.xmpp.setListener(this);
		this.xmpp.handleRequest("<login ip='" + ip + "' user='" + user + "' password='" + password +
				"' resource='" + resource + "' managementserver='" + managementserver + "'/>");
		// TODO Auto-generated method stub
		
	}

	@Override
	public void handleLog(String s, int level) {
		this.addLog(s, level, this.logLevel);		
	}

	@Override
	public void logout() {
		try {
			this.xmpp.handleRequest("<logout/>");
		}
		catch(Exception e){
			this.addLog("BaseDeviceImpl:logout error=" + e.toString(), LoggerI.DEBUG, this.logLevel);
		}
	}
	
	@Override
	public void sendRequest(Message message, Document context, ResponseHandlerI responseHandler)
			throws Exception {
		// TBD this should handle internally of the to is to self
		boolean internal = true;
		if(internal) {
			this.handleMessageInternal(message);
			Message response = Message.createResponse(message.getFrom(), message);
			this.handleResponseInternal(response, context, responseHandler);
		}
		else {
			if(Message.REQUEST.equals(message.getType())) {
				String clientData = ResponseData.generateClientData(
						message.getClientData(), context, responseHandler);
				message.updateClientData(clientData);
			}
			this.xmpp.sendMessage(message);
		}
	}

	@Override
	public void addLog(String s, int level, int ref) {
		this.log(s, level, ref);
	}

	@Override
	public void setLogLevel(int level) {
		this.logLevel = level;
	}
	
}
