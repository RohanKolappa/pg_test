package com.barco.device.vng.DeviceAdmin;

import org.dom4j.Document;

public interface DeviceAdminEngineObserverI {
	
	public void handleEngineNotification(String notificationType,Document notificationDocument);
	
}
