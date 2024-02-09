package com.barco.device.vng.DeviceAdmin.deviceengine;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.barco.device.common.DeviceDBI;
import com.barco.device.vng.DeviceAdmin.DeviceAdminEngineI;
import com.barco.device.vng.DeviceAdmin.DeviceAdminEngineObserverI;
import com.barco.utils.XMLUtils;

/**
 * The engine accepts and executes cli commands.
 * 
 * Spawns a thread that opens a serversocket at port CLI_PORT (10007) to parse
 * cli commands, which are delegated to this.handleNotification(line).
 * 
 * Stops when setResetServer(true) is called.
 * 
 * Manages a list of {@link DeviceAdminEngineObserverI}
 */
public abstract class BaseDeviceAdminEngineImpl implements Runnable,DeviceAdminEngineI {
	private static final String GET_STREAMING_STATUS_CLI_CMD = "get_streaming_status";
	protected volatile boolean resetThread = false;

	protected Logger log = null;
	protected List<DeviceAdminEngineObserverI> deviceServerObserverList = null;
	protected DeviceDBI  deviceDB= null;
	
	public BaseDeviceAdminEngineImpl (Logger log, DeviceDBI deviceDB){
		this.log =log;	
		this.deviceDB = deviceDB;
		setResetServer(false);
		Thread thread = new Thread(this,"DeviceServer");
		thread.start();		
	}
	
	@Override
	public void addObserver(DeviceAdminEngineObserverI deviceServerObserverObject) {
		if(this.deviceServerObserverList==null){
			this.deviceServerObserverList = new ArrayList<DeviceAdminEngineObserverI>();
		}
		this.deviceServerObserverList.add(deviceServerObserverObject);
	}
	
	public boolean isResetCLI() {
		return resetThread;
	}

	public void setResetServer(boolean bresetCLI) {
		resetThread = bresetCLI;
	}

	public BaseDeviceAdminEngineImpl() {
		
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
			serverSocket = new ServerSocket(DeviceAdminEngineConst.CLI_PORT);
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

		log.info("ConfigListener : Create ServerSocket");

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

		log.info("ConfigListener : Closing ServerSocket Connection");
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

		log.info("ConfigListener : Closing Client Socket Connection");
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

		log.info("ConfigListener : Accept Client Socket Connection");

		return client;
	}
}
