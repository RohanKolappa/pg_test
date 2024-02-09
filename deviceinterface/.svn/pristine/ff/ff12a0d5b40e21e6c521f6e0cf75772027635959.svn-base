package com.barco.device.http;

import java.io.IOException;
import java.util.Date;
import java.util.Hashtable;
import java.util.Map;
import java.util.UUID;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.DocumentHelper;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.continuation.ContinuationSupport;
import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.servlet.ServletHolder;
import org.eclipse.jetty.util.StringUtil;

import com.barco.device.common.Message;
import com.barco.device.common.MessageBusI;
import com.barco.device.common.MessageBusListenerI;
import com.barco.device.common.ServiceException;
import com.barco.utils.XMLUtils;



public class HTTPMessageBus extends HttpServlet implements MessageBusI {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	MessageBusListenerI listener = null;
	Server server = null;
	int state = MessageBusI.DISCONNECTED;
	Logger logger = null;
	
	Hashtable<String,Continuation> httpRequestCache = null;
	String urlPath = null;
	
	enum HTTPRequestToDIRequestMapping {
		GET {public String toString() {	return "Get";}},
		PUT {public String toString() {return "Add";}},
		POST{public String toString() {return "Set";}},
		DELETE{public String toString() {return "Delete";}}
	};
	private enum ContinuationStates {Pending,Error,Done};
	
	enum SessionRequestToServiceNameMapping {
		File {public String toString() { return "MediaStore";}},
		Dir {public String toString() {	return "MediaStore";}},
		Device {public String toString() {	return "DeviceAdmin";}},
		DeviceConfig {public String toString() { return "DeviceAdmin";}},
		DeviceStatus {public String toString() { return "DeviceAdmin";}},
		Media {public String toString() {	return "Media";}},
		Layout {public String toString() {	return "Layout";}}
	};

	@Override
	public int getState() {
		return state;
	}

	@Override
	public void sendMessage(Message message) throws Exception {
		String clientData  = message.getClientData();
		Continuation cc  = this.httpRequestCache.get(clientData);
		Document responseDoc  = message.getMessageDoc();
		String requestName = message.getRootName().replace("Request", "");
		Element responseDataEl = XMLUtils.getElementXpath(responseDoc, "//"+requestName+"ResponseData");		
		if(responseDataEl!=null && cc != null) {
		    HttpServletResponse response = (HttpServletResponse)cc.getServletResponse();
		    Element responseEl = null;
		    //ContinuationStates ccState = ContinuationStates.Done;
			String state = XMLUtils.getAttributeValueWithDefault(XMLUtils.getElementXpath(responseDoc, "//Header"), "state","");
			response.setContentType("text/xml");
			if(!state.equals("0")) {
				if(cc.getAttribute("RequestType").equals("PUT")) {
					response.setStatus(HttpServletResponse.SC_CREATED);
				} else {
					response.setStatus(HttpServletResponse.SC_OK);
				}				
				if(responseDataEl.elements()!=null && responseDataEl.elements().size()>0) {
					responseEl = (Element)responseDataEl.content().get(0);
				} else {
					responseEl = responseDataEl;
				}
				
				setContiuationState(cc, ContinuationStates.Done);
			} else {
				response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
				responseEl = (Element)DocumentHelper.createXPath(".//BarcoError").selectSingleNode(responseDataEl);
				setContiuationState(cc, ContinuationStates.Error);
				//ccState = ContinuationStates.Error;
			}
		
			response.getWriter().println(responseEl.asXML());
			//setContiuationState(cc, ccState);
		}	
	}

	@Override
	public void init(MessageBusListenerI listener, Logger log) {
		this.listener = listener;
		this.logger = log;
	}
	
	public void startServer(int port, String urlPath) throws Exception {
		if(state == MessageBusI.CONNECTED)
			return;
		server = new Server(port);
		this.urlPath = urlPath;
        ServletContextHandler servletContextHandler = new ServletContextHandler(server, urlPath, true, false);
        servletContextHandler.addServlet(new ServletHolder(this), "/");
        server.start();
		state = MessageBusI.CONNECTED;
		httpRequestCache = new Hashtable<String, Continuation>();
	}
	
	public void stopServer() throws Exception {
		if(state == MessageBusI.DISCONNECTED)
			return;
		server.stop();
		state = MessageBusI.DISCONNECTED;
		httpRequestCache = null;
	}

	public void service(HttpServletRequest request,HttpServletResponse response) throws IOException, ServletException {
		Continuation cc = ContinuationSupport.getContinuation(request);
		cc.setTimeout(2000);
		cc.suspend();
		
		if(cc.isInitial()) {
			setContiuationState(cc, ContinuationStates.Pending);
			setContinuationRequestType(cc, request.getMethod());
			String clientData = UUID.randomUUID().toString();
			addRequestToCache(cc, clientData);
			cc.setAttribute("State", ""+HttpServletResponse.SC_OK);
			String agentMessageType = null;
			try {
				agentMessageType = this.beginRequest(request, response,clientData);
				if(agentMessageType.equals(Message.EVENT)) {
					setContiuationState(cc, ContinuationStates.Done);
				}
			} catch (Exception e) {				
				e.printStackTrace();
				setContiuationState(cc, ContinuationStates.Error);
				response.setStatus(HttpServletResponse.SC_INTERNAL_SERVER_ERROR);
				response.getWriter().println();		
			}
			
		} 
		
		String continuationState = (String) cc.getAttribute("State");
		if(continuationState.equals(ContinuationStates.Done.toString()) || continuationState.equals(ContinuationStates.Error.toString())) {
			removeRequestFromCache(cc);
			cc.complete();
		}		
	}
	
	private synchronized void addRequestToCache(Continuation cc,String clientData){
		if(clientData!=null) {
			this.httpRequestCache.put(clientData,cc);
			cc.setAttribute("ClientData", clientData);
		}
		logger.debug("Request Added to Cache:="+clientData);
	}
	
	private void setContiuationState(Continuation cc,ContinuationStates state) {
		cc.setAttribute("State",state.toString());
	/*	if(state.equals(ContinuationStates.Done) || state.equals(ContinuationStates.Error)){
			logger.debug("Done with async request: resuming");
			cc.resume();
		}*/
		
	}
	
	private void setContinuationRequestType(Continuation cc, String requestType) {
		cc.setAttribute("RequestType", requestType);
		
	}
	
	private synchronized void removeRequestFromCache(Continuation cc){
		String clientData = (String)cc.getAttribute("ClientData");
		this.httpRequestCache.remove(clientData);
		logger.debug("Request Removed from Cache:="+clientData);	
		
	}
	

	public String beginRequest(HttpServletRequest request,HttpServletResponse response,String clientData) throws IOException, ServletException,Exception {
		String requestType = request.getMethod();
		String requestURL = request.getRequestURI();
		String requestRootName = StringUtil.replace(requestURL, urlPath, "");
		requestRootName = StringUtil.replace(requestRootName, "/", "");
		String diRequestType =HTTPRequestToDIRequestMapping.valueOf(requestType).toString();
		String diRequestName = diRequestType+requestRootName;
		String serviceName = SessionRequestToServiceNameMapping.valueOf(requestRootName).toString();
		String clientcallback = this.getClass().getName();
		Element requestElement = null;
		//handle Media and Layout as RPC requests
		if(!(serviceName.equals(SessionRequestToServiceNameMapping.Media.toString()) || serviceName.equals(SessionRequestToServiceNameMapping.Layout.toString()))) {	
			Map< String, String[]> queryParameters = request.getParameterMap();
			requestElement=DocumentHelper.createElement(diRequestName+"RequestData");
			for(String attributeName:queryParameters.keySet()) {
				StringBuilder buildString = new StringBuilder();
				String delim = "";
				for(String value:queryParameters.get(attributeName)){
					buildString.append(delim+ value); 
					delim=",";
				}
				requestElement.addAttribute(attributeName,buildString.toString());
			}
		}
		
		Document requestDoc= null;
		try {			
			 StringBuilder sb = new StringBuilder();
			 String s = "";
			 while ((s = request.getReader().readLine()) != null) {
				 sb.append(s);
			}
			requestDoc = DocumentHelper.parseText(sb.toString());
		} catch (Exception e) {	
			e.printStackTrace();
		}
		
		if(requestDoc != null) {
			requestElement = requestDoc.getRootElement();
		}
		
		String agentMessageType = Message.REQUEST;
		//handle Media and Layout as RPC requests
		if(serviceName.equals(SessionRequestToServiceNameMapping.Media.toString()) || serviceName.equals(SessionRequestToServiceNameMapping.Layout.toString())) {
			diRequestName = requestElement.getName();
		}
		
		
		diRequestName = requestElement.getName();
		if(diRequestName.endsWith("Event")){
			agentMessageType = Message.EVENT;				
		} else {
			diRequestName = diRequestName.replace("RequestData", "");
		}
	
		String httpUserJID = diRequestName+"@http/"+request.getRemoteHost();
		Message requestMessage = null;
		if(agentMessageType.equals(Message.REQUEST)) {
			//create request Doc
			String protocol =request.getHeader("X-Forwarded-Proto");
			if(protocol == null || protocol.isEmpty())
				protocol = "http";
			
			String xml = "<Request>" + "<Header serviceName='" + serviceName + "' requestName='" + diRequestName + "Request'  userJID='" + httpUserJID +
					"' requestNID=''><ClientData>" + clientData + "</ClientData>" + "<ClientCallback>" + clientcallback + "</ClientCallback>"  + "<Protocol>" + protocol +"</Protocol>" + "</Header>" +
					"<Data>" + requestElement.asXML()  + "</Data>" + "</Request>";		
			
			requestMessage = Message.createRequest(DocumentHelper.parseText(xml), "", httpUserJID);
			logger.debug("Handle Request requestName:="+diRequestName+" host:="+request.getRemoteHost()); 

		} if(agentMessageType.equals(Message.EVENT)) {		
			logger.debug("Handle Event eventName:="+diRequestName+" host:="+request.getRemoteHost()); 
			requestMessage = Message.createEvent(requestElement, "", httpUserJID, serviceName, diRequestName, String.valueOf(Level.INFO), Level.INFO);
		}
		requestMessage.setMessageBusClassName(this.getClass().getName());
		listener.handleMessage(requestMessage);
		return agentMessageType;
	}

	public boolean continueRequest(HttpServletRequest request,HttpServletResponse response) throws IOException, ServletException {
		// System.out.println("HTTPMessageBus:check");
		try {			
			response.getWriter().println("HTTPMessageBus:check " + new Date());
			response.getWriter().flush();
			return true;
		}
		catch(Exception e) {
			System.out.println("HTTPMessageBus:error=" + e.toString());
			return false;
		}
	}
    
	public static void main(String[] args) throws Exception {
		System.out.println("HTTPMessageBus:main");
		HTTPMessageBus bus = new HTTPMessageBus();
		bus.startServer(1080, "/deviceinterface");
		bus.server.join();
	}

	@Override
	public void login(String username, String password, String resource,
			String ipaddress, String appserverAgentJID)
			throws ServiceException, Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void logout() throws ServiceException, Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void sendSCDiscoveryMessage() throws ServiceException, Exception {
		// TODO Auto-generated method stub
		
	}

}
