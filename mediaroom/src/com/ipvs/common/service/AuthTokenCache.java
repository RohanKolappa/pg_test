package com.ipvs.common.service;

import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;
import java.util.UUID;

import javax.servlet.http.HttpServletResponse;
import javax.ws.rs.core.HttpHeaders;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;
import org.dom4j.Element;
import org.eclipse.jetty.continuation.Continuation;
import org.jboss.resteasy.util.Base64;
import org.jivesoftware.smack.ConnectionConfiguration;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.SmackConfiguration;
import org.jivesoftware.smack.XMPPConnection;
import org.jivesoftware.smack.XMPPException;
import org.jivesoftware.smack.filter.PacketIDFilter;
import org.jivesoftware.smack.packet.Authentication;
import org.jivesoftware.smack.packet.IQ;
import org.jivesoftware.smack.packet.IQ.Type;

import com.ipvs.appserver.AppServerAgentUtils;
import com.ipvs.assetadminservice.handler.AMConst;
import com.ipvs.client.ServiceClient;
import com.ipvs.common.IPVSScheduler;
import com.ipvs.common.IPVSTask;
import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.factory.MRResourceFactoryI;
import com.ipvs.common.factory.ServiceAgentFactoryI;
import com.ipvs.http.common.HttpConst;



public class AuthTokenCache extends IPVSTask{
	
	private Hashtable<String, UserResource> authTokenTable;
	private Hashtable<String, Continuation> httpRequestCache;
	 
	public MRLog log;
	
	private static int GC_SCHEDULE_INTERVAL = 15;
	private static long USER_KEEP_ALIVE_TIMEOUT = (30*1000); // 30 sec
	
	public AuthTokenCache(MRDB db, MRLog mrLog, XMPPI xmpp, MRResourceFactoryI resourceFactory, ServiceAgentFactoryI serviceAgentFactory){
		super(null, xmpp, mrLog, db, resourceFactory, serviceAgentFactory,"AuthCacheGCTask");
		authTokenTable = new Hashtable<String, UserResource>();
		httpRequestCache = new Hashtable<String, Continuation>();
		log = mrLog;
		
		//IPVSTask task = new AuthTokenCache(this, db, log, xmpp, resourceFactory, serviceAgentFactory);
		IPVSScheduler.getInstance().scheduleBySeconds(this, GC_SCHEDULE_INTERVAL);
	}
	
	public synchronized void reset() {
		authTokenTable = null;
		httpRequestCache = null;
	}

	public synchronized void addUserToCache(UserResource uRes) {
		if(uRes != null) {
			getAuthTokenTable().put(uRes.getUsername(), uRes);
			log.addInfo("AuthTokenCache: addUserToCache: Added User:= " + uRes.getUsername());
		}
	}
	
	public synchronized boolean deleteUserFromCache(UserResource uResource, boolean kick) throws MRException {
		if(getAuthTokenTable().containsKey(uResource.getUsername())) {
			UserResource uResFromCache = getAuthTokenTable().get(uResource.getUsername());
			
			if(!uResFromCache.getPassword().equals(uResource.getPassword())) {
				throw new MRException(HttpServletResponse.SC_UNAUTHORIZED,"Invalid User Credentials");
			}
			
			Continuation userContinuation = uResFromCache.getContinuation();
			
			log.addInfo("AuthTokenCache: deleteUserFromCache: Removed User:= " + uResource.getUsername());
			getAuthTokenTable().remove(uResFromCache.getUsername());
			
			if(userContinuation != null) {
				userContinuation.setAttribute("kick", String.valueOf(kick));
				uResFromCache.getContinuation().resume();
			} 
				
			try {
				AppServerAgentUtils.getSubscriptionManager(serviceAgentFactory).removeUserFromSubscriberPolicyCache(uResFromCache.getUserJID());
				MRXMLDBUtils.unsubscribe(MRRequest.createRequest(uResource.getUserJID()), db, log, "", "");
			} catch (Exception e) {
				//Ignore exception
			}
			uResource = null;
			uResFromCache = null;
			return true;
		}
		return false;
	}
	
	public synchronized boolean validateUser(UserResource uResource) throws Exception {
		if(uResource != null) {
			if(getAuthTokenTable().containsKey(uResource.getUsername())) {
				log.addInfo("AuthTokenCache: validateUser: Found User in Cache:= " + uResource.getUsername(),MRLog.INFO,MRLog.NOTIFY);
				return true;
			} else {
				// not in cache, read DB
				if(authenticateUser(uResource)) {
					log.addInfo("AuthTokenCache: validateUser: Adding User to Cache:= " + uResource.getUsername(),MRLog.INFO,MRLog.NOTIFY);
					String token = generateToken(uResource.getUsername());
					uResource.setToken(token);
					uResource.setLastAccessTime(System.currentTimeMillis());
					addUserToCache(uResource);
					return true;
				}
			}
		}
		return false;
	}
	
	public synchronized boolean authenticateUser(UserResource uResource) throws Exception {
		if(uResource.getUsername().isEmpty() || uResource.getPassword().isEmpty()) {
			return false;
		}
		
		XMPPConnection connection = null;

		try {
			ConnectionConfiguration config = new ConnectionConfiguration("localhost", 5222);
			connection = new XMPPConnection(config);

			connection.connect();

			Authentication packet = new Authentication();
			packet.setUsername(uResource.getUsername());
			packet.setPassword(uResource.getPassword());
			packet.setResource(uResource.getResource());
			packet.setType(Type.SET);

			PacketCollector collector = connection.createPacketCollector(new PacketIDFilter(packet.getPacketID()));
			connection.sendPacket(packet);

			IQ response = (IQ) collector.nextResult(SmackConfiguration.getPacketReplyTimeout());
			if (response != null && response.getType() != IQ.Type.ERROR) {
				log.addInfo("AuthTokenCache: authenticateUser: Successfully authenticated user: "+ uResource.getUsername(), MRLog.INFO,MRLog.NOTIFY);
				return true;
			}

			log.addInfo("AuthTokenCache: Error: " + (response != null ? response.getError() : "IQ Response is NULL" ),MRLog.ERROR, MRLog.NOTIFY);
		} catch (XMPPException e) {
			e.printStackTrace();
		} finally {
			connection.disconnect();
		}
	
		log.addInfo("AuthTokenCache: authenticateUser: Authentication failed for user: "+ uResource.getUsername(), MRLog.ERROR, MRLog.NOTIFY);
		return false;
	}
	
	public UserResource parseAuthHeader(HttpHeaders headers) throws Exception {
		String authHeader = headers.getRequestHeader(HttpHeaders.AUTHORIZATION).get(0);
		String resource = headers.getRequestHeader(HttpConst.X_BARCO_RESOURCE_HEADER).get(0);
		return parseAuthHeader(authHeader, resource);
	}

	public UserResource parseAuthHeader(String authHeader,String resource) throws Exception {
		if(authHeader == null || resource == null) {
			return null;
		}
		UserResource uRes = null;
		String basicPrefix = "Basic ";
		String authStr = "";

		if(authHeader.startsWith(basicPrefix)){
			authHeader = authHeader.substring(basicPrefix.length());
		}
		
		authStr = new String(Base64.decode(authHeader));

		String [] authTokens = authStr.split(":");

		if(authTokens != null && authTokens.length == 2) {
			String uName = authTokens[0].trim();
			String pass = authTokens[1].trim();
			
			UserResource userExists = getAuthTokenTable().get(uName);
			
			if (userExists != null) {
				boolean foundUserInCache = false;				
				
				if(userExists.getPassword().equals(pass)) {
					foundUserInCache=true;
				} else {
					throw new MRException(HttpServletResponse.SC_UNAUTHORIZED,"Invalid User Credentials");
				}
				
				if(foundUserInCache) {
					return userExists;
				}
			}
		
			
			uRes = new UserResource();
			uRes.setUsername(uName);
			uRes.setResource(resource);
			uRes.setPassword(pass);
			uRes.setUserJID(uRes.getUsername() + "@localhost/" + uRes.getResource());
			uRes.setLastAccessTime(System.currentTimeMillis());
			
			log.addInfo("AuthTokenCache: parseAuthHeader: HTTPLOGIN! UserName:="+uRes.getUsername() +" Resource:="+uRes.getResource() +" userJID:="+uRes.getUserJID(),MRLog.INFO,MRLog.NOTIFY);
			return uRes;

		}
		else {
			log.addInfo("AuthTokenCache: parseAuthHeader: Incorrect! provide username@resource and password",MRLog.INFO,MRLog.NOTIFY);
		}

		return null;
	}
	
	
	public synchronized void  addContinuationToUserResource(String uName, Continuation cc) {
		if(uName != null && getAuthTokenTable().containsKey(uName)) {
			getAuthTokenTable().get(uName).setContinuation(cc);
			getAuthTokenTable().get(uName).setLastAccessTime(System.currentTimeMillis());
			log.addInfo("AuthTokenCache: addContinuationToUserResource: Added Continuation for:= " + uName);
		}
	}

	public UserResource getUserResource(String username) {
		UserResource uRes = null;
		if(getAuthTokenTable().containsKey(username)){
			uRes = getAuthTokenTable().get(username);
			uRes.setLastAccessTime(System.currentTimeMillis());
		}
		return uRes;
	}
	
	public Hashtable<String, UserResource> getAuthTokenTable() {
		return authTokenTable;
	}

	@Override
	public void run() {
		UserResource uRes = null;
		for (String username : getAuthTokenTable().keySet()) {
			uRes = getAuthTokenTable().get(username);
			if(uRes.getContinuation() == null) {
				long currTimeinMilliSec = System.currentTimeMillis();
				if(currTimeinMilliSec - uRes.getLastAccessTime() >= USER_KEEP_ALIVE_TIMEOUT) {
					if(uRes.isPresenceAvailable()) {
						try {
							AppServerAgentUtils.getAppServerAgent(serviceAgentFactory).getResourcePresenceManager().updateResourceContactAndState(uRes.getUserJID(),true, AMConst.UNAVAILABLE, MRClientUtils.ASSET_STATE_OFFLINE, db, log,xmpp,resourceFactory,serviceAgentFactory);
						} catch (Exception e) {
							log.addInfo("AuthCacheGCTask: Failed to update the presence for user: " + uRes.getUsername());
							e.printStackTrace();
						}
					}
					log.addInfo("AuthCacheGCTask: Removing stale User from the Authorized cache: " + uRes.getUsername());
					try {
						deleteUserFromCache(uRes, false);
					} catch (MRException e) {
						//ignore
					}
				}
			}
		}
		
	}
	
	public String generateToken(String userName) throws UnsupportedEncodingException {
		//String key = UUID.randomUUID().toString().toUpperCase() + "|" + getUsername() + "|" + String.valueOf(System.currentTimeMillis());
		String key = UUID.randomUUID().toString().toUpperCase() + "|" + userName;
		return Base64.encodeBytes(key.getBytes());
	}
	
	public class UserResource {

		private String username = "";
		private String password = "";
		private String resource = "";
		private String userJID = "";
		private String token = "";
		private Continuation continuation;
		private long continuationExpiryTime = 0;
		public static final String DEFAULT_ENCODING="UTF-8";
		private long lastAccessTime;
		private boolean presenceAvailable = false;
		List<String> notificationList = null;
		List<String> gcNotificationList = null; //maintain GC list till user come back with acknowledge
		private long notificationTimestamp = 0;
		   
		
		public long getNotificationTimestamp() {
			return notificationTimestamp;
		}

		public String getUsername() {
			return username;
		}

		public void setUsername(String username) {
			this.username = username;
		}

		public String getPassword() {
			return password;
		}

		public void setPassword(String password) {
			this.password = password;
		}

		public String getResource() {
			return resource;
		}

		public void setResource(String resource) {
			this.resource = resource;
		}

		public String getToken() {
			return token;
		}

		private void setToken(String token) {
			this.token = token;
		}

		public String getUserJID() {
			return userJID;
		}

		public void setUserJID(String userJID) {
			this.userJID = userJID;
		}

		public Continuation getContinuation() {
			return continuation;
		}

		public void setContinuation(Continuation continuation) {
			this.continuation = continuation;
		}

		public long getContinuationExpiryTime() {
			return continuationExpiryTime;
		}

		public void setContinuationExpiryTime(long continuationExpiryTime) {
			this.continuationExpiryTime = continuationExpiryTime;
		}

		public long getLastAccessTime() {
			return lastAccessTime;
		}

		public void setLastAccessTime(long lastAccessTime) {
			this.lastAccessTime = lastAccessTime;
		}

		public boolean isPresenceAvailable() {
			return presenceAvailable;
		}

		public void setPresenceAvailable(boolean presenceAvail) {
			this.presenceAvailable = presenceAvail;
		}
		
		public synchronized void addNotification(String notificationXML) {
			//If presence false then ignore packet
			if(isPresenceAvailable()) {
				if(notificationList == null) {
					notificationList = new ArrayList<String>();	
				}
				
				if(gcNotificationList == null) {
					gcNotificationList = new ArrayList<String>();
				}
				log.addInfo("UserResource: addNotifications in userResource for userJID:="+userJID,MRLog.INFO,MRLog.NOTIFY);
				notificationList.add("<Notification>"+notificationXML+"</Notification>");
				
				//if continuation is there and is in suspended state then resume the continuation
			    if(continuation!=null && continuation.isSuspended()) {
			    	try {
						continuation.resume();
						log.addInfo("UserResource: Resume continuation for userJID:="+userJID,MRLog.INFO,MRLog.NOTIFY);						
					} catch (Exception e) {
						log.addInfo("Error While resuming continuation :="+e.toString(),MRLog.INFO,MRLog.NOTIFY);
					}
			    }
			}
		}
		
		public synchronized String getNotifcations() {
			if((notificationList==null || notificationList.isEmpty()) && (gcNotificationList==null || gcNotificationList.isEmpty())){
				return null;
			}
			
			StringBuffer notificationBuffer = new StringBuffer("<NotificationList>");
			
			//first add gc notifications if any thing is pending. String join will take care if no notifications in GC list 
			notificationBuffer.append(StringUtils.join(gcNotificationList, "\n"));
			notificationBuffer.append(StringUtils.join(notificationList, "\n"));
			notificationBuffer.append("</NotificationList>");

			//add notifications to GC list. GC list will clear if notification channel successfully delivered notifications to user. 
			gcNotificationList.addAll(notificationList);
			notificationTimestamp = System.currentTimeMillis();
			
			//clear from notification queue
			notificationList.clear();
			log.addInfo("UserResource: GetNotifications from userCache. userJID:="+userJID +" noofnotifcations:= "+gcNotificationList.size(),MRLog.INFO,MRLog.NOTIFY);

			return notificationBuffer.toString();
		}
		
		public synchronized void clearGCNotificationList() {
			if(gcNotificationList!=null) {
				gcNotificationList.clear();
				log.addInfo("UserResource: clear GC notification list for userJID:="+userJID,MRLog.INFO,MRLog.NOTIFY);
			}
		}

	}
	public Hashtable<String, Continuation> getHttpRequestCache() {
        return httpRequestCache;
    }
	public synchronized void addRequestToHttpCache(String clientData,Continuation cc){
        if(clientData!=null) {
            this.httpRequestCache.put(clientData,cc);
            cc.setAttribute("ClientData", clientData);
        }
        log.addInfo("Request Added to Cache:="+clientData);
    }
	public synchronized void removeRequestFromCache(Continuation cc){
        String clientData = (String)cc.getAttribute("ClientData");
        this.httpRequestCache.remove(clientData);
        log.addInfo("Request Removed from Cache:="+clientData);    
        
    }
	public synchronized void addResultAndResumeContinuation(MRRequest request, String logData, String state) throws Exception{
	    String clientData = request.getClientData();
	    if(clientData == null || clientData.isEmpty()) {
            return;
        }
        Continuation cc = this.httpRequestCache.get(clientData);
        if(cc != null && cc.isSuspended()) { //to check if request is http, asynch and suspended
            Document responseDoc = MRXMLUtils.stringToDocument(ServiceClient.generateResponseXML(request, logData));
            Element responseEl = MRXMLUtils.getElementXpath(responseDoc, "//"+ MRRequest.getRequestRootName(request.getRequestName())) ;
            String result = "";
            if (responseEl != null){
                result =responseEl.asXML();
            }else{
                result= responseDoc.asXML();
            }
            cc.setAttribute("result", result); 
            cc.setAttribute("REQUEST_STATE", state);
            cc.resume();
        }
        
    }
		
}
