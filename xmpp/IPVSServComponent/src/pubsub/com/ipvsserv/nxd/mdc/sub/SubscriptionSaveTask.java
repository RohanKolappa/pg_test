package com.ipvsserv.nxd.mdc.sub;

import java.util.TimerTask;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.xmpp.mdc.DomainManager;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class SubscriptionSaveTask extends TimerTask {
    private DomainManager engine = null;
    
    public SubscriptionSaveTask(DomainManager service) {
        engine = service;
    }
    
    @Override
    public void run() {
        try {
            // get all domains and check subs flag for update and save to db.
            save();
        } catch (Throwable e) {
            MRLog.error("SubscriptionSaveTask Error");
            MRLog.error(e);
        }
    }
    
    boolean isStop = false;
    
    public void stopServer() {
        isStop = true;
    }
    
    boolean threadRunning = false;
    
    public synchronized boolean isThreadRunning() {
        return threadRunning;
    }
    
    public synchronized void setThreadRunning(boolean threadRunning) {
        this.threadRunning = threadRunning;
    }
    
    public void save() {
        try {
            if (isStop) {
                return;
            }
            if (isThreadRunning()) {
                return;
            }
            setThreadRunning(true);
            // get all domains and check subs flag for update and save to db.
            engine.saveSubscriptions();
        } catch (Throwable e) {
            MRLog.error("SubscriptionSaveTask Error");
            MRLog.error(e);
        } finally {
            setThreadRunning(false);
        }
    }
    
}
