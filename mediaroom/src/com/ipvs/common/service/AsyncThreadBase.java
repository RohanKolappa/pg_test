package com.ipvs.common.service;

import java.util.ArrayList;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;

public abstract class AsyncThreadBase extends Thread {
	
	ArrayList<Object> queue;
	
	protected AsyncThreadBase()
	{
		queue = new ArrayList<Object>();
		start();
	}
	
	protected synchronized void addRequest(Object object ) {
		this.queue.add(object);
	}
	protected synchronized Object removeRequest() {
		Object request = queue.get(0);
		queue.remove(0);
		return request;
	}
	
	public void run() {
		while(true) {

			try {
				  try {
		                Thread.sleep(500);
		            } catch (InterruptedException e) {
		                e.printStackTrace();
		            }
				if(!queue.isEmpty()) {
					this.flushRequests();					
				}
			} catch(Exception e) {
				MRLog.println("AsynchThreadBase Failed to handle Request");
				e.printStackTrace();
			}
		}
	}
	
	void flushRequests() throws Exception {
		while(true) {
			Object rq = this.getNextRequest();
			if(rq == null)
				return;
			handleRequest(rq);
		}
	}

	private Object getNextRequest() {
		if(queue.isEmpty())
			return null;		
		return removeRequest();
	}	
	
	protected abstract void handleRequest(Object request) throws MRException,Exception ;
}
