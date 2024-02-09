package com.ipvs.proxy.app;

import java.util.TimerTask;

import com.ipvs.common.MRLog;
import com.ipvs.proxy.impl.ProxyClientUtils;

public class ProxyClientHeartBeat extends TimerTask{

	
	ProxyClient client = null;
	MRLog log = null;

	
	ProxyClientHeartBeat() {

	}

	ProxyClientHeartBeat(ProxyClient client, MRLog log) {
		this.client = client;
		this.log = log;
	}
	public void run() {
		try {
			String heartBeatMessage = ProxyClientUtils.generateHeartBeat(ProxyClient.getAck(), Integer.toString(ProxyClient.getSeq()+1), log);
			ProxyClient.setSeq(ProxyClient.getSeq()+1);
			client.sendHeartBeatToServer(heartBeatMessage);
			
		}
		catch(java.net.SocketException se) {
			this.cancel();
			client.setLastHeartBeatFromServerTS(0);
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}

	public ProxyClient getClient() {
		return client;
	}

	public void setClient(ProxyClient client) {
		this.client = client;
	}
	
}
