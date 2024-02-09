package com.ipvsserv.common.threadmodel;

import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class ChangeThreadName implements Runnable {
    private final String newName;
    private final Runnable runnable;
    
    public ChangeThreadName(Runnable runnable, String newName) {
        this.runnable = runnable;
        this.newName = newName;
    }
    
    public void run() {
        Thread currentThread = Thread.currentThread();
        String oldName = currentThread.getName();
        // MRLog.debug(" ChangeThreadName  current:" + oldName +" new: " + newName);
        if (newName != null) {
            setName(currentThread, newName);
        }
        try {
            runnable.run();
        } finally {
            setName(currentThread, oldName);
        }
    }
    
    private void setName(Thread thread, String name) {
        try {
            thread.setName(name);
        } catch (Exception ex) {
            // Probably SecurityException.
            MRLog.error("Failed to set the thread name.");
            MRLog.error(ex);
            
        }
    }
}
