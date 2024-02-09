package com.ipvsserv.common.log;

import org.apache.commons.logging.LogConfigurationException;
import org.apache.commons.logging.LogFactory;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class CommonsLogFactory extends LogFactory {
    
    private org.apache.commons.logging.Log log;
    
    public CommonsLogFactory() {
        log = new org.apache.commons.logging.Log() {
            
            public boolean isDebugEnabled() {
                return MRLog.isDebugEnabled();
            }
            
            public boolean isErrorEnabled() {
                return MRLog.isErrorEnabled();
            }
            
            public boolean isFatalEnabled() {
                return MRLog.isErrorEnabled();
            }
            
            public boolean isInfoEnabled() {
                return MRLog.isInfoEnabled();
            }
            
            public boolean isTraceEnabled() {
                return MRLog.isDebugEnabled();
            }
            
            public boolean isWarnEnabled() {
                return MRLog.isWarnEnabled();
            }
            
            public void trace(Object object) {
            // Ignore.
            }
            
            public void trace(Object object, Throwable throwable) {
            // Ignore.
            }
            
            public void debug(Object object) {
                MRLog.debug(object.toString());
            }
            
            public void debug(Object object, Throwable throwable) {
                MRLog.debug(object.toString(), throwable);
            }
            
            public void info(Object object) {
                // Send info log messages to debug because they are generally not useful.
                MRLog.debug(object.toString());
            }
            
            public void info(Object object, Throwable throwable) {
                // Send info log messages to debug because they are generally not useful.
                MRLog.debug(object.toString(), throwable);
            }
            
            public void warn(Object object) {
                MRLog.warn(object.toString());
            }
            
            public void warn(Object object, Throwable throwable) {
                MRLog.warn(object.toString(), throwable);
            }
            
            public void error(Object object) {
                MRLog.error(object.toString());
            }
            
            public void error(Object object, Throwable throwable) {
                MRLog.error(object.toString(), throwable);
            }
            
            public void fatal(Object object) {
                MRLog.error(object.toString());
            }
            
            public void fatal(Object object, Throwable throwable) {
                MRLog.error(object.toString(), throwable);
            }
        };
    }
    
    public Object getAttribute(String string) {
        return null;
    }
    
    public String[] getAttributeNames() {
        return new String[0];
    }
    
    public org.apache.commons.logging.Log getInstance(Class aClass) throws LogConfigurationException {
        return log;
    }
    
    public org.apache.commons.logging.Log getInstance(String string) throws LogConfigurationException {
        return log;
    }
    
    public void release() {

    }
    
    public void removeAttribute(String string) {

    }
    
    public void setAttribute(String string, Object object) {

    }
}
