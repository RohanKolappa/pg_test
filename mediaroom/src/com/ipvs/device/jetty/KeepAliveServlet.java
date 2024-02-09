package com.ipvs.device.jetty;

import javax.servlet.http.HttpServletRequest;

import org.eclipse.jetty.server.Server;
import org.eclipse.jetty.servlet.ServletContextHandler;
import org.eclipse.jetty.websocket.WebSocket;
import org.eclipse.jetty.websocket.WebSocket.OnTextMessage;
import org.eclipse.jetty.websocket.WebSocketServlet;

class KeepAliveWebSocket implements OnTextMessage {
	@Override
	public void onClose(int arg0, String arg1) {
		System.out.println("KeepAliveServlet:onClose");
	}

	@Override
	public void onOpen(Connection arg0) {
		System.out.println("KeepAliveServlet:onOpen");
	}

	@Override
	public void onMessage(String arg0) {
		System.out.println("KeepAliveServlet:onMessage");
	}	
}

public class KeepAliveServlet extends WebSocketServlet {

	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	@Override
	public WebSocket doWebSocketConnect(HttpServletRequest arg0, String arg1) {
		System.out.println("KeepAliveServlet:doWebSocketConnect");
		return new KeepAliveWebSocket();
	}

	public void doGet(HttpServletRequest arg0, String arg1) {
		System.out.println("KeepAliveServlet:doGet");
	}

	public static void main(String[] args) throws Exception {
		System.out.println("Hello");
		Server server = new Server(8080);
        ServletContextHandler servletContextHandler = new ServletContextHandler(server, "/mediaroom", true, false);
        servletContextHandler.addServlet(KeepAliveServlet.class, "/");
		server.start();
		server.join();
	}

}
