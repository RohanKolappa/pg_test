package com.ipvsserv.common.log;

import org.mortbay.log.Logger;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class JettyLog implements Logger {
    
    private boolean debugEnabled = MRLog.isJettyLogEnable();
    private JettyLog log = null;
    
    public boolean isDebugEnabled() {
        return debugEnabled && MRLog.isDebugEnabled();
    }
    
    public void setDebugEnabled(boolean b) {
    // Do nothing.
    }
    
    public void info(String string, Object object, Object object1) {
        // Send info log messages to debug because they are generally not useful.
        MRLog.debug("JettyLog:1 " + format(string, object, object1));
    }
    
    public void debug(String string, Throwable throwable) {
        MRLog.debug("JettyLog:2 " + string, throwable);
    }
    
    public void debug(String string, Object object, Object object1) {
        MRLog.debug("JettyLog:3 " + format(string, object, object1));
    }
    
    public void warn(String string, Object object, Object object1) {
        MRLog.debug("JettyLog:4" + format(string, object, object1));
    }
    
    public void warn(String string, Throwable throwable) {
        throwable.printStackTrace();
        MRLog.debug("JettyLog:5 " + string, throwable);
    }
    
    public Logger getLogger(String string) {
        return new JettyLog();
    }
    
    private String format(String msg, Object arg0, Object arg1) {
        int sub0 = msg.indexOf("{}");
        int sub1 = (sub0 > 0) ? (0) : (msg.indexOf("{}"));
        
        if (arg0 != null && sub0 > 0) msg = msg.substring(0, sub0) + arg0 + msg.substring(sub0 + 2);
        
        if (arg1 != null && sub1 > 0) msg = msg.substring(0, sub1) + arg1 + msg.substring(sub1 + 2);
        
        return msg;
    }
}
