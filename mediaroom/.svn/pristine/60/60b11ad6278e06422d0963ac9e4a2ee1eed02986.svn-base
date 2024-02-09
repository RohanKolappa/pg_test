package com.ipvs.subscriptions;

import java.util.Hashtable;
import java.util.List;

import org.apache.commons.lang.StringUtils;
import org.dom4j.Document;

import com.ipvs.common.MRException;
import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPF;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRRequest;
import com.ipvs.sessionservice.impl.SessionServiceConst;

public class SubscriptionManager {
	private MRLog log;
	@SuppressWarnings("unused")
	private XMPPF xf;
	private MRDB db;
	private String userJID;
	private Hashtable<String, Hashtable<String, Hashtable<String,SubscriptionCache>>> subscriberPolicyCache = new Hashtable<String, Hashtable<String, Hashtable<String,SubscriptionCache>>>();

	public SubscriptionManager(MRLog log, XMPPF xf, MRDB db, String userJID) {
		this.log = log;
		this.xf = xf;
		this.db = db;
	}

	public Document getSubscribers(String nid) throws MRException, Exception {
		return MRXMLDBUtils.getSubscribers(MRRequest.createRequest(userJID), log, db, nid, SessionServiceConst.SELECTFORMAT_EXTENDED);

	}

	public void updateSubscriptions(MRRequest request, List<String> nidList, String subscriptionId, String topic) throws Exception {
		MRXMLDBUtils.updateSubscriptions(request, db, log, StringUtils.join(nidList, ","), subscriptionId, topic);
	}

	public synchronized  Hashtable<String,SubscriptionCache> getSubscriberPoliciesForUser(String userJID, String objectType, MRLog log) {
		if (userJID == null || objectType == null || userJID.isEmpty()) {
			log.addInfo("ObjectPolicyLib: getSubscriberPolicyForUser: Cannot find user:= " + userJID + " in cache");
			return null;
		}

		Hashtable<String,  Hashtable<String,SubscriptionCache>> userPolicyTable = subscriberPolicyCache.get(objectType);
		if (userPolicyTable == null) {
			return null;
		}

		// log.addInfo("ObjectPolicyLib: getSubscriberPoliciesForUser: user:= "
		// + userJID);
		return userPolicyTable.get(userJID);
	}

	public synchronized void addUserToSubscriberPolicyCache(String userJID, String policyObjectType, String policyXpath, String subscriptionID, String protocol, MRLog log) {
		if (userJID == null || policyObjectType == null || policyXpath == null || userJID.isEmpty() || policyObjectType.isEmpty()) {
			log.addInfo("ObjectPolicyLib: addUserToSubscriberPolicyCache: Cannot add to cache. userJID:= " + userJID + " policyObjectType:= " + policyObjectType
					+ " policyXpath:= " + policyXpath);
			return;
		}
		try {
			Hashtable<String,  Hashtable<String,SubscriptionCache>> userPolicyTable;
			 SubscriptionCache subscriptionCache = null;
			 Hashtable<String,SubscriptionCache> subscriptionCacheSet = null;
			if (subscriberPolicyCache.containsKey(policyObjectType)) {
				userPolicyTable = subscriberPolicyCache.get(policyObjectType);
				subscriptionCacheSet = userPolicyTable.get(userJID);
				if (subscriptionCacheSet == null) {
					subscriptionCacheSet = new  Hashtable<String,SubscriptionCache>();
				} else {
						if(subscriptionCacheSet.containsKey(subscriptionID)) {
							subscriptionCache = subscriptionCacheSet.get(subscriptionID);
						}	
				}
				
				if(subscriptionCache == null) {
					subscriptionCache = new SubscriptionCache();
				}

			} else {
				subscriptionCache = new SubscriptionCache();
				subscriptionCacheSet = new Hashtable<String,SubscriptionCache>();
				userPolicyTable = new Hashtable<String,Hashtable<String,SubscriptionCache>>();				
			}
			if (subscriptionCache.getPolicyXpath() != null && subscriptionCache.getPolicyXpath().equalsIgnoreCase(policyXpath)) {
				return;
			}

			// add to the list and put it in cache.
			subscriptionCache.setPolicyXpath(policyXpath);
			subscriptionCache.setSubsriptionID(subscriptionID);
            subscriptionCache.setProtocol(protocol);
            
			subscriptionCacheSet.put(subscriptionID, subscriptionCache);
			userPolicyTable.put(userJID, subscriptionCacheSet);
			subscriberPolicyCache.put(policyObjectType, userPolicyTable);
			log.addInfo("ObjectPolicyLib: addUserToSubscriberPolicyCache: Added user policy to cache. policyObjectType:= " + policyObjectType + " userJID:= " + userJID);

		} catch (Exception e) {
			e.printStackTrace();
			log.addInfo("ObjectPolicyLib: addUserToSubscriberPolicyCache: Error adding to cache.");
		}
	}

	public synchronized void removeUserFromSubscriberPolicyCache(String userJID) {
		MRLog log = MRLog.getInstance(MRLog.CONSOLE);
		if (userJID == null || userJID.isEmpty()) {
			log.addInfo("ObjectPolicyLib: removeUserFromSubscriberPolicyCache: Cannot remove from cache. userJID:= " + userJID);
			return;
		}
		try {
			for (String policyObjectType : subscriberPolicyCache.keySet()) {
				subscriberPolicyCache.get(policyObjectType).remove(userJID);
			}
			log.addInfo("ObjectPolicyLib: removeUserFromSubscriberPolicyCache: Removed user policy from cache. userJID:= " + userJID);
		} catch (Exception e) {
			e.printStackTrace();
			log.addInfo("ObjectPolicyLib: removeUserFromSubscriberPolicyCache: Error removing from cache.");
		}
	}

	public synchronized void clearSubscriberPolicyCache() {
		try {
			subscriberPolicyCache.clear();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public Hashtable<String, Hashtable<String,  Hashtable<String,SubscriptionCache>>> getSubscriberPolicyCache() {
		return subscriberPolicyCache;
	}
	
	
	public Hashtable<String,  Hashtable<String,SubscriptionCache>> getCachedSubscriptions(String policyObjectType) {
	
		
		if(subscriberPolicyCache == null) {
			return null;
		}
		
		return subscriberPolicyCache.get(policyObjectType);
		
	}
	
	public void setDestinationInformation(String objectType,String userJID, String subscriptionID,String destPortNID,String ipAddress,String dstTemplateTitle, String authScheme,String expireTime) {
		Hashtable<String,  Hashtable<String,SubscriptionCache>> subscriberCache = subscriberPolicyCache.get(objectType);
		
		if(subscriberCache == null) { return; }
		
		Hashtable<String,SubscriptionCache> subscriptionCacheSet = subscriberCache.get(userJID);
		
		if(subscriptionCacheSet == null) {
			return;
		}

		if(subscriptionCacheSet.containsKey(subscriptionID)){
			SubscriptionCache cachedSubscription = subscriptionCacheSet.get(subscriptionID);
			System.out.println("subsciptionID: ="+subscriptionID +" userJID:="+userJID +" destination:+"+destPortNID +" Ipaddress:="+ipAddress);
			cachedSubscription.setDestPortNID(destPortNID);
			cachedSubscription.setIpAddress(ipAddress);
			cachedSubscription.setDstTemplateTitle(dstTemplateTitle);
			cachedSubscription.setAuthScheme(authScheme);
			cachedSubscription.setExpiryTime(expireTime);
		}									
	}

	
	
	public static class SubscriptionCache {
		private String subsriptionID = null;
		private String policyXpath = null;
		private String destPortNID = null;
		
		private String ipAddress = null;
		private String protocol = null;
		private String dstTemplateTitle = null;
		private String authScheme = null;
		private String expiryTime=null;
		
		public String getDestPortNID() {
			return destPortNID;
		}

		public void setDestPortNID(String destPortNID) {
			this.destPortNID = destPortNID;
		}
		
		public String getIpAddress() {
			return ipAddress;
		}

		public void setIpAddress(String ipAddress) {
			this.ipAddress = ipAddress;
		}

		public String getSubsriptionID() {
			return subsriptionID;
		}

		public void setSubsriptionID(String subsriptionID) {
			this.subsriptionID = subsriptionID;
		}

		public String getPolicyXpath() {
			return policyXpath;
		}

		public void setPolicyXpath(String policyXpath) {
			this.policyXpath = policyXpath;
		}

		public String getProtocol() {
			return protocol;
		}

		public void setProtocol(String protocol) {
			this.protocol = protocol;
		}

		public String getDstTemplateTitle() {
			return dstTemplateTitle;
		}

		public void setDstTemplateTitle(String dstTemplateTitle) {
			this.dstTemplateTitle = dstTemplateTitle;
		}

		public String getAuthScheme() {
			return authScheme;
		}

		public void setAuthScheme(String authScheme) {
			this.authScheme = authScheme;
		}

		public String getExpiryTime() {
			return expiryTime;
		}

		public void setExpiryTime(String expiryTime) {
			this.expiryTime = expiryTime;
		}

	}
}

