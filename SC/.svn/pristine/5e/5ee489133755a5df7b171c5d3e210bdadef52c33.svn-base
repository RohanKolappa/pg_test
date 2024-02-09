/**
 * Copyright (c) 2014 Barco n.v. All Rights Reserved. This software is confidential and proprietary information of Barco n.v. ("Confidential
 * Information"). You shall not disclose such Confidential Information and shall use it only in accordance with the terms of the license agreement you
 * entered into with Barco.
 */

package com.barco.nimbus.license;

import java.util.Date;
import java.util.Iterator;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

import org.apache.log4j.Logger;

import com.barco.nimbus.license.api.LicenseStatus;

/**
 * @author JMRA
 * 
 *         This Class helps License service to initialize schedulers for license
 *         expiration checks.
 */
public class LicenseScheduler implements LicenseSchedulerI {
	private static final Logger logger = Logger
			.getLogger(LicenseScheduler.class);
	private static int SCHEDULED_THREAD_POOL_COUNT = 2;
	private int TIMER_DELAY = 60;// in seconds

	private static Map<String, LicenseSchedulerTask> mScheduledTasksMap = new ConcurrentHashMap<String, LicenseSchedulerTask>();

	private LicenseSchedulerCallbackI licenseSchedulerCallback;
	private ScheduledExecutorService scheduledExecutorService;

	/**
	 * @param aLicenseSchedulerCallbackI
	 */
	private LicenseScheduler() {
		scheduledExecutorService = Executors
				.newScheduledThreadPool(SCHEDULED_THREAD_POOL_COUNT);

		// start system timer change handler thread.
		SystemTimeChangeTrackerTask systemTimeChecker = new SystemTimeChangeTrackerTask(
				System.currentTimeMillis());
		scheduledExecutorService.scheduleWithFixedDelay(systemTimeChecker, 5,
				TIMER_DELAY, TimeUnit.SECONDS);
	}

	/**
	 * @see com.barco.nimbus.license.LicenseSchedulerI#schedule(java.util.Date)
	 */
	@Override
	public synchronized void schedule(Date aLicenseExpiry) {
		logger.info(":schedule schedule to check license expiry for date - "
				+ aLicenseExpiry);

		long expiry = aLicenseExpiry.getTime();
		long systemCurrentTime = System.currentTimeMillis();

		long timeDiff = expiry - systemCurrentTime;

		if (timeDiff <= 0) {
			sendExpiryNotification();
			return;
		}

		LicenseSchedulerTask task = new LicenseSchedulerTask(
				"LicenseExpiryScheduler - " + System.currentTimeMillis(),
				aLicenseExpiry);

		// schedule a timer with initial delay till aLicenseExpiry.
		ScheduledFuture<?> sf = scheduledExecutorService.schedule(task,
				timeDiff, TimeUnit.MILLISECONDS);
		task.setScheduledFuture(sf);
		mScheduledTasksMap.put(task.getId(), task);

		logger.debug("schedule: scheduled for License Expiry - "
				+ aLicenseExpiry);
	}

	/**
	 * Cancel all schedulers
	 */
	public synchronized void clear() {
		logger.debug("clear: clearing all running schedulers");
		for (String taskId : mScheduledTasksMap.keySet()) {
			mScheduledTasksMap.get(taskId).getScheduledFuture()
					.cancel(Boolean.TRUE);
		}

		mScheduledTasksMap.clear();
	}

	public LicenseSchedulerCallbackI getLicenseSchedulerCallback() {
		return licenseSchedulerCallback;
	}

	public void setmLicenseSchedulerCallback(
			LicenseSchedulerCallbackI aLicenseSchedulerCallback) {
		licenseSchedulerCallback = aLicenseSchedulerCallback;
	}

	private void sendExpiryNotification() {
		licenseSchedulerCallback
				.notifyLicenseUpdate(LicenseStatus.LicenseExpired);
	}

	// Helper method useful to reset all schedulers.
	private synchronized void rescheduleAllSchedulers() {
		Iterator<String> it = mScheduledTasksMap.keySet().iterator();
		while (it.hasNext()) {
			String taksId = (String) it.next();
			LicenseSchedulerTask task = mScheduledTasksMap.get(taksId);
			task.getScheduledFuture().cancel(Boolean.TRUE);
			logger.debug("rescheduleAllSchedulers: rescheduling for date :"
					+ task.getExpiryDate() + "; taskCanceled for name:"
					+ task.getId() + "; isSchedulerCancelled: "
					+ task.getScheduledFuture().isCancelled());
			schedule(task.getExpiryDate());
			task.setScheduledFuture(null);
			mScheduledTasksMap.remove(taksId);
		}
	}

	/*
	 * Helper thread to schedule for license expiration.
	 */
	private final class LicenseSchedulerTask implements Runnable {
		private final Logger logger = Logger
				.getLogger(LicenseSchedulerTask.class.getName());

		private String name = "";
		private String id = "";
		private Date expiryDate;
		private ScheduledFuture<?> sf;

		/**
		 * @param anId
		 * @param aName
		 */
		public LicenseSchedulerTask(String anId, Date anExpiryDate) {
			id = anId;
			expiryDate = anExpiryDate;
			name = "LicenseExpiryScheduler for Date " + expiryDate.toString();
		}

		@Override
		public void run() {
			try {
				logger.info("License Expired for " + getName()
						+ "; sending notification to license service... ");

				sendExpiryNotification();
			} catch (Exception e) {
				logger.debug("failed to run schedular for " + this.getId(), e);
			} finally {
				try {
					cancelSchedulerAndRemoveFromMap(getScheduledFuture(), id);
				} catch (Exception e) {
					logger.debug("failed to stop shedular: " + getName());
				}
			}
		}

		private String getName() {
			return name;
		}

		private String getId() {
			return id;
		}

		private ScheduledFuture<?> getScheduledFuture() {
			return sf;
		}

		private void setScheduledFuture(ScheduledFuture<?> sf) {
			this.sf = sf;
		}

		private Date getExpiryDate() {
			return expiryDate;
		}
	}

	private synchronized void cancelSchedulerAndRemoveFromMap(
			ScheduledFuture<?> scheduledFuture, String schedulerId) {
		scheduledFuture.cancel(Boolean.TRUE);
		mScheduledTasksMap.remove(schedulerId);
	}

	/*
	 * Helper thread to check system date time change.
	 */
	private final class SystemTimeChangeTrackerTask implements Runnable {
		private final Logger logger = Logger
				.getLogger(SystemTimeChangeTrackerTask.class.getName());
		private long mLastChangedTime;

		/**
		 * @param lastChangedTime
		 */
		public SystemTimeChangeTrackerTask(long lastChangedTime) {
			mLastChangedTime = lastChangedTime;
		}

		@Override
		public void run() {
			try {
				long currTimeinMillis = System.currentTimeMillis();
				long timeDifferenceFromLastExec = currTimeinMillis
						- mLastChangedTime;
				long timeInSeconds = TimeUnit.MILLISECONDS
						.toSeconds(timeDifferenceFromLastExec);

				if (timeDifferenceFromLastExec <= 0) {
					sendExpiryNotification();
				} else if (timeInSeconds > TIMER_DELAY) {
					logger.info("System time modified rescheduling all schedulers... ");
					rescheduleAllSchedulers();
				}

				mLastChangedTime = currTimeinMillis;

			} catch (Exception e) {
				logger.warn(
						"Sytem Time checker - failed to reschedule all schedulers, if system time changes in the background, sheduler can't notify it.",
						e);
			}
		}
	}

	/*
	 * Will return singleton object for license scheduler.
	 */
	enum LicenseSchedulerSingleton {
		instance;

		private LicenseScheduler ls;

		private LicenseSchedulerSingleton() {
			ls = new LicenseScheduler();
		}

		/**
		 * @return BLM Singleton instance
		 */
		LicenseScheduler getLicenseScheduler() {
			return ls;
		}
	}
}
