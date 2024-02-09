package com.barco.device.http;

import java.io.IOException;
import java.util.Date;

import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;

import org.eclipse.jetty.continuation.Continuation;
import org.eclipse.jetty.continuation.ContinuationSupport;
import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.servlet.ServletContextHandler;

import com.barco.device.common.Message;
import com.barco.device.common.MessageBusI;
import com.barco.device.common.MessageBusListenerI;

public class HTTPMessageBus extends HttpServlet implements MessageBusI {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	MessageBusListenerI listener = null;
	Server server = null;
	int state = MessageBusI.DISCONNECTED;


	@Override
	public int getState() {
		return state;
	}

	@Override
	public void sendMessage(Message message) throws Exception {
		// TODO Auto-generated method stub
		
	}

	@Override
	public void setListener(MessageBusListenerI listener) {
		this.listener = listener;
	}
	
	public void startServer(int port, String urlPath) throws Exception {
		if(state == MessageBusI.CONNECTED)
			return;
		server = new Server(port);
        ServletContextHandler servletContextHandler = new ServletContextHandler(server, urlPath, true, false);
        servletContextHandler.addServlet(HTTPMessageBus.class, "/");
		server.start();
		state = MessageBusI.CONNECTED;
	}
	
	public void stopServer() throws Exception {
		if(state == MessageBusI.DISCONNECTED)
			return;
		server.stop();
		state = MessageBusI.DISCONNECTED;
	}

	public void service(HttpServletRequest request,HttpServletResponse response) throws IOException, ServletException {
		
		response.setContentType("text/html;charset=utf-8");
		response.setStatus(HttpServletResponse.SC_OK);
		
		Continuation cc = ContinuationSupport.getContinuation(request);
		cc.setTimeout(2000);
		cc.suspend();
		
		if(cc.isInitial())
			this.beginRequest(request, response);
		else {
			if(!this.continueRequest(request, response))
				cc.complete();
		}
	}

	public void beginRequest(HttpServletRequest request,HttpServletResponse response) throws IOException, ServletException {
		System.out.println("HTTPMessageBus:start");
		response.getWriter().println("HTTPMessageBus:start " + new Date());
		response.getWriter().flush();
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
		bus.startServer(18080, "/context");
		bus.server.join();
	}

}
