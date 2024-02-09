package com.ipvs.ext.proxy;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PipedInputStream;
import java.io.PipedOutputStream;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

import com.ipvs.ext.xmppclient.IpvsXmppLog;
import com.ipvs.ext.xmppclient.IpvsXmppLoggerI;
import com.ipvs.ext.xmppclient.IpvsXmppStreamClient;

public class ProxyServer {

	ServerSocket serverSocket;
	int port;
	int timeout;
	int logLevel;
	
	public ProxyServer(int port, int timeout, int logLevel) {
		this.port = port;
		this.timeout = timeout;
		this.logLevel = logLevel;
	}
	
	void run() {
		try {
		    serverSocket = new ServerSocket(port);
		} catch (IOException e) {
		    System.out.println("Could not listen on port: " + port);
		    System.exit(-1);
		}
	    System.out.println("Listening on port=" + port + " logLevel=" + logLevel + " timeout=" + timeout);
	    while(true) {
			try {
			    Socket clientSocket = serverSocket.accept();
				IpvsXmppStreamClient streamClient = new IpvsXmppStreamClient(clientSocket.getInputStream(), clientSocket.getOutputStream(), 
						timeout, logLevel);
				Thread t = new Thread(streamClient);
				t.start();
			} catch (IOException e) {
			    System.out.println("Accept failed: " + port);
			    System.exit(-1);
			} catch(Exception e) {
				e.printStackTrace();
			    System.out.println("Accept failed: " + port);
			    System.exit(-1);
			}
	    }
	}
	
	public static void main(String[] args) throws Exception {
		int port = 0;
		int timeout = 0;
		int logLevel = IpvsXmppLoggerI.DEBUG;
		String filename = null;
		int i = 0;
		String arg;
		try {
			while(i < args.length) {
				arg = args[i++];
				if("-p".equals(arg)) {
					port = Integer.parseInt(args[i++]);
				}
				if("-l".equals(arg)) {
					logLevel = Integer.parseInt(args[i++]);
				}
				if("-t".equals(arg)) {
					timeout = Integer.parseInt(args[i++]);
				}
				if("-f".equals(arg)) {
					filename = args[i++];
				}
			}
		}
		catch(Exception e)  {
			e.printStackTrace();
			port=0;
		}
		if(filename != null) {
			PipedOutputStream pipeSource= new PipedOutputStream();
			PipedInputStream pipeSink = new PipedInputStream(pipeSource);
			PrintWriter pipeWriter = new PrintWriter(pipeSource);

			IpvsXmppStreamClient streamClient = new IpvsXmppStreamClient(pipeSink, System.out, timeout, logLevel);
			Thread t = new Thread(streamClient);
			t.start();
			BufferedReader reader = new BufferedReader(new FileReader(new File(filename)));
			while(reader.ready()) {
				String s = reader.readLine();
				System.out.println("Read : s=" + s);
				pipeWriter.write(s);
				pipeWriter.flush();
			}
			while(t.isAlive()) {
				System.out.println("Waiting for streamClient to terminate ...");				
				Thread.sleep(1000);
			}
			System.exit(1);
		}

		if(port == 0) {
			System.out.println("Usage : -p <required port> -l <optional logLevel 1-5> -t <optional timeout in seconds> -f <filename>");
			System.exit(-1);
		}
		System.out.println("Started ProxyServer\n" + IpvsXmppStreamClient.getHelpString());
		ProxyServer socketClient = new ProxyServer(port, timeout, logLevel);
		socketClient.run();
	}
}
