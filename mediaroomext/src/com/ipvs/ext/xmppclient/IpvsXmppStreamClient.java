package com.ipvs.ext.xmppclient;

import java.io.BufferedReader;

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.io.PrintWriter;
import java.io.StringReader;
import java.net.SocketException;
import java.util.Vector;

import org.xmlpull.mxp1.MXParser;
import org.xmlpull.v1.XmlPullParser;

import com.ipvs.ext.provider.IpvsXmppParser;

class IpvsXmppStreamRequestThread extends Thread {
	boolean done;
	BufferedReader reader;
	XmlPullParser parser;
	int logLevel;
	Vector<String> queue;
	IpvsXmppParser providerParser = null;
	PipedOutputStream pipeSource = null;
	PipedInputStream pipeSink = null;
	PrintWriter pipeWriter = null;
	char [] buffer = new char[1024];
	
	IpvsXmppStreamRequestThread(BufferedReader reader, int logLevel) {
		this.reader = reader;
		this.done = false;
		this.queue = new Vector<String>();
		this.logLevel = logLevel;
		this.providerParser = new IpvsXmppParser();
		this.parser = new MXParser();
	}
	
	void init() throws Exception {
		pipeSource= new PipedOutputStream();
		pipeSink = new PipedInputStream(pipeSource);
		pipeWriter = new PrintWriter(pipeSource);

		parser.setInput(new BufferedReader(new InputStreamReader(pipeSink)));
		pipeWriter.write("<" + IpvsXmppStreamRequestThread.class.getName() + ">\n");
		pipeWriter.flush();
		parser.next(); // Consume the start token
	}
	
	synchronized void add(String s) {
		// IpvsXmppLog.log("IpvsXmppStreamRequestThread:add ...", IpvsXmppLoggerI.DEBUG, this.logLevel);
		this.queue.add(s);
		// IpvsXmppLog.log("IpvsXmppStreamRequestThread:add done", IpvsXmppLoggerI.DEBUG, this.logLevel);
	}
	
	synchronized String next() throws Exception {
		if(this.queue.isEmpty())
			return null;
    	return this.queue.remove(0);
	}

	String getNextRequest() throws Exception {
		while(reader.ready()) {
			int bytesRead = reader.read(buffer);
			pipeWriter.write(buffer, 0, bytesRead);
			pipeWriter.flush();
		}
    	return this.next();
	}

	void setDone() {
		IpvsXmppLog.log("IpvsXmppStreamRequestThread:setDone", IpvsXmppLoggerI.DEBUG, this.logLevel);
		this.done = true;
		pipeWriter.write("</" + IpvsXmppStreamRequestThread.class.getName() + ">\n");
		pipeWriter.flush();
	}

	static IpvsXmppStreamRequestThread createInstance(BufferedReader reader, int logLevel) throws Exception {
		IpvsXmppLog.log("IpvsXmppStreamRequestThread:createInstance ...", IpvsXmppLoggerI.DEBUG, logLevel);
		IpvsXmppStreamRequestThread t = new IpvsXmppStreamRequestThread(reader, logLevel);
		t.start();
		IpvsXmppLog.log("IpvsXmppStreamRequestThread:createInstance ... started", IpvsXmppLoggerI.DEBUG, logLevel);
		return t;
	}

	public void run() {
		try {
			this.init();
		}
		catch(Exception e) {
			e.printStackTrace();
			this.done = true;
		}
		while(!this.done) {
			try {
				parser.next(); // Will wait here until the next start token is available
				String s = providerParser.parse(parser);
				if((s != null) && (!s.trim().isEmpty())) {
					IpvsXmppLog.log("IpvsXmppStreamRequestThread:run adding request=" + s, IpvsXmppLoggerI.DEBUG, this.logLevel);
					this.add(s);
					// IpvsXmppLog.log("IpvsXmppStreamRequestThread:run done=" + done, IpvsXmppLoggerI.DEBUG, this.logLevel);
				}
			}
			catch(Exception e) {
				e.printStackTrace();
			}
		}
		IpvsXmppLog.log("IpvsXmppStreamRequestThread:run exited", IpvsXmppLoggerI.DEBUG, this.logLevel);
	}
}

public class IpvsXmppStreamClient extends IpvsXmppXMLClient implements Runnable{
	InputStream input;
	BufferedReader reader;
	PrintWriter writer;
	int maxTimeout;
	int timeout;
	Vector<String> requestQueue;
	IpvsXmppStreamRequestThread requestThread;
	
	public IpvsXmppStreamClient(InputStream input, OutputStream output, int timeout, int logLevel) {
		this.input = input;
		this.reader = new BufferedReader(new InputStreamReader(input));
		this.writer = new PrintWriter(output);
		this.maxTimeout = timeout;
		this.timeout = 0;
		this.setLogLevel(logLevel);
		this.requestThread = null;
		try {
			this.requestThread = IpvsXmppStreamRequestThread.createInstance(reader, logLevel);
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	protected void handleXML(String s) {
		writer.println(s);
		writer.flush();		
		IpvsXmppLog.log("IpvsXmppStreamClient:handleXML=" + s, IpvsXmppLoggerI.DEBUG, this.logLevel);
	}
	
	
	void resetTimeout() {
		this.timeout = 0;
	}

	boolean hasTimedOut() {
		if(this.client == null)
			return false;
		if(maxTimeout == 0)
			return false;
		this.timeout++;
		return this.timeout > this.maxTimeout;
	}

	boolean hasShutdown() {
		if(this.client == null)
			return false;
		return !client.connection.isConnected();
	}
	
	public void run() {
		if(this.requestThread == null)
			return;
		while(true) {
			try {
				Thread.sleep(1000); // Used to implement the timeout
				if(this.hasShutdown()) {
					IpvsXmppLog.log("StreamClient:run shutdown", IpvsXmppLoggerI.ERROR, this.logLevel);
					break;
				}
				String request = this.requestThread.getNextRequest();
				if(request != null) {
					this.handleRequest(request);
					if(request.startsWith("<logout"))
						break;
					this.resetTimeout();
				}
				else {
					if(this.hasTimedOut()) {
						IpvsXmppLog.log("StreamClient:run timeout", IpvsXmppLoggerI.ERROR, this.logLevel);
						break;
					}
					// System.out.print(".");
				}
			}
			catch(SocketException se) {
				this.printError("StreamClient:run client connection error" + se.toString());
				break;
			}
			catch(Exception e) {
				e.printStackTrace();
				this.printError("StreamClient:run handleRequest Failed :" + e.toString());
			}
		}

		// Stop the requestThread
		this.requestThread.setDone();

		// Clean up the XMPP connection
		this.logout();
		
		// Clean up the client connection
		try {
			reader.close();
			writer.close();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		IpvsXmppLog.log("StreamClient:run closed client connection", IpvsXmppLoggerI.INFO, this.logLevel);
	}
			
	public static void main(String[] args) throws Exception {
    	IpvsXmppStreamClient streamClient = new IpvsXmppStreamClient(System.in, System.out, 0, IpvsXmppLoggerI.DEBUG);
		streamClient.run();
		System.out.println("Waiting 5 seconds before exiting main ...");
		Thread.sleep(5000);
	}
	
	/*
	public static String getNextRequest(XmlPullParser xpp) throws Exception {
	    xpp.next();
	    int eventType = xpp.getEventType();
	    String rootName = xpp.getName();
	    StringBuffer sb = new StringBuffer();
	    while (true) {
	    	String name = xpp.getName();
	    	if(eventType == XmlPullParser.END_DOCUMENT) {
	    		break;
	    	} else if(eventType == XmlPullParser.START_TAG) {
	    		System.out.println("Start tag "+name);
	    		sb.append("<" + name + ">");
	    		if(xpp.isEmptyElementTag()) {
		    		sb.append("</" + name + ">");
			    	if(xpp.getName().equals(rootName))
			    		break;	    			
	    		}
		    } else if(eventType == XmlPullParser.END_TAG) {
		    	System.out.println("End tag "+name);
	    		sb.append("</" + name + ">");
		    	if(xpp.getName().equals(rootName))
		    		break;
		    } else if(eventType == XmlPullParser.TEXT) {
		    	System.out.println("Text "+xpp.getText());
		    	sb.append(xpp.getText());
		    }
	    	xpp.next();
	    }
		return sb.toString();
	}

    public static void main (String args[]) throws Exception {
		XmlPullParser xpp = new MXParser();
		xpp.setInput(new BufferedReader(new InputStreamReader(System.in)));
	    xpp.next(); // consume the root node
	    while (true) {
	    	String request = getNextRequest(xpp);
	    	if((request == null) || (request.isEmpty()))
	    		break;
	    	System.out.println(request);
	    }
	}
	*/
    
}
