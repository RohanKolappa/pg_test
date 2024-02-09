package com.ipvsserv.xmpp.mdc;

import java.util.ArrayList;

import com.ipvsserv.common.mdc.NIDSchemeMDC;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface DBCache {
    
    // public ///////////////////////////////////////////////////////////
    public abstract void removeFromCache(NIDSchemeMDC node, String nodeID, String action);
    
    public abstract String create(NIDSchemeMDC node, String nodeID, String xml, String action);
    
    public abstract String addToCache_read(NIDSchemeMDC node, String nodeID, String xml);
    
    // /////////////////////// read actions ////////////////////////////
    public abstract String read(NIDSchemeMDC node, String nodeID);
    
    public abstract ArrayList<String> discoItems(NIDSchemeMDC node, String nodeID);
    
    public abstract int nodeExists(NIDSchemeMDC node, String nodeID);
    
    // /////////////////////// add remove ////////////////////////////
    public abstract void markDirty(NIDSchemeMDC node, String nodeID, String ID);
    
    public abstract void unmarkDirty(NIDSchemeMDC node, String nodeID, String ID);
    
    public abstract void clear();
    
    /////////////////
    public abstract void enableDBCache(boolean enable);
    
    public abstract void enableDBCacheLog(boolean enable);
    
    public abstract void setMaxCount(int count);
    
    public abstract void init();
    
}
