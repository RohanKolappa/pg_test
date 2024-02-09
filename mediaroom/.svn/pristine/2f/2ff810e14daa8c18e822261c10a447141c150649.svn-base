package com.ipvs.nimbus;

import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentLinkedQueue;

import org.apache.log4j.Logger;
import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.common.db.NIDSchemeMDC;
import com.ipvs.common.service.MRDB;
import com.ipvs.common.service.MRServer;
import com.ipvs.common.service.MRXMLUtils;
import com.ipvs.common.service.UserManager;
import com.ipvs.common.MRLog;




public class MRPubSub implements MRPubSubI {

	//TODO add xmpp here.
	private Map<String, SubscriptionCollection> subsciberCache = new ConcurrentHashMap<String, SubscriptionCollection>();
	private static final String XMLNS_IPVSDBSERVICE = "ipvs:mediaroom:ipvsdbservice";
	private MRAppMessageBus messageBus;
	private DBNotifier dbNotifier;
	
	
	
	public MRPubSub(MRAppMessageBus appMessageBus) {
		this.messageBus = appMessageBus;
		this.dbNotifier = new DBNotifier();
		new Thread(this.dbNotifier).start();
	}

	@Override
	public boolean subscribeNode(String topicNID, String jid, String subscriptionId, String selectFormat) throws Exception {
		if (topicNID==null || topicNID.isEmpty()) {
			return false;
		}
		NIDSchemeMDC nodeScheme = new NIDSchemeMDC();
		nodeScheme.parse(topicNID);
	 	if ((nodeScheme.getType() == NIDSchemeMDC.UNKNOWN)) {
	 		return false;
	 	}
		return subscribe(nodeScheme, jid, subscriptionId, selectFormat);
	}
	
	@Override
	public boolean addDocumentSubNode(String topicNID, String jid) throws Exception {
		if (topicNID==null || topicNID.isEmpty()) {
			return false;
		}
		NIDSchemeMDC nodeScheme = new NIDSchemeMDC();
		nodeScheme.parse(topicNID);
		if ((nodeScheme.getType() == NIDSchemeMDC.UNKNOWN)) {
	 		return false;
	 	}
		return addDocumentSub(nodeScheme, jid);
	}

	@Override
	public boolean updateSubscriptions(String jid, List<String> nidList, String subscriptionId, String selectFormat) throws Exception {
		// clean up old subscriptions
		deleteSubscriptionsBasedOnSubscriptionId(subscriptionId);
		if (nidList != null) {
			// subscribe
			for (String topicNID : nidList) {
				if (topicNID.isEmpty()) {
					continue;
				}
				NIDSchemeMDC nodeScheme = new NIDSchemeMDC();
				nodeScheme.parse(topicNID);
				if ((nodeScheme.getType() == NIDSchemeMDC.UNKNOWN)) {
			 		continue;
			 	}
				subscribe(nodeScheme, jid, subscriptionId, selectFormat);
			}
		} 
		return true;
	}

	@Override
	public synchronized boolean deleteSubscriptionsBasedOnSubscriptionId(String subscriptionId) throws Exception {
			for (String topicNID : subsciberCache.keySet()) {
				SubscriptionCollection col = subsciberCache.get(topicNID);
				col.removeColSubJidBasedOnSubscriptionId(subscriptionId);
				col.removeDocSubJidBasedOnSubscriptionId(subscriptionId);

				if (col.isColSubEmpty()) {
					subsciberCache.remove(topicNID);
				}
			}
		return true;
	}
	
	@Override
	public synchronized boolean unsubscribeNode(String topicNID, String jid, String subscriptionId) throws Exception {
		NIDSchemeMDC node = new NIDSchemeMDC();
		try{			
			node.parse(topicNID);
		}catch(Exception ex){
			node=null;
		}
		
		return unsubscribe(node, jid, subscriptionId);
	}
	
	private synchronized boolean subscribe(NIDSchemeMDC node, String jid, String subscriptionId, String selectFormat) throws Exception {
			if (!subsciberCache.containsKey(node.getCollectionId())) {
				subsciberCache.put(node.getCollectionId(), new SubscriptionCollection(node.getCollectionId()));
			}
			subsciberCache.get(node.getCollectionId()).subscribe(node, jid, subscriptionId, selectFormat);
		return true;
	}
	
	public synchronized boolean addDocumentSub(NIDSchemeMDC node, String jid) throws Exception{       
        if (!subsciberCache.containsKey(node.getCollectionId())) {
        	subsciberCache.put(node.getCollectionId(), new SubscriptionCollection(node.getCollectionId()));
        }
        subsciberCache.get(node.getCollectionId()).subscribe(node, jid, "", "");       
    return true;
}
	
	
	private synchronized boolean unsubscribe(NIDSchemeMDC node, String jid, String subscriptionId) throws Exception {
        if (node == null) {
                // unsubscribe user if subscribed to the given subscriptionId 
            	if(subscriptionId != null && !subscriptionId.isEmpty()) {
            		for (SubscriptionCollection col : subsciberCache.values()) {
                        col.unsubscribe(null, jid, subscriptionId);
                    }
            	} else {
            		for (SubscriptionCollection col : subsciberCache.values()) {
                        col.unsubscribe(null, jid, null);
                    }
            	}
                return true;
        } else {
            	if (subsciberCache.containsKey(node.getCollectionId())) {
            		subsciberCache.get(node.getCollectionId()).unsubscribe(node, jid, null);
            	}
        }
        return true;
    }

	public Document getSubscribers(String topicNID, String selectFormat) throws Exception {
		String xmlStr = "<Subscribers/>";
		if (topicNID.isEmpty()) {
			return MRXMLUtils.stringToDocument(xmlStr);
		}
		NIDSchemeMDC node = new NIDSchemeMDC();
		node.parse(topicNID);
		StringBuffer xml = new StringBuffer();
		if (node.getNID() != null) {// For given node id
			if (!subsciberCache.isEmpty() && subsciberCache.containsKey(node.getCollectionId())) {
				xml.append("<Subscribers nid=\"" + node.getNID() + "\">");
				SubscriptionCollection col = subsciberCache.get(node.getCollectionId());
				xml.append(col.getCollJidInfo(node, selectFormat));
				if (node.getType() == NIDSchemeMDC.COLLECTION) {
					xml.append(col.getDocJidListInfo(node, selectFormat));
				} else if (node.getType() == NIDSchemeMDC.DOCUMENT) {
					xml.append(col.getDocJidInfo(node, selectFormat));
				}
				xml.append("</Subscribers>");
			}
		} else {// Node id is empty, return all subscribers
			if (!subsciberCache.isEmpty()) {
				for (SubscriptionCollection col : subsciberCache.values()) {
					String domain = "";
					if (node.getDomain().isEmpty()) {
						domain = "default";
					} else {
						domain = node.getDomain();
					}
					xml.append("<Subscribers nid=\"" + domain + "." + col.colsubnid + "\">");
					xml.append(col.getCollJidInfo(node, selectFormat));
					xml.append(col.getDocJidListInfo(node, selectFormat));
					xml.append("</Subscribers>");
				}
			}
		}
		if (!xml.toString().trim().isEmpty()) {
			xmlStr = xml.toString();
		}
		return MRXMLUtils.stringToDocument(xmlStr);
	}
	
	
	 @SuppressWarnings("unused")
	private boolean isPresent( MRPubsubSubscription subscription, MRLog log, MRDB db) throws Exception {		 
		  String userJID=subscription.getSubJid();
		  UserManager userManager = UserManager.getInstance();
		  String resource = org.jivesoftware.smack.util.StringUtils.parseResource(userJID);
		  String user = org.jivesoftware.smack.util.StringUtils.parseName(userJID);
			if(user.equals(MRServer.ADMIN)) {
			    return true;
			}
		  boolean available=  userManager.checkJID(userJID, log, db, resource);
		  if(available){
	    		return true;
	    	}else{
	    		unsubscribe(null, userJID, null);
	            return false;
	    	}
	    }
	 
	   
	public synchronized List<MRPubsubSubscription> getAutoSubscriptions(NIDSchemeMDC node) {
	        List<MRPubsubSubscription> subs = new ArrayList<MRPubsubSubscription>();
	       		if (subsciberCache.containsKey(node.getCollectionId())) {
	                    return subsciberCache.get(node.getCollectionId()).getAutoSubscriptions(node);
	                }
	      
	        return subs;
	    }
	    	
	
	public void sendNotification(String message) {
		try {
			Element messageEl = MRXMLUtils.stringToElement(message);
			String NID = MRXMLUtils.getAttributeValue(MRXMLUtils.getElement(messageEl, "updateEvent"), "nodeID", "");
			if (NID != null && !NID.isEmpty()) {
				NIDSchemeMDC node = new NIDSchemeMDC();
				node.parse(NID);
				List<MRPubsubSubscription> subscriptions = getAutoSubscriptions(node);
				if (subscriptions != null) {
					for(MRPubsubSubscription mrsub:subscriptions){
						messageBus.sendMessage(mrsub.getSubJid(),
								message, XMLNS_IPVSDBSERVICE);
					}
				}
			}
		} catch (Exception ex) {
			Logger.getLogger(getClass()).info("Error while Send Notification : "+ex.getMessage());
		}
	}
	
	public void addNotificationToQueue(String message) throws Exception {
		 dbNotifier.addNotificationToQueue(message);
	    }
	 
	
	  public static String getNotificationMessage(String action, MRLog log, MRDB db, String listName, String objectType, String NID, String childXpath, Element dataElement, String jid) throws Exception {
	           
	        try{
	                     
	            Element event = MRXMLUtils.newElement("updateEvent");
	            MRXMLUtils.addAttribute(event, "type", action);
	            MRXMLUtils.addAttribute(event, "serviceVer", MRXMLUtils.getServiceVersion(log));
	            MRXMLUtils.addAttribute(event, "JID", jid);
	            MRXMLUtils.addAttribute(event, "nodeID", NID);
	            MRXMLUtils.addAttribute(event, "objectType", objectType);
	            MRXMLUtils.addAttribute(event, "listName", listName);
	            MRXMLUtils.addAttribute(event, "revision", "");
	            if(childXpath!=null){
		            try {
		            	MRXMLUtils.addAttribute(event, "childXPath", URLEncoder.encode(childXpath, "UTF-8"));
			        } catch (Exception ex) {
			            MRLog.getLogger().error("error childXPath encode  " + childXpath, ex);
			        }    
	            }
	   	        
	            String  data=  "<x xmlns='"+XMLNS_IPVSDBSERVICE+"'>" + 
	            		 MRXMLUtils.elementToString(event) + "</x>";
	                  	            
	            return data;
	        }catch(Exception ex){
	            throw ex;
	        }
	    }   
	    
	 
	/**
	 * This class is responsible for subscribercache
	 * 
	 */
	static class SubscriptionCollection {

		private Hashtable<String, Hashtable<String, Hashtable<String, String>>> docs = null;
		private Hashtable<String, Hashtable<String, String>> colSubJids = null;
		private String colsubnid = "";
		private String dummy = "test";

		SubscriptionCollection(String collectionID) {
			colsubnid = collectionID;
			init();
		}

		synchronized void init() {
			docs = new Hashtable<String, Hashtable<String, Hashtable<String, String>>>();
			colSubJids = new Hashtable<String, Hashtable<String, String>>();
		}

		// ////////////////// colSubJids ////////////////////
		private boolean existColSubJid(String jid) {
			if (colSubJids.containsKey(jid)) {
				return true;
			}
			return false;
		}

		private void addColSubJid(String jid, String subId, String selectFormat) {
			if (!colSubJids.containsKey(jid)) {
				colSubJids.put(jid, new Hashtable<String, String>());
			}
			if (!subId.isEmpty()) {
				colSubJids.get(jid).put(subId, selectFormat);
			}

		}

		private void removeColSubJid(String jid) {
			if (colSubJids.containsKey(jid)) {
				colSubJids.remove(jid);
			}
		}

		private void addDocSubJid(String docuri, String jid, String subId, String selectFormat) {
			if (!docs.containsKey(docuri)) {
				docs.put(docuri, new Hashtable<String, Hashtable<String, String>>());
			}
			if (!docs.get(docuri).containsKey(jid)) {
				docs.get(docuri).put(jid, new Hashtable<String, String>());
			}
			if (!subId.isEmpty() && !selectFormat.isEmpty()) {
				docs.get(docuri).get(jid).put(subId, selectFormat);
			}
		}

		private void removeDocSubJid(String docuri, String jid) {
			if (docs.containsKey(docuri)) {
				if (docs.get(docuri).containsKey(jid)) {
					docs.get(docuri).remove(jid);
				}
				if (docs.get(docuri).isEmpty()) {
					docs.remove(docuri);
				}
			}
		}

		private void removeAllDocsSubJID(String jid) {
				if (!docs.isEmpty()) {
					Vector<String> removeList = new Vector<String>();
					for (String key : docs.keySet()) {
						Hashtable<String, Hashtable<String, String>> list = docs.get(key);
						if (list.containsKey(jid)) {
							list.remove(jid);
						}
						// remove the doc save memory
						if (list.isEmpty()) {
							removeList.add(key);
						}
					}
					// remove empty ones
					for (int index = 0; index < removeList.size(); index++) {
						String key = removeList.get(index);
						if (docs.containsKey(key)) {
							docs.remove(key);
						}
					}

				}
		}

		private Hashtable<String, String> removeSubscriptionId(Hashtable<String, String> subtopic, String subId) {
			if (!subtopic.isEmpty()) {
				if (subtopic.containsKey(subId)) {
					subtopic.remove(subId);
				}
				return subtopic;
			}
			return null;
		}

		public synchronized void addColSub(String jid, String subId, String selectFormat) {
			addColSubJid(jid, subId, selectFormat);
		}

		public synchronized void addDocSub(String docuri, String jid, String subId, String selectFormat) {
			addDocSubJid(docuri, jid, subId, selectFormat);
		}

		public synchronized void subscribe(NIDSchemeMDC node, String jidStr, String subscriptionId, String selectFormat) throws Exception {
				if (jidStr == null || jidStr.equals("")) {
					return;
				}
				jidStr = jidStr.trim();

				if (node.getType() == NIDSchemeMDC.COLLECTION) {
					// if not subscribed than add
					addColSubJid(jidStr, subscriptionId, selectFormat);
				} else {
					addDocSubJid(node.getDocUri(), jidStr, subscriptionId, selectFormat);
				}
		}

		public void removeColSubJidBasedOnSubscriptionId(String subscriptionId) throws Exception {
				if (!colSubJids.isEmpty()) {
					Vector<String> removeList = new Vector<String>();
					for (String jid : colSubJids.keySet()) {
						if (!colSubJids.get(jid).isEmpty()) {
							Hashtable<String, String> subtopic = removeSubscriptionId(colSubJids.get(jid), subscriptionId);
							if (subtopic != null && subtopic.isEmpty()) {
								removeList.add(jid);
							}
						}

					}
					if (!removeList.isEmpty()) {
						for (int index = 0; index < removeList.size(); index++) {
							String key = removeList.get(index);
							removeColSubJid(key);
						}
					}
				}
		}

		public void removeDocSubJidBasedOnSubscriptionId(String subscriptionId) throws Exception {
				if (!docs.isEmpty()) {
					Hashtable<String, Hashtable<String, String>> removeList = new Hashtable<String, Hashtable<String, String>>();
					for (String docuri : docs.keySet()) {
						if (!docs.get(docuri).isEmpty()) {
							for (String jid : docs.get(docuri).keySet()) {
								Hashtable<String, String> subtopic = removeSubscriptionId(docs.get(docuri).get(jid), subscriptionId);
								if (subtopic != null && subtopic.isEmpty()) {
									if (!removeList.containsKey(docuri)) {
										removeList.put(docuri, new Hashtable<String, String>());
									}
									removeList.get(docuri).put(jid, dummy);
								}
							}
						}
					}
					if (!removeList.isEmpty()) {
						for (String uri : removeList.keySet()) {
							if (!removeList.get(uri).isEmpty()) {
								for (String jid : removeList.get(uri).keySet()) {
									removeDocSubJid(uri, jid);
								}
							}
						}
					}
				}
		}
		
		 public synchronized List<MRPubsubSubscription> getAutoSubscriptions(NIDSchemeMDC node) {
	          List<MRPubsubSubscription> subs = new ArrayList<MRPubsubSubscription>();
	          try {
	              String collectionNodeID = node.getCollectionNodeID();
	              if (!colSubJids.isEmpty()) {
	                  for (Iterator<String> i = colSubJids.keySet().iterator(); i.hasNext();) {
	                      String jid = i.next();
	                      	MRPubsubSubscription subscription = new MRPubsubSubscription();
	                      	subscription.setSubJid(jid);
	                      	subscription.setSubColNid(collectionNodeID);
	                      	subscription.setSubNid(collectionNodeID);
	                      	subs.add(subscription);
	                  }
	              }
	              if (node.getType() != NIDSchemeMDC.COLLECTION) {
	                  String docsubnid = node.getDocNodeID();
	                  if (!docs.isEmpty() && docs.get(node.getDocUri()) != null) {
	                      for (Iterator<String> i = docs.get(node.getDocUri()).keySet().iterator(); i.hasNext();) {
	                          String jid = i.next();
	                          	MRPubsubSubscription subscription = new MRPubsubSubscription();
	                          	subscription.setSubJid(jid);
	                          	subscription.setSubColNid(collectionNodeID);
	                          	subscription.setSubNid(docsubnid);
	                          	subs.add(subscription);
	                      }
	                  }
	              }
	          } catch (Exception ex) {
	             
	          }
	          return subs;
	      }

		public synchronized boolean isColSubEmpty() {
			return (colSubJids.isEmpty() && docs.isEmpty());
		}

		public synchronized void unsubscribe(NIDSchemeMDC node, String jid, String subscriptionId) throws Exception {
				if (node == null) {
					if (subscriptionId != null) {
						// remove user based on subscriptionId
						removeColSubJidBasedOnSubscriptionId(subscriptionId);
						removeDocSubJidBasedOnSubscriptionId(subscriptionId);
					} else {
						// remove user
						removeColSubJid(jid);
						removeAllDocsSubJID(jid);
					}
					return;
				} else {
					if (node.getType() == NIDSchemeMDC.COLLECTION) {
						removeColSubJid(jid);
					} else {
						if (existColSubJid(jid)) {
							// remove all doc + user
							removeAllDocsSubJID(jid);
						} else {
							removeDocSubJid(node.getDocUri(), jid);
						}
					}
				}
		}

		public String getCollJidInfo(NIDSchemeMDC node, String selectFormat) {
			StringBuffer xml = new StringBuffer();
			if (!colSubJids.isEmpty()) {
				for (String jid : colSubJids.keySet()) {
					Hashtable<String, String> subtopic = colSubJids.get(jid);
					xml.append(fillJIDInfoForSubscribers(jid, subtopic, selectFormat));
				}
			}
			return xml.toString();
		}

		public String getDocJidListInfo(NIDSchemeMDC node, String selectFormat) {
			StringBuffer xml = new StringBuffer();
			Hashtable<String, Hashtable<String, String>> doc = new Hashtable<String, Hashtable<String, String>>();
			for (String docuri : docs.keySet()) {
				StringBuffer jidInfo = new StringBuffer();
				doc = docs.get(docuri);
				if (!doc.isEmpty()) {
					for (String jid : doc.keySet()) {
						Hashtable<String, String> subtopic = doc.get(jid);
						jidInfo.append(fillJIDInfoForSubscribers(jid, subtopic, selectFormat));
					}
				}
				xml.append(createSubscriberWrapperForJidInfo(node.getDomain(), colsubnid, docuri, jidInfo.toString()));
			}
			return xml.toString();
		}

		public String getDocJidInfo(NIDSchemeMDC node, String selectFormat) {
			StringBuffer xml = new StringBuffer();
			Hashtable<String, Hashtable<String, String>> doc = new Hashtable<String, Hashtable<String, String>>();
			StringBuffer jidInfo = new StringBuffer();

			if (docs.containsKey(node.getDocUri())) {
				doc = docs.get(node.getDocUri());
				if (!doc.isEmpty()) {
					for (String jid : doc.keySet()) {
						Hashtable<String, String> subtopic = doc.get(jid);
						jidInfo.append(fillJIDInfoForSubscribers(jid, subtopic, selectFormat));
					}
				}
				xml.append(jidInfo.toString());
			}
			return xml.toString();
		}

		public String createSubscriberWrapperForJidInfo(String domain, String collection, String docuri, String jidInfo) {
			if (!docuri.isEmpty()) {
				docuri = "." + docuri;
			}
			if (jidInfo != null && !jidInfo.trim().equals("")) {
				return "<Subscribers nid=\"" + domain + "." + collection + docuri + "\">" + jidInfo + "</Subscribers>";
			} else {
				return "";
			}
		}

		public String fillJIDInfoForSubscribers(String jid, Hashtable<String, String> subtopic, String selectFormat) {
			StringBuffer xml = new StringBuffer();
			xml.append("<JIDInfo jid=\"" + jid + "\">");
			if (selectFormat.equals("EXTENDED")) {
				if (!subtopic.isEmpty()) {
					for (String subId : subtopic.keySet()) {
						if (!subId.trim().equals("")) {
							xml.append("<Topic subscriptionId=\"" + subId.trim() + "\">");
							if (!subtopic.get(subId).trim().equals("")) {
								xml.append(subtopic.get(subId).trim());
							}
							xml.append("</Topic>");
						}
					}
				}
			}
			xml.append("</JIDInfo>");
			return xml.toString();
		}
	}


	
	public class DBNotifier implements Runnable{

		private static final long holdTimeMillis = 1000;
		private ConcurrentLinkedQueue<String> queue = new ConcurrentLinkedQueue<String>();

		@Override
		public void run() {
			
			Logger.getLogger(getClass()).info(":run started ...");
			try {
				while (true) {
					try{
						if (!queue.isEmpty()) {
							String message = queue.poll();
							if (message != null && !message.isEmpty()) {
								sendNotification(message);
							}
						} else {
							waitForNotification();
						}
					}catch(Exception ex){
						Logger.getLogger(getClass()).info("Error while operating on queue : "+ex.getMessage());
					}
				}
					
			} catch (Exception e) {
				Logger.getLogger(getClass()).info("Error while Starting notifier : "+e.getMessage());
			}
			Logger.getLogger(getClass()).info(":run exiting ...");
		}

		private void waitForNotification() {
			try {
				Thread.sleep(holdTimeMillis);
			} catch (InterruptedException e) {
			}
		}

		public void addNotificationToQueue(String message)
				throws Exception {
			queue.add(message);
		}
	}
	
}
