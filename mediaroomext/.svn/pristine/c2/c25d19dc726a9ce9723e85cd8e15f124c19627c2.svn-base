package com.ipvs.ext.proxy;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

class ProxyServerThread extends Thread {
	
	int port;
	int timeout;
	int logLevel;

	public ProxyServerThread(int port, int timeout, int logLevel) {
		this.port = port;
		this.timeout = timeout;
		this.logLevel = logLevel;
	}
	public void run() {
		ProxyServer server = new ProxyServer(port, timeout, logLevel);
		server.run();
	}
}

public class ProxyClient implements  Runnable{

	ServerSocket serverSocket;
	int port;
	int timeout;
	int logLevel;
	String serverIP;
	String user;
	String password;
	String resource;
	String clientVersion;

	public ProxyClient(int port, int timeout, int logLevel, 
			String serverIP, String user, String password, String resource, String clientVersion) {
		this.port = port;
		this.timeout = timeout;
		this.logLevel = logLevel;
		this.serverIP = serverIP;
		this.user = user;
		this.password = password;
		this.resource = resource;
		this.clientVersion = clientVersion;
	}

	public void run() {
		try {
			BufferedReader cmdline = new BufferedReader(new InputStreamReader(System.in));
			System.out.println("ProxyClient:run connecting to server hit enter to proceed");
			String data = cmdline.readLine();
			Socket socket = new Socket("localhost", this.port);
			BufferedReader reader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
			PrintWriter writer = new PrintWriter(socket.getOutputStream());
			// Login
			writer.write("<login ip='" + serverIP + 
					"' user='" + user +
					"' password='" + password +
					"' resource='" + resource +
					"'/>");
			writer.flush();
			while(reader.ready())
				System.out.println(reader.readLine());
			System.out.println("ProxyClient:run logged in hit enter to proceed");
			data = cmdline.readLine();
			// ServiceLogin
			writer.write("<iq><query><ServiceLogin clientVersion='" + clientVersion + "'/></query></iq>");
			writer.flush();
			while(reader.ready())
				System.out.println(reader.readLine());

			while(true) {
				System.out.println(">");
				data = cmdline.readLine();
				if(data.startsWith("<")) {
					System.out.println("ProxyClient:run sending data=" + data);
					writer.write(data);
					writer.flush();
					Thread.sleep(5000);
					System.out.println("ProxyClient:run sending data=" + data);
					writer.write(data);
					writer.flush();
					while(reader.ready())
						System.out.println(reader.readLine());
				}
				if(data.startsWith("q"))
					break;
			}
			socket.close();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
	}
	
	public static void main(String[] args) throws Exception {
		int port = 0;
		int timeout = 0;
		int logLevel = 5;
		int i = 0;
		String arg;
		String ip = null;
		String user = null;
		String password = null;
		String resource = null;
		String clientVersion = null;
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
				if("-i".equals(arg)) {
					ip = arg;
				}
				if("-u".equals(arg)) {
					user = arg;
				}
				if("-w".equals(arg)) {
					password = arg;
				}
				if("-r".equals(arg)) {
					resource = arg;
				}
				if("-v".equals(arg)) {
					clientVersion = arg;
				}
			}
		}
		catch(Exception e)  {
			e.printStackTrace();
			port=0;
		}
		if(port == 0) {
			System.out.println("Usage : " +
					"-p <required port> -l <optional logLevel 1-5> -t <optional timeout in seconds>" +
					"-i <serverip> -u <user> -w <password> -r <resource> -v <version>"
					);
			System.exit(-1);
		}
		ProxyServerThread t = new ProxyServerThread(port, timeout, logLevel);
		t.start();
		ProxyClient socketClient = new ProxyClient(port, timeout, logLevel, ip, user, password, resource, clientVersion);
		socketClient.run();
	}

}
