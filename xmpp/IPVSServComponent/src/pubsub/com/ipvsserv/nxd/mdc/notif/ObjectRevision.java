package com.ipvsserv.nxd.mdc.notif;

import java.util.Hashtable;
import java.util.Map;
import java.util.concurrent.atomic.AtomicLong;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.server.ServerEventListener;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class ObjectRevision implements ServerEventListener {
    private AtomicLong revision = new AtomicLong();
    private Map<String, ListRevision> revisions = new Hashtable<String, ListRevision>();
    private static ObjectRevision instance = null;
    
    public static synchronized ObjectRevision getInstance() {
        if (instance == null) {
            instance = new ObjectRevision();
        }
        return instance;
    }
    
    private ObjectRevision() {
        IpvsServer.getInstance().addServerEventListener(this);
    }
    
    public synchronized String getNextRevision(NIDSchemeMDC node, String action, String nodeID) {
        if (!revisions.containsKey(node.getCollectionNodeID())) {
            ListRevision listRevision = new ListRevision();
            long revision = CompUtils.longValue(IpvsServGlobals.getXMLProperty("versions." + node.getCollectionNodeID() + "", 0 + ""), 0);
            listRevision.setRevision(revision);
            revisions.put(node.getCollectionNodeID(), listRevision);
        }
        ListRevision listRevision = revisions.get(node.getCollectionNodeID());
        listRevision.setRevision(getNextRevisionGlobal());
        return listRevision.getRevision() + "";
    }
    
    private synchronized long getNextRevisionGlobal() {
        return revision.incrementAndGet();
    }
    
    static class ListRevision {
        long revisionNumber = 0;
        
        long getRevision() {
            return revisionNumber;
        }
        
        void setRevision(long initialRevision) {
            revisionNumber = initialRevision;
            ;
        }
    }
    
    public void saveRevisions() {
    //todo: save prev  revision numbers during restarts
    
    }
    
    @Override
    public void serverRestarted() {}
    
    @Override
    public void serverRestarting() {}
    
    @Override
    public void serverStarted() {}
    
    @Override
    public void serverStarting() {}
    
    @Override
    public void serverStopped() {}
    
    @Override
    public void serverStopping() {
        try {
            /*for (String key : revisions.keySet()) {
                ListRevision listRevision = revisions.get(key);
                IpvsServGlobals.setXMLProperty("versions." + key + "", listRevision.getRevision() + "");
            }*/
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
}
