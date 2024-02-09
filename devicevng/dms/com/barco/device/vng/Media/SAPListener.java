package com.barco.device.vng.Media;


import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.net.*;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;


class UDPListener {
	int port;
	String bcastIP;
	int timeout;
	MulticastSocket socket;
	byte[] buffer;
	DatagramPacket datagramPacket;
	
	void init(String bcastIP, int port, int timeout) throws Exception {
		this.bcastIP = bcastIP;
		this.timeout = timeout;
	    InetAddress group = InetAddress.getByName(bcastIP);
	    socket = new MulticastSocket(port);
	    socket.setSoTimeout(timeout);
	    socket.joinGroup(group);
	    buffer = new byte[10*1024];
        datagramPacket = new DatagramPacket(buffer, buffer.length);
	}

	String getNextData() throws Exception {
		try {
			socket.receive(datagramPacket);
			return new String(buffer, 0, datagramPacket.getLength());
		}
		catch(SocketTimeoutException ste) {
			return null;
		}
		catch(Exception e) {
			throw e;
		}
	}
	
	void close() {
		try {
			System.out.println("Closing socket");
			socket.close();
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
}

class SDPData {

	String mcastIP;
	String payload;
	long ts;
	
	SDPData(String mcastIP, String payload) {
		this.mcastIP = mcastIP;
		this.payload = payload;
		this.ts = System.currentTimeMillis();
	}
	
    static String tokenize(String sdp, String regex, String replacement) throws Exception {    	
		Matcher m = Pattern.compile(regex).matcher(sdp);
		if(!m.find())
			return sdp;
    	StringBuffer sb = new StringBuffer();
		m.appendReplacement(sb, replacement);
    	while(m.find()) {
    		m.appendReplacement(sb, replacement);
    	}
    	m.appendTail(sb);
		return sb.toString();
    }
    
	static String scrubSDP(String sdp) throws Exception {
		if((sdp.indexOf("ngs-d220") == -1)) // no need to process
			return sdp;
		sdp = tokenize(sdp, ";timestamp-offset=([0-9\\.]+);", ";");
		sdp = tokenize(sdp, ";seqnum-offset=([0-9\\.]+);", ";");
		sdp = tokenize(sdp, "\";", "");
		sdp = tokenize(sdp, "=\"", "=");
		return sdp;
	}
	
	static String getMcastIP(String sdp) throws Exception {
		Matcher m = Pattern.compile("(c=IN\\s+IP4)\\s+([0-9\\.]+)").matcher(sdp);
		m.find();
		String mcastIP = m.group(2);
		StringTokenizer st = new StringTokenizer(mcastIP, ".");
	    int firstbit = 0;
	    try {
	    	firstbit = Integer.parseInt(st.nextToken());
	    }
	    catch(Exception e) {
	       	return null;
	    }
	    if((firstbit < 224) || (firstbit > 239))
	       	return null;
	    return mcastIP;
	}
	
	static SDPData parse(String sdp) throws Exception {
		int startIndex = sdp.indexOf("v=0");
		if(startIndex == -1)
			return null;
       sdp = sdp.substring(startIndex);
       String mcastIP = getMcastIP(sdp);
       if(mcastIP == null)
    	   return null;
       sdp = scrubSDP(sdp);
       return new SDPData(mcastIP, sdp);
	}
}


public class SAPListener implements Runnable {

	String bcastIP;
	int port;
	long expiry;
	int timeout = 10000;
	String path;
	File folder;

	Hashtable<String, SDPData> sdpTable = new Hashtable<String, SDPData>();
	UDPListener listener = new UDPListener();
	boolean done = false;
	
	/*
	 * Listens for SAP broadcasts dectects when SDPs become available or unavailable
	 * @bcastIP : SAP broadcast IP ["224.2.127.254", "239.195.255.255", "239.255.255.255", "224.0.0.255"]
	 * @port : SAP broadcast port 9875
	 * @expiry : Time in milliseconds to wait for rebroadcast of SAP before marking as unavailable
	 * @path : Directory to keep track of sdps across restarts
	 */
	public SAPListener(String bcastIP, int port, long expiry, String path) throws Exception {
		this.bcastIP = bcastIP;
		this.port = port;
		this.expiry = expiry;
		this.path = path;
		folder = new File(path);
		File[] listOfFiles = folder.listFiles();
		for(int i = 0; i < listOfFiles.length; i++) {
			String mcastIP = listOfFiles[i].getName();
			sdpTable.put(mcastIP, new SDPData(mcastIP, null));
		}
	}
	
	/*
	 * A new SDP was found over SAP broadcast
	 * @mcastIP : The multicast IP address in the SDP
	 * @sdp : The sdp text
	 */
	public void handleSDPAvailable(String mcastIP, String sdp) {
		// System.out.println("SAPListener:handleSDPAvailable mcastIP=" + mcastIP);
	}

	/*
	 * A previously found SDP is no longer being broadcast over SAP
	 */
	public void handleSDPUnavailable(String mcastIP) {
		// System.out.println("SAPListener:handleSDPUnavailable mcastIP=" + mcastIP);
	}
	
	/*
	 * This marks the SAPListener as done and forces it to exit the thread
	 */
	public void setDone() {
		done = true;
	}

	
	@Override
	public void run() {
		try {
			listener.init(bcastIP, port, timeout);
		} catch (Exception e) {
			e.printStackTrace();
			return;
		}
		while(!done) {
			try {
				String data = listener.getNextData();
				
				// Check if we have new sdp available
				if(data != null) {
					SDPData sdpData = SDPData.parse(data);
					if(sdpData != null) {
						if(!sdpTable.containsKey(sdpData.mcastIP)) {
							if(sdpData.payload.indexOf("H264") == -1)
								continue;
							
							File file = new File(path, sdpData.mcastIP); // Create the file
							FileWriter writer = new FileWriter(file);
							writer.write(sdpData.payload);
							writer.close();

							this.handleSDPAvailable(sdpData.mcastIP, sdpData.payload);
						}
						else {
							long interval = sdpData.ts - sdpTable.get(sdpData.mcastIP).ts;
							// System.out.println("mcastIP=" + sdpData.mcastIP + " Interval=" + interval);
						}
						sdpTable.put(sdpData.mcastIP, sdpData); // so that the ts is updated
					}
				}
				
				// Check if existing sdp unavailable
				long now = System.currentTimeMillis();
				ArrayList<String> expiredList = new ArrayList<String>();
				Set<String> keys = sdpTable.keySet();
				for(String key:keys) {
					SDPData sdpData = sdpTable.get(key);
					if( (now - sdpData.ts) > expiry)
						expiredList.add(key);
				}
				for(String expired : expiredList) {
					sdpTable.remove(expired);
					File file = new File(path, expired); // Delete the file
					file.delete();
					this.handleSDPUnavailable(expired);
				}
				
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		this.listener.close();
	}
	
	public static void main(String[] args) throws Exception{
		int port = 9875;
		String bcastIP = "224.2.127.254";
		// String[] mcastIPList = {"224.2.127.254", "239.195.255.255", "239.255.255.255", "224.0.0.255"};
		long expiry = 60000;
		// long expiry = 3600 * 1000;
		String path = "C:\\miye\\sap";
		SAPListener sapListener = new SAPListener(bcastIP, port, expiry, path);
		Thread t = new Thread(sapListener);
		t.start();
		Thread.sleep(20000);
		sapListener.setDone();
	}


}
