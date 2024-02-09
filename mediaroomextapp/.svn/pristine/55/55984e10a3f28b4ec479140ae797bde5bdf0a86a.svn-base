package com.barco.device.handler.DeviceAdmin.impl.deviceserver;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

import com.barco.device.handler.DeviceAdmin.impl.DeviceServerI;
import com.barco.utils.mr.MRLog;

public abstract class BaseDeviceServerImpl implements Runnable,DeviceServerI {
	protected volatile boolean resetThread = false;
	protected String configFile = "/etc/config/Device.xml"; //TODO: Check and find best place to add this one

	protected MRLog log = null;
	
	public BaseDeviceServerImpl (MRLog log, String configFile){
		this.log =log;	
		this.configFile = configFile;
		setResetServer(false);
		Thread thread = new Thread(this,"DeviceServer");
		thread.start();		
	}

	public boolean isResetCLI() {
		return resetThread;
	}

	public void setResetServer(boolean bresetCLI) {
		resetThread = bresetCLI;
	}

	public BaseDeviceServerImpl() {
		
	}
	
	protected abstract void handleNotification(String notification);
	
	
	/**
	 * We are acting as a Server. Open the Socket connection and listen for messages from system. 
	 */
	public void run() {
		ServerSocket serverSocket = null;

		serverSocket = createServerSocketConnection();

		Socket clientSocket = null;

		while (!resetThread) {
			clientSocket = acceptConnection(serverSocket);

			if (clientSocket == null) {
				continue;
			}

			try {
				String notification = getNextcommand(clientSocket);

				if ((notification != null) && !notification.isEmpty()) {
					handleNotification(notification);
				}
			} catch (Exception e) {
				e.printStackTrace();
			}

			closeClientSocket(clientSocket);
			clientSocket = null;
		}

		closeServerSocket(serverSocket);
		serverSocket = null;
	}

	private String getNextcommand(Socket clientSocket) throws Exception {
		BufferedReader reader = null;
		StringBuffer sb = new StringBuffer();

		reader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()));      

		String line = null;

		while ((line = reader.readLine()) != null) {
			if (!clientSocket.isConnected()) {
				return null;
			}

			sb.append(line);
		}

		reader.close();

		return sb.toString();
	}


	private ServerSocket createServerSocketConnection() {
		ServerSocket serverSocket = null;

		try {
			serverSocket = new ServerSocket(DeviceServerConst.CLI_PORT);
		} catch (IOException e) {
			setResetServer(true);

			return null;
		}

		try {
			if (serverSocket != null) {
				serverSocket.setSoTimeout(100);
			}
		} catch (SocketException e) {
			//e.printStackTrace();
			return null;
		}

		log.addInfo("ConfigListener : Create ServerSocket");

		return serverSocket;
	}

	private void closeServerSocket(ServerSocket serverSocket) {
		if (serverSocket == null) {
			return;
		}

		try {
			serverSocket.close();
		} catch (IOException e) {
			//e.printStackTrace();
		}

		log.addInfo("ConfigListener : Closing ServerSocket Connection");
	}

	private void closeClientSocket(Socket clientSocket) {
		if (clientSocket == null) {
			return;
		}

		try {
			clientSocket.close();
		} catch (IOException e) {
			//e.printStackTrace();
		}

		log.addInfo("ConfigListener : Closing Client Socket Connection");
	}

	private Socket acceptConnection(ServerSocket serverSocket) {
		Socket client = null;

		try {
			client = serverSocket.accept();

			if (client == null) {
				return null;
			}

			client.setSoTimeout(100);
		} catch (Exception exp) {
			//exp.printStackTrace();
			return null;
		}

		log.addInfo("ConfigListener : Accept Client Socket Connection");

		return client;
	}

}
