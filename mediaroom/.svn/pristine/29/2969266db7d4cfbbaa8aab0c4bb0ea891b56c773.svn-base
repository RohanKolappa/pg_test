package com.ipvs.common.service;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.atomic.AtomicInteger;

public class CommonThreadPoolManager {
	ExecutorService notificationFixedPool;
	ExecutorService agentResponseFixedPool;
	ExecutorService auditEventFixedPool;
	ExecutorService autoStartFixedPool;
	private CommonThreadPoolManager(){
		notificationFixedPool= Executors.newFixedThreadPool(15,new CommonThreadFactory("Notification"));		
		agentResponseFixedPool= Executors.newFixedThreadPool(15,new CommonThreadFactory("AgentResponse"));
		auditEventFixedPool= Executors.newFixedThreadPool(15,new CommonThreadFactory("AuditEvent"));
		autoStartFixedPool = Executors.newFixedThreadPool(15,new CommonThreadFactory("AutoStart"));
	}
	public static CommonThreadPoolManager getInstance(){
		return new CommonThreadPoolManager();
	}
	public ExecutorService getNotificationFixedPool(){
		return notificationFixedPool;
	}
	public ExecutorService getAgentResponseFixedPool(){
		return agentResponseFixedPool;
	}
	public ExecutorService getAuditEventFixedPool(){
		return auditEventFixedPool;		
	}
	public ExecutorService getAutoStartFixedPool(){
		return autoStartFixedPool;
	}
static class CommonThreadFactory implements ThreadFactory {
	static final AtomicInteger poolNumber = new AtomicInteger(1);
	final ThreadGroup group;
	final AtomicInteger threadNumber = new AtomicInteger(1);
	final String namePrefix;
	public CommonThreadFactory(String name) {
		SecurityManager s = System.getSecurityManager();
		group = (s != null)? s.getThreadGroup() :Thread.currentThread().getThreadGroup();
		namePrefix = name +"-" +poolNumber.getAndIncrement() +"-thread-";
	}		
	
	/*public static CommonThreadFactory getInstance() {
		return new CommonThreadFactory("Notification-thread");
	}*/
	
	public Thread newThread(Runnable r) {
		Thread t = new Thread(group, r,namePrefix + threadNumber.getAndIncrement(),0);
		if (t.isDaemon())
			t.setDaemon(false);
		if (t.getPriority() != Thread.NORM_PRIORITY)
			t.setPriority(Thread.NORM_PRIORITY);
		return t;
	}
}
}
