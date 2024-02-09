package com.barco.device.base;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;

import com.barco.device.common.DeviceFrameworkI;
import com.barco.device.common.Message;

class Notification {
	long ts;
	Message m;
	Notification(Message m) {
		this.m = m;
		this.ts = System.currentTimeMillis();
	}
}
/**
 * A delaying construct around sendMessage: 
 * <li> queueMessage
 * schedules a message to be sent in 1 second (asynchronously
 * using a dedicated thread). 
 * <li> sendMessage sends a message in the caller thread, thereby
 * canceling an potentially pending message (with the same key)
 * from the queue.
 */
public class Notifier implements Runnable {
	static long holdTimeMillis = 1000;	
	DeviceFrameworkI deviceFramework;
	LinkedHashMap<String, Notification> queue = new LinkedHashMap<String, Notification>();
	Thread thread;
	
	public Notifier(DeviceFrameworkI deviceFramework) {
		this.deviceFramework = deviceFramework;
		this.thread = null;
	}
	
	public synchronized void sendNotification(String key, Message m) {
		this.deviceFramework.getLogger().info("Notifier:sendNotification key:="+key);
		this.queue.remove(key);
		try {
			this.deviceFramework.sendMessage(m);
		}
		catch(Exception e) {
			this.deviceFramework.getLogger().info("Notifier:sendNotification key:="+key + " error=" + e.toString());
		}
	}
	
	public synchronized void queueNotification(String key, Message m) {
		this.deviceFramework.getLogger().info("Notifier:queueNotification key:="+key);
		this.queue.put(key, new Notification(m));
		if((thread != null) && (this.thread.isAlive()))
			return;
		thread = new Thread(this);
		this.thread.start();
	}
	
	synchronized void flushNotifications() {
		Iterator<String> i = this.queue.keySet().iterator();
		long now = System.currentTimeMillis();
		ArrayList<String> keysToBeRemoved = new ArrayList<String>();
		while(i.hasNext()) {
			String key = i.next();
			Notification n = this.queue.get(key);
			if((now - n.ts) < Notifier.holdTimeMillis)
				continue;
			keysToBeRemoved.add(key);
			try {
				this.deviceFramework.sendMessage(n.m);
				this.deviceFramework.getLogger().info("Notifier:flushNotification key:="+key);
			}
			catch(Exception e) {
				this.deviceFramework.getLogger().info("Notifier:flushNotification key:="+key + " error=" + e.toString());
			}
		}
		for(String key: keysToBeRemoved)
			this.queue.remove(key);
	}

	public void run() {
		try {
			while(!this.queue.isEmpty()) {
				Thread.sleep(Notifier.holdTimeMillis);
				this.flushNotifications();
				Thread.yield();
			}
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
}
