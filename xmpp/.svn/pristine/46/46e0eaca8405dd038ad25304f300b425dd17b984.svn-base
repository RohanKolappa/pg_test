package com.ipvsserv.nxd.mdc.db.cache;

import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.dom4j.Element;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.mdc.RequestIQConst;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.xmpp.mdc.DBCache;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBCacheImpl implements DBCache {
    private static final long serialVersionUID = 1L;
    private int maxCount;
    private boolean ENABLE_DBCache = true;
    private Hashtable<String, String> dirty = null;
    private boolean debugEnable = false;
    private LinkedHashMapIPVS<String, String> cacheList = null;
    
    public DBCacheImpl() {

    }
    
    // public ///////////////////////////////////////////////////////////
    
    public synchronized void removeFromCache(NIDSchemeMDC node, String nodeID, String action) {
        if (!ENABLE_DBCache) {
            return;
        }
        if (nodeID != null) {
            String ID = "(" + action + " " + nodeID + ")";
            // remove col
            if (node.getType() == NIDSchemeMDC.COLLECTION) {
                removeItem(node.getCollectionNodeID(), ID);
            } else {
                removeItem(node.getDocNodeID(), ID);
                removeItem(node.getCollectionNodeID(), ID);
            }
        }
    }
    
    /* (non-Javadoc)
     * @see com.ipvsserv.openfire.pubsub.sedna.mdc.impl.DBCacheI#create(com.ipvsserv.openfire.pubsub.sedna.mdc.impl.NIDSchemeMDC, java.lang.String, java.lang.String, java.lang.String)
     */
    public synchronized String create(NIDSchemeMDC node, String nodeID, String xml, String action) {
        try {
            if (!ENABLE_DBCache) {
                return null;
            }
            removeFromCache(node, nodeID, action + " " + nodeID);
            if (nodeID != null && xml != null) {
                if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                    String ID = "( Document  " + nodeID + ")";
                    if (action.equalsIgnoreCase(RequestIQConst.ADD)) {
                        addItem(nodeID, xml, ID);
                    }
                }
                return xml;
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    /* (non-Javadoc)
     * @see com.ipvsserv.openfire.pubsub.sedna.mdc.impl.DBCacheI#addToCache_read(com.ipvsserv.openfire.pubsub.sedna.mdc.impl.NIDSchemeMDC, java.lang.String, java.lang.String)
     */
    public synchronized String addToCache_read(NIDSchemeMDC node, String nodeID, String xml) {
        try {
            if (!ENABLE_DBCache) {
                return null;
            }
            if (nodeID != null && xml != null && !xml.trim().equals("")) {
                String ID = "(addToCache_read " + nodeID + ")";
                if (isDirty(node, nodeID, ID)) {
                    return null;
                }
                if (node.getType() == NIDSchemeMDC.COLLECTION) {
                    addItem(nodeID, xml, ID);
                } else if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                    addItem(nodeID, xml, ID);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    // /////////////////////// read actions ////////////////////////////
    public synchronized String read(NIDSchemeMDC node, String nodeID) {
        try {
            if (!ENABLE_DBCache) {
                return null;
            }
            if (nodeID != null) {
                String ID = "(read " + nodeID + ")";
                return findItem(node, nodeID, ID);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    public synchronized ArrayList<String> discoItems(NIDSchemeMDC node, String nodeID) {
        try {
            if (!ENABLE_DBCache) {
                return null;
            }
            String xml = "";
            if (nodeID != null) {
                String ID = "(discoItems " + nodeID + ")";
                xml = findItem(node, nodeID, ID);
                if (xml != null && !xml.equals("")) {
                    if (node.getType() == NIDSchemeMDC.COLLECTION) {
                        if (debugEnable) {
                            MRLog.debug("**2**DISCOITEMS COLLECTION from cache " + nodeID);
                        }
                        Element el = Dom4jUtils.stringToElement(xml);
                        return parseList(el);
                    } else if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                        if (debugEnable) {
                            MRLog.debug("**2**DISCOITEMS DOCUMENT from cache " + nodeID);
                        }
                        Element el = Dom4jUtils.stringToElement(xml);
                        return parseList(el);
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    public synchronized int nodeExists(NIDSchemeMDC node, String nodeID) {
        try {
            if (!ENABLE_DBCache) {
                return -1;
            }
            String xml = "";
            if (nodeID != null) {
                String ID = "(nodeExists " + nodeID + ")";
                xml = findItem(node, nodeID, ID);
                if (xml != null && !xml.equals("")) {
                    if (node.getType() == NIDSchemeMDC.COLLECTION) {
                        if (debugEnable) {
                            MRLog.debug("**2**NODEEXISTS COLLECTION from cache " + nodeID);
                        }
                        return 1;
                    } else if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                        if (debugEnable) {
                            MRLog.debug("**2**NODEEXISTS DOCUMENT from cache " + nodeID);
                        }
                        return 1;
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return 0;
    }
    
    // /////////////////////// add remove ////////////////////////////
    
    public synchronized void markDirty(NIDSchemeMDC node, String nodeID, String ID) {
        if (!ENABLE_DBCache) {
            return;
        }
        if (nodeID != null) {
            setDirtyFlag(node, nodeID, ID);
        }
    }
    
    public synchronized void unmarkDirty(NIDSchemeMDC node, String nodeID, String ID) {
        if (!ENABLE_DBCache) {
            return;
        }
        if (nodeID != null) {
            clearDirtyFlag(node, nodeID, ID);
        }
    }
    
    public synchronized void clear() {
        if (!ENABLE_DBCache) {
            return;
        }
        clearItems();
    }
    
    // //////////////////////////////////////////// DBCache getInstance
    // ////////// items/////////////////////
    private void addItem(String nodeID, String xml, String ID) {
        try {
            //ensureItemsSize();
            cacheList.put(nodeID, xml);
            if (debugEnable) {
                MRLog.debug("**2**ADD to cache " + " L:" + cacheList.size() + " D:" + dirty.size() + " " + ID);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private void removeItem(String nodeID, String ID) {
        try {
            if (cacheList.containsKey(nodeID)) {
                cacheList.remove(nodeID);
                if (debugEnable) {
                    MRLog.debug("**2**DELETE from cache " + ID);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private String getItem(String nodeID, String ID) {
        try {
            if (cacheList.containsKey(nodeID)) {
                if (debugEnable) {
                    MRLog.debug("**2**GET from cache " + ID);
                }
                //resetItem(nodeID);
                return cacheList.get(nodeID);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    private String findItem(NIDSchemeMDC node, String nodeID, String ID) {
        try {
            String xml = "";
            if (nodeID != null) {
                if (isDirty(node, nodeID, ID)) {
                    return null;
                }
                if (node.getType() == NIDSchemeMDC.COLLECTION) {
                    if (debugEnable) {
                        MRLog.debug("**2**GET COLLECTION from cache " + nodeID);
                    }
                    // remove doc
                    return getItem(node.getCollectionNodeID(), ID);
                } else if (node.getType() == NIDSchemeMDC.DOCUMENT) {
                    if (debugEnable) {
                        MRLog.debug("**2**GET DOCUMENT from cache " + nodeID);
                    }
                    return getItem(node.getDocNodeID(), ID);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    private void clearItems() {
        try {
            cacheList.clear();
            dirty.clear();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    /*
    private void ensureItemsSize() {
    	// called from sync block
    	try {
    		if (cacheList.size() <= maxCount) {
    			return;
    		}
    		int count =cacheList.size() - maxCount;
    		if(count<1)
    			return;
    		Vector<String> list = new Vector<String>(); 
    		int index=0;
    		for (Iterator<String> iterator = cacheList.keySet().iterator(); iterator.hasNext();) {
    			if(index>count)
    				break;
    			String key = (String) iterator.next();
    			list.add(key);
    			index++;
    		}
    		for(int i=0;i<list.size();i++){
    			String keyRemove = (String) list.get(i);
    			if(cacheList.containsKey(keyRemove)){
    				cacheList.remove(keyRemove);
    				if (debugEnable)
    					if (keyRemove != null)
    						MRLog.debug("**2**DELETE from cache size " + cacheList.size() + "  " + keyRemove);
    			}
    		}
    	} catch (Exception ex) {
    		MRLog.error(ex);
    	}
    }*/
    /*
    private void resetItem(String nodeID) {
    	
    	// called from sync block
    	try {
    		if(cacheList.containsKey(nodeID)){
    			String xml = cacheList.remove(nodeID);
    			if(xml != null)
    				cacheList.put(nodeID, xml);
    			else
    				cacheList.put(nodeID, "");
    		}
    		
    	} catch (Exception ex) {
    		MRLog.error(ex);
    	}
    }*/
    // ////////// items/////////////////////
    // ////////// dirty /////////////////////
    private void setDirtyFlag(NIDSchemeMDC node, String nodeID, String ID) {
        
        try {
            // called from sync block
            if (nodeID != null) {
                if (!dirty.containsKey(node.getCollectionNodeID())) {
                    dirty.put(node.getCollectionNodeID(), "");
                }
                if (node.getType() != NIDSchemeMDC.COLLECTION) {
                    if (!dirty.containsKey(node.getDocNodeID())) {
                        dirty.put(node.getDocNodeID(), "");
                        if (debugEnable) {
                            MRLog.debug("**2**MARK_DIRTY    =" + dirty.size() + " " + nodeID + " " + ID);
                        }
                    }
                }
            }
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private void clearDirtyFlag(NIDSchemeMDC node, String nodeID, String ID) {
        
        try {
            // called from sync block
            if (nodeID != null) {
                if (dirty.containsKey(node.getCollectionNodeID())) {
                    dirty.remove(node.getCollectionNodeID());
                    if (debugEnable) {
                        MRLog.debug("**2**UN_MARK_DIRTY  col " + nodeID + " " + ID);
                    }
                }
                if (node.getType() != NIDSchemeMDC.COLLECTION) {
                    if (dirty.containsKey(node.getDocNodeID())) {
                        dirty.remove(node.getDocNodeID());
                        if (debugEnable) {
                            MRLog.debug("**2**UN_MARK_DIRTY  doc " + nodeID + " " + ID);
                        }
                    }
                }
            }
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private boolean isDirty(NIDSchemeMDC node, String nodeID, String ID) {
        try {
            // called from sync block
            if (nodeID != null) {
                if (node.getType() == NIDSchemeMDC.COLLECTION) {
                    if (!dirty.containsKey(node.getCollectionNodeID())) {
                        if (debugEnable) {
                            MRLog.debug("**2**IS_DIRTY col true " + ID);
                        }
                        return true;
                    }
                } else {
                    if (!dirty.containsKey(node.getDocNodeID())) {
                        if (debugEnable) {
                            MRLog.debug("**2**IS_DIRTY doc true " + ID);
                        }
                        return true;
                    }
                }
            }
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return false;
    }
    
    private ArrayList<String> parseList(Element el) {
        ArrayList<String> retList = new ArrayList<String>();
        try {
            List<?> list = Dom4jUtils.listXpath(el, "./*/@NID");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element object = (Element) iterator.next();
                String NID = Dom4jUtils.getAttributeValue(object, "NID", "").trim();
                if (!NID.equals("")) {
                    retList.add(NID);
                }
            }
            if (retList.size() > 0) {
                return retList;
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    static class LinkedHashMapIPVS<k, v> extends LinkedHashMap<String, String> {
        private static final long serialVersionUID = -3781443194122681803L;
        
        LinkedHashMapIPVS(int initialCapacity, float loadFactor, boolean accessOrder, int maxCount) {
            super(initialCapacity, loadFactor, accessOrder);
            //MAX_ENTRIES = IPVSGlobals.getInstance().getEnableDBCacheMaxSise();
            MAX_ENTRIES = maxCount;
            
        }
        
        private int MAX_ENTRIES = 100;
        
        @Override
        protected boolean removeEldestEntry(Map.Entry<String, String> eldest) {
            return size() > MAX_ENTRIES;
        }
        
    }
    
    @Override
    public void enableDBCache(boolean enable) {
        ENABLE_DBCache = enable;
    }
    
    @Override
    public void enableDBCacheLog(boolean enable) {
        debugEnable = enable;
    }
    
    @Override
    public void setMaxCount(int count) {
        maxCount = count;
    }
    
    @Override
    public void init() {
        dirty = new Hashtable<String, String>();
        float loadFactor = (float) 0.75;
        cacheList = new LinkedHashMapIPVS<String, String>(maxCount, loadFactor, true, maxCount);
    }
    
}
