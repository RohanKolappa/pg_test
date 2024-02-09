package com.ipvsserv.nxd.mdc.sub;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;
import java.util.Vector;

import org.dom4j.Document;
import org.dom4j.Element;
import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.mdc.RequestIQConst;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.SchemaConst;
import com.ipvsserv.nxd.mdc.notif.NotificationUtils;
import com.ipvsserv.nxd.mdc.notif.ObjectRevision;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;
import com.ipvsserv.xmpp.mdc.ComponentEngine;
import com.ipvsserv.xmpp.mdc.DBApi;
import com.ipvsserv.xmpp.mdc.NotificationsRouter;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class SubscriptionsHashMDC implements SubscriptionsMDCI {
    private static final String DOC_SUB_JID = "docsubjid";
    private static final String COL_SUB_JID = "colsubjid";
    private Hashtable<String, ColSub> colSubs = null;
    private static final String DEFAULT_DOC_ROOT = "ipvs";
    
    private boolean saveDB = false;
    private long saveDBLast = 0;
    private static boolean enableLog = false;
    private static boolean enableLogSummary = false;
    private boolean isEnabled = false;
    private boolean isSaveSubscriptionsToDB;
    private int saveSubscriptionsInterval;
    private DBApi dbApi = null;
    private ObjectRevision objectRevision = ObjectRevision.getInstance();
    
    public SubscriptionsHashMDC() {
        saveDBLast = System.currentTimeMillis();
        //isEnabled = IPVSGlobals.getInstance().isEnableSubscriptions();
        enableLog = IpvsServGlobals.getBooleanProperty("logs.enable.subscription");
        enableLogSummary = IpvsServGlobals.getBooleanProperty("logs.enable.notificationsummary");
        init();
    }
    
    private synchronized void init() {
        //SubscriptionsHashMDC.debugEnable = IPVSGlobals.getInstance().isEnableDebugSubLogs();
        colSubs = new Hashtable<String, ColSub>();
    }
    
    // //////////// document /////////////////////
    public synchronized boolean addDocumentSub(NIDSchemeMDC node, String jid, RequestMDCParams params) throws Exception{
        if (!isEnabled ) {
            return true;
        }
        try {
            if (!colSubs.containsKey(node.getCollectionId())) {
                colSubs.put(node.getCollectionId(), new ColSub(node.getCollectionId()));
            }
            colSubs.get(node.getCollectionId()).subscribe(node, jid, "", "", params);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return true;
    }
    
    public synchronized boolean deleteDocumentSub(NIDSchemeMDC node, RequestMDCParams params) {
        if (!isEnabled) {
            return true;
        }
        try {
            if (colSubs.containsKey(node.getCollectionId())) {
                colSubs.get(node.getCollectionId()).unsubscribe(node, "", null, params);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return true;
    }
    
    public synchronized boolean subscribe(NIDSchemeMDC node, String jid, String subscriptionId, String topic, RequestMDCParams params) throws Exception{
        if (!isEnabled) {
            return true;
        }
        try {
            if (!colSubs.containsKey(node.getCollectionId())) {
                colSubs.put(node.getCollectionId(), new ColSub(node.getCollectionId()));
            }
            colSubs.get(node.getCollectionId()).subscribe(node, jid, subscriptionId, topic, params);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return true;
    }
    
    public synchronized boolean deleteSubscriptionsBasedOnSubscriptionId(String subscriptionId) throws Exception{
        if (!isEnabled) {
            return true;
        }
        try {
        	for (Iterator<String> iterator = colSubs.keySet().iterator(); iterator.hasNext();) {
                String nid = iterator.next();
        		
        		ColSub col = colSubs.get(nid);
                col.removeColSubJidBasedOnSubscriptionId(subscriptionId);
        		col.removeDocSubJidBasedOnSubscriptionId(subscriptionId);
        		
        		if(col.isColSubEmpty()) {
        			colSubs.remove(nid);
        		}
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return true;
    }
    
    
    public synchronized boolean unsubscribe(NIDSchemeMDC node, String jid, String subscriptionId, RequestMDCParams params) {
        if (!isEnabled) {
            return true;
        }
        if (node == null) {
            try {
                // unsubscribe user if subcribed to the given subscriptionId 
            	if(subscriptionId != null && !subscriptionId.isEmpty()) {
            		for (Iterator<ColSub> iterator = colSubs.values().iterator(); iterator.hasNext();) {
                        ColSub col = iterator.next();
                        col.unsubscribe(null, jid, subscriptionId, params);
                    }
            	} else {
            		for (Iterator<ColSub> iterator = colSubs.values().iterator(); iterator.hasNext();) {
                        ColSub col = iterator.next();
                        col.unsubscribe(null, jid, null, params);
                    }
            	}
                return true;
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        } else {
        	try {
            	if (colSubs.containsKey(node.getCollectionId())) {
            		colSubs.get(node.getCollectionId()).unsubscribe(node, jid, null, params);
            	}
        	} catch (Exception ex) {
        		MRLog.error(ex);
        	}
        }
        return true;
    }
    
    public synchronized String getSubscribers(NIDSchemeMDC node, RequestMDCParams params, String selectFormat) {
    	StringBuffer xml = new StringBuffer();
        if (!isEnabled) {
            return "";
        }
        
        try {
        	if(node.getNID() != null) {// For given node id
        		if(!colSubs.isEmpty() && colSubs.containsKey(node.getCollectionId())) {
        			xml.append("<Subscribers nid=\"" + node.getNID()+ "\">");
        			ColSub col = colSubs.get(node.getCollectionId());
        			
        			xml.append(col.getCollJidInfo(node, params, selectFormat));
        			
        			if(node.getType() == NIDSchemeMDC.COLLECTION) {
        				xml.append(col.getDocJidListInfo(node, params, selectFormat));
        			}else if(node.getType() == NIDSchemeMDC.DOCUMENT) {
        				xml.append(col.getDocJidInfo(node, params, selectFormat));
        			}
        			xml.append("</Subscribers>");
        		}
        		
        	}else {// Node id is empty, return all subscribers
        		if(!colSubs.isEmpty()) {
        			for (ColSub col : colSubs.values()) {
        				if(node.getDomain().isEmpty()) 
        					node.setDomain("default");
        				xml.append("<Subscribers nid=\"" + node.getDomain() + "." + col.colsubnid + "\">");
        				xml.append(col.getCollJidInfo(node, params, selectFormat));
        				xml.append(col.getDocJidListInfo(node, params, selectFormat));
        				xml.append("</Subscribers>");
        			}
        		}
        	}
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return xml.toString();
    }
    
    public synchronized List<PubsubSubscription> getSubscriptions(NIDSchemeMDC node, RequestMDCParams params) {
    	return getAutoSubscriptions(node, params);
    }
    
    public synchronized List<PubsubSubscription> getAutoSubscriptions(NIDSchemeMDC node, RequestMDCParams params) {
        //this is synchronized. even though it is private, called from public non sync function
        List<PubsubSubscription> subs = new ArrayList<PubsubSubscription>();
        if (!isEnabled) {
            return subs;
        }
        
        try {
       		if (colSubs.containsKey(node.getCollectionId())) {
                    return colSubs.get(node.getCollectionId()).getAutoSubscriptions(node, params);
                }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return subs;
    }
    
    // ///////////////////////////////////
    public synchronized void loadSubscriptions(RequestMDCParams params) {
        if (!isEnabled) {
            return;
        }
        try {
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.setDomain(params.getDomainName());
            colSubs = new Hashtable<String, ColSub>();
            String ID = "SubscriptionsHashMDC loadDoc";
            String xml = getDBApi().getDocument(ID, params.getDomainName(), SchemaConst.DOC_TYPE_SCHEMA, node.getDocIdSub(), node.getCollectionNameSub());
            if (xml != null && !xml.trim().equals("")) {
                Document doc = Dom4jUtils.stringToDocument(xml);
                List<?> cols = Dom4jUtils.getList(doc.getRootElement());
                for (Iterator<?> i = cols.iterator(); i.hasNext();) {
                    Element colEL = (Element) i.next();
                    ColSub col = new ColSub(colEL.getName());
                    if (colSubs.containsKey(colEL.getName())) {
                        col = colSubs.get(colEL.getName());
                    }
                    List<?> colSubs = Dom4jUtils.getList(colEL);
                    for (Iterator<?> k = colSubs.iterator(); k.hasNext();) {
                        Element childEL = (Element) k.next();
                        if (childEL.getName().equalsIgnoreCase(SubscriptionsHashMDC.COL_SUB_JID)) {
                            String jid = Dom4jUtils.value(childEL, "").trim();
                            col.addColSub(jid, "", "");
                        } else {
                            List<?> docSubs = Dom4jUtils.getList(childEL);
                            for (Iterator<?> j = docSubs.iterator(); j.hasNext();) {
                                Element docSubEL = (Element) j.next();
                                if (docSubEL.getName().equalsIgnoreCase(SubscriptionsHashMDC.DOC_SUB_JID)) {
                                    String jid = Dom4jUtils.value(docSubEL, "").trim();
                                    col.addDocSub(childEL.getName(), jid, "", "");
                                }
                            }
                        }
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    // /////////////////////////////////// send notification
    
    private String toXml() {
        //called from sync block 
        StringBuffer buf = new StringBuffer(200);
        buf.append("<" + SubscriptionsHashMDC.DEFAULT_DOC_ROOT + ">");
        try {
            for (Iterator<ColSub> iterator = colSubs.values().iterator(); iterator.hasNext();) {
                ColSub col = iterator.next();
                buf.append(col.toXML());
            }
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        buf.append("</" + SubscriptionsHashMDC.DEFAULT_DOC_ROOT + ">");
        return buf.toString();
    }
    
    public synchronized void saveSubscriptions(RequestMDCParams params) {
        if (!isEnabled || !isSaveSubscriptionsToDB) {
            return;
        }
        if (!saveDB) { // test saving subscriptions{
            return;
        }
        
        if (System.currentTimeMillis() - saveDBLast < saveSubscriptionsInterval) {
            return;
        }
        try {
            saveDBLast = System.currentTimeMillis();
            String ID = "SubscriptionsHashMDC saveDoc";
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.setDomain(params.getDomainName());
            String xml = toXml();
            getDBApi().addDocument(ID, params.getDomainName(), SchemaConst.DOC_TYPE_SCHEMA, node.getDocIdSub(), node.getCollectionNameSub(), xml);//saveSubscriptions //(ID, params.getDocIdSub(), params.getCollectionNameSub(), xml, params);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            saveDB = false;
            saveDBLast = System.currentTimeMillis();
        }
    }
    
    public synchronized boolean ensureSubscriptionDoc(String ID, RequestMDCParams params, String xmlSchema) {
        /*if (!isEnabled) {
            return true;
        }
        try {
            try {
                String xml = toXml();
                try {
                    if (getDBApi().isDocumentExists(ID, params.getDomainName(), SchemaConst.DOC_TYPE_SCHEMA, params.getDocIdSub(), params.getCollectionNameSub())) {
                        getDBApi().deleteDocument(ID, params.getDomainName(), SchemaConst.DOC_TYPE_SCHEMA, params.getDocIdSub(), params.getCollectionNameSub());
                    }
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
                getDBApi().addDocument(ID, params.getDomainName(), SchemaConst.DOC_TYPE_SCHEMA, params.getDocIdSub(), params.getCollectionNameSub(), xml);
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        saveDB = false;*/
        return true;
    }
    
    public List<PubsubSubscription> getDeleteSubscription(NIDSchemeMDC node, RequestMDCParams params) {
        //calls sync  getSubscritions, hence it is not sync
        return getAutoSubscriptions(node, params);
    }
    
    public void sendDeleteDomainNotification(ComponentEngine component, NIDSchemeMDC node, List<PubsubSubscription> subscriptions, IQ iq, String nodeID, RequestMDCParams params, NotificationsRouter notificationsRouter) {
        if (!isEnabled) {
            return;
        }
        try {
            //todo: add subscription to CreateDomain DeleteDomain
            //practically no user will be listening.. may be some components can subscribe
            
            String to = iq.getTo().toString();
            // send notification to all users
            if (enableLogSummary) {
                MRLog.debug(" ============ sendDeleteDomainNotification hash " + node + " = " + subscriptions.size());
            }
            String revision = objectRevision.getNextRevision(node, "delete", nodeID);
            for (Iterator<PubsubSubscription> i = subscriptions.iterator(); i.hasNext();) {
                PubsubSubscription subscription = i.next();
                if(component.getPresenceHandler().isUserPresenceAvailable(subscription.getBareJID())){
                    Message message = NotificationUtils.getNotificationPacket("deleteDomain", iq.getID(), nodeID, params, "<delete/>", revision);//NotificationUtils.getDeleteNotificationPacket(nodeID, revision);
                    // MRLog.printlnTime(
                    // "-------- delete NOTIFICATION start-------------");
                    NotificationUtils.sendNotification(message, subscription, notificationsRouter, to);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
   
    public void sendCreateDomainNotification(ComponentEngine component, NIDSchemeMDC node, List<PubsubSubscription> subscriptions, IQ iq, String nodeID, RequestMDCParams params, NotificationsRouter notificationsRouter) {
        if (!isEnabled) {
            return;
        }
        try {
            //todo: add subscription to CreateDomain DeleteDomain
            //practically no user will be listening.. may be some components can subscribe
            
            String to = iq.getTo().toString();
            // send notification to all users
            if (enableLogSummary) {
                MRLog.debug(" ============ sendCreateDomainNotification hash " + node + " = " + subscriptions.size());
            }
            String revision = objectRevision.getNextRevision(node, "delete", nodeID);
            for (Iterator<PubsubSubscription> i = subscriptions.iterator(); i.hasNext();) {
                PubsubSubscription subscription = i.next();
                if(component.getPresenceHandler().isUserPresenceAvailable(subscription.getBareJID())){
                    Message message = NotificationUtils.getNotificationPacket("createDomain", iq.getID(), nodeID, params, "<delete/>", revision);//NotificationUtils.getDeleteNotificationPacket(nodeID, revision);
                    // MRLog.printlnTime(
                    // "-------- delete NOTIFICATION start-------------");
                    NotificationUtils.sendNotification(message, subscription, notificationsRouter, to);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private boolean isPresent( PubsubSubscription subscription, ComponentEngine component, RequestMDCParams params) {
    	if(component.getPresenceHandler().isUserPresenceAvailable(subscription.getBareJID())){
    		return true;
    	}else{
        	unsubscribe(null, subscription.getBareJID(), null, params);
            if (enableLogSummary) {
                MRLog.debug(" ============ User unavailable '" + subscription.getBareJID() +"'. remove subscriptions.");
            }
            return false;
    	}
    }
    
    public void sendDeleteNotification(ComponentEngine component, NIDSchemeMDC node, List<PubsubSubscription> subscriptions, IQ iq, String nodeID, RequestMDCParams params, NotificationsRouter notificationsRouter) {
        if (!isEnabled) {
            return;
        }
        try {
            String to = iq.getTo().toString();
            // send notification to all users
            if (enableLogSummary) {
                MRLog.debug(" ============ sendDeleteNotification hash " + node + " = " + subscriptions.size());
            }
            String revision = objectRevision.getNextRevision(node, "delete", nodeID);
            for (Iterator<PubsubSubscription> i = subscriptions.iterator(); i.hasNext();) {
                PubsubSubscription subscription = i.next();
                //if(component.getPresenceHandler().isUserPresenceAvailable(subscription.getBareJID())){
                if(isPresent(subscription, component, params)){
                    Message message = NotificationUtils.getNotificationPacket("delete", iq.getID(), nodeID, params, "<delete/>", revision);//NotificationUtils.getDeleteNotificationPacket(nodeID, revision);
                    // MRLog.printlnTime(
                    // "-------- delete NOTIFICATION start-------------");
                    NotificationUtils.sendNotification(message, subscription, notificationsRouter, to);
                }else{
                    if (enableLogSummary) {
                        MRLog.debug(" ============ ABORT sendDeleteNotification hash " + node + " User unavailable '" + subscription.getBareJID() +"'");
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    public void sendAddNotification(ComponentEngine component, NIDSchemeMDC node, IQ iq, String nodeID, String xml, RequestMDCParams params, NotificationsRouter notificationsRouter) {
        if (!isEnabled) {
            return;
        }
        try {
            String to = iq.getTo().toString();
            // list of subNode (current/parent node) and subJid
            //calls sync  getSubscritions, hence it is not sync

            List<PubsubSubscription> subscriptions = getAutoSubscriptions(node, params);

            String revision = objectRevision.getNextRevision(node, "add", nodeID);
            
            // send notification to all users
            if (enableLogSummary) {
                MRLog.debug(" ============ sendAddNotification hash " + nodeID + " = " + subscriptions.size());
            }
            for (Iterator<PubsubSubscription> i = subscriptions.iterator(); i.hasNext();) {
                PubsubSubscription subscription = i.next();
                //if(component.getPresenceHandler().isUserPresenceAvailable(subscription.getBareJID())){
                if(isPresent(subscription, component, params)){
                    Message message = NotificationUtils.getNotificationPacket("add", iq.getID(), nodeID, params, xml, revision);
                    //NotificationUtils.getAddUpdateNotificationPacket(nodeID, notificationEL, update, revision);
                    NotificationUtils.sendNotification(message, subscription, notificationsRouter, to);
                }else{
                    if (enableLogSummary) {
                        MRLog.debug(" ============ ABORT sendAddNotification hash " + node + " User unavailable '" + subscription.getBareJID() +"'");
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }    
    public void sendUpdateNotification(ComponentEngine component, NIDSchemeMDC node, IQ iq, String nodeID, String xml, RequestMDCParams params, NotificationsRouter notificationsRouter) {
        if (!isEnabled) {
            return;
        }
        try {
            String to = iq.getTo().toString();
            // list of subNode (current/parent node) and subJid
            //calls sync  getSubscritions, hence it is not sync
            List<PubsubSubscription> subscriptions = getAutoSubscriptions(node, params);
            String revision = objectRevision.getNextRevision(node, "update", nodeID);
            
            // send notification to all users
            if (enableLogSummary) {
                MRLog.debug(" ============ sendReplaceNotification hash " + nodeID + " = " + subscriptions.size());
            }
            for (Iterator<PubsubSubscription> i = subscriptions.iterator(); i.hasNext();) {
                PubsubSubscription subscription = i.next();
                //if(component.getPresenceHandler().isUserPresenceAvailable(subscription.getBareJID())){
                if(isPresent(subscription, component, params)){
                    Message message = NotificationUtils.getNotificationPacket("replace", iq.getID(), nodeID, params, xml, revision);
                    //NotificationUtils.getAddUpdateNotificationPacket(nodeID, notificationEL, update, revision);
                    NotificationUtils.sendNotification(message, subscription, notificationsRouter, to);
                }else{
                    if (enableLogSummary) {
                        MRLog.debug(" ============ ABORT sendReplaceNotification hash " + node + " User unavailable '" + subscription.getBareJID() +"'");
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    // /////////////////////////// colSub class start ///////////////
    static class ColSub {
        private Hashtable<String, Hashtable<String, Hashtable<String, String>>> docs = null;
        private Hashtable<String, Hashtable<String, String>> colSubJids = null;
        private String colsubnid = "";
        private String dummy = "test";
        
        ColSub(String collectionID) {
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
        
        private void addColSubJid(String jid, String subId, String topic) {
            if (!colSubJids.containsKey(jid)) {
                colSubJids.put(jid, new Hashtable<String, String>());
            }
            if(!subId.isEmpty())
            	colSubJids.get(jid).put(subId, topic);
                
                /*if (debugEnable) {
                    MRLog.debug("SUB:" + colsubnid + " COL:ADD_JID:" + jid);
                }*/
            
            
        }
        
        private void removeColSubJid(String jid) {
            if (colSubJids.containsKey(jid)) {
                colSubJids.remove(jid);
                /*if (debugEnable) {
                    MRLog.debug("SUB:" + colsubnid + " COL:REMOVE_JID:" + jid);
                }*/
            }
        }
        
        // ////////////////// colSubJids ////////////////////
        // ////////////////// docSubJids ////////////////////
        private void addDocSubJid(String docuri, String jid, String subId, String topic) {
            if (!docs.containsKey(docuri)) {
                docs.put(docuri, new Hashtable<String, Hashtable<String, String>>());
                /*if (debugEnable) {
                    MRLog.debug("SUB:" + colsubnid + " DOC:ADD_DOC: new j:" + jid + " d: " + docuri);
                }*/
            }
            if (!docs.get(docuri).containsKey(jid)) {
                docs.get(docuri).put(jid, new Hashtable<String, String>());
            }    
            if(!subId.isEmpty() && !topic.isEmpty())
            	docs.get(docuri).get(jid).put(subId, topic);
                
                /*if (debugEnable) {
                    MRLog.debug("SUB:" + colsubnid + " DOC:ADD_JID j:" + jid + " d: " + docuri);
                }*/
            
        }
        
        private void removeDocSubJid(String docuri, String jid) {
            if (docs.containsKey(docuri)) {
                if (docs.get(docuri).containsKey(jid)) {
                    docs.get(docuri).remove(jid);
                    /*if (debugEnable) {
                        MRLog.debug("SUB:" + colsubnid + " DOC:REMOVE_JID: j:" + jid + " d: " + docuri);
                    }*/
                }
                if (docs.get(docuri).isEmpty()) {
                    docs.remove(docuri);
                    /*if (debugEnable) {
                        MRLog.debug("SUB:" + colsubnid + " DOC:REMOVE_DOC: j:" + jid + " d: " + docuri);
                    }*/
                }
            }
        }
        
        private void removeDoc(String docuri) {
            if (docs.containsKey(docuri)) {
                docs.remove(docuri);
                /*if (debugEnable) {
                    MRLog.debug("SUB:" + colsubnid + " DOC:REMOVE_DOC:  d: " + docuri);
                }*/
            }
        }
        
        private void removeAllDocsSubJID(String jid) {
            try {
                int i = 0;
                int k = 0;
                StringBuffer buf = new StringBuffer();
                if (!docs.isEmpty()) {
                    
                    Vector<String> removeList = new Vector<String>();
                    for (Iterator<String> index = docs.keySet().iterator(); index.hasNext();) {
                        String key = index.next();
                        Hashtable<String, Hashtable<String, String>> list = docs.get(key);
                        if (list.containsKey(jid)) {
                            list.remove(jid);
                            i++;
                        }
                        // remove the doc save memory
                        if (list.isEmpty()) {
                            removeList.add(key);
                            k++;
                            /*if (debugEnable) {
                                buf.append(key + ",");
                            }*/

                        }
                    }
                    //remove empty ones 	
                    for (int index = 0; index < removeList.size(); index++) {
                        String key = removeList.get(index);
                        if (docs.containsKey(key)) {
                            docs.remove(key);
                        }
                    }
                    
                }
                /*if (debugEnable) {
                    if (i > 0) {
                        MRLog.debug("SUB:" + colsubnid + " DOC:REMOVE_JIDS: j: " + jid + " count:" + i);
                    }
                }
                if (debugEnable) {
                    if (k > 0) {
                        MRLog.debug("SUB:" + colsubnid + " DOC:REMOVE_DOCS: j: " + jid + " count:" + k + " doclist:" + buf.toString());
                    }
                }*/
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            
        }
        
        private Hashtable<String, String> removeSubscriptionId(Hashtable<String, String> subtopic, String subId) {
        	if(!subtopic.isEmpty()) {
        		if(subtopic.containsKey(subId))
        			subtopic.remove(subId);
        		return subtopic;
        	}
        	return null;
        }
        
        // ////////////////// docSubJids ////////////////////
        public synchronized void addColSub(String jid, String subId, String topic) {
            addColSubJid(jid, subId, topic);
        }
        
        public synchronized void addDocSub(String docuri, String jid, String subId, String topic) {
            addDocSubJid(docuri, jid, subId, topic);
        }
        
        public synchronized void subscribe(NIDSchemeMDC node, String jidStr, String subscriptionId, String topic, RequestMDCParams params) throws Exception {
            try {
                if (jidStr != null && jidStr.equals("")) {
                   return;
                }  
                jidStr=jidStr.trim();
                
                if (node.getType() == NIDSchemeMDC.COLLECTION) {
                    // if not scol subscribed than add
                    addColSubJid(jidStr, subscriptionId, topic);
                    // remove all doc + user
                    // removeAllDocsSubJID(jidStr);
                } else {
                    // if (existColSubJid(jidStr)) {
                        // remove all doc + user
                        // removeAllDocsSubJID(jidStr);
                   // } else {
                        addDocSubJid(node.getDocUri(), jidStr, subscriptionId, topic);
                   // }
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        
        public void removeColSubJidBasedOnSubscriptionId(String subscriptionId) throws Exception {
        	try {
        		if(!colSubJids.isEmpty()) {
        			Vector<String> removeList = new Vector<String>();
					for (Iterator<String> i = colSubJids.keySet().iterator(); i.hasNext();) {
						String jid = i.next();
						if(!colSubJids.get(jid).isEmpty()) {
							Hashtable<String, String> subtopic = removeSubscriptionId(colSubJids.get(jid), subscriptionId);
							if(subtopic != null && subtopic.isEmpty()) {
	            				removeList.add(jid);
							}
						}
						
					}
					if(!removeList.isEmpty()) {
						for (int index = 0; index < removeList.size(); index++) {
							String key = removeList.get(index);
							removeColSubJid(key);
						}
					}
            	}
			} catch (Exception e) {
				 MRLog.error(e);
			}
        }
        
        public void removeDocSubJidBasedOnSubscriptionId(String subscriptionId) throws Exception {
        	try {
        		
        		if(!docs.isEmpty()) {
        			Hashtable<String, Hashtable<String, String>> removeList = new Hashtable<String, Hashtable<String, String>>();
        			for (Iterator<String> i = docs.keySet().iterator(); i.hasNext();) {
        				String docuri = i.next();
        				if(!docs.get(docuri).isEmpty()) {
        					for (Iterator <String> k = docs.get(docuri).keySet().iterator(); k.hasNext();) {
								String jid = k.next();
								Hashtable<String, String> subtopic = removeSubscriptionId(docs.get(docuri).get(jid), subscriptionId);
								if(subtopic != null && subtopic.isEmpty()) { 
									if(!removeList.containsKey(docuri)) {
										removeList.put(docuri, new Hashtable<String, String>());
									}
									removeList.get(docuri).put(jid, dummy);
								}
							}
        				}
        			}
        			if(!removeList.isEmpty()) {
        				for (Iterator<String> i = removeList.keySet().iterator(); i.hasNext();) {
							String uri = i.next();
							if(!removeList.get(uri).isEmpty()) {
								for (Iterator<String> k = removeList.get(uri).keySet().iterator(); k.hasNext();) {
									String jid = k.next();
									removeDocSubJid(uri, jid);
								}
							}
						}
        			}
            	}
			} catch (Exception e) {
				 MRLog.error(e);
			}
        }
        
        public synchronized boolean isColSubEmpty() {
        	return (colSubJids.isEmpty() && docs.isEmpty());
        }
        
        public synchronized void unsubscribe(NIDSchemeMDC node, String jid, String subscriptionId, RequestMDCParams params) {
            try {
                if (node == null) {
                	if(subscriptionId !=null) {
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
                	// regular unsubscribe called on delete or explicit unsub gor
                	// user and node
                	// remove subscription for node+user
                	if (node.getType() == NIDSchemeMDC.COLLECTION) {
                		removeColSubJid(jid);
                	}else {
                    	if (existColSubJid(jid)) {
                        	// remove all doc + user
                        	removeAllDocsSubJID(jid);
                    	} else {
                    		removeDocSubJid(node.getDocUri(), jid);
                    	}
                	}
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        
        public synchronized List<PubsubSubscription> getAutoSubscriptions(NIDSchemeMDC node, RequestMDCParams params) {
            List<PubsubSubscription> subs = new ArrayList<PubsubSubscription>();
            try {
                String collectionNodeID = node.getCollectionNodeID();
                if (!colSubJids.isEmpty()) {
                    for (Iterator<String> i = colSubJids.keySet().iterator(); i.hasNext();) {
                        String jid = i.next();
                        	PubsubSubscription subscription = new PubsubSubscription();
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
                            	PubsubSubscription subscription = new PubsubSubscription();
                            	subscription.setSubJid(jid);
                            	subscription.setSubColNid(collectionNodeID);
                            	subscription.setSubNid(docsubnid);
                            	subs.add(subscription);
                        }
                    }
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            return subs;
        }
        
        public String getCollJidInfo(NIDSchemeMDC node, RequestMDCParams params, String selectFormat) {
        	StringBuffer xml = new StringBuffer();
        	if(!colSubJids.isEmpty()) {
        		for (String jid : colSubJids.keySet()) {
        			Hashtable<String, String> subtopic = colSubJids.get(jid);
        			xml.append(fillJIDInfoForSubscribers(jid, params, subtopic, selectFormat));
        		}
        	}
        	return xml.toString();
        }
        
        public String getDocJidListInfo(NIDSchemeMDC node, RequestMDCParams params, String selectFormat) {
        	StringBuffer xml = new StringBuffer();
        	Hashtable<String, Hashtable<String, String>> doc = new Hashtable<String, Hashtable<String, String>>();
        	for (String docuri : docs.keySet()) {
				StringBuffer jidInfo = new StringBuffer();
				doc = docs.get(docuri);
				if(!doc.isEmpty()) {
					for (String jid : doc.keySet()) {
						Hashtable<String, String> subtopic = doc.get(jid);
						jidInfo.append(fillJIDInfoForSubscribers(jid, params, subtopic, selectFormat));
					}
				}
				xml.append(createSubscriberWrapperForJidInfo(node.getDomain(), colsubnid, docuri, jidInfo.toString()));
			}
        	return xml.toString();
        }
        
        public String getDocJidInfo(NIDSchemeMDC node, RequestMDCParams params, String selectFormat) {
        	StringBuffer xml = new StringBuffer();
        	Hashtable<String, Hashtable<String, String>> doc = new Hashtable<String, Hashtable<String, String>>();
        	StringBuffer jidInfo = new StringBuffer();
    		
			if(docs.containsKey(node.getDocUri())) {
				doc = docs.get(node.getDocUri());
				if(!doc.isEmpty()) {
					for (String jid : doc.keySet()) {
						Hashtable<String, String> subtopic = doc.get(jid);
						jidInfo.append(fillJIDInfoForSubscribers(jid, params, subtopic, selectFormat));
					}
				}
				xml.append(jidInfo.toString());
			}
        	return xml.toString();
        }
        
        
        public String createSubscriberWrapperForJidInfo(String domain, String collection, String docuri, String jidInfo) {
        	if(!docuri.isEmpty())
        		docuri = "." + docuri;
        	if(jidInfo != null && !jidInfo.trim().equals(""))
        		return  "<Subscribers nid=\"" + domain + "." + collection + docuri + "\">" + jidInfo + "</Subscribers>";
        	else
        		return "";
        }
       
        public String fillJIDInfoForSubscribers(String jid, RequestMDCParams params, Hashtable<String, String> subtopic, String selectFormat) {
        	StringBuffer xml = new StringBuffer();
        	xml.append("<JIDInfo jid=\"" + jid + "\">");
            if(selectFormat.equals("EXTENDED")) {
            	if(!subtopic.isEmpty()) {
            		for (String subId : subtopic.keySet()) {
            			if(!subId.trim().equals("")) {
            				xml.append("<Topic subscriptionId=\"" + subId.trim() + "\">");
            				if(!subtopic.get(subId).trim().equals(""))
            					xml.append(subtopic.get(subId).trim());
            				xml.append("</Topic>");
            			}
            		}
            	}
           }
            xml.append("</JIDInfo>");
            
            return xml.toString();
        }
        
        public synchronized String toXML() {
            StringBuffer buf = new StringBuffer(200);
            buf.append("<" + colsubnid + ">");
            try {
                if (!colSubJids.isEmpty()) {
                    for (Iterator<String> i = colSubJids.keySet().iterator(); i.hasNext();) {
                        String jid = i.next();
                        buf.append("<" + SubscriptionsHashMDC.COL_SUB_JID + ">");
                        buf.append(jid);
                        buf.append("</" + SubscriptionsHashMDC.COL_SUB_JID + ">");
                    }
                }
                if (!docs.isEmpty()) {
                    for (Iterator<String> i = docs.keySet().iterator(); i.hasNext();) {
                        String docuri = i.next();
                        buf.append("<" + docuri + ">");
                        try {
                            if (docs.containsKey(docuri)) {
                                Hashtable<String, Hashtable<String, String>> doc = docs.get(docuri);
                                if (!doc.isEmpty()) {
                                    for (Iterator<String> k = doc.keySet().iterator(); k.hasNext();) {
                                        String jid = k.next();
                                        buf.append("<" + SubscriptionsHashMDC.DOC_SUB_JID + ">");
                                        buf.append(jid);
                                        buf.append("</" + SubscriptionsHashMDC.DOC_SUB_JID + ">");
                                    }
                                }
                            }
                        } catch (Exception ex) {
                            MRLog.error(ex);
                        }
                        buf.append("</" + docuri + ">");
                    }
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            buf.append("</" + colsubnid + ">");
            return buf.toString();
        }
    }
    
    @Override
    public void enableSubscriptionsLog(boolean enable) {
        
        enableLog = enable;
        
    }
    
    @Override
    public void setSaveSubscriptionsInterval(int interval) {
        saveSubscriptionsInterval = interval;
        
    }
    
    @Override
    public void setSaveSubscriptionsToDB(boolean enable) {
        isSaveSubscriptionsToDB = enable;
    }
    
    @Override
    public void enableSubscriptions(boolean enable) {
        isEnabled = enable;
    }
    
    // /////////////////////////// colSub class end ///////////////
    
    ///////////// add db api for impl sedna db xml api ////////////
    @Override
    public DBApi getDBApi() {
        return dbApi;
    }
    
    @Override
    public void setDBApi(DBApi dbApi) {
        this.dbApi = dbApi;
    }
    ///////////// add db api for impl sedna db xml api ////////////
    
}

