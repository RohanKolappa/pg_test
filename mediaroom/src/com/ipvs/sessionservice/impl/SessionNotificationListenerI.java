package com.ipvs.sessionservice.impl;

public interface SessionNotificationListenerI {

	public void handleNotification(String sessionId, String notification);

}
