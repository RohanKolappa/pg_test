package com.ipvs.xmpp.client.smack;

import javax.jmdns.ServiceEvent;
import javax.jmdns.ServiceListener;

import com.ipvs.common.XMPPEventQueue;

public interface DiscoveryServiceI extends ServiceListener{

	public abstract void startDiscoveryService();

    public abstract void stopDiscoveryService();
    
	public abstract void serviceAdded(ServiceEvent event);
	 
	public abstract void serviceRemoved(ServiceEvent event);
	
	public abstract void serviceResolved(ServiceEvent event);
	 
	public abstract void setXMPPEventQueue(XMPPEventQueue xmppEventQueue);
	
	public abstract String getUserJID();
	
	public abstract void setUserJID(String userJID);
}
