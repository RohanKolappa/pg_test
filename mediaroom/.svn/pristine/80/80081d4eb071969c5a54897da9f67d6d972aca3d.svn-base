package com.ipvs.assetadminservice.file.handler;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.atomic.AtomicInteger;


public class PublishFileThreadFactory implements ThreadFactory {
	static final AtomicInteger poolNumber = new AtomicInteger(1);
	final ThreadGroup group;
	final AtomicInteger threadNumber = new AtomicInteger(1);
	final String namePrefix;
	private static ExecutorService fixedPool;
	private static PublishFileThreadFactory publishFileThreadFactory = null;
	public static PublishFileThreadFactory getInstance(String name, Integer capacity){
		if(publishFileThreadFactory==null){
			publishFileThreadFactory = new PublishFileThreadFactory(name);
			fixedPool = Executors.newFixedThreadPool(capacity,publishFileThreadFactory);
		}
		return publishFileThreadFactory;
	}
	private PublishFileThreadFactory(String name) {
		SecurityManager s = System.getSecurityManager();
		group = (s != null)? s.getThreadGroup() :Thread.currentThread().getThreadGroup();
		namePrefix = name +"-" +poolNumber.getAndIncrement() +"-thread-";
	}		
	public Thread newThread(Runnable r) {
		Thread t = new Thread(group, r,namePrefix + threadNumber.getAndIncrement(),0);
		if (t.isDaemon())
			t.setDaemon(false);
		if (t.getPriority() != Thread.NORM_PRIORITY)
			t.setPriority(Thread.NORM_PRIORITY);
		return t;
	}
	public ExecutorService getThreadPool(){
		return fixedPool;
	}
}
