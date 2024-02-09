package com.ipvs.common.service;

import java.util.Hashtable;
import java.util.concurrent.Semaphore;

public class IPVSMonitor {
	
	static IPVSMonitor monitor = new IPVSMonitor();
	Hashtable<String,Semaphore> list = new Hashtable<String,Semaphore>();
	
	public static IPVSMonitor getMonitor() {
		return monitor;
	}
	
	public Semaphore getLock(String id, String name) throws Exception {
		Semaphore lock = this.addLock(id, name);
		lock.acquire();		
		return lock;
	}
	
	public synchronized Semaphore addLock(String id, String name) throws Exception {
		if(!list.containsKey(id)) {
			list.put(id, new Semaphore(1, true));
		}
		Semaphore lock = list.get(id);
		return lock;
	}
	
	public synchronized void releaseLock(String id, Semaphore lock) {
		
		// Check if we need to now gc the monitor for the ID
		if(!lock.hasQueuedThreads()) {
			if(list.containsKey(id))
				list.remove(id);
			//System.out.println("RelaseLock: Monitor list size=" + list.size() +" id:="+id);
		}
		
		//release lock after removing from queue. 
		//hasQuequedThreads return false if some thread able to acquired lock as current thread already released. 
		//Next thread who is acquired never get released because we already removed object from list. Itrack #VNG0001-2691
		lock.release();
		
	}
	
	
	public static void main(String[] args) throws Exception {
		String id = "id";
		for(int i = 0; i < 5; i++) {
			Thread.sleep(1000);
			(new Thread(new IPVSMonitorTest(String.valueOf(i), id))).start();
		}
		System.out.println("IPVSMonitorTest Done");
	}
}

class IPVSMonitorTest implements Runnable  {
	String id;
	String name;
	static final int RUN_TIME = 5000;
	
	IPVSMonitorTest(String name, String id) {
		this.name = name;
		this.id = id;
	}
	
	public void run() {
		System.out.println(this.name + " Acquiring ...");
		
		// Get the lock
		Semaphore lock = null;
		try {
			lock = IPVSMonitor.getMonitor().getLock(id, name);
		}
		catch(Exception e) {
			System.out.println(this.name + " Acquiring ... Failed");
			e.printStackTrace();
			return;
		}
		System.out.println(this.name + " Acquiring ... Done");
		
		// Execute 
		try {	
			System.out.println(this.name + " Executing ...");
			Thread.sleep(RUN_TIME);
			System.out.println(this.name + " Executing for ... Done");
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		
		// Release the lock
		IPVSMonitor.getMonitor().releaseLock(id, lock);
	}
	
}

