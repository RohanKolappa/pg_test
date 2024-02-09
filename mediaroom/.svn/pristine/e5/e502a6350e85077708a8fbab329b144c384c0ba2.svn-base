package com.ipvs.proxy.app;

import java.io.BufferedWriter;
import java.io.OutputStreamWriter;
import java.sql.Timestamp;
import java.util.TimerTask;

import com.ipvs.common.MRLog;
import com.ipvs.proxy.impl.ProxyClientUtils;
import com.ipvs.proxy.impl.ProxyConstants;

public class ProxyServerHeartBeat extends TimerTask{

	ProxyServer client = null;
	MRLog log;
	ProxyServerHeartBeat() {

	}

	ProxyServerHeartBeat(ProxyServer client, MRLog log) {
		this.client = client;
		this.log = log;
		this.log.addInfo("HearBeatServer initialized");
	}

	public void run() {
		try {

			
			
			if(client.getLastHeartBeatFromClientTS()!=null && !ProxyConstants.ENABLE_DISCONNECT) {
				Timestamp cusTS = new Timestamp(System.currentTimeMillis());
				long diff = cusTS.getTime() - client.getLastHeartBeatFromClientTS().getTime();
				if(diff>ProxyClientUtils.getHeartbeatFrequency()*3) {
					this.log.addInfo("Did not receive heartbeat from client");
					this.cancel();
					client=null;
					
				}
				else {
					this.log.addInfo("Sending heartbeat message");	
					String heartBeatMessage = ProxyClientUtils.generateHeartBeat(ProxyServer.getAck(), Integer.toString(ProxyServer.getSeq()+1), log);
					ProxyServer.setSeq(ProxyServer.getSeq()+1);
					if(client.getClient()!=null){
						BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(client.getClient().getOutputStream()));
						ProxyClientUtils.socketWrite(writer, heartBeatMessage, log);
					}	
					
				}
			}

			
		}
		catch(java.net.SocketException se) {
			this.cancel();
			client = null;
			
		}
		catch (Exception e) {
			
			e.printStackTrace();
		}
		
	}

	public ProxyServer getClient() {
		return client;
	}

	public void setClient(ProxyServer client) {
		this.client = client;
	}

}
