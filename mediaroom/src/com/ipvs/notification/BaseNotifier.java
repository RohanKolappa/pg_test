package com.ipvs.notification;

import java.util.Iterator;
import java.util.LinkedHashMap;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.service.AuthTokenCache;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.sessionservice.impl.SessionNotifier;



public abstract class BaseNotifier implements Runnable {
	protected static long holdTimeMillis = 1000;	
	protected LinkedHashMap<String, NotificationRequest> queue = new LinkedHashMap<String, NotificationRequest>();
	protected MRLog log;	
	protected XMPPF xf;
	protected String userJID;
	protected MRDB db;
	protected boolean reset = false; 
//	protected AppServerAgent appServerAgent = null;
	protected AuthTokenCache authTokenCache = null;
	
	public static final String ROOM_NOTIFICATION_TYPE  = "Room";
	
	public BaseNotifier(MRDB db,MRLog log,XMPPF xf, String userJID, AuthTokenCache authTokenCache) {		
		this.log =log;
		this.db =db;
		this.xf= xf;
		this.userJID = userJID;
		this.authTokenCache = authTokenCache;
	}
	
	public abstract void queueNotification(String notificationType, String notificationRequestData, String notificationId, String notificationMessage, String channel);

	public abstract void expediteNotification(String notificationType, String notificationRequestData, String notificationId, String notificationMessage, String channel);
	
	protected abstract void sendNotification(NotificationRequest sn);
	
	protected synchronized void processNotification(String notificationType,String notificationRequestData,String notificationId, String notificationMessage, boolean expedite, String nameSpace, String channel) {
		// this.log.addInfo("SessionNotifier:queueNotification notificationMessage=" +
		//		notificationMessage, MRLog.OK, MRLog.NOTIFY);
		this.log.addInfo("BaseNotifier:processNotification notificationId:="+notificationId, MRLog.OK, MRLog.NOTIFY);
		NotificationRequest sn = new NotificationRequest(notificationType, notificationRequestData, notificationId,notificationMessage, nameSpace, channel);
		
		if(this.queue.containsKey(notificationId)) {
			//System.out.println("SessionNotifier:queueNotification key exists=" + notificationId);
			this.queue.remove(notificationId);
		}
		if(expedite) {
			this.sendNotification(sn);
		} else {
			this.queue.put(notificationId, sn);
		}
		// System.out.println("SessionNotifier:queueNotification queue size=" + this.queue.size());
		// this.flushNotifications();
	}
	
	synchronized void flushNotifications() {
		Iterator<NotificationRequest> i = this.queue.values().iterator();
		Long now = System.currentTimeMillis();
		while(i.hasNext()) {
			NotificationRequest sn = i.next();
			if((now - sn.timestamp) < SessionNotifier.holdTimeMillis)
			  return;
			this.sendNotification(sn);
	    	i.remove();
		}
	}

	// Flush all notifications for this session
	public synchronized void flushNotificationsOnRoomDelete(String roomNID) {
		Iterator<NotificationRequest> i = this.queue.values().iterator();
		while(i.hasNext()) {
			NotificationRequest sn = i.next();
			if(sn.notificationRequestData == roomNID) {
				this.log.addInfo("BaseNotifier:flushNotificationsOnRoomDelete message=" +
						sn.notificationMessage, MRLog.OK, MRLog.NOTIFY);
				this.sendNotification(sn);
		    	i.remove();
			}
		}
	}
	
	public void run() {
		this.log.addInfo(this.getClass().getCanonicalName() + ":run started ...", MRLog.OK, MRLog.NOTIFY);
		try {
			while(!reset) {
				// System.out.print(".");
				Thread.sleep(SessionNotifier.holdTimeMillis/2);
				// System.out.print("+");
				this.flushNotifications();
				Thread.yield();
			}
		}
		catch(Exception e) {
			e.printStackTrace();
		}
		this.log.addInfo(this.getClass().getCanonicalName() + ":run exiting ...", MRLog.OK, MRLog.NOTIFY);
	}
	
	public void setReset(boolean reset) {
		this.reset = reset;
	}
	
	public void sendMessage(String userJID, String messageXML,String nameSpace) throws Exception{
		//System.out.println(StringUtils.substringBefore(userJID, "@localhost"));
		
		AuthTokenCache.UserResource userResource  = authTokenCache.getUserResource(StringUtils.substringBefore(userJID, "@localhost"));
		//System.out.println("userResource +=========="+userResource);
		if(userResource==null || !userJID.equals(userResource.getUserJID())) {
			//send notification through xmpp
			this.xf.xmpp(userJID).sendMessage(userJID, messageXML,nameSpace);
		} else {
			//send notification through http
			//TODO:Remove x element from message XML	
			Document messageDoc = MRXMLUtils.stringToDocument(messageXML);
			MRXMLUtils.removeNameSpace(messageDoc.getRootElement());
			messageXML = MRXMLUtils.elementToString(MRXMLUtils.getFirstChild(messageDoc.getRootElement()));
			userResource.addNotification(messageXML);
		}
	}

}
