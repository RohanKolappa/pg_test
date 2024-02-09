package com.ipvs.common;


import java.util.ArrayList;
import java.util.Calendar;
import java.util.Hashtable;
import java.util.List;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;


public class IPVSScheduler {
	
	private static Hashtable<String, List<ScheduledFuture<?>>> schedulerTaskCollection = new Hashtable<String, List<ScheduledFuture<?>>>();
		
	private static IPVSScheduler theIPVSScheduler;
	private static final ScheduledThreadPoolExecutor theScheduledExecutor = new ScheduledThreadPoolExecutor(5);
	private static final MRLog log = MRLog.getInstance(MRLog.CONSOLE);
	
	private IPVSScheduler() {}
	
	public static synchronized IPVSScheduler getInstance()
	{
		if (theIPVSScheduler == null) {
			theIPVSScheduler = new IPVSScheduler();
		}		
		return theIPVSScheduler;
	}
	
	public synchronized void scheduleDaily(IPVSTask task, int scheduleTime)
	{
		Calendar calender = Calendar.getInstance();
		
		int deltaTime = 0;
		int currentHour = calender.get(Calendar.HOUR_OF_DAY);
		int currentMinute = calender.get(Calendar.MINUTE);
		int currentSecond = calender.get(Calendar.SECOND);
		
		int currentTimeInMins = (currentHour * 3600) + (currentMinute * 60) + currentSecond;
		int scheduleTimeInMins = scheduleTime * 3600;
		int totalNumOfSecs = 24 * 3600;
		
		if (scheduleTimeInMins >= currentTimeInMins) {
			deltaTime = scheduleTimeInMins - currentTimeInMins;
		} else {
			deltaTime = (totalNumOfSecs - currentTimeInMins) + scheduleTimeInMins;
		}
		
		scheduleTask(task, deltaTime, totalNumOfSecs, TimeUnit.SECONDS,false);
	}
	
	public synchronized void scheduleBySeconds(IPVSTask task, int scheduleTime) {
		scheduleTask(task, scheduleTime, scheduleTime, TimeUnit.SECONDS, false);
	}
	public synchronized void scheduleOneShot(IPVSTask task, int initialDelay) {
	    scheduleTask(task, initialDelay, initialDelay, TimeUnit.SECONDS, true);
	}
	public synchronized void scheduleTask(IPVSTask task, int initialDelay, int scheduleTime, TimeUnit unit, boolean isOneShotSchedule) {
		try 
		{
		    ScheduledFuture<?> futureObj;
			String key = task.getTaskID();
			if(!isOneShotSchedule){
			    futureObj = theScheduledExecutor.scheduleAtFixedRate(task, (long)initialDelay, (long)scheduleTime, unit);
			}
			else{
			    futureObj =  theScheduledExecutor.schedule(task, initialDelay, TimeUnit.SECONDS);
			}
												 			
			if (!schedulerTaskCollection.containsKey(key)) {
				List<ScheduledFuture<?>> list = new ArrayList<ScheduledFuture<?>>();
				list.add(futureObj);
				schedulerTaskCollection.put(key, list);	
			} else {				
				List<ScheduledFuture<?>> list = schedulerTaskCollection.get(key);
				list.add(futureObj);
			}
			
			log.addInfo("IPVSScheduler::Size of the task queue = " + getTaskQueue().size(), MRLog.OK, MRLog.NOTIFY);			
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public synchronized void removeTask(String taskID)
	{
		try 
		{	
			if (taskID == null || taskID.isEmpty())
				return;
			
			List<ScheduledFuture<?>> taskList = schedulerTaskCollection.get(taskID);
			
			if (taskList == null) {
				return;
			}
			
			for (ScheduledFuture<?> futureObj : taskList) {
				if (!futureObj.cancel(false)) {					
					log.addInfo("IPVSScheduler::Task deletion failed", MRLog.OK, MRLog.NOTIFY);
				}
			}
									
			if (!schedulerTaskCollection.isEmpty()) {
				schedulerTaskCollection.remove(taskID);
			}
			
			log.addInfo("IPVSScheduler::Size of the task queue = " + getTaskQueue().size(), MRLog.OK, MRLog.NOTIFY);			
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}

	public void shutDown()
	{
		try
		{
			if (!theScheduledExecutor.isTerminated()) {
				theScheduledExecutor.shutdown();
			}
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
	
	public long getActiveTaskCount()
	{
		return theScheduledExecutor.getActiveCount();
	}
	
	public long getPoolSize()
	{
		return theScheduledExecutor.getPoolSize();
	}
	
	public BlockingQueue<Runnable> getTaskQueue()
	{
		return theScheduledExecutor.getQueue();
	}
	
}
