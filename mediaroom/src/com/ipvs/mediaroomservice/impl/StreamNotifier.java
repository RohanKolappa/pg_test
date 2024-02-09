package com.ipvs.mediaroomservice.impl;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.service.MRDB;
import com.ipvs.notification.BaseNotifier;
import com.ipvs.notification.NotificationRequest;
import com.ipvs.xmpp.client.smack.packets.IPVSMessageExtension;

public class StreamNotifier extends BaseNotifier {

	public StreamNotifier(MRDB db,MRLog log,XMPPF xf, String userJID) {
		super(db,log, xf, userJID, null);
	}
	
	public void queueNotification(String notificationType, String notificationRequestData, String notificationId, String notificationMessage, String channel) {
		this.processNotification(notificationType,notificationRequestData,notificationId,  notificationMessage, false, IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS, channel);
	}

	public void expediteNotification(String notificationType, String notificationRequestData, String notificationId, String notificationMessage, String channel) {
		this.processNotification(notificationType,notificationRequestData,notificationId, notificationMessage, true, IPVSMessageExtension.IPVS_AGENTMESSAGE_XMLNS, channel);
	}
	
	
	 protected  void sendNotification(NotificationRequest sn) {		
			try {	
				//send notification to room
				if(sn.getNotificationType().equals(ROOM_NOTIFICATION_TYPE)) {
					this.log.addInfo("StreamNotifier:sendRoomNotification Key:="+sn.getNotificationId() +" roomNID:="+sn.getNotificationRequestData(), MRLog.OK, MRLog.NOTIFY);
					//instead of room send to appserver jid to avoid message flood on the client side
					//this.xmpp.sendRoomMessage(sn.getNotificationRequestData(), sn.getNotificationMessage(),sn.getNameSpace());
					this.xf.xmpp(userJID).sendMessage(sn.getNotificationRequestData(), sn.getNotificationMessage(),sn.getNameSpace());
				} 
			}
			catch(Exception e) {
				e.printStackTrace();
				this.log.addInfo("StreamNotifier:sendNotification Key:="+sn.getNotificationId() +" roomNID:="+sn.getNotificationRequestData() +" exception=" + e.toString(), MRLog.OK, MRLog.NOTIFY);
			}
	}
}
