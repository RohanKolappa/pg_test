package com.ipvs.notification;


public class NotificationRequest {
	String notificationType;
	String notificationRequestData;
	String notificationId;
	String notificationMessage;
	Long timestamp;
	String nameSpace;
	String notificationChannel;
	
	
	NotificationRequest(String notificationType,String notificationRequestData, 
						String notificationId, String notificationMessage, String nameSpace, String notificationChannel) {
		this.notificationType = notificationType;
		this.notificationRequestData = notificationRequestData;
		this.notificationId = notificationId;
		this.notificationMessage = notificationMessage;
		this.timestamp = System.currentTimeMillis();
		this.nameSpace = nameSpace;
		this.notificationChannel = notificationChannel;
	}

	/**
	 * @return the notificationType
	 */
	public String getNotificationType() {
		return notificationType;
	}

	/**
	 * @return the notificationRequestData
	 */
	public String getNotificationRequestData() {
		return notificationRequestData;
	}

	/**
	 * @return the notificationId
	 */
	public String getNotificationId() {
		return notificationId;
	}

	/**
	 * @return the notificationMessage
	 */
	public String getNotificationMessage() {
		return notificationMessage;
	}

	/**
	 * @return the timestamp
	 */
	public Long getTimestamp() {
		return timestamp;
	}

	/**
	 * @return the nameSpace
	 */
	public String getNameSpace() {
		return nameSpace;
	}
	
	/**
	 * @return the notificationChannel
	 */
	public String getNotificationChannel() {
		return notificationChannel;
	}

}
