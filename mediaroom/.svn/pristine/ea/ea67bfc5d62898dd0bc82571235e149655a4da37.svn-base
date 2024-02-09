package com.ipvs.sessionservice.impl;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRDB;
import com.ipvs.notification.BaseNotifier;
import com.ipvs.notification.NotificationRequest;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;



public class SessionNotifier extends BaseNotifier {

	public SessionNotificationListenerI listener = null;
	
	public SessionNotifier(MRDB db,MRLog log,XMPPF xf, String userJID, AuthTokenCache authTokenCache) {
		super(db,log, xf, userJID,  authTokenCache);
	}
	
	public void setSessionNotificationListsner(SessionNotificationListenerI listener) {
		this.listener = listener;
	}
		
	public void queueNotification(String notificationType, String notificationRequestData, String notificationId, String notificationMessage, String channel) {
		this.processNotification(notificationType, notificationRequestData, notificationId, notificationMessage, false, IPVSMessageExtension.IPVS_SESSION_XMLNS, channel);
	}

	public void expediteNotification(String notificationType, String notificationRequestData, String notificationId, String notificationMessage, String channel) {
		this.processNotification(notificationType, notificationRequestData, notificationId, notificationMessage, true, IPVSMessageExtension.IPVS_SESSION_XMLNS, channel);
	}
	
	 protected  void sendNotification(NotificationRequest sn) {		
		try {	
			//send notification to room
			if(sn.getNotificationType().equals(ROOM_NOTIFICATION_TYPE)) {
				this.log.addInfo("SessionNotifier:sendNotification Key:="+sn.getNotificationId(), MRLog.OK, MRLog.NOTIFY);
				// this.xmpp.sendRoomMessage(sn.getNotificationRequestData(), sn.getNotificationMessage(),sn.getNameSpace());
				//this.xf.xmpp(userJID).sendMessage(sn.getNotificationRequestData(), sn.getNotificationMessage(),sn.getNameSpace());
				sendMessage(sn.getNotificationRequestData(), sn.getNotificationMessage(),sn.getNameSpace());
				if(listener != null) {
					listener.handleNotification(sn.getNotificationChannel(), sn.getNotificationMessage());
				}
			} 
		}
		catch(Exception e) {
			e.printStackTrace();
			this.log.addInfo("StreamNotifier:sendNotification exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
		}
	}
}
