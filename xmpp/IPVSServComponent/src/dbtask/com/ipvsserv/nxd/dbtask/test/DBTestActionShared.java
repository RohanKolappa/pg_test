package com.ipvsserv.nxd.dbtask.test;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBTestActionShared {
    private static DBTestActionShared instance = null;
    private AtomicInteger collections;
    private Map<Integer, AtomicInteger> documents = null;
    
    public synchronized static DBTestActionShared getInstance() {
        if (instance == null) instance = new DBTestActionShared();
        return instance;
    }
    
    public DBTestActionShared() {
        init();
    }
    
    private int doRawRandomNumber(int min, int max) {
        int rawRandomNumber = (int) (Math.random() * (max - min + 1)) + min;
        return rawRandomNumber;
    }
    
    ///////////// collections ///////////////
    public synchronized int getCollections() {
        return collections.get();
    }
    
    private synchronized int _getAndIncrementCollection() {
        //create a new collection
        Integer col = collections.getAndIncrement();
        return col;
    }
    
    private synchronized int _ensureDocument(Integer col, Integer docs) {
        while (collections.get() < col) {
            Integer col1 = collections.getAndIncrement();
            if (documents.get(col1) == null) documents.put(col1, new AtomicInteger(0));
        }
        if (col > collections.get()) {
            _getAndIncrementCollection();
        }
        if (documents.get(col) == null) documents.put(col, new AtomicInteger(docs));
        return col;
    }
    
    public synchronized int getAndIncrementCollection() {
        //create a new collection
        Integer col = _getAndIncrementCollection();
        _ensureDocument(col, 0);
        return col;
    }
    
    public synchronized int getAndIncrementCollection(int documentCount) {
        //create a new collection
        Integer col = _getAndIncrementCollection();
        _ensureDocument(col, documentCount);
        return col;
    }
    
    public synchronized int decrementAndGetCollection() {
        //delete a collection
        Integer col = collections.getAndDecrement();
        documents.remove(col);
        return col;
    }
    
    ///////////// documents ///////////////
    public synchronized int getDocuments(Integer col) throws Exception {
        if (documents.containsKey(col)) {
            if (documents.get(col).get() == -1) {
                return documents.get(col).get();
            } else {
                throw new Exception("No documents");
            }
        } else {
            throw new Exception("No collections");
        }
    }
    
    public synchronized int getAndIncrementDocuments(Integer col) throws Exception {
        if (documents.containsKey(col)) {
            return documents.get(col).getAndIncrement();
        } else {
            throw new Exception("No collections");
        }
    }
    
    public synchronized int decrementAndGetDocuments(Integer col) throws Exception {
        if (documents.containsKey(col)) {
            return documents.get(col).decrementAndGet();
        } else {
            throw new Exception("No documents");
        }
    }
    
    /////////////////////
    public synchronized boolean isCollectionHasDocuments(Integer col) {
        AtomicInteger docs = documents.get(col);
        int max = docs.get();
        if (max <= 0) return false;
        return true;
    }
    
    public synchronized int getRandomCollection() throws Exception {
        int max = collections.get();
        //if(max==0){
        //   throw new Exception("No collections");
        //}
        randomCollectionWithDoc++;
        if (randomCollectionWithDoc >= max) randomCollectionWithDoc = 0;
        return randomCollectionWithDoc;
    }
    
    int randomCollectionWithDoc = -1;
    
    public synchronized int getRandomCollectionWithDoc() throws Exception {
        int max = collections.get();
        
        randomCollectionWithDoc++;
        if (documents.get(randomCollectionWithDoc) == null || documents.get(randomCollectionWithDoc).get() == 0) {
            randomCollectionWithDoc = 0;
        }
        while (randomCollectionWithDoc < max) {
            if (documents.get(randomCollectionWithDoc) != null && documents.get(randomCollectionWithDoc).get() > 0) {
                return randomCollectionWithDoc;
            }
            randomCollectionWithDoc++;
        }
        if (randomCollectionWithDoc >= max) {
            randomCollectionWithDoc = 0;
            if (documents.get(randomCollectionWithDoc) == null || documents.get(randomCollectionWithDoc).get() == 0) {
                throw new Exception("There are no collection which has documents in it");
            }
        }
        return randomCollectionWithDoc;
    }
    
    public synchronized int getRandomDocument(int col) throws Exception {
        if (col == -1) throw new Exception("No collections ");
        AtomicInteger docs = documents.get(col);
        if (docs == null) {
            MRLog.debug("docs ==null getRandomDocument " + col);
        }
        int max = docs.get();
        
        int randomCol = doRawRandomNumber(0, max - 1);
        return randomCol;
    }
    
    public synchronized boolean isCollectionInitialized() throws Exception {
        if (collections.get() == 0) {
            return false;
        }
        return true;
    }
    
    public synchronized void init() {
        collections = new AtomicInteger();
        documents = new LinkedHashMap<Integer, AtomicInteger>();
    }
    
    public synchronized void init(Map<String, Integer> resourceList, String collectionPrefix) throws Exception {
        init();
        MRLog.debug("DBTestActionShared init colllection count: " + resourceList.size());
        for (Iterator<String> iterator = resourceList.keySet().iterator(); iterator.hasNext();) {
            String collectionName = iterator.next();
            Integer docCount = resourceList.get(collectionName);
            if (collectionName.indexOf(collectionPrefix) != -1) {
                String colNumber = collectionName.substring(collectionName.indexOf(collectionPrefix) + collectionPrefix.length());
                Integer col = CompUtils.intValue(colNumber, -1);
                if (col != -1) {
                    MRLog.debug(" DBTestActionShared init  documents in collection " + collectionPrefix + col + " " + docCount);
                    _ensureDocument(col, docCount);
                }
            }
        }
    }
}
