package com.ipvs.nimbus;

import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.MRLog;
import com.ipvs.common.service.MRDB;

/**
 * Contract for CRUD operations on a subscriber cache on the basis of topics.
 * 
 */
public interface MRPubSubI {

	/**
	 * Get list of subscribers for entity in db for particular select format.
	 * 
	 * @param topicNID
	 *            db node representing cached entity
	 * @param selectFormat
	 *            select format i.e. basic, extended
	 * @return jid's of clients which are to be notified
	 * @throws Exception
	 */
	public Document getSubscribers(String topicNID, String selectFormat) throws Exception;

	/**
	 * Update subscription for JID.
	 * 
	 * @param jid
	 *            userJID
	 * @param nidList
	 *            list of db node representing cached entity
	 * @param subscriptionId
	 *            unique id for a userJID for same db node for different topics
	 * @param selectFormat
	 *            select format i.e. basic, extended
	 * @return
	 * @throws Exception
	 */
	public boolean updateSubscriptions(String jid, List<String> nidList, String subscriptionId, String selectFormat) throws Exception;

	/**
	 * Delete subscription by subscription id.
	 * 
	 * @param subscriptionId
	 *            unique id for a userJID for same db node for different topics
	 * @return
	 * @throws Exception
	 */
	public boolean deleteSubscriptionsBasedOnSubscriptionId(String subscriptionId) throws Exception;

	/**
	 * Subscribe user for a db node on a selectFormat.
	 * 
	 * @param topicNID
	 *            db node representing cached entity
	 * @param jid
	 *            userJID
	 * @param subscriptionId
	 *            unique id for a userJID for same db node for different topics
	 * @param selectFormat
	 *            select format i.e. basic, extended level of details
	 * @return
	 * @throws Exception
	 */
	boolean subscribeNode(String topicNID, String jid, String subscriptionId, String selectFormat) throws Exception;

	/**
	 * Remove user entries from cache.
	 * 
	 * @param topicNID
	 *            db node representing cached entity
	 * @param jid
	 *            userJID
	 * @param subscriptionId
	 *            unique id for a userJID for same db node for different topics
	 * @return
	 * @throws Exception
	 */
	boolean unsubscribeNode(String topicNID, String jid, String subscriptionId) throws Exception;
	
	/**
	 * 
	 * @param topicNID
	 * @param jid
	 * @return
	 * @throws Exception
	 */
	boolean addDocumentSubNode(String topicNID, String jid) throws Exception;
	

	/**
	 * @param message
	 */
	public void sendNotification(String message);
	
	
	/**
	 * @param message
	 * @throws Exception
	 */
	public void addNotificationToQueue(String message) throws Exception;

}